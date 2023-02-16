#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libiosexec.h>

extern char **environ;

int
main(int argc, char **argv)
{
	int err;
	if (argc != 2)
		return errno;

	char *args[2] = { basename(argv[1]), NULL };

	err = ie_execvpe(args[0], args, environ);
	fprintf(stderr, "ie_execvpe: %s\n", strerror(errno));
	return errno;
}
