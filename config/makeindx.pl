#!/usr/local/bin/perl
#<PRE> netscape <B>insists</B> on interperting this as html
#
# makeindx.pl <html directory|html file list> <@header files list|header file>
#
# for example, from the Library/User directory:
#   ../../config/makeindx.pl ../src ../src/WWW*.html
# what it does:
#   1. get a list of all the html files in ../src.
#   2. read each DLL definition (../src/WWW*.html) and associate it with 
#      each of its embedded files (included header files).
#   3. read all the html files from 1 and scan for functions.
#   4. print these sorted by function name,
#   5. DLL name,
#   6. and module name
#
# EGP July 5 96
#

# output an HTML pointer to $source[#$naneTag] labeled $name
# also output and optional extra pointer to $dll named $dll
sub numberEach
{
    local($source, $name, $nameTag, $dll) = @_;
    if ($name eq "") {
	print "\n";
	next;
    }
#    $dll =~ s/\.docs//;
    print "  <LI><A HREF=\"$Dir$source";
    if (defined($nameTag)) {
	print "#$nameTag";
    }
    print "\">$name</A>";
    if (defined($dll) && $dll ne '') {
	print " --- <A HREF=\"#$dll\">$dll</A>";
    }
    print "\n";
}

# find functions in a module
sub findFunctions
{
    local($source, $nameTag) = @_;
    return if ($source =~ /sysdep/);
    return if ($source =~ /WWW/);
    if (!open (SOURCE, $source)) {
	warn "Can't open $source: $!\n";
	return;
    }
    $source =~ s/.*\/(\w+\.html)/$1/;
    while (<SOURCE>) {
        if (/<A +NAME *= *"([^"]*)">/i) #"
	{
            $nameTag = $1;
        }
	s/<[^>]*>//g;
	if (!/^extern\s/) {
            next;
        }
        local($func);
        # func (
        if (/(\w*)\W*\(/) {
            $func = $1;
	# ^extern [const|unsigned|signed] word func;
        } elsif (/^extern\W+const|unsigned|signed\W+\w*\W+(\w*);/){
            $func = $1;
        } elsif (/^extern\W+\w*\W+(\w*);/){
            $func = $1;
	# func, func
        } elsif (/,/) {
	    while (/(\w+)\W*,(.*)/) {
		$func = $1;
		$_ = $2;
		$Functions{$func} = $source.'#'.$nameTag;
		push(@FunctionList, $func);
	    }
	    if (/(\w+)\W*;/) {
		$func = $1;
	    } else {
		/(\w+)/;
		$func = $1;
		warn "likely line wrap for list ($source,$.): \"$_\"?\n";
	    }
	} else {
	    warn "skipping ($source,$.)g: \"$_\"?\n";
	    next;
	}
        $Functions{$func} = $source.'#'.$nameTag;
        push(@FunctionList, $func);
#        print "$func $Functions{$func}\n";
    }
}

#sub getDLLs
#{
#    local($list) = @_;
#    open(LIST, $list) || die "Couldn't open list file \"$list\":$!\n";
#    $list =~ s/.*\/(\w+\.docs)/$1/;
#     $list =~ s/\.docs//;
#    push(@DLLList, $list);
#    while (<LIST>) {
#	 chop;
#	 if (defined($Modules{$_})) {
#	     $Modules{$_} .= "#$list";
#	     $DLLS{$_} .= "$list";
#	 } else {
#	     warn "unknown file \"$_\".\n";
#	 }
#    }
#}

# reads include file for a DLL and looks for included modules
sub getDLLs
{
    local($list) = @_;
    open(LIST, "grep \"#include\" $list|grep -v sysdep|") || die "Couldn't open list file \"$list\":$!\n";
    $list =~ s/.*\/(\w+\.html)/$1/;
    $list =~ s/\.html//;
    push(@DLLList, $list);
    while (<LIST>) {
	chop;
	s/<[^>]*>//g;
	s/#include\s+\"//;
	s/\.h\".*/.html/;
	if (defined($Modules{$_})) {
	    $Modules{$_} .= "#$list";
	    $DLLS{$_} .= "$list";
	} else {
	    warn "unknown file \"$_\" in \"$list\".\n";
	}
    }
}

sub printHeader
{
    print <<"EndOfHeader";
<HTML>
<HEAD>
  <TITLE>W3C Reference Library libwww external symbol list</TITLE>
</HEAD>
<BODY>
<H1>
  External Symbols
</H1>
<PRE>
/*
**      (c) COPYRIGHT MIT 1996.
**      Please first read the full copyright statement in the file COPYRIGH.
*/
</PRE>
Here is a list of all the external symbols in the 
libwww API documentation. For more comprehensive descriptions, see the 
<A HREF="../User/Overview.html">user documentation</A>
<P>
The functions are sorted by 
<UL>
<LI><A HREF="#byFunction">name</A>, 
<LI><A HREF="#byDLL">DLL</A>,
<LI><A HREF="#byModule">module</A>
EndOfHeader
#<PRE>
}

sub printByFunction
{
    print <<"EndOfByFunction";
</UL>
<P>
<A NAME="byFunction"><H2>Sorted by Function</H2></A>
<UL>
EndOfByFunction
}

sub printByDLL
{
    print <<"EndOfByDLL";
</UL>
<P>
<A NAME="byDLL"><H2>Sorted by DLL</H2></A>
<UL>
EndOfByDLL
}

sub printDLLName
{
    local($dll, $link) = @_;
    ($link .= substr($dll, 0, 4)) =~ tr/a-z/A-Z/;
    $link = $Dir.$link.substr($dll, 4).'.html';
    print <<"EndOfDLLName";
</UL>
<P>
<A NAME="$dll"><A HREF="$link"><H3>$dll</H3></A></A>
<UL>
EndOfDLLName
}

sub printByModule
{
    print <<"EndOfByModule";
</UL>
<P>
<A NAME="byModule"><H2>Sorted by Module</H2></A>
<UL>
EndOfByModule
}

sub printModuleName
{
    local($module, $dll, $text) = @_;
    $text = "from the <A HREF=\"#$dll\">$dll</A> module\n" if(defined($dll));
    print <<"EndOfModuleName";
</UL>
<P>
<A NAME="$module"><H3>$module</H3></A>
$text
<UL>
EndOfModuleName
}

sub printFooter
{
    print <<"EndOfFooter";
</UL>
This list was generated automatily and may be regenerated at any time. 
Please see <A HREF="../../config/makeindx.pl">makeindx.pl script</A> 
for instructions.
</BODY>
</HTML>
EndOfFooter
}

sub readModules
{
    local($check, $line) = @_;

    # get a list of the modules
    if (-d $check) {
	open(CHECK, "ls -1 $check/*.html |")
	    || die "Can open ls -1 $check/*.html |: $!\n";
	$Dir = $check;
	$Dir .= '/' if(!($Dir =~ /\/$/));
    } else {
	open(CHECK, $check) || die "Can open $check: $!\n";
	$Dir = $check =~ /(.*\/)\w+\.html$/;
    }

    # put this list into %Modules and @ModuleList
    while ($line = <CHECK>) {
	chop($line);
	$line =~ s/.*\/(\w+\.html)/$1/;  # get name from path
	next if($line =~ /^WWW/);
	next if($line eq 'sysdep.html');
	$Modules{$line} .= ' 1st';       # Define $Modules{$module}
	push(@ModuleList, $line);        # add to ModuleList
    }
}

&readModules(shift(@ARGV)); # step 1

# 2. find out in what DLLs these modules reside (eg HTReqMan is in wwwcore)
if (@ARGV[0] =~ /@(.*)/) {
    local($name) = $1;
    if ($name =~ /\w+/) {
	if (!open(LIST, $name)) {
	    die "Can't open \"$1\", $!";
	}
	while (<LIST>) {
	    chop;
	    &getDLLs($_);
	}
    } else {
	while (<STDIN>) {
	    chop;
	    &getDLLs($_);
	}
    }
} else {
    foreach $source (@ARGV) {
	&getDLLs($source);
    }
}
&printHeader;
@ModuleList = sort @ModuleList;
@DLLList = sort @DLLList;

# 3. scan for funtions in these modules
foreach $file (@ModuleList) {
    &findFunctions($Dir.$file);
}
@FunctionList = sort @FunctionList;

# 4. output sorted by function name
&printByFunction;
foreach $func (@FunctionList) {
    local($source, $nameTag) = split('#', $Functions{$func});
    undef($nameTag) if ($nameTag eq '');
    local($dll) = $DLLS{$source};
    &numberEach($source, $func, $nameTag, $dll);
}

# 5. output sorted by DLL name
&printByDLL;
foreach $checkDll (@DLLList) {
    printDLLName($checkDll);
    foreach $func (@FunctionList) {
	local($source, $nameTag) = split('#', $Functions{$func});
	undef($nameTag) if ($nameTag eq '');
	local($dll) = $DLLS{$source};
	next if($dll ne $checkDll);
	&numberEach($source, $func, $nameTag);
    }
}

# 6. output sorted by module name
&printByModule;
foreach $module (@ModuleList) {
    local($dll) = $DLLS{$module};
    printModuleName($module, $dll);
    foreach $func (@FunctionList) {
	local($source, $nameTag) = split('#', $Functions{$func});
	undef($nameTag) if ($nameTag eq '');
	next if($source ne $module);
	&numberEach($source, $func, $nameTag);
    }
}
&printFooter;
