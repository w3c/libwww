/*			FTP access functions		HTFTP.h
**			====================
*/

/*	Retrieve File from Server
**	-------------------------
**
** On exit,
**	returns		Socket number for file if good.
**			<0 if bad.
*/
extern int HTFTP_open_file_read
PARAMS
((
  CONST char * name,
  HTParentAnchor * anchor
));

/*	Close socket opened for reading a file, and get final message
**	-------------------------------------------------------------
**
*/
extern int HTFTP_close_file
PARAMS ((int soc));


/*	Return Host Name
**	----------------
*/
extern CONST char * HTHostName
NOPARAMS;
