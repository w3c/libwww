#!/usr/local/bin/perl
# 
# syntax: makedefs.pl [options] [files | @[filelist]]
#   where options are 
#     -d<directory>: look for files in <directory>.
#     -header<text>: add <text> to top of file.
#     -e: no more options (like grep -e). Allows access to files with
#         a leading - in their name.
#     <files> is one or more files in the form <filename[@<offset>]>
#       where <filename> is a file in which to look for functions.
#     <offset> is the desired offset for the first functions. The <offset> 
#       directive will be ignored if it will cause a conflict with a 
#       function offset already defined in the def file.
#     [filelist] is a file containing these lists. If ommited, makedefs
#       looks to stdin for a filelist.
#
# example call: ../../../config/makedefs.pl -headerEXPORTS -d.. @wwwcore.files
#   read the list of files from wwwcore.files, grep these files for 'PUBLIC',
#   and send the output to stdout.
#
# EGP July 5 96
#

$NumberDefs'granularity = 100;  # how much the index changes for each file
$NumberDefs'at = 24;            # target at sign column
$NumberDefs'tab = 8;            # how wide a tab looks in your editor
$NumberDefs'offset = 0;         # starting number
$NumberDefs'index;
$WorkDir = '';

sub NumberDefs'numberEach
{
    package NumberDefs;
    local($name, $passedOffset) = @_;
    if ($name eq "") {
	print "\n";
	next;
    }

    if ($passedOffset && $passedOffset > $offset) {
	$index = $offset = $passedOffset;
    } elsif ($name =~ /^;/) {
	$index = $offset = (int($index / $granularity) +1) * $granularity;
    }

    print $name;

    $len = length($name);
    if ($len >= $at) {
	print " ";
    } else {
	$tabz = (($at-$len)/$tab);
	for ($i = 0; $i < $tabz; $i++) {
	    print "\t";
	}
    }
    print "@ ", $index, "\n";
    $index ++;
}

# numberAll - call this to renumber a def file
#
sub NumberDefs'numberAll
{
    package NumberDefs;
    $INPUT = $_[0];
    while(<$INPUT>) {
	local($name) =  split(" ",$_);
	&NumberDefs'numberEach($name);
    }
}

sub GrepPublic
{
    local($source, $offset) = split('@', @_[0]);
    if (!open (SOURCE, $WorkDir.$source)) {
	warn "Can't open $WorkDir$source: $!\n"; 
	return;
    }
    &NumberDefs'numberEach(';'.$source, $offset); # add source file as a source
    while (<SOURCE>) {
	if (!/^PUBLIC\s/) {
            next;
        }
        if (!/(\w*)\W*\(/) {
            warn "assuming global: ", $_;
            next;
        }
        local($func) = $1;
#       printf "\$_:\"%s\" \$func:\"%s\"\n", $_, $func;
        &NumberDefs'numberEach($func);
    }
}

sub ReadArg
{
    local($arg) = substr(@_[0], 1);
    return 0 if ($arg eq 'e');
    if (substr($arg, 0, 6) eq 'header') {
	print substr($arg, 6), "\n";
    } elsif (substr($arg, 0, 1) eq 'd') {
	$WorkDir = substr($arg, 1);
	$WorkDir .= '/' if (!($WorkDir =~ /\/$/));
    } else {
	print "unknown argument \"$arg\".\n";
    }
    return 1;
}

while (@ARGV[0] =~ /^-/ && &ReadArg(shift(@ARGV))) {}
if (@ARGV[0] =~ /^@(.*)/) {
    local($name) = $1;
    if ($name =~ /\w+/) {
	if (!open(LIST, $name)) {
	    die "Can't open \"$name\", $!";
	}
	while (<LIST>) {
	    chop;
	    &GrepPublic($_);
	    print "\n";
	}
	exit 0;
    }
    while (<STDIN>) {
	chop;
	&GrepPublic($_);
	print "\n";
    }
    exit 0;
}
foreach $source (@ARGV) {
    &GrepPublic($source);
}

#&NumberDefs'numberAll(STDIN);

