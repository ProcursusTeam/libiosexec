#include <libkern/OSCacheControl.h>

void __clear_cache(void *start, void *end) {
  // On Darwin, sys_icache_invalidate() provides __builtin___clear_cache functionality
  // __has_builtin(__builtin___clear_cache) check passes, but Apple did not provide "___clear_cache" symbol for embedded devices which normally exists in libcompiler_rt.dylib
  // Since Procursus links everything against libiosexec, adding this missing symbol here would be pretty.
  sys_icache_invalidate(start, end - start);
}
