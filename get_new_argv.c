#include <errno.h>
#include "utils.h"
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libiosexec.h"

#define STRINGS_ARE_NOT_EQUAL(x, y, len) strncmp(x, y, len)

static inline int has_non_printable_char(char* str, size_t n) {
    for (int i = 0; i < n; i++) {
        if (!isprint(str[i])) {
                return 1;
        }
    }

    return 0;
}

char** get_new_argv(const char* path, char* const argv[]) {
    char* first_line = NULL;
    size_t read_amount = 0;

    FILE* ptr = fopen(path, "r");
    if (!ptr) {
        errno = ENOENT;
        return NULL;
    }

    // If the line cannot be retrieved/is empty set errno to ENOEXEC
    if (getline(&first_line, &read_amount, ptr) == 0) {
        errno = ENOEXEC;
        return NULL;
    }

    size_t first_line_len = strlen(first_line);

    // If the line has a CRLF terminator, just move the length back by one and let the code
    // below cut both off.
    if (first_line[first_line_len - 2] == '\r') {
        first_line_len--;
    }

    // Strip the newline off the end of the string.
    first_line[first_line_len - 1] = '\0';
    first_line_len--;

    if (has_non_printable_char(first_line, first_line_len)) {
        errno = ENOEXEC;
        return NULL;
    }

    bool hasBang = 1;

    if (STRINGS_ARE_NOT_EQUAL("#!", first_line, 2)) {
        hasBang = 0;
    }

    char** argv_new = calloc(1024, 1);
    if (!argv_new) {
        errno = ENOMEM;
        return NULL;
    }

    int offset = 0;
    char* freeme = first_line;

    if (hasBang) {
        // Remove the shebang from the line, for parsing.
        first_line += 2;

        char* state;
        char* token = strtok_r(first_line, " ", &state);
        char* interp = token;
        char* arg_to_interpreter = strtok_r(NULL, "", &state);

        if (!strncmp(interp, "/bin", strlen("/bin"))) {
            char* interp_redirected = calloc(strlen(interp) + strlen(SHEBANG_REDIRECT_PATH) + 1, 1);
            strcat(strcat(interp_redirected, SHEBANG_REDIRECT_PATH), interp);
            argv_new[0] = interp_redirected;
        }

        else if (!strncmp(interp, "/usr/bin", strlen("/usr/bin"))) {
            char* interp_redirected = calloc(strlen(interp) + strlen(SHEBANG_REDIRECT_PATH) + 1, 1);
            strcat(strcat(interp_redirected, SHEBANG_REDIRECT_PATH), interp);
            argv_new[0] = interp_redirected;
        }

        else {
            argv_new[0] = strdup(interp);
        }

        if (arg_to_interpreter != NULL) {
            argv_new[1] = strdup(arg_to_interpreter);
            offset++;
        }

    } else {
        char* default_interp = calloc(strlen(SHEBANG_REDIRECT_PATH) + strlen("/bin/sh") + 1, 1);
        strcat(strcat(default_interp, SHEBANG_REDIRECT_PATH), "/bin/sh");
        argv_new[0] = default_interp;
    }

    size_t argcount = 0;
    while(argv[argcount]) argcount++;

    argv_new[1 + offset] = strdup(path);

    for (int i = 1; i < argcount; i++) {
        argv_new[offset + i + 1] = strdup(argv[i]);
    }
    argv_new[offset + argcount + 1] = NULL;

    free(freeme);
    return argv_new;
}

void free_new_argv(char** argv) {
    for(int i = 0; argv[i] != NULL; i++) {
        free(argv[i]);
    }

    free(argv);
}
