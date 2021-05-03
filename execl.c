#include <stdlib.h>
#include "libiosexec.h"
#include <stdarg.h>

int ie_execl(const char *path, const char *argv0, ...) {
	int argc;
	va_list ap;
	va_start(ap, argv0);
	for (argc=1; va_arg(ap, const char *); argc++);
	va_end(ap);
	{
		int i;
		char *argv[argc+1];
		va_start(ap, argv0);
		argv[0] = (char *)argv0;

		for (i=1; i<argc; i++)
			argv[i] = va_arg(ap, char *);

		argv[i] = NULL;
		va_end(ap);
		return ie_execv(path, argv);
	}
}

int ie_execle(const char *path, const char *argv0, ...)
{
	int argc;
	va_list ap;
	va_start(ap, argv0);
	for (argc=1; va_arg(ap, const char *); argc++);
	va_end(ap);
	{
		int i;
		char *argv[argc+1];
		char **envp;
		va_start(ap, argv0);
		argv[0] = (char *)argv0;
		for (i=1; i<=argc; i++)
			argv[i] = va_arg(ap, char *);
		envp = va_arg(ap, char **);
		va_end(ap);
		return ie_execve(path, argv, envp);
	}
}

int ie_execlp(const char *file, const char *argv0, ...) {
	int argc;
	va_list ap;
	va_start(ap, argv0);
	for (argc=1; va_arg(ap, const char *); argc++);
	va_end(ap);
	{
		int i;
		char *argv[argc+1];
		va_start(ap, argv0);
		argv[0] = (char *)argv0;
		for (i=1; i<argc; i++)
			argv[i] = va_arg(ap, char *);
		argv[i] = NULL;
		va_end(ap);
		return ie_execvp(file, argv);
	}
}
