# HOW TO USE

### define the symbols:
```
#include <libiosexec.h>
#define execl ie_execl
#define execle ie_execle
#define execlp ie_execlp
#define execv ie_execv
#define execvp ie_execvp
#define execvpe ie_execvpe
#define execve ie_execve
```

### alias the symbols with ld: `-alias,_ie_execve,_execve`
