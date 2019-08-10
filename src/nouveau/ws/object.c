#include "priv.h"

#include <nvif/class.h>
#include <nvif/ioctl.h>

static int
nouveau_ws_object_ioctl(struct nouveau_ws_object *obj,
                        void *data,
                        uint32_t size)
{
   assert(obj);
   union {
      struct nvif_ioctl_v0 v0;
   } *args = data;
   struct nouveau_ws_object_priv *priv = object_priv(obj);
   uint32_t argc = size;

   if (object_priv(obj)->abi) {
      args->v0.route = 0xff;
      args->v0.token = obj->handle;
   } else {
      args->v0.owner = NVIF_IOCTL_V0_OWNER_ANY;
      args->v0.route = 0x00;

      if (obj->parent)
         args->v0.object = (uint64_t)obj;
      else
         args->v0.object = 0;
   }

   return drmCommandWriteRead(priv->fd, DRM_NOUVEAU_NVIF, args, argc);
}

int
nouveau_ws_object_new_root(struct nouveau_ws_drm *drm,
                           struct nouveau_ws_object **obj)
{
   struct nouveau_ws_object_priv *priv = CALLOC_STRUCT(nouveau_ws_object_priv);
   if (!priv)
      return -ENOMEM;

   priv->fd = drm->fd;
   *obj = &priv->base;
   return 0;
}

int
nouveau_ws_object_new(struct nouveau_ws_object *parent,
                      uint64_t handle,
                      uint32_t oclass,
                      void *data,
                      uint32_t size,
                      struct nouveau_ws_object **obj)
{
   assert(obj);
   assert(parent);

   struct {
      struct nvif_ioctl_v0 ioctl;
      struct nvif_ioctl_new_v0 new;
   } *args;
   int (*func)(struct nouveau_ws_object *);
   uint32_t argc = sizeof(*args) + size;

   struct nouveau_ws_object_priv *priv = CALLOC_STRUCT(nouveau_ws_object_priv);
   if (!priv)
      return -ENOMEM;

   priv->base.parent = parent;
   priv->base.handle = handle;
   priv->base.oclass = oclass;
   priv->fd = object_priv(parent)->fd;

   int ret;
   priv->abi = nouveau_ws_abi16_object(&priv->base, &func);
   if (!priv->abi) {
      args = CALLOC(1, argc);
      if (!args) {
         FREE(priv);
         return -ENOMEM;
      }

      args->ioctl.version = 0;
      args->ioctl.type = NVIF_IOCTL_V0_NEW;
      args->new.version = 0;
      args->new.route = NVIF_IOCTL_V0_ROUTE_NVIF;
      args->new.token = (unsigned long)(void *)priv;
      args->new.object = (unsigned long)(void *)priv;
      args->new.handle = handle;
      args->new.oclass = oclass;
      memcpy(args->new.data, data, size);
      ret = nouveau_ws_object_ioctl(parent, args, argc);
      memcpy(data, args->new.data, size);
      FREE(args);
   } else {
      assert(data);
      assert(func);
      assert(size);
      priv->base.data = CALLOC(1, size);
      if (!priv->base.data)
         return -ENOMEM;
      memcpy(priv->base.data, data, size);
      ret = func(&priv->base);
   }

   *obj = &priv->base;
   if (ret)
      nouveau_ws_object_del(obj);
   return ret;
}

void
nouveau_ws_object_del(struct nouveau_ws_object **obj)
{
   assert(obj);
   if (!*obj)
      return;

   struct {
      struct nvif_ioctl_v0 ioctl;
      struct nvif_ioctl_del del;
   } args = {
      .ioctl.type = NVIF_IOCTL_V0_DEL,
   };

   nouveau_ws_object_ioctl(*obj, &args, sizeof(args));
   FREE(*obj);
   *obj = NULL;
}

int
nouveau_ws_object_mclass(struct nouveau_ws_object *obj,
                         const struct nouveau_mclass *mclass)
{
   assert(false);
   return -1;
}

int
nouveau_ws_object_mthd(struct nouveau_ws_object *obj,
                       uint32_t mthd,
                       void *data,
                       uint32_t size)
{
   assert(obj);
   struct {
      struct nvif_ioctl_v0 ioctl;
      struct nvif_ioctl_mthd_v0 mthd;
   } *args;
   uint32_t argc = sizeof(*args) + size;
   args = CALLOC(1, argc);
   if (!args)
      return -ENOMEM;

   args->ioctl.version = 0;
   args->ioctl.type = NVIF_IOCTL_V0_MTHD;
   args->mthd.version = 0;
   args->mthd.method = mthd;

   memcpy(args->mthd.data, data, size);
   int ret = nouveau_ws_object_ioctl(obj, args, argc);
   memcpy(data, args->mthd.data, size);
   FREE(args);
   return ret;
}
