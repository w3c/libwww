#!/usr/local/bin/perl -ni

if (!/^PUBLIC\s/) {
    print;
    next;
}
if (!/(\w*)\W*\(/) {
    warn "assuming global: ", $_;
    next;
}
local($func) = $1;
s/$func/EXPORT $func/;
s/\*EXPORT/\* EXPORT/;
print;

