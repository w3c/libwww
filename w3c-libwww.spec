# Note that this is NOT a relocatable package
# @(#) $Id$
# Author: Jim Gettys

%define ver	5.2.8
%define rel	1
%define prefix	/usr/

Summary: Web client library of common code
Name: w3c-libwww
Version: %ver
Release: %rel
Copyright: W3C (see: http://www.w3.org/Consortium/Legal/copyright-software.html)
Group: Libraries
Source: http://www.w3.org/Library/Distribution/w3c-libwww-%{ver}.tar.gz
BuildRoot: /var/tmp/w3c-libwww-%{PACKAGE_VERSION}-root
Distribution: W3C
Vendor: W3C - World Wide Web Consortium
Packager: Jim Gettys <jg@w3.org>
URL: http://www.w3.org/Library
Docdir: %{prefix}/doc/

Icon: Lib48x.gif

%description

Libwww is a general-purpose client side Web API written in C for Unix
and Windows (Win32). With a highly extensible and layered API, it can
accommodate many different types of client applications including
clients, robots, etc.  The purpose of libwww is to provide a highly
optimized HTTP sampInternet protocols and to serve as a testbed for
protocol experiments. It is used by a number of major software
packages (e.g. Lynx), and its HTTP/1.1 implementation is a high quality,
optimized, production quality implemetation.

   Author: Henrik Frystyk Nielsen http://www.w3.org/People/frystyk
   Packager: Jim Gettys http://www.w3.org/People/Gettys

%package devel
Summary: Libwww web library and library of support code
Group: Libraries
Requires: w3c-libwww

%description devel
Static libraries and header files for libwww, which are available as public
libraries.

%package apps
Summary: Applications built using Libwww web library: e.g. Robot, command line tool, etc.
Group: Applications/Networking
Requires: w3c-libwww
Icon: robot48x.gif

%description apps

Web applications built using Libwww: Robot, Command line tool, 
line mode browser.  The Robot can crawl web sites faster, and
with lower load, than any other web walker that we know of, 
due to its extensive pipelining and use of HTTP/1.1.

The command line tool (w3c) is very useful for manipulation of 
Web sites that implement more than just HTTP GET (e.g. PUT, 
 POST, etc.).

The line mode browser is a minimal line mode web browser; 
often useful to convert to ascii text.  Currently unavailable
until someone updates it to some new interfaces. (hint, hint...)

%prep
%setup
%build
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=/usr --with-regex --with-zlib

make RPM_OPT_FLAGS="$RPM_OPT_FLAGS"

%install
rm -rf $RPM_BUILD_ROOT

make prefix=$RPM_BUILD_ROOT%{prefix} install

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-, root, root)


%{prefix}/lib/libwww*.so.*
%{prefix}/lib/libxml*.so.*
%{prefix}/lib/libmd5.so.*

%doc *.html */*.html */*/*.html Icons/*/*.gif

%files apps
%defattr(-, root, root)
%dir %{prefix}/bin/*


%files devel
%defattr(-, root, root)
%dir %{prefix}/include/w3c-libwww/*.h
%{prefix}/lib/libwww*.so
%{prefix}/lib/libxml*.so
%{prefix}/lib/lib*a



