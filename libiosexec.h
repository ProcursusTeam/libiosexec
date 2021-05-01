int ie_execl_shim(const char* path, const char* arg0, ...);
int ie_execle_shim(const char* path, const char* arg0, ...);
int ie_execlp_shim(const char* file, const char* arg0, ...);

int ie_execv_shim(const char* path, char *const argv[]);
int ie_execvp_shim(const char* file, char* const argv[]);

int ie_execve_shim(const char* path, char* const argv[], char* const envp[]); 
