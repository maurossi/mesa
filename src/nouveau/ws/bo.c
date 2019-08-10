#include "priv.h"

#include "os/os_mman.h"
#include "util/u_atomic.h"

static void
nouveau_ws_bo_info(struct nouveau_ws_bo_priv *bo,
                   struct drm_nouveau_gem_info *info,
                   struct nouveau_ws_device *dev)
{
   bo->map_handle = info->map_handle;

   bo->base.flags = 0;
   bo->base.handle = info->handle;
   bo->base.offset = info->offset;
   bo->base.size = info->size;

   if (info->domain & NOUVEAU_GEM_DOMAIN_VRAM)
      bo->base.flags |= NOUVEAU_BO_VRAM;
   if (info->domain & NOUVEAU_GEM_DOMAIN_GART)
      bo->base.flags |= NOUVEAU_BO_GART;
   if (!(info->tile_flags & NOUVEAU_GEM_TILE_NONCONTIG))
      bo->base.flags |= NOUVEAU_BO_CONTIG;
   if (bo->map_handle)
      bo->base.flags |= NOUVEAU_BO_MAP;

   if (dev->chipset >= 0xc0) {
      bo->base.config.memtype   = (info->tile_flags & 0xff00) >> 8;
      bo->base.config.tile_mode = info->tile_mode;
   } else if (dev->chipset >= 0x50) {
      bo->base.config.memtype   = (info->tile_flags & 0x07f00) >> 8 |
                                  (info->tile_flags & 0x30000) >> 9;
      bo->base.config.tile_mode = info->tile_mode << 4;
   } else {
      assert(false);
   }
}

int
nouveau_ws_bo_new(struct nouveau_ws_device *dev,
                  uint32_t flags,
                  uint32_t align,
                  uint64_t size,
                  struct nouveau_ws_bo_config *config,
                  struct nouveau_ws_bo **bo)
{
   assert(dev && bo);

   struct nouveau_ws_bo_priv *priv = CALLOC_STRUCT(nouveau_ws_bo_priv);
   if (!priv)
      return -ENOMEM;

   struct drm_nouveau_gem_new req = {};
   struct drm_nouveau_gem_info *info = &req.info;

   priv->base.flags = flags;
   priv->base.size = size;

   if (priv->base.flags & NOUVEAU_BO_VRAM)
      info->domain |= NOUVEAU_GEM_DOMAIN_VRAM;
   if (priv->base.flags & NOUVEAU_BO_GART)
      info->domain |= NOUVEAU_GEM_DOMAIN_GART;
   if (!info->domain)
      info->domain |= NOUVEAU_GEM_DOMAIN_VRAM |
                      NOUVEAU_GEM_DOMAIN_GART;

   if (priv->base.flags & NOUVEAU_BO_MAP)
      info->domain |= NOUVEAU_GEM_DOMAIN_MAPPABLE;

   if (priv->base.flags & NOUVEAU_BO_COHERENT)
      info->domain |= NOUVEAU_GEM_DOMAIN_COHERENT;

   if (!(priv->base.flags & NOUVEAU_BO_CONTIG))
      info->tile_flags = NOUVEAU_GEM_TILE_NONCONTIG;

   info->size = priv->base.size;
   req.align = align;

   assert(!config);

   int ret = drmCommandWriteRead(dev->drm->fd, DRM_NOUVEAU_GEM_NEW, &req,
                                 sizeof(req));
   if (!ret)
      nouveau_ws_bo_info(priv, info, dev);

   p_atomic_set(&priv->refcnt, 1);
   priv->drm = dev->drm;
   *bo = &priv->base;
   return ret;
}

static void
nouveau_ws_bo_del(struct nouveau_ws_bo *bo)
{
   assert(bo);
   struct drm_gem_close req = { .handle = bo->handle };
   drmIoctl(bo_priv(bo)->drm->fd, DRM_IOCTL_GEM_CLOSE, &req);
   if (bo->map)
      os_munmap(bo->map, bo->size);
   FREE(bo);
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

   if (bo)
      p_atomic_inc(&bo_priv(bo)->refcnt);
   if (*pref) {
      if (!p_atomic_dec_return(&bo_priv(*pref)->refcnt))
         nouveau_ws_bo_del(*pref);
   }
}

int
nouveau_ws_bo_map(struct nouveau_ws_bo *bo,
                  uint32_t access,
                  struct nouveau_ws_client *client)
{
   assert(bo);

   struct nouveau_ws_bo_priv *priv = bo_priv(bo);

   if (!bo->map) {
      bo->map = os_mmap(0, bo->size, PROT_READ | PROT_WRITE, MAP_SHARED,
                        priv->drm->fd, priv->map_handle);
      if (bo->map == MAP_FAILED) {
         bo->map = NULL;
         return -errno;
      }
   }
   return nouveau_ws_bo_wait(bo, access, client);
}

// TODO: remove
int
nouveau_ws_bo_wait(struct nouveau_ws_bo *bo,
                   uint32_t access,
                   struct nouveau_ws_client *client)
{
   assert(bo);

   struct drm_nouveau_gem_cpu_prep req;

   if (!(access & NOUVEAU_BO_RDWR))
      return 0;

   req.handle = bo->handle;
   req.flags = 0;

   if (access & NOUVEAU_BO_WR)
      req.flags |= NOUVEAU_GEM_CPU_PREP_WRITE;
   if (access & NOUVEAU_BO_NOBLOCK)
      req.flags |= NOUVEAU_GEM_CPU_PREP_NOWAIT;

   return drmCommandWrite(bo_priv(bo)->drm->fd, DRM_NOUVEAU_GEM_CPU_PREP,
                          &req, sizeof(req));
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
