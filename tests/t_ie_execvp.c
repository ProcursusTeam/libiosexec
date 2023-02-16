#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libiosexec.h>

int
main(int argc, char **argv)
{
	int err;
	if (argc != 2)
		return errno;

	char *args[2] = { basename(argv[1]), NULL };

	err = ie_execvp(args[0], args);
	fprintf(stderr, "ie_execvp: %s\n", strerror(errno));
	return errno;
}
