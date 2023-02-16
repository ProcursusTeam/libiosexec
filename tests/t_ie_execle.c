#include <errno.h>
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

	err = ie_execl(argv[1], argv[1], NULL, environ);
	fprintf(stderr, "ie_execle: %s\n", strerror(errno));
	return errno;
}
