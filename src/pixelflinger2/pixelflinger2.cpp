/**
 **
 ** Copyright 2010, The Android Open Source Project
 **
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 **
 **     http://www.apache.org/licenses/LICENSE-2.0
 **
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 */

#include "pixelflinger2.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "src/talloc/hieralloc.h"

void gglError(unsigned error)
{
   assert(0);
}

static void DepthRangef(GGLInterface * iface, GLclampf zNear, GLclampf zFar)
{
   GGL_GET_CONTEXT(ctx, iface);
   ctx->viewport.n = VectorComp_t_CTR((zNear + zFar) / 2);
   ctx->viewport.f = VectorComp_t_CTR((zFar - zNear) / 2);
}

static void Viewport(GGLInterface * iface, GLint x, GLint y, GLsizei width, GLsizei height)
{
   GGL_GET_CONTEXT(ctx, iface);
   ctx->viewport.x = VectorComp_t_CTR(x + width / 2);
   ctx->viewport.y = VectorComp_t_CTR(y + height / 2);
   ctx->viewport.w = VectorComp_t_CTR(width / 2);
   ctx->viewport.h = VectorComp_t_CTR(height / 2);
}

static void CullFace(GGLInterface * iface, GLenum mode)
{
   GGL_GET_CONTEXT(ctx, iface);
   if (GL_FRONT > mode || GL_FRONT_AND_BACK < mode)
      gglError(GL_INVALID_ENUM);
   else
      ctx->cullState.cullFace = mode - GL_FRONT;
}

static void FrontFace(GGLInterface * iface, GLenum mode)
{
   GGL_GET_CONTEXT(ctx, iface);
   if (GL_CW > mode || GL_CCW < mode)
      gglError(GL_INVALID_ENUM);
   else
      ctx->cullState.frontFace = mode - GL_CW;
}

static void BlendColor(GGLInterface * iface, GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
   GGL_GET_CONTEXT(ctx, iface);
   ctx->blendState.color[0] = MIN2(MAX2(red * 255, 0.0f), 255.0f);
   ctx->blendState.color[1] = MIN2(MAX2(green * 255, 0.0f), 255.0f);
   ctx->blendState.color[2] = MIN2(MAX2(blue * 255, 0.0f), 255.0f);
   ctx->blendState.color[3] = MIN2(MAX2(alpha * 255, 0.0f), 255.0f);
   SetShaderVerifyFunctions(iface);
}

static void BlendEquationSeparate(GGLInterface * iface, GLenum modeRGB, GLenum modeAlpha)
{
   GGL_GET_CONTEXT(ctx, iface);
   if (GL_FUNC_ADD != modeRGB && (GL_FUNC_SUBTRACT > modeRGB ||
                                  GL_FUNC_REVERSE_SUBTRACT < modeRGB))
      return gglError(GL_INVALID_ENUM);
   if (GL_FUNC_ADD != modeRGB && (GL_FUNC_SUBTRACT > modeRGB ||
                                  GL_FUNC_REVERSE_SUBTRACT < modeRGB))
      return gglError(GL_INVALID_ENUM);
   ctx->blendState.ce = modeRGB - GL_FUNC_ADD;
   ctx->blendState.ae = modeAlpha - GL_FUNC_ADD;
   SetShaderVerifyFunctions(iface);
}

static void BlendFuncSeparate(GGLInterface * iface, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
   GGL_GET_CONTEXT(ctx, iface);
   if (GL_ZERO != srcRGB && GL_ONE != srcRGB &&
         (GL_SRC_COLOR > srcRGB || GL_SRC_ALPHA_SATURATE < srcRGB) &&
         (GL_CONSTANT_COLOR > srcRGB || GL_ONE_MINUS_CONSTANT_ALPHA < srcRGB))
      return gglError(GL_INVALID_ENUM);
   if (GL_ZERO != srcAlpha && GL_ONE != srcAlpha &&
         (GL_SRC_COLOR > srcAlpha || GL_SRC_ALPHA_SATURATE < srcAlpha) &&
         (GL_CONSTANT_COLOR > dstRGB || GL_ONE_MINUS_CONSTANT_ALPHA < dstRGB))
      return gglError(GL_INVALID_ENUM);
   if (GL_ZERO != dstRGB && GL_ONE != dstRGB &&
         (GL_SRC_COLOR > dstRGB || GL_ONE_MINUS_DST_COLOR < dstRGB) && // GL_SRC_ALPHA_SATURATE only for source
         (GL_CONSTANT_COLOR > dstRGB || GL_ONE_MINUS_CONSTANT_ALPHA < dstRGB))
      return gglError(GL_INVALID_ENUM);
   if (GL_ZERO != dstAlpha && GL_ONE != dstAlpha &&
         (GL_SRC_COLOR > dstAlpha || GL_ONE_MINUS_DST_COLOR < dstAlpha) &&
         (GL_CONSTANT_COLOR > dstRGB || GL_ONE_MINUS_CONSTANT_ALPHA < dstRGB))
      return gglError(GL_INVALID_ENUM);
   if (srcAlpha == GL_SRC_ALPHA_SATURATE) // it's just 1 instead of min(sa, 1 - da) for alpha channel
      srcAlpha = GL_ONE;
   // in c++ it's templated function for color and alpha,
   // so it requires setting srcAlpha to GL_ONE to run template again only for alpha
   ctx->blendState.scf = srcRGB <= GL_ONE ? srcRGB :
                         (srcRGB <= GL_SRC_ALPHA_SATURATE ? srcRGB - GL_SRC_COLOR + 2
                          : srcRGB - GL_CONSTANT_COLOR + 11);

   ctx->blendState.saf = srcAlpha <= GL_ONE ? srcAlpha :
                         (srcAlpha <= GL_SRC_ALPHA_SATURATE ? srcAlpha - GL_SRC_COLOR + 2
                          : srcAlpha - GL_CONSTANT_COLOR + 11);

   ctx->blendState.dcf = dstRGB <= GL_ONE ? dstRGB :
                         (dstRGB <= GL_SRC_ALPHA_SATURATE ? dstRGB - GL_SRC_COLOR + 2
                          : dstRGB - GL_CONSTANT_COLOR + 11);

   ctx->blendState.daf = dstAlpha <= GL_ONE ? dstAlpha :
                         (dstAlpha <= GL_SRC_ALPHA_SATURATE ? dstAlpha - GL_SRC_COLOR + 2
                          : dstAlpha - GL_CONSTANT_COLOR + 11);

   SetShaderVerifyFunctions(iface);

}

static void EnableDisable(GGLInterface * iface, GLenum cap, GLboolean enable)
{
   GGL_GET_CONTEXT(ctx, iface);
   bool changed = false;
   switch (cap) {
   case GL_BLEND:
      changed |= ctx->blendState.enable ^ enable;
      ctx->blendState.enable = enable;
      break;
   case GL_CULL_FACE:
      changed |= ctx->cullState.enable ^ enable;
      ctx->cullState.enable = enable;
      break;
   case GL_DEPTH_TEST:
      changed |= ctx->bufferState.depthTest ^ enable;
      ctx->bufferState.depthTest = enable;
      break;
   case GL_STENCIL_TEST:
      changed |= ctx->bufferState.stencilTest ^ enable;
      ctx->bufferState.stencilTest = enable;
      break;
   default:
      gglError(GL_INVALID_ENUM);
      break;
   }
   if (changed)
      SetShaderVerifyFunctions(iface);
}

static void InitializeGGLState(GGLInterface * iface)
{
   iface->DepthRangef = DepthRangef;
   iface->Viewport = Viewport;
   iface->CullFace = CullFace;
   iface->FrontFace = FrontFace;
   iface->BlendColor = BlendColor;
   iface->BlendEquationSeparate = BlendEquationSeparate;
   iface->BlendFuncSeparate = BlendFuncSeparate;
   iface->EnableDisable = EnableDisable;

   InitializeBufferFunctions(iface);
   InitializeRasterFunctions(iface);
   InitializeScanLineFunctions(iface);
   InitializeShaderFunctions(iface);
   InitializeTextureFunctions(iface);

   iface->EnableDisable(iface, GL_DEPTH_TEST, false);
   iface->DepthFunc(iface, GL_LESS);
   iface->ClearColor(iface, 0, 0, 0, 0);
   iface->ClearDepthf(iface, 1.0f);

   iface->EnableDisable(iface, GL_STENCIL_TEST, false);
   iface->StencilFuncSeparate(iface, GL_FRONT_AND_BACK, GL_ALWAYS, 0, 0xff);
   iface->StencilOpSeparate(iface, GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_KEEP);

   iface->FrontFace(iface, GL_CCW);
   iface->CullFace(iface, GL_BACK);
   iface->EnableDisable(iface, GL_CULL_FACE, false);

   iface->EnableDisable(iface, GL_BLEND, false);
   iface->BlendColor(iface, 0, 0, 0, 0);
   iface->BlendEquationSeparate(iface, GL_FUNC_ADD, GL_FUNC_ADD);
   iface->BlendFuncSeparate(iface, GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);

   for (unsigned i = 0; i < GGL_MAXCOMBINEDTEXTUREIMAGEUNITS; i++)
      iface->SetSampler(iface, i, NULL);

   SetShaderVerifyFunctions(iface);
}

GGLInterface * CreateGGLInterface()
{
   //GGLContext * ctx = (GGLContext *)calloc(1, sizeof(GGLContext) + sizeof(__GLcontextRec));
   GGLContext * ctx = (GGLContext *)calloc(1, sizeof(GGLContext));
   if (!ctx)
      return NULL;
   assert((void *)ctx == (void *)&ctx->interface);
   //ctx->glCtx = (GLcontext *)((char *)ctx + sizeof(GGLContext));

   //_glapi_set_context(ctx->glCtx);
   //_mesa_init_constants(&Const);

   puts("InitializeGGLState");
   InitializeGGLState(&ctx->interface);
   return &ctx->interface;
}

void DestroyGGLInterface(GGLInterface * iface)
{
   GGLContext * ctx = (GGLContext *)iface;
   assert((void *)ctx == (void *)iface);

   DestroyShaderFunctions(iface);

   ctx->glCtx = NULL;

   free(ctx);
   
#if USE_LLVM_TEXTURE_SAMPLER
   puts("USE_LLVM_TEXTURE_SAMPLER");
#endif
#if USE_LLVM_SCANLINE
   puts("USE_LLVM_SCANLINE");
#endif
#if USE_LLVM_EXECUTIONENGINE
   puts("USE_LLVM_EXECUTIONENGINE");
#endif
   hieralloc_report_brief(NULL, stdout);
}