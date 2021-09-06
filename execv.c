#include "utils.h"
#if defined (__APPLE__)
#include <sys/syslimits.h>
#elif defined __linux__
#include <linux/limits.h>
#endif
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libiosexec.h"
#include <sys/uio.h>

extern char** environ;

int ie_execve(const char* path, char* const argv[], char* const envp[]) {
    execve(path, argv, envp);
    int execve_ret = errno;

    if (execve_ret != EPERM && execve_ret != ENOEXEC) {
        return -1;
    }

    char** argv_new = get_new_argv(path, argv);
    if (argv_new == NULL) {
        return -1;
    }

    int ret = ie_execve(argv_new[0], argv_new, envp);
    int saved_errno = errno;
    free_new_argv(argv_new);
    errno = saved_errno;
    return ret;
}

int ie_execv(const char *path, char *const argv[]) {
    return ie_execve(path, argv, environ);
}

int ie_execvpe(const char *name, char *const *argv, char *const *envp) {
	char **memp;
	int cnt;
	size_t lp, ln, len;
	char *p;
	int eacces = 0;
	char *bp, *cur, *path, buf[PATH_MAX];

	/*
	 * Do not allow null name
	 */
	if (name == NULL || *name == '\0') {
		errno = ENOENT;
		return (-1);
 	}

	/* If it's an absolute or relative path name, it's easy. */
	if (strchr(name, '/')) {
		bp = (char *)name;
		cur = path = NULL;
		goto retry;
	}
	bp = buf;

	/* Get the path we're searching. */
	if (!(path = getenv("PATH")))
		path = DEFAULT_PATH;
	len = strlen(path) + 1;
	cur = alloca(len);
	if (cur == NULL) {
		errno = ENOMEM;
		return (-1);
	}
	strlcpy(cur, path, len);
	path = cur;
	while ((p = strsep(&cur, ":"))) {
		/*
		 * It's a SHELL path -- double, leading and trailing colons
		 * mean the current directory.
		 */
		if (!*p) {
			p = ".";
			lp = 1;
		} else
			lp = strlen(p);
		ln = strlen(name);

		/*
		 * If the path is too long complain.  This is a possible
		 * security issue; given a way to make the path too long
		 * the user may execute the wrong program.
		 */
		if (lp + ln + 2 > sizeof(buf)) {
			struct iovec iov[3];

			iov[0].iov_base = "execvp: ";
			iov[0].iov_len = 8;
			iov[1].iov_base = p;
			iov[1].iov_len = lp;
			iov[2].iov_base = ": path too long\n";
			iov[2].iov_len = 16;
			(void)writev(STDERR_FILENO, iov, 3);
			continue;
		}
		bcopy(p, buf, lp);
		buf[lp] = '/';
		bcopy(name, buf + lp + 1, ln);
		buf[lp + ln + 1] = '\0';

retry:		(void)ie_execve(bp, argv, envp);
		switch(errno) {
		case E2BIG:
			goto done;
		case EISDIR:
		case ELOOP:
		case ENAMETOOLONG:
		case ENOENT:
			break;
		case ENOEXEC:
			for (cnt = 0; argv[cnt]; ++cnt)
				;
			memp = alloca((cnt + 2) * sizeof(char *));
			if (memp == NULL)
				goto done;
			memp[0] = "sh";
			memp[1] = bp;
			bcopy(argv + 1, memp + 2, cnt * sizeof(char *));
			(void)ie_execve(DEFAULT_SHELL, memp, envp);
			goto done;
		case ENOMEM:
			goto done;
		case ENOTDIR:
			break;
		case ETXTBSY:
			/*
			 * We used to retry here, but sh(1) doesn't.
			 */
			goto done;
		case EACCES:
			eacces = 1;
			break;
		default:
			goto done;
		}
	}
	if (eacces)
		errno = EACCES;
	else if (!errno)
		errno = ENOENT;
done:
	return (-1);
}

int ie_execvp(const char *file, char *const argv[])
{
	return ie_execvpe(file, argv, environ);
}
