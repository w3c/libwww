<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<HTML>
 <HEAD>
  <TITLE>Latest libwww Changes Committed to CVS</TITLE>
  <LINK rel=STYLESHEET href="http://www.w3.org/StyleSheets/Area/Overview.css" type="text/css">
 </HEAD>
<BODY TEXT="#000000" BGCOLOR="#FFFFFF" LINK="#0000EE" VLINK="#551A8B">
  <A HREF="http://www.w3.org/"><IMG ALT="W3C" SRC="http://www.w3.org/Icons/WWW/w3c_home" BORDER="0" WIDTH="72" HEIGHT="48"></A>
  <A HREF="http://www.w3.org/Library/"><IMG ALT="libwww" SRC="http://www.w3.org/Icons/WWW/Cogs48x" BORDER="0"></A>
  <A HREF="./">CVS Changes</A>

<H1>
  Latest libwww Changes Committed to CVS
</H1>

Here you can find all the <A
HREF="http://www.w3.org/Library/">libwww</A> code changes after a
certain date/time. If you haven't already then <A
HREF="http://www.w3.org/Library/cvs.html">get the libwww code base
directly from CVS</A>. You can also browse the <A
HREF="http://dev.w3.org/cgi-bin/cvsweb/">online libwww CVS
repository</A> directly - this may be easier if you are looking for
changes to a specific file.
<P>
If you already have checked out the code then you can get the latest
changes by doing a <A
HREF="http://www.w3.org/Library/cvs.html#update">CVS update</A> and <A
HREF="http://www.w3.org/Library/cvs.html#Easy">recompile the
code</A>.
<P>
Discussions about libwww should happen on the <A
HREF="http://www.w3.org/Library/#Forums">&lt;www-lib@w3.org&gt;</A>
mailing list.<P>

<! SQL set root "http://dev.w3.org/cgi-bin/cvsweb">
<! SQL connect >
<! SQL database cvsloglibwww >

<! SQL setdefault cnt 0 >
<! SQL setdefault hours 0 >
<! SQL setdefault days 0 >
<! SQL setdefault offset 0 >

<form action=latest.sql#z1>
<TABLE CELLSPACING=0 CELLPADDING=0>
    <TR>
      <TD>
	<INPUT type=radio name=days CHECKED value=0></TD>
      <TD>In the last
	<INPUT type=text name=hours value=1 size=4> hours</TD>
    </TR>
    <TR>
      <TD>
	<INPUT type=radio name=days value=1></TD>
      <TD>In the last day
    </TR>
    <TR>
      <TD>
	<INPUT type=radio name=days value=7></TD>
      <TD>In the last week
    </TR>
    <TR>
      <TD>
	<INPUT type=radio name=days value=14></TD>
      <TD>In the last 2 weeks
    </TR>
    <TR>
      <TD>
	<INPUT type=radio name=days value=28></TD>
      <TD>In the last 4 weeks
    </TR>
    <TR>
      <TD>
	<INPUT type=radio name=days value=56></TD>
      <TD>In the last 8 weeks
    </TR>
</TABLE>
<input type=submit>
</form>

<! SQL if  0 < $days >
<! SQL setexpr offset $days*3600*24 >
<! SQL else >
<! SQL setexpr offset $hours*3600 >
<! SQL endif >

<HR>
<A NAME="z1">
<! SQL query "select trim('/sources/public' from locations.location), users.username, logs.date, logs.operation, comments.comment from locations, users, comments, logs where logs.date > from_unixtime(unix_timestamp(now())-$offset) and locations.id=logs.location and users.id=logs.username and comments.id=logs.comment order by logs.date DESC limit $cnt,10" q1 >

<!-- Put in table -->
<table width="100%">
<tr> <th>Location</th> <th>User</th> <th>Last Checkin&nbsp;Time</th> <th>Operation</th> <th>Comment</th></tr>
<! sql print_rows q1 "<tr> <td><a href=\"$root@q1.0\">@q1.0</a></td> <td>@q1.1</td> <td>@q1.2</td> <td>@q1.3</td><td>@q1.4</td></tr>\n" >
</table>

<!-- Put in navigation links -->
<! sql if $NUM_ROWS = 0 >
No match found.
<! sql else >
<center>
<! sql if 9 < $cnt >
<! sql print "<a href=\"latest.sql\?hours=#hours&days=#days&cnt=" >
<! sql eval $cnt - 10 >
<! sql print "\#z1\">Prev</a>" >
<! sql else >
Prev
<! sql endif >
<! sql if $NUM_ROWS = 10 >
<! sql print "<a href=\"latest.sql\?hours=#hours&days=#days&cnt=" >
<! sql eval $cnt + 10 >
<! sql print "\#z1\">Next</a>" >
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
