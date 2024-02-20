#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_postbuild_info_current_size_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_postbuild_info_current_size_args[] = {
   { 0, 8 },
   { 8, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g13<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
add(1)          g14<1>UD        g13<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(1)         g1UD            g14UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g3.1<2>F        g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g10.1<2>F       g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g3<2>F          g2<0,1,0>F                      { align1 1Q F@2 compacted };
mov(8)          g10<2>F         g2.2<0,1,0>F                    { align1 1Q F@2 compacted };
add(8)          g5<1>D          g3<8,4,2>D      168D            { align1 1Q F@2 compacted };
cmp.l.f0.0(8)   g6<1>UD         g5<1,1,0>UD     0x000000a8UD    { align1 1Q I@1 compacted };
mov(8)          g8<2>UD         g5<4,4,1>UD                     { align1 1Q };
add(8)          g7<1>D          -g6<8,8,1>D     g3.1<8,4,2>D    { align1 1Q I@2 };
mov(8)          g8.1<2>UD       g7<4,4,1>UD                     { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g12UD           g8UD            nullUD          0x04203582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 2, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
send(8)         nullUD          g10UD           g12UD           0x04003586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1Q $2 };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(8)         nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1Q A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_postbuild_info_current_size_code[] = {
    0x80000065, 0x0d058220, 0x02000004, 0xffffffc0,
    0xe20e1940, 0x00010d03, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa000e0c, 0x00340000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x21030061, 0x001102cc,
    0x00030061, 0x0a260aa0, 0x00000264, 0x00000000,
    0x21031261, 0x00110204, 0x210a1261, 0x00110244,
    0xa1051240, 0x0a8e0303, 0xf0061970, 0x0a800503,
    0x00030061, 0x08060220, 0x00340505, 0x00000000,
    0x00031a40, 0x07052660, 0x06460605, 0x00440326,
    0x00031961, 0x08260220, 0x00340705, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034131, 0x0c140000, 0xfb040814, 0x000c0000,
    0x80002101, 0x00000000, 0x00000000, 0x00000000,
    0x80001101, 0x00000000, 0x00000000, 0x00000000,
    0x00034231, 0x00000000, 0xfb0c0a14, 0x000c0c14,
    0x80030061, 0x7e050220, 0x00460005, 0x00000000,
    0x80030931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_postbuild_info_current_size = {
   .prog_data = {
      .base.nr_params = 12,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 288,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_postbuild_info_current_size_relocs,
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
   .arg_count = 2,
   .args = gfx125_bvh_postbuild_info_current_size_args,
   .code = gfx125_bvh_postbuild_info_current_size_code,
};
const char *gfx125_bvh_postbuild_info_current_size_sha1 = "7d5f2340850254598b069b1cb0f3719de63dec96";
