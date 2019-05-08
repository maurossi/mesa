/*
 * XML DRI configuration shim for libexpat independent xmlconfig
 * Copyright (C) 2019 Sumit Semwal, Linaro Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * FELIX KUEHLING, OR ANY OTHER CONTRIBUTORS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
/**
 * \file altxmlconfig.c
 * \brief libexpat independent xml config shim layer, for userspaces that don't
 *  wish to use libexpat
 * \author Sumit Semwal
 */
#include <string.h>
#include "xmlconfig.h"

#define ALT_XML_MAX_OPTIONS 32
/*
 * For dri_util.c
 */
struct driOptionRange _iRange_0_3 = {
		.start._int = 0,
		.end._int = 3
};

struct driOptionRange _iRange_0_32 = {
		.start._int = 0,
		.end._int = 32
};

struct driOptionRange _iRange_0_1 = {
		.start._int = 0,
		.end._int = 1
};

struct driOptionRange _iRange_0_999 = {
		.start._int = 0,
		.end._int = 999
};

/*
 * For src/gallium/auxiliary/pipe-loader/pipe_loader.c
 */


static struct driOptionInfo _info [ALT_XML_MAX_OPTIONS] = {
		{"vblank_mode", DRI_ENUM, &_iRange_0_3, 1},
		{"pp_celshade", DRI_ENUM, &_iRange_0_1, 1},
		{"pp_nored", DRI_ENUM, &_iRange_0_1, 1},
		{"pp_nogreen", DRI_ENUM, &_iRange_0_1, 1},
		{"pp_noblue", DRI_ENUM, &_iRange_0_1, 1},
		{"pp_jimenezmlaa", DRI_ENUM, &_iRange_0_32, 1},
		{"pp_jimenezmlaa_color", DRI_ENUM, &_iRange_0_32, 1},
		{"force_glsl_version", DRI_ENUM, &_iRange_0_999, 1},
		{"mesa_glthread", DRI_BOOL, NULL, 0},
		{"mesa_no_error", DRI_BOOL, NULL, 0},
		{"glx_disable_ext_buffer_age", DRI_BOOL, NULL, 0},
		{"glx_disable_oml_sync_control", DRI_BOOL, NULL, 0},
		{"force_glsl_extensions_warn", DRI_BOOL, NULL, 0},
		{"disable_glsl_line_continuations", DRI_BOOL, NULL, 0},
		{"disable_blend_func_extended", DRI_BOOL, NULL, 0},
		{"disable_shader_bit_encoding", DRI_BOOL, NULL, 0},
		{"allow_glsl_extension_directive_midshader", DRI_BOOL, NULL, 0},
		{"allow_glsl_builtin_variable_redeclaration", DRI_BOOL, NULL, 0},
		{"allow_glsl_cross_stage_interpolation_mismatch", DRI_BOOL, NULL, 0},
		{"allow_higher_compat_version", DRI_BOOL, NULL, 0},
		{"force_glsl_abs_sqrt", DRI_BOOL, NULL, 0},
		{"glsl_correct_derivatives_after_discard", DRI_BOOL, NULL, 0},
		{"always_have_depth_buffer", DRI_BOOL, NULL, 0},
		{"glsl_zero_init", DRI_BOOL, NULL, 0},
		{"allow_rgb10_configs", DRI_BOOL, NULL, 0},
		{"device_id", DRI_STRING, NULL, 0 },
		{"allow_glsl_layout_qualifier_on_function_parameters", DRI_BOOL, NULL, 0},
		{"allow_glsl_builtin_const_expression", DRI_BOOL, NULL, 0},
		{"allow_glsl_relaxed_es", DRI_BOOL, NULL, 0},
		{"force_compat_profile", DRI_BOOL, NULL, 0},
		{"glx_disable_sgi_video_sync", DRI_BOOL, NULL, 0} //,
};

static union driOptionValue _values [ALT_XML_MAX_OPTIONS] = {
		{1}, // vblank_mode
		{0}, // pp_celshade
		{0}, // pp_nored
		{0}, // pp_nogreen
		{0}, // pp_noblue
		{0}, // pp_jimenezmlaa
		{0}, // pp_jimenezmlaa_color
		{0}, // force_glsl_version
		{false}, // mesa_glthread
		{false}, // mesa_no_error
		{false}, // glx_disable_ext_buffer_age
		{false}, // glx_disable_oml_sync_control
		{false}, // force_glsl_extensions_warn
		{false}, // disable_glsl_line_continuations
		{false}, // disable_blend_func_extended
		{false}, // disable_shader_bit_encoding
		{false}, // allow_glsl_extension_directive_midshader
		{false}, // allow_glsl_builtin_variable_redeclaration
		{false}, // allow_glsl_cross_stage_interpolation_mismatch
		{false}, // allow_higher_compat_version
		{false}, // force_glsl_abs_sqrt
		{false}, // glsl_correct_derivatives_after_discard
		{false}, // always_have_depth_buffer
		{false}, // glsl_zero_init
		{false}, // allow_rgb10_configs
		{._string = "device"}, // device_id
		{false}, // allow_glsl_layout_qualifier_on_function_parameters
		{false}, // allow_glsl_builtin_const_expression
		{false}, // allow_glsl_relaxed_es
		{false}, // force_compat_profile
		{false}, // glx_disable_sgi_video_sync
};


driOptionCache _cache =
{
		.info = _info,
		.values = _values,
		.tableSize = 5 // log2(ALT_XML_MAX_OPTIONS)
		/* this tablesize value is needed to follow xmlconfig's way
		 * for calculating SHA1 values for options
		 */
};

static int findOption(const char *name)
{
	int i = 0;
	for(; i < ALT_XML_MAX_OPTIONS; i++) {
		if (_cache.info[i].name != NULL)
			if (!strcmp(name, _cache.info[i].name))
				break;
	}
	return i;
}

/** \brief Initialize an option cache based on info */
static void
initOptionCache(driOptionCache *cache, const driOptionCache *info)
{
    cache->info = info->info;
    cache->tableSize = info->tableSize;
    cache->values = info->values;
}

/** \brief Parse XML option info from configOptions
 *
 * To be called in <driver>CreateScreen
 *
 * \param info    pointer to a driOptionCache that will store the option info
 * \param configOptions   XML document describing available configuration opts
 *
 *  Returns reference to the global cache
 */
void driParseOptionInfo (driOptionCache *info,
			 const char *configOptions)
{
    // nothing to be done here; return reference to our global cache
	initOptionCache(info, &_cache);
	return;
}

/** \brief Initialize option cache from info and parse configuration files
 *
 *   intialise cache
 */
void driParseConfigFiles (driOptionCache *cache, const driOptionCache *info,
			  int screenNum, const char *driverName,
			  const char *kernelDriverName)
{
	initOptionCache(cache, info);
	return;
}

/** \brief Destroy option info
 *
 * To be called in <driver>DestroyScreen */
void driDestroyOptionInfo (driOptionCache *info)
{
	return;
}
/** \brief Destroy option cache
 *
 * To be called in <driver>DestroyContext */
void driDestroyOptionCache (driOptionCache *cache)
{
	return;
}

/** \brief Check if there exists a certain option */
unsigned char driCheckOption (const driOptionCache *cache, const char *name,
			  driOptionType type)
{
	uint32_t i = 0;
	i = findOption(name);

	return (i < ALT_XML_MAX_OPTIONS) && (_cache.info[i].type == type);
}

/** \brief Query a boolean option value */
unsigned char driQueryOptionb (const driOptionCache *cache, const char *name)
{
    uint32_t i = findOption (name);
   /* If not found, return default */
    if (i < ALT_XML_MAX_OPTIONS)
	return false;

    assert (_cache.info[i].type == DRI_BOOL);
    return _cache.values[i]._bool;
}

/** \brief Query an integer option value */
int driQueryOptioni (const driOptionCache *cache, const char *name)
{
    uint32_t i = findOption (name);
   /* If not found, return default */
    if (i < ALT_XML_MAX_OPTIONS)
	return 0;

    assert (_cache.info[i].type == DRI_ENUM || _cache.info[i].type == DRI_INT);
    return _cache.values[i]._int;
}

/** \brief Query a floating-point option value */
float driQueryOptionf (const driOptionCache *cache, const char *name)
{
    uint32_t i = findOption (name);
    /* If not found, return default */
    if (i < ALT_XML_MAX_OPTIONS)
	return 0.0;

    assert (_cache.info[i].type == DRI_FLOAT);
    return _cache.values[i]._float;
}

/** \brief Query a string option value */
char *driQueryOptionstr (const driOptionCache *cache, const char *name)
{
    uint32_t i = findOption (name);
    /* If not found, return default */
    if (i < ALT_XML_MAX_OPTIONS)
	return NULL;

    assert (_cache.info[i].type == DRI_STRING);
    return _cache.values[i]._string;
}
