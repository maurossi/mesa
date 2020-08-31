/*
 * Copyright 2020 Collabora, Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Authors (Collabora):
 *   Emil Velikov <emil.velikov@collabora.com>
 *
 */

#ifndef _SYMBOL_CACHE_H_
#define _SYMBOL_CACHE_H_

#include <dlfcn.h>
#include <stdbool.h>
#include "c11/threads.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * For use when caching/sharing of state is required across APIs.
 *
 * In particular this must work at the driver level and track if misc state
 * is initialized - be that did we call llvm init, or driver specific winsys
 * cache is present, or otherwise.
 */

/**
 * Tracks if llvm was initialized, since doing so twice brings annoying
 * messages to stdout/stderr.
 */
struct amdgpu_llvm_init
{
   bool initialized;
   mtx_t mutex;
};

struct hash_table;

/**
 * Reuse the winsys, as applicable, by having a driver specific hash table.
 */
struct winsys_cache
{
   struct hash_table *hash;
   mtx_t hash_mutex;
};

#define NOUVEAU_WINSYS_CACHE __nouveau_winsys_cache
#define RADEON_WINSYS_CACHE __radeon_winsys_cache

/* We only set the macro when it's used - in the driver winsys code */
#ifdef SYM_INSTALL_DIR

/*
 * Fetch a symbol called "name" from the global namespace. Alternatively
 * dlopen(RTLD_GLOBAL) the symbol cache/sharing DSO promoting its symbols
 * amongst which "name" must exist.
 * All this happens under a mutex as provided by the caller.
 *
 * This allows us to use RTLD_GLOBAL only for the caching DSO, instead of the
 * driver itself. The latter of which would have various dependencies which
 * may lead to issues like symbol collision et al.
 *
 * Additionally, we no longer need to export the cache symbol from each target
 * nor do we need the linker specific dynamic list. Updating the these has
 * been a hit and miss in the past.
 */
static inline int
_symbol_cache(const char *name, void **out_sym, mtx_t *mutex)
{
#define SYMBOL_DSO SYM_INSTALL_DIR "/mesa_tls.so"

   void *sym;
   int ret;

   /* Return if already fetched */
   if (*out_sym)
      return 0;

   mtx_lock(mutex);

   /* Clear any prior dlerror */
   dlerror();

   /* Look for the symbol within the global namespace with RTLD_DEFAULT */
   sym = dlsym(RTLD_DEFAULT, name);
   if (!sym) {
      /* Explicitly open the DSO with GLOBAL and NOW. This promotes all its
       * internal symbols which we rely upon with the dlsym above.
       *
       * Note: technically this also promotes the symbols of the DSO
       * dependencies. That is fine, since there are no dependencies.
       */

      void *dso = dlopen(SYMBOL_DSO, RTLD_GLOBAL | RTLD_NOW);
      if (dso)
         sym = dlsym(dso, name);
   }

   /* Clear any errors set */
   dlerror();

   *out_sym = sym;
   ret = sym != NULL ? 0 : -1;

   mtx_unlock(mutex);

   return ret;
}

/* Two level macro, to allow having x as a macro itself */
#define __str_1(x) #x
#define __str(x) __str_1(x)

#define symbol_cache(sym, lock) \
   _symbol_cache(__str(sym), (void **)&sym, lock)

#endif /* SYM_INSTALL_DIR */

#ifdef __cplusplus
}
#endif

#endif /* _SYMBOL_CACHE_H_ */
