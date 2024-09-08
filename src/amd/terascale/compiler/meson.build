# Copyright © 2023 Vitaliy Triang3l Kuzmin

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

amd_terascale_compiler_files = files(
  '../../../gallium/drivers/r600/sfn/sfn_alu_defines.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_alu_defines.h',
  '../../../gallium/drivers/r600/sfn/sfn_alu_readport_validation.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_alu_readport_validation.h',
  '../../../gallium/drivers/r600/sfn/sfn_assembler.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_assembler.h',
  '../../../gallium/drivers/r600/sfn/sfn_callstack.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_callstack.h',
  '../../../gallium/drivers/r600/sfn/sfn_conditionaljumptracker.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_conditionaljumptracker.h',
  '../../../gallium/drivers/r600/sfn/sfn_defines.h',
  '../../../gallium/drivers/r600/sfn/sfn_debug.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_debug.h',
  '../../../gallium/drivers/r600/sfn/sfn_instr.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_instr.h',
  '../../../gallium/drivers/r600/sfn/sfn_instr_alu.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_instr_alu.h',
  '../../../gallium/drivers/r600/sfn/sfn_instr_alugroup.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_instr_alugroup.h',
  '../../../gallium/drivers/r600/sfn/sfn_instr_controlflow.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_instr_controlflow.h',
  '../../../gallium/drivers/r600/sfn/sfn_instr_export.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_instr_export.h',
  '../../../gallium/drivers/r600/sfn/sfn_instr_fetch.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_instr_fetch.h',
  '../../../gallium/drivers/r600/sfn/sfn_instr_mem.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_instr_mem.h',
  '../../../gallium/drivers/r600/sfn/sfn_instr_lds.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_instr_lds.h',
  '../../../gallium/drivers/r600/sfn/sfn_instr_tex.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_instr_tex.h',
  '../../../gallium/drivers/r600/sfn/sfn_instrfactory.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_instrfactory.h',
  '../../../gallium/drivers/r600/sfn/sfn_liverangeevaluator.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_liverangeevaluator.h',
  '../../../gallium/drivers/r600/sfn/sfn_liverangeevaluator_helpers.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_liverangeevaluator_helpers.h',
  '../../../gallium/drivers/r600/sfn/sfn_memorypool.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_memorypool.h',
  '../../../gallium/drivers/r600/sfn/sfn_nir.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_nir.h',
  '../../../gallium/drivers/r600/sfn/sfn_nir_legalize_image_load_store.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_nir_lower_64bit.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_nir_lower_alu.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_nir_lower_alu.h',
  '../../../gallium/drivers/r600/sfn/sfn_nir_lower_tex.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_nir_lower_tex.h',
  '../../../gallium/drivers/r600/sfn/sfn_nir_lower_fs_out_to_vector.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_nir_lower_fs_out_to_vector.h',
  '../../../gallium/drivers/r600/sfn/sfn_nir_lower_tess_io.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_nir_vectorize_vs_inputs.c',
  '../../../gallium/drivers/r600/sfn/sfn_optimizer.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_peephole.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_ra.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_ra.h',
  '../../../gallium/drivers/r600/sfn/sfn_scheduler.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_scheduler.h',
  '../../../gallium/drivers/r600/sfn/sfn_shader.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_shader.h',
  '../../../gallium/drivers/r600/sfn/sfn_shader_cs.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_shader_cs.h',
  '../../../gallium/drivers/r600/sfn/sfn_shader_fs.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_shader_fs.h',
  '../../../gallium/drivers/r600/sfn/sfn_shader_gs.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_shader_gs.h',
  '../../../gallium/drivers/r600/sfn/sfn_shader_tess.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_shader_tess.h',
  '../../../gallium/drivers/r600/sfn/sfn_shader_vs.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_shader_vs.h',
  '../../../gallium/drivers/r600/sfn/sfn_split_address_loads.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_split_address_loads.h',
  '../../../gallium/drivers/r600/sfn/sfn_valuefactory.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_valuefactory.h',
  '../../../gallium/drivers/r600/sfn/sfn_virtualvalues.cpp',
  '../../../gallium/drivers/r600/sfn/sfn_virtualvalues.h',
)

amd_terascale_compiler_cpp_args = []
if cpp.has_type('std::pmr::monotonic_buffer_resource', prefix : '#include <memory_resource>')
  amd_terascale_compiler_cpp_args += '-DHAVE_MEMORY_RESOURCE'
endif

amd_terascale_compiler_includes = [
  inc_amd_common,
  inc_compiler,
]

libamd_terascale_compiler = static_library(
  'amd_terascale_compiler',
  [amd_terascale_compiler_files],
  cpp_args : amd_terascale_compiler_cpp_args,
  include_directories : [
    amd_terascale_compiler_includes,
    # TODO(Triang3l): Remove the following once r600_pipe_common.h isn't referenced anymore.
    inc_gallium,
    inc_gallium_aux,
  ],
  dependencies : [idep_amd_terascale_common, idep_mesautil, idep_nir, idep_nir_headers],
  gnu_symbol_visibility : 'hidden',
)

idep_amd_terascale_compiler = declare_dependency(
  link_with : libamd_terascale_compiler,
  include_directories : amd_terascale_compiler_includes,
)
