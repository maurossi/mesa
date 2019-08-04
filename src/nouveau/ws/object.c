#include "priv.h"

int
nouveau_ws_object_new(struct nouveau_ws_object *parent,
                      uint64_t handle,
                      uint32_t oclass,
                      void *data,
                      uint32_t length,
                      struct nouveau_ws_object **obj)
{
   assert(parent && obj);

   struct nouveau_ws_object_priv *priv = CALLOC_STRUCT(nouveau_ws_object_priv);
   if (!priv)
      return -1;

   struct nouveau_ws_object_priv *ppar = nouveau_ws_object_priv(parent);
   int ret = nouveau_object_new(ppar->object, handle, oclass, data, length, &priv->object);
   if (ret) {
      FREE(priv);
      return ret;
   }
   *obj = &priv->base;
   assert(priv->object->parent == ppar->object);
   priv->base.parent = parent;
   priv->base.handle = priv->object->handle;
   priv->base.oclass = priv->object->oclass;
   return 0;
}

void
nouveau_ws_object_del(struct nouveau_ws_object **obj)
{
   assert(obj);
   if (!*obj)
      return;

   struct nouveau_ws_object_priv *priv = nouveau_ws_object_priv(*obj);
   nouveau_object_del(&priv->object);
   *obj = NULL;
   FREE(priv);
}

int
nouveau_ws_object_mclass(struct nouveau_ws_object *obj,
                         const struct nouveau_mclass *mclass)
{
   assert(false);
   return -1;
}

struct nouveau_ws_object*
nouveau_ws_from_object(struct nouveau_object *obj,
                       struct nouveau_ws_object *parent)
{
   if (parent)
      assert(nouveau_ws_object_priv(parent)->object == obj->parent);
   else
      assert(!obj->parent);
   assert(!obj->data);

   struct nouveau_ws_object_priv *priv = CALLOC_STRUCT(nouveau_ws_object_priv);
   priv->base.parent = parent;
   priv->base.handle = obj->handle;
   priv->base.oclass = obj->oclass;
   priv->object = obj;
   return &priv->base;
}
