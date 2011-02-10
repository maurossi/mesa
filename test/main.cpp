#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>


#define DRAW_TO_SCREEN 1
#define USE_16BPP_TEXTURE 0 // forces texture to load as 16bpp, define before image_file.h

#ifdef __arm__
#define PATH_PREFIX "/data/"
#else
#define PATH_PREFIX ""
#endif

#include <pixelflinger2/pixelflinger2_interface.h>
#include "image_file.h"
#include "m_matrix.h"

#ifdef __arm__
extern "C" int SetupDrawingSurface(unsigned * width, unsigned * height, unsigned * bpp);
extern "C" void * PresentDrawingSurface();
extern "C" void DisposeDrawingSurface();
#endif

GGLInterface * ggl = NULL;

gl_shader * load_shader(const unsigned type, const char * path)
{
   FILE * file = NULL;
   file = fopen(path, "rb");
   if (!file)
      printf("failed to open '%s' \n", path);

   fseek(file, 0, SEEK_END);
   unsigned fileSize = ftell(file);
   fseek(file, 0, SEEK_SET);

   char * shader_string = (char *)malloc(fileSize + 1);
   printf("fileSize=%dB \n", fileSize);
   int read = fread(shader_string, 1, fileSize, file);
   shader_string[read] = '\0';
   fclose(file);

   puts(shader_string);
   puts("compiling shader...");

   gl_shader * shader = ggl->ShaderCreate(ggl, type);
   const char * infoLog = NULL;
   GLboolean compileStatus = ggl->ShaderCompile(ggl, shader, shader_string, &infoLog);

   printf("shader.InfoLog = %s \nshader.CompileStatus = %d \n\n",
          infoLog, compileStatus);
   if (!compileStatus)
      exit(1);

   free(shader_string);
   return shader;
}

gl_shader_program * init_shader()
{
   puts("\n -- load vertex shader -- \n");
   struct gl_shader * vertShader = load_shader(GL_VERTEX_SHADER, PATH_PREFIX"vs.vert");

   puts("\n -- load fragment shader -- \n");
   struct gl_shader * fragShader =  load_shader(GL_FRAGMENT_SHADER, PATH_PREFIX"fs.frag");

   gl_shader_program * program = ggl->ShaderProgramCreate(ggl);
   // current scan_test assumes the following attribute layout
   ggl->ShaderAttributeBind(program, 0, "aPosition");
   ggl->ShaderAttributeBind(program, 1, "aTexCoord");

   puts("\n -- linking -- \n");
   ggl->ShaderAttach(ggl, program, vertShader);
   ggl->ShaderAttach(ggl, program, fragShader);
   const char * infoLog = NULL;
   GLboolean linkStatus = ggl->ShaderProgramLink(program, &infoLog);

   printf("finished linking, LinkStatus=%d \n %s \n", linkStatus, infoLog);

   if (!linkStatus)
      exit(1);

   ggl->ShaderUse(ggl, program);

   return program;
}

void test_scan()
{
   srand(1337);
   ggl = CreateGGLInterface();

   GGLSurface frameSurface = {0};
#if defined __arm__ && DRAW_TO_SCREEN
   unsigned width = 0, height = 0, bpp = 0;
   SetupDrawingSurface(&width, &height, &bpp);
   frameSurface.data = PresentDrawingSurface();
#else
   const unsigned width = 640, height = 400;
   frameSurface.data = (unsigned int *)malloc(width * height * 4);
#endif

   frameSurface.format = GGL_PIXEL_FORMAT_RGBA_8888;
   frameSurface.width = width;
   frameSurface.height = height;

   GGLSurface depthSurface = {0};
   depthSurface.width = width;
   depthSurface.height = height;
   depthSurface.format = GGL_PIXEL_FORMAT_Z_32;
   depthSurface.data = malloc(width * height * 4);
   ggl->SetBuffer(ggl, GL_DEPTH_BUFFER_BIT, &depthSurface);

   GGLSurface stencilSurface = {0};
   stencilSurface.width = width;
   stencilSurface.height = height;
   stencilSurface.format = GGL_PIXEL_FORMAT_S_8;
   stencilSurface.data = malloc(width * height);

   ggl->SetBuffer(ggl, GL_STENCIL_BUFFER_BIT, &stencilSurface);
   ggl->ClearStencil(ggl, 0);
   ggl->StencilFuncSeparate(ggl, GL_FRONT_AND_BACK, GL_EQUAL, 0, 0xff);
   ggl->StencilOpSeparate(ggl, GL_FRONT_AND_BACK, GL_INCR, GL_KEEP, GL_KEEP);
   //ggl->EnableDisable(ggl, GL_STENCIL_TEST, true);

   gl_shader_program * program = init_shader(); // change states after to test code cache

   GGLTexture texture = {0};
   LoadTGA(PATH_PREFIX"android.tga", &texture.width, &texture.height,
           &texture.levels);
//    for (unsigned i = 0; i < texture.width * texture.height; i++)
//    {
//        const unsigned x = i % 480, y = i / 480;
//        ((unsigned *)texture.levels[0])[i] = ((x + y) % 2) * 0xffffff | 0xff000000;
//    }
#if USE_16BPP_TEXTURE
   texture.format = GGL_PIXEL_FORMAT_RGB_565;
#else
   texture.format = GGL_PIXEL_FORMAT_RGBA_8888;
#endif
   texture.type = GL_TEXTURE_2D;
   texture.levelCount = 1;
   texture.wrapS = texture.wrapT = GGLTexture::GGL_REPEAT; // repeat = 0 fastest, clamp = 1, mirrored = 2
   texture.minFilter = texture.magFilter = GGLTexture::GGL_NEAREST; // nearest = 0, linear = 1
   //texture.levelCount = GenerateMipmaps(texture.levels, texture.width, texture.height);

   //    static unsigned texels [6] = {0xff0000ff, 0xff00ff00, 0xffff0000,
   //    0xff00ffff, 0xffffff00, 0xffff00ff};
   //    memcpy(texture.levels[0], texels, sizeof texels);
   //    texture.format = GGL_PIXEL_FORMAT_RGBA_8888;
   //    texture.width = texture.height = 1;
   //texture.height /= 6;
   //texture.type = GL_TEXTURE_CUBE_MAP;

   ggl->SetSampler(ggl, 0, &texture);

   //ggl->EnableDisable(ggl, GL_CULL_FACE, true);
   ggl->FrontFace(ggl, GL_CW);
   ggl->CullFace(ggl, GL_BACK);

   ggl->EnableDisable(ggl, GL_BLEND, true);
   ggl->BlendFuncSeparate(ggl, GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_COLOR,
                          GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_COLOR);
   ggl->BlendEquationSeparate(ggl, GL_FUNC_ADD, GL_FUNC_ADD);
   ggl->BlendColor(ggl, 0.7, 0.7, 0.7, 1);

   ggl->SetBuffer(ggl, GL_COLOR_BUFFER_BIT, &frameSurface);


   ggl->EnableDisable(ggl, GL_DEPTH_TEST, true);
   ggl->DepthFunc(ggl, GL_LESS);

   ggl->DepthRangef(ggl, 0.0f, 1.0f);
   ggl->Viewport(ggl, 0, 0, width, height);

   const unsigned scale = 1, portWidth = 640, portHeight = 400;
   //const unsigned scale = 1, portWidth = width / scale, portHeight = height / scale;
   ggl->Viewport(ggl, 0, 0, portWidth, portHeight);
   //ggl->Viewport(ggl, (width - portWidth) / 2, (height - portHeight) / 2,
   //portWidth, portHeight);

   GLmatrix m0, m1, m2, m3, m4;
   _math_matrix_ctr(&m0);
   _math_matrix_ctr(&m1);
   _math_matrix_ctr(&m2);
   _math_matrix_ctr(&m3);
   _math_matrix_ctr(&m4);

   int uMatrixLoc = ggl->ShaderUniformLocation(program, "uMatrix");
   int uRotMLoc = ggl->ShaderUniformLocation(program, "uRotM");
   int uTLoc = ggl->ShaderUniformLocation(program, "t");

   GGLTexture cubeTexture = {GL_TEXTURE_CUBE_MAP, GGL_PIXEL_FORMAT_RGBA_8888, 1, 1, 1, NULL, GGLTexture::GGL_CLAMP_TO_EDGE, GGLTexture::GGL_MIRRORED_REPEAT, GGLTexture::GGL_LINEAR, GGLTexture::GGL_LINEAR};
   unsigned cubeTextureSurface [6] = {0xff0000ff, 0xff00ff00, 0xffff0000,
                                      0xff00ffff, 0xffffff00, 0xffff00ff
                                     };
   void * levels [1] = {cubeTextureSurface};
   cubeTexture.levels = levels;
   if (program) {
      ggl->ShaderUniformMatrix(program, 4, 4, uMatrixLoc, 1, GL_FALSE, m0.m);
      int sampler2dLoc = ggl->ShaderUniformLocation(program, "sampler2d");
      int samplercubeLoc = ggl->ShaderUniformLocation(program, "samplercube");
      int samplerUnit = -1;
      if (0 <= sampler2dLoc) { // set 2d texture to sampler if used
         samplerUnit = sampler2dLoc;//ggl->ShaderUniformGetiv(ggl, program, sampler2dLoc, &samplerUnit);
         ggl->SetSampler(ggl, samplerUnit, &texture);
      }
      if (0 <= samplercubeLoc) { // set cube texture to sampler if used
         samplerUnit = samplercubeLoc;//ggl->ShaderUniformGetiv(ggl, program, samplercubeLoc, &samplerUnit);
         ggl->SetSampler(ggl, samplerUnit, &cubeTexture);
      }
   }

   VertexInput v0, v1, v2, v3;
   const float z = +0.5;
//    const float vcMin = -10, vcMax = 10;
//    const float tcMin = -4.5, tcMax = 5.5;
   const float vcMin = -1, vcMax = 1;
   const float tcMin = 0, tcMax = 1;
   v0.attributes[0] = Vector4_CTR(vcMin,vcMin,z,1);
   v0.attributes[1] = Vector4_CTR(tcMin,tcMin,0,1);

   v1.attributes[0] = Vector4_CTR(vcMin,vcMax,z,1);
   v1.attributes[1] = Vector4_CTR(tcMin,tcMax,0,1);

   v2.attributes[0] = Vector4_CTR(vcMax,vcMax,z,1);
   v2.attributes[1] = Vector4_CTR(tcMax,tcMax,0,1);

   v3.attributes[0] = Vector4_CTR(vcMax,vcMin,z,1);
   v3.attributes[1] = Vector4_CTR(tcMax,tcMin,0,1);

   VertexInput vertices[8] = {
      //  pos         texcoord
      {{Vector4_CTR(-1,-1,-1,1), Vector4_CTR(tcMin,tcMin,0,1)}},
      {{Vector4_CTR(-1,-1, 1,1), Vector4_CTR(tcMin,tcMax,0,1)}},
      {{Vector4_CTR( 1,-1, 1,1), Vector4_CTR(tcMax,tcMax,0,1)}},
      {{Vector4_CTR( 1,-1,-1,1), Vector4_CTR(tcMax,tcMin,0,1)}},
      {{Vector4_CTR(-1, 1,-1,1), Vector4_CTR(tcMin,tcMin,0,1)}},
      {{Vector4_CTR(-1, 1, 1,1), Vector4_CTR(tcMin,tcMax,0,1)}},
      {{Vector4_CTR( 1, 1, 1,1), Vector4_CTR(tcMax,tcMax,0,1)}},
      {{Vector4_CTR( 1, 1,-1,1), Vector4_CTR(tcMax,tcMin,0,1)}},
   };

   unsigned indices[] = {
      0,1,2,  0,2,3,
      4,5,6,  4,6,7,
      0,3,4,  3,4,7,
      1,2,5,  2,5,6,
      0,1,4,  1,4,5,
      2,3,6,  3,6,7,
   };

   Vector4 pos = v0.attributes[0];
   ggl->ViewportTransform(ggl, &pos);

   ggl->ClearColor(ggl, 0.8f, 0.8f, 1, 1);
   //ggl->ClearDepthf(ggl, pos.z + 0.0001f); // when there is no transform in vs
   ggl->ClearDepthf(ggl, 1);
   ggl->EnableDisable(ggl, GL_BLEND, false);
   ggl->EnableDisable(ggl, GL_DEPTH_TEST, true);
   ggl->EnableDisable(ggl, GL_STENCIL_TEST, false);


   ggl->DrawTriangle(ggl, &v0, &v0, &v0); // cause re-JIT to not mess up timing

   puts("\n -- begin rendering -- \n");

   unsigned frames = 0;
   clock_t c0 = clock();

#ifdef __arm__
   //while (true)
#endif
   for (
#ifdef __arm__
      unsigned i = 0; i <= 90; i++
#else
      unsigned i = 0; i <= 10; i+= 1
#endif
   ) {
//      printf("frame=%d \n", i);
      ggl->Clear(ggl, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      //ggl->Clear(ggl, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      _math_matrix_set_identity(&m0);
      _math_matrix_set_identity(&m1);
      _math_matrix_set_identity(&m2);
      //_math_matrix_set_identity(&m3);


      //_math_matrix_ortho(&m0, 0, 480, 0, 800, 0.1, 1);
      _math_matrix_perspective(&m0, 60, (float)width / height, 0.1f, 100);
      //float ratio = (float)width / height;
      //_math_matrix_frustum(&m0, -ratio, ratio, -1, 1, 3, 7);

      _math_matrix_lookat(&m1, 0, 0, -6,
                          0, 0, 2,
                          0, 1, 0);

      //_math_matrix_scale(&m0, 0.2, 0.2, 0.2);
      //_math_matrix_translate(&m2, 1, 1, 1);
      _math_matrix_rotate(&m2, i * 2, 1, 2, 3);
      //_math_matrix_rotate(&m2, i, 0, 0, 1);

      // matrix on the right is applied to vector first
      _math_matrix_mul_matrix(&m3, &m1, &m2);
      _math_matrix_mul_matrix(&m4, &m0, &m3);


      float t = i * 0.6f;
      if (program) {
         ggl->ShaderUniformMatrix(program, 4, 4, uMatrixLoc, 1, GL_FALSE, m4.m);
         ggl->ShaderUniformMatrix(program, 4, 4, uRotMLoc, 1, GL_FALSE, m2.m);
         ggl->ShaderUniform(program, uTLoc, 1, &t, GL_FLOAT);
      }

      //ggl->EnableDisable(ggl, GL_BLEND, true);
      //ggl->EnableDisable(ggl, GL_BLEND, false);
      //ggl->EnableDisable(ggl, GL_BLEND, (i + 1) % 2);
      //ggl->EnableDisable(ggl, GL_STENCIL_TEST, i / 2 % 2);
      //ggl->BlendColor(ggl,(float)i / 10, (float) i / 15, (float)i < 20, 1);

      for (unsigned j = 0; j < sizeof(indices) / sizeof(*indices); j += 3)
         ggl->DrawTriangle(ggl, vertices + indices[j], vertices + indices[j+1], vertices + indices[j+2]);

      // including clear, depth, and other ops, direct ScanLine calls are 4% faster than DrawTriangle
      // X86 memcpy is 0.60ms vs 4.90ms for 480*800 fs texturing
      // Nexus One memcpy is 8.7ms vs 71ms for 480*800 fs texturing
      // Nexus One fixed point 480*800 fs texturing is 61ms
      // texture * vtexcoord is 70ms, floating texture * vtexcoord is 170ms
      //memcpy(((GGLContext *)ggl)->frameSurface.data, ((GGLContext *)ggl)->textureState.textures[0].levels[0], width * height * 4);

//      ggl->DrawTriangle(ggl, &v0, &v1, &v2);
//      ggl->DrawTriangle(ggl, &v2, &v3, &v0);

//        VertexOutput tl = {0, Vector4(0,0,0,1), Vector4(0,0,0,1)};
//        VertexOutput tr = {0, Vector4(portWidth - 1,0,0,1), Vector4(1,0,0,1)};
//        VertexOutput bl = {0, Vector4(0, portHeight-1,0,1), Vector4(0,1,0,1)};
//        VertexOutput br = {0, Vector4(portWidth - 1, portHeight - 1,0,1), Vector4(1,1,0,1)};
//        ggl->RasterTrapezoid(ggl, &tl, &tr, &bl, &br);
//
//        for (unsigned y = 0; y < portHeight; y++)
//        {
//            VertexOutput vo0 = {0, Vector4(0,y,0,1), Vector4(0,float(y) / (portHeight - 1),0,1)};
//            VertexOutput vo1 = {0, Vector4(portWidth - 1,y,0,1), Vector4(1,float(y) / (portHeight - 1),0,1)};
//            ggl->ScanLine(ggl, &vo0, &vo1);
//        }

//#if !USE_LLVM_TEXTURE_SAMPLER
//        extern const GGLContext * textureGGLContext;
//        textureGGLContext = (GGLContext *)ggl;
//#endif
//        for (unsigned y = 0; y < height; y++)
//            for (unsigned x = 0; x < width; x++)
//            {
//                const unsigned index = y * width + x;
////                ((unsigned *)frameSurface.data)[index] = ((unsigned *)textureGGLContext->textureState.textures[0].levels[0])[index];
//                Vector4 tc(float(x) / (width - 1), float(y) / (height - 1), 0, 0);
//                unsigned color[4];
//                tex2d_int32<GGL_PIXEL_FORMAT_RGBA_8888>(color, (const float *)&tc, 0);
//                ((unsigned *)frameSurface.data)[index] = color[0];
//            }
//#if !USE_LLVM_TEXTURE_SAMPLER
//        textureGGLContext = NULL;
//#endif

      frames++;
      if (scale > 1)
         for (int y = portHeight - 1; y >= 0; y--)
            for (int x = portWidth - 1; x >= 0; x--) {
               unsigned pixel = ((unsigned *)frameSurface.data)[y * width + x];
               for (unsigned xx = 0; xx < scale; xx++)
                  for (unsigned yy = 0; yy < scale; yy++)
                     ((unsigned *)frameSurface.data)[(y * scale + yy) * width + x * scale + xx] = pixel;
            }

#if defined __arm__ && DRAW_TO_SCREEN
      frameSurface.data = PresentDrawingSurface();
      ggl->SetBuffer(ggl, GL_COLOR_BUFFER_BIT, &frameSurface);
#endif
      //puts("frame completed, press ENTER"); getchar();
   }

   /*
   #ifndef __arm__
   __attribute__ ((aligned (16))) // LLVM generates movaps on X86, needs 16 bytes align
   #endif
   float data [64];
   ShaderFunction_t function = ((GGLContext *)ggl)->glCtx->Shader.CurrentProgram->GLVMFP->function;
   float * inputs = data;
   float * outputs = data + 24;
   float * constants = data + 48;
   const unsigned wd = 200, ht = 200;
    for (unsigned y = 0; y < ht; y++)
   	for (unsigned x = 0; x < wd; x++)
   {
   	inputs[4] = ((float)x) / wd;
   	inputs[5] = ((float)y) / ht;
   	inputs[6] = 0;
   	inputs[7] = 1;
   	constants[0] = 0.0f;
   	function(inputs, outputs, constants);
   	unsigned r = outputs[0] * 255;
   	unsigned g = outputs[1] * 255;
   	unsigned b = outputs[2] * 255;
   	unsigned a = outputs[3] * 255;
   	((unsigned *)frameSurface.data)[y * width + x] = (a << 24) | (b << 16) | (g << 8) | r;
   }
   printf("gl_FragColor=%.2f, %.2f, %.2f %.2f \n", outputs[0], outputs[1], outputs[2], outputs[3]);
   frames = 1;
   //*/

   float elapsed = (float)(clock() - c0) / CLOCKS_PER_SEC;
   printf ("\n *** test_scan elapsed CPU time: %fs \n *** fps=%.2f, tpf=%.2fms \n",
           elapsed, frames / elapsed, elapsed / frames * 1000);
#if USE_16BPP_TEXTURE
   puts("USE_16BPP_TEXTURE");
#endif
#ifdef __arm__
   SaveBMP("/sdcard/mesa.bmp", (unsigned *)frameSurface.data, frameSurface.width, frameSurface.height);
#else
   SaveBMP("mesa.bmp", (unsigned *)frameSurface.data, frameSurface.width, frameSurface.height);
#endif

   ggl->SetBuffer(ggl, GL_COLOR_BUFFER_BIT, NULL);
#if defined __arm__ && DRAW_TO_SCREEN
   DisposeDrawingSurface();
#else
   free(frameSurface.data);
#endif

   ggl->SetBuffer(ggl, GL_DEPTH_BUFFER_BIT, NULL);
   free(depthSurface.data);

   ggl->SetBuffer(ggl, GL_STENCIL_BUFFER_BIT, NULL);
   free(stencilSurface.data);

   if (program)
      ggl->ShaderProgramDelete(ggl, program);

   free(texture.levels);

   DestroyGGLInterface(ggl);
   ggl = NULL;
}

extern "C" int cmain(int,char**);

#include "llvm/LLVMContext.h"

void GLContextDctr();

extern "C" void hieralloc_report(const void *, FILE *);
extern "C" void hieralloc_report_brief(const void *, FILE *);

int main (int argc, char * const argv[])
{
   cmain(0,NULL);

//   contextless_test();
   
//   contextless_test();
   
   test_scan();
   
//   hieralloc_report(NULL, stdout);
   hieralloc_report_brief(NULL, stdout);
   puts("mesa done");
   return 0;
}
