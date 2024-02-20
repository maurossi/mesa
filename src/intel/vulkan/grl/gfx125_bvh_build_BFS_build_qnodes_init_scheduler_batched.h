#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_build_BFS_build_qnodes_init_scheduler_batched_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_build_BFS_build_qnodes_init_scheduler_batched_args[] = {
   { 0, 8 },
   { 8, 4 },
   { 12, 4 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g27<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
add(1)          g28<1>UD        g27<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(1)         g1UD            g28UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g21.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g18<1>D         g2.3<0,1,0>D                    { align1 1Q };
mov(8)          g21<2>F         g2<0,1,0>F                      { align1 1Q F@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
add(8)          g1<1>D          g21<8,4,2>D     16D             { align1 1Q F@1 compacted };
add(8)          g6<1>D          g21<8,4,2>D     24D             { align1 1Q compacted };
cmp.l.f0.0(8)   g3<1>UD         g1<8,8,1>UD     g21<8,4,2>UD    { align1 1Q I@2 };
mov(8)          g13<2>UD        g1<4,4,1>UD                     { align1 1Q };
cmp.l.f0.0(8)   g7<1>UD         g6<8,8,1>UD     g21<8,4,2>UD    { align1 1Q I@3 };
mov(8)          g15<2>UD        g6<4,4,1>UD                     { align1 1Q };
add(8)          g4<1>D          -g3<8,8,1>D     g21.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g8<1>D          -g7<8,8,1>D     g21.1<8,4,2>D   { align1 1Q I@3 };
mov(8)          g13.1<2>UD      g4<4,4,1>UD                     { align1 1Q I@2 };
mov(8)          g15.1<2>UD      g8<4,4,1>UD                     { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(8)         g5UD            g13UD           nullUD          0x04203582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 2, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $1 };
add(8)          g20<1>D         g5<1,1,0>D      g6<1,1,0>D      { align1 1Q $1.dst compacted };
add(8)          g17<1>D         g2.2<0,1,0>D    -g20<1,1,0>D    { align1 1Q I@1 compacted };
mov(8)          g19<1>D         g17<8,8,1>D                     { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g15UD           g17UD           0x0400f586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 4 flat )  base_offset 0  { align1 1Q $2 };
add(8)          g9<1>D          g17<1,1,0>D     15D             { align1 1Q $2.src compacted };
mov(8)          g24<1>D         g17<8,8,1>D                     { align1 1Q };
shr(8)          g23<1>UD        g9<1,1,0>UD     0x00000004UD    { align1 1Q I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g21UD           g23UD           0x04003586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1Q $3 };
add(8)          g10<1>D         g21<8,4,2>D     40D             { align1 1Q $3.src compacted };
mov(8)          g26<1>D         g2.3<0,1,0>D                    { align1 1Q };
cmp.l.f0.0(8)   g11<1>UD        g10<8,8,1>UD    g21<8,4,2>UD    { align1 1Q I@2 };
mov(8)          g24<2>UD        g10<4,4,1>UD                    { align1 1Q $3.src };
add(8)          g12<1>D         -g11<8,8,1>D    g21.1<8,4,2>D   { align1 1Q I@2 };
mov(8)          g24.1<2>UD      g12<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g24UD           g26UD           0x04001586                0x00000040
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 1 flat )  base_offset 0  { align1 1Q $4 };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(8)         nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1Q A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_build_BFS_build_qnodes_init_scheduler_batched_code[] = {
    0x80000065, 0x1b058220, 0x02000004, 0xffffffc0,
    0xe21c1940, 0x00011b03, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa001c0c, 0x00340000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x21150061, 0x001102cc,
    0x00030061, 0x12050660, 0x00000264, 0x00000000,
    0x21151161, 0x00110204, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0xa1011140, 0x010e1503,
    0xa1060040, 0x018e1503, 0x00031a70, 0x03050220,
    0x52460105, 0x00441506, 0x00030061, 0x0d060220,
    0x00340105, 0x00000000, 0x00031b70, 0x07050220,
    0x52460605, 0x00441506, 0x00030061, 0x0f060220,
    0x00340605, 0x00000000, 0x00031c40, 0x04052660,
    0x06460305, 0x00441526, 0x00031b40, 0x08052660,
    0x06460705, 0x00441526, 0x00031a61, 0x0d260220,
    0x00340405, 0x00000000, 0x00031a61, 0x0f260220,
    0x00340805, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x00034131, 0x05140000,
    0xfb040d14, 0x000c0000, 0xa1142140, 0x06000502,
    0xa1111940, 0x14210242, 0x00031961, 0x13050660,
    0x00461105, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00034231, 0x00000000,
    0xfb0c0f14, 0x003c1124, 0xa1093240, 0x00f01103,
    0x00030061, 0x18050660, 0x00461105, 0x00000000,
    0xe1171a68, 0x00400903, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00034331, 0x00000000,
    0xfb0c1514, 0x000c1714, 0xa10a3340, 0x028e1503,
    0x00030061, 0x1a050660, 0x00000264, 0x00000000,
    0x00031a70, 0x0b050220, 0x52460a05, 0x00441506,
    0x00033361, 0x18060220, 0x00340a05, 0x00000000,
    0x00031a40, 0x0c052660, 0x06460b05, 0x00441526,
    0x00031961, 0x18260220, 0x00340c05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034431, 0x00000000, 0xfb0c1814, 0x00041a0c,
    0x80030061, 0x7e050220, 0x00460005, 0x00000000,
    0x80030931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_build_BFS_build_qnodes_init_scheduler_batched = {
   .prog_data = {
      .base.nr_params = 12,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 576,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_build_BFS_build_qnodes_init_scheduler_batched_relocs,
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
      .push.cross_thread.dwords = 12,
      .push.cross_thread.regs = 2,
      .push.cross_thread.size = 64,
   },
   .args_size = 16,
   .arg_count = 3,
   .args = gfx125_bvh_build_BFS_build_qnodes_init_scheduler_batched_args,
   .code = gfx125_bvh_build_BFS_build_qnodes_init_scheduler_batched_code,
};
const char *gfx125_bvh_build_BFS_build_qnodes_init_scheduler_batched_sha1 = "91a441090c3f5b661797f2e3c6d59148f162de6a";
