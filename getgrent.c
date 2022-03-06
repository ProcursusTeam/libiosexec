/*	$OpenBSD: getgrent.c,v 1.48 2019/07/02 15:54:05 deraadt Exp $ */
/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * Portions Copyright (c) 1994, Jason Downs. All Rights Reserved.
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

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <grp.h>
#include <errno.h>

#include "libiosexec.h"

#define __THREAD_NAME(name)	__CONCAT(_thread_tagname_,name)

#define _THREAD_PRIVATE_KEY(name)					\
	static void *__THREAD_NAME(name)

#define _THREAD_PRIVATE_MUTEX_LOCK(name)		do {} while (0)
#define _THREAD_PRIVATE_MUTEX_UNLOCK(name)		do {} while (0)

#define _THREAD_PRIVATE(keyname, storage, error)	&(storage)

#define      _GR_BUF_LEN             (1024+200*sizeof(char*))

/* This global storage is locked for the non-rentrant functions */
_THREAD_PRIVATE_KEY(gr_storage);
static struct group_storage {
#define	MAXGRP		200
	char *members[MAXGRP];
#define	MAXLINELENGTH	1024
	char line[MAXLINELENGTH];
} gr_storage;
#define GETGR_R_SIZE_MAX	_GR_BUF_LEN

/* File pointers are locked with the 'gr' mutex */
_THREAD_PRIVATE_KEY(gr);
static FILE *_gr_fp;
static struct group _gr_group;
static int _gr_stayopen;
static int grscan(int, gid_t, const char *, struct group *, struct group_storage *,
	int *);
static int start_gr(void);
static void endgrent_basic(void);

static struct group *getgrnam_gs(const char *, struct group *,
	struct group_storage *);
static struct group *getgrgid_gs(gid_t, struct group *,
	struct group_storage *);


struct group *
_getgrent_yp(int *foundyp)
{
	struct group *p_gr = (struct group*)_THREAD_PRIVATE(gr, _gr_group, NULL);
	struct group_storage *gs = (struct group_storage *)_THREAD_PRIVATE(gr_storage,
	    gr_storage, NULL);

	_THREAD_PRIVATE_MUTEX_LOCK(gr);
	if ((!_gr_fp && !start_gr()) || !grscan(0, 0, NULL, p_gr, gs, foundyp))
		p_gr = NULL;
	_THREAD_PRIVATE_MUTEX_UNLOCK(gr);
	return (p_gr);
}

struct group *
ie_getgrent(void)
{
	return (_getgrent_yp(NULL));
}

static struct group *
getgrnam_gs(const char *name, struct group *p_gr, struct group_storage *gs)
{
	int rval;

	_THREAD_PRIVATE_MUTEX_LOCK(gr);
	if (!start_gr())
		rval = 0;
	else {
		rval = grscan(1, 0, name, p_gr, gs, NULL);
		if (!_gr_stayopen)
			endgrent_basic();
	}
	_THREAD_PRIVATE_MUTEX_UNLOCK(gr);
	return(rval ? p_gr : NULL);
}

struct group *
ie_getgrnam(const char *name)
{
	struct group *p_gr = (struct group*)_THREAD_PRIVATE(gr,_gr_group,NULL);
	struct group_storage *gs = (struct group_storage *)_THREAD_PRIVATE(gr_storage,
	    gr_storage, NULL);

	return getgrnam_gs(name, p_gr, gs);
}

int
ie_getgrnam_r(const char *name, struct group *grp, char *buffer,
	size_t bufsize, struct group **result)
{
	int errnosave;
	int ret;

	if (bufsize < GETGR_R_SIZE_MAX)
		return ERANGE;
	errnosave = errno;
	errno = 0;
	*result = getgrnam_gs(name, grp, (struct group_storage *)buffer);
	if (*result == NULL)
		ret = errno;
	else
		ret = 0;
	errno = errnosave;
	return ret;
}

static struct group *
getgrgid_gs(gid_t gid, struct group *p_gr, struct group_storage *gs)
{
	int rval;

	_THREAD_PRIVATE_MUTEX_LOCK(gr);
	if (!start_gr())
		rval = 0;
	else {
		rval = grscan(1, gid, NULL, p_gr, gs, NULL);
		if (!_gr_stayopen)
			endgrent_basic();
	}
	_THREAD_PRIVATE_MUTEX_UNLOCK(gr);
	return(rval ? p_gr : NULL);
}

struct group *
ie_getgrgid(gid_t gid)
{
	struct group *p_gr = (struct group*)_THREAD_PRIVATE(gr, _gr_group, NULL);
	struct group_storage *gs = (struct group_storage *)_THREAD_PRIVATE(gr_storage,
	    gr_storage, NULL);

	return getgrgid_gs(gid, p_gr, gs);
}

int
ie_getgrgid_r(gid_t gid, struct group *grp, char *buffer, size_t bufsize,
	struct group **result)
{
	int errnosave;
	int ret;

	if (bufsize < GETGR_R_SIZE_MAX)
		return ERANGE;
	errnosave = errno;
	errno = 0;
	*result = getgrgid_gs(gid, grp, (struct group_storage *)buffer);
	if (*result == NULL)
		ret = errno;
	else
		ret = 0;
	errno = errnosave;
	return ret;
}

static int
start_gr(void)
{

	if (_gr_fp) {
		rewind(_gr_fp);
		return(1);
	}


	return((_gr_fp = fopen(_PATH_GROUP, "re")) ? 1 : 0);
}

void
ie_setgrent(void)
{
	int saved_errno;

	saved_errno = errno;
	setgroupent(0);
	errno = saved_errno;
}

int
setgroupent(int stayopen)
{
	int retval;

	_THREAD_PRIVATE_MUTEX_LOCK(gr);
	if (!start_gr())
		retval = 0;
	else {
		_gr_stayopen = stayopen;
		retval = 1;
	}
	_THREAD_PRIVATE_MUTEX_UNLOCK(gr);
	return (retval);
}

static
void
endgrent_basic(void)
{
	int saved_errno;

	if (_gr_fp) {
		saved_errno = errno;
		fclose(_gr_fp);
		_gr_fp = NULL;
		errno = saved_errno;
	}
}

void
ie_endgrent(void)
{
	_THREAD_PRIVATE_MUTEX_LOCK(gr);
	endgrent_basic();
	_THREAD_PRIVATE_MUTEX_UNLOCK(gr);
}

static int
grscan(int search, gid_t gid, const char *name, struct group *p_gr,
    struct group_storage *gs, int *foundyp)
{
	char *cp, **m;
	char *bp, *endp;
	u_long ul;
	char **members;
	char *line;
	int saved_errno;

	if (gs == NULL)
		return 0;
	members = gs->members;
	line = gs->line;
	saved_errno = errno;

	for (;;) {
		if (!fgets(line, sizeof(gs->line), _gr_fp)) {
			if (feof(_gr_fp) && !ferror(_gr_fp))
				errno = saved_errno;
			return 0;
		}
		bp = line;
		/* skip lines that are too big */
		if (!strchr(line, '\n')) {
			int ch;

			while ((ch = getc_unlocked(_gr_fp)) != '\n' &&
			    ch != EOF)
				;
			continue;
		}
		p_gr->gr_name = strsep(&bp, ":\n");
		if (search && name && strcmp(p_gr->gr_name, name))
			continue;
		p_gr->gr_passwd = strsep(&bp, ":\n");
		if (!(cp = strsep(&bp, ":\n")))
			continue;
		ul = strtoul(cp, &endp, 10);
		if (endp == cp || *endp != '\0' || ul >= GID_MAX)
			continue;
		p_gr->gr_gid = ul;
		if (search && name == NULL && p_gr->gr_gid != gid)
			continue;
		cp = NULL;
		if (bp == NULL)
			continue;
		for (m = p_gr->gr_mem = members;; bp++) {
			if (m == &members[MAXGRP - 1])
				break;
			if (*bp == ',') {
				if (cp) {
					*bp = '\0';
					*m++ = cp;
					cp = NULL;
				}
			} else if (*bp == '\0' || *bp == '\n' || *bp == ' ') {
				if (cp) {
					*bp = '\0';
					*m++ = cp;
				}
				break;
			} else if (cp == NULL)
				cp = bp;
		}
		*m = NULL;
		errno = saved_errno;
		return 1;
	}
	/* NOTREACHED */
}
