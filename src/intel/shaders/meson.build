# Copyright © 2023 Intel Corporation
# SPDX-License-Identifier: MIT

intel_float64_spv_h = custom_target(
  'float64_spv.h',
  input : [glsl2spirv, float64_glsl_file],
  output : 'float64_spv.h',
  command : [
    prog_python, '@INPUT@', '@OUTPUT@',
    prog_glslang,
    '--create-entry', 'main',
    '--vn', 'float64_spv_source',
    '--glsl-version', '450',
    '-Olib',
  ]
)

intel_shader_files = files(
  'libintel_shaders.h',
  'generate.cl',
  'generate_draws.cl',
  'generate_draws_iris.cl',
  'memcpy.cl',
  'query_copy.cl',
)

intel_shaders_gens = [ [ 80,   8],
                       [ 90,   9],
                       [110,  11],
                       [120,  12],
                       [125, 125],
                       [200,  20] ]
intel_shaders = []
foreach gen : intel_shaders_gens
  intel_shaders += 'intel_gfx@0@_shaders_code.h'.format(gen[1])
endforeach

idep_intel_shaders = declare_dependency(
  sources : intel_shaders,
  include_directories : include_directories('.'),
)
