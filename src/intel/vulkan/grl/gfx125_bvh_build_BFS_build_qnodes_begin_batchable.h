#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_build_BFS_build_qnodes_begin_batchable_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_build_BFS_build_qnodes_begin_batchable_args[] = {
   { 0, 8 },
   { 8, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g106<1>UD       g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g78<1>UD        g0.1<0,1,0>UD                   { align1 1H };
add(1)          g107<1>UD       g106<0,1,0>UD   0x00000000UD    { align1 WE_all 1N I@2 compacted };
mov(8)          g10<1>UW        0x76543210V                     { align1 WE_all 1Q };
shl(16)         g19<1>D         g78<8,8,1>D     0x00000004UD    { align1 1H I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(1)         g1UD            g107UD          nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
add(8)          g10.8<1>UW      g10<1,1,0>UW    0x0008UW        { align1 WE_all 1Q I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         g21<1>D         g10<8,8,1>UW                    { align1 1H };
and(16)         g23<1>UD        g21<1,1,0>UD    0x0000000fUD    { align1 1H I@1 compacted };
add(16)         g25<1>D         g19<1,1,0>D     g23<1,1,0>D     { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g96.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g11.1<2>F       g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g98.1<2>F       g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g17.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g96<2>F         g2<0,1,0>F                      { align1 1Q F@4 compacted };
mov(8)          g11<2>F         g2<0,1,0>F                      { align1 2Q F@4 compacted };
mov(8)          g98<2>F         g2.2<0,1,0>F                    { align1 1Q F@4 compacted };
mov(8)          g17<2>F         g2.2<0,1,0>F                    { align1 2Q F@4 compacted };
mov(8)          g27<1>UD        g96.1<8,4,2>UD                  { align1 1Q F@4 };
add(8)          g100<1>D        g96<8,4,2>D     24D             { align1 1Q compacted };
mov(8)          g28<1>UD        g11.1<8,4,2>UD                  { align1 2Q F@3 };
add(8)          g29<1>D         g11<8,4,2>D     24D             { align1 2Q compacted };
cmp.l.f0.0(8)   g101<1>UD       g100<8,8,1>UD   g96<8,4,2>UD    { align1 1Q I@3 };
mov(8)          g80<2>UD        g100<4,4,1>UD                   { align1 1Q };
cmp.l.f0.0(8)   g30<1>UD        g29<8,8,1>UD    g11<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g82<2>UD        g29<4,4,1>UD                    { align1 2Q };
add(8)          g102<1>D        -g101<8,8,1>D   g96.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g31<1>D         -g30<8,8,1>D    g11.1<8,4,2>D   { align1 2Q I@3 };
mov(8)          g80.1<2>UD      g102<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g82.1<2>UD      g31<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g55UD           g80UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
cmp.l.f0.0(16)  null<1>UD       g25<8,8,1>UD    g55<8,8,1>UD    { align1 1H $1.dst };
(+f0.0) if(16)  JIP:  LABEL0          UIP:  LABEL0              { align1 1H };
add(8)          g103<1>D        g96<8,4,2>D     36D             { align1 1Q compacted };
add(8)          g56<1>D         g11<8,4,2>D     36D             { align1 2Q compacted };
add(8)          g37<1>D         g96<8,4,2>D     44D             { align1 1Q compacted };
add(8)          g38<1>D         g11<8,4,2>D     44D             { align1 2Q compacted };
shl(16)         g40<1>D         g25<8,8,1>D     0x00000004UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(16)         g1<1>D          0D                              { align1 1H F@1 };
mov(16)         g3<1>D          0D                              { align1 1H };
mov(16)         g7<1>D          1D                              { align1 1H };
cmp.l.f0.0(8)   g104<1>UD       g103<8,8,1>UD   g96<8,4,2>UD    { align1 1Q I@7 };
cmp.l.f0.0(8)   g34<1>UD        g56<8,8,1>UD    g11<8,4,2>UD    { align1 2Q I@7 };
mov(8)          g81<2>UD        g103<4,4,1>UD                   { align1 1Q $1.src };
mov(8)          g83<2>UD        g56<4,4,1>UD                    { align1 2Q $1.src };
add(16)         g42<1>D         g37<1,1,0>D     g40<1,1,0>D     { align1 1H I@7 compacted };
cmp.l.f0.0(16)  g39<1>UD        g37<1,1,0>UD    0x0000002cUD    { align1 1H compacted };
add(8)          g105<1>D        -g104<8,8,1>D   g96.1<8,4,2>D   { align1 1Q I@6 };
add(8)          g35<1>D         -g34<8,8,1>D    g11.1<8,4,2>D   { align1 2Q I@6 };
cmp.l.f0.0(16)  g44<1>UD        g42<1,1,0>UD    g37<1,1,0>UD    { align1 1H I@4 compacted };
mov(8)          g85<2>UD        g42<4,4,1>UD                    { align1 1Q };
mov(8)          g87<2>UD        g43<4,4,1>UD                    { align1 2Q };
mov(8)          g81.1<2>UD      g105<4,4,1>UD                   { align1 1Q I@5 };
mov(8)          g83.1<2>UD      g35<4,4,1>UD                    { align1 2Q I@5 };
add3(16)        g46<1>D         -g39<8,8,1>D    g27<8,8,1>D     -g44<1,1,1>D { align1 1H I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g36UD           g81UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
mov(8)          g85.1<2>UD      g46<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g87.1<2>UD      g47<4,4,1>UD                    { align1 2Q I@2 };
add(16)         g5<1>D          g36<1,1,0>D     g25<1,1,0>D     { align1 1H $1.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g85UD           g1UD            0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $0 };
mul(8)          acc0<1>UD       g5<8,8,1>UD     0x0058UW        { align1 1Q $0.src };
mul(16)         g49<1>D         g5<1,1,0>D      88W             { align1 1H $0.src compacted };
mov(8)          g51<1>UD        g98.1<8,4,2>UD                  { align1 1Q F@2 };
mov(8)          g52<1>UD        g17.1<8,4,2>UD                  { align1 2Q F@1 };
mov(16)         g9<1>D          0D                              { align1 1H };
mov(16)         g11<1>D         0D                              { align1 1H };
mov(16)         g13<1>D         0D                              { align1 1H };
mov(16)         g15<1>D         0D                              { align1 1H };
mach(8)         g47<1>UD        g5<1,1,0>UD     0x00000058UD    { align1 1Q compacted AccWrEnable };
add(8)          g53<1>D         g98<8,4,2>D     g49<1,1,0>D     { align1 1Q I@7 compacted };
add(8)          g54<1>D         g17<8,4,2>D     g50<1,1,0>D     { align1 2Q I@7 compacted };
mul(8)          acc0<1>UD       g6<8,8,1>UD     0x0058UW        { align1 2Q };
cmp.l.f0.0(8)   g32<1>UD        g53<8,8,1>UD    g98<8,4,2>UD    { align1 1Q I@3 };
cmp.l.f0.0(8)   g33<1>UD        g54<8,8,1>UD    g17<8,4,2>UD    { align1 2Q I@3 };
add(16)         g58<1>D         g53<1,1,0>D     64D             { align1 1H compacted };
mach(8)         g48<1>UD        g6<8,8,1>UD     0x00000058UD    { align1 2Q AccWrEnable };
cmp.l.f0.0(16)  g60<1>UD        g58<1,1,0>UD    g53<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g86<2>UD        g58<4,4,1>UD                    { align1 1Q $0.src };
mov(8)          g88<2>UD        g59<4,4,1>UD                    { align1 2Q $0.src };
add3(16)        g57<1>D         g51<8,8,1>D     g47<8,8,1>D     -g32<1,1,1>D { align1 1H I@4 };
add(16)         g62<1>D         -g60<1,1,0>D    g57<1,1,0>D     { align1 1H I@1 compacted };
mov(8)          g86.1<2>UD      g62<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g88.1<2>UD      g63<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g86UD           g9UD            0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $2 };
add(16)         g63<1>D         g53<1,1,0>D     32D             { align1 1H compacted };
cmp.l.f0.0(16)  g65<1>UD        g63<1,1,0>UD    g53<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g87<2>UD        g63<4,4,1>UD                    { align1 1Q $2.src };
mov(8)          g89<2>UD        g64<4,4,1>UD                    { align1 2Q $2.src };
add(16)         g67<1>D         -g65<1,1,0>D    g57<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g87.1<2>UD      g67<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g89.1<2>UD      g68<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g68UD           g87UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
add(16)         g72<1>D         g68<1,1,0>D     12D             { align1 1H $3.dst compacted };
cmp.l.f0.0(16)  g74<1>UD        g72<1,1,0>UD    g68<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g91<2>UD        g72<4,4,1>UD                    { align1 1Q };
mov(8)          g93<2>UD        g73<4,4,1>UD                    { align1 2Q };
add(16)         g76<1>D         -g74<1,1,0>D    g70<1,1,0>D     { align1 1H @3 $3.dst compacted };
mov(8)          g91.1<2>UD      g76<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g93.1<2>UD      g77<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g77UD           g91UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
add(16)         g95<1>D         g77<1,1,0>D     1D              { align1 1H $4.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g91UD           g95UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $5 };

LABEL0:
endif(16)       JIP:  LABEL1                                    { align1 1H };

LABEL1:
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_build_BFS_build_qnodes_begin_batchable_code[] = {
    0x80000065, 0x6a058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x4e050220, 0x00000024, 0x00000000,
    0xe26b1a40, 0x00016a03, 0x80030061, 0x0a054410,
    0x00000000, 0x76543210, 0x00041b69, 0x13058660,
    0x02464e05, 0x00000004, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa006b0c, 0x00340000, 0x640a1a40, 0x00800a95,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00040061, 0x15050160, 0x00460a05, 0x00000000,
    0xe0171965, 0x00f01503, 0xa0191940, 0x17001302,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x21600061, 0x001102cc, 0x2a0b0061, 0x001102cc,
    0x00030061, 0x62260aa0, 0x00000264, 0x00000000,
    0x00130061, 0x11260aa0, 0x00000264, 0x00000000,
    0x21601461, 0x00110204, 0x2a0b1461, 0x00110204,
    0x21621461, 0x00110244, 0x2a111461, 0x00110244,
    0x00031461, 0x1b050220, 0x00446026, 0x00000000,
    0xa1640040, 0x018e6003, 0x00131361, 0x1c050220,
    0x00440b26, 0x00000000, 0xaa1d0040, 0x018e0b03,
    0x00031b70, 0x65050220, 0x52466405, 0x00446006,
    0x00030061, 0x50060220, 0x00346405, 0x00000000,
    0x00131b70, 0x1e050220, 0x52461d05, 0x00440b06,
    0x00130061, 0x52060220, 0x00341d05, 0x00000000,
    0x00031c40, 0x66052660, 0x06466505, 0x00446026,
    0x00131b40, 0x1f052660, 0x06461e05, 0x00440b26,
    0x00031a61, 0x50260220, 0x00346605, 0x00000000,
    0x00131a61, 0x52260220, 0x00341f05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x37140000, 0xfb045024, 0x00040000,
    0x00042170, 0x00010220, 0x52461905, 0x00463705,
    0x01040022, 0x0001c060, 0x00000430, 0x00000430,
    0xa1670040, 0x024e6003, 0xaa380040, 0x024e0b03,
    0xa1250040, 0x02ce6003, 0xaa260040, 0x02ce0b03,
    0x00040069, 0x28058660, 0x02461905, 0x00000004,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x00041161, 0x01054660, 0x00000000, 0x00000000,
    0x00040061, 0x03054660, 0x00000000, 0x00000000,
    0x00040061, 0x07054660, 0x00000000, 0x00000001,
    0x00031f70, 0x68050220, 0x52466705, 0x00446006,
    0x00131f70, 0x22050220, 0x52463805, 0x00440b06,
    0x00033161, 0x51060220, 0x00346705, 0x00000000,
    0x00133161, 0x53060220, 0x00343805, 0x00000000,
    0xa02a1f40, 0x28002502, 0xe7270070, 0x02c02503,
    0x00031e40, 0x69052660, 0x06466805, 0x00446026,
    0x00131e40, 0x23052660, 0x06462205, 0x00440b26,
    0x272c1c70, 0x25002a03, 0x00030061, 0x55060220,
    0x00342a05, 0x00000000, 0x00130061, 0x57060220,
    0x00342b05, 0x00000000, 0x00031d61, 0x51260220,
    0x00346905, 0x00000000, 0x00131d61, 0x53260220,
    0x00342305, 0x00000000, 0x00041d52, 0x2e042e68,
    0x0e2e2705, 0x2c051b05, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x00044131, 0x24140000,
    0xfb045124, 0x00040000, 0x00031961, 0x55260220,
    0x00342e05, 0x00000000, 0x00131a61, 0x57260220,
    0x00342f05, 0x00000000, 0xa0052140, 0x19002402,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x00000000, 0xfb0c5524, 0x003c0144,
    0x00033041, 0x20018220, 0x01460505, 0x00580058,
    0x60313041, 0x05800502, 0x00031261, 0x33050220,
    0x00446226, 0x00000000, 0x00131161, 0x34050220,
    0x00441126, 0x00000000, 0x00040061, 0x09054660,
    0x00000000, 0x00000000, 0x00040061, 0x0b054660,
    0x00000000, 0x00000000, 0x00040061, 0x0d054660,
    0x00000000, 0x00000000, 0x00040061, 0x0f054660,
    0x00000000, 0x00000000, 0xfe2f0049, 0x05800503,
    0xa1351f40, 0x310e6202, 0xaa361f40, 0x320e1102,
    0x00130041, 0x20018220, 0x01460605, 0x00580058,
    0x00031b70, 0x20050220, 0x52463505, 0x00446206,
    0x00131b70, 0x21050220, 0x52463605, 0x00441106,
    0xa03a0040, 0x04003503, 0x00130049, 0x30058222,
    0x02460605, 0x00000058, 0x273c1a70, 0x35003a03,
    0x00033061, 0x56060220, 0x00343a05, 0x00000000,
    0x00133061, 0x58060220, 0x00343b05, 0x00000000,
    0x00041c52, 0x39040e68, 0x0e2e3305, 0x20052f05,
    0xa03e1940, 0x39023c02, 0x00031961, 0x56260220,
    0x00343e05, 0x00000000, 0x00131a61, 0x58260220,
    0x00343f05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb0c5624, 0x003c0944, 0xa03f0040, 0x02003503,
    0x27411970, 0x35003f03, 0x00033261, 0x57060220,
    0x00343f05, 0x00000000, 0x00133261, 0x59060220,
    0x00344005, 0x00000000, 0xa0431b40, 0x39024102,
    0x00031961, 0x57260220, 0x00344305, 0x00000000,
    0x00131a61, 0x59260220, 0x00344405, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x44240000, 0xfb045724, 0x000c0000,
    0xa0482340, 0x00c04403, 0x274a1970, 0x44004803,
    0x00030061, 0x5b060220, 0x00344805, 0x00000000,
    0x00130061, 0x5d060220, 0x00344905, 0x00000000,
    0xa04cb340, 0x46024a02, 0x00031961, 0x5b260220,
    0x00344c05, 0x00000000, 0x00131a61, 0x5d260220,
    0x00344d05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044431, 0x4d140000,
    0xfb045b24, 0x00040000, 0xa05f2440, 0x00104d03,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044531, 0x00000000, 0xfb0c5b24, 0x00045f14,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x80030061, 0x7e050220, 0x00460005, 0x00000000,
    0x80040931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_build_BFS_build_qnodes_begin_batchable = {
   .prog_data = {
      .base.nr_params = 12,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 1600,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_build_BFS_build_qnodes_begin_batchable_relocs,
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
      .push.cross_thread.dwords = 12,
      .push.cross_thread.regs = 2,
      .push.cross_thread.size = 64,
   },
   .args_size = 16,
   .arg_count = 2,
   .args = gfx125_bvh_build_BFS_build_qnodes_begin_batchable_args,
   .code = gfx125_bvh_build_BFS_build_qnodes_begin_batchable_code,
};
const char *gfx125_bvh_build_BFS_build_qnodes_begin_batchable_sha1 = "088249b92f17353c7bcc6397f67c90a18724ba80";
