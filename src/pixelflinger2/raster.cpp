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

#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "pixelflinger2.h"
#include "src/mesa/main/mtypes.h"

#ifdef SHADER_SOA
static struct tgsi_exec_machine machine;
#endif

static inline void LerpVector4(const Vector4 * a, const Vector4 * b,
                               const VectorComp_t x, Vector4 * d) __attribute__((always_inline));
static inline void LerpVector4(const Vector4 * a, const Vector4 * b,
                               const VectorComp_t x, Vector4 * d)
{
   assert(a != d && b != d);
   //d = (b - a) * x + a;
   (*d) = (*b);
   (*d) -= (*a);
   (*d) *= x;
   (*d) += (*a);
}

static inline void InterpolateVertex(const VertexOutput * a, const VertexOutput * b, const VectorComp_t x,
                                     VertexOutput * v, const unsigned varyingCount)
{
   LerpVector4(&a->position, &b->position, x, &v->position);
   for (unsigned i = 0; i < varyingCount; i++)
      LerpVector4(a->varyings + i, b->varyings + i, x, v->varyings + i);
   LerpVector4(&a->frontFacingPointCoord, &b->frontFacingPointCoord,
               x, &v->frontFacingPointCoord); // gl_PointCoord
   v->frontFacingPointCoord.y = a->frontFacingPointCoord.y; // gl_FrontFacing not interpolated

}

void GGLProcessVertex(const gl_shader_program * program, const VertexInput * input,
                      VertexOutput * output, const float (*constants)[4])
{
   ShaderFunction_t function = (ShaderFunction_t)program->_LinkedShaders[MESA_SHADER_VERTEX]->function;
   function(input, output, constants);
}

static void ProcessVertex(const GGLInterface * iface, const VertexInput * input,
                          VertexOutput * output)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);

//#if !USE_LLVM_TEXTURE_SAMPLER
//    extern const GGLContext * textureGGLContext;
//    textureGGLContext = ctx;
//#endif
//

//   memcpy(ctx->glCtx->CurrentProgram->ValuesVertexInput, input, sizeof(*input));
//   ctx->glCtx->CurrentProgram->_LinkedShaders[MESA_SHADER_VERTEX]->function();
//   memcpy(output, ctx->glCtx->CurrentProgram->ValuesVertexOutput, sizeof(*output));

   GGLProcessVertex(ctx->CurrentProgram, input, output, ctx->CurrentProgram->ValuesUniform);
//   const Vector4 * constants = (Vector4 *)
//    ctx->glCtx->Shader.CurrentProgram->VertexProgram->Parameters->ParameterValues;
//	ctx->glCtx->Shader.CurrentProgram->GLVMVP->function(input, output, constants);
//
//#if !USE_LLVM_TEXTURE_SAMPLER
//    textureGGLContext = NULL;
//#endif
}

#include <pthread.h>

struct WorkerArgs {
   const GGLInterface * iface;
   unsigned startY, endY, varyingCount;
   VertexOutput bV, cV, bDx, cDx;
   int width, height;
   volatile bool hasWork;
   bool quit;

   static void * RasterTrapezoidWorker(void * threadArgs) {
      WorkerArgs * args = (WorkerArgs *)threadArgs;
      VertexOutput clip0, clip1, * left, * right;
      while (!args->quit) {
         if (!args->hasWork)
            continue;
         for (unsigned y = args->startY; y <= args->endY; y += 2) {
            do {
               if (args->bV.position.x < 0) {
                  if (args->cV.position.x < 0)
                     break;
                  InterpolateVertex(&args->bV, &args->cV, -args->bV.position.x /
                                    (args->cV.position.x - args->bV.position.x),
                                    &clip0, args->varyingCount);
                  left = &clip0;
               } else
                  left = &args->bV;
               if ((int)args->cV.position.x >= (int)args->width) {
                  if (args->bV.position.x >= (int)args->width)
                     break;
                  InterpolateVertex(&args->bV, &args->cV, (args->width - 1 - args->bV.position.x) /
                                    (args->cV.position.x - args->bV.position.x),
                                    &clip1, args->varyingCount);
                  right = &clip1;
               } else
                  right = &args->cV;
               args->iface->ScanLine(args->iface, left, right);
            } while (false);
            for (unsigned i = 0; i < args->varyingCount; i++) {
               args->bV.varyings[i] += args->bDx.varyings[i];
               args->cV.varyings[i] += args->cDx.varyings[i];
            }
            args->bV.position += args->bDx.position;
            args->cV.position += args->cDx.position;
            args->bV.frontFacingPointCoord += args->bDx.frontFacingPointCoord;
            args->cV.frontFacingPointCoord += args->cDx.frontFacingPointCoord;
         }
         args->hasWork = false;
      }
      pthread_exit(NULL);
      return NULL;
   }
};

static void RasterTrapezoid(const GGLInterface * iface, const VertexOutput * tl,
                            const VertexOutput * tr, const VertexOutput * bl,
                            const VertexOutput * br)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);

   assert(tl->position.x <= tr->position.x && bl->position.x <= br->position.x);
   assert(tl->position.y <= bl->position.y && tr->position.y <= br->position.y);
   assert(fabs(tl->position.y - tr->position.y) < 1 && fabs(bl->position.y - br->position.y) < 1);

   const unsigned width = ctx->frameSurface.width, height = ctx->frameSurface.height;
   const unsigned varyingCount = ctx->CurrentProgram->VaryingSlots;


   // tlv-trv and blv-brv are parallel and horizontal
   VertexOutput tlv(*tl), trv(*tr), blv(*bl), brv(*br);
   VertexOutput tmp;

   // vertically clip
   if ((int)tlv.position.y < 0) {
      InterpolateVertex(&tlv, &blv, (0 - tlv.position.y) / (blv.position.y - tlv.position.y),
                        &tmp, varyingCount);
      tlv = tmp;
   }
   if ((int)trv.position.y < 0) {
      InterpolateVertex(&trv, &brv, (0 - trv.position.y) / (brv.position.y - trv.position.y),
                        &tmp, varyingCount);
      trv = tmp;
   }
   if ((int)blv.position.y >= (int)height) {
      InterpolateVertex(&tlv, &blv, (height - 1 - tlv.position.y) / (blv.position.y - tlv.position.y),
                        &tmp, varyingCount);
      blv = tmp;
   }
   if ((int)brv.position.y >= (int)height) {
      InterpolateVertex(&trv, &brv, (height - 1 - trv.position.y) / (brv.position.y - trv.position.y),
                        &tmp, varyingCount);
      brv = tmp;
   }

//   // horizontally clip
//   if ((int)tlv.position.x < 0) {
//      InterpolateVertex(&tlv, &trv, (0 - tlv.position.x) / (trv.position.x - tlv.position.x),
//                        &tmp, varyingCount);
//      tlv = tmp;
//   }
//   if ((int)blv.position.x < 0) {
//      InterpolateVertex(&blv, &brv, (0 - blv.position.x) / (brv.position.x - blv.position.x),
//                        &tmp, varyingCount);
//      blv = tmp;
//   }
//   if ((int)trv.position.x >= (int)width) {
//      InterpolateVertex(&tlv, &trv, (width - 1 - tlv.position.x) / (trv.position.x - tlv.position.x),
//                        &tmp, varyingCount);
//      trv = tmp;
//   }
//   if ((int)brv.position.x >= (int)width) {
//      InterpolateVertex(&blv, &brv, (width - 1 - blv.position.x) / (brv.position.x - blv.position.x),
//                        &tmp, varyingCount);
//      brv = tmp;
//   }

   const unsigned int startY = tlv.position.y;
   const unsigned int endY = blv.position.y;

   if (endY < startY)
      return;

   const VectorComp_t yDistInv = VectorComp_t_CTR(1.0f / (endY - startY));

   // bV and cV are left and right vertices on a horizontal line in quad
   // bDx and cDx are iterators from tlv to blv, trv to brv for bV and cV

   VertexOutput bV(tlv), cV(trv);
   VertexOutput bDx(blv), cDx(brv);

   for (unsigned i = 0; i < varyingCount; i++) {
      bDx.varyings[i] -= tlv.varyings[i];
      bDx.varyings[i] *= yDistInv;

      cDx.varyings[i] -= trv.varyings[i];
      cDx.varyings[i] *= yDistInv;
   }

   bDx.position -= tlv.position;
   bDx.position *= yDistInv;

   cDx.position -= trv.position;
   cDx.position *= yDistInv;

   bDx.frontFacingPointCoord -= tlv.frontFacingPointCoord; // gl_PointCoord
   bDx.frontFacingPointCoord *= yDistInv;
   bDx.frontFacingPointCoord.y = VectorComp_t_Zero; // gl_FrontFacing not interpolated
   cDx.frontFacingPointCoord -= trv.frontFacingPointCoord; // gl_PointCoord
   cDx.frontFacingPointCoord *= yDistInv;
   cDx.frontFacingPointCoord.y = VectorComp_t_Zero; // gl_FrontFacing not interpolated

   static WorkerArgs args; // TODO: fix this static

#define DUAL_THREAD 1

#if DUAL_THREAD
   static pthread_t thread;
   if (!thread) {
      int rc = pthread_create(&thread, NULL, WorkerArgs::RasterTrapezoidWorker, &args);
      assert(!rc);
   }
   args.bV = bV;
   args.cV = cV;
   for (unsigned i = 0; i < varyingCount; i++) {
      args.bV.varyings[i] += bDx.varyings[i];
      bDx.varyings[i] += bDx.varyings[i];
      args.cV.varyings[i] += cDx.varyings[i];
      cDx.varyings[i] += cDx.varyings[i];
   }
   args.bV.position += bDx.position;
   bDx.position += bDx.position;
   args.cV.position += cDx.position;
   cDx.position += cDx.position;
   args.bV.frontFacingPointCoord += bDx.frontFacingPointCoord;
   bDx.frontFacingPointCoord += bDx.frontFacingPointCoord;
   args.cV.frontFacingPointCoord += cDx.frontFacingPointCoord;
   cDx.frontFacingPointCoord += cDx.frontFacingPointCoord;
   args.iface = iface;
   args.bDx = bDx;
   args.cDx = cDx;
   args.varyingCount = varyingCount;
   args.startY = startY + 1;
   args.endY = endY;
   args.width = width;
   args.height = height;
   if (args.startY <= args.endY)
      args.hasWork = true;
#endif

   VertexOutput * left, * right;
   VertexOutput clip0, clip1;

   for (unsigned y = startY; y <= endY; y += 1 + DUAL_THREAD) {
      do {
         if (bV.position.x < 0) {
            if (cV.position.x < 0)
               break;
            InterpolateVertex(&bV, &cV, -bV.position.x / (cV.position.x - bV.position.x),
                              &clip0, varyingCount);
            left = &clip0;
         } else
            left = &bV;
         if ((int)cV.position.x >= (int)width) {
            if (bV.position.x >= (int)width)
               break;
            InterpolateVertex(&bV, &cV, (width - 1 - bV.position.x) / (cV.position.x - bV.position.x),
                              &clip1, varyingCount);
            right = &clip1;
         } else
            right = &cV;
         iface->ScanLine(iface, left, right);
      } while (false);
      for (unsigned i = 0; i < varyingCount; i++) {
         bV.varyings[i] += bDx.varyings[i];
         cV.varyings[i] += cDx.varyings[i];
      }
      bV.position += bDx.position;
      cV.position += cDx.position;
      bV.frontFacingPointCoord += bDx.frontFacingPointCoord;
      cV.frontFacingPointCoord += cDx.frontFacingPointCoord;
   }

   while (args.hasWork)
      ; // wait
}

static void RasterTriangle(const GGLInterface * iface, const VertexOutput * v1,
                           const VertexOutput * v2, const VertexOutput * v3)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   const unsigned varyingCount = ctx->CurrentProgram->VaryingSlots;
   const unsigned height = ctx->frameSurface.height;
   const VertexOutput * a = v1, * b = v2, * d = v3;
   //abc is a triangle, bcd is another triangle, they share bc as horizontal edge
   //c is between a and d, xy is screen coord

   //first sort 3 vertices by MIN y first
   if (v2->position.y < v1->position.y) {
      a = v2;
      b = v1;
   }
   if (v3->position.y < a->position.y) {
      d = b;
      b = a;
      a = v3;
   } else if (v3->position.y < b->position.y) {
      d = b;
      b = v3;
   }

   assert(a->position.y <= b->position.y && b->position.y <= d->position.y);

   VertexOutput cVertex;
   const VertexOutput* c = &cVertex;

   const VectorComp_t cLerp = (b->position.y - a->position.y) /
                              MAX2(VectorComp_t_One, (d->position.y - a->position.y));
   // create 4th vertex, same y as b to form two triangles/trapezoids sharing horizontal edge
   InterpolateVertex(a, d, cLerp, &cVertex, varyingCount);

   if (c->position.x < b->position.x) {
      const VertexOutput * tmp = c;
      c = b;
      b = tmp;
   }

   if ((int)a->position.y < (int)height && (int)b->position.y >= 0)
      RasterTrapezoid(iface, a, a, b, c);
   //b->position.y += VectorComp_t_One;
   //c->position.y += VectorComp_t_One;
   if ((int)b->position.y < (int)height && (int)d->position.y >= 0)
      RasterTrapezoid(iface, b, c, d, d);
}

static void DrawTriangle(const GGLInterface * iface, const VertexInput * vin1,
                         const VertexInput * vin2, const VertexInput * vin3)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);

   VertexOutput vouts[3];
   VertexOutput * v1 = vouts + 0, * v2 = vouts + 1, * v3 = vouts + 2;

#ifdef SHADER_SOA
   assert(0); // not implemented
#endif

   iface->ProcessVertex(iface, vin1, v1);
   iface->ProcessVertex(iface, vin2, v2);
   iface->ProcessVertex(iface, vin3, v3);

   v1->position /= v1->position.w;
   v2->position /= v2->position.w;
   v3->position /= v3->position.w;

   iface->ViewportTransform(iface, &v1->position);
   iface->ViewportTransform(iface, &v2->position);
   iface->ViewportTransform(iface, &v3->position);

   VectorComp_t area;
   area = v1->position.x * v2->position.y - v2->position.x * v1->position.y;
   area += v2->position.x * v3->position.y - v3->position.x * v2->position.y;
   area += v3->position.x * v1->position.y - v1->position.x * v3->position.y;
   area *= 0.5f;

   if (GL_CCW == ctx->cullState.frontFace + GL_CW)
      (unsigned &)area ^= 0x80000000;

   if (ctx->cullState.enable) {
      switch (ctx->cullState.cullFace + GL_FRONT) {
      case GL_FRONT:
         if (!((unsigned &)area & 0x80000000)) // +ve, front facing
            return;
         break;
      case GL_BACK:
         if ((unsigned &)area & 0x80000000) // -ve, back facing
            return;
         break;
      case GL_FRONT_AND_BACK:
         return;
      default:
         assert(0);
      }
   }

   v1->frontFacingPointCoord.y = v2->frontFacingPointCoord.y =
                                    v3->frontFacingPointCoord.y = !((unsigned &)area & 0x80000000) ?
                                                                  VectorComp_t_One : VectorComp_t_Zero;

   iface->StencilSelect(iface, ((unsigned &)area & 0x80000000) ? GL_BACK : GL_FRONT);

//    if (0)
//    {
//        GGLContext * ctx =(GGLContext *)iface;
//        for (unsigned sampler = 0; sampler < GGL_MAXCOMBINEDTEXTUREIMAGEUNITS; sampler++)
//        {
//            if (!((1 << sampler) & ctx->glCtx->Shader.CurrentProgram->FragmentProgram->SamplersUsed))
//                continue;
//            const GGLTexture * texture = ctx->textureState.textures + sampler;
//            int level = texture->width * texture->height / (area * 2) - 4;
//            assert(texture->levels);
//            ctx->textureState.textureData[sampler] = texture->levels[0];
//            ctx->textureState.textureDimensions[sampler * 2] = texture->width;
//            ctx->textureState.textureDimensions[sampler * 2 + 1] = texture->height;
//            for (unsigned i = 1; i < texture->levelCount && i <= level; i++)
//            {
//                ctx->textureState.textureData[sampler] = texture->levels[i];
//                ctx->textureState.textureDimensions[sampler * 2] += 1;
//                ctx->textureState.textureDimensions[sampler * 2] /= 2;
//                ctx->textureState.textureDimensions[sampler * 2 + 1] += 1;
//                ctx->textureState.textureDimensions[sampler * 2 + 1] /= 2;
//            }
//        }
//    }

   // TODO DXL view frustum clipping
   iface->RasterTriangle(iface, v1, v2, v3);

}

static void PickRaster(GGLInterface * iface)
{
   iface->ProcessVertex = ProcessVertex;
   iface->DrawTriangle = DrawTriangle;
   iface->RasterTriangle = RasterTriangle;
   iface->RasterTrapezoid = RasterTrapezoid;
}

static void ViewportTransform(const GGLInterface * iface, Vector4 * v)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   v->x = v->x * ctx->viewport.w + ctx->viewport.x;
   v->y = v->y * ctx->viewport.h + ctx->viewport.y;
   v->z = v->z * ctx->viewport.f + ctx->viewport.n;
}


void InitializeRasterFunctions(GGLInterface * iface)
{
   GGL_GET_CONTEXT(ctx, iface);
   ctx->PickRaster = PickRaster;
   iface->ViewportTransform = ViewportTransform;
}
