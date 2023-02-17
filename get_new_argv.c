#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libiosexec.h"
#include "libiosexec_private.h"

#include "utils.h"

// Stolen from xnu/bsd/kern/kern_exec.c
#define IS_WHITESPACE(ch) ((ch == ' ') || (ch == '\t'))
#define IS_EOL(ch) ((ch == '#') || (ch == '\n') || (ch == '\0'))

char **
get_new_argv(const char *path, char *const argv[])
{
	int argc = 0, interpc = 0;
	char **newargv = NULL;
	char **interpargv = NULL;
	char interpbuf[512] = {0};
	int fd = 0;
	ssize_t sz = 0;
	char *startp = NULL, *endp = NULL;

	if ((fd = open(path, O_RDONLY)) == -1)
		return NULL;

	while (argv[argc] != NULL)
		argc++;

	if ((sz = read(fd, interpbuf, 2)) == 2) {
		if (interpbuf[0] == '#' && interpbuf[1] == '!') {
			if ((sz = read(fd, interpbuf, 510)) == -1) {
				errno = ENOEXEC; /* If we fail to read we should bail */
				return NULL;
			}
			/* Find start and end pointer first */
			startp = interpbuf;
			for (int i = 0; i < 510; i++, startp++) {
				if (IS_EOL(*startp)) {
					errno = ENOEXEC; /* No shebang!? */
					return NULL;
				} else if (IS_WHITESPACE(*startp)) {
					/* Keep looking */
				} else {
					break; /* Found start! */
				}
			}
			endp = startp;
			for (int i = 0; i < 510; i++, endp++) {
				if (IS_EOL(*endp)) {
					break; /* Found end */
				}
			}
			if (!IS_EOL(*endp)) {
				errno = ENOEXEC;
				return NULL;
			}
			/* Walk endp back to the first non-whitespace */
			while (IS_EOL(*endp) || IS_WHITESPACE(*endp))
				endp--;
			endp++; /* Character after the last valid char */
			*endp = '\0';
			while (startp) {
				char *ch = startp;
				while (*ch && !IS_WHITESPACE(*ch))
					ch++;
				if (*ch == '\0') {
					interpc++;
					interpargv = realloc(interpargv, interpc * sizeof(char *));
					interpargv[interpc - 1] = strdup(startp);
					startp = NULL;
				} else {
					*ch = '\0';
					interpc++;
					interpargv = realloc(interpargv, interpc * sizeof(char *));
					interpargv[interpc - 1] = strdup(startp);
					startp = ch + 1;
					while (IS_WHITESPACE(*startp))
						startp++;
				}
			}
#if LIBIOSEXEC_PREFIXED_ROOT
			char *interp = interpargv[0];
			if (strncmp(interp, "/noredirect", strlen("/noredirect")) == 0) {
				memmove(interp, interp + strlen("/noredirect"), (strlen(interp) + 1) - strlen("/noredirect"));
			} else if (strncmp(interp, "/bin", 4) == 0 || strncmp(interp, "/usr/bin", 8) == 0) {
				char *newinterp = calloc(strlen(interp) + strlen(SHEBANG_REDIRECT_PATH) + 1, 1);
				strcat(strcat(newinterp, SHEBANG_REDIRECT_PATH), interp);
				free(interp);
				interpargv[0] = newinterp;
			}
#endif
		}
	}

	newargv = realloc(interpargv, (interpc + argc + 1) * sizeof(char *));
	newargv[interpc + argc] = NULL;
	/* i = 1 to avoid old argv[0] being passed to interpreter */
        newargv[interpc] = strdup(path);
	for (int i = 1; i < argc; i++) {
		newargv[interpc + i] = strdup(argv[i]);
	}

finish:
	close(fd);
	return newargv;
}


void free_new_argv(char** argv) {
	for (int i = 0; argv[i] != NULL; i++) {
		free(argv[i]);
		argv[i] = NULL;
	}
	free(argv);
}
