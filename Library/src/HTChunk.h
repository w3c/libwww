/*		Chunk handling:	Flexible arrays
**		===============================
**
** This module implements a flexible array. It is a general utility module.
** A chunk is a structure which may be extended.  These routines create
** and append data to chnuks, automatically reallocating them as necessary.
**
*/


typedef struct {
	int	size;		/* In bytes			*/
	int	growby;		/* Allocation unit in bytes	*/
	int	allocated;	/* Current size of *data	*/
	char *	data;		/* Pointer to malloced area or 0 */
} HTChunk;


#ifdef SHORT_NAMES
#define HTChunkClear		HTChClea
#define HTChunkPutc		HTChPutc
#define HTChunkPuts		HTChPuts
#define HTChunkCreate		HTChCrea
#define HTChunkTerminate	HTChTerm
#define HTChunkEnsure		HtChEnsu
#endif


/*	Create new chunk
**
** On entry,
**
**	growby		The number of bytes to allocate at a time
**			when the chunk is later extended. Arbitrary but
**			normally a trade-off time vs. memory
**
** On exit,
**	returns		A chunk pointer to the new chunk,
*/
extern HTChunk * HTChunkCreate PARAMS((int growby));


/*	Free a chunk
**
** On entry,
**	ch	A valid chunk pointer made by HTChunkCreate()
**
** On exit,
**	ch	is invalid and may not be used.
*/
extern void HTChunkFree PARAMS((HTChunk * ch));


/*	Clear a chunk
**
** On entry,
**	ch	A valid chunk pointer made by HTChunkCreate()
**
** On exit,
**	*ch	The size of the chunk is zero.
*/
extern void HTChunkClear PARAMS((HTChunk * ch));


/*	Ensure a chunk has a certain space in
**
** On entry,
**	ch	A valid chunk pointer made by HTChunkCreate()
**	s	The size required
**
** On exit,
**	*ch	Has size at least s
*/
extern void HTChunkEnsure PARAMS((HTChunk * ch, int s));


/*	Append a character to a  chunk
**
** On entry,
**	ch	A valid chunk pointer made by HTChunkCreate()
**	c	The character to be appended
** On exit,
**	*ch	Is one character bigger
*/

extern void HTChunkPutc PARAMS((HTChunk * ch, char c));

/*	Append a string to a  chunk
**
** On entry,
**	ch	A valid chunk pointer made by HTChunkCreate()
**	s	Tpoints to a zero-terminated string to be appended
** On exit,
**	*ch	Is bigger by strlen(s)
*/

extern void HTChunkPuts PARAMS((HTChunk * ch, const char *s));


/*	Append a zero character to a  chunk
**
** On entry,
**	ch	A valid chunk pointer made by HTChunkCreate()
** On exit,
**	*ch	Is one character bigger
*/

extern void HTChunkTerminate PARAMS((HTChunk * ch));
