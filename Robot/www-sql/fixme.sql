
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<HTML>
 <HEAD>
  <TITLE>The Webbot Log Database - Broken HTML, bad style, etc</TITLE>
  <LINK rel=STYLESHEET href="http://www.w3.org/StyleSheets/Area/Overview.css" type="text/css">
 </HEAD>
<A HREF="http://www.w3.org/"><IMG ALT="W3C" SRC="http://www.w3.org/Icons/WWW/w3c_home" BORDER="0" WIDTH="72"
    HEIGHT="48"></A>
<A HREF="http://www.w3.org/Robot/"><IMG ALT="Robot" SRC="http://www.w3.org/Icons/WWW/robot48x" BORDER="0"></A>
<A HREF="./">Top</A>

<H1>
  The Webbot Log Database - Broken HTML, Bad Style, etc
</H1>

What's wrong on our web site and where to find it :)

<! SQL connect >
<! SQL database webbot >
<! SQL set root "http://www.w3.org">

<H2>
  <A NAME="t1">Search for Images WITH NO ALT Text
</H2>

<! SQL setdefault o1 0 >
<form action=fixme.sql#t1>
  Area:
  <SELECT NAME="a1">
    <OPTION SELECTED><! sql print "?a1">
    <OPTION>Amaya
    <OPTION>Architecture
    <OPTION>ComLine
    <OPTION>DesignIssues
    <OPTION>International
    <OPTION>Jigsaw
    <OPTION>Library
    <OPTION>LineMode
    <OPTION>MarkUp
    <OPTION>Member
    <OPTION>MiniServ
    <OPTION>Protocols
    <OPTION>Replication
    <OPTION>Robot
    <OPTION>Style
    <OPTION>Team
    <OPTION>WAI
  </SELECT>
  <input type=submit>
</FORM>

<! SQL query "select u1.uri, u2.uri from links, uris as u1, uris as u2 where links.link_type='image' and isnull(links.comment) and links.source=u1.id and u1.uri like '/?a1/%' and links.destination=u2.id limit $o1,20" q1 >

<!-- Put in table -->
<table width="100%">
<tr> <th width="50%">Referenced From</th> <th>Image Location</th> </tr>
<! sql print_rows q1 "<tr> <td><a href=\"$root@q1.0\">@q1.0</a></td> <td><a href=\"$root@q1.1\">@q1.1</a></td> </tr>\n" >
</table>

<!-- Put in navigation links -->
<! sql if $NUM_ROWS = 0 >
No match found.
<! sql else >
<center>
<! sql if 19 < $o1 >
<! sql print "<a href=\"fixme.sql\?a1=#a1&o1=" >
<! sql eval $o1 - 20 >
<! sql print "\#t1\">Prev</a>" >
<! sql else >
Prev
<! sql endif >
<! sql if $NUM_ROWS = 20 >
<! sql print "<a href=\"fixme.sql\?a1=#a1&o1=" >
<! sql eval $o1 + 20 >
<! sql print "\#t1\">Next</a>" >
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
