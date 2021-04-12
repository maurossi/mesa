#!/bin/bash

COMMON_CARGS="
-nostdlibinc
-nostdlib
-fbuiltin
$(abs_include_deps)
$(abs_link_deps)
-fuse-ld=lld
"

COMMON_CXXARGS=$COMMON_CARGS
