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
$plotwindow = 0;
$local = '<$from>';
$remote = '<$to>';
$LPlot = 'a2b.xplot';
$RPlot = 'b2a.xplot';
$Tcpdump = 'STDIN';
$Usage_first = 1;

# other initializations
$ClientPackets = 0;
$ServerPackets = 0;
$ClientBytes = 0;
$ServerBytes = 0;
$StartTime = 0;

sub usage
{
    return if (!$Usage_first);
    $Usage_first = 0;
    print <<"END_OF_USAGE";
Usage: $0 [-w] [-lname<host:port>] [-rname<host:port>] [-lplot<file>] [-rplot<file>] [-?] [-help]
-w: plotwindow
-lname/-rname<host:port>: set local or remote name to <host:port>
-lplot/-rplot<file>: plot local or remote packets in <file>
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
	$plotwindow = 1;
    } elsif (substr($arg, 0, 5) eq 'lname') {
        $local = substr($arg, 5);
    } elsif (substr($arg, 0, 5) eq 'rname') {
        $remote = substr($arg, 5);
    } elsif (substr($arg, 0, 5) eq 'lplot') {
        $LPlot = substr($arg, 5);
    } elsif (substr($arg, 0, 5) eq 'rplot') {
        $RPlot = substr($arg, 5);
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

while (@ARGV[0] =~ /^-/ && &ReadArg(shift(@ARGV))) {}
if (($inputFile = shift(@ARGV))) {
    open (TCPDUMP, "<$inputFile") || die "error opening \"$inputFile\" for reading: $!\n";
    $Tcpdump = 'TCPDUMP';
}

open(ABPL, ">$LPlot") || die "error opening \"$LPlot\" for writing: $!\n";
open(BAPL, ">$RPlot") || die "error opening \"$RPlot\" for writing: $!\n";

@lastsendseq = (-1, -1);
@lastsendseqlast = (-1, -1);

for ($lineNo = 1; <$Tcpdump>; $lineNo++) {
    chop;
    split(/ /);
    $time = $_[0];
    $from = $_[1];
    $to = $_[3];
    chop($to);			# strip off colon
    $flags = $_[4];

    $local = eval($1) if ($local =~ m/<(\$\w+)>/);
    $remote = eval($1) if ($remote =~ m/<(\$\w+)>/);

    if ($from eq $local) {
	$topl = BAPL;
	$frompl = ABPL;
	$lastndx = 0;
	$ClientPackets++;
	$bytes = 'ClientBytes';
    } else {
	$topl = ABPL;
	$frompl = BAPL;
	$lastndx = 1;
	$ServerPackets++;
	$bytes = 'ServerBytes';
    }

    if ($lineNo == 1) {
	print ABPL "timeval signed\ntitle\n$local-->$remote\n";
	print BAPL "timeval signed\ntitle\n$remote-->$local\n";
	$StartTime = $time;
    }

    if (/$flags ([0-9]*):([0-9]*)\(([0-9]*)\) /) {
	$lastsendseq[$lastndx] = $sendseq = $1;
	if (!defined $firstseq[$lastndx]) {
	    $firstseq[$lastndx] = $1;
	}
	$lastsendseqlast[$lastndx] = $sendseqlast = $2;
	$datalength = $3;
	$$bytes += $3;
    } else {
	$sendseq = $lastsendseq[$lastndx];
	$sendseqlast = $lastsendseq[$lastndx];
    }

    $sendseq -= $firstseq[$lastndx];
    $sendseqlast -= $firstseq[$lastndx];

    if (/win ([0-9]*)/) {	# remove space in "]*) /" to handle win <num><EOL> - EGP
	$window = $1;
    } else {
	$window = -1;
    }

    if (/ack ([0-9]*) /) {
	$ack = $1;
	if ($lastndx) {
	    $ack -= $firstseq[0] if ($ack >= $firstseq[0]);
	} else {
	    $ack -= $firstseq[1] if ($ack >= $firstseq[1]);
	}
    } else {
	$ack = -1;
    }

    print $frompl "darrow $time $sendseq\n";
    print $frompl "uarrow $time $sendseqlast\n";
    print $frompl "line $time $sendseq $time $sendseqlast\n";

    if ($ack != -1) {
	$winend = $ack + $window;
	if (defined $last_time[$lastndx]) {
	    print $topl "line $last_time[$lastndx] $last_ack[$lastndx] ";
	    print $topl "$time $last_ack[$lastndx]\n";
	    if ($last_ack[$lastndx] != $ack) {
		print $topl "line $time $last_ack[$lastndx] $time $ack\n";
	    } else {
		print $topl "dtick $time $ack\n";
	    }

	    if ($plotwindow) {
		print $topl "line $last_time[$lastndx] $last_wind[$lastndx] ";
		print $topl "$time $last_wind[$lastndx]\n";
		if ($last_wind[$lastndx] != $winend) {
		    print $topl "line $time $last_wind[$lastndx] $time $winend\n";
		} else {
		    print $topl "utick $time $winend\n";
		}
	    }
	}
	$last_time[$lastndx] = $time;
	$last_ack[$lastndx] = $ack;
	$last_wind[$lastndx] = $winend;
    }
}

print ABPL "go\n";
print BAPL "go\n";
close(ABPL); close(BAPL);

$TotalPackets = $ClientPackets+$ServerPackets;
$TotalBytes = $ClientBytes+$ServerBytes;

print "client to server: $ClientPackets packets $ClientBytes bytes\n";
print "server to client: $ServerPackets packets $ServerBytes bytes\n";
print "           total: $TotalPackets packets $TotalBytes bytes\n";
print "            time: ", &subTimes($time, $StartTime), "\n";
print "      efficiency: ", $TotalBytes/($TotalBytes + $TotalPackets*40), "\n";

exit 0;

