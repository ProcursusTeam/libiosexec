int ie_execl(const char* path, const char* arg0, ...);
int ie_execle(const char* path, const char* arg0, ...);
int ie_execlp(const char* file, const char* arg0, ...);

int ie_execv(const char* path, char *const argv[]);
int ie_execvp(const char* file, char* const argv[]);
int ie_execvpe(const char* file, char* const argv[], char* const envp[]);
int ie_execve(const char* path, char* const argv[], char* const envp[]); 
