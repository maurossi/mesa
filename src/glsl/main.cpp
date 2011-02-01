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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "ast.h"
#include "glsl_parser_extras.h"
#include "glsl_parser.h"
#include "ir_optimization.h"
#include "ir_print_visitor.h"
#include "program.h"
#include "loop_analysis.h"

#include "ir_to_llvm.h"

#include "src/pixelflinger2/pixelflinger2.h"

GGLInterface * ggl = NULL;
   
extern "C" struct gl_shader *
_mesa_new_shader(struct gl_context *ctx, GLuint name, GLenum type);

extern "C" void
_mesa_reference_shader(struct gl_context *ctx, struct gl_shader **ptr,
                       struct gl_shader *sh);

/* Copied from shader_api.c for the stand-alone compiler.
 */
void
_mesa_reference_shader(struct gl_context *ctx, struct gl_shader **ptr,
                       struct gl_shader *sh)
{
   *ptr = sh;
}

struct gl_shader *
_mesa_new_shader(struct gl_context *ctx, GLuint name, GLenum type)
{
   struct gl_shader *shader;

   (void) ctx;

   assert(type == GL_FRAGMENT_SHADER || type == GL_VERTEX_SHADER);
   shader = hieralloc_zero(NULL, struct gl_shader);
   if (shader) {
      shader->Type = type;
      shader->Name = name;
      shader->RefCount = 1;
   }
   return shader;
}

static void
initialize_context(struct gl_context *ctx, gl_api api)
{
   memset(ctx, 0, sizeof(*ctx));

   ctx->API = api;

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
}

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
   { "glsl-es",  0, &glsl_es,  1 },
   { "dump-ast", 0, &dump_ast, 1 },
   { "dump-hir", 0, &dump_hir, 1 },
   { "dump-lir", 0, &dump_lir, 1 },
   { "link",     0, &do_link,  1 },
   { "do-jit",   0, &do_jit,   1 },
   { NULL, 0, NULL, 0 }
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


void
compile_shader(struct gl_context *ctx, struct gl_shader *shader)
{
   struct _mesa_glsl_parse_state *state =
      new(shader) _mesa_glsl_parse_state(ctx, shader->Type, shader);

   const char *source = shader->Source;
   state->error = preprocess(state, &source, &state->info_log,
			     state->extensions, ctx->API);

   if (!state->error) {
      _mesa_glsl_lexer_ctor(state, source);
      _mesa_glsl_parse(state);
      _mesa_glsl_lexer_dtor(state);
   }

   if (dump_ast) {
      foreach_list_const(n, &state->translation_unit) {
	 ast_node *ast = exec_node_data(ast_node, n, link);
	 ast->print();
      }
      printf("\n\n");
   }

   shader->ir = new(shader) exec_list;
   if (!state->error && !state->translation_unit.is_empty())
      _mesa_ast_to_hir(shader->ir, state);

   /* Print out the unoptimized IR. */
   if (!state->error && dump_hir) {
      validate_ir_tree(shader->ir);
      _mesa_print_ir(shader->ir, state);
   }

   /* Optimization passes */
   if (!state->error && !shader->ir->is_empty()) {
      bool progress;
      do {
	 progress = do_common_optimization(shader->ir, false, 32);
      } while (progress);

      validate_ir_tree(shader->ir);
   }


   /* Print out the resulting IR */
   if (!state->error && dump_lir) {
      _mesa_print_ir(shader->ir, state);
   }

   shader->symbols = state->symbols;
   shader->CompileStatus = !state->error;
   shader->Version = state->language_version;
   memcpy(shader->builtins_to_link, state->builtins_to_link,
	  sizeof(shader->builtins_to_link[0]) * state->num_builtins_to_link);
   shader->num_builtins_to_link = state->num_builtins_to_link;

   if (shader->InfoLog)
      hieralloc_free(shader->InfoLog);

   shader->InfoLog = state->info_log;

   /* Retain any live IR, but trash the rest. */
   reparent_ir(shader->ir, shader);

   hieralloc_free(state);

   return;
}

struct SymbolLookupContext
{
   const GGLContext * gglCtx;
   const gl_shader_program * program;
   const gl_shader * shader;
};

#define DRAW_TO_SCREEN 1
#include "image_file.h"

#if defined __arm__ && defined DRAW_TO_SCREEN
extern "C" int SetupDrawingSurface(unsigned * width, unsigned * height, unsigned * bpp);
extern "C" void * PresentDrawingSurface();
extern "C" void DisposeDrawingSurface();
#endif 

void execute(SymbolLookupContext * ctx)
{
   const gl_shader * shader = ctx->shader;
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
   //unsigned scale = 1, portWidth = width / scale, portHeight = height / scale;
   unsigned scale = 1, portWidth = width / 4, portHeight = height / 4;
      
   float * uniform = (float *)ctx->program->ValuesUniform;
   float * attribute = (float *)ctx->program->ValuesVertexInput;
   float * varying = (float *)ctx->program->ValuesVertexOutput;
   float * output = ((VertexOutput*)ctx->program->ValuesVertexOutput)->fragColor[0].f;
   int glFragColorLocation = 0;
   int vTexCoordLocation = -1;
   if (shader->symbols->get_variable("vTexCoord"))
      vTexCoordLocation = shader->symbols->get_variable("vTexCoord")->location;
   int vNormalLocation = -1;
   if (shader->symbols->get_variable("vNormal"))
      vNormalLocation = shader->symbols->get_variable("vNormal")->location;
   if (shader->symbols->get_variable("uRotM") && 0)
   {
      ir_variable * var = shader->symbols->get_variable("uRotM");
      float * matrix = uniform + 4 * 1 + 4 * shader->symbols->get_variable("uRotM")->location;
      memset(matrix, 0, 16 * sizeof(*matrix));
      matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1;
      matrix[28] = 0;
      matrix[29] = 0;
      matrix[30] = 0;
      matrix[31] = 0;
   }
   printf("executing... \n function=%p \n", shader->function);

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
   for (frames = 1; frames <= 10; frames++)
   {
      for (unsigned y = 0; y < portHeight; y++)
         for (unsigned x = 0; x < portWidth; x++) {
            if (vTexCoordLocation > -1)
            {
               varying[vTexCoordLocation * 4 + 0] = ((float)x) / (portWidth - 1);
               varying[vTexCoordLocation * 4 + 1] = ((float)y) / (portHeight - 1);
               varying[vTexCoordLocation * 4 + 2] = 0;
               varying[vTexCoordLocation * 4 + 3] = 1;
            }
            if (vNormalLocation > -1)
            {
               varying[vNormalLocation * 4 + 0] = 0;
               varying[vNormalLocation * 4 + 1] = 1;
               varying[vNormalLocation * 4 + 2] = 0;
               varying[vNormalLocation * 4 + 3] = 1;
            }
            shader->function();
            unsigned r = output[0] * 255;
            unsigned g = output[1] * 255;
            unsigned b = output[2] * 255;
            unsigned a = output[3] * 255;
//            unsigned r = *(unsigned *)(outputs + 0);
//            unsigned g = *(unsigned *)(outputs + 1);
//            unsigned b = *(unsigned *)(outputs + 2);
//            unsigned a = *(unsigned *)(outputs + 3);
            frameSurface[y * width + x] = (a << 24) | (b << 16) | (g << 8) | r;
//            frameSurface[y * width + x] = *(unsigned *)outputs;
         }
      //*
      if (scale > 1)
         for (int y = portHeight - 1; y >= 0; y--)
            for (int x = portWidth - 1; x >= 0; x--)
            {
               unsigned pixel = ((unsigned *)frameSurface)[y * width + x];
               for (unsigned xx = 0; xx < scale; xx++)
                  for (unsigned yy = 0; yy < scale; yy++)
                     ((unsigned *)frameSurface)[(y * scale + yy) * width + x * scale + xx] = pixel;
            }
      //*/
#if defined __arm__ && DRAW_TO_SCREEN
      frameSurface = (unsigned *)PresentDrawingSurface();
#endif
   }

   float elapsed = (float)(clock() - c0) / CLOCKS_PER_SEC;
   printf ("\n *** test_scan elapsed CPU time: %fs \n *** fps=%.2f, tpf=%.2fms \n",
           elapsed, frames / elapsed, elapsed / frames * 1000);
   printf("gl_FragColor=%.2f, %.2f, %.2f %.2f \n", output[0], output[1], output[2], output[3]);
   //assert(0.1f < outputs[3]);
#if defined __arm__
   SaveBMP("/sdcard/mesa.bmp", frameSurface, width, height);
#else
   SaveBMP("mesa.bmp", frameSurface, width, height);
#endif
#if DRAW_TO_SCREEN
   void DisposeDrawingSurface();
#else
   delete frameSurface;
#endif
 
}

#if USE_LLVM_EXECUTIONENGINE

#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Target/TargetSelect.h>

/*void jit(llvm::Module * mod, gl_shader * shader)
{
#ifndef __arm__
   __attribute__ ((aligned (16))) // LLVM generates movaps on X86, needs 16 bytes align
#endif
   float data [64];
   memset(data, 0xff, sizeof(data));

   llvm::InitializeNativeTarget();

   std::string errorString;
   llvm::EngineBuilder engineBuilder(mod);
   engineBuilder.setEngineKind(llvm::EngineKind::JIT);
   engineBuilder.setErrorStr(&errorString);
#ifdef __arm__
   engineBuilder.setMAttrs(llvm::SmallVector<std::string, 1>(1,"vfp3"));
   mod->setTargetTriple("armv7-none-linux-gnueabi");
#endif

   llvm::ExecutionEngine * ee = engineBuilder.create();
   if (!ee)
      puts(errorString.c_str());
   assert(ee);

   ee->DisableLazyCompilation();

   if ((mod->getFunction("putchar")))
      ee->updateGlobalMapping(mod->getFunction("putchar"), (void *)putchar);
   if ((mod->getFunction("sinf")))
      ee->updateGlobalMapping(mod->getFunction("sinf"), (void *)sinf);
   if ((mod->getFunction("cosf")))
      ee->updateGlobalMapping(mod->getFunction("cosf"), (void *)cosf);
   if ((mod->getFunction("powf")))
      ee->updateGlobalMapping(mod->getFunction("powf"), (void *)cosf);

   ee->updateGlobalMapping(mod->getGlobalVariable("gl_FragColor"), (void *)(data + 0));
   ee->updateGlobalMapping(mod->getGlobalVariable("gl_FragCoord"), (void *)(data + 4));
   ee->updateGlobalMapping(mod->getGlobalVariable("gl_FrontFacing"), (void *)(data + 8));
   ee->updateGlobalMapping(mod->getGlobalVariable("vTexCoord"), (void *)(data + 12));
   ee->updateGlobalMapping(mod->getGlobalVariable("t"), (void *)(data + 36));

   llvm::Function * func = mod->getFunction("main");
   assert(func);

   void (* function)() = (void (*)())ee->getPointerToFunction(func);
   execute(function, data);
   puts("USE_LLVM_EXECUTIONENGINE");
}*/

#else

#include <bcc/bcc.h>
#include <dlfcn.h>

static void* symbolLookup(void* pContext, const char* name)
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
      else
      {
         for (unsigned i = 0; i < program->Uniforms->NumUniforms && !symbol; i++)
            if (!strcmp(program->Uniforms->Uniforms[i].Name, name))
               symbol = program->ValuesUniform + program->Uniforms->Uniforms[i].Pos;
         for (unsigned i = 0; i < program->Attributes->NumParameters && !symbol; i++)
            if (!strcmp(program->Attributes->Parameters[i].Name, name))
            {
               assert(program->Attributes->Parameters[i].Location
                  < sizeof(VertexInput) / sizeof(float[4]));
               symbol = program->ValuesVertexInput + program->Attributes->Parameters[i].Location;
            }
         for (unsigned i = 0; i < program->Varying->NumParameters && !symbol; i++)
            if (!strcmp(program->Varying->Parameters[i].Name, name))
            {
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
   //getchar();
   assert(symbol);
   return (void *)symbol;
}

void jit(gl_shader * shader, gl_shader_program * program, const GGLContext * gglCtx)
{
   SymbolLookupContext ctx = {gglCtx, program, shader};
   
   BCCScriptRef script = bccCreateScript();
   bccReadModule(script, "glsl", (LLVMModuleRef)shader->module, 0);
   int result = 0;
   assert(0 == bccGetError(script));
   bccRegisterSymbolCallback(script, symbolLookup, &ctx);
   assert(0 == bccGetError(script));
   bccPrepareExecutable(script, NULL, 0);
   result = bccGetError(script);
   if (result != 0) {
      puts("failed bcc_compile");
      assert(0);
      return;
   }

   shader->function = (void (*)())bccGetFuncAddr(script, "main");
   result = bccGetError(script);
   if (result != BCC_NO_ERROR)
      fprintf(stderr, "Could not find '%s': %d\n", "main", result);
   else
      printf("bcc_compile %s=%p \n", "main", shader->function);
   
   if (GL_FRAGMENT_SHADER == shader->Type)
      execute(&ctx);
}

#endif

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
   struct gl_context local_ctx;
   struct gl_context *ctx = &local_ctx;

   int c;
   int idx = 0;
   while ((c = getopt_long(argc, argv, "", compiler_opts, &idx)) != -1)
      /* empty */ ;


   if (argc <= optind)
      usage_fail(argv[0]);

   initialize_context(ctx, (glsl_es) ? API_OPENGLES2 : API_OPENGL);
   ggl = CreateGGLInterface();
   
   struct gl_shader_program *whole_program;

   whole_program = hieralloc_zero (NULL, struct gl_shader_program);
   assert(whole_program != NULL);
   whole_program->Attributes = hieralloc_zero(whole_program, gl_program_parameter_list);
   whole_program->Varying = hieralloc_zero(whole_program, gl_program_parameter_list);
   for (/* empty */; argc > optind; optind++) {
      whole_program->Shaders = (struct gl_shader **)
         hieralloc_realloc(whole_program, whole_program->Shaders,
         struct gl_shader *, whole_program->NumShaders + 1);
      assert(whole_program->Shaders != NULL);

      struct gl_shader *shader = hieralloc_zero(whole_program, gl_shader);

      whole_program->Shaders[whole_program->NumShaders] = shader;
      whole_program->NumShaders++;

      const unsigned len = strlen(argv[optind]);
      if (len < 6)
	 usage_fail(argv[0]);

      const char *const ext = & argv[optind][len - 5];
      if (strncmp(".vert", ext, 5) == 0)
	 shader->Type = GL_VERTEX_SHADER;
      else if (strncmp(".geom", ext, 5) == 0)
	 shader->Type = GL_GEOMETRY_SHADER;
      else if (strncmp(".frag", ext, 5) == 0)
	 shader->Type = GL_FRAGMENT_SHADER;
      else
	 usage_fail(argv[0]);

      shader->Source = load_text_file(whole_program, argv[optind]);
      if (shader->Source == NULL) {
	 printf("File \"%s\" does not exist.\n", argv[optind]);
	 exit(EXIT_FAILURE);
      }

      compile_shader(ctx, shader);

      if (!shader->CompileStatus) {
	 printf("Info log for %s:\n%s\n", argv[optind], shader->InfoLog);
	 status = EXIT_FAILURE;
	 break;
      }
   }

   puts("link");
   
   if ((status == EXIT_SUCCESS) && do_link)  {
      link_shaders(ctx, whole_program);
      status = (whole_program->LinkStatus) ? EXIT_SUCCESS : EXIT_FAILURE;

      if (strlen(whole_program->InfoLog) > 0)
         printf("Info log for linking:\n%s\n", whole_program->InfoLog);
   }

   for (unsigned i = 0; i < whole_program->Attributes->NumParameters; i++)
   {
      const gl_program_parameter & attribute = whole_program->Attributes->Parameters[i];
      printf("attribute '%s': location=%d slots=%d \n", attribute.Name, attribute.Location, attribute.Slots);
   }
   for (unsigned i = 0; i < whole_program->Varying->NumParameters; i++)
   {
      const gl_program_parameter & varying = whole_program->Varying->Parameters[i];
      printf("varying '%s': vs_location=%d fs_location=%d \n", varying.Name, varying.BindLocation, varying.Location);
   }
   for (unsigned i = 0; i < whole_program->Uniforms->NumUniforms; i++)
   {
      const gl_uniform & uniform = whole_program->Uniforms->Uniforms[i];
      printf("uniform '%s': location=%d type=%s \n", uniform.Name, uniform.Pos, uniform.Type->name);
   }
         
   puts("jit");

   GGLTexture texture = {0};  
   LoadTGA(texturePath, &texture.width, &texture.height, &texture.levels);
   texture.format = GGL_PIXEL_FORMAT_RGBA_8888;
   texture.type = GL_TEXTURE_2D;
   texture.levelCount = 1;
   texture.wrapS = texture.wrapT = 0; // repeat = 0 fastest, clamp = 1, mirrored = 2
   texture.minFilter = texture.magFilter = 1; // nearest = 0, linear = 1
   ggl->SetSampler(ggl, 0, &texture);
   
   static unsigned cubeTextureSurface [6] = {0xff0000ff, 0xff00ff00, 0xffff0000, 
   0xff00ffff, 0xffffff00, 0xffff00ff};
   GGLTexture cubeTexture = {GL_TEXTURE_CUBE_MAP, GGL_PIXEL_FORMAT_RGBA_8888, 1, 1, 1, cubeTextureSurface, 1, 2, 1, 1};  
   
   for (unsigned i = 0; do_jit && i < MESA_SHADER_TYPES; i++) {
      struct gl_shader *shader = whole_program->_LinkedShaders[i];
      if (!shader)
         continue;
      ir_variable * sampler = NULL;
      if ((sampler = shader->symbols->get_variable("samp2D")) && sampler->location >= 0)
         ggl->SetSampler(ggl, sampler->location, &texture);
      if ((sampler = shader->symbols->get_variable("samp2DA")) && sampler->location >= 0)
         ggl->SetSampler(ggl, sampler->location, &texture); 
      if ((sampler = shader->symbols->get_variable("sampCube")) && sampler->location >= 0)
         ggl->SetSampler(ggl, sampler->location, &cubeTexture);
         
      do_mat_op_to_vec(shader->ir);

      puts("\n *** IR for JIT *** \n");
      //_mesa_print_ir(ir, NULL);

      llvm::Module * module = glsl_ir_to_llvm_module(shader->ir, (GGLContext *)ggl);
      assert(module);
      shader->module = module;
      puts("\n *** Module for JIT *** \n");
      //module->dump();
      jit(shader, whole_program, (GGLContext *)ggl);
   
      puts("jitted");
   }
   
   free(texture.levels);
   DestroyGGLInterface((GGLInterface *)ggl);
   for (unsigned i = 0; i < MESA_SHADER_TYPES; i++)
      hieralloc_free(whole_program->_LinkedShaders[i]);

   hieralloc_free(whole_program);
      
   _mesa_glsl_release_types();
   _mesa_glsl_release_functions();

   printf("mesa exit(%d) \n", status);
   hieralloc_report_brief(NULL, stdout);
   return status;
}
