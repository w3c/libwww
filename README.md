# Libwww - the W3C Protocol Library

Libwww is a highly modular, general-purpose client side Web API
written in C for Unix and Windows (Win32). It's well suited for both
small and large applications, like browser/editors, robots, batch
tools, etc. Pluggable modules provided with libwww include complete
HTTP/1.1 (with caching, pipelining, PUT, POST, Digest Authentication,
deflate, etc), MySQL logging, FTP, HTML/4, XML (expat), RDF (SiRPAC),
WebDAV, and much more. The purpose of libwww is to serve as a testbed
for protocol experiments.

This is a complete mirror of the W3C libwww repository.

For more info, see http://www.w3.org/Library/

## How to build libwww

First, to get the expat sources (which libwww depends on) run this command:

    git submodule update --init

After that has completed, you can build the libwww sources by following the
steps in the INSTALL.html file.
