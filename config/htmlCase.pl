#!/usr/local/bin/perl

$Mode = 'l'; #	toupper, alternate, binary
$LastWasUp = 0;
reset %WordCount;

sub translateWord
{
    local ($word) = @_;
    $word =~ tr/A-Z/a-z/;
    $WordCount{$word}++;
    if ($Mode eq 'l') {
	$word =~ tr/A-Z/a-z/;
    } elsif ($Mode eq 'u') {
	$word =~ tr/a-z/A-Z/;
    } elsif ($Mode eq 'a') {
	if ($LastWasUp) {
	    $word =~ tr/A-Z/a-z/;
	    $LastWasUp = 0;
	} else {
	    $word =~ tr/a-z/A-Z/;
	    $LastWasUp = 1;
	}
    } else {
	local ($bits)=$WordCount{$word} - 1;
	local (@letters) = split(//,$word);
	foreach $index (0..$#letters)
	{
	    if ($bits % 2)
	    {
		$letters[$index] =~ tr/a-z/A-Z/;
		substr($word, $index, 1) = $letters[$index];
	    }
	    $bits >>= 1;
	}
    }
    return $word;
}

sub translateTag
{
    local ($text, $inTag) = @_;
    return $text if (!$inTag);
    local ($ret, $atom, $inQuote, $lookFor) = ('', $[, 0, '([ \t"\'=])');
    local (@atoms) = split(/$lookFor/, $text);
    for ($atom = $[; $atom <= $#atoms; $atom++) {
	local ($word) = $atoms[$atom];
	if ($inQuote && $word eq $inQuote) {
	    $inQuote = 0;
	} elsif (!$inQuote && ($word eq '"' || $word eq '\'')) {
	    $inQuote = $word;
	} elsif (!$inQuote && !($word =~ m/$lookFor/)) {
	    $word = &translateWord($word);
	}
	$ret .= $word;
    }
    return $ret;
}

sub main()
{
    local ($input, $output) = @_;
    local ($inTag) = (0);
    while (<$input>) {
	local ($curPos, $lastPos) = ($[, $[);
	while (($curPos = index($_, $inTag ? '>' : '<', $curPos)) >= $[) {
	    print $output &translateTag(substr($_, $lastPos, $curPos - $lastPos), $inTag), $inTag ? '>' : '<';
	    $inTag ^= 1;
	    $curPos++;
	    $lastPos = $curPos;
	}
	print $output &translateTag(substr($_, $lastPos), $inTag);
    }
}

if ($#ARGV == '0') {
    $Mode = $ARGV[0];
} else {
    die "need a l (lower) | u (upper) | a (alternating) | b (binary)";
}
&main('stdin', 'stdout');

