#include "Libdata.h"

#define max_keyname 20

Tcl_HashTable   HTableAnchor;
Tcl_HashTable	HTableAssoc;
Tcl_HashTable	HTableLink;
Tcl_HashTable	HTableList;
Tcl_HashTable	HTableReq;
Tcl_HashTable	HTableStream;
Tcl_HashTable	HTableVoid;
Tcl_HashTable	HTableUser;
Tcl_HashTable   HTableProt;
Tcl_HashTable   HTableCallback;
Tcl_HashTable	HTableMIMEParseSet;

/*This function has limited use to lookup a keyname given an anchor.
  if there is no associated keyname, NULL is returned 			*/

char *Keyname_lookupAnchor(HTAnchor *anchor) {

    Tcl_HashEntry *entryPtr;	
    Tcl_HashSearch search;
    HTAnchor *anchor_obj;
    char *keyname = NULL;
	
    for (entryPtr = Tcl_FirstHashEntry(&HTableAnchor, &search); entryPtr ; entryPtr = Tcl_NextHashEntry(&search)) {
	anchor_obj = (HTAnchor *) Tcl_GetHashValue(entryPtr);
	if (anchor_obj == anchor) 
	    StrAllocCopy(keyname, Tcl_GetHashKey(&HTableAnchor, entryPtr));
    }
    return keyname;
}

char *Keyname_lookupAssocList(HTAssocList *list) {

    Tcl_HashEntry *entryPtr;
    Tcl_HashSearch search;
    HTAssocList *list_obj;
    char *keyname = NULL;
	
    for (entryPtr = Tcl_FirstHashEntry(&HTableAssoc, &search); entryPtr; entryPtr = Tcl_NextHashEntry(&search)) {
	list_obj = (HTAssocList *) Tcl_GetHashValue(entryPtr);
	if (list_obj == list) 
	    StrAllocCopy(keyname, Tcl_GetHashKey(&HTableAssoc, entryPtr));
    }
    return keyname;
}

char *Keyname_lookupLinks(HTLink *link) {

    Tcl_HashEntry *entryPtr;	
    Tcl_HashSearch search;
    HTLink *link_obj;
    char *keyname = NULL;
	
    for (entryPtr = Tcl_FirstHashEntry(&HTableLink, &search); entryPtr; entryPtr = Tcl_NextHashEntry(&search)) {
	link_obj = (HTLink *) Tcl_GetHashValue(entryPtr);
	if (link_obj == link) 
	    StrAllocCopy(keyname, Tcl_GetHashKey(&HTableLink, entryPtr));
    }
    return keyname;
}

char *Keyname_lookupList(HTList *list) {

    Tcl_HashEntry *entryPtr;	
    Tcl_HashSearch search;
    HTList *list_obj;
    char *keyname = NULL;
	
    for (entryPtr = Tcl_FirstHashEntry(&HTableList, &search); entryPtr; entryPtr = Tcl_NextHashEntry(&search)) {
	list_obj = (HTList *) Tcl_GetHashValue(entryPtr);
	if (list_obj == list) 
	    StrAllocCopy(keyname, Tcl_GetHashKey(&HTableList, entryPtr));
    }
    return keyname;
}

char *Keyname_lookupRequest(HTRequest *request) {

    Tcl_HashEntry *entryPtr;	
    Tcl_HashSearch search;
    HTRequest *request_obj;
    char *keyname = NULL;
	
    for (entryPtr = Tcl_FirstHashEntry(&HTableReq, &search); entryPtr; entryPtr = Tcl_NextHashEntry(&search)) {
	request_obj = (HTRequest *) Tcl_GetHashValue(entryPtr);
	if (request_obj == request) 
	    StrAllocCopy(keyname, Tcl_GetHashKey(&HTableReq, entryPtr));
    }
    return keyname;
}

char *Keyname_lookupStream(HTStream *stream) {

    Tcl_HashEntry *entryPtr;	
    Tcl_HashSearch search;
    HTStream *stream_obj;
    char *keyname = NULL;
	
    for (entryPtr = Tcl_FirstHashEntry(&HTableStream, &search); entryPtr; entryPtr = Tcl_NextHashEntry(&search)) {
	stream_obj = (HTStream *) Tcl_GetHashValue(entryPtr);
	if (stream_obj == stream) 
	    StrAllocCopy(keyname, Tcl_GetHashKey(&HTableStream, entryPtr));
    }
    return keyname;
}

char *Keyname_lookupVoid(void *obj) {

    Tcl_HashEntry *entryPtr;	
    Tcl_HashSearch search;
    void *void_obj;
    char *keyname = NULL;
	
    for (entryPtr = Tcl_FirstHashEntry(&HTableVoid, &search); entryPtr; entryPtr = Tcl_NextHashEntry(&search)) {
	void_obj = (void *) Tcl_GetHashValue(entryPtr);
	if (void_obj == obj) 
	    StrAllocCopy(keyname, Tcl_GetHashKey(&HTableVoid, entryPtr));
    }
    return keyname;
}

char *Keyname_lookupUser(HTUserProfile *obj) {

    Tcl_HashEntry *entryPtr;	
    Tcl_HashSearch search;
    HTUserProfile *user_obj;
    char *keyname = NULL;
	
    for (entryPtr = Tcl_FirstHashEntry(&HTableUser, &search); entryPtr; entryPtr = Tcl_NextHashEntry(&search)) {
	user_obj = (HTUserProfile *) Tcl_GetHashValue(entryPtr);
	if (user_obj == obj) 
	    StrAllocCopy(keyname, Tcl_GetHashKey(&HTableUser, entryPtr));
    }
    return keyname;
}

char *Keyname_lookupHTMIMEParseSet(HTMIMEParseSet *obj) {

    Tcl_HashEntry *entryPtr;	
    Tcl_HashSearch search;
    HTMIMEParseSet *user_obj;
    char *keyname = NULL;
	
    for (entryPtr = Tcl_FirstHashEntry(&HTableUser, &search); entryPtr; entryPtr = Tcl_NextHashEntry(&search)) {
	user_obj = (HTMIMEParseSet *) Tcl_GetHashValue(entryPtr);
	if (user_obj == obj) 
	    StrAllocCopy(keyname, Tcl_GetHashKey(&HTableUser, entryPtr));
    }
    return keyname;
}

char *Keyname_lookupCallback(HTRequestCallback *obj) {

    Tcl_HashEntry *entryPtr;	
    Tcl_HashSearch search;
    HTRequestCallback *user_obj;
    char *keyname = NULL;
	
    for (entryPtr = Tcl_FirstHashEntry(&HTableUser, &search); entryPtr; entryPtr = Tcl_NextHashEntry(&search)) {
	user_obj = (HTRequestCallback *) Tcl_GetHashValue(entryPtr);
	if (user_obj == obj) 
	    StrAllocCopy(keyname, Tcl_GetHashKey(&HTableUser, entryPtr));
    }
    return keyname;
}

char *Keyname_lookupNet(HTNet *obj) {

    Tcl_HashEntry *entryPtr;	
    Tcl_HashSearch search;
    HTNet *user_obj;
    char *keyname = NULL;
	
    for (entryPtr = Tcl_FirstHashEntry(&HTableUser, &search); entryPtr; entryPtr = Tcl_NextHashEntry(&search)) {
	user_obj = (HTNet *) Tcl_GetHashValue(entryPtr);
	if (user_obj == obj) 
	    StrAllocCopy(keyname, Tcl_GetHashKey(&HTableUser, entryPtr));
    }
    return keyname;
}

Tcl_HashEntry *CreateNewAnchorEntry(char *name, char **final_keyname) {
    int new = 0;
    int id  = 0;
    Tcl_HashEntry *entryPtr;
    char *keyname = malloc(max_keyname);
    
    while (!new) {
	sprintf(keyname, "%s_%d", name, id);
	entryPtr = Tcl_CreateHashEntry(&HTableAnchor, keyname, &new);
	++id;
    }
    *final_keyname = keyname;
    return entryPtr;
}

Tcl_HashEntry *CreateNewListEntry(char *name, char **final_keyname) {
    int new = 0;
    int id  = 0;
    Tcl_HashEntry *entryPtr;
    char *keyname = malloc(sizeof(name)+5);
    
    while (!new) {
	sprintf(keyname, "%s_%d", name, id);
	entryPtr = Tcl_CreateHashEntry(&HTableList, keyname, &new);
	++id;
    }
    *final_keyname = keyname;
    return entryPtr;
}

Tcl_HashEntry *CreateNewLinkEntry(char *name, char **final_keyname) {
    int new = 0;
    int id;
    Tcl_HashEntry *entryPtr;
    char *keyname = malloc(sizeof(name)+5);
    
    for(id=0; (!new); ++id) {
	sprintf(keyname, "%s_%d", name, id);
	entryPtr = Tcl_CreateHashEntry(&HTableLink, keyname, &new);
    }
    *final_keyname = keyname;
    return entryPtr;
}

/* Yields an entry with a new unique name, given a base to describe the
   keyname. To retrieve the keyname, the string is contained in final_keyname*/

Tcl_HashEntry *CreateNewRequestEntry(char *name, char **final_keyname) {
    int new = 0;
    int id  = 0;
    Tcl_HashEntry *entryPtr;
    char *keyname = malloc(max_keyname);
    
    while (!new) {
	sprintf(keyname, "%s_%d", name, id);
        entryPtr = Tcl_CreateHashEntry(&HTableReq, keyname, &new);	
	++id;
    }
    *final_keyname = keyname;
    return entryPtr;
}

Tcl_HashEntry *CreateNewStreamEntry(char *name, char **final_keyname) {
    int new = 0;
    int id  = 0;
    Tcl_HashEntry *entryPtr;
    char *keyname = malloc(max_keyname);
    
    while (!new) {
	sprintf(keyname, "%s_%d", name, id);
        entryPtr = Tcl_CreateHashEntry(&HTableStream, keyname, &new);	
	++id;
    }
    *final_keyname = keyname;
    return entryPtr;
}

Tcl_HashEntry *CreateNewProtocolEntry(char *name, char **final_keyname) {
  int new =0;
  int id = 0;
  Tcl_HashEntry *entryPtr;
  char *keyname = malloc(max_keyname);
  
  while (!new) {
    sprintf(keyname, "%s_%d", name, id);
    entryPtr = Tcl_CreateHashEntry(&HTableProt, keyname, &new);
    ++id;
  }
  *final_keyname = keyname;
  return entryPtr;
}

Tcl_HashEntry *CreateNewVoidEntry(char *name, char **final_keyname) {
    int new = 0;
    int id  = 0;
    Tcl_HashEntry *entryPtr;
    char *keyname = malloc(max_keyname);
    
    while (!new) {
	sprintf(keyname, "%s_%d", name, id);
        entryPtr = Tcl_CreateHashEntry(&HTableVoid, keyname, &new);	
	++id;
    }
    *final_keyname = keyname;
    return entryPtr;
}

Tcl_HashEntry *CreateNewUserEntry(char *name, char **final_keyname) {
    int new = 0;
    int id  = 0;
    Tcl_HashEntry *entryPtr;
    char *keyname = malloc(max_keyname);
    
    while (!new) {
	sprintf(keyname, "%s_%d", name, id);
        entryPtr = Tcl_CreateHashEntry(&HTableUser, keyname, &new);	
	++id;
    }
    *final_keyname = keyname;
    return entryPtr;
}

Tcl_HashEntry *CreateNewAssocListEntry(char *name, char **final_keyname) {
  int new = 0;
  int id  = 0;
  Tcl_HashEntry *entryPtr;
  char *keyname = malloc(max_keyname);
    
  while (!new) {
    sprintf(keyname, "%s_%d", name, id);
    entryPtr = Tcl_CreateHashEntry(&HTableAssoc, keyname, &new);	
    ++id;
  }
  *final_keyname = keyname;
  return entryPtr;
}

Tcl_HashEntry *CreateNewCallbackEntry(char *name, char **final_keyname) {
  int new = 0;
  int id = 0;
  Tcl_HashEntry *entryPtr;
  char *keyname = malloc(max_keyname);
  
  while (!new) {
    sprintf(keyname, "%s_%d", name, id);
    entryPtr = Tcl_CreateHashEntry(&HTableCallback, keyname, &new);
    ++id;
  }
 *final_keyname = keyname;
 return entryPtr;
}

Tcl_HashEntry *CreateNewEntry(Tcl_HashTable *table, char *name, char **final_keyname) 
{
    int new = 0;
    int id = 0;
    Tcl_HashEntry *entryPtr;
    char *keyname = malloc(max_keyname);
  
    while (!new) {
	sprintf(keyname, "%s_%d", name, id);
	entryPtr = Tcl_CreateHashEntry(table, keyname, &new);
	++id;
    }
    *final_keyname = keyname;
    return entryPtr;
}

Tcl_HashEntry *FindVoidEntry(char *void_key) 
{
    Tcl_HashEntry *entryPtr;
  
    entryPtr = Tcl_FindHashEntry(&HTableAnchor, void_key);
    if (!entryPtr) return entryPtr;
  
    entryPtr = Tcl_FindHashEntry(&HTableReq, void_key);
    if (!entryPtr) return entryPtr;

    return NULL;
}
