#!/usr/local/bin/perl -w

$InPrefix = 'ppptest.www26.tcpdump.';

@Locations = ('pppsniff',
	      'www26');

@OutTemplates = ('"A1-LH-http-11/" . $location . "-ft-" . $run . ".txt"',
		 '"A1-LH-http-11/" . $location . "-cv-" . $run . ".txt"',
		 '"J5-LH-http-11/" . $location . "-ft-" . $run . ".txt"',
		 '"J5-LH-http-11/" . $location . "-cv-" . $run . ".txt"',
                 '"A1-LH-http-11-pl/" . $location . "-ft-" . $run . ".txt"',
		 '"A1-LH-http-11-pl/" . $location . "-cv-" . $run . ".txt"',
		 '"J5-LH-http-11-pl/" . $location . "-ft-" . $run . ".txt"',
		 '"J5-LH-http-11-pl/" . $location . "-cv-" . $run . ".txt"',
		 '"J5-LH-http-11-pl-zip/" . $location . "-ft-" . $run . ".txt"',
		 '"J5-LH-http-11-pl-zip/" . $location . "-cv-" . $run . ".txt"');

@PingChecks = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

$Runs = 5;

sub main
{
    local ($location);
    foreach $location (@Locations) {
#	print "location: \"$location\".\n";
	local ($input) = $InPrefix . $location;
	open (INPUT, "<$input") || die "Can't open \"$input\": $!.\n";
	local ($lineNo) = 0;
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
		print "post - outFile: \"$outFile\" otherFile: \"$otherFile\".\n";
		open (OUTFILE, ">$outFile") || die "Can't open \"$outFile\": $!.\n";
		open (OTHERFILE, ">$otherFile") || die "Can't open \"$otherFile\": $!.\n";
		local ($ignoreOthers, $echo) = (1, 0);
		while (<INPUT>) {
		    $lineNo++;
		    /[\d.]+\s+(\S+)\s>\s(\S+):/;
		    local ($from, $to) = ($1, $2);
		    if (($from =~ /www26/ && $to =~ /the-dart/) || ($from =~ /the-dart/ && $to =~ /www26/)) {
			if (/echo/) {
			    if (/echo request/) {
				$ignoreOthers = 1;
			    } else {
				$echo++;
				print "$input:$lineNo: echo: $echo pingCheck: $pingCheck.\n";
				last if ($echo == $pingCheck);
			    }
			} else {
			    die "$input:$lineNo: not enough pings.\n" if ($echo);
			    $ignoreOthers = 0;
			    print OUTFILE $_;
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

