<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<HTML>
 <HEAD>
  <TITLE>The Webbot Log Database - Requests</TITLE>
  <LINK rel=STYLESHEET href="http://www.w3.org/StyleSheets/Area/Overview.css" type="text/css">
 </HEAD>
</HEAD>
<BODY BGCOLOR="#ffffff" TEXT="#000000">
<A HREF="http://www.w3.org/"><IMG ALT="W3C" SRC="http://www.w3.org/Icons/WWW/w3c_home" BORDER="0" WIDTH="72"
    HEIGHT="48"></A>
<A HREF="http://www.w3.org/Robot/"><IMG ALT="Robot" SRC="http://www.w3.org/Icons/WWW/robot48x" BORDER="0"></A>
<A HREF="./">Top</A>
<H1>
  The Webbot Log Database - Requests
</H1>
<P>

<! SQL connect >
<! SQL database webbot >
<! SQL setdefault sort requests.request_time >
<! SQL set root "http://www.w3.org">


<H2>
  <A NAME="t1">Search for Broken Links</A>
</H2>

Select the area that you are interested in.<P>

<! SQL setdefault o1 0 >
<FORM ACTION="requests.sql#t1">
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

<! SQL query "select u1.uri, u2.uri, requests.request_time, requests.method from requests, links, uris as u1, uris as u2 where requests.status=404 and u1.id=requests.uri and links.destination=requests.uri and u2.id=links.source and u1.uri like '/?a1/%' order by u2.uri limit $o1,20" q1 >

<!-- If we have any data -->
<! sql if $NUM_ROWS = 0 >
No match found.
<! sql else >

<!-- Put in table -->
<table width="90%">
<tr><th>This link is Broken</th> <th>It is Referenced from</th> <th>Requested at</th> <th>Method</th> </tr>
<! sql print_rows q1 "<tr><td><a href=\"$root@q1.0\">@q1.0</a></td><td><a href=\"$root@q1.1\">@q1.1</a></td><td>@q1.2</td><td>@q1.3</td></tr>\n" >
</table>

<!-- Put in navigation links -->
<center>
<! sql if 19 < $o1 >
<! sql print "<a href=\"requests.sql\?a1=#a1&o1=" >
<! sql eval $o1 - 20 >
<! sql print "\#t1\">Prev</a>" >
<! sql else >
Prev
<! sql endif >
<! sql if $NUM_ROWS = 20 >
<! sql print "<a href=\"requests.sql\?a1=#a1&o1=" >
<! sql eval $o1 + 20 >
<! sql print "\#t1\">Next</a>" >
<! sql else >
Next
<! sql endif >
</center>
<! sql endif >

<H2>
  <A NAME="t2">Search for Temporary Redirections</A>
</H2>

Select the area that you are interested in.<P>

<! SQL setdefault o2 0 >
<FORM ACTION="requests.sql#t2">
  Area:
  <SELECT NAME="a2">
    <OPTION SELECTED><! sql print "?a2">
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

<! SQL query "select u1.uri, u2.uri, requests.request_time, requests.method from requests, links, uris as u1, uris as u2 where requests.status=302 and u1.id=requests.uri and links.destination=requests.uri and u2.id=links.source and u1.uri like '/?a2/%' order by u2.uri limit $o2,20" q2 >

<!-- If we have any data -->
<! sql if $NUM_ROWS = 0 >
No match found.
<! sql else >

<!-- Put in table -->
<table width="90%">
<tr><th>Temporary Redirection</th> <th>It is Referenced from</th> <th>Requested at</th> <th>Method</th> </tr>
<! sql print_rows q2 "<tr><td><a href=\"$root@q2.0\">@q2.0</a></td><td><a href=\"$root@q2.1\">@q2.1</a></td><td>@q2.2</td><td>@q2.3</td></tr>\n" >
</table>

<!-- Put in navigation links -->
<center>
<! sql if 19 < $o2 >
<! sql print "<a href=\"requests.sql\?a2=#a2&o2=" >
<! sql eval $o2 - 20 >
<! sql print "\#t2\">Prev</a>" >
<! sql else >
Prev
<! sql endif >
<! sql if $NUM_ROWS = 20 >
<! sql print "<a href=\"requests.sql\?a2=#a2&o2=" >
<! sql eval $o2 + 20 >
<! sql print "\#t2\">Next</a>" >
<! sql else >
Next
<! sql endif >
</center>
<! sql endif >

<H2>
  <A NAME="t3">Search for Permanent Redirections</A>
</H2>

Select the area that you are interested in.<P>

<! SQL setdefault o3 0 >
<FORM ACTION="requests.sql#t3">
  Area:
  <SELECT NAME="a3">
    <OPTION SELECTED><! sql print "?a3">
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

<! SQL query "select u1.uri, u2.uri, requests.request_time, requests.method from requests, links, uris as u1, uris as u2 where requests.status=301 and u1.id=requests.uri and links.destination=requests.uri and u2.id=links.source and u1.uri like '/?a3/%' order by u2.uri limit $o3,20" q3 >

<!-- If we have any data -->
<! sql if $NUM_ROWS = 0 >
No match found.
<! sql else >

<!-- Put in table -->
<table width="90%">
<tr><th>Permananet Redirection</th> <th>It is Referenced from</th> <th>Requested at</th> <th>Method</th> </tr>
<! sql print_rows q3 "<tr><td><a href=\"$root@q3.0\">@q3.0</a></td><td><a href=\"$root@q3.1\">@q3.1</a></td><td>@q3.2</td><td>@q3.3</td></tr>\n" >
</table>

<!-- Put in navigation links -->
<center>
<! sql if 19 < $o3 >
<! sql print "<a href=\"requests.sql\?a3=#a3&o3=" >
<! sql eval $o3 - 20 >
<! sql print "\#t3\">Prev</a>" >
<! sql else >
Prev
<! sql endif >
<! sql if $NUM_ROWS = 20 >
<! sql print "<a href=\"requests.sql\?a3=#a3&o3=" >
<! sql eval $o3 + 20 >
<! sql print "\#t3\">Next</a>" >
<! sql else >
Next
<! sql endif >
</center>
<! sql endif >

<H2>
  <A NAME="t4">Search for any Status Code, you like</A>
</H2>

Select the area that you are interested in.<P>

<! SQL setdefault o4 0 >
<! SQL setdefault status 0 >
<FORM ACTION="requests.sql#t4">
  Area:
  <SELECT NAME="a4">
    <OPTION SELECTED><! sql print "?a4">
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
  with status code
  <input name="status" size=3 value="<! sql print "?status">">
  <input type=submit>
</FORM>

<! SQL query "select u1.uri, u2.uri, requests.request_time, requests.method from requests, links, uris as u1, uris as u2 where requests.status=?status and u1.id=requests.uri and links.destination=requests.uri and u2.id=links.source and u1.uri like '/?a4/%' order by u2.uri limit $o4,20" q4 >

<!-- If we have any data -->
<! sql if $NUM_ROWS = 0 >
No match found.
<! sql else >

<!-- Put in table -->
<table width="90%">
<tr><th>Got <! sql print "#status" > When Requesting This Address</th> <th>It is Referenced from</th> <th>Requested at</th> <th>Method</th> </tr>
<! sql print_rows q4 "<tr><td><a href=\"$root@q4.0\">@q4.0</a></td><td><a href=\"$root@q4.1\">@q4.1</a></td><td>@q4.2</td><td>@q4.3</td></tr>\n" >
</table>

<!-- Put in navigation links -->
<center>
<! sql if 19 < $o4 >
<! sql print "<a href=\"requests.sql\?a4=#a4&o4=" >
<! sql eval $o4 - 20 >
<! sql print "\#t4\">Prev</a>" >
<! sql else >
Prev
<! sql endif >
<! sql if $NUM_ROWS = 20 >
<! sql print "<a href=\"requests.sql\?a4=#a4&o4=" >
<! sql eval $o4 + 20 >
<! sql print "\#t4\">Next</a>" >
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
