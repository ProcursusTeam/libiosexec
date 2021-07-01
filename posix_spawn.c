#include <spawn.h>
#include <stdlib.h>
#include <errno.h>
#include "libiosexec.h"

int ie_posix_spawn(pid_t *restrict pid, const char *restrict path,
                       const posix_spawn_file_actions_t *restrict file_actions,
                       const posix_spawnattr_t *restrict attrp,
                       char *const argv[restrict],
                       char *const envp[restrict]) {
   posix_spawn(pid, path, file_actions, attrp, argv, envp);
   int err = errno;
   if (err != EPERM && err != ENOEXEC) {
       return -1;
   }

   char** new_args = get_new_argv(path, argv);
   if (new_args == NULL) {
       return -1;
   }

   int ret = posix_spawn(pid, new_args[0], file_actions, attrp, new_args, envp);
   free(new_args);
   return ret;
}

int ie_posix_spawnp(pid_t *restrict pid, const char *restrict path,
                       const posix_spawn_file_actions_t *restrict file_actions,
                       const posix_spawnattr_t *restrict attrp,
                       char *const argv[restrict],
                       char *const envp[restrict]) {
   posix_spawnp(pid, path, file_actions, attrp, argv, envp);
   int err = errno;
   if (err != EPERM && err != ENOEXEC) {
       return -1;
   }

   char** new_args = get_new_argv(path, argv);
   if (new_args == NULL) {
       return -1;
   }

   int ret = posix_spawnp(pid, new_args[0], file_actions, attrp, new_args, envp);
   free(new_args);
   return ret;
}
