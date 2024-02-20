#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_misc_mem_set_size_ptr_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_misc_mem_set_size_ptr_args[] = {
   { 0, 8 },
   { 8, 4 },
   { 16, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g48<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g38<1>UD        g0.1<0,1,0>UD                   { align1 1H };
and(16)         g8<1>UD         g0.2<0,1,0>UD   0x000000ffUD    { align1 1H compacted };
add(1)          g49<1>UD        g48<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@3 compacted };
mov(8)          g3<1>UW         0x76543210V                     { align1 WE_all 1Q };
shl(16)         g18<1>D         g38<8,8,1>D     0x00000005UD    { align1 1H I@4 };
shl(16)         g10<1>D         g8<8,8,1>D      0x00000004UD    { align1 1H I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
send(1)         g1UD            g49UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
add(8)          g3.8<1>UW       g3<1,1,0>UW     0x0008UW        { align1 WE_all 1Q I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         g12<1>D         g3<8,8,1>UW                     { align1 1H };
add(16)         g14<1>D         g12<1,1,0>D     g10<1,1,0>D     { align1 1H I@1 compacted };
and(16)         g16<1>UD        g14<1,1,0>UD    0x0000001fUD    { align1 1H I@1 compacted };
add(16)         g20<1>D         g16<1,1,0>D     g18<1,1,0>D     { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g4.1<2>F        g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g6.1<2>F        g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g40.1<2>F       g2.5<0,1,0>F                    { align1 1Q };
mov(8)          g42.1<2>F       g2.5<0,1,0>F                    { align1 2Q };
mov(8)          g4<2>F          g2<0,1,0>F                      { align1 1Q F@4 compacted };
mov(8)          g6<2>F          g2<0,1,0>F                      { align1 2Q F@4 compacted };
mov(8)          g40<2>F         g2.4<0,1,0>F                    { align1 1Q F@4 compacted };
mov(8)          g42<2>F         g2.4<0,1,0>F                    { align1 2Q F@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
send(16)        g22UD           g40UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
cmp.z.f0.0(16)  g26<1>D         g24<1,1,0>D     0D              { align1 1H $1.dst compacted };
cmp.l.f0.0(16)  g28<1>UD        g20<1,1,0>UD    g22<1,1,0>UD    { align1 1H @2 $1.dst compacted };
cmp.g.f0.0(16)  g32<1>UD        g24<1,1,0>UD    0x00000000UD    { align1 1H compacted };
and(16)         g30<1>UD        g26<1,1,0>UD    g28<1,1,0>UD    { align1 1H I@2 compacted };
or.nz.f0.0(16)  null<1>UD       g32<8,8,1>UD    g30<8,8,1>UD    { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL0          UIP:  LABEL0              { align1 1H };
add(8)          g33<1>D         g4<8,4,2>D      g20<1,1,0>D     { align1 1Q F@4 compacted };
add(8)          g34<1>D         g6<8,4,2>D      g21<1,1,0>D     { align1 2Q F@3 compacted };
mov(16)         g47<4>UB        g2.2<0,1,0>UD                   { align1 1H };
cmp.l.f0.0(8)   g35<1>UD        g33<8,8,1>UD    g4<8,4,2>UD     { align1 1Q I@3 };
cmp.l.f0.0(8)   g36<1>UD        g34<8,8,1>UD    g6<8,4,2>UD     { align1 2Q I@3 };
mov(8)          g41<2>UD        g33<4,4,1>UD                    { align1 1Q $1.src };
mov(8)          g43<2>UD        g34<4,4,1>UD                    { align1 2Q $1.src };
mov(16)         g45<1>UD        g47<32,8,4>UB                   { align1 1H I@5 };
add(8)          g37<1>D         -g35<8,8,1>D    g4.1<8,4,2>D    { align1 1Q I@5 };
add(8)          g38<1>D         -g36<8,8,1>D    g6.1<8,4,2>D    { align1 2Q I@5 };
mov(8)          g41.1<2>UD      g37<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g43.1<2>UD      g38<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g41UD           g45UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $1 };

LABEL0:
endif(16)       JIP:  LABEL1                                    { align1 1H };

LABEL1:
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_misc_mem_set_size_ptr_code[] = {
    0x80000065, 0x30058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x26050220, 0x00000024, 0x00000000,
    0xe0080065, 0x0ff10043, 0xe2311b40, 0x00013003,
    0x80030061, 0x03054410, 0x00000000, 0x76543210,
    0x00041c69, 0x12058660, 0x02462605, 0x00000005,
    0x00041c69, 0x0a058660, 0x02460805, 0x00000004,
    0x80001c01, 0x00000000, 0x00000000, 0x00000000,
    0x80004031, 0x01140000, 0xfa00310c, 0x00340000,
    0x64031b40, 0x00800395, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00040061, 0x0c050160,
    0x00460305, 0x00000000, 0xa00e1940, 0x0a000c02,
    0xe0101965, 0x01f00e03, 0xa0141940, 0x12001002,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x21040061, 0x001102cc, 0x2a060061, 0x001102cc,
    0x00030061, 0x28260aa0, 0x000002a4, 0x00000000,
    0x00130061, 0x2a260aa0, 0x000002a4, 0x00000000,
    0x21041461, 0x00110204, 0x2a061461, 0x00110204,
    0x21281461, 0x0011025c, 0x2a2a1461, 0x0011025c,
    0x80001101, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x16240000, 0xfb042824, 0x000c0000,
    0xac1a2170, 0x00001803, 0x271ca170, 0x16001403,
    0xeb200070, 0x00001803, 0x201e1a65, 0x1c001a03,
    0x00041966, 0x00010220, 0x22462005, 0x00461e05,
    0x01040022, 0x0001c060, 0x000000e0, 0x000000e0,
    0xa1211440, 0x140e0402, 0xaa221340, 0x150e0602,
    0x00040061, 0x2f070200, 0x00000244, 0x00000000,
    0x00031b70, 0x23050220, 0x52462105, 0x00440406,
    0x00131b70, 0x24050220, 0x52462205, 0x00440606,
    0x00033161, 0x29060220, 0x00342105, 0x00000000,
    0x00133161, 0x2b060220, 0x00342205, 0x00000000,
    0x00041d61, 0x2d050020, 0x00662f07, 0x00000000,
    0x00031d40, 0x25052660, 0x06462305, 0x00440426,
    0x00131d40, 0x26052660, 0x06462405, 0x00440626,
    0x00031a61, 0x29260220, 0x00342505, 0x00000000,
    0x00131a61, 0x2b260220, 0x00342605, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x00000000, 0xf3082924, 0x00022d14,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x80030061, 0x7e050220, 0x00460005, 0x00000000,
    0x80040931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_misc_mem_set_size_ptr = {
   .prog_data = {
      .base.nr_params = 14,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 640,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_misc_mem_set_size_ptr_relocs,
      .base.uses_atomic_load_store = false,
      .local_size = { 32, 1, 1 },
      .prog_offset = { 0, 0, 0 },
      .prog_mask = 2,
      .prog_spilled = 0,
      .uses_barrier = false,
      .uses_num_work_groups = false,
      .push.per_thread.dwords = 0,
      .push.per_thread.regs = 0,
      .push.per_thread.size = 0,
      .push.cross_thread.dwords = 14,
      .push.cross_thread.regs = 2,
      .push.cross_thread.size = 64,
   },
   .args_size = 24,
   .arg_count = 3,
   .args = gfx125_misc_mem_set_size_ptr_args,
   .code = gfx125_misc_mem_set_size_ptr_code,
};
const char *gfx125_misc_mem_set_size_ptr_sha1 = "710ab3ed15de4fbf43a306720d7ff2d18ed75df6";
