/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "src/pixelflinger2/pixelflinger2.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <llvm/LLVMContext.h>

#include "src/talloc/hieralloc.h"
#include "src/mesa/main/mtypes.h"
#include "src/mesa/program/prog_parameter.h"
#include "src/mesa/program/prog_uniform.h"
#include "src/glsl/glsl_types.h"
#include "src/glsl/ir.h"

extern void link_shaders(struct gl_context *ctx, struct gl_shader_program *prog);

extern "C" void _mesa_reference_shader(struct gl_context *ctx, struct gl_shader **ptr,
                                          struct gl_shader *sh)
{
   *ptr = sh;
}

extern "C" gl_shader * _mesa_new_shader(struct gl_context *ctx, GLuint name, GLenum type)
{
   struct gl_shader *shader;
   assert(type == GL_FRAGMENT_SHADER || type == GL_VERTEX_SHADER);
   shader = hieralloc_zero(ctx, struct gl_shader);
   if (shader) {
      shader->Type = type;
      shader->Name = name;
      shader->RefCount = 1;
   }
   return shader;
}

extern "C" void _mesa_delete_shader(struct gl_context *ctx, struct gl_shader *shader)
{
   if (!shader)
      return;
   if (1 == shader->RefCount)
      hieralloc_free(shader);
   else
      shader->DeletePending = true;
}

static gl_shader * ShaderCreate(const GGLInterface * iface, GLenum type)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   if (GL_VERTEX_SHADER != type && GL_FRAGMENT_SHADER != type) {
      gglError(GL_INVALID_ENUM);
      return NULL;
   }
   gl_shader * shader = _mesa_new_shader(ctx->glCtx, 0, type);
   if (!shader)
      gglError(GL_OUT_OF_MEMORY);
   assert(1 == shader->RefCount);
   return shader;
}

extern "C" void compile_shader(struct gl_context *ctx, struct gl_shader *shader);

static GLboolean ShaderCompile(const GGLInterface * iface, gl_shader * shader,
                               const char * glsl, char ** infoLog)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   if (!glsl) {
      gglError(GL_INVALID_VALUE);
      return GL_FALSE;
   }
   shader->Source = glsl;
   compile_shader(ctx->glCtx, shader);
   shader->Source = NULL;
   if (infoLog)
      *infoLog = shader->InfoLog;
   return shader->CompileStatus;
}

static void ShaderDelete(const GGLInterface * iface, gl_shader * shader)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   _mesa_delete_shader(ctx->glCtx, shader);
}

static gl_shader_program * ShaderProgramCreate(const GGLInterface * iface)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   gl_shader_program * program = hieralloc_zero(ctx->glCtx, struct gl_shader_program);
   if (!program) {
      gglError(GL_OUT_OF_MEMORY);
      return NULL;
   }
   program->Attributes = hieralloc_zero(program, gl_program_parameter_list);
   if (!program->Attributes) {
      hieralloc_free(program);
      gglError(GL_OUT_OF_MEMORY);
      return NULL;
   }
   program->Varying = hieralloc_zero(program, gl_program_parameter_list);
   if (!program->Varying) {
      hieralloc_free(program);
      gglError(GL_OUT_OF_MEMORY);
      return NULL;
   }
   return program;
}

static void ShaderAttach(const GGLInterface * iface, gl_shader_program * program,
                         gl_shader * shader)
{
   for (unsigned i = 0; i < program->NumShaders; i++)
      if (program->Shaders[i]->Type == shader->Type || program->Shaders[i] == shader)
         return gglError(GL_INVALID_OPERATION);

   program->Shaders = (gl_shader **)hieralloc_realloc
                      (program, program->Shaders, gl_shader *, program->NumShaders + 1);
   if (!program->Shaders) {
      gglError(GL_OUT_OF_MEMORY);
      assert(0);
      return;
   }
   program->Shaders[program->NumShaders] = shader;
   program->NumShaders++;
   shader->RefCount++;
}

static void ShaderDetach(const GGLInterface * iface, gl_shader_program * program,
                         gl_shader * shader)
{
   for (unsigned i = 0; i < program->NumShaders; i++)
      if (program->Shaders[i] == shader) {
         program->NumShaders--;
         program->Shaders[i] = program->Shaders[program->NumShaders];
         shader->RefCount--;
         if (1 == shader->RefCount && shader->DeletePending)
            iface->ShaderDelete(iface, shader);
         return;
      }
   gglError(GL_INVALID_OPERATION);
}

static GLboolean ShaderProgramLink(const GGLInterface * iface, gl_shader_program * program,
                                   char ** infoLog)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   link_shaders(ctx->glCtx, program);
   if (infoLog)
      *infoLog = program->InfoLog;
   return program->LinkStatus;
}

struct gl_program;
struct ShaderKey;

static void GetShaderKey(const GGLContext * ctx, const gl_program * shader, ShaderKey * key)
{
//    memset(key, 0, sizeof(*key));
//    if (GL_FRAGMENT_SHADER == shader->Target)
//    {
//        key->scanLineKey.frontStencil = ctx->frontStencil;
//        key->scanLineKey.backStencil = ctx->backStencil;
//        key->scanLineKey.bufferState = ctx->bufferState;
//        key->scanLineKey.blendState = ctx->blendState;
//    }
//
//    for (unsigned i = 0; i < GGL_MAXCOMBINEDTEXTUREIMAGEUNITS; i++)
//        if (shader->SamplersUsed & (1 << i))
//        {
//            const GGLTexture & texture = ctx->textureState.textures[i];
//            key->textureFormats[i] = texture.format;
//            assert((1 << 2) > texture.wrapS);
//            key->textureParameters[i] |= texture.wrapS;
//            assert((1 << 2) > texture.wrapT);
//            key->textureParameters[i] |= texture.wrapT << 2;
//            assert((1 << 3) > texture.minFilter);
//            key->textureParameters[i] |= texture.minFilter << (2 + 2);
//            assert((1 << 1) > texture.magFilter);
//            key->textureParameters[i] |= texture.magFilter << (2 + 2 + 3);
//        }
}

static inline char HexDigit(unsigned char d)
{
   return (d > 9 ? d + 'A' - 10 : d + '0');
}

static const unsigned SHADER_KEY_STRING_LEN = GGL_MAXCOMBINEDTEXTUREIMAGEUNITS * 4 + 2;

static void GetShaderKeyString(const GLenum type, const ShaderKey * key,
                               char * buffer, const unsigned bufferSize)
{
//    assert(1 == sizeof(char));
//    assert(0xff >= GGL_PIXEL_FORMAT_COUNT);
//    assert(SHADER_KEY_STRING_LEN <= bufferSize);
//    char * str = buffer;
//    if (GL_VERTEX_SHADER == type)
//        *str++ = 'v';
//    else if (GL_FRAGMENT_SHADER == type)
//        *str++ = 'f';
//    else
//        assert(0);
//    for (unsigned i = 0; i < GGL_MAXCOMBINEDTEXTUREIMAGEUNITS; i++)
//    {
//        *str++ = HexDigit(key->textureFormats[i] / 16);
//        *str++ = HexDigit(key->textureFormats[i] % 16);
//        *str++ = HexDigit(key->textureParameters[i] / 16);
//        *str++ = HexDigit(key->textureParameters[i] % 16);
//    }
//    *str++ = '\0';
}

//static const unsigned SCANLINE_KEY_STRING_LEN = 2 * sizeof(((ShaderKey *)0)->scanLineKey) +
//                                            3 + SHADER_KEY_STRING_LEN;

static char * GetScanlineKeyString(const ShaderKey * key, char * buffer,
                                   const unsigned bufferSize)
{
//    assert(1 == sizeof(char));
//    assert(0xff >= GGL_PIXEL_FORMAT_COUNT);
//    assert(SCANLINE_KEY_STRING_LEN <= bufferSize);
//    char * str = buffer;
//    *str++ = 's';
//    const unsigned char * start = (const unsigned char *)&key->scanLineKey;
//    const unsigned char * const end = start + sizeof(key->scanLineKey);
//    for (; start < end; start++)
//    {
//        *str++ = HexDigit(*start / 16);
//        *str++ = HexDigit(*start % 16);
//    }
//    GetShaderKeyString(GL_FRAGMENT_SHADER, key, str, bufferSize - (str - buffer));
//    return buffer;
   return NULL;
}

static void ShaderUse(GGLInterface * iface, gl_shader_program * program)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   assert(program);
   if (!program) {
      ctx->glCtx->CurrentProgram = NULL;
      // so drawing calls will do nothing until ShaderUse with a program
      SetShaderVerifyFunctions(iface);
      return;
   }

//    if (program->VertexProgram)
//    {
//        if (!program->STVP)
//        {
//            program->STVP = CALLOC_STRUCT(st_vertex_program);
//            program->STVP->Base = *program->VertexProgram;
//            st_translate_vertex_program(ctx->glCtx, program->STVP, NULL, NULL, NULL);
//        }
//
//        _mesa_update_shader_textures_used(program->VertexProgram);
//
//        ShaderKey shaderKey;
//        GetShaderKey(ctx, program->VertexProgram, &shaderKey);
//        ShaderFunction_t function = NULL;
//        if (!program->GLVMVP || NULL == (function = program->GLVMVP->functions[shaderKey]))
//        {
//            char shaderName [SHADER_KEY_STRING_LEN] = {0};
//            GetShaderKeyString(GL_VERTEX_SHADER, &shaderKey, shaderName, Elements(shaderName));
//            create_program(program->STVP->state.tokens, GALLIVM_VS, &program->GLVMVP,
//                           &ctx->glCtx->Shader.cpu, ctx, program->VertexProgram,
//                           shaderName, NULL);
//            program->GLVMVP->functions[shaderKey] = program->GLVMVP->function;
//            debug_printf("jit new vertex shader %p \n", program->GLVMVP->function); //getchar();
//        }
//        else
//        {
//            program->GLVMVP->function = function;
//            //debug_printf("use cached vertex shader %p \n", function);
//        }
//        ctx->PickRaster(iface);
//    }
//    if (program->FragmentProgram)
//    {
//        if (!program->STFP)
//        {
//            program->STFP = CALLOC_STRUCT(st_fragment_program);
//            program->STFP->Base = *program->FragmentProgram;
//            st_translate_fragment_program(ctx->glCtx, program->STFP, NULL);
//        }
//
//        _mesa_update_shader_textures_used(program->FragmentProgram);
//
//        ShaderKey shaderKey;
//        GetShaderKey(ctx, program->FragmentProgram, &shaderKey);
//        ShaderFunction_t function = NULL;
//        if (!program->GLVMFP || NULL == (function = program->GLVMFP->functions[shaderKey]))
//        {
//            char shaderName [SHADER_KEY_STRING_LEN] = {0};
//            GetShaderKeyString(GL_FRAGMENT_SHADER, &shaderKey, shaderName, Elements(shaderName));
//
//            char scanlineName [SCANLINE_KEY_STRING_LEN] = {0};
//            GetScanlineKeyString(&shaderKey, scanlineName, Elements(scanlineName));
//            create_program(program->STFP->state.tokens, GALLIVM_FS,  &program->GLVMFP,
//                           &ctx->glCtx->Shader.cpu, ctx, program->FragmentProgram,
//                           shaderName, scanlineName);
//            program->GLVMFP->functions[shaderKey] = program->GLVMFP->function;
//            debug_printf("jit new fragment shader %p \n", program->GLVMFP->function);
//        }
//        else
//        {
//            program->GLVMFP->function = function;
//            //debug_printf("use cached fragment shader %p \n", function);
//        }
//        ctx->PickScanLine(iface);
//    }
//    ctx->glCtx->CurrentProgram = program;
}

static void ShaderProgramDelete(const GGLInterface * iface, gl_shader_program * program)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   if (ctx->glCtx->CurrentProgram == program) {
      ctx->glCtx->CurrentProgram = NULL;
      SetShaderVerifyFunctions(const_cast<GGLInterface *>(iface));
   }

   for (unsigned i = 0; i < program->NumShaders; i++) {
      iface->ShaderDelete(iface, program->Shaders[i]);
      iface->ShaderDetach(iface, program, program->Shaders[i]);
   }

   for (unsigned i = 0; i < MESA_SHADER_TYPES; i++)
      iface->ShaderDelete(iface, program->_LinkedShaders[i]);
}

static void ShaderAttributeBind(const GGLInterface * iface, const gl_shader_program * program,
                                GLuint index, const GLchar * name)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   int i = _mesa_add_parameter(program->Attributes, name);
   program->Attributes->Parameters[i].BindLocation = index;
}

static GLint ShaderAttributeLocation(const GGLInterface * iface, const gl_shader_program * program,
                                     const char * name)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   int i = _mesa_get_parameter(program->Attributes, name);
   if (i >= 0)
      return program->Attributes->Parameters[i].Location;
   return -2;
}

static GLint ShaderUniformLocation(const GGLInterface * iface, const gl_shader_program * program,
                                   const char * name)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   for (unsigned i = 0; i < program->Uniforms->NumUniforms; i++)
      if (!strcmp(program->Uniforms->Uniforms[i].Name, name))
         return program->Uniforms->Uniforms[i].Pos;
//    return _mesa_get_shader_uniform_location(ctx->glCtx, program, name);
   return -2;
}

static void ShaderUniformGetfv(const GGLInterface * iface, gl_shader_program * program,
                               GLint location, GLfloat * params)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   memcpy(params, program->ValuesUniform + location, sizeof(*program->ValuesUniform));
}

static void ShaderUniformGetiv(const GGLInterface * iface, gl_shader_program * program,
                               GLint location, GLint * params)
{
   // TODO: sampler uniform
   GGL_GET_CONST_CONTEXT(ctx, iface);
   memcpy(params, program->ValuesUniform + location, sizeof(*program->ValuesUniform));
}

static GLint ShaderUniform(const GGLInterface * iface, gl_shader_program * program,
                           GLint location, GLsizei count, const GLvoid *values, GLenum type)
{
   // TODO: sampler uniform
   GGL_GET_CONST_CONTEXT(ctx, iface);
//    if (!program)
//    {
//        gglError(GL_INVALID_OPERATION);
//        return -2;
//    }
//    return _mesa_uniform(ctx->glCtx, program, location, count, values, type);
   return -2;
}

static void ShaderUniformMatrix(const GGLInterface * iface, gl_shader_program * program,
                                GLint cols, GLint rows, GLint location, GLsizei count,
                                GLboolean transpose, const GLfloat *values)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
//    if (!program)
//        return gglError(GL_INVALID_OPERATION);
//    _mesa_uniform_matrix(ctx->glCtx, program, cols, rows, location, count, transpose, values);
}

static void ShaderVerifyProcessVertex(const GGLInterface * iface, const VertexInput * input,
                                      VertexOutput * output)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   if (ctx->glCtx->CurrentProgram) {
      ShaderUse(const_cast<GGLInterface *>(iface), ctx->glCtx->CurrentProgram);
      if (ShaderVerifyProcessVertex != iface->ProcessVertex)
         iface->ProcessVertex(iface, input, output);
   }
}

static void ShaderVerifyDrawTriangle(const GGLInterface * iface, const VertexInput * v0,
                                     const VertexInput * v1, const VertexInput * v2)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   if (ctx->glCtx->CurrentProgram) {
      ShaderUse(const_cast<GGLInterface *>(iface), ctx->glCtx->CurrentProgram);
      if (ShaderVerifyDrawTriangle != iface->DrawTriangle)
         iface->DrawTriangle(iface, v0, v1, v2);
   }
}

static void ShaderVerifyRasterTriangle(const GGLInterface * iface, const VertexOutput * v1,
                                       const VertexOutput * v2, const VertexOutput * v3)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   if (ctx->glCtx->CurrentProgram) {
      ShaderUse(const_cast<GGLInterface *>(iface), ctx->glCtx->CurrentProgram);
      if (ShaderVerifyRasterTriangle != iface->RasterTriangle)
         iface->RasterTriangle(iface, v1, v2, v3);
   }
}

static void ShaderVerifyRasterTrapezoid(const GGLInterface * iface, const VertexOutput * tl,
                                        const VertexOutput * tr, const VertexOutput * bl,
                                        const VertexOutput * br)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   if (ctx->glCtx->CurrentProgram) {
      ShaderUse(const_cast<GGLInterface *>(iface), ctx->glCtx->CurrentProgram);
      if (ShaderVerifyRasterTrapezoid != iface->RasterTrapezoid)
         iface->RasterTrapezoid(iface, tl, tr, bl, br);
   }
}

static void ShaderVerifyScanLine(const GGLInterface * iface, const VertexOutput * v1,
                                 const VertexOutput * v2)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   if (ctx->glCtx->CurrentProgram) {
      ShaderUse(const_cast<GGLInterface *>(iface), ctx->glCtx->CurrentProgram);
      if (ShaderVerifyScanLine != iface->ScanLine)
         iface->ScanLine(iface, v1, v2);
   }
}

// called after state changes so that drawing calls will trigger JIT
void SetShaderVerifyFunctions(struct GGLInterface * iface)
{
   iface->ProcessVertex = ShaderVerifyProcessVertex;
   iface->DrawTriangle = ShaderVerifyDrawTriangle;
   iface->RasterTriangle = ShaderVerifyRasterTriangle;
   iface->RasterTrapezoid = ShaderVerifyRasterTrapezoid;
   iface->ScanLine = ShaderVerifyScanLine;
}

static void InitializeGLContext(struct gl_context *ctx)
{
   memset(ctx, 0, sizeof(*ctx));
   ctx->API = API_OPENGLES2;
   ctx->Extensions.ARB_draw_buffers = GL_TRUE;
   ctx->Extensions.ARB_fragment_coord_conventions = GL_TRUE;
   ctx->Extensions.EXT_texture_array = GL_TRUE;
   ctx->Extensions.NV_texture_rectangle = GL_TRUE;

   /* 1.10 minimums. */
   ctx->Const.MaxLights = 8;
   ctx->Const.MaxClipPlanes = 8;
   ctx->Const.MaxTextureUnits = 2;

   /* More than the 1.10 minimum to appease parser tests taken from
    * apps that (hopefully) already checked the number of coords.
    */
   ctx->Const.MaxTextureCoordUnits = 4;

   ctx->Const.VertexProgram.MaxAttribs = 16;
   ctx->Const.VertexProgram.MaxUniformComponents = 512;
   ctx->Const.MaxVarying = 8;
   ctx->Const.MaxVertexTextureImageUnits = 0;
   ctx->Const.MaxCombinedTextureImageUnits = 2;
   ctx->Const.MaxTextureImageUnits = 2;
   ctx->Const.FragmentProgram.MaxUniformComponents = 64;

   ctx->Const.MaxDrawBuffers = 2;

   ctx->Driver.NewShader = _mesa_new_shader;
   ctx->Driver.DeleteShader = _mesa_delete_shader;
}

void InitializeShaderFunctions(struct GGLInterface * iface)
{
   GGL_GET_CONTEXT(ctx, iface);
   ctx->llvmCtx = new llvm::LLVMContext();
   ctx->glCtx = hieralloc(NULL, gl_context);
   InitializeGLContext(ctx->glCtx);

   iface->ShaderCreate = ShaderCreate;
   iface->ShaderCompile = ShaderCompile;
   iface->ShaderDelete = ShaderDelete;
   iface->ShaderProgramCreate = ShaderProgramCreate;
   iface->ShaderAttach = ShaderAttach;
   iface->ShaderDetach = ShaderDetach;
   iface->ShaderProgramLink = ShaderProgramLink;
   iface->ShaderUse = ShaderUse;
   iface->ShaderProgramDelete = ShaderProgramDelete;
   iface->ShaderAttributeBind = ShaderAttributeBind;
   iface->ShaderAttributeLocation = ShaderAttributeLocation;
   iface->ShaderUniformLocation = ShaderUniformLocation;
   iface->ShaderUniformGetfv = ShaderUniformGetfv;
   iface->ShaderUniformGetiv = ShaderUniformGetiv;
   iface->ShaderUniform = ShaderUniform;
   iface->ShaderUniformMatrix = ShaderUniformMatrix;
}

void DestroyShaderFunctions(GGLInterface * iface)
{
   GGL_GET_CONTEXT(ctx, iface);
   _mesa_glsl_release_types();
   _mesa_glsl_release_functions();
   hieralloc_free(ctx->glCtx);
   delete ctx->llvmCtx;
}
