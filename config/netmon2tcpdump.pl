#!/usr/local/bin/perl

sub main
{
    local ($input, $output) = @_;
    local ($line);
    local ($lineNo) = (0);
    while  (($line = <$input>)) {
	local ($num, $time, $srcMac, $dstMac, $type, $rest);
	chop $line;
	$lineNo++;
	$line =~ /^(\d+)\s+([\d\.]+)\s+([\dA-F]+)\s+([\dA-F]+)\s+(\w+)\s+(.*)/ || 
	    die "IP: No match on line $lineNo: $_.\n";
	($num, $time, $srcMac, $dstMac, $type, $rest) = ($1, $2, $3, $4, $5, $6);
	if ($type eq 'TCP') {
	    local ($flags, $len, $seqStart, $seqEnd, $ack, $win, $srcPort, $dstPort, $srcIP, $dstIP);
	    $rest =~ /^([A-Z\.]+),\s+len:\s+(\d+),\s+seq:\s*(\d+)-(\d+),\s+ack:\s*(\d+),\s+win:\s*(\d+),\s+src:\s+(\d+)\s+dst:\s+(\d+)\s+([\w\d\.]+)\s+([\w\d\.]+)\s+IP/ || 
		die "TCP: No match on line $lineNo: $_.\n";
	    ($flags, $len, $seqStart, $seqEnd, $ack, $win, $srcPort, $dstPort, $srcIP, $dstIP) = ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10);
	    $flags =~ s/\.|A//g;
	    $flags = '.' if ($flags eq '');
	    print $output "$time $srcIP.$srcPort > $dstIP.$dstPort: $flags";
	    print $output " $seqStart:$seqEnd(", $seqEnd - $seqStart, ")" if ($seqEnd != $seqStart || $flags =~ /S/);
	    print $output " ack $ack" if ($ack);
	    print $output " win $win" if ($win);
	    print $output "\n";
	} elsif ($type == 'ICMP') {
	    local ($message, $whatIsA, $a, $whatIsB, $b, $srcIP, $dstIP);
	    $rest =~ /^([^,]*),\s+(From|To)\s+([\w\d\.]+)\s+(To|From)\s+([\w\d\.]+)\s+([\w\d\.]+)\s+([\w\d\.]+)\s+IP/ || 
		die "ICMP: No match on line $lineNo: $_.\n";
	    local ($message, $whatIsA, $a, $whatIsB, $b, $srcIP, $dstIP) = ($1, $2, $3, $4, $5, $6, $7);
	    print $output "$time $srcIP > $dstIP: ";
	    if ($message eq 'Echo') {
		print $output "echo request";
	    } elsif ($message eq 'Echo Reply') {
		print $output "echo reply";
	    } else {
		die "IP: Don't know how to handle message \"$message\".\n";
	    }
	    print $output "\n";
	    $from = '';
	    $to = '';
	} else {
	    die "IP: Don't know how to handle \"$type\".\n";
	}
    }
}

open(INPUT, "<BIG.www26.robot.netmon.BIG") || die "Can't open input: $!\n";
open(OUTPUT, ">BIG.www26.robot.tcpdump.BIG") || die "Can't open output: $!\n";

main('INPUT', 'OUTPUT');

