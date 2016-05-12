#
# Copyright (C) 2016 Android-x86, Mauro Rossi <issor.oruam@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#

ifeq ($(LOCAL_MODULE_CLASS),)
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
endif

isl_format_layout_deps := \
	$(LOCAL_PATH)/isl_format_layout_gen.bash \
	$(LOCAL_PATH)/isl_format_layout.csv

intermediates := $(call local-generated-sources-dir)

define bash-gen
	@mkdir -p $(dir $@)
	@echo "Gen Bash: $(PRIVATE_MODULE) <= $(notdir $(@))"
	$(hide) $(PRIVATE_SCRIPT) < $(PRIVATE_CSV) > $@
endef

$(intermediates)/isl_format_layout.c: PRIVATE_SCRIPT := bash -c $(LOCAL_PATH)/isl_format_layout_gen.bash
$(intermediates)/isl_format_layout.c: PRIVATE_CSV := $(LOCAL_PATH)/isl_format_layout.csv
$(intermediates)/isl_format_layout.c: $(isl_format_layout_deps)
	$(call bash-gen)

LOCAL_SRC_FILES := $(filter-out $(ISL_GENERATED_FILES), $(LOCAL_SRC_FILES))

LOCAL_GENERATED_SOURCES += $(addprefix $(intermediates)/, \
	$(ISL_GENERATED_FILES))
