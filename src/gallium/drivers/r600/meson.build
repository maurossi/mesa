# Copyright © 2017 Intel Corporation
# SPDX-License-Identifier: MIT

files_r600 = files(
  'compute_memory_pool.c',
  'compute_memory_pool.h',
  'eg_debug.c',
  'evergreen_compute.c',
  'evergreen_compute.h',
  'evergreen_compute_internal.h',
  'evergreen_hw_context.c',
  'evergreen_state.c',
  'r600_blit.c',
  'r600_dump.c',
  'r600_dump.h',
  'r600_hw_context.c',
  'r600_pipe.c',
  'r600_pipe.h',
  'r600_public.h',
  'r600_sfn.cpp',
  'r600_sfn.h',
  'r600_shader.c',
  'r600_shader.h',
  'r600_state.c',
  'r600_state_common.c',
  'r600_uvd.c',
  'cayman_msaa.c',
  'r600_buffer_common.c',
  'r600_cs.h',
  'r600_gpu_load.c',
  'r600_perfcounter.c',
  'r600_pipe_common.c',
  'r600_pipe_common.h',
  'r600_query.c',
  'r600_query.h',
  'r600_streamout.c',
  'r600_test_dma.c',
  'r600_texture.c',
  'r600_viewport.c',
  'radeon_uvd.c',
  'radeon_uvd.h',
  'radeon_vce.c',
  'radeon_vce.h',
  'radeon_video.c',
  'radeon_video.h',
  )

egd_tables_h = custom_target(
  'egd_tables.h',
  input : ['egd_tables.py', 'evergreend.h'],
  output : 'egd_tables.h',
  command : [prog_python, '@INPUT@'],
  capture : true,
)

r600_c_args = []
if with_gallium_clover
  if dep_elf.found()
    r600_c_args += '-DHAVE_OPENCL'
  else
    warning('r600 requires libelf to support opencl.')
  endif
endif

libr600 = static_library(
  'r600',
  [files_r600, egd_tables_h, sha1_h],
  c_args : [r600_c_args, '-Wstrict-overflow=0'],
  gnu_symbol_visibility : 'hidden',
  include_directories : [
    inc_src, inc_mapi, inc_mesa, inc_include, inc_gallium, inc_gallium_aux, inc_amd_common,
    inc_gallium_drivers,
  ],
  dependencies: [dep_libdrm_radeon, dep_elf, dep_llvm, idep_amd_terascale_common, idep_amd_terascale_compiler, idep_nir, idep_nir_headers, idep_mesautil],
)

driver_r600 = declare_dependency(
  compile_args : '-DGALLIUM_R600',
  link_with : [libr600, libmesa, libradeonwinsys],
)

if with_tests
  subdir('sfn/tests')
endif


