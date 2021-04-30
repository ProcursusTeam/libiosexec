#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define STRINGS_ARE_NOT_EQUAL(x, y, len) strncmp(x, y, len)

static inline int has_non_printable_char(char* str, size_t n) {
    for (int i = 0; i < n; i++) {
        if (!isprint(str[i])) {
                return 1;
        }
    }

    return 0;
} 

int libiosexec_execve_shim(const char* path, char* const argv[], char* const envp[]) {
    execve(path, argv, envp);
    int execve_ret = errno;

    if (execve_ret != EPERM) {
        return -1;
    }

    char* first_line = NULL;
    size_t read_amount = 0;
    
    FILE* ptr = fopen(path, "r");
    if (!ptr) {
        return -1;
    }

    // If the line cannot be retrieved/is empty return the errno value/ENOEXEC
    if (getline(&first_line, &read_amount, ptr) == 0) {
        return -1;
    }

    size_t first_line_len = strlen(first_line);

    // Strip the newline off the end of the string.
    first_line[first_line_len - 1] = '\0';
    first_line_len--;

    if (STRINGS_ARE_NOT_EQUAL("#!", first_line, 2)) {
        return -1;
    }

    if (has_non_printable_char(first_line, first_line_len)) {
        return -1;
    }

    // Remove the shebang from the line, for parsing. 
    char* freeme = first_line;
    first_line += 2;
    
    char* state;
    char* token = strtok_r(first_line, " ", &state);
    char* interp = token;
    char* arg_to_interpreter = strtok_r(NULL, "", &state);

    char* argv_new[1024];
    int offset = 0; 

    argv_new[0] = interp;
    if (arg_to_interpreter != NULL) {
        argv_new[1] = arg_to_interpreter;
        offset++;
    }

    size_t argcount = 0;
    while(argv[argcount]) argcount++;

    for (int i = 0; i < argcount; i++) {
        argv_new[offset + i + 1] = argv[i];
    }
    argv_new[offset + argcount + 1] = NULL;

    int ret = execve(interp, argv_new, envp);
    free(freeme);
    return ret;
}