#include "libiosexec.h"
#include <stdbool.h>

IOSEXEC_HIDDEN char* deduplicate_path_seperators(const char*);
IOSEXEC_HIDDEN char *__strchrnul(const char *s, int c);
IOSEXEC_HIDDEN bool is_shell_script(const char* path);
