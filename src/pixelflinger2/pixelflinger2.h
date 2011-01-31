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

#ifndef _PIXELFLINGER2_H_
#define _PIXELFLINGER2_H_

#define USE_LLVM_TEXTURE_SAMPLER 1
#define USE_LLVM_SCANLINE 1

#ifndef USE_LLVM_EXECUTIONENGINE
#define USE_LLVM_EXECUTIONENGINE 0 // 1 to use llvm::Execution, 0 to use libBCC, requires modifying makefile
#endif

#define debug_printf printf

#include "pixelflinger2/pixelflinger2_interface.h"

#ifndef MIN2
#  define MIN2(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX2
#  define MAX2(a, b) ((a) > (b) ? (a) : (b))
#endif

namespace llvm {
    class LLVMContext;
};

#if !USE_LLVM_SCANLINE
typedef int BlendComp_t;
#endif

#define GGL_GET_CONTEXT(context, interface) GGLContext * context = (GGLContext *)interface;
#define GGL_GET_CONST_CONTEXT(context, interface) const GGLContext * context = \
    (const GGLContext *)interface; (void)context;

struct GGLContext
{
    GGLInterface interface; // must be first member so that GGLContext * == GGLInterface *
    
    GGLSurface frameSurface;
    GGLSurface depthSurface;
    GGLSurface stencilSurface;
    
    struct __GLcontextRec * glCtx; // mesa constants and others used for shader compiling and executing
    llvm::LLVMContext * llvmCtx;
    
    struct 
    {
        int depth; // assuming ieee 754 32 bit float and 32 bit 2's complement int; z_32
        unsigned color; // clear value; rgba_8888
        unsigned stencil; // s_8; repeated to clear 4 pixels at a time
    } clearState; 
    
    struct StencilState
    {
        unsigned char ref, mask; // ref is masked during StencilFuncSeparate
        
        // GL_NEVER = 0, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, 
        // GL_ALWAYS; value = GLenum  & 0x7 (GLenum is 0x200-0x207) 
        unsigned char func; // compare function
        
        // GL_ZERO = 0, GL_KEEP = 1, GL_REPLACE, GL_INCR, GL_DECR, GL_INVERT, GL_INCR_WRAP, 
        // GL_DECR_WRAP = 7; value = 0 | GLenum - GL_KEEP | GL_INVERT | GLenum - GL_INCR_WRAP
        unsigned char sFail, dFail, dPass; // operations
    } frontStencil, backStencil; // all affect scanline jit
    
    mutable struct ActiveStencilState // do not change layout, used in GenerateScanLine
    {
        unsigned char face; // FRONT = 0, BACK = 1
        unsigned char ref, mask;
    } activeStencil; // after primitive assembly, call StencilSelect        
    
    struct BufferState // all affect scanline jit
    {        
        unsigned stencilTest : 1; 
        unsigned depthTest : 1;
        // same as sf/bFunc; GL_NEVER = 0, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, 
        // GL_GEQUAL, GL_ALWAYS = 7; value = GLenum  & 0x7 (GLenum is 0x200-0x207)
        unsigned depthFunc : 3; 
    } bufferState;
    
    struct BlendState // all values affect scanline jit
    {
#if USE_LLVM_SCANLINE
        unsigned char color[4]; // rgba[0,255]
#else
        Vec4<BlendComp_t> color;
#endif        
         
        unsigned scf : 4, saf : 4, dcf : 4, daf : 4; // GL_ZERO = 0, GL_ONE, GL_SRC_COLOR = 2,
        // GL_ONE_MINUS_SRC_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 
        // GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, 
        // GL_SRC_ALPHA_SATURATE, GL_CONSTANT_COLOR = 11, GL_ONE_MINUS_CONSTANT_COLOR,
        // GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA;
        // value = 0,1 | GLenum - GL_SRC_COLOR + 2 | GLenum - GL_CONSTANT_COLOR + 11
        
        unsigned ce : 3, ae : 3; // GL_FUNC_ADD = 0, GL_FUNC_SUBTRACT = 4,
        // GL_FUNC_REVERSE_SUBTRACT = 5; value = GLenum - GL_FUNC_ADD
       
        unsigned enable : 1;
    } blendState;
    
    struct 
    {
        // format affects vs and fs jit
        GGLTexture textures[GGL_MAXCOMBINEDTEXTUREIMAGEUNITS]; // the active samplers
        // array of pointers to texture surface data; used by LLVM generated texture sampler
        void * textureData[GGL_MAXCOMBINEDTEXTUREIMAGEUNITS]; 
        // array of texture dimensions; used by LLVM generated texture sampler
        unsigned textureDimensions[GGL_MAXCOMBINEDTEXTUREIMAGEUNITS * 2];
    } textureState;
    
    // called by ShaderUse to set to proper rendering functions
    void (* PickScanLine)(GGLInterface * iface);
    void (* PickRaster)(GGLInterface * iface);
    
    // viewport params are transformed so that Zw = Zd * f + n
	// and Xw/Yw = x/y + Xd/Yd * w/h
    struct { VectorComp_t x, y, w, h, n, f; } viewport; // should be moved into libAgl2
    
    struct // should be moved into libAgl2
    {
        unsigned enable : 1;
        unsigned frontFace : 1; // GL_CW = 0, GL_CCW, actual value is GLenum - GL_CW
        unsigned cullFace : 2; // GL_FRONT = 0, GL_BACK, GL_FRONT_AND_BACK, value = GLenum - GL_FRONT
    } cullState;
};

#define _PF2_TEXTURE_DATA_NAME_ "gl_PF2TEXTURE_DATA" /* sampler data pointers used by LLVM */
#define _PF2_TEXTURE_DIMENSIONS_NAME_ "gl_PF2TEXTURE_DIMENSIONS" /* sampler dimensions used by LLVM */

void gglError(unsigned error); // not implmented, just an assert

// they just set the function pointers
void InitializeBufferFunctions(GGLInterface * iface);
void InitializeRasterFunctions(GGLInterface * iface);
void InitializeScanLineFunctions(GGLInterface * iface);
void InitializeTextureFunctions(GGLInterface * iface);

void InitializeShaderFunctions(GGLInterface * iface); // set function pointers and create needed objects
void SetShaderVerifyFunctions(GGLInterface * iface); // called by state change functions
void DestroyShaderFunctions(GGLInterface * iface); // destroy needed objects
// actual gl_shader and gl_shader_program is created and destroyed by ShaderCreate/Free, 
// and ShaderProgramCreate/Free. 

#endif // #ifndef _PIXELFLINGER2_H_
