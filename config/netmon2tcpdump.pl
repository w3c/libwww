#!/usr/local/bin/perl -w

$ErrPrefix = 'netmon2tcpdump!';
$TimeThreshold = 5; # seconds
$WatchSyns = 0;

sub main
{
    local ($input, @outNames) = @_;
    local ($line);
    local ($lineNo, $output, $outNo) = (0, 'OUTPUT', '1');
    undef $lastTime;
    while  (($line = <$input>)) {
	local ($num, $time, $srcMac, $dstMac, $flags, $len, $seqStart, $seqEnd, $ack, $win, $srcPort, $dstPort, $srcIP, $dstIP);
	chop $line;
	$lineNo++;
	$line =~ /^(\d+)\s+([\d\.]+)\s+([\dA-F]+)\s+([\dA-F]+)\s+TCP\s+([A-Z\.]+),\s+len:\s+(\d+),\s+seq:\s*(\d+)-(\d+),\s+ack:\s*(\d+),\s+win:\s*(\d+),\s+src:\s+(\d+)\s+dst:\s+(\d+)\s+([\w\d\.]+)\s+([\w\d\.]+)\s+IP/ || 
	    die "No match on line $lineNo: $_.\n";
	($num, $time, $srcMac, $dstMac, $flags, $len, $seqStart, $seqEnd, $ack, $win, $srcPort, $dstPort, $srcIP, $dstIP) = ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14);
	$flags =~ s/\.|A//g;
	$flags = '.' if ($flags eq '');
	if (!defined $lastTime || $time - $lastTime > $TimeThreshold || ($WatchSyns && $flags =~ /S/ && $dstIP =~ /pfunk/)) {
	    if (defined($lastTime)) {
		close $output;
	    }
	    local ($outName) = $outNames[$outNameIndex++];
	    local ($fname) = "$outName-$outNo.txt";
	    if ($outNameIndex > $#outNames) {
		$outNameIndex = 0;
		$outNo++;
	    }
	    open ($output, ">$fname") || die "$ErrPrefix can't open \"$fname\": $!.\n";
	    print stderr "$ErrPrefix writing to \"$fname\".\n";
	}
	$lastTime = $time;
	print $output "$time $srcIP.$srcPort > $dstIP.$dstPort: $flags";
	print $output " $seqStart:$seqEnd(", $seqEnd - $seqStart, ")" if ($seqEnd != $seqStart || $flags =~ /S/);
	print $output " ack $ack" if ($ack);
	print $output " win $win" if ($win);
	print $output "\n";
    }
}

if ($ARGV[0] eq '-S') {
    $WatchSyns = 1;
    shift @ARGV;
}

main('stdin', @ARGV);

