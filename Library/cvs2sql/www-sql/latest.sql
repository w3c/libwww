<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<HTML>
 <HEAD>
  <TITLE>
     Changes Committed to the libwww CVS Source Tree
  </TITLE>
  <LINK rel=STYLESHEET href="http://www.w3.org/StyleSheets/Area/Overview.css" type="text/css">
 </HEAD>
<A HREF="http://www.w3.org/"><IMG ALT="W3C" SRC="http://www.w3.org/Icons/WWW/w3c_home" BORDER="0" WIDTH="72"
    HEIGHT="48"></A>
<A HREF="http://www.w3.org/Robot/"><IMG ALT="libwww" SRC="http://www.w3.org/Icons/WWW/Cogs48x" BORDER="0"></A>
<A HREF="./">Top</A>

<H1>
  Changes Committed to the libwww CVS Source Tree
</H1>

<! SQL set root "http://dev.w3.org/cgi-bin/cvsweb">
<! SQL connect >
<! SQL database cvsloglibwww >

Find all the documents that have changed after a certain
date/time.

<! SQL setdefault o3 0 >
<! SQL setdefault hours 0 >
<! SQL setdefault days 0 >
<! SQL setdefault offset 0 >

<form action=latest.sql#lm>
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
</TABLE>
<input type=submit>
</form>

<! SQL if  0 < $days >
<! SQL setexpr offset $days*3600*24 >
<! SQL else >
<! SQL setexpr offset $hours*3600 >
<! SQL endif >

<HR>
<! SQL query "select trim('/sources/public' from locations.location), users.username, logs.date, logs.operation, logs.comment from locations, users, logs where logs.date > from_unixtime(unix_timestamp(now())-$offset) and locations.id=logs.location and users.id=logs.username order by logs.date DESC limit $o3,20" q1 >

<!-- Put in table -->
<table width="100%">
<tr> <th>Location</th> <th>User</th> <th>Last Check-in Time</th> <th>Operation</th> <th>Comment</th></tr>
<! sql print_rows q1 "<tr> <td><a href=\"$root@q1.0\">@q1.0</a></td> <td>@q1.1</td> <td>@q1.2</td> <td>@q1.3</td><td>@q1.4</td></tr>\n" >
</table>

<!-- Put in navigation links -->
<! sql if $NUM_ROWS = 0 >
No match found.
<! sql else >
<center>
<! sql if 19 < $o3 >
<! sql print "<a href=\"latest.sql\?lm=#lm&o3=" >
<! sql eval $o3 - 20 >
<! sql print "\#lm\">Prev</a>" >
<! sql else >
Prev
<! sql endif >
<! sql if $NUM_ROWS = 20 >
<! sql print "<a href=\"latest.sql\?lm=#lm&o3=" >
<! sql eval $o3 + 20 >
<! sql print "\#lm\">Next</a>" >
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
