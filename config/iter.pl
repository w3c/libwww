#!/usr/local/bin/perl -w

$FileSearch = '-1.txt';
$FileIndex = 1;
$Runs = 5;
$Debug = 0;
@Methods = ('ft', 'cv');

sub showBody
{
    local($output) = @_;
    print $output <<"END_OF_HEAD";
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2//EN">
<HTML>
<HEAD>
   <TITLE>xplots</TITLE>
   <META NAME="GENERATOR" CONTENT="Iter/0.9Lead (perl; I; Linux 2.0.18 i586) [iter.pl]">
</HEAD>
<BODY>

<H1><A NAME="xplots">xplots</A></H1>
The following data is a compilation of tcpdumps taken with different client/server 
combinations. Most combinations were comfounded by operating the the client in two
modes, ft (first time - grabbing documents and headers) and cv (cash validation - 
just getting the headers). This data is furthur <A HREF="#summary">summarized</A>.
<UL>
END_OF_HEAD
}

sub endBody
{
    local($output) = @_;
    print $output "</UL>\n<HR>\n";
    print $output "<H1><A NAME=\"summary\">summaries</A></H1>\n";
    print $output "<TABLE ALIGN=ABSCENTER BORDER=1 CELLSPACING=0 CELLPADDING=0 >\n<TR>\n<TD>id</TD>\n";
    foreach $method (@Methods) {
	print $output <<"END_OF_SUMMARY_HEAD";
<TD ALIGN=CENTER><B>$method</B>\npackets</TD>
<TD ALIGN=CENTER>bytes</TD>
<TD>time</TD>
<TD>effic</TD>
END_OF_SUMMARY_HEAD
    }
    print $output "</TR>\n";
    foreach $dir (@ARGV) {
	print $output "<TR>\n<TD>$dir</TD>\n";
	foreach $method (@Methods) {
	    local ($key) = ($dir.'.'.$method);
	    if (defined $AvgPackets{$key}) {
		printf $output "<TD ALIGN=CENTER><A NAME=\"$key.summary\"><A HREF=\"#$key.item\">%.1f</A></A></TD>\n<TD ALIGN=CENTER>%.1f</TD>\n<TD>%.2f</TD>\n<TD>%.3f</TD>\n", $AvgPackets{$key}, $AvgBytes{$key}, $AvgTime{$key}, $AvgEffic{$key};
	    } else {
		print $output "<TD ALIGN=CENTER></TD>\n<TD ALIGN=CENTER></TD>\n<TD></TD>\n<TD></TD>\n";
	    }
	    }
    print $output "</TR>\n";
    }
    print $output "</TABLE>\n";
    print $output "</BODY>\n</HTML>\n";
}

sub showProtocol
{
    local($output, $protocol) = @_;
    print $output <<"END_OF_PROTOCOL";

<LI><A NAME="$protocol"></A><H2>$protocol</H2></LI>
<UL>
END_OF_PROTOCOL
}

sub endProtocol
{
    local($output, $method, $protocol) = @_;
    local ($key) = "$protocol.$method";
    $AvgPackets /= $Runs;
    $AvgBytes /= $Runs;
    $AvgTime /= $Runs;
    $AvgEffic /= $Runs;
    printf "$key: %.1f packets %d bytes took %.3f - effic: %.3f.\n", $AvgPackets, $AvgBytes, $AvgTime, $AvgEffic;
    printf $output "<LI><H3><A NAME=\"$key.item\"><A HREF=\"#$key.summary\">average: %.1f packets %d bytes took %.3f - effic: %.3f.</A></A></H3></LI>\n", 
		$AvgPackets, $AvgBytes, $AvgTime, $AvgEffic;
    ($AvgPackets{$key}, $AvgBytes{$key}, $AvgTime{$key}, $AvgEffic{$key}) = 
	($AvgPackets, $AvgBytes, $AvgTime, $AvgEffic);
    print $output "</UL>\n";
}

sub showMethod
{
    local($output, $method, $protocol) = @_;
    print $output <<"END_OF_METHOD";

<LI><A NAME="$protocol-$method"></A><H3>$method</H3></LI>
<UL>
END_OF_METHOD
}

sub endMethod
{
    local($output, $method, $protocol) = @_;
    print $output "</UL>\n";
}

sub showRun
{
    local($output, $run, $method, $protocol) = @_;
    print $output <<"END_OF_RUN";

<LI><A NAME="$protocol-$method-$run"></A><H3>run-$run:</H3></LI>
<TABLE ALIGN=ABSCENTER BORDER=1 CELLSPACING=0 CELLPADDING=0 >
<TR>
<TD ALIGN=CENTER>id</TD>
<TD ALIGN=CENTER>packets</TD>
<TD ALIGN=CENTER>bytes</TD>
<TD>time</TD>
<TD>effic</TD>
</TR>
END_OF_RUN
}

sub endRun
{
    local($output, $method, $protocol) = @_;
    print $output "</TABLE>\n";
}

sub showLine
{
    local($output, $line, $run, $method, $protocol) = @_;
    local($from, $to, $mode, $xplot, $packets, $bytes, $time, $effic) = split(' ', $line);
    print $output "<TR><A NAME=\"$protocol-$method-$run-$from\"></A>\n<TD>";
    print $output "<EM>" if ($mode eq 'server');
    print $output "<A HREF=\"$protocol/$xplot\">$from</A>";
    print $output "</EM>" if ($mode eq 'server');
    print $output "\n</TD>\n<TD ALIGN=CENTER>$packets</TD>\n";
    print $output "<TD ALIGN=CENTER>$bytes</TD>\n";
    printf $output "<TD>%.3f</TD>\n", $time;
    printf "$line, $run, $method, $protocol, $effic\n" if (!defined $effic);
    printf $output "<TD>%.3f</TD>\n", $effic;
    print $output "</TR>\n";
}

sub showSummary
{
    local($output, $summary, $run, $method, $protocol) = @_;
    local($id, $packets, $bytes, $time, $effic) = split(' ', $summary);
    $AvgPackets += $packets;
    $AvgBytes += $bytes;
    $AvgTime += $time;
    $AvgEffic += $effic;
    print $output "<A NAME=\"$protocol-$method-$run-summary\"></A><TR>\n";
    print $output "<TD ALIGN=CENTER>-total-</TD>\n";
    print $output "<TD ALIGN=CENTER>$packets</TD>\n";
    print $output "<TD ALIGN=CENTER>$bytes</TD>\n";
    printf $output "<TD>%.3f</TD>\n", $time;
    printf $output "<TD>%.3f</TD>\n", $effic;
    print $output "</TR>\n";
}

if ($ARGV[0] eq '-d') {
    $Debug = 1;
    shift @ARGV;
}

open(HTML, '>summary.html') || die "iter: Couldn't open summary file \"summary.html\" for writing: $!.\n";
&showBody('HTML');

foreach $dir (@ARGV) {
    chop $dir if ($dir =~ /.*\/$/);
    if (!open(ONES, "cd $dir && ls *$FileSearch |")) {
	warn "iter: Couldn't run 'ls \"*$FileSearch\"': $!.\n";
	next;
    }
    &showProtocol('HTML', $dir);
    foreach $file (<ONES>) {
	chop $file;
	$i = index($file, $FileSearch);
	next if ($i == $[ - 1);
	$i += $FileIndex;
	$AvgPackets = 0;
	$AvgBytes = 0;
	$AvgTime = 0;
	$AvgEffic = 0;
	$method = substr($file, $i-3, 2);
	&showMethod('HTML', $method, $dir);
	foreach $run (1 .. $Runs) {
	    substr($file, $i, 1) = $run;
	    $command = "cd $dir && ../tcpdump2xplot.pl -q -s -f -c -listlist -plot'\$from[0].\":\".\$fromPort.\"-\".\$to[0].\":\".\$toPort.\".$method-$run.\".xplot' $file";
	    if ($Debug) {
		print $command, "\n";
		next;
	    }
	    if (!open(CMD, "$command |")) {
		warn "iter: Couldn't run '$command': $!.\n";
		next;
	    }
	    while (<CMD>) {
		print;
	    }
	    open(LIST, "<$dir/list") || die "iter: Couldn't open \"$dir/list\" for reading: $!.\n";
	    &showRun('HTML', $run, $method, $dir);
	    while (<LIST>) {
		chop;
		if (/^summary: /) {
		    &showSummary('HTML', $_, $run, $method, $dir)
		} else {
		    &showLine('HTML', $_, $run, $method, $dir);
		}
	    }
	    &endRun('HTML', $run, $method, $dir);
	}
	&endProtocol('HTML', $method, $dir);
    }
    &endMethod('HTML', $dir);
}

&endBody('HTML');
close(HTML);

