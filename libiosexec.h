#ifndef LIBIOSEXEC_H
#define LIBIOSEXEC_H

#include <spawn.h>

#define IOSEXEC_PUBLIC __attribute__ ((visibility ("default")))
#define IOSEXEC_HIDDEN __attribute__ ((visibility ("hidden")))

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

IOSEXEC_PUBLIC int ie_execl(const char* path, const char* arg0, ...);
IOSEXEC_PUBLIC int ie_execle(const char* path, const char* arg0, ...);
IOSEXEC_PUBLIC int ie_execlp(const char* file, const char* arg0, ...);

IOSEXEC_PUBLIC int ie_execv(const char* path, char *const argv[]);
IOSEXEC_PUBLIC int ie_execvp(const char* file, char* const argv[]);
IOSEXEC_PUBLIC int ie_execvpe(const char* file, char* const argv[], char* const envp[]);
IOSEXEC_PUBLIC int ie_execve(const char* path, char* const argv[], char* const envp[]); 
IOSEXEC_PUBLIC int ie_posix_spawn(pid_t *pid, const char *path, const posix_spawn_file_actions_t *file_actions, const posix_spawnattr_t *attrp, char *const argv[], char *const envp[]);
IOSEXEC_PUBLIC int ie_posix_spawnp(pid_t *pid, const char *name, const posix_spawn_file_actions_t *file_actions, const posix_spawnattr_t *attrp, char *const argv[], char *const envp[]);

#ifdef LIBIOSEXEC_INTERNAL
IOSEXEC_HIDDEN char** get_new_argv(const char* path, char* const argv[]);
IOSEXEC_HIDDEN void free_new_argv(char** argv);
// PATH_MAX for Darwin
#define PATH_MAX 1024
#define DEFAULT_PATH "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/bin/X11:/usr/games";
#define DEFAULT_SHELL "/bin/sh"
#define SHEBANG_REDIRECT_PATH "/private/preboot/procursus/usr"
#endif // LIBIOSEXEC_INTERNAL

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#  if TARGET_OS_IPHONE
#    ifndef LIBIOSEXEC_INTERNAL
#      define execl ie_execl
#      define execle ie_execle
#      define execlp ie_execlp
#      define execv ie_execv
#      define execvp ie_execvp
#      define execvpe ie_execvpe
#      define execve ie_execve
#      define posix_spawn ie_posix_spawn
#      define posix_spawnp ie_posix_spawnp
#      define getpwent ie_getpwent
#      define getpwuid ie_getpwuid
#      define getpwuid_r ie_getpwuid_r
#      define getpwnam ie_getpwnam
#      define getpwnam_r ie_getpwnam_r
#      define setpassent ie_setpassent
#      define setpwent ie_setpwent
#      define endpwent ie_endpwent
#      define getgrent ie_getgrent
#      define getgrgid ie_getgrgid
#      define getgrgid_r ie_getgrgid_r
#      define getgrnam ie_getgrnam
#      define getgrnam_r ie_getgrnam_r
#      define setgrent ie_setgrent
#      define endgrent ie_endgrent
#    endif // LIBIOSEXEC_INTERNAL
#  endif // TARGET_OS_IPHONE
#endif // __APPLE__
  
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LIBIOSEXEC_H
