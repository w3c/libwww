#!/usr/local/bin/perl
# -*- Perl -*-
#
# Copyright 1996 Massachusetts Institute of Technology
#
# Permission to use, copy, modify, and distribute this software and
# its documentation for any purpose and without fee is hereby
# granted, provided that both the above copyright notice and this
# permission notice appear in all copies, that both the above
# copyright notice and this permission notice appear in all
# supporting documentation, and that the name of M.I.T. not be used
# in advertising or publicity pertaining to distribution of the
# software without specific, written prior permission.  M.I.T. makes
# no representations about the suitability of this software for any
# purpose.  It is provided "as is" without express or implied
# warranty.
# 
# THIS SOFTWARE IS PROVIDED BY M.I.T. ``AS IS''.  M.I.T. DISCLAIMS
# ALL EXPRESS OR IMPLIED WARRANTIES WITH REGARD TO THIS SOFTWARE,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT
# SHALL M.I.T. BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
# USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

# defaults:
$Plotwindow = 0;
$PlotTemplate = '$from[0].":".$fromPort."-".$to[0].":".$toPort.".xplot"';
$Tcpdump = 'STDIN';
$Usage_first = 1;
$BreakOnSyns = 0;
$EndOnFins = 0;
$Quiet = 0;
$Cumulative = 0;
$TimeConvert = 0;
$ForceRelative = 0;
$FinThreshold = 1; # seconds

# other initializations
#$Packets;
#$Bytes;
#$StartTime = 0;
#$LastTime;
#$State;	# 'synSent', 'synRecvd', 'synackSent', 'synackRecvd', 'finSent', 'finRecvd', 'finackSent', 'finackRecvd'
#$ListFile;

sub usage
{
    return if (!$Usage_first);
    $Usage_first = 0;
    print <<"END_OF_USAGE";
Usage: $0 [-w] [-s] [-c] [-plot[filename]] [-list[filename]] [-?] [-help]
-w: plot window.
-s: break up conversations on syns.
-f: ignore socket activity after a fin (until socket is re-used)
-c: cumulative - adds all data coming from a server
-plot[filename]: plot packets in <filename>.
    The <filename> may be built out of $from (host and port), $fromHost, 
    $fromPort, $from[0..n] for the segments of the domain name. '$from.".xplot"'
    would be abc.def.com:1234. The corresponding fields exist for the to field.
    default: '$from[0].":".$fromPort."-".$to[0].":".$toPort.".xplot"'.
-list[filename]: output the list of generated plot files to filename.
-r: relative sequence numbers.
-t: time convert - insure that time is in decimal number of seconds.
-q: quiet - no visible output.
-?/-help: this message.
END_OF_USAGE
}

sub ReadArg
{
    local($arg) = substr(@_[0], 1);
    return 0 if ($arg eq 'e');
    if ($arg eq '?' || $arg eq 'help') {
	&usage;
	exit(0);
    } elsif ($arg eq 'w') {
	$Plotwindow = 1;
    } elsif ($arg eq 's') {
	$BreakOnSyns = 1;
    } elsif ($arg =~ /^f(\d*)/) {
	$EndOnFins = 1;
	$FinThreshold = $1 if ($1 ne '');
    } elsif ($arg eq 'c') {
	$Cumulative = 1;
    } elsif (substr($arg, 0, 4) eq 'plot') {
        $PlotTemplate = substr($arg, 4);
    } elsif (substr($arg, 0, 4) eq 'list') {
        $ListFile = substr($arg, 4);
	open($ListFile, ">$ListFile") || die "error opening \"$ListFile\" for writing: $!\n";
    } elsif ($arg eq 't') {
	$TimeConvert = 1;
    } elsif ($arg eq 'r') {
	$ForceRelative = 1;
    } elsif ($arg eq 'q') {
	$Quiet = 1;
    } else {
	&usage();
        print "unknown argument \"$arg\".\n";
    }
    return 1;
}

sub justSeconds
{
    local($time) = @_;
    return $time if (!($time =~ /:/));
    local($hr, $mn, $sec) = split(':', $time, 3);
    return $hr*3600 + $mn*60 + $sec;
}

sub subTimes
{
    local($big, $little) = @_;
    $big = &justSeconds($big);
    $little = &justSeconds($little);
    return ($big - $little);
}

sub newConversation
{
    local($from, $to, $time) = @_;

    # create variables for PlotTemplate
    $from =~ /(.*)\.(\d+)$/;
    local($fromHost, $fromPort) = ($1, $2);
    local(@from) = split('\.', $fromHost);
    $to =~ /(.*)\.(\d+)$/;
    local($toHost, $toPort) = ($1, $2);
    local(@to) = split('\.', $toHost);

    # create Xplot file from template
    $XplotName{$from} = eval($PlotTemplate);
    open($from, ">$XplotName{$from}") || die "error opening \"$from\" for writing: $!\n";
    print $from "timeval signed\ntitle\n$from-->$to\n";

    # initialize conversation
    $To{$from} = $to;
    $LastSendSeq{$from.'-'.$to} = -1;
    $StartTime{$from} = $time;
    push(@Froms, $from);
    $AckOffset{$from} = $SeqOffset{$from} = 0;
    $Ignored{$from.'-'.$to} = 0;
}

sub closeOut
{
    local($from) = @_;
    print $from "go\n";
    $TotalPackets += $Packets{$from};
    $TotalBytes += $Bytes{$from};
    print "$from: $Packets{$from} packets $Bytes{$from} bytes took ", 
	&subTimes($LastTime{$from}, $StartTime{$from}), "\n" if (!Quiet);
    $Mode{$from} = 'client' if (!defined($Mode{$from})); # added to handle bogus NT netmon dumps
    print $ListFile "$from $To{$from} $Mode{$from} $XplotName{$from} $Packets{$from} $Bytes{$from} ", 
	&subTimes($LastTime{$from}, $StartTime{$from}), ' ', $Bytes{$from}/($Bytes{$from} + $Packets{$from}*40), "\n" if $ListFile;
    $MaxLast = $LastTime{$from} if (!defined($MaxLast) || $LastTime{$from} > $MaxLast);
    $MinFirst = $StartTime{$from} if (!defined($MinFirst) || $StartTime{$from} < $MinFirst);
}

sub clearOut
{
    local($from, $to) = @_;

    # these attributes are specific to a conversation
    delete $LastSendSeq{$from.'-'.$to};
    delete $FirstSeq{$from.'-'.$to};
    delete $LastAckTime{$from.'-'.$to};
    delete $LastAckSeq{$from.'-'.$to};
    delete $LastWind{$from.'-'.$to};

    # these stats are aggregated for all converstaions on a server
    delete $Packets{$from};
    delete $Bytes{$from};
    delete $LastTime{$from};
    delete $StartTime{$from};
    delete $Mode{$from};
    delete $Served{$from};	# not needed now because server is never cleared
    delete $To{$from};
    delete $XplotName{$from};
    delete $Ignored{$from.'-'.$to};
}

sub removeFrom
{
    local($from) = @_;
    local($i);
    foreach $i ($[ .. $#Froms) {
	if ($Froms[$i] eq $from) {
	    splice(@Froms, $i, 1);
	    return 1;
	}
    }
    return 0;
}

while (@ARGV[0] =~ /^-/ && &ReadArg(shift(@ARGV))) {}
if (($inputFile = shift(@ARGV))) {
    open (TCPDUMP, "<$inputFile") || die "error opening \"$inputFile\" for reading: $!\n";
    $Tcpdump = 'TCPDUMP';
}

for ($lineNo = 1; <$Tcpdump>; $lineNo++) {
    local($ackIsZero) = (0);
    chop;
    split(/ /);
    $time = $_[0];
    $from = $_[1];
    $to = $_[3];
    chop($to);			# strip off colon
    $flags = $_[4];

    last if (/\d+\s+packets/);
    if ($_[2] ne '>') {
	print stderr "tcpdump2xplot: Malformed entry in dump file $inputFile:$lineNo \"$_\"\n";
	next;
    }
    $time = &justSeconds($time) if ($TimeConvert);
    &newConversation($from, $to, $time) if (!(defined($StartTime{$from})));

    if ($flags =~ /S/) {
	if ($Mode{$from} eq 'client') {		# re-used client socket's syn
	    if ($BreakOnSyns) {
		&closeOut($from);
		&removeFrom($from);
		&clearOut($from, $to);
		&newConversation($from, $to, $time);
		$Mode{$from} = 'client';
	    }
	} elsif ($Mode{$from} eq 'server') {	# server's syn
	    $Served{$from}++;
	} else {				# client socket's syn
	    $Mode{$from} = 'client';
	    $Mode{$to} = 'server';
	}
    }

    if ($flags =~ /F/ && $Mode{$from} eq 'client' && $EndOnFins && !$Ignored{$from.'-'.$to}) {
	local ($dif) = $time - $LastTime{$from};
	if ($dif > $FinThreshold) {
	    print stderr "tcpdump2xplot: delayed F ($dif seconds) in dump file $inputFile:$lineNo \"$_\"\n";
	}
	$Ignored{$from.'-'.$to} = 1;
    }

    next if ($Ignored{$from.'-'.$to} || $Ignored{$to.'-'.$from});
    $LastTime{$from} = $time;
    $Packets{$from}++;

    if (/$flags ([0-9]*):([0-9]*)\(([0-9]*)\) /) {
	$LastSendSeq{$from.'-'.$to} = $sendseq = $1;
	$sendseqlast = $2;
	$datalength = $3;
	$Bytes{$from} += $3;
	if (!defined $FirstSeq{$from.'-'.$to}) {
	    if ($ForceRelative) {
		$LastSendSeq{$from.'-'.$to} = $sendseq = 0;
		$sendseqlast = $2 - $1;
		$FirstSeq{$from.'-'.$to} = 0;
		$ackIsZero = 1;
	    } else {
		$FirstSeq{$from.'-'.$to} = $1;
	    }
	}
    } else {
	$sendseq = $LastSendSeq{$from.'-'.$to};
	$sendseqlast = $LastSendSeq{$from.'-'.$to};
    }

    if (/win ([0-9]*)/) {	# remove space in "]*) /" to handle win <num><EOL> - EGP
	$window = $1;
    } else {
	$window = -1;
    }

    if (/ack ([0-9]*) /) {
	$ack = $ackIsZero ? 0 : $1;
    } else {
	$ack = -1;
    }

    if ($Cumulative) {
	# adjust sequence number to be relative to last ack.
	$sendseqlast -= $sendseq - $SeqOffset{$from};
	$sendseq = $SeqOffset{$from};
	$SeqOffset{$from} = $sendseqlast;
    } else {
	# adjust sequence number to be relative to start of conversation.
	$sendseq -= $FirstSeq{$from.'-'.$to};
	$sendseqlast -= $FirstSeq{$from.'-'.$to};
    }
    print $from "darrow $time $sendseq\n";
    print $from "uarrow $time $sendseqlast\n";
    print $from "line $time $sendseq $time $sendseqlast\n";

    if ($ack != -1) {
	$winend = $ack + $window;
	if (defined $LastAckTime{$to.'-'.$from}) {
	    local($lastAckSeq, $ackSeq) = ($LastAckSeq{$to.'-'.$from}, $ack);
	    if ($Cumulative) {
		# adjust sequence number to be relative to last ack.
		$ackSeq -= $lastAckSeq - $AckOffset{$to};
		$lastAckSeq = $AckOffset{$to};
		$AckOffset{$to} = $ackSeq;
	    } else {
		# adjust sequence number to be relative to start of conversation.
		$lastAckSeq -= $FirstSeq{$to.'-'.$from};
		$ackSeq -= $FirstSeq{$to.'-'.$from};
	    }

	    print $to "line $LastAckTime{$to.'-'.$from} $lastAckSeq ";
	    print $to "$time $lastAckSeq\n";
	    if ($LastAckSeq{$to.'-'.$from} != $ack) {
		print $to "line $time $lastAckSeq $time $ackSeq\n";
	    } else {
		print $to "dtick $time $ackSeq\n";
	    }

	    if ($Plotwindow) {
		local($lastWinSeq, $winSeq) = ($LastWind{$to.'-'.$from}, $winend);
		if ($Cumulative) {
		    # adjust sequence number to be relative to last ack.
		    $winSeq -= $lastWinSeq - $AckOffset{$to};
		    $lastWinSeq = $AckOffset{$to};
		} else {
		    # adjust sequence number to be relative to start of conversation.
		    $lastWinSeq -= $FirstSeq{$to.'-'.$from};
		    $winSeq -= $FirstSeq{$to.'-'.$from};
		}

		print $to "line $LastAckTime{$to.'-'.$from} $lastWinSeq ";
		print $to "$time $lastWinSeq\n";
		if ($LastWind{$to.'-'.$from} != $winend) {
		    print $to "line $time $lastWinSeq $time $winSeq\n";
		} else {
		    print $to "utick $time $winSeq\n";
		}
	    }
	}
	$LastAckTime{$to.'-'.$from} = $time;
	$LastAckSeq{$to.'-'.$from} = $ack;
	$LastWind{$to.'-'.$from} = $winend;
    }
}

foreach $from (@Froms) {
    &closeOut($from);
}
print "summary: $TotalPackets packets $TotalBytes bytes took ", &subTimes($MaxLast, $MinFirst), 
    " efficiency: ", $TotalBytes/($TotalBytes + $TotalPackets*40), "\n" if (!Queit);

print $ListFile "summary: $TotalPackets $TotalBytes ",
    &subTimes($MaxLast, $MinFirst), " ", $TotalBytes/($TotalBytes + $TotalPackets*40), "\n" if ($ListFile);

close($ListFile) if ($ListFile);

exit 0;

