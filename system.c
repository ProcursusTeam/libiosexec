/*
 * Copyright (c) 1988, 1993
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
 * 4. Neither the name of the University nor the names of its contributors
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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstrict-prototypes"

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)system.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/lib/libc/stdlib/system.c,v 1.11 2007/01/09 00:28:10 imp Exp $");

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <spawn.h>
#include <paths.h>
#include <errno.h>

#include <crt_externs.h>
#define environ (*_NSGetEnviron())

#include <TargetConditionals.h>

#include <pthread.h>

#include "libiosexec.h"

static pthread_mutex_t __systemfn_mutex = PTHREAD_MUTEX_INITIALIZER;

int
ie_system(const char *command)
{
	pid_t pid, savedpid;
	int pstat, err;
	struct sigaction ign, intact, quitact;
	sigset_t newsigblock, oldsigblock, defaultsig;
	posix_spawnattr_t attr;
	short flags = POSIX_SPAWN_SETSIGMASK;
	const char *argv[] = {"sh", "-c", command, NULL};

	if (command == NULL) {		/* just checking... */
		if (access(_PATH_BSHELL, F_OK) == -1)	/* if no sh or no access */
			return(0);
		else
			return(1);
	}

	if ((err = posix_spawnattr_init(&attr)) != 0) {
		errno = err;
		return -1;
	}
	(void)sigemptyset(&defaultsig);

	pthread_mutex_lock(&__systemfn_mutex);
	/*
	 * Ignore SIGINT and SIGQUIT, block SIGCHLD. Remember to save
	 * existing signal dispositions.
	 */
	ign.sa_handler = SIG_IGN;
	(void)sigemptyset(&ign.sa_mask);
	ign.sa_flags = 0;
	(void)sigaction(SIGINT, &ign, &intact);
	if (intact.sa_handler != SIG_IGN) {
		sigaddset(&defaultsig, SIGINT);
		flags |= POSIX_SPAWN_SETSIGDEF;
	}
	(void)sigaction(SIGQUIT, &ign, &quitact);
	if (quitact.sa_handler != SIG_IGN) {
		sigaddset(&defaultsig, SIGQUIT);
		flags |= POSIX_SPAWN_SETSIGDEF;
	}
	(void)sigemptyset(&newsigblock);
	(void)sigaddset(&newsigblock, SIGCHLD);
	(void)sigprocmask(SIG_BLOCK, &newsigblock, &oldsigblock);
	(void)posix_spawnattr_setsigmask(&attr, &oldsigblock);
	if (flags & POSIX_SPAWN_SETSIGDEF) {
		(void)posix_spawnattr_setsigdefault(&attr, &defaultsig);
	}
	(void)posix_spawnattr_setflags(&attr, flags);

	err = ie_posix_spawn(&pid, _PATH_BSHELL, NULL, &attr, (char *const *)argv, environ);
	(void)posix_spawnattr_destroy(&attr);
	if (err == 0) {
		savedpid = pid;
		do {
			pid = wait4(savedpid, &pstat, 0, (struct rusage *)0);
		} while (pid == -1 && errno == EINTR);
		if (pid == -1) pstat = -1;
	} else if (err == ENOMEM || err == EAGAIN) { /* as if fork failed */
		pstat = -1;
	} else {
		pstat = W_EXITCODE(127, 0); /* couldn't exec shell */
	}

	(void)sigaction(SIGINT, &intact, NULL);
	(void)sigaction(SIGQUIT,  &quitact, NULL);
	(void)sigprocmask(SIG_SETMASK, &oldsigblock, NULL);
	pthread_mutex_unlock(&__systemfn_mutex);
	return(pstat);
}

#pragma clang diagnostic pop
