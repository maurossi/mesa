#include "priv.h"

int
nouveau_ws_bo_new(struct nouveau_ws_device *dev,
                  uint32_t flags,
                  uint32_t align,
                  uint64_t size,
                  struct nouveau_ws_bo_config *config,
                  struct nouveau_ws_bo **bo)
{
   assert(false);
   return -1;
}

int
nouveau_ws_bo_wrap(struct nouveau_ws_device *dev,
                   uint32_t handle,
                   struct nouveau_ws_bo **bo)
{
   assert(false);
   return -1;
}

int
nouveau_ws_bo_name_ref(struct nouveau_ws_device *v,
                       uint32_t name,
                       struct nouveau_ws_bo **bo)
{
   assert(false);
   return -1;
}

int
nouveau_ws_bo_name_get(struct nouveau_ws_bo *bo,
                       uint32_t *name)
{
   assert(false);
   return -1;
}

void
nouveau_ws_bo_ref(struct nouveau_ws_bo *bo,
                  struct nouveau_ws_bo **pref)
{
   assert(false);
}

int
nouveau_ws_bo_map(struct nouveau_ws_bo *bo,
                  uint32_t access,
                  struct nouveau_ws_client *client)
{
   assert(false);
   return -1;
}

int
nouveau_ws_bo_wait(struct nouveau_ws_bo *bo,
                   uint32_t access,
                   struct nouveau_ws_client *client)
{
   assert(false);
   return -1;
}

int
nouveau_ws_bo_prime_handle_ref(struct nouveau_ws_device *dev,
                               int prime_fd,
                               struct nouveau_ws_bo **bo)
{
   assert(false);
   return -1;
}

int
nouveau_ws_bo_set_prime(struct nouveau_ws_bo *bo,
                        int *prime_fd)
{
   assert(false);
   return -1;
}
