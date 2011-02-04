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

int cmain(int argc, char **argv)
{
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
      "   gl_PointSize = 432.0; \n"
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
   ggl->ShaderAttributeBind(ggl, program0, 2, "aTexCoord");
   ggl->ShaderAttributeBind(ggl, program0, 3, "aPosition");

   GLboolean linkStatus = ggl->ShaderProgramLink(ggl, program0, &infoLog);
   if (!linkStatus)
      fprintf(stderr, "failed to link program 0, infoLog: \n %s \n", infoLog);
   assert(linkStatus);

   ggl->ShaderUse(ggl, program0);

   unsigned texels0 [] = {0xffffffff, 0x22222222, 0x66666666, 0xffffffff};
   GGLTexture_t texture0 = {GL_TEXTURE_2D, GGL_PIXEL_FORMAT_RGBA_8888,
                            2, 2, 1, // width, height, levelCount
                            texels0, 1, 2, 1, 1
                           }; // levels, wrapS, wrapT, minFilter, magFilter

   int sampler2dLoc = ggl->ShaderUniformLocation(ggl, program0, "sampler2d");
   if (0 <= sampler2dLoc) {
      int samplerUnit = -1;
      //ggl->ShaderUniformGetiv(ggl, program0, sampler2dLoc, &samplerUnit);
      samplerUnit = sampler2dLoc;
      ggl->SetSampler(ggl, samplerUnit, &texture0);
   }

   Vector4 uVec4 = {1.125f, 1.5f, 1.75f, 1.75f};
   int uVec4Loc = ggl->ShaderUniformLocation(ggl, program0, "uVec4");
   ggl->ShaderUniform(ggl, program0, uVec4Loc, 1, &uVec4, GL_FLOAT_VEC4);

   VertexInput_t v0 = {0};
   v0.attributes[2] = VECTOR4_CTR(0,0,1,1); // aTexCoord
   v0.attributes[3] = VECTOR4_CTR(0.25f, 0.25f, 0.5f,1); // aPosition

   VertexOutput_t vout0 = {0};
   ggl->ProcessVertex(ggl, &v0, &vout0);
   if (memcmp(&vout0.position,&v0.attributes[3],sizeof(vout0.position))) {
      fprintf(stderr, "gl_Position != aPosition \n");
      assert(0);
   }

   int vTexCoordIndex = ggl->ShaderVaryingLocation(ggl, program0, "vTexCoord", NULL) - 2;
   VECTOR4_OP_UNARY(vout0.varyings[vTexCoordIndex],-=,uVec4);
   if (memcmp(&vout0.varyings[vTexCoordIndex],&v0.attributes[2],sizeof uVec4)) {
      fprintf(stderr, "vTexCoord != aTexCoord + uVec4 \n");
      assert(0);
   }
   Vector4 ones = {1,1,1,1};
   int vTexColorIndex = ggl->ShaderVaryingLocation(ggl, program0, "vTexColor", NULL) - 2;
   if (memcmp(&vout0.varyings[vTexColorIndex],&ones,sizeof ones)) { // should be the last texel color
      fprintf(stderr, "vTexColor != Vector4(1,1,1,1) \n");
      assert(0);
   }
   if (vout0.pointSize.x != 432) {
      fprintf(stderr, "gl_PointSize != 432 \n");
      assert(0);
   }

// TODO DXL linear filtering needs to be fixed for texcoord outside of [0,1]
   v0.attributes[2] = VECTOR4_CTR(0,0, 0.5f, 0.5f);
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

// TODO DXL scanline and fs test

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
