#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>

bool
is_shell_script(const char *path)
{
	int fd = 0;
	ssize_t sz = 0;
	char buf[2] = {0};

	if (access(path, R_OK) == -1)
		return false;

	if ((fd = open(path, O_RDONLY)) == -1)
		return false;

	if ((sz = read(fd, buf, 2)) != 2) {
		close(fd);
		return false;
	}

	if (buf[0] == '#' && buf[1] == '!') {
		close(fd);
		return true;
	}

	close(fd);
	return false;
}
