#include "priv.h"

int
nouveau_ws_bo_new(struct nouveau_ws_device *dev,
                  uint32_t flags,
                  uint32_t align,
                  uint64_t size,
                  struct nouveau_ws_bo_config *config,
                  struct nouveau_ws_bo **bo)
{
   assert(bo);

   struct nouveau_ws_bo_priv *priv = CALLOC_STRUCT(nouveau_ws_bo_priv);
   if (!priv)
      return -1;

   struct nouveau_ws_device_priv *pdev = nouveau_ws_device_priv(dev);
   union nouveau_bo_config drm_config = {
      .nv50 = {
         .memtype = config ? config->memtype : 0,
         .tile_mode = config ? config->tile_mode : 0,
      },
   };
   assert(drm_config.nvc0.memtype == drm_config.nv50.memtype);
   assert(drm_config.nvc0.tile_mode == drm_config.nv50.tile_mode);

   int ret = nouveau_bo_new(pdev->device, flags, align, size, config ? &drm_config : NULL, &priv->bo);
   if (ret || !priv->bo) {
      FREE(priv);
      return ret;
   }

   *bo = &priv->base;
   priv->base.handle = priv->bo->handle;
   priv->base.offset = priv->bo->offset;
   priv->base.size = priv->bo->size;
   priv->base.flags = priv->bo->flags;
   priv->base.config.memtype = priv->bo->config.nv50.memtype;
   priv->base.config.tile_mode = priv->bo->config.nv50.tile_mode;
   priv->ref_cnt = 1;
   return 0;
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
   assert(pref);
   struct nouveau_ws_bo_priv *pbo = nouveau_ws_bo_priv(bo);
   struct nouveau_ws_bo_priv *ppref = nouveau_ws_bo_priv(*pref);
   struct nouveau_bo *drm_bo = ppref ? ppref->bo : NULL;

   if (pbo)
      pbo->ref_cnt++;

   nouveau_bo_ref(pbo ? pbo->bo : NULL, &drm_bo);
   if (ppref) {
      if (!--(ppref->ref_cnt))
         FREE(ppref);
   }
   *pref = bo;
}

int
nouveau_ws_bo_map(struct nouveau_ws_bo *bo,
                  uint32_t access,
                  struct nouveau_ws_client *client)
{
   assert(bo);
   struct nouveau_ws_bo_priv *pbo = nouveau_ws_bo_priv(bo);
   struct nouveau_ws_client_priv *pclient = nouveau_ws_client_priv(client);
   int ret = nouveau_bo_map(pbo->bo, access, pclient ? pclient->client : NULL);
   if (ret)
      return ret;
   bo->map = pbo->bo->map;
   return 0;
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
   assert(bo);
   struct nouveau_ws_bo_priv *pbo = nouveau_ws_bo_priv(bo);
   return nouveau_bo_set_prime(pbo->bo, prime_fd);
}
