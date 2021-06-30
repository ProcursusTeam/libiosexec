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

#define STRINGS_ARE_NOT_EQUAL(x, y, len) strncmp(x, y, len)

extern char** environ;

static inline int has_non_printable_char(char* str, size_t n) {
    for (int i = 0; i < n; i++) {
        if (!isprint(str[i])) {
                return 1;
        }
    }

    return 0;
} 

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

    int ret = execve(argv_new[0], argv_new, envp);
    free(argv_new);
    return ret;
}

int ie_execv(const char *path, char *const argv[]) {
    return ie_execve(path, argv, environ);
}

int ie_execvpe(const char *file, char *const argv[], char *const envp[])
{
	const char *p, *z, *path = getenv("PATH");
	size_t l, k;
	int seen_eacces = 0;

	errno = ENOENT;
	if (!*file) return -1;

	if (strchr(file, '/'))
		return ie_execve(file, argv, envp);

	if (!path) path = "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/bin/X11:/usr/games";
	k = strnlen(file, NAME_MAX+1);
	if (k > NAME_MAX) {
		errno = ENAMETOOLONG;
		return -1;
	}
	l = strnlen(path, PATH_MAX-1)+1;

	for(p=path; ; p=z) {
		char b[l+k+1];
		z = __strchrnul(p, ':');
		if (z-p >= l) {
			if (!*z++) break;
			continue;
		}
		memcpy(b, p, z-p);
		b[z-p] = '/';
		memcpy(b+(z-p)+(z>p), file, k+1);
		ie_execve(b, argv, envp);
		switch (errno) {
		case EACCES:
			seen_eacces = 1;
		case ENOENT:
		case ENOTDIR:
			break;
		default:
			return -1;
		}
		if (!*z++) break;
	}
	if (seen_eacces) errno = EACCES;
	return -1;
}

int ie_execvp(const char *file, char *const argv[])
{
	return ie_execvpe(file, argv, environ);
}
