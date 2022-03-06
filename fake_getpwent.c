#include <pwd.h>

#include "libiosexec.h"

struct passwd *
ie_getpwent(void) {
	return getpwent();
}

struct passwd *
ie_getpwnam(const char *name) {
	return getpwnam(name);
}

int
ie_getpwnam_r(const char *name, struct passwd *pw, char *buf, size_t buflen, struct passwd **pwretp) {
	return getpwnam_r(name, pw, buf, buflen, pwretp);
}

struct passwd *
ie_getpwuid(uid_t uid) {
	return getpwuid(uid);
}

int
ie_getpwuid_r(uid_t uid, struct passwd *pw, char *buf, size_t buflen, struct passwd **pwretp) {
	return getpwuid_r(uid, pw, buf, buflen, pwretp);
}

int
ie_setpassent(int stayopen) {
	return setpassent(stayopen);
}

void
ie_setpwent(void) {
	return setpwent();
}

void
ie_endpwent(void) {
	return endpwent();
}

char *
ie_user_from_uid(uid_t uid, int nouser) {
	return user_from_uid(uid, nouser);
}
