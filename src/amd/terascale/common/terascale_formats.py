COPYRIGHT = '''\
/*
 * Copyright (c) 2024 Vitaliy Triang3l Kuzmin
 *
 * Based in part on the address library for AMD drivers (AddrLib) which is:
 * Copyright (c) 2007-2024 Advanced Micro Devices, Inc. All Rights Reserved.
 * https://github.com/GPUOpen-Drivers/pal/tree/dc99f22e2999cbefb5d46bec9a8beb9a9b6fa5e8/src/core/imported/addrlib
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
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
'''

import os
import sys

sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '../../../util/format'))
import u_format_parse


def get_first_non_void_channel_index(channels, among_channel_indices):
    for channel_index in among_channel_indices:
        if channels[channel_index].type != u_format_parse.VOID:
            return channel_index
    return None


SWIZZLE_MAP = {
    u_format_parse.SWIZZLE_X: 'X',
    u_format_parse.SWIZZLE_Y: 'Y',
    u_format_parse.SWIZZLE_Z: 'Z',
    u_format_parse.SWIZZLE_W: 'W',
    u_format_parse.SWIZZLE_0: '0',
    u_format_parse.SWIZZLE_1: '1',
}


class FormatInfo:
    def __init__(self):
        # Initialize to zero hardware values, and all swizzle components to missing.
        # The format index must always be set, however.
        self.format = None
        self.number_type = 'UNORM'
        # For floating-point formats, `signed` should be set to True only if it makes a difference
        # (such as for BC6).
        # For non-existent and void channels, `signed` must be set to that of the first non-void
        # channel.
        self.signed = [False, False, False, False]
        self.swizzle = [
            u_format_parse.SWIZZLE_0,
            u_format_parse.SWIZZLE_0,
            u_format_parse.SWIZZLE_0,
            u_format_parse.SWIZZLE_1,
        ]
        self.cb_color_swap = 'STD'
        self.supports_sq_vertex_fetch = False
        self.supports_sq_texture_fetch = False
        self.supports_cb_color = False

    def set_all_signed(self, signed):
        for channel_index in range(4):
            self.signed[channel_index] = signed


class NumberTypeInfo:
    def __init__(self, number_type, opposite_signedness_number_type=None, signed=False):
        self.number_type = number_type
        self.signed = signed
        self.opposite_signedness_number_type = opposite_signedness_number_type

    def is_equal_ignoring_sign(self, other_number_type):
        if other_number_type is None:
            return False
        return (other_number_type == self.number_type or
                other_number_type == self.opposite_signedness_number_type)


NUMBER_TYPE_INFOS = {
    'UNORM': NumberTypeInfo('UNORM', 'SNORM'),
    'SNORM': NumberTypeInfo('SNORM', 'UNORM', True),
    'USCALED': NumberTypeInfo('USCALED', 'SSCALED'),
    'SSCALED': NumberTypeInfo('SSCALED', 'USCALED', True),
    'UINT': NumberTypeInfo('UINT', 'SINT'),
    'SINT': NumberTypeInfo('SINT', 'UINT', True),
    'SRGB': NumberTypeInfo('SRGB'),
    'FLOAT': NumberTypeInfo('FLOAT'),
}


# Never returns SRGB.
def get_number_type(channel, void_number_type=None):
    if channel.type == u_format_parse.VOID:
        return void_number_type
    if channel.type == u_format_parse.FLOAT:
        return 'FLOAT'
    signed = channel.type == u_format_parse.SIGNED
    if not signed and channel.type != u_format_parse.UNSIGNED:
        return None
    if channel.pure:
        number_type_without_signedness = 'INT'
    elif channel.norm:
        number_type_without_signedness = 'NORM'
    else:
        number_type_without_signedness = 'SCALED'
    return ('S' if signed else 'U') + number_type_without_signedness


# According to CB_COLOR#_INFO FORMAT documentation.
# For number types that are implied by the data format, while the hardware ignores the number type
# field, this lists number types which may end up being used by the channels implicitly.
# If the data format is not in this dictionary, it's not supported for CB_COLOR usage on R8xx at
# all.
CB_COLOR_R8XX_SUPPORTED_NUMBER_TYPES = {
    '8': set(['UNORM', 'SNORM', 'UINT', 'SINT']),
    '16': set(['UNORM', 'SNORM', 'UINT', 'SINT', 'FLOAT']),
    '16_FLOAT': set(['FLOAT']),
    '8_8': set(['UNORM', 'SNORM', 'UINT', 'SINT']),
    '5_6_5': set(['UNORM', 'SNORM']),
    '1_5_5_5': set(['UNORM', 'SNORM']),
    '4_4_4_4': set(['UNORM', 'SNORM']),
    '5_5_5_1': set(['UNORM', 'SNORM']),
    '32': set(['UINT', 'SINT', 'FLOAT']),
    '32_FLOAT': set(['FLOAT']),
    '16_16': set(['UNORM', 'SNORM', 'UINT', 'SINT', 'FLOAT']),
    '16_16_FLOAT': set(['FLOAT']),
    '8_24': set(['UNORM', 'UINT']),
    '24_8': set(['UNORM', 'UINT']),
    '10_11_11': set(['FLOAT']),
    '10_11_11_FLOAT': set(['FLOAT']),
    '2_10_10_10': set(['UNORM', 'SNORM', 'UINT', 'SINT']),
    '8_8_8_8': set(['UNORM', 'SNORM', 'UINT', 'SINT', 'SRGB']),
    '10_10_10_2': set(['UNORM', 'SNORM', 'UINT', 'SINT']),
    'X24_8_32_FLOAT': set(['UINT', 'FLOAT']),
    '32_32': set(['UINT', 'SINT', 'FLOAT']),
    '32_32_FLOAT': set(['FLOAT']),
    '16_16_16_16': set(['UNORM', 'SNORM', 'UINT', 'SINT', 'FLOAT']),
    '16_16_16_16_FLOAT': set(['FLOAT']),
    '32_32_32_32': set(['UINT', 'SINT', 'FLOAT']),
    '32_32_32_32_FLOAT': set(['FLOAT']),
}


CB_COLOR_SWAP_EXPORT_COMPONENTS_FOR_NR_CHANNELS = {
    1: {
        'STD': [0],
        'ALT': [1],
        'STD_REV': [2],
        'ALT_REV': [3],
    },
    2: {
        'STD': [0, 1],
        'ALT': [0, 3],
        'STD_REV': [1, 0],
        'ALT_REV': [3, 0],
    },
    3: {
        'STD': [0, 1, 2],
        'ALT': [0, 1, 3],
        'STD_REV': [2, 1, 0],
        'ALT_REV': [3, 1, 0],
    },
    4: {
        'STD': [0, 1, 2, 3],
        'ALT': [2, 1, 0, 3],
        'STD_REV': [3, 2, 1, 0],
        'ALT_REV': [3, 0, 1, 2],
    },
}


# cb_color_swap_export_components are:
# [0-based data channel index: 0-based color export component index]
# length being the number of data channels.
def is_swizzle_supported_by_cb_color(format_swizzle, cb_color_swap_export_components):
    last_channel_swizzle = u_format_parse.SWIZZLE_X + (len(cb_color_swap_export_components) - 1)
    for component_swizzle_index, component_swizzle in enumerate(format_swizzle):
        if not (component_swizzle >= u_format_parse.SWIZZLE_X and
                component_swizzle <= last_channel_swizzle):
            # This component of the color export is unused in the format, don't care which channel
            # it ends up in, if at all. The client API implementation, if the client API has formats
            # with void channels, is expected to, if the unused bits must be unmodified, exclude the
            # export component from the write mask, and if it's alpha, replace the destination alpha
            # blend factor with 1 if such formats exist at all in the client API.
            continue
        # Export component is actually used - check if it goes to the correct data channel.
        if (component_swizzle_index !=
            cb_color_swap_export_components[component_swizzle - u_format_parse.SWIZZLE_X]):
            return False
    return True


def get_format_info(pipe_format, is_r8xx):
    if pipe_format.name == 'PIPE_FORMAT_NONE':
        return None

    info = FormatInfo()

    # Subsampled formats.
    if pipe_format.layout == 'subsampled':
        # BG_RG bytes: Y0, Z, Y1, X.
        # GB_GR bytes: Z, Y0, X, Y1.
        info.swizzle[0] = u_format_parse.SWIZZLE_X
        info.swizzle[1] = u_format_parse.SWIZZLE_Y
        info.swizzle[2] = u_format_parse.SWIZZLE_Z
        info.supports_sq_texture_fetch = True
        if (pipe_format.name == 'PIPE_FORMAT_R8G8_B8G8_UNORM' or
            pipe_format.name == 'PIPE_FORMAT_VYUY'):
            info.format = 'GB_GR'
            info.swizzle[0] = u_format_parse.SWIZZLE_Z
            info.swizzle[2] = u_format_parse.SWIZZLE_X
            info.cb_color_swap = 'STD_REV'
        elif (pipe_format.name == 'PIPE_FORMAT_G8R8_G8B8_UNORM' or
              pipe_format.name == 'PIPE_FORMAT_YVYU'):
            info.format = 'BG_RG'
            info.swizzle[0] = u_format_parse.SWIZZLE_Z
            info.swizzle[2] = u_format_parse.SWIZZLE_X
            info.cb_color_swap = 'STD_REV'
        elif pipe_format.name == 'PIPE_FORMAT_G8R8_B8R8_UNORM':
            info.format = 'GB_GR'
            info.swizzle[0] = u_format_parse.SWIZZLE_Y
            info.swizzle[1] = u_format_parse.SWIZZLE_Z
            info.swizzle[2] = u_format_parse.SWIZZLE_X
        elif pipe_format.name == 'PIPE_FORMAT_R8G8_R8B8_UNORM':
            info.format = 'BG_RG'
            info.swizzle[0] = u_format_parse.SWIZZLE_Y
            info.swizzle[1] = u_format_parse.SWIZZLE_Z
            info.swizzle[2] = u_format_parse.SWIZZLE_X
        elif pipe_format.name == 'PIPE_FORMAT_B8R8_G8R8_UNORM':
            info.format = 'GB_GR'
            info.swizzle[0] = u_format_parse.SWIZZLE_Y
            info.swizzle[1] = u_format_parse.SWIZZLE_X
        elif pipe_format.name == 'PIPE_FORMAT_R8B8_R8G8_UNORM':
            info.format = 'BG_RG'
            info.swizzle[0] = u_format_parse.SWIZZLE_Y
            info.swizzle[1] = u_format_parse.SWIZZLE_X
        elif (pipe_format.name == 'PIPE_FORMAT_G8B8_G8R8_UNORM' or
              pipe_format.name == 'PIPE_FORMAT_YUYV'):
            info.format = 'BG_RG'
        elif (pipe_format.name == 'PIPE_FORMAT_B8G8_R8G8_UNORM' or
              pipe_format.name == 'PIPE_FORMAT_UYVY'):
            info.format = 'GB_GR'
        return info if info.format is not None else None

    for swizzle_index, swizzle in enumerate(pipe_format.le_swizzles):
        if swizzle != u_format_parse.SWIZZLE_NONE:
            info.swizzle[swizzle_index] = swizzle

    # Non-plain formats.

    if pipe_format.layout != u_format_parse.PLAIN:
        info.number_type = 'SRGB' if pipe_format.colorspace == u_format_parse.SRGB else 'UNORM'

        info.supports_sq_texture_fetch = True

        if pipe_format.name == 'PIPE_FORMAT_R11G11B10_FLOAT':
            info.format = '10_11_11_FLOAT'
            info.number_type = 'FLOAT'
            info.supports_sq_vertex_fetch = True
            info.supports_cb_color = True
            return info

        if pipe_format.name == 'PIPE_FORMAT_R9G9B9E5_FLOAT':
            info.format = '5_9_9_9_SHAREDEXP'
            info.number_type = 'FLOAT'
            info.supports_sq_vertex_fetch = True
            return info

        # TODO(Triang3l): Handle PIPE_FORMAT_R1_UNORM, see if it's 1 or 1_REVERSED, and what the
        # usage requirements are.

        if (pipe_format.name == 'PIPE_FORMAT_DXT1_RGB' or
            pipe_format.name == 'PIPE_FORMAT_DXT1_RGBA' or
            pipe_format.name == 'PIPE_FORMAT_DXT1_SRGB' or
            pipe_format.name == 'PIPE_FORMAT_DXT1_SRGBA'):
            info.format = 'BC1'
            return info

        if (pipe_format.name == 'PIPE_FORMAT_DXT3_RGBA' or
            pipe_format.name == 'PIPE_FORMAT_DXT3_SRGBA'):
            info.format = 'BC2'
            return info

        if (pipe_format.name == 'PIPE_FORMAT_DXT5_RGBA' or
            pipe_format.name == 'PIPE_FORMAT_DXT5_SRGBA'):
            info.format = 'BC3'
            return info

        if pipe_format.name == 'PIPE_FORMAT_RGTC1_UNORM':
            info.format = 'BC4'
            return info

        if pipe_format.name == 'PIPE_FORMAT_RGTC1_SNORM':
            info.format = 'BC4'
            info.number_type = 'SNORM'
            info.signed[0] = True
            return info

        if pipe_format.name == 'PIPE_FORMAT_RGTC2_UNORM':
            info.format = 'BC5'
            return info

        if pipe_format.name == 'PIPE_FORMAT_RGTC2_SNORM':
            info.format = 'BC5'
            info.number_type = 'SNORM'
            info.set_all_signed(True)
            return info

        if (pipe_format.name == 'PIPE_FORMAT_BPTC_RGBA_UNORM' or
            pipe_format.name == 'PIPE_FORMAT_BPTC_SRGBA'):
            if not is_r8xx:
                return None
            info.format = 'BC7'
            return info

        if pipe_format.name == 'PIPE_FORMAT_BPTC_RGB_FLOAT':
            if not is_r8xx:
                return None
            info.format = 'BC6'
            info.number_type = 'FLOAT'
            info.set_all_signed(True)
            return info

        if pipe_format.name == 'PIPE_FORMAT_BPTC_RGB_UFLOAT':
            if not is_r8xx:
                return None
            info.format = 'BC6'
            info.number_type = 'FLOAT'
            return info

        return None

    # Plain formats.

    # All non-block formats support vertex fetch.
    info.supports_sq_vertex_fetch = True
    # Some formats don't support texture fetch, see the VTX_WORD1 documentation in the R700 ISA
    # reference.
    info.supports_sq_texture_fetch = True
    # Some formats don't support CB_COLOR usage, and some don't for certain number types, see the
    # CB_COLOR register reference.
    info.supports_cb_color = True

    nr_channels = pipe_format.nr_channels()

    if (pipe_format.colorspace == u_format_parse.RGB or
        pipe_format.colorspace == u_format_parse.SRGB or
        pipe_format.colorspace == u_format_parse.YUV):

        # Handle 1_5_5_5 and 5_5_5_1 separately because the single-bit is always UNORM (or USCALED,
        # which is the same for 1 bit) or UINT.
        if nr_channels == 4:
            if (pipe_format.le_channels[0].size == 5 and
                pipe_format.le_channels[1].size == 5 and
                pipe_format.le_channels[2].size == 5 and
                pipe_format.le_channels[3].size == 1):
                info.format = '1_5_5_5'
                packed_5_bit_channel_indices = range(3)
                packed_1_bit_channel_index = 3
            elif (pipe_format.le_channels[0].size == 1 and
                  pipe_format.le_channels[1].size == 5 and
                  pipe_format.le_channels[2].size == 5 and
                  pipe_format.le_channels[3].size == 5):
                info.format = '5_5_5_1'
                packed_5_bit_channel_indices = range(1, 4)
                packed_1_bit_channel_index = 0

            if info.format is not None:
                packed_1_bit_channel = pipe_format.le_channels[packed_1_bit_channel_index]
                packed_1_bit_channel_number_type = get_number_type(packed_1_bit_channel)
                packed_5_bit_first_non_void_channel_index = get_first_non_void_channel_index(
                    pipe_format.le_channels, packed_5_bit_channel_indices)

                if packed_5_bit_first_non_void_channel_index is not None:
                    # There is a 5-bit channel.

                    info.number_type = get_number_type(
                        pipe_format.le_channels[packed_5_bit_first_non_void_channel_index])
                    if info.number_type is None or info.number_type == 'FLOAT':
                        return None
                    number_type_info = NUMBER_TYPE_INFOS[info.number_type]

                    # All 5-bit channels must have the same number type aside from the signedness.
                    for channel_index in packed_5_bit_channel_indices:
                        channel_number_type = get_number_type(
                            pipe_format.le_channels[channel_index], info.number_type)
                        if not number_type_info.is_equal_ignoring_sign(channel_number_type):
                            return None
                        channel_signed = NUMBER_TYPE_INFOS[channel_number_type].signed
                        info.signed[channel_index] = channel_signed
                        if channel_signed != number_type_info.signed:
                            # All 5-bit channels must have the same signedness for it to be
                            # expressible in SQ vertex fetch constants and CB_COLOR.
                            info.supports_sq_vertex_fetch = False
                            info.supports_cb_color = False

                    # The 1-bit channel, if not void, must be unsigned and must have a number type
                    # compatible with the 5-bit channels.
                    # UNORM and USCALED are the same for 1 bit.
                    info.signed[packed_1_bit_channel_index] = False
                    if packed_1_bit_channel_number_type is not None:
                        if (info.number_type == 'UNORM' or info.number_type == 'SNORM' or
                            info.number_type == 'USCALED' or info.number_type == 'SSCALED'):
                            if (packed_1_bit_channel_number_type != 'UNORM' and
                                packed_1_bit_channel_number_type != 'USCALED'):
                                return None
                        elif info.number_type == 'UINT' or info.number_type == 'SINT':
                            if packed_1_bit_channel_number_type != 'UINT':
                                return None

                else:
                    # X5X5X5U1 (for 1_5_5_5) or U1X5X5X5 (for 5_5_5_1).
                    info.number_type = packed_1_bit_channel_number_type
                    info.set_all_signed(False)
                    if info.number_type == 'UNORM' or info.number_type == 'USCALED':
                        # UNORM and USCALED are the same for 1 bit, but UNORM is supported by more
                        # GPU subsystems.
                        info.number_type = 'UNORM'
                    elif info.number_type != 'UINT':
                        return None

        # General plain format case.

        if info.format is None:
            first_non_void_channel_index = get_first_non_void_channel_index(
                pipe_format.le_channels, range(nr_channels))
            if first_non_void_channel_index is None:
                return None

            info.number_type = get_number_type(pipe_format.le_channels[first_non_void_channel_index])
            if info.number_type is None:
                return None
            number_type_info = NUMBER_TYPE_INFOS[info.number_type]

            # Set the signedness of nonexistent channels to that of the first non-void channel for
            # consistency.
            info.set_all_signed(number_type_info.signed)
            for channel_index in range(nr_channels):
                channel_number_type = get_number_type(
                    pipe_format.le_channels[channel_index], info.number_type)
                # All channels must have compatible number types.
                if not number_type_info.is_equal_ignoring_sign(channel_number_type):
                    return None
                channel_signed = NUMBER_TYPE_INFOS[channel_number_type].signed
                info.signed[channel_index] = channel_signed
                if channel_signed != number_type_info.signed:
                    # All channels must have the same signedness for it to be expressible in SQ
                    # vertex fetch constants and CB_COLOR.
                    info.supports_sq_vertex_fetch = False
                    info.supports_cb_color = False

            is_integer = info.number_type == 'UINT' or info.number_type == 'SINT'
            is_float = info.number_type == 'FLOAT'

            if nr_channels == 1:
                if pipe_format.le_channels[0].size == 8:
                    if is_float:
                        return None
                    info.format = '8'
                elif pipe_format.le_channels[0].size == 16:
                    info.format = '16_FLOAT' if is_float else '16'
                elif pipe_format.le_channels[0].size == 32:
                    if is_float:
                        info.format = '32_FLOAT'
                    else:
                        info.format = '32'
                        if not is_integer:
                            info.supports_cb_color = False

            elif nr_channels == 2:
                if (pipe_format.le_channels[0].size == 4 and
                    pipe_format.le_channels[1].size == 4):
                    if is_float:
                        return None
                    info.format = '4_4'
                elif (pipe_format.le_channels[0].size == 8 and
                      pipe_format.le_channels[1].size == 8):
                    if is_float:
                        return None
                    info.format = '8_8'
                elif (pipe_format.le_channels[0].size == 16 and
                      pipe_format.le_channels[1].size == 16):
                    info.format = '16_16_FLOAT' if is_float else '16_16'
                elif (pipe_format.le_channels[0].size == 32 and
                      pipe_format.le_channels[1].size == 32):
                    if is_float:
                        info.format = '32_32_FLOAT'
                    else:
                        info.format = '32_32'
                        if not is_integer:
                            info.supports_cb_color = False

            elif nr_channels == 3:
                if (pipe_format.le_channels[0].size == 2 and
                    pipe_format.le_channels[1].size == 3 and
                    pipe_format.le_channels[2].size == 3):
                    if is_float:
                        return None
                    info.format = '3_3_2'
                    info.supports_sq_texture_fetch = False
                elif (pipe_format.le_channels[0].size == 5 and
                      pipe_format.le_channels[1].size == 6 and
                      pipe_format.le_channels[2].size == 5):
                    if is_float:
                        return None
                    info.format = '5_6_5'
                elif (pipe_format.le_channels[0].size == 5 and
                      pipe_format.le_channels[1].size == 5 and
                      pipe_format.le_channels[2].size == 6):
                    if is_float:
                        return None
                    info.format = '6_5_5'
                elif (pipe_format.le_channels[0].size == 11 and
                      pipe_format.le_channels[1].size == 11 and
                      pipe_format.le_channels[2].size == 10):
                    if is_float:
                        info.format = '10_11_11_FLOAT'
                    else:
                        info.format = '10_11_11'
                        info.supports_sq_texture_fetch = False
                elif (pipe_format.le_channels[0].size == 10 and
                      pipe_format.le_channels[1].size == 11 and
                      pipe_format.le_channels[2].size == 11):
                    info.format = '11_11_10_FLOAT' if is_float else '11_11_10'
                    info.supports_sq_texture_fetch = False
                elif (pipe_format.le_channels[0].size == 8 and
                      pipe_format.le_channels[1].size == 8 and
                      pipe_format.le_channels[2].size == 8):
                    if is_float:
                        return None
                    info.format = '8_8_8'
                    info.supports_sq_texture_fetch = False
                    info.supports_cb_color = False
                elif (pipe_format.le_channels[0].size == 16 and
                      pipe_format.le_channels[1].size == 16 and
                      pipe_format.le_channels[2].size == 16):
                    info.format = '16_16_16_FLOAT' if is_float else '16_16_16'
                    info.supports_sq_texture_fetch = False
                    info.supports_cb_color = False
                elif (pipe_format.le_channels[0].size == 32 and
                      pipe_format.le_channels[1].size == 32 and
                      pipe_format.le_channels[2].size == 32):
                    info.format = '32_32_32_FLOAT' if is_float else '32_32_32'
                    info.supports_cb_color = False

            elif nr_channels == 4:
                if (pipe_format.le_channels[0].size == 4 and
                    pipe_format.le_channels[1].size == 4 and
                    pipe_format.le_channels[2].size == 4 and
                    pipe_format.le_channels[3].size == 4):
                    if is_float:
                        return None
                    info.format = '4_4_4_4'
                elif (pipe_format.le_channels[0].size == 10 and
                      pipe_format.le_channels[1].size == 10 and
                      pipe_format.le_channels[2].size == 10 and
                      pipe_format.le_channels[3].size == 2):
                    if is_float:
                        return None
                    info.format = '2_10_10_10'
                elif (pipe_format.le_channels[0].size == 8 and
                      pipe_format.le_channels[1].size == 8 and
                      pipe_format.le_channels[2].size == 8 and
                      pipe_format.le_channels[3].size == 8):
                    if is_float:
                        return None
                    info.format = '8_8_8_8'
                elif (pipe_format.le_channels[0].size == 2 and
                      pipe_format.le_channels[1].size == 10 and
                      pipe_format.le_channels[2].size == 10 and
                      pipe_format.le_channels[3].size == 10):
                    if is_float:
                        return None
                    info.format = '10_10_10_2'
                elif (pipe_format.le_channels[0].size == 16 and
                      pipe_format.le_channels[1].size == 16 and
                      pipe_format.le_channels[2].size == 16 and
                      pipe_format.le_channels[3].size == 16):
                    info.format = '16_16_16_16_FLOAT' if is_float else '16_16_16_16'
                elif (pipe_format.le_channels[0].size == 32 and
                      pipe_format.le_channels[1].size == 32 and
                      pipe_format.le_channels[2].size == 32 and
                      pipe_format.le_channels[3].size == 32):
                    if is_float:
                        info.format = '32_32_32_32_FLOAT'
                    else:
                        info.format = '32_32_32_32'
                        if not is_integer:
                            info.supports_cb_color = False

        if pipe_format.colorspace == u_format_parse.SRGB:
            if info.format != '8_8_8_8' or info.number_type != 'UNORM':
                return None
            info.number_type = 'SRGB'

    elif pipe_format.colorspace == u_format_parse.ZS:
        # DB always uses little endian, endian swapping may only be needed for cases where the
        # depth / stencil formats are used merely to store color data without DB access (primarily
        # for linear surfaces, which are inherently not compatible with DB).
        # Not allowing number types other than those expected by DB for the given hardware data
        # formats even for non-DB usage for simplicity (as of this writing, there are no
        # pipe_formats demanding that).
        # For R8xx, where depth and stencil are separate, still returning the combined format for
        # implementation simplicity (even though DB works with separate 32_FLOAT and 8 surfaces,
        # X24_8_32_FLOAT is still supported as a color format). The client API implementation
        # working with these format infos should use separate infos for the depth aspect and the
        # S8_UINT stencil aspect instead of the one for the combined format.
        stencil_channel = None
        if nr_channels == 1:
            if pipe_format.le_channels[0].size == 8:
                info.format = '8'
                stencil_channel = pipe_format.le_channels[0]
                info.number_type = get_number_type(stencil_channel, 'UINT')
                info.cb_color_swap = 'ALT'
            # Formats with 24-bit depth are always 2-component.
            elif pipe_format.le_channels[0].size == 16:
                info.format = '16'
                info.number_type = get_number_type(pipe_format.le_channels[0], 'UNORM')
                if info.number_type != 'UNORM':
                    return None
            elif pipe_format.le_channels[0].size == 32:
                info.format = '32_FLOAT'
                info.number_type = get_number_type(pipe_format.le_channels[0], 'FLOAT')
                if info.number_type != 'FLOAT':
                    return None
        elif nr_channels == 2:
            # R6xx (with combined depth and stencil aspects) doesn't have a Z16S8 format for either
            # DB or SQ/CB.
            # On R8xx (with separate depth and stencil aspects), client API implementations should
            # explicitly use the info for the Z16 aspect instead of the full Z16S8.
            if (pipe_format.le_channels[0].size == 24 and
                pipe_format.le_channels[1].size == 8):
                info.format = '8_24'
                info.number_type = get_number_type(pipe_format.le_channels[0], 'UNORM')
                if info.number_type == 'FLOAT':
                    info.format = '8_24_FLOAT'
                    info.supports_sq_texture_fetch = False
                elif info.number_type != 'UNORM':
                    return None
                stencil_channel = pipe_format.le_channels[1]
            elif (pipe_format.le_channels[0].size == 8 and
                  pipe_format.le_channels[1].size == 24):
                info.format = '24_8'
                info.number_type = get_number_type(pipe_format.le_channels[1], 'UNORM')
                if info.number_type == 'FLOAT':
                    info.format = '24_8_FLOAT'
                    info.supports_sq_texture_fetch = False
                elif info.number_type != 'UNORM':
                    return None
                stencil_channel = pipe_format.le_channels[0]
                info.cb_color_swap = 'STD_REV'
            # Combined Z32S8 is exposed as a Z32S8X24 pipe_format.
        elif nr_channels == 3:
            if (pipe_format.le_channels[0].size == 32 and
                pipe_format.le_channels[1].size == 8 and
                pipe_format.le_channels[2].size == 24):
                if pipe_format.le_channels[2].type != u_format_parse.VOID:
                    return None
                info.format = 'X24_8_32_FLOAT'
                info.number_type = get_number_type(pipe_format.le_channels[0], 'FLOAT')
                if info.number_type != 'FLOAT':
                    return None
                stencil_channel = pipe_format.le_channels[1]
        if stencil_channel is not None and get_number_type(stencil_channel, 'UINT') != 'UINT':
            return None

    if info.format is None:
        return None

    if is_r8xx:
        format_cb_color_number_types = CB_COLOR_R8XX_SUPPORTED_NUMBER_TYPES.get(info.format)
        if format_cb_color_number_types is None or info.number_type not in format_cb_color_number_types:
            info.supports_cb_color = False

    cb_color_swap_nr_channels = nr_channels
    if pipe_format.colorspace == u_format_parse.ZS:
        cb_color_swap_nr_channels = min(2, cb_color_swap_nr_channels)
    cb_color_swap_for_swizzle = None
    for cb_color_swap, cb_color_swap_export_components in \
        CB_COLOR_SWAP_EXPORT_COMPONENTS_FOR_NR_CHANNELS[cb_color_swap_nr_channels].items():
        if is_swizzle_supported_by_cb_color(info.swizzle, cb_color_swap_export_components):
            cb_color_swap_for_swizzle = cb_color_swap
            break
    if cb_color_swap_for_swizzle is not None:
        info.cb_color_swap = cb_color_swap_for_swizzle
    else:
        info.supports_cb_color = False

    return info


def print_format_info(pipe_format, is_r8xx):
    info = get_format_info(pipe_format, is_r8xx)
    if info is None:
        return
    print('   [{0}] = TERASCALE_FORMAT_INFO_INIT({1}),'.format(
        pipe_format.name,
        ', '.join([
            info.format,
            info.number_type,
            '{0}{1}{2}{3}'.format(
                int(info.signed[3]),
                int(info.signed[2]),
                int(info.signed[1]),
                int(info.signed[0])),
            SWIZZLE_MAP[info.swizzle[0]],
            SWIZZLE_MAP[info.swizzle[1]],
            SWIZZLE_MAP[info.swizzle[2]],
            SWIZZLE_MAP[info.swizzle[3]],
            info.cb_color_swap,
            str(int(info.supports_sq_vertex_fetch)),
            str(int(info.supports_sq_texture_fetch)),
            str(int(info.supports_cb_color)),
        ])))


def print_format_infos(pipe_formats, is_r8xx):
    print('const struct terascale_format_info terascale_format_info_r{0}xx[PIPE_FORMAT_COUNT] = {{'.format(
        8 if is_r8xx else 6))
    for pipe_format in pipe_formats:
        print_format_info(pipe_format, is_r8xx)
    print('};')


def main():
    pipe_formats = u_format_parse.parse(sys.argv[1])

    print('/* This file is autogenerated by terascale_formats.py from u_format.csv. Do not edit directly. */')
    print(COPYRIGHT)
    print('''\
#include "terascale_format.h"

#include "util/format/u_format.h"

#define TERASCALE_FORMAT_INFO_INIT( \\
   arg_format, \\
   arg_number_type, \\
   arg_channels_signed, \\
   arg_swizzle_r, arg_swizzle_g, arg_swizzle_b, arg_swizzle_a, \\
   arg_cb_color_swap, \\
   arg_supports_sq_vertex_fetch, arg_supports_sq_texture_fetch, arg_supports_cb_color) \\
   { \\
      .format = TERASCALE_FORMAT_INDEX_##arg_format, \\
      .number_type = TERASCALE_FORMAT_NUMBER_TYPE_##arg_number_type, \\
      .channels_signed = 0b##arg_channels_signed, \\
      .swizzle_r = TERASCALE_SWIZZLE_##arg_swizzle_r, \\
      .swizzle_g = TERASCALE_SWIZZLE_##arg_swizzle_g, \\
      .swizzle_b = TERASCALE_SWIZZLE_##arg_swizzle_b, \\
      .swizzle_a = TERASCALE_SWIZZLE_##arg_swizzle_a, \\
      .cb_color_swap = TERASCALE_FORMAT_CB_COLOR_SWAP_##arg_cb_color_swap, \\
      .supports_sq_vertex_fetch = arg_supports_sq_vertex_fetch, \\
      .supports_sq_texture_fetch = arg_supports_sq_texture_fetch, \\
      .supports_cb_color = arg_supports_cb_color, \\
   }
''')
    print_format_infos(pipe_formats, False)
    print()
    print_format_infos(pipe_formats, True)


if __name__ == '__main__':
    main()
