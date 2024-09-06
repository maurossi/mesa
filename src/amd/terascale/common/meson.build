# Copyright © 2024 Vitaliy Triang3l Kuzmin

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

amd_terascale_registers_files = files(
  '../../../gallium/drivers/r600/eg_sq.h',
  '../../../gallium/drivers/r600/evergreend.h',
  '../../../gallium/drivers/r600/r600_opcodes.h',
  '../../../gallium/drivers/r600/r600_sq.h',
  '../../../gallium/drivers/r600/r600d.h',
  '../../../gallium/drivers/r600/r600d_common.h',
  '../../../gallium/drivers/r600/r700_sq.h',
)

idep_amd_terascale_registers = declare_dependency(
  sources : [amd_terascale_registers_files],
)

amd_terascale_formats_c = custom_target(
  'terascale_formats.c',
  input : ['terascale_formats.py', '../../../util/format/u_format.yaml'],
  output : 'terascale_formats.c',
  command : [prog_python, '@INPUT@'],
  depend_files : files('../../../util/format/u_format_parse.py'),
  capture : true,
)

amd_terascale_common_files = files(
  '../../../gallium/drivers/r600/eg_asm.c',
  '../../../gallium/drivers/r600/r600_asm.c',
  '../../../gallium/drivers/r600/r600_asm.h',
  '../../../gallium/drivers/r600/r600_formats.h',
  '../../../gallium/drivers/r600/r600_isa.c',
  '../../../gallium/drivers/r600/r600_isa.h',
  '../../../gallium/drivers/r600/r600_shader_common.h',
  '../../../gallium/drivers/r600/r700_asm.c',
  'terascale_format.c',
  'terascale_format.h',
  'terascale_wddm.h',
)

amd_terascale_common_includes = [
  inc_amd_common,
  inc_util,
]

libamd_terascale_common = static_library(
  'amd_terascale_common',
  [amd_terascale_common_files, amd_terascale_formats_c],
  include_directories : amd_terascale_common_includes,
  dependencies : [
    idep_amd_terascale_registers,
    idep_mesautil,
  ],
  gnu_symbol_visibility : 'hidden',
)

idep_amd_terascale_common = declare_dependency(
  link_with : libamd_terascale_common,
  include_directories : amd_terascale_common_includes,
)
