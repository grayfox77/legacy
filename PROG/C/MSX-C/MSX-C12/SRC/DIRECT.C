/*
	(C) Copyright by ASCII Corporation, 1989
		All rights Reserved.

	File: direct.c		Directory Manipulate Functions

	Includes following functions
		expargs	chdir	mkdir	rmdir	getcwd
*/
#pragma	nonrec
#include <stdio.h>
#include <bdosfunc.h>

STATUS	_whlpath(whole, prog)
char	whole[], *prog;
{
	extern	STATUS	_ffirst();
	FIB	fib;

	if(_ffirst(prog, &fib, (TINY)0) == ERROR)
		return(ERROR);
	whole[0] = fib.dc + 'A' - 1;
	whole[1] = ':';
	whole[2] = '\\';
	bdosh(_WPATH, whole + 3);
	return(OK);
}

int     _cmp(p, q)
char    **p, **q;
{
    return (strcmp(*p, *q));
}


int	expargs(argc, argv, maxargc, xargv)
int	argc, maxargc;
char	*argv[], *xargv[];
{
	char	fib[64];
	int     xargc, topc;
	char    rc, *s, *p, **topv;
	char    *last_item, *term, termc;
	unsigned len;
	TINY    flag;

	extern  char    *_p_path();
	extern  STATUS  _ffirst(), _fnext();

	xargc = 0;
	while (--argc >= 0) {
		p = *argv++;
		while (1) {
			while (*p == '+')
				p++;
			if (*p == '\0')
				break;
			termc = *(term = _p_path(p, &last_item, &flag));
			*term = '\0';
			if (flag & 0x20) {	/* wild card */
				topv = xargv;   
				topc = xargc;
				len = last_item - p;
				rc = _ffirst(p, fib, (TINY)0);
				while (rc != ERROR) {
					if (xargc >= maxargc)
						goto ErrorExit;
					if ((s = alloc(len+strlen(fib+1)+1)) == NULL)
						goto ErrorExit;
					memcpy(s, p, len);
					strcpy(s+len, fib+1);
					*xargv++ = s;
					xargc++;
					rc = _fnext(fib);
				}
				qsort(topv, xargc - topc, sizeof(char *), _cmp);
			} else {
				if (xargc >= maxargc)
					goto ErrorExit;
				*xargv++ = p;
				xargc++;
			}
			*(p = term) = termc;	/* restore terminator */
		}
	}
	return (xargc);
ErrorExit:
	while (xargc--)
		free(*--xargv);
	return (ERROR);
}

STATUS	chdir(path)		/*  change current work directory  */
char	*path;
{
	if(calla(BDOS, 0, 0, (int)_CHDIR, (int)path))
		return(ERROR);	/*  error return  */
	else
		return(OK);	/*  successful chdir  */
}

#define	_DIR	0x10		/*  directory attribute : bit4  */

STATUS	mkdir(path)		/*  make directory  */
char	*path;
{
	if(calla(BDOS, 0, 0, (_DIR << 8) + (int)_CREATE, (int)path))
		return(ERROR);	/*  error return  */
	else
		return(OK);	/*  successful mkdir  */
}

TINY	_attr(path)		/*  get attribute of directory entry  */
char	*path;
{
	XREG	r;

	r.bc = (unsigned)_ATTR;
	r.de = (unsigned)path;
	r.af = (unsigned)0;
	callxx(BDOS, &r);
	if(r.af >> 8)
		return(ERROR);	/*  error return  */
	else
		return((TINY)r.hl);	/*  successful get attribute  */
}

STATUS	rmdir(path)		/*  remove directory  */
char	*path;
{
	TINY	a;

	if((a = _attr(path)) == ERROR || (a & _DIR) == 0)
		return(ERROR);
	if(calla(BDOS, 0, 0, (int)_DELETE, (int)path))
		return(ERROR);	/*  error return  */
	else
		return(OK);	/*  successful rmdir  */
}

char	*getcwd(cwd, n)		/*  get current work directory  */
char	*cwd;
int	n;
{
	char	cd[64+3];

	cd[0] = bdos(_CURDRV) + 'A';
	cd[1] = ':';
	cd[2] = '\\';
	if(calla(BDOS, 0, 0, (int)_GETCD, (int)(cd + 3)))
		return(NULL);	/*  error return  */
	if(cwd == NULL)		/*  buf not specified  */
		if((cwd = alloc(n)) == NULL)	/*  alloc buf  */
			return(NULL);
	return(strncpy(cwd, cd, n));
}

                           