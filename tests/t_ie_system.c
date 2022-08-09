#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libiosexec.h>

int
main(int argc, char **argv)
{
	int err = 0;
	if (argc != 2)
		exit(1);

	err = ie_system(argv[1]);
	if (err == -1)
		fprintf(stderr, "ie_system: %s\n", strerror(errno));
	else if (WEXITSTATUS(err) == 127)
		fprintf(stderr, "ie_system: Failed to execute shell\n");

	exit(WEXITSTATUS(err));
}
