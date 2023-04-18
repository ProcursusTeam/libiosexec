/*	$OpenBSD: getusershell.c,v 1.18 2019/12/10 02:35:16 millert Exp $ */
/*
 * Copyright (c) 1985, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <paths.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/errno.h>

#include "libiosexec.h"
#include "libiosexec_private.h"

#ifdef _PATH_BSHELL
#undef _PATH_BSHELL
#endif

#ifdef _PATH_CSHELL
#undef _PATH_CSHELL
#endif

#ifdef _PATH_KSHELL
#undef _PATH_KSHELL
#endif

#ifdef _PATH_SHELLS
#undef _PATH_SHELLS
#endif

#define _PATH_BSHELL SHEBANG_REDIRECT_PATH "/usr/bin/sh"
#define _PATH_CSHELL SHEBANG_REDIRECT_PATH "/usr/bin/csh"
#define _PATH_KSHELL SHEBANG_REDIRECT_PATH "/usr/bin/ksh"

#define _PATH_SHELLS SHEBANG_REDIRECT_PATH "/etc/shells"

/*
 * Local shells should NOT be added here.  They should be added in
 * /etc/shells.
 */

static char *okshells[] = { _PATH_BSHELL, _PATH_CSHELL, _PATH_KSHELL, NULL };
static char **curshell, **shells;
static char **initshells(void);
static void *reallocarray(void *optr, size_t nmemb, size_t size);

/*
 * Get a list of shells from _PATH_SHELLS, if it exists.
 */
char *
ie_getusershell(void)
{
	char *ret;

	if (curshell == NULL)
		curshell = initshells();
	ret = *curshell;
	if (ret != NULL)
		curshell++;
	return (ret);
}

void
ie_endusershell(void)
{
	char **s;

	if ((s = shells))
		while (*s)
			free(*s++);
	free(shells);
	shells = NULL;

	curshell = NULL;
}

void
ie_setusershell(void)
{

	curshell = initshells();
}

static char **
initshells(void)
{
	size_t nshells, nalloc, linesize;
	char *line;
	FILE *fp;

	free(shells);
	shells = NULL;

	if ((fp = fopen(_PATH_SHELLS, "re")) == NULL)
		return (okshells);

	line = NULL;
	nalloc = 10; // just an initial guess
	nshells = 0;
	shells = reallocarray(NULL, nalloc, sizeof (char *));
	if (shells == NULL)
		goto fail;
	linesize = 0;
	while (getline(&line, &linesize, fp) != -1) {
		if (*line != '/')
			continue;
		line[strcspn(line, "#\n")] = '\0';
		if (!(shells[nshells] = strdup(line)))
			goto fail;

		if (nshells + 1 == nalloc) {
			char **new = reallocarray(shells, nalloc * 2, sizeof(char *));
			if (!new)
				goto fail;
			shells = new;
			nalloc *= 2;
		}
		nshells++;
	}
	free(line);
	shells[nshells] = NULL;
	(void)fclose(fp);
	return (shells);

fail:
	free(line);
	while (nshells)
		free(shells[nshells--]);
	free(shells);
	shells = NULL;
	(void)fclose(fp);
	return (okshells);
}


// The following taken from OpenBSD reallocarray.c

/*
 * This is sqrt(SIZE_MAX+1), as s1*s2 <= SIZE_MAX
 * if both s1 < MUL_NO_OVERFLOW and s2 < MUL_NO_OVERFLOW
 */
#define MUL_NO_OVERFLOW	((size_t)1 << (sizeof(size_t) * 4))

static void *
reallocarray(void *optr, size_t nmemb, size_t size)
{
	if ((nmemb >= MUL_NO_OVERFLOW || size >= MUL_NO_OVERFLOW) &&
	    nmemb > 0 && SIZE_MAX / nmemb < size) {
		errno = ENOMEM;
		return NULL;
	}
	if (size == 0 || nmemb == 0)
	    return NULL;
	return realloc(optr, size * nmemb);
}
