#ifndef LIBIOSEXEC_H
#define LIBIOSEXEC_H

#if defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int ie_execl(const char* path, const char* arg0, ...);
int ie_execle(const char* path, const char* arg0, ...);
int ie_execlp(const char* file, const char* arg0, ...);

int ie_execv(const char* path, char *const argv[]);
int ie_execvp(const char* file, char* const argv[]);
int ie_execvpe(const char* file, char* const argv[], char* const envp[]);
int ie_execve(const char* path, char* const argv[], char* const envp[]); 

#ifndef LIBIOSEXEC_INTERNAL
    #define execl ie_execl
    #define execle ie_execle
    #define execlp ie_execlp
    #define execv ie_execv
    #define execvp ie_execvp
    #define execvpe ie_execvpe
    #define execve ie_execve
#endif
  
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TARGET_OS_IPHONE
#endif // __APPLE__

#endif // LIBIOSEXEC_H
