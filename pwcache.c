/*
 * Copyright (c) 2008-2018 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
#include <stdio.h>
#include <pwd.h>
#include <grp.h>

#include "libiosexec.h"

char *
ie_user_from_uid(uid_t uid, int nouser)
{
	struct passwd *pw;
	static char buf[16];

	pw = ie_getpwuid(uid);
	if (pw != NULL) return pw->pw_name;

	if (nouser) return NULL;

	snprintf(buf, sizeof(buf), "%u", uid);
	return buf;
}

char *
ie_group_from_gid(gid_t gid, int nogroup)
{
	struct group *gr;
	static char buf[16];

	gr = ie_getgrgid(gid);
	if (gr != NULL) return gr->gr_name;

	if (nogroup) return NULL;

	snprintf(buf, sizeof(buf), "%u", gid);
	return buf;
}
