extern void init_strings(void);

extern int random_tcl(ClientData, Tcl_Interp *, int, char **);

extern int generate_char_tcl(ClientData, Tcl_Interp *, int, char **);

extern int add_item_tcl(ClientData, Tcl_Interp *, int, char **);

extern int generate_login_tcl(ClientData, Tcl_Interp *, int, char **);

extern int generate_genericurl_tcl(ClientData, Tcl_Interp *, int, char **);

extern void random_seed(void);

extern int ftpURL_tcl(ClientData, Tcl_Interp *, int, char **);

extern int httpURL_tcl(ClientData, Tcl_Interp *, int, char **);

extern int gopherURL_tcl(ClientData, Tcl_Interp *, int, char **);

extern int telnetURL_tcl(ClientData, Tcl_Interp *, int, char **);
