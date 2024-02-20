#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_build_BFS_build_qnodes_scheduler_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_build_BFS_build_qnodes_scheduler_args[] = {
   { 0, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g23<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(8)          g20<1>D         0D                              { align1 1Q };
add(1)          g24<1>UD        g23<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(1)         g1UD            g24UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g17.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g17<2>F         g2<0,1,0>F                      { align1 1Q F@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
add(8)          g1<1>D          g17<8,4,2>D     28D             { align1 1Q F@1 compacted };
add(8)          g5<1>D          g17<8,4,2>D     4D              { align1 1Q compacted };
cmp.l.f0.0(8)   g2<1>UD         g1<8,8,1>UD     g17<8,4,2>UD    { align1 1Q A@1 };
mov(8)          g13<2>UD        g1<4,4,1>UD                     { align1 1Q };
cmp.l.f0.0(8)   g6<1>UD         g5<8,8,1>UD     g17<8,4,2>UD    { align1 1Q I@3 };
mov(8)          g15<2>UD        g5<4,4,1>UD                     { align1 1Q };
add(8)          g3<1>D          -g2<8,8,1>D     g17.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g7<1>D          -g6<8,8,1>D     g17.1<8,4,2>D   { align1 1Q I@3 };
mov(8)          g13.1<2>UD      g3<4,4,1>UD                     { align1 1Q I@2 };
mov(8)          g15.1<2>UD      g7<4,4,1>UD                     { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(8)         g4UD            g13UD           nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g8UD            g15UD           nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.dst };
sel.l(8)        g19<1>UD        g8<1,1,0>UD     g4<1,1,0>UD     { align1 1Q $2.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g17UD           g19UD           0x04003586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1Q $3 };
add(8)          g9<1>D          g17<8,4,2>D     40D             { align1 1Q $3.src compacted };
cmp.l.f0.0(8)   g10<1>UD        g9<8,8,1>UD     g17<8,4,2>UD    { align1 1Q I@1 };
mov(8)          g20<2>UD        g9<4,4,1>UD                     { align1 1Q $3.src };
add(8)          g11<1>D         -g10<8,8,1>D    g17.1<8,4,2>D   { align1 1Q I@2 };
mov(8)          g20.1<2>UD      g11<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g12UD           g20UD           nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $4 };
cmp.nz.f0.0(8)  null<1>D        g12<8,8,1>D     0D              { align1 1Q $4.dst };
(-f0.0) sel(8)  g22<1>UD        g4<8,8,1>UD     0x00000000UD    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g20UD           g22UD           0x04001586                0x00000040
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 1 flat )  base_offset 0  { align1 1Q $5 };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(8)         nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1Q A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_build_BFS_build_qnodes_scheduler_code[] = {
    0x80000065, 0x17058220, 0x02000004, 0xffffffc0,
    0x00030061, 0x14054660, 0x00000000, 0x00000000,
    0xe2181a40, 0x00011703, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa00180c, 0x00340000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x21110061, 0x001102cc,
    0x21111161, 0x00110204, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0xa1011140, 0x01ce1103,
    0xa1050040, 0x004e1103, 0x00030970, 0x02050220,
    0x52460105, 0x00441106, 0x00030061, 0x0d060220,
    0x00340105, 0x00000000, 0x00031b70, 0x06050220,
    0x52460505, 0x00441106, 0x00030061, 0x0f060220,
    0x00340505, 0x00000000, 0x00031c40, 0x03052660,
    0x06460205, 0x00441126, 0x00031b40, 0x07052660,
    0x06460605, 0x00441126, 0x00031a61, 0x0d260220,
    0x00340305, 0x00000000, 0x00031a61, 0x0f260220,
    0x00340705, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x00034131, 0x040c0000,
    0xfb040d14, 0x00040000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00034231, 0x080c0000,
    0xfb040f14, 0x00040000, 0x80002101, 0x00000000,
    0x00000000, 0x00000000, 0x30132262, 0x04000803,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034331, 0x00000000, 0xfb0c1114, 0x000c1314,
    0xa1093340, 0x028e1103, 0x00031970, 0x0a050220,
    0x52460905, 0x00441106, 0x00033361, 0x14060220,
    0x00340905, 0x00000000, 0x00031a40, 0x0b052660,
    0x06460a05, 0x00441126, 0x00031961, 0x14260220,
    0x00340b05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00034431, 0x0c0c0000,
    0xfb041414, 0x00040000, 0x00032470, 0x00018660,
    0x26460c05, 0x00000000, 0x11030062, 0x16058220,
    0x02460405, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00034531, 0x00000000,
    0xfb0c1414, 0x0004160c, 0x80030061, 0x7e050220,
    0x00460005, 0x00000000, 0x80030931, 0x00000004,
    0x30007e0c, 0x00000000, 0x20000060, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_build_BFS_build_qnodes_scheduler = {
   .prog_data = {
      .base.nr_params = 10,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 608,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_build_BFS_build_qnodes_scheduler_relocs,
      .base.uses_atomic_load_store = false,
      .local_size = { 1, 1, 1 },
      .prog_offset = { 0, 0, 0 },
      .prog_mask = 1,
      .prog_spilled = 0,
      .uses_barrier = false,
      .uses_num_work_groups = false,
      .push.per_thread.dwords = 0,
      .push.per_thread.regs = 0,
      .push.per_thread.size = 0,
      .push.cross_thread.dwords = 10,
      .push.cross_thread.regs = 2,
      .push.cross_thread.size = 64,
   },
   .args_size = 8,
   .arg_count = 1,
   .args = gfx125_bvh_build_BFS_build_qnodes_scheduler_args,
   .code = gfx125_bvh_build_BFS_build_qnodes_scheduler_code,
};
const char *gfx125_bvh_build_BFS_build_qnodes_scheduler_sha1 = "f48862fc7e364c8281f057f1a6a00dd2bd606a47";
