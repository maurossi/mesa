#ifndef __NV30_SCREEN_H__
#define __NV30_SCREEN_H__

#include <stdio.h>

#include "util/list.h"

#include "nouveau_debug.h"
#include "nouveau_screen.h"
#include "nouveau_fence.h"
#include "nouveau_heap.h"
#include "nv30/nv30_winsys.h"
#include "nv30/nv30_resource.h"

struct nv30_context;

struct nv30_screen {
   struct nouveau_screen base;

   struct nv30_context *cur_ctx;

   struct nouveau_ws_bo *notify;

   struct nouveau_ws_object *ntfy;
   struct nouveau_ws_object *fence;

   struct nouveau_ws_object *query;
   struct nouveau_heap *query_heap;
   struct list_head queries;

   struct nouveau_ws_object *null;
   struct nouveau_ws_object *eng3d;
   struct nouveau_ws_object *m2mf;
   struct nouveau_ws_object *surf2d;
   struct nouveau_ws_object *swzsurf;
   struct nouveau_ws_object *sifm;

   /*XXX: nvfx state */
   struct nouveau_heap *vp_exec_heap;
   struct nouveau_heap *vp_data_heap;

   unsigned max_sample_count;
};

static inline struct nv30_screen *
nv30_screen(struct pipe_screen *pscreen)
{
   return (struct nv30_screen *)pscreen;
}

#endif
