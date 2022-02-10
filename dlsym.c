#include <dlfcn.h>
#include <string.h>
#include "libiosexec.h"

static char *handled_symbols[] = {
	"execl",
	"execle",
	"execlp",
	"execv",
	"execvp",
	"execvpe",
	"posix_spawn",
	"posix_spawnp",
	"uname"
};

void *
ie_dlsym(void* handle, const char* symbol)
{
	char* final_symbol = "ie_";

	for (int i = 0; i < 9; i++) {
		if (strcmp(handled_symbols[i], symbol) == 0) {
			strcat(final_symbol, symbol);
		}
	}
	if (strcmp("ie_", final_symbol) == 0) {
		return dlsym(handle, symbol);
	}
	return dlsym(handle, final_symbol);
}
