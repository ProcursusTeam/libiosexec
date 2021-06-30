#include "utils.h"
#if defined (__APPLE__)
#include <sys/syslimits.h>
#elif defined __linux__
#include <linux/limits.h>
#endif
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
        return NULL;
    }

    // If the line cannot be retrieved/is empty return the errno value/ENOEXEC
    if (getline(&first_line, &read_amount, ptr) == 0) {
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
        return NULL;
    }

    bool hasBang = 1;

    if (STRINGS_ARE_NOT_EQUAL("#!", first_line, 2)) {
        hasBang = 0;
    }

    char** argv_new = calloc(1024, 1);
    if (!argv_new) {
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

        argv_new[0] = interp;
        if (arg_to_interpreter != NULL) {
            argv_new[1] = arg_to_interpreter;
            offset++;
        }
    } else {
        argv_new[0] = "/bin/sh";
    }

    size_t argcount = 0;
    while(argv[argcount]) argcount++;

    argv_new[1 + offset] = (char *)path;

    for (int i = 1; i < argcount; i++) {
        argv_new[offset + i + 1] = argv[i];
    }
    argv_new[offset + argcount + 1] = NULL;

    free(freeme);
    return argv_new;
}
