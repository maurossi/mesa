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
   context->pipe.set_debug_callback = nouveau_set_debug_callback;
   return nouveau_ws_pushbuf_new(context->client, screen->channel,
                                 4, 512 * 1024, 1,
                                 &context->pushbuf);
}

void
nouveau_context_destroy(struct nouveau_context *ctx)
{
   int i;

   for (i = 0; i < NOUVEAU_MAX_SCRATCH_BUFS; ++i)
      if (ctx->scratch.bo[i])
         nouveau_ws_bo_ref(NULL, &ctx->scratch.bo[i]);

   nouveau_ws_pushbuf_del(&ctx->pushbuf);
   FREE(ctx);
}
