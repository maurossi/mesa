#include "priv.h"

int
nouveau_ws_getparam(struct nouveau_ws_device *dev,
                    uint64_t param,
                    uint64_t *value)
{
   assert(dev);
   struct drm_nouveau_getparam r = {
      .param = param,
   };

   int fd = dev->drm->fd;
   int ret = drmCommandWriteRead(fd, DRM_NOUVEAU_GETPARAM, &r, sizeof(r));
   *value = r.value;
   return ret;
}
