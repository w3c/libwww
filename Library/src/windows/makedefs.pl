#!/usr/local/bin/perl

$NumberDefs'at = 24;            # target at sign column
$NumberDefs'tab = 8;            # how wide a tab looks in your editor
$NumberDefs'offset = 0;         # starting number
$NumberDefs'index;

sub NumberDefs'numberEach
{
    package NumberDefs;
    local($name) = @_;
    if ($name eq "") {
	print "\n";
	next;
    }
    if ($offset == 0) {
	print "EXPORTS\n";
    }
    print $name;
    if ($name =~ /^;/) {
	($module, $atsign, $start) = split(" ", $_);
#	print "\"", $module, "\" ", $start;
	if ($start && $start > $offset) {
	    $offset = $start;
	} else {
	    $offset += 100;
	}
	$index = $offset;
    }
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
    local($source) = @_;
    if (!open (SOURCE, "../".$source)) {
	warn "Can't open ../$source: $!\n"; 
	return;
    }
    &NumberDefs'numberEach(';'.$source); # add source file as a source
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

if (@ARGV[0] =~ /@(.*)/) {
    local($name) = $1;
    if ($name =~ /\w+/) {
	if (!open(LIST, $name)) {
	    die "Can't open \"$1\", $!";
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

