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

void
nouveau_context_init(struct nouveau_context *context)
{
   context->pipe.set_debug_callback = nouveau_set_debug_callback;
}
