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
		exit(1);

	char *args[2] = { argv[1], NULL };

	err = ie_execve(argv[1], args, environ);
	fprintf(stderr, "ie_execve: %s\n", strerror(errno));
	exit(1);
}
