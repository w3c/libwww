<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<HTML>
 <HEAD>
  <TITLE>The Webbot Log Database - Resources</TITLE>
  <LINK rel=STYLESHEET href="http://www.w3.org/StyleSheets/Area/Overview.css" type="text/css">
 </HEAD>
<A HREF="http://www.w3.org/"><IMG ALT="W3C" SRC="http://www.w3.org/Icons/WWW/w3c_home" BORDER="0" WIDTH="72"
    HEIGHT="48"></A>
<A HREF="http://www.w3.org/Robot/"><IMG ALT="Robot" SRC="http://www.w3.org/Icons/WWW/robot48x" BORDER="0"></A>
<A HREF="./">Top</A>

<H1>
  The Webbot Log Database - Resources
</H1>

Here you can search for information about our documents including titles, last modified dates, etc.

<! SQL connect >
<! SQL database webbot >
<! SQL set root "http://www.w3.org">

<H2>
  <A NAME="title">Search for Document Titles</A>
</H2>

Type in parts of the title that you are interestes in. Case doesn't
matter.<P>

<! SQL setdefault o1 0 >
<form action=resources.sql#title>
  <input name=title size=60 value="<! sql print "?title">">
  <input type=submit><br>
</form>

<! SQL query "select uris.uri, resources.title, resources.content_language, resources.charset, resources.content_type, resources.content_encoding, resources.content_language, resources.last_modified from resources, uris where resources.title like '%?title%' and uris.id=resources.uri order by resources.last_modified DESC limit $o1,20" q1 >

<!-- Put in table -->
<table width="100%">
<tr> <th width="50%">Title</th> <th>Language</th> <th>Charset</th> <th>Content Type</th> <th>Content Encoding</th> <th>Content Language</th> <th>Last Modified</th> </tr>
<! sql print_rows q1 "<tr> <td><a href=\"$root@q1.0\">@q1.1</a></td> <td>@q1.2</td> <td>@q1.3</td> <td>@q1.4</td> <td>@q1.5</td> <td>@q1.6</td> <td>@q1.7</td> </tr>\n" >
</table>

<!-- Put in navigation links -->
<! sql if $NUM_ROWS = 0 >
No match found.
<! sql else >
<center>
<! sql if 19 < $o1 >
<! sql print "<a href=\"resources.sql\?title=#title&o1=" >
<! sql eval $o1 - 20 >
<! sql print "\#title\">Prev</a>" >
<! sql else >
Prev
<! sql endif >
<! sql if $NUM_ROWS = 20 >
<! sql print "<a href=\"resources.sql\?title=#title&o1=" >
<! sql eval $o1 + 20 >
<! sql print "\#title\">Next</a>" >
<! sql else >
Next
<! sql endif >
</center>
<! sql endif >

<H2>
  <A NAME="alt">Search for Image ALT Text
</H2>

Type in parts of the ALT text you are interestes in. Case doesn't
matter.<P>

<! SQL setdefault o2 0 >
<form action=resources.sql#alt>
  <input name=alt size=60 value="<! sql print "?alt">">
  <input type=submit><br>
</form>
<! SQL query "select links.comment, u1.uri, u2.uri from links, uris as u1, uris as u2 where links.link_type='image' and links.comment like '%?alt%' and links.source=u1.id and links.destination=u2.id limit $o2,20" q1 >

<!-- Put in table -->
<table width="100%">
<tr> <th width="50%">Alt Text</th> <th>Image Location</th> </tr>
<! sql print_rows q1 "<tr> <td><a href=\"$root@q1.1\">@q1.0</a></td> <td><a href=\"$root@q1.2\">@q1.2</a></td> </tr>\n" >
</table>

<!-- Put in navigation links -->
<! sql if $NUM_ROWS = 0 >
No match found.
<! sql else >
<center>
<! sql if 19 < $o2 >
<! sql print "<a href=\"resources.sql\?alt=#alt&o2=" >
<! sql eval $o2 - 20 >
<! sql print "\#alt\">Prev</a>" >
<! sql else >
Prev
<! sql endif >
<! sql if $NUM_ROWS = 20 >
<! sql print "<a href=\"resources.sql\?alt=#alt&o2=" >
<! sql eval $o2 + 20 >
<! sql print "\#alt\">Next</a>" >
<! sql else >
Next
<! sql endif >
</center>
<! sql endif >

<H2>
  <A NAME="lm">Which Documents have Changed Since...</A>
</H2>

Find all the documents that have changed after acertain date/time. Type in the date and time using the following format: <B>YYYY-MM-DD HH:MM:SS</B>, for example <B>1990-05-01 12:00:00</B>.

<! SQL setdefault o3 0 >
<! SQL setdefault lm "2000-01-01 12:00:00" >
<form action=resources.sql#lm>
  <input name=lm size=60 value="<! sql print "?lm">">
  <input type=submit><br>
</form>
<! SQL query "select uris.uri, resources.content_language, resources.charset, resources.content_type, resources.content_encoding, resources.last_modified from resources, uris where resources.last_modified > '$lm' and uris.id=resources.uri order by resources.last_modified DESC limit $o3,20" q1 >

<!-- Put in table -->
<table width="100%">
<tr> <th WIDTH="50%">Address</th> <th>Language</th> <th>Charset</th> <th>Content Type</th> <th>Content Encoding</th> <th>Last Modified</th> </tr>
<! sql print_rows q1 "<tr> <td><a href=\"$root@q1.0\">@q1.0</a></td> <td>@q1.1</td> <td>@q1.2</td> <td>@q1.3</td><td>@q1.4</td> <td>@q1.5</td> </tr>\n" >
</table>

<!-- Put in navigation links -->
<! sql if $NUM_ROWS = 0 >
No match found.
<! sql else >
<center>
<! sql if 19 < $o3 >
<! sql print "<a href=\"resources.sql\?lm=#lm&o3=" >
<! sql eval $o3 - 20 >
<! sql print "\#lm\">Prev</a>" >
<! sql else >
Prev
<! sql endif >
<! sql if $NUM_ROWS = 20 >
<! sql print "<a href=\"resources.sql\?lm=#lm&o3=" >
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
