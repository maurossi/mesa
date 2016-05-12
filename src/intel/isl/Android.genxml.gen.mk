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

# included by isl Android.mk for source generation

ifeq ($(LOCAL_MODULE_CLASS),)
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
endif

intermediates := $(call local-generated-sources-dir)

LOCAL_C_INCLUDES += $(intermediates)

# This is the list of auto-generated files headers
sources := \
	genxml/gen6_pack.h \
	genxml/gen7_pack.h \
	genxml/gen75_pack.h \
	genxml/gen8_pack.h \
	genxml/gen9_pack.h

sources := $(addprefix $(intermediates)/, $(sources))

LOCAL_GENERATED_SOURCES += $(sources)

define header-gen
	@mkdir -p $(dir $@)
	@echo "Gen Header: $(PRIVATE_MODULE) <= $(notdir $(@))"
	$(hide) $(PRIVATE_SCRIPT) $(PRIVATE_XML) > $@
endef

$(intermediates)/genxml/gen6_pack.h : PRIVATE_SCRIPT := python3 $(LOCAL_PATH)/../genxml/gen_pack_header.py
$(intermediates)/genxml/gen6_pack.h : PRIVATE_XML := $(LOCAL_PATH)/../genxml/gen6.xml
$(intermediates)/genxml/gen6_pack.h : $(LOCAL_PATH)/../genxml/gen6.xml $(LOCAL_PATH)/../genxml/gen_pack_header.py
	$(call header-gen)

$(intermediates)/genxml/gen7_pack.h : PRIVATE_SCRIPT := python3 $(LOCAL_PATH)/../genxml/gen_pack_header.py
$(intermediates)/genxml/gen7_pack.h : PRIVATE_XML := $(LOCAL_PATH)/../genxml/gen7.xml
$(intermediates)/genxml/gen7_pack.h : $(LOCAL_PATH)/../genxml/gen7.xml $(LOCAL_PATH)/../genxml/gen_pack_header.py
	$(call header-gen)

$(intermediates)/genxml/gen75_pack.h : PRIVATE_SCRIPT := python3 $(LOCAL_PATH)/../genxml/gen_pack_header.py
$(intermediates)/genxml/gen75_pack.h : PRIVATE_XML := $(LOCAL_PATH)/../genxml/gen75.xml
$(intermediates)/genxml/gen75_pack.h : $(LOCAL_PATH)/../genxml/gen75.xml $(LOCAL_PATH)/../genxml/gen_pack_header.py
	$(call header-gen)

$(intermediates)/genxml/gen8_pack.h : PRIVATE_SCRIPT := python3 $(LOCAL_PATH)/../genxml/gen_pack_header.py
$(intermediates)/genxml/gen8_pack.h : PRIVATE_XML := $(LOCAL_PATH)/../genxml/gen8.xml
$(intermediates)/genxml/gen8_pack.h : $(LOCAL_PATH)/../genxml/gen8.xml $(LOCAL_PATH)/../genxml/gen_pack_header.py
	$(call header-gen)

$(intermediates)/genxml/gen9_pack.h : PRIVATE_SCRIPT := python3 $(LOCAL_PATH)/../genxml/gen_pack_header.py
$(intermediates)/genxml/gen9_pack.h : PRIVATE_XML := $(LOCAL_PATH)/../genxml/gen9.xml
$(intermediates)/genxml/gen9_pack.h : $(LOCAL_PATH)/../genxml/gen9.xml $(LOCAL_PATH)/../genxml/gen_pack_header.py
	$(call header-gen)
