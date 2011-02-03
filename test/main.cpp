/*
 * Copyright © 2008, 2009 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include <cstdlib>
#include <cstdio>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <pixelflinger2/pixelflinger2_interface.h>
#include "../src/talloc/hieralloc.h"

GGLInterface * ggl = NULL;

/* Returned string will have 'ctx' as its hieralloc owner. */
static char *
load_text_file(void *ctx, const char *file_name)
{
   char *text = NULL;
   struct stat st;
   ssize_t total_read = 0;
   int fd = open(file_name, O_RDONLY);

   if (fd < 0) {
      return NULL;
   }

   if (fstat(fd, & st) == 0) {
      text = (char *) hieralloc_size(ctx, st.st_size + 1);
      if (text != NULL) {
         do {
            ssize_t bytes = read(fd, text + total_read,
                                 st.st_size - total_read);
            if (bytes < 0) {
               free(text);
               text = NULL;
               break;
            }

            if (bytes == 0) {
               break;
            }

            total_read += bytes;
         } while (total_read < st.st_size);

         text[total_read] = '\0';
      }
   }

   close(fd);

   return text;
}

int glsl_es = 0;
int dump_ast = 0;
int dump_hir = 0;
int dump_lir = 0;
int do_link = 0;
int do_jit = 0;

const struct option compiler_opts[] = {
   { "glsl-es",  0, &glsl_es,  1
   }, { "dump-ast", 0, &dump_ast, 1 }, { "dump-hir", 0, &dump_hir, 1 }, { "dump-lir", 0, &dump_lir, 1 }, { "link",     0, &do_link,  1 }, { "do-jit",   0, &do_jit,   1 }, { NULL, 0, NULL, 0 }
};

/**
 * \brief Print proper usage and exit with failure.
 */
void
usage_fail(const char *name)
{

   const char *header =
      "usage: %s [options] <file.vert | file.geom | file.frag>\n"
      "\n"
      "Possible options are:\n";
   printf(header, name, name);
   for (const struct option *o = compiler_opts; o->name != 0; ++o) {
      printf("    --%s\n", o->name);
   }
   exit(EXIT_FAILURE);
}

#define DRAW_TO_SCREEN 1
#include "image_file.h"

#if defined __arm__ && defined DRAW_TO_SCREEN
extern "C" int SetupDrawingSurface(unsigned * width, unsigned * height, unsigned * bpp);
extern "C" void * PresentDrawingSurface();
extern "C" void DisposeDrawingSurface();
#endif

void execute(const gl_shader_program * program)
{
   puts("execute");
   
   //const gl_shader * shader = ctx->glCtx->CurrentProgram->_LinkedShaders[MESA_SHADER_FRAGMENT];
   const gl_shader * shader = NULL;
#if defined __arm__ && DRAW_TO_SCREEN
   unsigned width = 0, height = 0, bpp = 0;
   int err = SetupDrawingSurface(&width, &height, &bpp);
   assert(!err);
   assert(32 == bpp);
   unsigned * frameSurface = (unsigned *)PresentDrawingSurface();
#else
   const unsigned width = 1280, height = 800;
   unsigned * frameSurface = new unsigned [width * height];
#endif
   //const unsigned scale = 16, portWidth = 80, portHeight = 50;
   unsigned scale = 1, portWidth = width / scale, portHeight = height / scale;
   //unsigned scale = 1, portWidth = width / 4, portHeight = height / 4;

   GGLSurface colorSurface = {width, height, GGL_PIXEL_FORMAT_RGBA_8888, frameSurface, width, sizeof(GGLSurface)};
   ggl->SetBuffer(ggl, GL_COLOR_BUFFER_BIT, &colorSurface);
   ggl->EnableDisable(ggl, GL_DEPTH_TEST, false);

//   float * uniform = (float *)ctx->glCtx->CurrentProgram->ValuesUniform;
//   float * attribute = (float *)ctx->glCtx->CurrentProgram->ValuesVertexInput;
//   float * varying = (float *)ctx->glCtx->CurrentProgram->ValuesVertexOutput;
//   float * output = ((VertexOutput*)ctx->glCtx->CurrentProgram->ValuesVertexOutput)->fragColor[0].f;
//   int glFragColorLocation = 0;
   int vTexCoordLocation = ggl->ShaderVaryingLocation(ggl, program, "vTexCoord", NULL);
//   if (shader->symbols->get_variable("vTexCoord"))
//      vTexCoordLocation = shader->symbols->get_variable("vTexCoord")->location;
//   int vNormalLocation = -1;
//   if (shader->symbols->get_variable("vNormal"))
//      vNormalLocation = shader->symbols->get_variable("vNormal")->location;
//   if (shader->symbols->get_variable("uRotM") && 0) {
//      ir_variable * var = shader->symbols->get_variable("uRotM");
//      float * matrix = uniform + 4 * 1 + 4 * shader->symbols->get_variable("uRotM")->location;
//      memset(matrix, 0, 16 * sizeof(*matrix));
//      matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1;
//      matrix[28] = 0;
//      matrix[29] = 0;
//      matrix[30] = 0;
//      matrix[31] = 0;
//   }
//   printf("executing... \n function=%p \n", shader->function);

   /*
   #ifdef __arm__
   {
      volatile unsigned wait = 1;
      printf("waiting for attach, set wait(%p) to 0 \n", &wait);
      puts("");
      while (wait);
   }
   #endif
   //*/



   unsigned frames = 1;
   clock_t c0 = clock();

   //while(true)
   for (frames = 1; frames <= 20; frames++) {
      for (unsigned y = 0; y < portHeight; y++) {
         VertexOutput v0, v1;
         v0.position = Vector4(0, y, 0, 0);
         v1.position = Vector4(portWidth - 1, y ,0 ,0);
         if (vTexCoordLocation > -1) {
            v0.varyings[vTexCoordLocation - 2] = Vector4(0, (float)y / (portHeight - 1), 0, 1);
            v1.varyings[vTexCoordLocation - 2] = Vector4(1, (float)y / (portHeight - 1), 0, 1);
         }
         ggl->ScanLine(ggl, &v0, &v1);

//         for (unsigned x = 0; x < portWidth; x++) {
//            if (vTexCoordLocation > -1)
//            {
//               varying[vTexCoordLocation * 4 + 0] = ((float)x) / (portWidth - 1);
//               varying[vTexCoordLocation * 4 + 1] = ((float)y) / (portHeight - 1);
//               varying[vTexCoordLocation * 4 + 2] = 0;
//               varying[vTexCoordLocation * 4 + 3] = 1;
//            }
////            if (vNormalLocation > -1)
////            {
////               varying[vNormalLocation * 4 + 0] = 0;
////               varying[vNormalLocation * 4 + 1] = 1;
////               varying[vNormalLocation * 4 + 2] = 0;
////               varying[vNormalLocation * 4 + 3] = 1;
////            }
//            shader->function();
//            unsigned r = output[0] * 255;
//            unsigned g = output[1] * 255;
//            unsigned b = output[2] * 255;
//            unsigned a = output[3] * 255;
//            frameSurface[y * width + x] = (a << 24) | (b << 16) | (g << 8) | r;
//         }
      }
      //*
      if (scale > 1)
         for (int y = portHeight - 1; y >= 0; y--)
            for (int x = portWidth - 1; x >= 0; x--) {
               unsigned pixel = ((unsigned *)frameSurface)[y * width + x];
               for (unsigned xx = 0; xx < scale; xx++)
                  for (unsigned yy = 0; yy < scale; yy++)
                     ((unsigned *)frameSurface)[(y * scale + yy) * width + x * scale + xx] = pixel;
            }
      //*/
#if defined __arm__ && DRAW_TO_SCREEN
      frameSurface = (unsigned *)PresentDrawingSurface();
      colorSurface.data = frameSurface;
      ggl->SetBuffer(ggl, GL_COLOR_BUFFER_BIT, &colorSurface);
#endif
   }

   float elapsed = (float)(clock() - c0) / CLOCKS_PER_SEC;
   printf ("\n *** test_scan elapsed CPU time: %fs \n *** fps=%.2f, tpf=%.2fms \n",
           elapsed, frames / elapsed, elapsed / frames * 1000);
   //printf("gl_FragColor=%.2f, %.2f, %.2f %.2f \n", output[0], output[1], output[2], output[3]);
#if defined __arm__
   SaveBMP("/sdcard/mesa.bmp", frameSurface, width, height);
#else
   SaveBMP("mesa.bmp", frameSurface, width, height);
#endif
   //assert(0.1f < output[3]);
#if DRAW_TO_SCREEN
   void DisposeDrawingSurface();
#else
   delete frameSurface;
#endif

}

int
main(int argc, char **argv)
{
   static char basePath [256] = {0};
   static char texturePath [256] = {0};
   static char fragPath [256] = {0};
   static char vertPath [256] = {0};
   static char cubeTexturePath [256] = {0};
   static const char fragFile[] = "fs.frag";
   static const char vertFile[] = "vs.vert";
   static const char textureFile[] = "android.tga";
   static const char cubeTextureFile[] = "cube.tga";

   strncpy(basePath, argv[0], strrchr(argv[0], '/') - argv[0] + 1);
   strcpy(fragPath, basePath);
   strcat(fragPath, fragFile);
   strcpy(vertPath, basePath);
   strcat(vertPath, vertFile);
   strcpy(texturePath, basePath);
   strcat(texturePath, textureFile);
   strcpy(cubeTexturePath, basePath);
   strcat(cubeTexturePath, cubeTextureFile);
   //*
   if (1 == argc) {
      const char * args [] = {argv[0], "--dump-hir", "--do-jit", "--link", "--glsl-es", fragPath, vertPath};
      argc = sizeof(args) / sizeof(*args);
      argv = (char **)args;
   }
   //*/
   int status = EXIT_SUCCESS;
   int c;
   int idx = 0;
   while ((c = getopt_long(argc, argv, "", compiler_opts, &idx)) != -1)
      /* empty */ ;


   if (argc <= optind)
      usage_fail(argv[0]);

   //initialize_context(ctx, (glsl_es) ? API_OPENGLES2 : API_OPENGL);
   ggl = CreateGGLInterface();

   struct gl_shader_program * program = ggl->ShaderProgramCreate(ggl);
   for (/* empty */; argc > optind; optind++) {
      const unsigned len = strlen(argv[optind]);
      if (len < 6)
         usage_fail(argv[0]);

      const char *const ext = & argv[optind][len - 5];
      GLenum Type;
      if (strncmp(".vert", ext, 5) == 0)
         Type = GL_VERTEX_SHADER;
      else if (strncmp(".geom", ext, 5) == 0)
         //Type = GL_GEOMETRY_SHADER;
         assert(0);
      else if (strncmp(".frag", ext, 5) == 0)
         Type = GL_FRAGMENT_SHADER;
      else
         usage_fail(argv[0]);

      struct gl_shader * shader = ggl->ShaderCreate(ggl, Type);


      char * source = load_text_file(program, argv[optind]);
      if (source == NULL) {
         printf("File \"%s\" does not exist.\n", argv[optind]);
         exit(EXIT_FAILURE);
      }

      char * infoLog = NULL;
      if (!ggl->ShaderCompile(ggl, shader, source, &infoLog)) {
         printf("Info log for %s:\n%s\n", argv[optind], infoLog);
         status = EXIT_FAILURE;
         break;
      }
      hieralloc_free(source);
      ggl->ShaderAttach(ggl, program, shader);
   }

   puts("link");

   if ((status == EXIT_SUCCESS) && do_link)  {
      const char * infoLog = NULL;
      bool linkStatus = ggl->ShaderProgramLink(ggl, program, &infoLog);
      status = linkStatus ? EXIT_SUCCESS : EXIT_FAILURE;
      assert(linkStatus);
      if (strlen(infoLog) > 0)
         printf("Info log for linking:\n%s\n", infoLog);
   }

   puts("jit");

   GGLTexture texture = {0};
   LoadTGA(texturePath, &texture.width, &texture.height, &texture.levels);
   texture.format = GGL_PIXEL_FORMAT_RGBA_8888;
   texture.type = GL_TEXTURE_2D;
   texture.levelCount = 1;
   texture.wrapS = texture.wrapT = 0; // repeat = 0 fastest, clamp = 1, mirrored = 2
   texture.minFilter = texture.magFilter = 0; // nearest = 0, linear = 1
   ggl->SetSampler(ggl, 0, &texture);

   ggl->ShaderUse(ggl, program);

   texture.minFilter = texture.magFilter = 1; // nearest = 0, linear = 1
   ggl->SetSampler(ggl, 0, &texture);

   ggl->ShaderUse(ggl, program);

   static unsigned cubeTextureSurface [6] = {0xff0000ff, 0xff00ff00, 0xffff0000,
         0xff00ffff, 0xffffff00, 0xffff00ff
                                            };
   GGLTexture cubeTexture = {GL_TEXTURE_CUBE_MAP, GGL_PIXEL_FORMAT_RGBA_8888, 1, 1, 1, cubeTextureSurface, 1, 2, 1, 1};


   int samplerLocation = -1;
   if (0 <= (samplerLocation = ggl->ShaderUniformLocation(ggl, program, "samp2D")))
      ggl->SetSampler(ggl, samplerLocation, &texture);
   if (0 <= (samplerLocation = ggl->ShaderUniformLocation(ggl, program, "samp2DA")))
      ggl->SetSampler(ggl, samplerLocation, &texture);
   if (0 <= (samplerLocation = ggl->ShaderUniformLocation(ggl, program, "sampCube")))
      ggl->SetSampler(ggl, samplerLocation, &texture);

   execute(program);
//      puts("\n *** IR for JIT *** \n");
//      //_mesa_print_ir(ir, NULL);
//
//      shader->executable = hieralloc_zero(shader, Executable);
//      llvm::Module * module = glsl_ir_to_llvm_module(shader->ir, (GGLContext *)ggl);
//      assert(module);
//      shader->executable->module = module;
//      puts("\n *** Module for JIT *** \n");
//      //module->dump();
//      jit(shader, program, (GGLContext *)ggl);
//      puts("jitted");

   free(texture.levels);

   ggl->ShaderProgramDelete(ggl, program);

   DestroyGGLInterface((GGLInterface *)ggl);

   printf("mesa exit(%d) \n", status);
   hieralloc_report_brief(NULL, stdout);
   return status;
}
