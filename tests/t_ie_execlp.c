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
		exit(1);

	char *arg = basename(argv[1]);

	err = ie_execlp(arg, arg, NULL);
	fprintf(stderr, "ie_execlp: %s\n", strerror(errno));
	exit(1);
}
