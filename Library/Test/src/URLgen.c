#include<stdio.h>
#include<stdlib.h>
#include"tcl.h"
#include"WWWLib.h"

/* Additional function to create random urls according to rfc1738 */
/* Also, try to make it, such that it returns the component parts. */

#define lowalpha "abcdefghijklmnopqrstuvwxyz"
#define highalpha "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define alpha "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define digit "0123456789"
#define alphadigit "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
#define safe "$-_.+"
#define extra "!*'(),"
#define national "{}|\\^~[]`"
#define punctuation "<>#%\""
#define hex "0123456789ABCDEFabcdef"
#define reserved ";/?:@&="

char *unreserved;
int unreserved_length = 73;

int HTRandom(int);

void init_strings() {
  unreserved = malloc(73);
  StrAllocCat(unreserved, alpha);
  StrAllocCat(unreserved, digit);
  StrAllocCat(unreserved, safe);
  StrAllocCat(unreserved, extra);
}

char *generate_char(char *str) {
  
  int index = HTRandom((int) strlen(str));
  char *result = malloc(sizeof(str[index]));
  sprintf(result, "%c", str[index]);
  return (result);
}

char *generate_escape() {

  char *escape=malloc(3);
  StrAllocCat(escape, "%");
  if (HTRandom(1)) {
     StrAllocCat(escape, generate_char("0123456789ABCDEF"));
     StrAllocCat(escape, generate_char("0123456789ABCDEF"));
  } else {
     StrAllocCat(escape, generate_char("0123456789abcdef"));
     StrAllocCat(escape, generate_char("0123456789abcdef"));
  }	    
  return (escape);
}

char *generate_unreserved() {
  
  int index = HTRandom((int) strlen(unreserved));
  char *result = malloc(sizeof(unreserved[index]));
  sprintf(result, "%c", unreserved[index]);
  return (result);
}

char *generate_uchar(char *addition) {
  
  char *result = malloc(3);
  int len = (int) strlen(addition);
  char *str = malloc(unreserved_length+len);
  int total = unreserved_length + len + 50;
  int index = HTRandom(total);

  if (index > (unreserved_length+len)) {
    result = generate_escape();
  } else {
    StrAllocCat(str, unreserved);
    StrAllocCat(str, addition);
    sprintf(result, "%c", str[index]);
  }
  return (result);
}

char *generate_xchar() {
  
  char *result = malloc(3);
  char *str = malloc(unreserved_length+7);
  int total = unreserved_length + 7 + 50;
  int index = HTRandom(total);

  if (index > (unreserved_length+7)) {
    result = generate_escape();
  } else {
    StrAllocCat(str, unreserved);
    StrAllocCat(str, reserved);
    sprintf(result, "%c", str[index]);
  }
  return (result);
}

char *add_item(int size, char *start, char *str) {
 
  char *search = malloc(size);
  int length = HTRandom(size);
  int i;
  StrAllocCat(search, start);
  if (!length) length = 1;
  for (i = 1; i <= length; i++) {
     StrAllocCat(search, generate_char(str));
  }
  return(search);
}

char *add_uitem(int size, char *start, char *str) {
 
  char *search = malloc(size);
  int length = HTRandom(size);
  int i;
  StrAllocCat(search, start);
  /*  if (!length) length = 1;*/
  for (i = 1; i <= length; i++) {
     StrAllocCat(search, generate_uchar(str));
  }
  return(search);
}

char *add_xitem(int size, char *start) {
 
  char *search = malloc(size);
  int length = HTRandom(size);
  int i;
  StrAllocCat(search, start);
  if (!length) length = 1;
  for (i = 1; i <= length; i++) {
     StrAllocCat(search, generate_xchar());
  }
  return(search);
}

char *generate_scheme() {
  char *gen_str = malloc(sizeof(lowalpha)+13);
 
  StrAllocCat(gen_str, lowalpha);
  StrAllocCat(gen_str, digit);
  StrAllocCat(gen_str, "+-.");

  return(add_item(10, "", gen_str));
}

char *generate_host() {

  char *host = malloc(91);
  int i, j, length;
  int num = HTRandom(5);

  /*Domain label, alphadigit | alphadigit *[ alphadigit } "-" ] alphadigit */
  
  for (i = 1; i < num; i++) {
    if (HTRandom(1)) StrAllocCat(host, generate_char(alphadigit));
    else {
      StrAllocCat(host, generate_char(alphadigit));
      length = HTRandom(13);
      for (j = 1; j <= length; j++) {
	if (HTRandom(10) > 9) StrAllocCat(host, "-");
	else StrAllocCat(host, generate_char(alphadigit));
      }
      StrAllocCat(host, generate_char(alphadigit));
      StrAllocCat(host, ".");
    }
  }

  /* Top Label, alpha | alpha *[ alphadigit | "-" ] alphadigit */

  if (HTRandom(1)) StrAllocCat(host, generate_char(alpha));
  else {
    StrAllocCat(host, generate_char(alpha));
    length = HTRandom(13);
    for (j = 1; j <= length; j++) {
      if (HTRandom(10) > 9) StrAllocCat(host, "-");
      else StrAllocCat(host, generate_char(alphadigit));
    }
    StrAllocCat(host, generate_char(alphadigit));
  }
  return(host);
}

char *generate_hostport() {

  char *hostport = malloc(100);
  char *nums = malloc(sizeof("255.255.255.255"));
  
  /*for address */
  if (HTRandom(3)) {
    StrAllocCat(hostport, generate_host());
  }
  /* for actual ip numbers */
  else {
    sprintf(nums, "%d.%d.%d.%d",HTRandom(255),HTRandom(255),HTRandom(255),HTRandom(255));
    StrAllocCat(hostport, nums);
  }

  /* For port num */
  if (HTRandom(1)) {
    sprintf(nums, ":%d",HTRandom(9999));
    StrAllocCat(hostport, nums);
  }
  return(hostport);
}

char *generate_login() {

  char *login = malloc(150);
  /*  char *port = malloc(sizeof("9999"));*/
  /*  int parts = HTRandom(4);*/

  /* user and password */
  if (HTRandom(1)) {
    StrAllocCat(login, add_uitem(15,"",";?&="));
    if (HTRandom(1)) {
      StrAllocCat(login, add_uitem(15,":",";?&="));
    }
    StrAllocCat(login, "@");
  }
  StrAllocCat(login, generate_hostport());
  return(login);
}

int generate_login_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 1) {
    Tcl_AppendResult(interp, generate_login(), NULL);      
    return TCL_OK;
  }
  return TCL_ERROR;
}

char *generate_urlpath(int size) {

  /*  char *path = malloc(size);*/
  return(add_xitem(HTRandom(size),""));
}

char *ftpURL() {

  char *url = malloc(200);
  int number = HTRandom(8);
  int i;
 
  StrAllocCat(url, "ftp://");
  StrAllocCat(url, generate_login());

  /* fpath */
  if (HTRandom(1)) {
    if (!number) number = 1;
    for (i = 1; i <= number; i++) {
      StrAllocCat(url, "/");
      StrAllocCat(url, generate_uchar("?:@&="));
    }
    /* ftptype */
    if (HTRandom(5) > 4) {
      StrAllocCat(url, ";type=");
      StrAllocCat(url, generate_char("AIDaid"));
    }
  }
  return(url);
}

char *httpURL() {

  char *url = malloc(200);
  int number = HTRandom(8);
  int i;
 
  StrAllocCat(url, "http://");
  StrAllocCat(url, generate_hostport());
  /* hpath */
  if (HTRandom(1)) {
    if (!number) number = 1;
    for (i = 1; i <= number; i++) {
      StrAllocCat(url, "/");
      StrAllocCat(url, generate_uchar(";:@&="));
    }
    /* search */
    if (HTRandom(5) > 4) {
      StrAllocCat(url, "?");
      StrAllocCat(url, generate_uchar(";:@&="));
    }
  }
  return(url);
}

char *gopherURL() {
  char *url = malloc(200);

  StrAllocCat(url, "gopher://");
  StrAllocCat(url, generate_hostport());
  if (HTRandom(1)) {
    StrAllocCat(url, "/");
    if (HTRandom(2)) {
      /* gtype */
      StrAllocCat(url, generate_xchar());
      if (HTRandom(2)) {
	/* selector */
	StrAllocCat(url, add_xitem(15,""));
	if (HTRandom(1)) {
	  StrAllocCat(url, "%09");
	  if (HTRandom(5) > 4) {
	    StrAllocCat(url, "?");
	    StrAllocCat(url, generate_uchar(";:@&="));
	    if (HTRandom(1)) {
	      StrAllocCat(url, "%09");
	      StrAllocCat(url, add_xitem(15,""));
	    }
	  }
	}
      }
    }
  }
  return(url);
}

char *telnetURL() {

  char *url = malloc(200);
 
  StrAllocCat(url, "telnet://");
  StrAllocCat(url, generate_login());
  StrAllocCat(url, "/");
  return(url);
}


/********************************/

int HTRandom(int n) {
  return rand() % (n + 1);
}

void random_seed() {
  srand(time(NULL));
}

int generate_char_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 2) {
    char *str = argv[1];
    Tcl_AppendResult(interp, generate_char(str), NULL);      
    return TCL_OK;
  }
  return TCL_ERROR;
}

/* TCL versions of functions */

int random_tcl(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {
  if (argc == 2) {
    int n;
    char *result = malloc(50);
    if (Tcl_GetInt(interp, argv[1], &n) == TCL_OK) {
      sprintf(result, "%d", rand() % n);
      Tcl_AppendResult(interp, result, NULL);      
      return TCL_OK;
    }
  }
  return TCL_ERROR;
}

int add_item_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 4) {
    int size;
    char *int_str = argv[1];
    char *start = argv[2];
    char *str = argv[3];
    if (start && str && (Tcl_GetInt(interp, int_str, &size) == TCL_OK)) {
      Tcl_AppendResult(interp, add_item(size,start,str), NULL);      
      return TCL_OK;
    }
  }
  return TCL_ERROR;
}

int generate_genericurl_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 1) {
    Tcl_AppendResult(interp, generate_scheme(),"://",generate_login(), "/",\
		     generate_urlpath(50), NULL);      
    return TCL_OK;
  }
  return TCL_ERROR;
}

int ftpURL_tcl(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {

  if (argc == 1) {
    Tcl_AppendResult(interp, ftpURL(), NULL);      
    return TCL_OK;
  }
  return TCL_ERROR;
}

int httpURL_tcl(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {

  if (argc == 1) {
    Tcl_AppendResult(interp, httpURL(), NULL);      
    return TCL_OK;
  }
  return TCL_ERROR;
}

int gopherURL_tcl(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {

  if (argc == 1) {
    Tcl_AppendResult(interp, gopherURL(), NULL);      
    return TCL_OK;
  }
  return TCL_ERROR;
}

int telnetURL_tcl(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {

  if (argc == 1) {
    Tcl_AppendResult(interp, telnetURL(), NULL);      
    return TCL_OK;
  }
  return TCL_ERROR;
}

int generateURL_tcl(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {
  return TCL_OK;
}





