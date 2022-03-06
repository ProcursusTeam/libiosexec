#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)

char *__strchrnul(const char *s, int c)
{
	c = (unsigned char)c;
	if (!c) return (char *)s + strlen(s);

#ifdef __GNUC__
	typedef size_t __attribute__((__may_alias__)) word;
	const word *w;
	for (; (uintptr_t)s % ALIGN; s++)
		if (!*s || *(unsigned char *)s == c) return (char *)s;
	size_t k = ONES * c;
	for (w = (void *)s; !HASZERO(*w) && !HASZERO(*w^k); w++);
	s = (void *)w;
#endif
	for (; *s && *(unsigned char *)s != c; s++);
	return (char *)s;
}

char *deduplicate_path_seperators(const char *path) {
    if (path == NULL) {
        return NULL;
    }
    char *begin = calloc(strlen(path), 1);
    char* curr = begin;
    // always copy first character
    *curr = *path;
    // if it's just a single `NUL`, we're done
    if (*curr == '\0') {
        return curr;
    }
    for (path++; *path; ++path) {
        // path points to next char to read
        // curr points to last written
        if (*curr != *path || (*curr != '\\' && *curr != '/')) {
            // only copy if not duplicate slash
            *(++curr) = *path;
        }
    }

    curr++;
    *curr = '\0';  // terminate string
    return begin;
}

bool is_shell_script(const char* path) {
    if (access(path, R_OK)) {
        return false;
    }

    FILE* ptr = fopen(path, "r");
    if (!ptr) {
        return false;
    }

    char buf[2] = {0};

    fread(buf, 1, 2, ptr);
    fclose(ptr);

    return !strncmp(buf, "#!", sizeof(buf));
}
