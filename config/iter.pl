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
   <TITLE>Summary of Test Runs with Automatic Generated Xplots</TITLE>
   <META NAME="GENERATOR" CONTENT="Iter/0.9Lead (perl; I; Linux 2.0.18 i586) [iter.pl]">
</HEAD>
<BODY>

<H1><A NAME="xplots">Summary of Test Runs with Automatic Generated Xplots</A></H1>
The following data is a compilation of tcpdumps taken with different client/server 
combinations. Each testrun generates three data sets: HTTP/1.0, HTTP/1.1, and HTTP pipelining.<P>

<H2>Naming Scheme</H2>

The names of the runs are named as follows:

<PRE>
    &lt;SERVER&gt; - &lt;BANDWIDTH&gt; - &lt;LATENCY&gt; - &lt;HTTP CLIENT VERSION&gt;
</PRE>

<DL>
<DT><I>SERVER</I>
<DD><B>J</B> stands for jigsaw and <B>A</B> stands for Apache. The number is for us to keep track of which version of each we are trying.
<DT><I>BANDWIDTH</I>
<DD><B>H</B> stands for high (for example a LAN) and <B>L</B> stands for low, for example PPP
<DT><I>LATENCY</I>
<DD><B>H</B> stands for high latency, for example WAN) and <B>L</B> stands for low, for example LAN
<DT><I>HTTP CLIENT VERSION</I>
<DD>These are the suffixes used: <B>http-10</B> is plain HTTP/1.0, <B>http-11</B> is without pipelining, <B>http-11-pl</B> is with pipelining, and <B>http-11-pl-zip</B> is HTTP/1.1 with pipelining and deflate content encoding.
</DL>

Each combination is run 5 times and each run consists of a first time retrieval followed by a cache validation. That is in all 10 times pr combination. We use this for naming the results:

<UL>
<LI><B>ft</B> (first time - grabbing documents and headers)
<LI><B>cv</B> (cash validation - just getting the headers)
</UL>

As an example, take the following name:

<PRE>
    A1-LH-http-11
</PRE>

<UL>
<LI>The server is <I>Apache</I>
<LI>The bandwidth is <I>low</I>
<LI>The latency is <I>high</I>
<LI>The requests are <I>non-pipelined HTTP/1.1</I>
</UL>

<H2>Data Summary</H2>

The data is furthur <A HREF="#summary">summarized</A> at the end of this file.

<P>
<HR>
<P>
END_OF_HEAD
}

sub endBody
{
    local($output) = @_;
    print $output "<HR>\n";
    print $output "<H1><A NAME=\"summary\">Summaries</A></H1>\n";
    print $output "<TABLE ALIGN=CENTER BORDER=1 CELLSPACING=0 CELLPADDING=2 WIDTH=\"80%\">\n<TR>\n<TH>id</TH>\n";
    foreach $method (@Methods) {
	print $output <<"END_OF_SUMMARY_HEAD";
<TH>$method\npackets</TH>
<TH>bytes</TH>
<TH>time</TH>
<TH>TCP Overhead in %</TH>
END_OF_SUMMARY_HEAD
    }
    print $output "</TR>\n";
    foreach $dir (@ARGV) {
	print $output "<TR>\n<TD>$dir</TD>\n";
	foreach $method (@Methods) {
	    local ($key) = ($dir.'.'.$method);
	    if (defined $AvgPackets{$key}) {
		printf $output "<TD ALIGN=CENTER><A NAME=\"$key.summary\" HREF=\"#$key.item\">%.1f</A></TD>\n<TD ALIGN=CENTER>%.1f</TD>\n<TD ALIGN=CENTER>%.2f</TD>\n<TD ALIGN=CENTER>%.1f</TD>\n", $AvgPackets{$key}, $AvgBytes{$key}, $AvgTime{$key}, $AvgEffic{$key};
	    } else {
		print $output "<TD ALIGN=CENTER></TD>\n<TD ALIGN=CENTER></TD>\n<TD></TD>\n<TD></TD>\n";
	    }
	    }
    print $output "</TR>\n";
    }
    $date = `date`;
    print $output "</TABLE><P><HR><ADDRESS>Generated on $date</ADDRESS>\n";
    print $output "</BODY>\n</HTML>\n";
}

sub showProtocol
{
    local($output, $protocol) = @_;
    print $output <<"END_OF_PROTOCOL";

<H2><A NAME="$protocol">$protocol</A></H2>
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
    $AvgEffic = 100 * (1-$AvgEffic);    
    printf "$key: %.1f packets %d bytes took %.3f - overhead: %.1f%%\n", $AvgPackets, $AvgBytes, $AvgTime, $AvgEffic;
    printf $output "<B><A NAME=\"$key.item\" HREF=\"#$key.summary\">average: %.1f packets %d bytes took %.3f - overhead: %.1f%%</A></B>\n", 
		$AvgPackets, $AvgBytes, $AvgTime, $AvgEffic;
    ($AvgPackets{$key}, $AvgBytes{$key}, $AvgTime{$key}, $AvgEffic{$key}) = 
	($AvgPackets, $AvgBytes, $AvgTime, $AvgEffic);
    print $output "\n";
}

sub showMethod
{
    local($output, $method, $protocol) = @_;
    print $output <<"END_OF_METHOD";

<H3><A NAME="$protocol-$method">$method</A></H3>
END_OF_METHOD
}

sub endMethod
{
    local($output, $method, $protocol) = @_;
    print $output "\n";
}

sub showRun
{
    local($output, $run, $method, $protocol) = @_;
    print $output <<"END_OF_RUN";

<TABLE ALIGN=CENTER BORDER=1 CELLSPACING=0 CELLPADDING=2>
<CAPTION>
  <B><A NAME="$protocol-$method-$run">run-$run</A></B>
</CAPTION>
<TR>
<TH>Host and port (<A HREF=\"$protocol/run-$method-$run.txt\">Raw dump</A>)</TH>
<TH>Packets</TH>
<TH>Bytes</TH>
<TH>Time</TH>
<TH>TCP Overhead in %</TH>
</TR>
END_OF_RUN
}

sub endRun
{
    local($output, $method, $protocol) = @_;
    print $output "</TABLE><P>\n";
}

sub showLine
{
    local($output, $line, $run, $method, $protocol) = @_;
    local($from, $to, $mode, $xplot, $packets, $bytes, $time, $effic) = split(' ', $line);
    print $output "<TR><TD>\n";
    print $output "<EM>" if ($mode eq 'server');
    print $output "<A NAME=\"$protocol-$method-$run-$from\" HREF=\"$protocol/$xplot\">$from</A>";
    print $output "</EM>" if ($mode eq 'server');
    print $output "\n</TD>\n<TD ALIGN=CENTER>$packets</TD>\n";
    print $output "<TD ALIGN=CENTER>$bytes</TD>\n";
    printf $output "<TD ALIGN=CENTER>%.3f</TD>\n", $time;
    warn "$protocol $method $run: can't parse \"$line\".\n" if (!defined $effic);
    printf $output "<TD ALIGN=CENTER>%.1f</TD>\n", 100 * (1-$effic);
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
    printf $output "<TD ALIGN=CENTER>%.3f</TD>\n", $time;
    printf $output "<TD ALIGN=CENTER>%.1f</TD>\n", 100 * (1-$effic);
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
	    local ($base) = substr($file, 0, index($file, '-'));
	    $command = "cd $dir && ../tcpdump2xplot.pl -q -s -f -c -listlist -plot'\"".$base."-\".\$from[0].\":\".\$fromPort.\"-\".\$to[0].\":\".\$toPort.\".$method-$run.\".xplot' $file";
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

