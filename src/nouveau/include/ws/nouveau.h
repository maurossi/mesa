#ifndef __NOUVEAU_WS_H__
#define __NOUVEAU_WS_H__

#include <stdbool.h>
#include <stdint.h>

#include <util/list.h>

#include <nouveau_drm.h>
#include <nouveau/nouveau.h>

struct nouveau_ws_client {
   struct nouveau_ws_device *device;
};

struct nouveau_ws_object {
   struct nouveau_ws_object *parent;
   uint64_t handle;
   uint32_t oclass;
   void *data;
};

struct nouveau_ws_device {
   struct nouveau_ws_drm *drm;
   struct nouveau_ws_object *object;
   uint32_t chipset;
   uint64_t vram_size;
};

struct nouveau_ws_drm {
   struct nouveau_ws_object *object;
   int fd;
   uint32_t version;
};

struct nouveau_ws_bufctx {
   struct list_head pending;
   struct list_head current;
};

struct nouveau_ws_bufref {
   struct list_head thead;
   uint32_t flags;
   uint32_t priv_data;
   void *priv;
};

struct nouveau_ws_pushbuf {
   struct nouveau_ws_client *client;
   struct nouveau_ws_object *channel;
   struct nouveau_ws_bufctx *bufctx;
   void (*kick_notify)(struct nouveau_ws_pushbuf *);
   void *user_priv;
   uint32_t *cur;
   uint32_t *end;
};

struct nouveau_ws_pushbuf_refn {
   struct nouveau_ws_bo *bo;
   uint32_t flags;
};

struct nouveau_ws_bo_config {
   uint32_t memtype;
   uint32_t tile_mode;
};

struct nouveau_ws_bo {
   uint32_t handle;
   uint64_t size;
   uint32_t flags;
   uint64_t offset;
   void *map;
   struct nouveau_ws_bo_config config;
};

int
nouveau_ws_client_new(struct nouveau_ws_device *,
                      struct nouveau_ws_client **);
void
nouveau_ws_client_del(struct nouveau_ws_client **);

int
nouveau_ws_device_new(struct nouveau_ws_drm *,
                      int32_t oclass,
                      void *data,
                      uint32_t size,
                      struct nouveau_ws_device **);
void
nouveau_ws_device_del(struct nouveau_ws_device **);

int
nouveau_ws_object_new(struct nouveau_ws_object *parent,
                      uint64_t handle,
                      uint32_t oclass,
                      void *data,
                      uint32_t length,
                      struct nouveau_ws_object **);
void
nouveau_ws_object_del(struct nouveau_ws_object **);
int
nouveau_ws_object_mclass(struct nouveau_ws_object *,
                         const struct nouveau_mclass *);

int
nouveau_ws_drm_new(int fd,
                   struct nouveau_ws_drm **);
void
nouveau_ws_drm_del(struct nouveau_ws_drm **);

int
nouveau_ws_bo_new(struct nouveau_ws_device *,
                  uint32_t flags,
                  uint32_t align,
                  uint64_t size,
                  struct nouveau_ws_bo_config *,
                  struct nouveau_ws_bo **);
int
nouveau_ws_bo_wrap(struct nouveau_ws_device *,
                   uint32_t handle,
                   struct nouveau_ws_bo **);
int
nouveau_ws_bo_name_ref(struct nouveau_ws_device *v,
                       uint32_t name,
                       struct nouveau_ws_bo **);
int
nouveau_ws_bo_name_get(struct nouveau_ws_bo *,
                       uint32_t *name);
void
nouveau_ws_bo_ref(struct nouveau_ws_bo *,
                  struct nouveau_ws_bo **);
int
nouveau_ws_bo_map(struct nouveau_ws_bo *,
                  uint32_t access,
                  struct nouveau_ws_client *);
int
nouveau_ws_bo_wait(struct nouveau_ws_bo *,
                   uint32_t access,
                   struct nouveau_ws_client *);
int
nouveau_ws_bo_prime_handle_ref(struct nouveau_ws_device *,
                               int prime_fd,
                               struct nouveau_ws_bo **);
int
nouveau_ws_bo_set_prime(struct nouveau_ws_bo *,
                        int *prime_fd);

int
nouveau_ws_bufctx_new(struct nouveau_ws_client *,
                      int bins,
                      struct nouveau_ws_bufctx **);
void
nouveau_ws_bufctx_del(struct nouveau_ws_bufctx **);
struct nouveau_ws_bufref *
nouveau_ws_bufctx_refn(struct nouveau_ws_bufctx *,
                       int bin,
                       struct nouveau_ws_bo *,
                       uint32_t flags);
struct nouveau_ws_bufref *
nouveau_ws_bufctx_mthd(struct nouveau_ws_bufctx *,
                       int bin,
                       uint32_t packet,
                       struct nouveau_ws_bo *,
                       uint64_t data,
                       uint32_t flags,
                       uint32_t vor,
                       uint32_t tor);
void
nouveau_ws_bufctx_reset(struct nouveau_ws_bufctx *,
                        int bin);

int
nouveau_ws_pushbuf_new(struct nouveau_ws_client *,
                       struct nouveau_ws_object *chan,
                       int nr,
                       uint32_t size,
                       bool immediate,
                       struct nouveau_ws_pushbuf **);
void
nouveau_ws_pushbuf_del(struct nouveau_ws_pushbuf **);
int
nouveau_ws_pushbuf_space(struct nouveau_ws_pushbuf *,
                         uint32_t dwords,
                         uint32_t relocs,
                         uint32_t pushes);
void
nouveau_ws_pushbuf_data(struct nouveau_ws_pushbuf *,
                        struct nouveau_ws_bo *,
                        uint64_t offset,
                        uint64_t length);
int
nouveau_ws_pushbuf_refn(struct nouveau_ws_pushbuf *,
                        struct nouveau_ws_pushbuf_refn *,
                        int nr);
void
nouveau_ws_pushbuf_reloc(struct nouveau_ws_pushbuf *,
                         struct nouveau_ws_bo *,
                         uint32_t data,
                         uint32_t flags,
                         uint32_t vor,
                         uint32_t tor);
int
nouveau_ws_pushbuf_validate(struct nouveau_ws_pushbuf *);
int
nouveau_ws_pushbuf_kick(struct nouveau_ws_pushbuf *,
                        struct nouveau_ws_object *chan);
struct nouveau_ws_bufctx *
nouveau_ws_pushbuf_bufctx(struct nouveau_ws_pushbuf *,
                          struct nouveau_ws_bufctx *);

int
nouveau_ws_getparam(struct nouveau_ws_device *,
                    uint64_t param,
                    uint64_t *value);

#endif
