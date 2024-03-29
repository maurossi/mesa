# Copyright © 2017 Intel Corporation

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

files_libvirgl = files(
  'virgl_buffer.c',
  'virgl_context.c',
  'virgl_encode.c',
  'virgl_query.c',
  'virgl_resource.c',
  'virgl_screen.c',
  'virgl_staging_mgr.c',
  'virgl_streamout.c',
  'virgl_transfer_queue.c',
  'virgl_texture.c',
  'virgl_tgsi.c',
  'virgl_video.c',
)

libvirgl = static_library(
  'virgl',
  [ files_libvirgl ],
  gnu_symbol_visibility : 'hidden',
  include_directories : [inc_include, inc_src, inc_mapi, inc_mesa, inc_gallium, inc_gallium_aux, inc_virtio],
  dependencies : [dep_libdrm, idep_mesautil, idep_xmlconfig, idep_nir],
)

virgl_deps = [libvirgl]
if not with_platform_windows
  virgl_deps += libvirgldrm
  virgl_deps += libvirglvtest
endif

driver_virgl = declare_dependency(
  compile_args : '-DGALLIUM_VIRGL',
  link_with : virgl_deps,
)

if with_tests
  subdir('tests')
endif
