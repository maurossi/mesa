#include "symbol_cache.h"

#define DECL_CACHE(x) \
   struct winsys_cache x ## _WINSYS_CACHE = { \
      .hash_mutex = _MTX_INITIALIZER_NP, \
   }

DECL_CACHE(NOUVEAU);
DECL_CACHE(RADEON);
