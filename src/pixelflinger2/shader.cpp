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
#include <map>

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <bcc/bcc.h>
#include <dlfcn.h>


#include "src/talloc/hieralloc.h"
#include "src/mesa/main/mtypes.h"
#include "src/mesa/program/prog_parameter.h"
#include "src/mesa/program/prog_uniform.h"
#include "src/glsl/glsl_types.h"
#include "src/glsl/ir_to_llvm.h"

struct ShaderKey {
   struct ScanLineKey {
      GGLStencilState frontStencil, backStencil;
      GGLBufferState bufferState;
      GGLBlendState blendState;
   } scanLineKey;
   GGLPixelFormat textureFormats[GGL_MAXCOMBINEDTEXTUREIMAGEUNITS];
   unsigned char textureParameters[GGL_MAXCOMBINEDTEXTUREIMAGEUNITS]; // wrap and filter
   bool operator <(const ShaderKey & rhs) const {
      return memcmp(this, &rhs, sizeof(*this)) < 0;
   }
};

struct Instance {
   llvm::Module * module;
   struct BCCOpaqueScript * script;
   void (* function)();
   ~Instance() {
      // TODO: check bccDisposeScript, which seems to dispose llvm::Module
      if (script)
         bccDisposeScript(script);
      else if (module)
         delete module;
   }
};

struct Executable { // codegen info
   std::map<ShaderKey, Instance *> instances;
};

bool do_mat_op_to_vec(exec_list *instructions);

extern void link_shaders(struct gl_context *ctx, struct gl_shader_program *prog);

extern "C" void compile_shader(const struct gl_context *ctx, struct gl_shader *shader);

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
   if (shader->RefCount > 1) {
      shader->DeletePending = true;
      return;
   }
   if (shader->executable) {
      for (std::map<ShaderKey, Instance *>::iterator it=shader->executable->instances.begin();
            it != shader->executable->instances.end(); it++)
         (*it).second->~Instance();
      shader->executable->instances.~map();
   }
   hieralloc_free(shader);
}

gl_shader * GGLShaderCreate(GLenum type)
{
   return _mesa_new_shader(NULL, 0, type);
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

GLboolean GGLShaderCompile(const gl_context * glCtx, gl_shader * shader,
                           const char * glsl, const char ** infoLog)
{
   shader->Source = glsl;
   compile_shader(glCtx, shader);
   shader->Source = NULL;
   if (infoLog)
      *infoLog = shader->InfoLog;
   return shader->CompileStatus;
}

static GLboolean ShaderCompile(const GGLInterface * iface, gl_shader * shader,
                               const char * glsl, const char ** infoLog)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   if (!glsl) {
      gglError(GL_INVALID_VALUE);
      return GL_FALSE;
   }
   return GGLShaderCompile(ctx->glCtx, shader, glsl, infoLog);
}

void GGLShaderDelete(gl_shader * shader)
{
   _mesa_delete_shader(NULL, shader);
}

static void ShaderDelete(const GGLInterface * iface, gl_shader * shader)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   _mesa_delete_shader(ctx->glCtx, shader);
}

gl_shader_program * GGLShaderProgramCreate()
{
   gl_shader_program * program = hieralloc_zero(NULL, struct gl_shader_program);
   if (!program)
      return NULL;
   program->Attributes = hieralloc_zero(program, gl_program_parameter_list);
   if (!program->Attributes) {
      hieralloc_free(program);
      return NULL;
   }
   program->Varying = hieralloc_zero(program, gl_program_parameter_list);
   if (!program->Varying) {
      hieralloc_free(program);
      return NULL;
   }
   return program;
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

unsigned GGLShaderAttach(gl_shader_program * program, gl_shader * shader)
{
   for (unsigned i = 0; i < program->NumShaders; i++)
      if (program->Shaders[i]->Type == shader->Type || program->Shaders[i] == shader)
         return GL_INVALID_OPERATION;

   program->Shaders = (gl_shader **)hieralloc_realloc
                      (program, program->Shaders, gl_shader *, program->NumShaders + 1);
   if (!program->Shaders) {
      assert(0);
      return GL_OUT_OF_MEMORY;
   }
   program->Shaders[program->NumShaders] = shader;
   program->NumShaders++;
   shader->RefCount++;
   return GL_NO_ERROR;
}

static void ShaderAttach(const GGLInterface * iface, gl_shader_program * program,
                         gl_shader * shader)
{
   unsigned error = GGLShaderAttach(program, shader);
   if (GL_NO_ERROR != error)
      gglError(error);
}

unsigned GGLShaderDetach(gl_shader_program * program, gl_shader * shader)
{
   for (unsigned i = 0; i < program->NumShaders; i++)
      if (program->Shaders[i] == shader) {
         program->NumShaders--;
         program->Shaders[i] = program->Shaders[program->NumShaders];
         shader->RefCount--;
         if (1 == shader->RefCount && shader->DeletePending)
            GGLShaderDelete(shader);
         return GL_NO_ERROR;
      }
   return (GL_INVALID_OPERATION);
}

static void ShaderDetach(const GGLInterface * iface, gl_shader_program * program,
                         gl_shader * shader)
{
   unsigned error = GGLShaderDetach(program, shader);
   if (GL_NO_ERROR != error)
      gglError(error);
}

GLboolean GGLShaderProgramLink(gl_context * glCtx, gl_shader_program * program,
                               const char ** infoLog)
{
   link_shaders(glCtx, program);
   if (infoLog)
      *infoLog = program->InfoLog;
   if (!program->LinkStatus)
      return program->LinkStatus;
   for (unsigned i = 0; i < program->Attributes->NumParameters; i++) {
      const gl_program_parameter & attribute = program->Attributes->Parameters[i];
      printf("attribute '%s': location=%d slots=%d \n", attribute.Name, attribute.Location, attribute.Slots);
   }
   for (unsigned i = 0; i < program->Varying->NumParameters; i++) {
      const gl_program_parameter & varying = program->Varying->Parameters[i];
      printf("varying '%s': vs_location=%d fs_location=%d \n", varying.Name, varying.BindLocation, varying.Location);
   }
   for (unsigned i = 0; i < program->Uniforms->NumUniforms; i++) {
      const gl_uniform & uniform = program->Uniforms->Uniforms[i];
      printf("uniform '%s': location=%d type=%s \n", uniform.Name, uniform.Pos, uniform.Type->name);
   }
   return program->LinkStatus;
}
static GLboolean ShaderProgramLink(const GGLInterface * iface, gl_shader_program * program,
                                   const char ** infoLog)
{
   GGL_GET_CONST_CONTEXT(ctx, iface);
   return GGLShaderProgramLink(ctx->glCtx, program, infoLog);
}

static void GetShaderKey(const GGLContext * ctx, const gl_shader * shader, ShaderKey * key)
{
   memset(key, 0, sizeof(*key));
   if (GL_FRAGMENT_SHADER == shader->Type) {
      key->scanLineKey.frontStencil = ctx->frontStencil;
      key->scanLineKey.backStencil = ctx->backStencil;
      key->scanLineKey.bufferState = ctx->bufferState;
      key->scanLineKey.blendState = ctx->blendState;
   }

   for (unsigned i = 0; i < GGL_MAXCOMBINEDTEXTUREIMAGEUNITS; i++)
      if (shader->SamplersUsed & (1 << i)) {
         const GGLTexture & texture = ctx->textureState.textures[i];
         key->textureFormats[i] = texture.format;
         assert((1 << 2) > texture.wrapS);
         key->textureParameters[i] |= texture.wrapS;
         assert((1 << 2) > texture.wrapT);
         key->textureParameters[i] |= texture.wrapT << 2;
         assert((1 << 3) > texture.minFilter);
         key->textureParameters[i] |= texture.minFilter << (2 + 2);
         assert((1 << 1) > texture.magFilter);
         key->textureParameters[i] |= texture.magFilter << (2 + 2 + 3);
      }
}

static inline char HexDigit(unsigned char d)
{
   return (d > 9 ? d + 'A' - 10 : d + '0');
}

static const unsigned SHADER_KEY_STRING_LEN = GGL_MAXCOMBINEDTEXTUREIMAGEUNITS * 4 + 2;

static void GetShaderKeyString(const GLenum type, const ShaderKey * key,
                               char * buffer, const unsigned bufferSize)
{
   assert(1 == sizeof(char));
   assert(0xff >= GGL_PIXEL_FORMAT_COUNT);
   assert(SHADER_KEY_STRING_LEN <= bufferSize);
   char * str = buffer;
   if (GL_VERTEX_SHADER == type)
      *str++ = 'v';
   else if (GL_FRAGMENT_SHADER == type)
      *str++ = 'f';
   else
      assert(0);
   for (unsigned i = 0; i < GGL_MAXCOMBINEDTEXTUREIMAGEUNITS; i++) {
      *str++ = HexDigit(key->textureFormats[i] / 16);
      *str++ = HexDigit(key->textureFormats[i] % 16);
      *str++ = HexDigit(key->textureParameters[i] / 16);
      *str++ = HexDigit(key->textureParameters[i] % 16);
   }
   *str++ = '\0';
}

static const unsigned SCANLINE_KEY_STRING_LEN = 2 * sizeof(ShaderKey::scanLineKey) + 3 + SHADER_KEY_STRING_LEN;

static char * GetScanlineKeyString(const ShaderKey * key, char * buffer,
                                   const unsigned bufferSize)
{
   assert(1 == sizeof(char));
   assert(0xff >= GGL_PIXEL_FORMAT_COUNT);
   assert(SCANLINE_KEY_STRING_LEN <= bufferSize);
   char * str = buffer;
   *str++ = 's';
   const unsigned char * start = (const unsigned char *)&key->scanLineKey;
   const unsigned char * const end = start + sizeof(key->scanLineKey);
   for (; start < end; start++) {
      *str++ = HexDigit(*start / 16);
      *str++ = HexDigit(*start % 16);
   }
   GetShaderKeyString(GL_FRAGMENT_SHADER, key, str, bufferSize - (str - buffer));
   return buffer;
}

struct SymbolLookupContext {
   const GGLContext * gglCtx;
   const gl_shader_program * program;
   const gl_shader * shader;
};

static void* SymbolLookup(void* pContext, const char* name)
{
   SymbolLookupContext * ctx = (SymbolLookupContext *)pContext;
   const gl_shader * shader = ctx->shader;
   const gl_shader_program * program = ctx->program;
   const GGLContext * gglCtx = ctx->gglCtx;
   const void * symbol = (void*)dlsym(RTLD_DEFAULT, name);
   if (NULL == symbol) {
      if (!strcmp(_PF2_TEXTURE_DATA_NAME_, name))
         symbol = (void *)gglCtx->textureState.textureData;
      else if (!strcmp(_PF2_TEXTURE_DIMENSIONS_NAME_, name))
         symbol = (void *)gglCtx->textureState.textureDimensions;
      else {
         for (unsigned i = 0; i < program->Uniforms->NumUniforms && !symbol; i++)
            if (!strcmp(program->Uniforms->Uniforms[i].Name, name))
               symbol = program->ValuesUniform + program->Uniforms->Uniforms[i].Pos;
         for (unsigned i = 0; i < program->Attributes->NumParameters && !symbol; i++)
            if (!strcmp(program->Attributes->Parameters[i].Name, name)) {
               assert(program->Attributes->Parameters[i].Location
                      < sizeof(VertexInput) / sizeof(float[4]));
               symbol = program->ValuesVertexInput + program->Attributes->Parameters[i].Location;
            }
         for (unsigned i = 0; i < program->Varying->NumParameters && !symbol; i++)
            if (!strcmp(program->Varying->Parameters[i].Name, name)) {
               int index = -1;
               if (GL_VERTEX_SHADER == shader->Type)
                  index = program->Varying->Parameters[i].BindLocation;
               else if (GL_FRAGMENT_SHADER == shader->Type)
                  index = program->Varying->Parameters[i].Location;
               else
                  assert(0);
               assert(index >= 0);
               assert(index < sizeof(VertexOutput) / sizeof(float[4]));
               symbol = program->ValuesVertexOutput + index;
            }
         assert(symbol >= program->ValuesVertexInput &&
                symbol < (char *)program->ValuesUniform + 16 * program->Uniforms->Slots - 3);
      };
   }
   printf("symbolLookup '%s'=%p \n", name, symbol);
   assert(symbol);
   return (void *)symbol;
}

static void CodeGen(Instance * instance, const char * mainName, gl_shader * shader,
                    gl_shader_program * program, const GGLContext * gglCtx)
{
   SymbolLookupContext ctx = {gglCtx, program, shader};
   int result = 0;

   BCCScriptRef & script = instance->script;
   script = bccCreateScript();
   result = bccReadModule(script, "glsl", (LLVMModuleRef)instance->module, 0);
   assert(0 == result);
   result = bccRegisterSymbolCallback(script, SymbolLookup, &ctx);
   assert(0 == result);
   result = bccPrepareExecutable(script, NULL, 0);

   result = bccGetError(script);
   if (result != 0) {
      puts("failed bcc_compile");
      assert(0);
      return;
   }

   instance->function = (void (*)())bccGetFuncAddr(script, mainName);
   assert(instance->function);
   result = bccGetError(script);
   if (result != BCC_NO_ERROR)
      fprintf(stderr, "Could not find '%s': %d\n", "main", result);
   else
      printf("bcc_compile %s=%p \n", mainName, instance->function);
}

void GenerateScanLine(const GGLContext * gglCtx, const gl_shader_program * program, llvm::Module * mod,
                      const char * shaderName, const char * scanlineName);

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

   for (unsigned i = 0; i < MESA_SHADER_TYPES; i++) {
      if (!program->_LinkedShaders[i])
         continue;
      gl_shader * shader = program->_LinkedShaders[i];
      if (!shader->executable) {
         shader->executable = hieralloc_zero(shader, Executable);
         shader->executable->instances = std::map<ShaderKey, Instance *>();
      }

      ShaderKey shaderKey;
      GetShaderKey(ctx, shader, &shaderKey);
      Instance * instance = shader->executable->instances[shaderKey];
      if (!instance) {
         puts("begin jit new shader");
         instance = hieralloc_zero(shader->executable, Instance);
         instance->module = new llvm::Module("glsl", *ctx->llvmCtx);

         char shaderName [SHADER_KEY_STRING_LEN] = {0};
         GetShaderKeyString(shader->Type, &shaderKey, shaderName, sizeof shaderName / sizeof *shaderName);

         char mainName [SHADER_KEY_STRING_LEN + 6] = {"main"};
         strcat(mainName, shaderName);

         do_mat_op_to_vec(shader->ir);

         llvm::Module * module = glsl_ir_to_llvm_module(shader->ir, instance->module, ctx, shaderName);
         if (!module)
            assert(0);
#if USE_LLVM_SCANLINE
         if (GL_FRAGMENT_SHADER == shader->Type) {
            char scanlineName [SCANLINE_KEY_STRING_LEN] = {0};
            GetScanlineKeyString(&shaderKey, scanlineName, sizeof scanlineName / sizeof *scanlineName);
            GenerateScanLine(ctx, program, module, mainName, scanlineName);
            CodeGen(instance, scanlineName, shader, program, ctx);
         } else
#endif
            CodeGen(instance, mainName, shader, program, ctx);
         shader->executable->instances[shaderKey] = instance;
         debug_printf("jit new shader '%s'(%p) \n", mainName, instance->function);
      } else
//         debug_printf("use cached shader %p \n", instance->function);
         ;

      shader->function  = instance->function;

      if (GL_VERTEX_SHADER == shader->Type)
         ctx->PickRaster(iface);
      else if (GL_FRAGMENT_SHADER == shader->Type)
         ctx->PickScanLine(iface);
      else
         assert(0);
   }

   ctx->glCtx->CurrentProgram = program;
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
   return -1;
}

static GLint ShaderVaryingLocation(const GGLInterface_t * iface, const gl_shader_program_t * program,
                                   const char * name, GLint * vertexOutputLocation)
{
   for (unsigned int i = 0; i < program->Varying->NumParameters; i++)
      if (!strcmp(program->Varying->Parameters[i].Name, name)) {
         if (vertexOutputLocation)
            *vertexOutputLocation = program->Varying->Parameters[i].BindLocation;
         return program->Varying->Parameters[i].Location;
      }
   return -1;
}

static GLint ShaderUniformLocation(const GGLInterface * iface, const gl_shader_program * program,
                                   const char * name)
{
   for (unsigned i = 0; i < program->Uniforms->NumUniforms; i++)
      if (!strcmp(program->Uniforms->Uniforms[i].Name, name))
         return program->Uniforms->Uniforms[i].Pos;
   return -1;
}

static void ShaderUniformGetfv(const GGLInterface * iface, gl_shader_program * program,
                               GLint location, GLfloat * params)
{
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
   if (!program) {
      gglError(GL_INVALID_OPERATION);
      return -2;
   }
   int start = location;
   int slots = 0, elems = 0;
   switch (type) {
   case GL_INT:
   case GL_FLOAT:
   case  GL_BOOL:
      slots = count;
      elems = 1;
      break;
   case GL_FLOAT_VEC2: // fall through
   case  GL_INT_VEC2: // fall through
   case  GL_BOOL_VEC2:
      slots = count;
      elems = 2;
      break;
   case  GL_INT_VEC3: // fall through
   case  GL_BOOL_VEC3: // fall through
   case  GL_FLOAT_VEC3: // fall through
      slots = count;
      elems = 3;
      break;
   case  GL_INT_VEC4: // fall through
   case  GL_FLOAT_VEC4: // fall through
   case  GL_BOOL_VEC4: // fall through
      slots = count;
      elems = 4;
      break;
   default:
      assert(0);
   }
   if (0 < start)
      return -1;
   if (start + slots > program->Uniforms->Slots)
      return -1;
   for (int i = 0; i < slots; i++)
      memcpy(program->ValuesUniform + start + i, values, elems * sizeof(float));
   return start;
}

static void ShaderUniformMatrix(const GGLInterface * iface, gl_shader_program * program,
                                GLint cols, GLint rows, GLint location, GLsizei count,
                                GLboolean transpose, const GLfloat *values)
{
   if (location == -1)
      return;
   assert(cols == rows);
   int start = location, slots = cols * count;
   if (start < 0 || start + slots > program->Uniforms->Slots)
      return gglError(GL_INVALID_OPERATION);
   for (unsigned i = 0; i < slots; i++) {
      float * column = program->ValuesUniform[start + i];
      for (unsigned j = 0; j < rows; j++)
         column[j] = *(values++);
   }
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
   iface->ShaderVaryingLocation = ShaderVaryingLocation;
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
