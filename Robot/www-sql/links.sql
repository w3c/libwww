<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<HTML>
 <HEAD>
  <TITLE>The Webbot Log Database - Link Types</TITLE>
  <LINK rel=STYLESHEET href="http://www.w3.org/StyleSheets/Area/Overview.css" type="text/css">
 </HEAD>
<A HREF="http://www.w3.org/"><IMG ALT="W3C" SRC="http://www.w3.org/Icons/WWW/w3c_home" BORDER="0" WIDTH="72"
    HEIGHT="48"></A>
<A HREF="http://www.w3.org/Robot/"><IMG ALT="Robot" SRC="http://www.w3.org/Icons/WWW/robot48x" BORDER="0"></A>
<A HREF="./">Top</A>
<H1>
  The Webbot Log Database - Link Types
</H1>
<P>
<H2>
  <A NAME="hints">Hints</A>
</H2>

These are some useful hints for handling wildcard matching:

<DL COMPACT>
<DT><B>%</B><DD>Matches any number of characters, even zero characters. 
<DT><B>_</B><DD>Matches exactly one character. 
<DT><B>\%</B><DD>Matches one %. 
<DT><B>\_</B><DD>Matches one _. 
</DL>

<! SQL connect >
<! SQL database webbot >
<! SQL set root "http://www.w3.org">

<H2>
  <A NAME="t1">Link Relationships (Typed links)</A>
</H2>

Type in the link relationship that you are interested in, for example
<B>stylesheet</B>, <B>image</B>, <B>referer</B>, <B>up</B>,
<B>TOC</B>, etc. Case doesn't matter and you can use the <A
href="#hints">wildcards above</A>.<P>

<! SQL setdefault o1 0 >
<form action="links.sql#t1">
  <input name="link" size=20 value="<! sql print "?link">">
  <input type="submit">
</form>
<P>
<! SQL query "select links.link_type, u1.uri, u2.uri from links, uris as u1, uris as u2 where links.link_type like '?link' and links.source=u1.id and links.destination=u2.id limit $o1,20" q1 >

<!-- Put in table -->
<table width="90%">
<tr> <th>Source</th> <th>Link Type</th> <th>Destination</th> </tr>
<! sql print_rows q1 "<tr> <td><a href=\"$root@q1.1\">@q1.1</a></td> </td> <td>@q1.0</td> <td><a href=\"$root@q1.2\">@q1.2</a></tr>\n" >
</table>

<!-- Put in navigation links -->
<! sql if $NUM_ROWS = 0 >
No match found.
<! sql else >
<center>
<! sql if 19 < $o1 >
<! sql print "<a href=\"links.sql\?link=#link&o1=" >
<! sql eval $o1 - 20 >
<! sql print "\#t1\">Prev</a>" >
<! sql else >
Prev
<! sql endif >
<! sql if $NUM_ROWS = 20 >
<! sql print "<a href=\"links.sql\?link=#link&o1=" >
<! sql eval $o1 + 20 >
<! sql print "\#t1\">Next</a>" >
<! sql else >
Next
<! sql endif >
</center>
<! sql endif >

<H2>
  <A NAME="out">Where does this Address link to?</A>
</H2>

Paste in the URI that you want to find out the <B>outgoing</B> links from this address. Make the URI relative to <CODE>http://www.w3.org</CODE> so that you have something like <CODE>/Library/</CODE>. You can use the <A href="#hints">wildcards above</A>, like for example <CODE>/Library/%</CODE>.<P>

<! SQL setdefault o2 0 >
<form action="links.sql#out">
  <input name="out" size=60 value="<! sql print "?out">">
  <input type="submit">
</form>

<! SQL query "select links.link_type, u2.uri, u1.uri from links, uris as u1, uris as u2 where u2.uri like '?out' and links.source=u2.id and links.destination=u1.id limit $o2,20" q1 > 

<!-- Put in table -->
<table width="90%">
<tr> <th>Source</th> <th>Link Type</th> <th>Destination</th> </tr>
<! sql print_rows q1 "<tr> <td><a href=\"$root@q1.1\">@q1.1</a></td> </td> <td>@q1.0</td> <td><a href=\"$root@q1.2\">@q1.2</a></tr>\n" >
</table>

<!-- Put in navigation links -->
<! sql if $NUM_ROWS = 0 >
No match found.
<! sql else >
<center>
<! sql if 19 < $o2 >
<! sql print "<a href=\"links.sql\?out=#out&o2=" >
<! sql eval $o2 - 20 >
<! sql print "\#out\">Prev</a>" >
<! sql else >
Prev
<! sql endif >
<! sql if $NUM_ROWS = 20 >
<! sql print "<a href=\"links.sql\?out=#out&o2=" >
<! sql eval $o2 + 20 >
<! sql print "\#out\">Next</a>" >
<! sql else >
Next
<! sql endif >
</center>
<! sql endif >

<H2>
  <A NAME="in">Who links to this Address?</A>
</H2>

Paste in the URI that you want to find out the <B>incoming</B> links to this address. Make the URI relative to <CODE>http://www.w3.org</CODE> so that you have something like <CODE>/Library/</CODE>. You can use the <A href="#hints">wildcards above</A>, like for example <CODE>/Library/%</CODE>.<P>

<! SQL setdefault o3 0 >
<form action="links.sql#in">
  <input name="in" size=60 value="<! sql print "?in">">
  <input type="submit">
</form>

<! SQL query "select links.link_type, u1.uri, u2.uri from links, uris as u1, uris as u2 where u2.uri like '?in' and links.source=u1.id and links.destination=u2.id limit $o3,20" q1 > 

<!-- Put in table -->
<table width="90%">
<tr> <th>Source</th> <th>Link Type</th> <th>Destination</th> </tr>
<! sql print_rows q1 "<tr> <td><a href=\"$root@q1.1\">@q1.1</a></td> </td> <td>@q1.0</td> <td><a href=\"$root@q1.2\">@q1.2</a></tr>\n" >
</table>

<!-- Put in navigation links -->
<! sql if $NUM_ROWS = 0 >
No match found.
<! sql else >
<center>
<! sql if 19 < $o3 >
<! sql print "<a href=\"links.sql\?in=#in&o3=" >
<! sql eval $o3 - 20 >
<! sql print "\#in\">Prev</a>" >
<! sql else >
Prev
<! sql endif >
<! sql if $NUM_ROWS = 20 >
<! sql print "<a href=\"links.sql\?in=#in&o3=" >
<! sql eval $o3 + 20 >
<! sql print "\#in\">Next</a>" >
<! sql else >
Next
<! sql endif >
</center>
<! sql endif >

<P>
<HR>
<ADDRESS>
  <A href="http://www.w3.org/People/Frystyk/">Henrik Frystyk Nielsen</a><br>
  @(#) $Id$
</ADDRESS>
 </BODY>
</HTML>
