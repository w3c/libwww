/*	a_stdio.h, from agl@glas2.glas.apc.org (Anton Tropashko) 
//      patch applied by HWL 18/7/94
*/
#ifndef A_STDIO_H
#define A_STDIO_H
#include <stdio.h>
#define H 64000
int	a_print(char	*, int, FILE *fp);
char	*agets(char *, int psize, FILE *fp);
int	aputc(char, FILE *);
int	awrite(const void *str, int size, int nmemb, FILE *fp);
int	anprint(char *str, int psize, FILE *fp);
int	do_null(),do_k2a(),do_a2k(),do_strip8(),do_enc(int),doienc(int);
int	doinull(),doia2k();
int	a_spawn(char	*argv[], char *f_stdin, char *f_stdout,
		char *f_stderr);
struct	ARc {
	int	locale,
		encoding,
		i_encoding;
/*public: ARc()	{	locale=0; encoding=0; i_encoding=0; }*/
};
extern	struct ARc arc;
#endif
