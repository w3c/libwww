#include "WWWLib.h"

/* very minimal HTStream - it needs no state as it does nothing but call TTYPrint */
struct _HTStream {
    const HTStreamClass *	isa;
};

PRIVATE int Kwik_put_block (HTStream * me, const char * b, int l)
{
	char * space;
	if ((space = (char *) HT_CALLOC(l+1, sizeof(char))) == NULL)
	    HT_OUTOFMEM("char");
    memcpy(space, b, l);
    HTPrint(space);
    HT_FREE(space);
    return HT_OK;
}

PRIVATE int Kwik_put_character (HTStream * me, char c)
{
    return Kwik_put_block(me, &c, 1);
}

PRIVATE int Kwik_put_string (HTStream * me, const char * s)
{
    HTPrint(s);
    return HT_OK;
}

PRIVATE int Kwik_flush (HTStream * me)
{
    return HT_OK;
}

PUBLIC int Kwik_free (HTStream * me)
{
   HT_FREE(me);
    return HT_OK;
}

PRIVATE int Kwik_abort (HTStream * me, HTList * e)
{
   HT_FREE(me);
    return HT_OK;
}

PRIVATE const HTStreamClass KwikClass =
{		
    "KwikClass",
    Kwik_flush,
    Kwik_free,
    Kwik_abort,
    Kwik_put_character,
    Kwik_put_string,
    Kwik_put_block
};

PUBLIC HTStream * Kwik_new (void)
{
    HTStream * me;
	if ((me = (HTStream *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
	    HT_OUTOFMEM("Kwik");
    me->isa = &KwikClass;
    return me;
}

