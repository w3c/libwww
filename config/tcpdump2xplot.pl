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
$Plot = '$from.".xplot"';
$Tcpdump = 'STDIN';
$Usage_first = 1;
$BreakOnSyns = 0;

# other initializations
#$Packets;
#$Bytes;
#$StartTime = 0;
#$LastTime;
#$State;	# 'synSent', 'synRecvd', 'synackSent', 'synackRecvd', 'finSent', 'finRecvd', 'finackSent', 'finackRecvd'

sub usage
{
    return if (!$Usage_first);
    $Usage_first = 0;
    print <<"END_OF_USAGE";
Usage: $0 [-w] [-s] [-plot<file>] [-?] [-help]
-w: plot window
-s: break up conversations on syns
-plot<file>: plot packets in <file>
-?/-help: this message
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
    } elsif (substr($arg, 0, 4) eq 'plot') {
        $Plot = substr($arg, 4);
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
    local($name) = eval($Plot);
    open($from, ">$name") || die "error opening \"$from\" for writing: $!\n";
    $LastSendSeq{$from} = -1;
    $LastSendSeqLast{$from} = -1;
    print $from "timeval signed\ntitle\n$from-->$to\n";
    $StartTime{$from} = $time;
    push(@Froms, $from);
}

sub closeOut
{
    local($from) = @_;
    print $from "go\n";
    $TotalPackets += $Packets{$from};
    $TotalBytes += $Bytes{$from};
    print "$from: $Packets{$from} packets $Bytes{$from} bytes took ", 
	&subTimes($LastTime{$from}, $StartTime{$from}), "\n";
    $MaxLast = $LastTime{$from} if (!defined($MaxLast) || $LastTime{$from} > $MaxLast);
    $MinFirst = $StartTime{$from} if (!defined($MinFirst) || $StartTime{$from} < $MinFirst);
}

sub clearOut
{
    local($from) = @_;
    delete $LastSendSeq{$from};
    delete $LastSendSeqLast{$from};
    delete $Packets{$from};
    delete $Bytes{$from};
    delete $LastTime{$from};
    delete $StartTime{$from};
    delete $firstseq{$from};
    delete $last_time{$from};
    delete $last_ack{$from};
    delete $last_wind{$from};
    delete $Mode{$from};
    delete $Served{$from};	# not needed now because server is never cleared
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
    chop;
    split(/ /);
    $time = $_[0];
    $from = $_[1];
    $to = $_[3];
    chop($to);			# strip off colon
    $flags = $_[4];

    last if (/\d+\s+packets/);
    &newConversation($from, $to, $time) if (!(defined($StartTime{$from})));

    if ($flags =~ /S/) {
	if ($Mode{$from} eq 'client') {		# re-used client socket's syn
	    if ($BreakOnSyns) {
		&closeOut($from);
		&removeFrom($from);
		&clearOut($from);
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

    $LastTime{$from} = $time;
    $Packets{$from}++;

    if (/$flags ([0-9]*):([0-9]*)\(([0-9]*)\) /) {
	$LastSendSeq{$from} = $sendseq = $1;
	if (!defined $firstseq{$from}) {
	    $firstseq{$from} = $1;
	}
	$LastSendSeqLast{$from} = $sendseqlast = $2;
	$datalength = $3;
	$Bytes{$from} += $3;
    } else {
	$sendseq = $LastSendSeq{$from};
	$sendseqlast = $LastSendSeq{$from};
    }

    $sendseq -= $firstseq{$from};
    $sendseqlast -= $firstseq{$from};

    if (/win ([0-9]*)/) {	# remove space in "]*) /" to handle win <num><EOL> - EGP
	$window = $1;
    } else {
	$window = -1;
    }

    if (/ack ([0-9]*) /) {
	$ack = $1;
	$ack -= $firstseq{$to} if ($ack >= $firstseq{$to});
    } else {
	$ack = -1;
    }

    print $from "darrow $time $sendseq\n";
    print $from "uarrow $time $sendseqlast\n";
    print $from "line $time $sendseq $time $sendseqlast\n";

    if ($ack != -1) {
	$winend = $ack + $window;
	if (defined $last_time{$from}) {
	    print $to "line $last_time{$from} $last_ack{$from} ";
	    print $to "$time $last_ack{$from}\n";
	    if ($last_ack{$from} != $ack) {
		print $to "line $time $last_ack{$from} $time $ack\n";
	    } else {
		print $to "dtick $time $ack\n";
	    }

	    if ($Plotwindow) {
		print $to "line $last_time{$from} $last_wind{$from} ";
		print $to "$time $last_wind{$from}\n";
		if ($last_wind{$from} != $winend) {
		    print $to "line $time $last_wind{$from} $time $winend\n";
		} else {
		    print $to "utick $time $winend\n";
		}
	    }
	}
	$last_time{$from} = $time;
	$last_ack{$from} = $ack;
	$last_wind{$from} = $winend;
    }
}

foreach $from (@Froms) {
    &closeOut($from);
}

print "           total: $TotalPackets packets $TotalBytes bytes took ",
    &subTimes($MaxLast, $MinFirst), "\n";
print "      efficiency: ", $TotalBytes/($TotalBytes + $TotalPackets*40), "\n";

exit 0;

