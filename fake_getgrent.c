#include <grp.h>

#include "libiosexec.h"

struct group *
ie_getgrgid(gid_t gid) {
	return getgrgid(gid);
}

int
ie_getgrgid_r(gid_t gid, struct group *grp, char *buffer, size_t bufsize, struct group **result) {
	return getgrgid_r(gid, grp, buffer, bufsize, result);
}

struct group *
ie_getgrnam(const char *name) {
	return getgrnam(name);
}

int
ie_getgrnam_r(const char *name, struct group *grp, char *buffer, size_t bufsize, struct group **result) {
	return getgrnam_r(name, grp, buffer, bufsize, result);
}

struct group *
ie_getgrent(void) {
	return getgrent();
}

void
ie_setgrent(void) {
	return setgrent();
}

void
ie_endgrent(void) {
	return endgrent();
}

char *
ie_group_from_gid(gid_t group, int nogroup) {
	return group_from_gid(group, nogroup);
}
