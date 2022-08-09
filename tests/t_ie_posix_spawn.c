#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spawn.h>
#include <sys/wait.h>

#include <libiosexec.h>

int
main(int argc, char **argv)
{
	int err;
	pid_t pid;
	if (argc != 2)
		exit(1);

	const char *args[2] = { argv[1], NULL };

	err = ie_posix_spawn(&pid, args[0], NULL, NULL, (char * const *)args, NULL);

	if (err != 0)
		fprintf(stderr, "ie_posix_spawn: %s", strerror(errno));

	waitpid(pid, NULL, 0);

	return err;
}
