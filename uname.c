#include <string.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>
#include "libiosexec.h"

int
ie_uname(name)
	struct utsname *name;
{
	int mib[2], rval;
	size_t len;
	char *p;

	rval = 0;

	mib[0] = CTL_KERN;
	mib[1] = KERN_OSTYPE;
	len = sizeof(name->sysname);
	if (sysctl(mib, 2, &name->sysname, &len, NULL, 0) == -1)
		rval = -1;

	mib[0] = CTL_KERN;
	mib[1] = KERN_HOSTNAME;
	len = sizeof(name->nodename);
	if (sysctl(mib, 2, &name->nodename, &len, NULL, 0) == -1)
		rval = -1;

	mib[0] = CTL_KERN;
	mib[1] = KERN_OSRELEASE;
	len = sizeof(name->release);
	if (sysctl(mib, 2, &name->release, &len, NULL, 0) == -1)
		rval = -1;

	/* The version may have newlines in it, turn them into spaces. */
	mib[0] = CTL_KERN;
	mib[1] = KERN_VERSION;
	len = sizeof(name->version);
	if (sysctl(mib, 2, &name->version, &len, NULL, 0) == -1)
		rval = -1;
	else
		for (p = name->version; len--; ++p)
			if (*p == '\n' || *p == '\t') {
				if (len > 1) {
					*p = ' ';
				} else {
					*p = '\0';
				}
			}
	mib[0] = CTL_HW;
#if defined(__arm64__) && defined(__APPLE__)
	char *ie_machine = "arm64";
	strcpy(&name->machine, ie_machine);
#else
	mib[1] = HW_MACHINE;
	len = sizeof(name->machine);
	if (sysctl(mib, 2, &name->machine, &len, NULL, 0) == -1)
		rval = -1;
#endif
	return (rval);
}
