#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_misc_copy_mock_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_misc_copy_mock_args[] = {
   { 0, 8 },
   { 8, 8 },
   { 16, 4 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g43<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g33<1>UD        g0.1<0,1,0>UD                   { align1 1H };
add(1)          g44<1>UD        g43<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@2 compacted };
mov(8)          g3<1>UW         0x76543210V                     { align1 WE_all 1Q };
shl(16)         g18<1>D         g33<8,8,1>D     0x00000004UD    { align1 1H I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(1)         g1UD            g44UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
add(8)          g3.8<1>UW       g3<1,1,0>UW     0x0008UW        { align1 WE_all 1Q I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         g14<1>D         g3<8,8,1>UW                     { align1 1H };
and(16)         g16<1>UD        g14<1,1,0>UD    0x0000000fUD    { align1 1H I@1 compacted };
add(16)         g4<1>D          g16<1,1,0>D     g18<1,1,0>D     { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g6.1<2>F        g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g8.1<2>F        g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g10.1<2>F       g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g12.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
shl(16)         g20<1>D         g1<0,1,0>D      0x00000004UD    { align1 1H };
mov(8)          g6<2>F          g2<0,1,0>F                      { align1 1Q F@4 compacted };
mov(8)          g8<2>F          g2<0,1,0>F                      { align1 2Q F@4 compacted };
mov(8)          g10<2>F         g2.2<0,1,0>F                    { align1 1Q F@4 compacted };
mov(8)          g12<2>F         g2.2<0,1,0>F                    { align1 2Q F@4 compacted };

LABEL1:
cmp.ge.f0.0(16) null<1>UD       g4<8,8,1>UD     g2.4<0,1,0>UD   { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL0        UIP:  LABEL0              { align1 1H };
add(8)          g22<1>D         g10<8,4,2>D     g4<1,1,0>D      { align1 1Q F@2 compacted };
add(8)          g23<1>D         g12<8,4,2>D     g5<1,1,0>D      { align1 2Q F@1 compacted };
add(8)          g28<1>D         g6<8,4,2>D      g4<1,1,0>D      { align1 1Q F@4 compacted };
add(8)          g29<1>D         g8<8,4,2>D      g5<1,1,0>D      { align1 2Q F@3 compacted };
cmp.l.f0.0(8)   g24<1>UD        g22<8,8,1>UD    g10<8,4,2>UD    { align1 1Q I@4 };
cmp.l.f0.0(8)   g25<1>UD        g23<8,8,1>UD    g12<8,4,2>UD    { align1 2Q I@4 };
mov(8)          g34<2>UD        g22<4,4,1>UD                    { align1 1Q $1.src };
mov(8)          g36<2>UD        g23<4,4,1>UD                    { align1 2Q $1.src };
cmp.l.f0.0(8)   g30<1>UD        g28<8,8,1>UD    g6<8,4,2>UD     { align1 1Q I@6 };
cmp.l.f0.0(8)   g31<1>UD        g29<8,8,1>UD    g8<8,4,2>UD     { align1 2Q I@6 };
mov(8)          g38<2>UD        g28<4,4,1>UD                    { align1 1Q $2.src };
mov(8)          g40<2>UD        g29<4,4,1>UD                    { align1 2Q $2.src };
add(8)          g26<1>D         -g24<8,8,1>D    g10.1<8,4,2>D   { align1 1Q I@7 };
add(8)          g27<1>D         -g25<8,8,1>D    g12.1<8,4,2>D   { align1 2Q I@7 };
add(8)          g32<1>D         -g30<8,8,1>D    g6.1<8,4,2>D    { align1 1Q I@6 };
add(8)          g33<1>D         -g31<8,8,1>D    g8.1<8,4,2>D    { align1 2Q I@6 };
mov(8)          g34.1<2>UD      g26<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g36.1<2>UD      g27<4,4,1>UD                    { align1 2Q I@4 };
mov(8)          g38.1<2>UD      g32<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g40.1<2>UD      g33<4,4,1>UD                    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g27UD           g34UD           nullUD          0x08200980                0x00000000
                            ugm MsgDesc: ( load, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(16)         g42<1>UD        g27<32,8,4>UB                   { align1 1H $1.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g38UD           g42UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $2 };
add(16)         g4<1>D          g4<1,1,0>D      g20<1,1,0>D     { align1 1H compacted };

LABEL0:
while(16)       JIP:  LABEL1                                    { align1 1H };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_misc_copy_mock_code[] = {
    0x80000065, 0x2b058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x21050220, 0x00000024, 0x00000000,
    0xe22c1a40, 0x00012b03, 0x80030061, 0x03054410,
    0x00000000, 0x76543210, 0x00041b69, 0x12058660,
    0x02462105, 0x00000004, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa002c0c, 0x00340000, 0x64031a40, 0x00800395,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00040061, 0x0e050160, 0x00460305, 0x00000000,
    0xe0101965, 0x00f00e03, 0xa0041940, 0x12001002,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x21060061, 0x001102cc, 0x2a080061, 0x001102cc,
    0x00030061, 0x0a260aa0, 0x00000264, 0x00000000,
    0x00130061, 0x0c260aa0, 0x00000264, 0x00000000,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x00040069, 0x14058660, 0x02000104, 0x00000004,
    0x21061461, 0x00110204, 0x2a081461, 0x00110204,
    0x210a1461, 0x00110244, 0x2a0c1461, 0x00110244,
    0x00041a70, 0x00010220, 0x42460405, 0x00000284,
    0x01040028, 0x0001c660, 0x00000198, 0x00000198,
    0xa1161240, 0x040e0a02, 0xaa171140, 0x050e0c02,
    0xa11c1440, 0x040e0602, 0xaa1d1340, 0x050e0802,
    0x00031c70, 0x18050220, 0x52461605, 0x00440a06,
    0x00131c70, 0x19050220, 0x52461705, 0x00440c06,
    0x00033161, 0x22060220, 0x00341605, 0x00000000,
    0x00133161, 0x24060220, 0x00341705, 0x00000000,
    0x00031e70, 0x1e050220, 0x52461c05, 0x00440606,
    0x00131e70, 0x1f050220, 0x52461d05, 0x00440806,
    0x00033261, 0x26060220, 0x00341c05, 0x00000000,
    0x00133261, 0x28060220, 0x00341d05, 0x00000000,
    0x00031f40, 0x1a052660, 0x06461805, 0x00440a26,
    0x00131f40, 0x1b052660, 0x06461905, 0x00440c26,
    0x00031e40, 0x20052660, 0x06461e05, 0x00440626,
    0x00131e40, 0x21052660, 0x06461f05, 0x00440826,
    0x00031c61, 0x22260220, 0x00341a05, 0x00000000,
    0x00131c61, 0x24260220, 0x00341b05, 0x00000000,
    0x00031c61, 0x26260220, 0x00342005, 0x00000000,
    0x00131c61, 0x28260220, 0x00342105, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x1b140000, 0xf3002224, 0x00020000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x00042161, 0x2a050020, 0x00661b07, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x00000000, 0xf3082624, 0x00022a14,
    0xa0040040, 0x14000402, 0x00040027, 0x00014060,
    0x00000000, 0xfffffe58, 0x80030061, 0x7e050220,
    0x00460005, 0x00000000, 0x80040931, 0x00000004,
    0x30007e0c, 0x00000000, 0x20000060, 0x00000000,
};
static const struct brw_kernel gfx125_misc_copy_mock = {
   .prog_data = {
      .base.nr_params = 13,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 768,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_misc_copy_mock_relocs,
      .base.uses_atomic_load_store = false,
      .local_size = { 16, 1, 1 },
      .prog_offset = { 0, 0, 0 },
      .prog_mask = 2,
      .prog_spilled = 0,
      .uses_barrier = false,
      .uses_num_work_groups = false,
      .push.per_thread.dwords = 0,
      .push.per_thread.regs = 0,
      .push.per_thread.size = 0,
      .push.cross_thread.dwords = 13,
      .push.cross_thread.regs = 2,
      .push.cross_thread.size = 64,
   },
   .args_size = 20,
   .arg_count = 3,
   .args = gfx125_misc_copy_mock_args,
   .code = gfx125_misc_copy_mock_code,
};
const char *gfx125_misc_copy_mock_sha1 = "2d6f40adc6b3f8db879cc969b306c376ea35f1a4";
