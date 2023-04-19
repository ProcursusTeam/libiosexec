#include <unistd.h>

#include "libiosexec.h"

char *
ie_getusershell(void)
{
	return getusershell();
}

void
ie_endusershell(void)
{
	endusershell();
}

void
ie_setusershell(void)
{
	setusershell();
}
