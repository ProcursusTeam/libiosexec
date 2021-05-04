# HOW TO USE

### define the symbols:
```
/* wrapper functions to make iOS shell scripts function correctly */
#if defined(__APPLE__)
  #include <TargetConditionals.h>
  #if TARGET_OS_IPHONE
    #define ENABLE_IOSEXEC
    #include <libiosexec.h>
  #endif
#endif
```
