#include "nouveau_context.h"

static void
nouveau_set_debug_callback(struct pipe_context *pipe,
                           const struct pipe_debug_callback *cb)
{
   struct nouveau_context *context = nouveau_context(pipe);

   if (cb)
      context->debug = *cb;
   else
      memset(&context->debug, 0, sizeof(context->debug));
}

int
nouveau_context_init(struct nouveau_context *context)
{
   struct nouveau_screen *screen = context->screen;
   int ret;

   context->pipe.set_debug_callback = nouveau_set_debug_callback;

   ret = nouveau_client_new(screen->device, &context->client);
   if (ret)
      return ret;

   ret = nouveau_pushbuf_new(context->client, screen->channel,
                             4, 512 * 1024, 1,
                             &context->pushbuf);
   if (ret)
      return ret;

   nouveau_fence_list_init(&context->fence, screen, context->pushbuf);
   return ret;
}

void
nouveau_context_destroy(struct nouveau_context *ctx)
{
   int i;

   for (i = 0; i < NOUVEAU_MAX_SCRATCH_BUFS; ++i)
      if (ctx->scratch.bo[i])
         nouveau_bo_ref(NULL, &ctx->scratch.bo[i]);

   nouveau_pushbuf_del(&ctx->pushbuf);
   nouveau_client_del(&ctx->client);

   FREE(ctx);
}
