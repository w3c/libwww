#!/usr/local/bin/perl
# 
# syntax: winConfigure.pl <top directory>
#
# EGP May 26 98
#


my $top = shift;
$top =~ s/\/$//;
my $arg0 = $0;
$arg0 =~ s/\/[^\/]+$//;
(defined $top) || ($top = '.');
&checkDir($top);

sub parseMakefile {
    my $dir = shift;
    my $defs = shift;
    my $file = $dir.'/Makefile.am';
    local *FILE;
    open(FILE, "< $file") || die "can't open \"$file\": $!";
    my $continuation = ''; # lines ending in a \\
    while (<FILE>) {
	my $line = $_;
	$line =~ s/#.*//;
	my $fullLine = $continuation.$line;
	$continuation = '';
	($line =~ s/\s*\\$//) && ($continuation .= $fullLine, next);
	($line =~ m/:/) && ($continuation = $fullLine, next);
	($fullLine =~ m/:/) && ($line =~ m/^\s+\S+/) && ($continuation = $fullLine,next);
#    $fullLine =~ s/\n/ /g;   # 'cause I can't seem to match "as\ndf"=~m/^.*$/ !!!
	(&getDef($fullLine, $defs)) && (next);
#    ($fullLine =~ m/(\S+)\s*=\s*(.+)/) && ($DEFS{$1} = $2, next);
#    ($fullLine =~ m/(\S+)\s*:\s*(.+)/) && ($RULES{$1} = $2, next);
	($line eq "\n") || (warn "what is $file:$.:\"$line\"?\n");
    }
    &getDef($continuation);
#  ($continuation =~ m/(\S+)\s*=\s*(.+)/) && ($DEFS{$1} = $2);
#  ($continuation =~ m/(\S+)\s*:\s*(.+)/) && ($RULES{$1} = $2);
}

sub getDef {
    my $check = shift;
    my $defs = shift;
    chomp $check;
    if ($check =~ m/\A(\S+)\s*=/m) {
	my ($name, $value) = split('=', $check, 2);
	$name =~ s/\s//g;
	$value =~ s/\\//g;
	$value =~ s/\s+/ /g;
	$value =~ s/^\s//;
	my @values = split(' ', $value);
	$$defs{$name} = \@values;
	return 1;
    }
    # now something for rules... someday - EGP
    if ($check =~ m/\A(\S+)\s*:/m) {
	return 1;
    }
    return 0;
}

# use makedefs.pl;

sub checkDir {
    my $dir = shift;
    my (%DEFS);
    my (%DEPENDENTS, %VERBS);
    &parseMakefile($dir, \%DEFS);
    my @subdirs = @{$DEFS{'SUBDIRS'}};
    my @builtSources = @{$DEFS{'BUILT_SOURCES'}};
    my $built;
    foreach $built (@{$DEFS{'BUILT_SOURCES'}}) {
	my $dest = $dir.'/'.$built;
	my $input = $dest;
	$input =~ s/\.[^\.\/]*$/.files/;
	if ($built =~ m/\.def$/) {
	    `perl $arg0/makedefs.pl -headerEXPORTS -d$dir/.. \@$input > $dest`;
	} else {
	    &htmlToH($dest);
	}
    }
    my $subdir;
    foreach $subdir (@{$DEFS{'SUBDIRS'}}) {
	&checkDir($dir.'/'.$subdir);
    }
}

sub htmlToH
{
    my $output = shift;
    my $input = $output;

    local (*OPEN, *CLOSE, *NL) = (\0, \1, \2);
    my %transition = (
		      '_TEXT' => ['','',''], 
		      'html'  => ['','',''],
		      'title'  => ['					','',''],
		      'pre'  => ['	','','	'],
		      'h1'  => ['!','!',''],
		      'h2'  => ['.','.',''],
		      'h3'  => ['(',')',''],
		      'ul'  => ['	 ','','	 '],
		      'li'  => ['o ','','	 '],
		      );
    if ($output =~ m/\.h$/) {
	$transition{'html'} = ["/*","*/",""];
	$transition{'pre'} = ["*/\n","\n/*",""];
    }

    $input =~ s/\.[^\.\/]*$//;
    $input .= '.html';
    (open(INPUT, "< $input")) || (warn "can't open input file \"$input\": $!", return);
    (open(OUTPUT, "> $output")) || (warn "can't open output file \"$output\": $!", return);
    my $contents = join('', <INPUT>);
    my $inTag = 0;
    my $tag = '';
    my $elem;
    foreach $elem (split('[<>]', $contents)) {
	if ($inTag) {
#      print OUTPUT ${$transition{$tag}}[$CLOSE] if (defined $transition{$tag});
	    ($close, $tag, $space, $rest) = $elem =~ m/(\/?)(\S+)(\s*)(.*)/si;
	    $tag =~ tr/A-Z/a-z/;
	    if ($close eq '/') {
		print OUTPUT $ {$transition{$tag}}[$CLOSE] if (defined $transition{$tag});
	    } else {
		print OUTPUT $ {$transition{$tag}}[$OPEN] if (defined $transition{$tag});
	    }
	} else {
	    (defined $transition{$tag}) && ($elem =~ s/\n/"\n".$ {$transition{$tag}}[$NL]/ge);
	    $elem =~ s/\&lt;/</g;
	    $elem =~ s/\&gt;/>/g;
	    $elem =~ s/\&amp;/&/g;
	    print OUTPUT $elem;
	}
	$inTag ^= 1;
    }
}

