/*Common functions to retrieve data structures out of tcl hash tables*/
/*Began implementation on 11/2/95*/

#include "tcl.h"
#include "WWWLib.h"

extern char *Keyname_lookupAnchor(HTAnchor *);

extern char *Keyname_lookupAssocList(HTAssocList *);

extern char *Keyname_lookupLinks(HTLink *);

extern char *Keyname_lookupList(HTList *);

extern char *Keyname_lookupRequest(HTRequest *);

extern char *Keyname_lookupStream(HTStream *);

extern char *Keyname_lookupVoid(void *);

extern char *Keyname_lookupUser(HTUserProfile *);

extern char *Keyname_lookupHTMIMEParseSet(HTMIMEParseSet *);

extern char *Keyname_lookupCallback(HTRequestCallback *);

extern char *Keyname_lookupNet(HTNet *);

extern Tcl_HashEntry *CreateNewEntry(Tcl_HashTable *table, char *name, 
				     char **final_keyname);


extern Tcl_HashEntry *CreateNewAnchorEntry(char *, char **);

extern Tcl_HashEntry *CreateNewListEntry(char *, char **);

extern Tcl_HashEntry *CreateNewLinkEntry(char *, char **);

extern Tcl_HashEntry *CreateNewRequestEntry(char *, char **);

extern Tcl_HashEntry *CreateNewStreamEntry(char *, char **);

extern Tcl_HashEntry *CreateNewVoidEntry(char *, char **);

extern Tcl_HashEntry *CreateNewUserEntry(char *, char **);

extern Tcl_HashEntry *CreateNewAssocListEntry(char *, char **);
