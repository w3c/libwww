#!/usr/local/bin/perl -w

@Locations = ('BIG');

$InPrefix = 'BIG.www26.msiens.tcpdump.';
@OutTemplates = ('"A1-LH-netscape-10/" . $location . "-ft-" . $run . ".txt"',
		 '"A1-LH-netscape-10/" . $location . "-cv-" . $run . ".txt"',
		 '"J5-LH-iexplore-10/" . $location . "-ft-" . $run . ".txt"',
		 '"J5-LH-iexplore-10/" . $location . "-cv-" . $run . ".txt"', 
                 '"J5-LH-netscape-10/" . $location . "-ft-" . $run . ".txt"',
                 '"J5-LH-netscape-10/" . $location . "-cv-" . $run . ".txt"',
		 '"A1-LH-iexplore-10/" . $location . "-ft-" . $run . ".txt"',
		 '"A1-LH-iexplore-10/" . $location . "-cv-" . $run . ".txt"');
@PingChecks = (1, 2, 7, 8, 3, 4, 5, 6);
$Runs = 3;

#$InPrefix = 'BIG.www26.tcpdump.robot.';
#@OutTemplates = ('"A1-LH-http-11/" . $location . "-ft-" . $run . ".txt"',
#		 '"A1-LH-http-11/" . $location . "-cv-" . $run . ".txt"',
#		 '"J5-LH-http-11/" . $location . "-ft-" . $run . ".txt"',
#		 '"J5-LH-http-11/" . $location . "-cv-" . $run . ".txt"',
#		 '"A1-LH-http-11-pl/" . $location . "-ft-" . $run . ".txt"',
#		 '"A1-LH-http-11-pl/" . $location . "-cv-" . $run . ".txt"',
#		 '"J5-LH-http-11-pl/" . $location . "-ft-" . $run . ".txt"',
#		 '"J5-LH-http-11-pl/" . $location . "-cv-" . $run . ".txt"',
#		 '"A1-LH-http-11-pl-zip/" . $location . "-ft-" . $run . ".txt"',
#		 '"A1-LH-http-11-pl-zip/" . $location . "-cv-" . $run . ".txt"',
#		 '"J5-LH-http-11-pl-zip/" . $location . "-ft-" . $run . ".txt"',
#		 '"J5-LH-http-11-pl-zip/" . $location . "-cv-" . $run . ".txt"');
#@PingChecks = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
#$Runs = 5;

$Client = 'BIG';
$Server = 'www26';

sub main
{
    local ($location);
    foreach $location (@Locations) {
#	print "location: \"$location\".\n";
	local ($input) = $InPrefix . $location;
	open (INPUT, "<$input") || die "Can't open \"$input\": $!.\n";
	local ($lineNo, $pass) = (0, '');
	local ($run);
	for ($run = 1; $run <= $Runs; $run++) {
#	    print "run: \"$run\".\n";
	    local ($outTemplate, $pingCheck);
	    local (@pingChecks) = @PingChecks;
	    foreach $outTemplate (@OutTemplates) {
		$pingCheck = shift(@pingChecks);
#		print "pre - outTemplate: \"$outTemplate\".\n";
		local ($outFile, $otherFile);
		$outFile = eval $outTemplate;
		$otherFile = $outFile . '.other';
		print "$input:$lineNo: starting \"$outFile\"\n";
		open (OUTFILE, ">$outFile") || die "Can't open \"$outFile\": $!.\n";
		open (OTHERFILE, ">$otherFile") || die "Can't open \"$otherFile\": $!.\n";
		local ($ignoreOthers, $echo) = (1, 0);
		goto havePass if ($pass ne '');
		while (<INPUT>) {
		    $lineNo++;
		  havePass:
		    if ($pass ne '') {
			$_ = $pass;
			$pass = '';
		    }
		    /[\d.]+\s+(\S+)\s>\s(\S+):/;
		    local ($from, $to) = ($1, $2);
		    if (!defined $from || !defined $to) {
			warn "$input:$lineNo: couldn't parse \"$_\".\n" if (!/ arp /);
			$from = '-' if (!defined $from);
			$to = '-' if (!defined $to);
		    }
		    if (($from =~ /$Client/ && $to =~ /$Server/) || ($from =~ /$Server/ && $to =~ /$Client/)) {
			if (/echo/) {
			    if (/echo request/) {
				$ignoreOthers = 1;
			    } else {
				$echo++;
				#print "$input:$lineNo: echo: $echo pingCheck: $pingCheck.\n";
				last if ($echo == $pingCheck);
			    }
			} else {
			    if ($echo) {
				if (/: R /) {
				    warn "$input:$lineNo: reset ignored.\n";
				} elsif (/: S /) {
				    warn "$input:$lineNo: not enough pings.\n";
				    $pass = $_;
				    last;
				}
			    } else {
				$ignoreOthers = 0;
				print OUTFILE $_;
			    }
			}
		    } else {
			if (!$ignoreOthers) {
			    print OTHERFILE $_;
			}
		    }
		}
		close OUTFILE;
		close OTHERFILE;
	    }
	}
	close INPUT;
    }
}

&main;

