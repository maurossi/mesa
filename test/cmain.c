#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>

#include <pixelflinger2/pixelflinger2_interface.h>

int ApproximatelyEqual(const Vector4 lhs, const Vector4 rhs, const float t)
{
   if (fabs(lhs.x - rhs.x) > t)
      return 0;
   if (fabs(lhs.y - rhs.y) > t)
      return 0;
   if (fabs(lhs.z - rhs.z) > t)
      return 0;
   if (fabs(lhs.w - rhs.w) > t)
      return 0;
   return 1;
}

extern void * llvmCtx;

void contextless_test()
{
   static const char vsGLSL [] =
      "uniform vec4 uVec4; \n"
      "uniform sampler2D sampler2d; \n"
      "attribute vec4 aPosition; \n"
      "attribute vec4 aTexCoord; \n"
      "varying vec4 vTexCoord; \n"
      "varying vec4 vTexColor; \n"
      "void main() { \n"
      "   gl_Position = aPosition; \n"
      "   vTexCoord = aTexCoord; \n"
      "   vTexColor = texture2D(sampler2d, aTexCoord.zw); \n"
      "   gl_PointSize = 432.0; \n"
      "}";
   gl_shader_t * vs = GGLShaderCreate(GL_VERTEX_SHADER);
   const char * infoLog = NULL;
   if (!GGLShaderCompile(vs, vsGLSL, &infoLog)) {
      printf("GGLShaderCompile vs failed:\n%s\n", infoLog);
      assert(0);
   }
   static const char fsGLSL [] =
      "uniform vec4 uVec4; \n"
      "uniform sampler2D sampler2d; \n"
      "varying vec4 vTexCoord; \n"
      "varying vec4 vTexColor; \n"
      "void main() { \n"
      "   gl_FragColor = texture2D(sampler2d, vTexCoord.zw); \n"
      "}";
   gl_shader_t * fs = GGLShaderCreate(GL_FRAGMENT_SHADER);
   if (!GGLShaderCompile(fs, fsGLSL, &infoLog)) {
      printf("GGLShaderCompile fs failed:\n%s\n", infoLog);
      assert(0);
   }
   gl_shader_program_t * prog = GGLShaderProgramCreate();
   unsigned glError = GL_NO_ERROR;
   glError = GGLShaderAttach(prog, vs);
   assert(GL_NO_ERROR == glError);
   glError = GGLShaderAttach(prog, fs);
   assert(GL_NO_ERROR == glError);
   GGLShaderAttributeBind(prog, 4, "aPosition");
   GGLShaderAttributeBind(prog, 5, "aTexCoord");
   if (!GGLShaderProgramLink(prog, &infoLog)) {
      printf("GGLShaderProgramLink failed:\n%s\n", infoLog);
      assert(0);
   }
//   llvm::LLVMContext * llvmCtx = new llvm::LLVMContext();
   GGLState_t gglState = {0};
   unsigned texels0 [] = {0xff10ffff, 0x22222222, 0x66666666, 0xffffffff};
   GGLTexture_t texture0 = {GL_TEXTURE_2D, GGL_PIXEL_FORMAT_RGBA_8888,
                            2, 2, 1, // width, height, levelCount
                            texels0, GGL_CLAMP_TO_EDGE, GGL_CLAMP_TO_EDGE,
                            GGL_NEAREST, GGL_NEAREST
                           };
   gglState.textureState.textures[0] = texture0;
   gglState.textureState.textureData[0] = gglState.textureState.textures[0].levels;
   gglState.textureState.textureDimensions[0 * 2 + 0] = gglState.textureState.textures[0].width;
   gglState.textureState.textureDimensions[0 * 2 + 1] = gglState.textureState.textures[0].height;
   GGLShaderUse(llvmCtx, &gglState, prog);

   VertexInput_t input = {0, 0, 0, 0};
   input.attributes[4] = VECTOR4_CTR(0,0,0,1);
   input.attributes[5] = VECTOR4_CTR(0,0,0,0);
   VertexOutput_t output = {0};
   GGLProcessVertex(prog, &input, &output, NULL);
   int vTexColor = -1;
   GGLShaderVaryingLocation(prog, "vTexColor", &vTexColor);
   if (vTexColor >= 0) {
      if (memcmp(((Vector4 *)&output) + vTexColor, &VECTOR4_CTR(1,1,16/255.0f,1), sizeof(Vector4))) {
         puts("((Vector4 *)&output)[vTexColor] != Vector4(1,1,0,1)");
         assert(0);
      }
   } else {
      puts("vTexColor < 0");
      assert(0);
   }

   static const char fsGLSL1 [] =
      "uniform vec4 uVec4; \n"
      "uniform sampler2D sampler2d; \n"
      "varying vec4 vTexCoord; \n"
      "varying vec4 vTexColor; \n"
      "void main() { \n"
      "   gl_FragColor = vTexColor; \n"
      "}";
   gl_shader_t * fs1 = GGLShaderCreate(GL_FRAGMENT_SHADER);
   if (!GGLShaderCompile(fs1, fsGLSL1, &infoLog)) {
      printf("GGLShaderCompile fs failed:\n%s\n", infoLog);
      assert(0);
   }
   gl_shader_program_t * prog1 = GGLShaderProgramCreate();
   glError = GGLShaderAttach(prog1, vs);
   assert(GL_NO_ERROR == glError);
   glError = GGLShaderAttach(prog1, fs1);
   assert(GL_NO_ERROR == glError);
   GGLShaderAttributeBind(prog1, 1, "aPosition");
   GGLShaderAttributeBind(prog1, 2, "aTexCoord");
   if (!GGLShaderProgramLink(prog1, &infoLog)) {
      printf("GGLShaderProgramLink failed:\n%s\n", infoLog);
      assert(0);
   }

   GGLShaderUse(llvmCtx, &gglState, prog1);
   VertexInput_t input1 = {0};
   input1.attributes[1] = VECTOR4_CTR(1,1,0,1);
   input1.attributes[2] = VECTOR4_CTR(1,1,0,0);
   VertexOutput_t output1 = {0};
   GGLProcessVertex(prog1, &input1, &output1, NULL);
   int vTexCoord = -1;
   assert(2 == GGLShaderAttributeLocation(prog1, "aTexCoord"));
   GGLShaderVaryingLocation(prog1, "vTexCoord", &vTexCoord);
   if (vTexCoord >= 0) {
      if (memcmp(((Vector4 *)&output1) + vTexCoord, input1.attributes + 2, sizeof(Vector4))) {
         puts("((Vector4 *)&output1)[vTexCoord] != input1.attributes[1]");
         assert(0);
      }
   } else {
      puts("vTexCoord < 0");
      assert(0);
   }

   puts("***\n finished contextless_test \n***");

   GGLShaderProgramDelete(prog);
   GGLShaderProgramDelete(prog1);
   
   GLContextDctr();
}

int cmain(int argc, char **argv)
{
   contextless_test();
   
   const char * infoLog = NULL;

   GGLInterface_t * ggl = CreateGGLInterface();

   gl_shader_t * shader0 = ggl->ShaderCreate(ggl, GL_VERTEX_SHADER);
   assert(shader0);
   const char * glsl0 =
      "uniform vec4 uVec4; \n"
      "uniform sampler2D sampler2d; \n"
      "attribute vec4 aPosition; \n"
      "attribute vec4 aTexCoord; \n"
      "varying vec4 vTexCoord; \n"
      "varying vec4 vTexColor; \n"
      "void main() { \n"
      "   gl_Position = aPosition; \n"
      "   vTexCoord = aTexCoord + uVec4; \n"
      "   vTexColor = texture2D(sampler2d, aTexCoord.zw); \n"
      "   gl_PointSize = 432; \n"
      "}";
   puts(glsl0);
   GLboolean compileStatus = ggl->ShaderCompile(ggl, shader0, glsl0, &infoLog);
   if (!compileStatus)
      fprintf(stderr, "failed to compile vertex shader 0, infoLog: \n %s \n", infoLog);
   assert(compileStatus);

   gl_shader_t * shader1 = ggl->ShaderCreate(ggl, GL_FRAGMENT_SHADER);
   assert(shader1);
   const char * glsl1 =
      "uniform vec4 uVec4; \n"
      "uniform sampler2D sampler2d; \n"
      "varying vec4 vTexCoord; \n"
      "varying vec4 vTexColor; \n"
      "void main() { \n"
      "   gl_FragColor = vTexCoord + vTexColor; \n"
      "}";
   puts(glsl1);
   compileStatus = ggl->ShaderCompile(ggl, shader1, glsl1, &infoLog);
   if (!compileStatus)
      fprintf(stderr, "failed to compile fragment shader 0, infoLog: \n %s \n", infoLog);
   assert(compileStatus);

   gl_shader_program_t * program0 = ggl->ShaderProgramCreate(ggl);
   assert(program0);

   ggl->ShaderAttach(ggl, program0, shader0);
   ggl->ShaderAttach(ggl, program0, shader1);
   ggl->ShaderAttributeBind(program0, 2, "aTexCoord");
   ggl->ShaderAttributeBind(program0, 3, "aPosition");

   GLboolean linkStatus = ggl->ShaderProgramLink(program0, &infoLog);
   if (!linkStatus)
      fprintf(stderr, "failed to link program 0, infoLog: \n %s \n", infoLog);
   assert(linkStatus);

   ggl->ShaderUse(ggl, program0);

   unsigned texels0 [] = {0xffffffff, 0x22222222, 0x66666666, 0xffffffff};
   GGLTexture_t texture0 = {GL_TEXTURE_2D, GGL_PIXEL_FORMAT_RGBA_8888,
                            2, 2, 1, // width, height, levelCount
                            texels0, GGL_CLAMP_TO_EDGE, GGL_MIRRORED_REPEAT, GGL_LINEAR, GGL_LINEAR
                           }; // levels, wrapS, wrapT, minFilter, magFilter

   int sampler2dLoc = ggl->ShaderUniformLocation(program0, "sampler2d");
   if (0 <= sampler2dLoc) {
      int samplerUnit = -1;
      //ggl->ShaderUniformGetiv(ggl, program0, sampler2dLoc, &samplerUnit);
      samplerUnit = sampler2dLoc;
      ggl->SetSampler(ggl, samplerUnit, &texture0);
   }

   Vector4 uVec4 = {1.125f, 1.5f, 1.75f, 1.75f};
   int uVec4Loc = ggl->ShaderUniformLocation(program0, "uVec4");
   ggl->ShaderUniform(program0, uVec4Loc, 1, &uVec4, GL_FLOAT_VEC4);

   VertexInput_t v0 = {0};
   v0.attributes[2] = VECTOR4_CTR(0,0,1,1); // aTexCoord
   v0.attributes[3] = VECTOR4_CTR(0.25f, 0.25f, 0.5f,1); // aPosition

   VertexOutput_t vout0 = {0};
   ggl->ProcessVertex(ggl, &v0, &vout0);
   if (memcmp(&vout0.position,&v0.attributes[3],sizeof(vout0.position))) {
      fprintf(stderr, "gl_Position != aPosition \n");
      assert(0);
   }

   int vTexCoordIndex = ggl->ShaderVaryingLocation(program0, "vTexCoord", NULL) - 2;
   VECTOR4_OP_UNARY(vout0.varyings[vTexCoordIndex],-=,uVec4);
   if (memcmp(&vout0.varyings[vTexCoordIndex],&v0.attributes[2],sizeof uVec4)) {
      fprintf(stderr, "vTexCoord != aTexCoord + uVec4 \n");
      assert(0);
   }
   Vector4 ones = {1,1,1,1};
   int vTexColorIndex = ggl->ShaderVaryingLocation(program0, "vTexColor", NULL) - 2;
   if (memcmp(&vout0.varyings[vTexColorIndex],&ones,sizeof ones)) { // should be the last texel color
      fprintf(stderr, "vTexColor != Vector4(1,1,1,1) \n");
      assert(0);
   }
   if (vout0.pointSize.x != 432) {
      fprintf(stderr, "gl_PointSize != 432 \n");
      assert(0);
   }

   v0.attributes[2] = VECTOR4_CTR(0,0, 1.5f, 1.5f);
   texture0.wrapS = GGL_REPEAT;
   texture0.wrapT = GGL_REPEAT;

   sampler2dLoc = ggl->ShaderUniformLocation(program0, "sampler2d");
   if (0 <= sampler2dLoc) {
      int samplerUnit = -1;
      //ggl->ShaderUniformGetiv(ggl, program0, sampler2dLoc, &samplerUnit);
      samplerUnit = sampler2dLoc;
      ggl->SetSampler(ggl, samplerUnit, &texture0);
   }
   ggl->ShaderUse(ggl, program0);
   ggl->ProcessVertex(ggl, &v0, &vout0);
   const float filtered = (float)(0xff + 0x22 + 0x66 + 0xff) / (4 * 0xff);
   if (!ApproximatelyEqual(vout0.varyings[vTexColorIndex],
                           VECTOR4_CTR(filtered, filtered, filtered, filtered), 1.0f / 255)) {
      fprintf(stderr, "failed linear filter and/or wrapS and wrapT test");
      assert(0);
   }

   const unsigned width = 60, height = 100;

   GGLSurface_t colorSurface = {width, height, GGL_PIXEL_FORMAT_RGBA_8888, malloc(width * height * 4)};
   assert(colorSurface.data);
   ggl->SetBuffer(ggl, GL_COLOR_BUFFER_BIT, &colorSurface);

   GGLSurface_t depthSurface = {width, height, GGL_PIXEL_FORMAT_Z_32, malloc(width * height * 4)};
   assert(depthSurface.data);
   ggl->SetBuffer(ggl, GL_DEPTH_BUFFER_BIT, &depthSurface);

   GGLSurface_t stencilSurface = {width, height, GGL_PIXEL_FORMAT_S_8, malloc(width * height * 1)};
   assert(stencilSurface.data);
   ggl->SetBuffer(ggl, GL_STENCIL_BUFFER_BIT, &stencilSurface);

   ggl->ClearColor(ggl, 0.1f, 0.1f, 0.1f, 1.0f);
   ggl->ClearDepthf(ggl, 0.5f);

// TODO DXL test scanline and fs

   free(colorSurface.data);
   colorSurface.data = NULL;
   free(depthSurface.data);
   depthSurface.data = NULL;
   free(stencilSurface.data);
   stencilSurface.data = NULL;

   ggl->ShaderProgramDelete(ggl, program0);

   puts("*******************");
   puts("*** end of test ***");
   puts("*******************");

   DestroyGGLInterface(ggl);
   return 0;
}
