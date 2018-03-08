#include "brw_context.h"
#include "brw_defines.h"
#include "intel_mipmap_tree.h"

void
gen9_set_astc5x5_wa_mode(struct brw_context *brw,
                         enum brw_astc5x5_wa_mode_t mode)
{
   assert(mode == BRW_ASTC5x5_WA_MODE_NONE ||
          gen9_astc5x5_wa_required(brw));

   if (mode == BRW_ASTC5x5_WA_MODE_NONE ||
       brw->astc5x5_wa_mode == mode) {
      return;
   }

   if (brw->astc5x5_wa_mode != BRW_ASTC5x5_WA_MODE_NONE) {
      const uint32_t flags = PIPE_CONTROL_CS_STALL |
         PIPE_CONTROL_TEXTURE_CACHE_INVALIDATE;
      brw_emit_pipe_control_flush(brw, flags);
   }

   brw->astc5x5_wa_mode = mode;
}
