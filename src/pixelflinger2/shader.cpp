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

#include <llvm/LLVMContext.h>

static gl_shader * ShaderCreate(const GGLInterface * iface, GLenum type)
{
    if (GL_VERTEX_SHADER != type && GL_FRAGMENT_SHADER != type)
    {
        gglError(GL_INVALID_ENUM);
        return NULL;
    }
//    gl_shader * shader = _mesa_new_shader(0, type);
//    if(!shader)
//        gglError(GL_OUT_OF_MEMORY);
//    return shader;
    return NULL;
}

static GLboolean ShaderCompile(const GGLInterface * iface, gl_shader * shader,
                               const char * glsl, char ** infoLog)
{
    GGL_GET_CONST_CONTEXT(ctx, iface);
    if (!glsl)
    {
        gglError(GL_INVALID_VALUE);
        return GL_FALSE; 
    }
//    shader->Source = glsl;
//    _slang_compile(ctx->glCtx, shader);
//    shader->Source = NULL;
//    if (infoLog)
//        *infoLog = shader->InfoLog;
//    return shader->CompileStatus;
   return GL_FALSE;
}

static void ShaderFree(const GGLInterface * iface, gl_shader * shader)
{
    GGL_GET_CONST_CONTEXT(ctx, iface);
//    _mesa_free_shader(ctx->glCtx, shader);
}

static gl_shader_program * ShaderProgramCreate(const GGLInterface * iface)
{
//    gl_shader_program * program = _mesa_new_shader_program(0);
//    if (!program)
//        gglError(GL_OUT_OF_MEMORY);
//    return program;
   return NULL;
}

static GLboolean ShaderProgramLink(const GGLInterface * iface, gl_shader_program * program,
                                   const unsigned count, gl_shader ** shaders, char ** infoLog)
{
    GGL_GET_CONST_CONTEXT(ctx, iface);
    
//    program->NumShaders = count;
//    program->Shaders = shaders;
//    
//    _slang_link(ctx->glCtx, program);
//    
//    program->NumShaders = 0;
//    program->Shaders = NULL;
//    
//    if (infoLog)
//        *infoLog = program->InfoLog;
//    
//    return program->LinkStatus;
   return GL_FALSE;
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
    if (!program)
    {
//        ctx->glCtx->Shader.CurrentProgram = NULL;
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
//    ctx->glCtx->Shader.CurrentProgram = program;
}

static void ShaderProgramFree(const GGLInterface * iface, gl_shader_program * program)
{
    GGL_GET_CONST_CONTEXT(ctx, iface);
//    if (ctx->glCtx->Shader.CurrentProgram == program)
//    {
//        ctx->glCtx->Shader.CurrentProgram = NULL;
//        SetShaderVerifyFunctions(const_cast<GGLInterface *>(iface));
//    }
//    assert(program);
//    if (program->GLVMVP)
//        gallivm_prog_delete(ctx->glCtx->Shader.cpu, program->GLVMVP);
//    program->GLVMVP = NULL;
//    if (program->GLVMFP)
//        gallivm_prog_delete(ctx->glCtx->Shader.cpu, program->GLVMFP);
//    program->GLVMFP = NULL;
//    SAFE_FREE(program->STVP);
//    SAFE_FREE(program->STFP);
//    _mesa_free_shader_program(ctx->glCtx, program);
}

static void ShaderAttributeBind(const GGLInterface * iface, const gl_shader_program * program, 
                               GLuint index, const GLchar * name)
{
    GGL_GET_CONST_CONTEXT(ctx, iface);
//    _mesa_bind_attrib_location(ctx->glCtx, program, index, name, DEFAULTP);
}

static GLint ShaderAttributeLocation(const GGLInterface * iface, const gl_shader_program * program,
                                     const char * name)
{
    GGL_GET_CONST_CONTEXT(ctx, iface);
//    return _mesa_get_attrib_location(ctx->glCtx, program, name);
   return -2;
}

static GLint ShaderUniformLocation(const GGLInterface * iface, const gl_shader_program * program,
                                   const char * name)
{
    GGL_GET_CONST_CONTEXT(ctx, iface);
//    return _mesa_get_shader_uniform_location(ctx->glCtx, program, name);
   return -2;
}

static void ShaderUniformGetfv(const GGLInterface * iface, gl_shader_program * program,
                               GLint location, GLfloat * params)
{
    GGL_GET_CONST_CONTEXT(ctx, iface);
//    _mesa_get_uniformfv(ctx->glCtx, program, location, params);
}

static void ShaderUniformGetiv(const GGLInterface * iface, gl_shader_program * program,
                               GLint location, GLint * params)
{
    GGL_GET_CONST_CONTEXT(ctx, iface);
//    _mesa_get_uniformiv(ctx->glCtx, program, location, params);
}

static GLint ShaderUniform(const GGLInterface * iface, gl_shader_program * program,
                          GLint location, GLsizei count, const GLvoid *values, GLenum type)
{
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
//    if (ctx->glCtx->Shader.CurrentProgram)
//    {
//        ShaderUse(const_cast<GGLInterface *>(iface), ctx->glCtx->Shader.CurrentProgram);
//        if (ShaderVerifyProcessVertex != iface->ProcessVertex)
//            iface->ProcessVertex(iface, input, output);
//    }
}

static void ShaderVerifyDrawTriangle(const GGLInterface * iface, const VertexInput * v0, 
                                     const VertexInput * v1, const VertexInput * v2)
{
//    GGL_GET_CONST_CONTEXT(ctx, iface);
//    if (ctx->glCtx->Shader.CurrentProgram)
//    {
//        ShaderUse(const_cast<GGLInterface *>(iface), ctx->glCtx->Shader.CurrentProgram);
//        if (ShaderVerifyDrawTriangle != iface->DrawTriangle)
//            iface->DrawTriangle(iface, v0, v1, v2);
//    }
}

static void ShaderVerifyRasterTriangle(const GGLInterface * iface, const VertexOutput * v1, 
                                       const VertexOutput * v2, const VertexOutput * v3)
{
//    GGL_GET_CONST_CONTEXT(ctx, iface);
//    if (ctx->glCtx->Shader.CurrentProgram)
//    {
//        ShaderUse(const_cast<GGLInterface *>(iface), ctx->glCtx->Shader.CurrentProgram);
//        if (ShaderVerifyRasterTriangle != iface->RasterTriangle)
//            iface->RasterTriangle(iface, v1, v2, v3);
//    }
}

static void ShaderVerifyRasterTrapezoid(const GGLInterface * iface, const VertexOutput * tl, 
                                        const VertexOutput * tr, const VertexOutput * bl,
                                        const VertexOutput * br)
{
    GGL_GET_CONST_CONTEXT(ctx, iface);
//    if (ctx->glCtx->Shader.CurrentProgram)
//    {
//        ShaderUse(const_cast<GGLInterface *>(iface), ctx->glCtx->Shader.CurrentProgram);
//        if (ShaderVerifyRasterTrapezoid != iface->RasterTrapezoid)
//            iface->RasterTrapezoid(iface, tl, tr, bl, br);
//    }
}

static void ShaderVerifyScanLine(const GGLInterface * iface, const VertexOutput * v1,
                                 const VertexOutput * v2)
{
    GGL_GET_CONST_CONTEXT(ctx, iface);
//    if (ctx->glCtx->Shader.CurrentProgram)
//    {
//        ShaderUse(const_cast<GGLInterface *>(iface), ctx->glCtx->Shader.CurrentProgram);
//        if (ShaderVerifyScanLine != iface->ScanLine)
//            iface->ScanLine(iface, v1, v2);
//    }
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

void InitializeShaderFunctions(struct GGLInterface * iface)
{
    GGL_GET_CONTEXT(ctx, iface);
	puts("ctx->llvmCtx = new llvm::LLVMContext");
    ctx->llvmCtx = new llvm::LLVMContext();
    printf("ctx->llvmCtx=%p \n", ctx->llvmCtx);
	
    iface->ShaderCreate = ShaderCreate;
    iface->ShaderCompile = ShaderCompile;
    iface->ShaderFree = ShaderFree;
    iface->ShaderProgramCreate = ShaderProgramCreate;
    iface->ShaderProgramLink = ShaderProgramLink;
    iface->ShaderUse = ShaderUse;
    iface->ShaderProgramFree = ShaderProgramFree;
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
//    if (ctx->glCtx->Shader.cpu)
//    {
//        gallivm_cpu_engine_delete(ctx->glCtx->Shader.cpu);   
//        ctx->glCtx->Shader.cpu = NULL;
//    }
//    SAFE_DELETE(ctx->llvmCtx);
}