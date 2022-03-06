#include <spawn.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>

#include "utils.h"
#include "libiosexec.h"
#include "libiosexec_private.h"

int ie_posix_spawn(pid_t *pid, const char *path,
                       const posix_spawn_file_actions_t *file_actions,
                       const posix_spawnattr_t *attrp,
                       char *const argv[],
                       char *const envp[]) {
   bool bypass_sysposix_spawn = false;
   int err;

#if LIBIOSEXEC_PREFIXED_ROOT == 1
   if (is_shell_script(path)) {
       bypass_sysposix_spawn = true;
   }
#endif

   if (!bypass_sysposix_spawn)
       err = posix_spawn(pid, path, file_actions, attrp, argv, envp);

   if (!bypass_sysposix_spawn && (err != EPERM && err != ENOEXEC)) {
       return err;
   }

   char** new_args = get_new_argv(path, argv);
   if (new_args == NULL) {
       return errno;
   }

   int ret = posix_spawn(pid, new_args[0], file_actions, attrp, new_args, envp);
   int saved_errno = errno;
   free_new_argv(new_args);
   errno = saved_errno;
   return ret;
}

int
ie_posix_spawnp(pid_t *pid, const char *file,
		const posix_spawn_file_actions_t *file_actions,
		const posix_spawnattr_t *attrp,
		char *const argv[], char *const envp[])
{
	const char *env_path;
	char *bp;
	char *cur;
	char *p;
	char **memp;
	int lp;
	int ln;
	int cnt;
	int err = 0;
	int eacces = 0;
	struct stat sb;
	char path_buf[PATH_MAX];

	if ((env_path = getenv("PATH")) == NULL)
		env_path = DEFAULT_PATH;

	/* If it's an absolute or relative path name, it's easy. */
	if (strchr(file, '/')) {
		bp = (char *)file;
		cur = NULL;
		goto retry;
	}
	bp = path_buf;

	/* If it's an empty path name, fail in the usual POSIX way. */
	if (*file == '\0')
		return (ENOENT);

	if ((cur = alloca(strlen(env_path) + 1)) == NULL)
		return ENOMEM;
	strcpy(cur, env_path);
	while ((p = strsep(&cur, ":")) != NULL) {
		/*
		 * It's a SHELL path -- double, leading and trailing colons
		 * mean the current directory.
		 */
		if (*p == '\0') {
			p = ".";
			lp = 1;
		} else {
			lp = strlen(p);
		}
		ln = strlen(file);

		/*
		 * If the path is too long complain.  This is a possible
		 * security issue; given a way to make the path too long
		 * the user may spawn the wrong program.
		 */
		if (lp + ln + 2 > sizeof(path_buf)) {
			err = ENAMETOOLONG;
			goto done;
		}
		bcopy(p, path_buf, lp);
		path_buf[lp] = '/';
		bcopy(file, path_buf + lp + 1, ln);
		path_buf[lp + ln + 1] = '\0';

retry:		err = ie_posix_spawn(pid, bp, file_actions, attrp, argv, envp);
		switch (err) {
		case E2BIG:
		case ENOMEM:
		case ETXTBSY:
			goto done;
		case ELOOP:
		case ENAMETOOLONG:
		case ENOENT:
		case ENOTDIR:
			break;
		case ENOEXEC:
			for (cnt = 0; argv[cnt]; ++cnt)
				;
			memp = alloca((cnt + 2) * sizeof(char *));
			if (memp == NULL) {
				/* errno = ENOMEM; XXX override ENOEXEC? */
				goto done;
			}
			memp[0] = "sh";
			memp[1] = bp;
			bcopy(argv + 1, memp + 2, cnt * sizeof(char *));
			err = ie_posix_spawn(pid, "/bin/sh", file_actions, attrp, memp, envp);
			goto done;
		default:
			/*
			 * EACCES may be for an inaccessible directory or
			 * a non-executable file.  Call stat() to decide
			 * which.  This also handles ambiguities for EFAULT
			 * and EIO, and undocumented errors like ESTALE.
			 * We hope that the race for a stat() is unimportant.
			 */
			if (stat(bp, &sb) != 0)
				break;
			if (err == EACCES) {
				eacces = 1;
				continue;
			}
			goto done;
		}
	}
	if (eacces)
		err = EACCES;
	else
		err = ENOENT;
done:
	return (err);
}
