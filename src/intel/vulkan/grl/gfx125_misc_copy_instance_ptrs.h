#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_misc_copy_instance_ptrs_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_misc_copy_instance_ptrs_args[] = {
   { 0, 8 },
   { 8, 8 },
   { 16, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g33<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g21<1>UD        g0.1<0,1,0>UD                   { align1 1H };
add(1)          g34<1>UD        g33<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@2 compacted };
mov(16)         g7<1>UD         g21<8,8,1>UD                    { align1 1H I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(1)         g1UD            g34UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g3.1<2>F        g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g9.1<2>F        g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g5.1<2>F        g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g11.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g35.1<2>F       g2.5<0,1,0>F                    { align1 1Q };
mov(8)          g13.1<2>F       g2.5<0,1,0>F                    { align1 2Q };
mov(8)          g3<2>F          g2<0,1,0>F                      { align1 1Q F@6 compacted };
mov(8)          g9<2>F          g2<0,1,0>F                      { align1 2Q F@6 compacted };
mov(8)          g5<2>F          g2.2<0,1,0>F                    { align1 1Q F@6 compacted };
mov(8)          g11<2>F         g2.2<0,1,0>F                    { align1 2Q F@6 compacted };
mov(8)          g35<2>F         g2.4<0,1,0>F                    { align1 1Q F@6 compacted };
mov(8)          g13<2>F         g2.4<0,1,0>F                    { align1 2Q F@6 compacted };
shr(8)          g15<1>UD        g35<8,4,2>UD    0x00000008UD    { align1 1Q F@2 compacted };
and(8)          g17<1>UD        g35<8,4,2>UD    0xffffff00UD    { align1 1Q };
shr(8)          g16<1>UD        g13<8,4,2>UD    0x00000008UD    { align1 2Q F@1 compacted };
and(8)          g18<1>UD        g13<8,4,2>UD    0xffffff00UD    { align1 2Q };
add(8)          g19<1>D         g35<8,4,2>D     -g17<1,1,0>D    { align1 1Q I@3 compacted };
add3(16)        g25<1>D         65534W          -g15<8,8,1>D    g21<1,1,1>D { align1 1H I@3 };
add(8)          g20<1>D         g13<8,4,2>D     -g18<1,1,0>D    { align1 2Q I@3 compacted };
mov(16)         g27<1>D         -g25<8,8,1>D                    { align1 1H I@2 };
and(16)         g21<1>UD        g19<8,8,1>UD    0xffffffc0UD    { align1 1H I@2 };
shr(16)         g23<1>UD        g21<1,1,0>UD    0x00000006UD    { align1 1H I@1 compacted };
cmp.l.f0.0(16)  null<1>UD       g27<8,8,1>UD    g23<8,8,1>UD    { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL0          UIP:  LABEL0              { align1 1H };
shl(16)         g28<1>D         -g25<8,8,1>D    0x00000006UD    { align1 1H };
add(16)         g30<1>D         g17<1,1,0>D     g28<1,1,0>D     { align1 1H I@1 compacted };
shr(16)         g32<1>UD        g30<1,1,0>UD    0x00000006UD    { align1 1H I@1 compacted };
add(8)          g39<1>D         g3<8,4,2>D      g30<1,1,0>D     { align1 1Q F@6 compacted };
add(8)          g46<1>D         g9<8,4,2>D      g31<1,1,0>D     { align1 2Q F@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
shl(16)         g34<1>D         g32<8,8,1>D     0x00000003UD    { align1 1H I@3 };
mov(8)          g49<2>UD        g39<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g51<2>UD        g46<4,4,1>UD                    { align1 2Q I@3 };
add(8)          g36<1>D         g5<8,4,2>D      g34<1,1,0>D     { align1 1Q A@3 compacted };
add(8)          g35<1>D         g11<8,4,2>D     g35<1,1,0>D     { align1 2Q A@3 compacted };
cmp.l.f0.0(8)   g37<1>UD        g36<8,8,1>UD    g5<8,4,2>UD     { align1 1Q I@2 };
mov(8)          g53<2>UD        g36<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g40<1>UD        g39<8,8,1>UD    g3<8,4,2>UD     { align1 1Q };
mov(8)          g55<2>UD        g35<4,4,1>UD                    { align1 2Q I@4 };
cmp.l.f0.0(8)   g36<1>UD        g35<8,8,1>UD    g11<8,4,2>UD    { align1 2Q };
cmp.l.f0.0(8)   g47<1>UD        g46<8,8,1>UD    g9<8,4,2>UD     { align1 2Q };
add(8)          g38<1>D         -g37<8,8,1>D    g5.1<8,4,2>D    { align1 1Q I@6 };
add(8)          g41<1>D         -g40<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@5 };
add(8)          g37<1>D         -g36<8,8,1>D    g11.1<8,4,2>D   { align1 2Q I@4 };
add(8)          g48<1>D         -g47<8,8,1>D    g9.1<8,4,2>D    { align1 2Q I@4 };
mov(8)          g53.1<2>UD      g38<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g49.1<2>UD      g41<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g55.1<2>UD      g37<4,4,1>UD                    { align1 2Q I@4 };
mov(8)          g51.1<2>UD      g48<4,4,1>UD                    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g38UD           g53UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
mov(8)          g42<2>UD        g38<4,4,1>UD                    { align1 1Q $1.dst };
mov(8)          g44<2>UD        g39<4,4,1>UD                    { align1 2Q $1.dst };
mov(8)          g42.1<2>UD      g40<4,4,1>UD                    { align1 1Q @2 $1.dst };
mov(8)          g44.1<2>UD      g41<4,4,1>UD                    { align1 2Q @2 $1.dst };
fbl(1)          g22<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g22<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000400UD    { align1 WE_all 1N A@1 };
mov(1)          g70<2>D         g[a0 320]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g70.1<2>D       g[a0 324]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(8)          g54.1<2>UD      g70.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g54<2>UD        g70<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g34<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g59UD    g54UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $2 };
mov(1)          f0<1>UD         g34<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g22<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000600UD    { align1 WE_all 1N A@1 };
mov(1)          g71<2>D         g[a0 32]<0,1,0>D                { align1 WE_all 1N A@1 };
mov(1)          g71.1<2>D       g[a0 36]<0,1,0>D                { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(8)          g55.1<2>UD      g71.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g55<2>UD        g71<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g55UD           g59UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $3 };

LABEL0:
endif(16)       JIP:  LABEL1                                    { align1 1H };

LABEL1:
shr(16)         g50<1>UD        g17<1,1,0>UD    0x00000008UD    { align1 1H compacted };

LABEL3:
cmp.ge.f0.0(16) null<1>UD       g7<8,8,1>UD     g50<8,8,1>UD    { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL2        UIP:  LABEL2              { align1 1H };
shl(16)         g52<1>D         g7<8,8,1>D      0x00000008UD    { align1 1H $1.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
shr(16)         g54<1>UD        g52<1,1,0>UD    0x00000006UD    { align1 1H I@1 compacted };
add(8)          g45<1>D         g3<8,4,2>D      g52<1,1,0>D     { align1 1Q F@6 compacted };
add(8)          g68<1>D         g9<8,4,2>D      g53<1,1,0>D     { align1 2Q F@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
shl(16)         g56<1>D         g54<8,8,1>D     0x00000003UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
mov(8)          g71<2>UD        g45<4,4,1>UD                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@3 };
mov(8)          g73<2>UD        g68<4,4,1>UD                    { align1 2Q };
add(8)          g42<1>D         g5<8,4,2>D      g56<1,1,0>D     { align1 1Q A@3 compacted };
add(8)          g57<1>D         g11<8,4,2>D     g57<1,1,0>D     { align1 2Q A@3 compacted };
cmp.l.f0.0(8)   g43<1>UD        g42<8,8,1>UD    g5<8,4,2>UD     { align1 1Q I@2 };
mov(8)          g75<2>UD        g42<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g46<1>UD        g45<8,8,1>UD    g3<8,4,2>UD     { align1 1Q };
cmp.l.f0.0(8)   g58<1>UD        g57<8,8,1>UD    g11<8,4,2>UD    { align1 2Q I@4 };
mov(8)          g77<2>UD        g57<4,4,1>UD                    { align1 2Q };
cmp.l.f0.0(8)   g69<1>UD        g68<8,8,1>UD    g9<8,4,2>UD     { align1 2Q };
add(8)          g44<1>D         -g43<8,8,1>D    g5.1<8,4,2>D    { align1 1Q I@6 };
add(8)          g47<1>D         -g46<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $3.src };
add(8)          g59<1>D         -g58<8,8,1>D    g11.1<8,4,2>D   { align1 2Q I@5 };
add(8)          g70<1>D         -g69<8,8,1>D    g9.1<8,4,2>D    { align1 2Q I@4 };
mov(8)          g75.1<2>UD      g44<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g71.1<2>UD      g47<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g77.1<2>UD      g59<4,4,1>UD                    { align1 2Q I@4 };
mov(8)          g73.1<2>UD      g70<4,4,1>UD                    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g60UD           g75UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $5 };
mov(8)          g64<2>UD        g60<4,4,1>UD                    { align1 1Q $5.dst };
mov(8)          g66<2>UD        g61<4,4,1>UD                    { align1 2Q $5.dst };
mov(8)          g64.1<2>UD      g62<4,4,1>UD                    { align1 1Q @2 $5.dst };
mov(8)          g66.1<2>UD      g63<4,4,1>UD                    { align1 2Q @2 $5.dst };
fbl(1)          g23<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N $4.src compacted };
shl(1)          a0<1>UD         g23<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000800UD    { align1 WE_all 1N A@1 };
mov(1)          g80<2>D         g[a0]<0,1,0>D                   { align1 WE_all 1N A@1 };
mov(1)          g80.1<2>D       g[a0 4]<0,1,0>D                 { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(8)          g76.1<2>UD      g80.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g76<2>UD        g80<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g33<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g81UD    g76UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $6 };
mov(1)          f0<1>UD         g33<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g23<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000800UD    { align1 WE_all 1N A@1 };
mov(1)          g83<2>D         g[a0 224]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g83.1<2>D       g[a0 228]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(8)          g77.1<2>UD      g83.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g77<2>UD        g83<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g77UD           g81UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $7 };
add(16)         g72<1>D         g52<1,1,0>D     64D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
shr(16)         g74<1>UD        g72<1,1,0>UD    0x00000006UD    { align1 1H I@1 compacted };
add(8)          g55<1>D         g3<8,4,2>D      g72<1,1,0>D     { align1 1Q compacted };
add(8)          g88<1>D         g9<8,4,2>D      g73<1,1,0>D     { align1 2Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
shl(16)         g76<1>D         g74<8,8,1>D     0x00000003UD    { align1 1H I@3 };
mov(8)          g91<2>UD        g55<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g93<2>UD        g88<4,4,1>UD                    { align1 2Q I@3 };
add(8)          g48<1>D         g5<8,4,2>D      g76<1,1,0>D     { align1 1Q I@3 compacted };
add(8)          g77<1>D         g11<8,4,2>D     g77<1,1,0>D     { align1 2Q I@4 compacted };
mov(8)          g95<2>UD        g48<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g97<2>UD        g77<4,4,1>UD                    { align1 2Q I@2 };
cmp.l.f0.0(8)   g78<1>UD        g77<8,8,1>UD    g11<8,4,2>UD    { align1 2Q };
cmp.l.f0.0(8)   g49<1>UD        g48<8,8,1>UD    g5<8,4,2>UD     { align1 1Q };
cmp.l.f0.0(8)   g89<1>UD        g88<8,8,1>UD    g9<8,4,2>UD     { align1 2Q };
cmp.l.f0.0(8)   g56<1>UD        g55<8,8,1>UD    g3<8,4,2>UD     { align1 1Q };
add(8)          g79<1>D         -g78<8,8,1>D    g11.1<8,4,2>D   { align1 2Q I@4 };
add(8)          g54<1>D         -g49<8,8,1>D    g5.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g90<1>D         -g89<8,8,1>D    g9.1<8,4,2>D    { align1 2Q I@4 };
add(8)          g57<1>D         -g56<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@4 };
mov(8)          g97.1<2>UD      g79<4,4,1>UD                    { align1 2Q I@4 };
mov(8)          g95.1<2>UD      g54<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g93.1<2>UD      g90<4,4,1>UD                    { align1 2Q I@4 };
mov(8)          g91.1<2>UD      g57<4,4,1>UD                    { align1 1Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g80UD           g95UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $8 };
mov(8)          g84<2>UD        g80<4,4,1>UD                    { align1 1Q $8.dst };
mov(8)          g86<2>UD        g81<4,4,1>UD                    { align1 2Q $8.dst };
mov(8)          g84.1<2>UD      g82<4,4,1>UD                    { align1 1Q @2 $8.dst };
mov(8)          g86.1<2>UD      g83<4,4,1>UD                    { align1 2Q @2 $8.dst };
fbl(1)          g24<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g24<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000a00UD    { align1 WE_all 1N A@1 };
mov(1)          g100<2>D        g[a0 128]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g100.1<2>D      g[a0 132]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
mov(8)          g96.1<2>UD      g100.1<0,1,0>UD                 { align1 WE_all 1Q I@1 };
mov(8)          g96<2>UD        g100<0,1,0>UD                   { align1 WE_all 1Q I@1 };
mov(1)          g32<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g101UD   g96UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $9 };
mov(1)          f0<1>UD         g32<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g24<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000a00UD    { align1 WE_all 1N A@1 };
mov(1)          g103<2>D        g[a0 352]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g103.1<2>D      g[a0 356]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
mov(8)          g97.1<2>UD      g103.1<0,1,0>UD                 { align1 WE_all 1Q I@2 };
mov(8)          g97<2>UD        g103<0,1,0>UD                   { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g97UD           g101UD          0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $10 };
add(16)         g92<1>D         g52<1,1,0>D     128D            { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
shr(16)         g94<1>UD        g92<1,1,0>UD    0x00000006UD    { align1 1H I@1 compacted };
add(8)          g61<1>D         g3<8,4,2>D      g92<1,1,0>D     { align1 1Q compacted };
add(8)          g108<1>D        g9<8,4,2>D      g93<1,1,0>D     { align1 2Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
shl(16)         g96<1>D         g94<8,8,1>D     0x00000003UD    { align1 1H I@3 };
mov(8)          g111<2>UD       g61<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g113<2>UD       g108<4,4,1>UD                   { align1 2Q I@3 };
add(8)          g58<1>D         g5<8,4,2>D      g96<1,1,0>D     { align1 1Q I@3 compacted };
add(8)          g97<1>D         g11<8,4,2>D     g97<1,1,0>D     { align1 2Q I@4 compacted };
mov(8)          g115<2>UD       g58<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g117<2>UD       g97<4,4,1>UD                    { align1 2Q I@2 };
cmp.l.f0.0(8)   g98<1>UD        g97<8,8,1>UD    g11<8,4,2>UD    { align1 2Q };
cmp.l.f0.0(8)   g59<1>UD        g58<8,8,1>UD    g5<8,4,2>UD     { align1 1Q };
cmp.l.f0.0(8)   g109<1>UD       g108<8,8,1>UD   g9<8,4,2>UD     { align1 2Q };
cmp.l.f0.0(8)   g62<1>UD        g61<8,8,1>UD    g3<8,4,2>UD     { align1 1Q };
add(8)          g99<1>D         -g98<8,8,1>D    g11.1<8,4,2>D   { align1 2Q I@4 };
add(8)          g60<1>D         -g59<8,8,1>D    g5.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g110<1>D        -g109<8,8,1>D   g9.1<8,4,2>D    { align1 2Q I@4 };
add(8)          g63<1>D         -g62<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@4 };
mov(8)          g117.1<2>UD     g99<4,4,1>UD                    { align1 2Q I@4 };
mov(8)          g115.1<2>UD     g60<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g113.1<2>UD     g110<4,4,1>UD                   { align1 2Q I@4 };
mov(8)          g111.1<2>UD     g63<4,4,1>UD                    { align1 1Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g100UD          g115UD          nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $11 };
mov(8)          g104<2>UD       g100<4,4,1>UD                   { align1 1Q $11.dst };
mov(8)          g106<2>UD       g101<4,4,1>UD                   { align1 2Q $11.dst };
mov(8)          g104.1<2>UD     g102<4,4,1>UD                   { align1 1Q @2 $11.dst };
mov(8)          g106.1<2>UD     g103<4,4,1>UD                   { align1 2Q @2 $11.dst };
fbl(1)          g25<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g25<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000c00UD    { align1 WE_all 1N A@1 };
mov(1)          g120<2>D        g[a0 256]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g120.1<2>D      g[a0 260]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mov(8)          g116.1<2>UD     g120.1<0,1,0>UD                 { align1 WE_all 1Q I@1 };
mov(8)          g116<2>UD       g120<0,1,0>UD                   { align1 WE_all 1Q I@1 };
mov(1)          g31<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g121UD   g116UD          nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $12 };
mov(1)          f0<1>UD         g31<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g25<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000c00UD    { align1 WE_all 1N A@1 };
mov(1)          g123<2>D        g[a0 480]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g123.1<2>D      g[a0 484]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mov(8)          g117.1<2>UD     g123.1<0,1,0>UD                 { align1 WE_all 1Q I@2 };
mov(8)          g117<2>UD       g123<0,1,0>UD                   { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g117UD          g121UD          0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $13 };
add(16)         g112<1>D        g52<1,1,0>D     192D            { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
shr(16)         g114<1>UD       g112<1,1,0>UD   0x00000006UD    { align1 1H I@1 compacted };
add(8)          g67<1>D         g3<8,4,2>D      g112<1,1,0>D    { align1 1Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.dst };
add(8)          g1<1>D          g9<8,4,2>D      g113<1,1,0>D    { align1 2Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
shl(16)         g116<1>D        g114<8,8,1>D    0x00000003UD    { align1 1H I@3 };
mov(8)          g17<2>UD        g67<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g19<2>UD        g1<4,4,1>UD                     { align1 2Q I@3 };
add(8)          g64<1>D         g5<8,4,2>D      g116<1,1,0>D    { align1 1Q I@3 compacted };
add(8)          g117<1>D        g11<8,4,2>D     g117<1,1,0>D    { align1 2Q I@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
mov(8)          g21<2>UD        g64<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g23<2>UD        g117<4,4,1>UD                   { align1 2Q I@2 };
cmp.l.f0.0(8)   g118<1>UD       g117<8,8,1>UD   g11<8,4,2>UD    { align1 2Q };
cmp.l.f0.0(8)   g65<1>UD        g64<8,8,1>UD    g5<8,4,2>UD     { align1 1Q };
cmp.l.f0.0(8)   g2<1>UD         g1<8,8,1>UD     g9<8,4,2>UD     { align1 2Q F@1 };
cmp.l.f0.0(8)   g68<1>UD        g67<8,8,1>UD    g3<8,4,2>UD     { align1 1Q };
add(8)          g119<1>D        -g118<8,8,1>D   g11.1<8,4,2>D   { align1 2Q I@4 };
add(8)          g66<1>D         -g65<8,8,1>D    g5.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g13<1>D         -g2<8,8,1>D     g9.1<8,4,2>D    { align1 2Q I@4 };
add(8)          g69<1>D         -g68<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@4 };
mov(8)          g23.1<2>UD      g119<4,4,1>UD                   { align1 2Q I@4 };
mov(8)          g21.1<2>UD      g66<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g19.1<2>UD      g13<4,4,1>UD                    { align1 2Q I@4 };
mov(8)          g17.1<2>UD      g69<4,4,1>UD                    { align1 1Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g120UD          g21UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $14 };
mov(8)          g124<2>UD       g120<4,4,1>UD                   { align1 1Q $14.dst };
mov(8)          g126<2>UD       g121<4,4,1>UD                   { align1 2Q $14.dst };
mov(8)          g124.1<2>UD     g122<4,4,1>UD                   { align1 1Q @2 $14.dst };
mov(8)          g126.1<2>UD     g123<4,4,1>UD                   { align1 2Q @2 $14.dst };
fbl(1)          g26<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
shl(1)          a0<1>UD         g26<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000e00UD    { align1 WE_all 1N A@1 };
mov(1)          g31<2>D         g[a0 384]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g31.1<2>D       g[a0 388]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
mov(8)          g22.1<2>UD      g31.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g22<2>UD        g31<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g30<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g28UD    g22UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $15 };
mov(1)          f0<1>UD         g30<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g26<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000200UD    { align1 WE_all 1N A@1 };
mov(1)          g32<2>D         g[a0 32]<0,1,0>D                { align1 WE_all 1N A@1 };
mov(1)          g32.1<2>D       g[a0 36]<0,1,0>D                { align1 WE_all 1N };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
mov(8)          g23.1<2>UD      g32.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g23<2>UD        g32<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g23UD           g28UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $4 };
add3(16)        g7<1>D          0x0003UW        g15<8,8,1>D     g7<1,1,1>D { align1 1H };

LABEL2:
while(16)       JIP:  LABEL3                                    { align1 1H };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_misc_copy_instance_ptrs_code[] = {
    0x80000065, 0x21058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x15050220, 0x00000024, 0x00000000,
    0xe2221a40, 0x00012103, 0x00041a61, 0x07050220,
    0x00461505, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa00220c, 0x00340000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x21030061, 0x001102cc,
    0x2a090061, 0x001102cc, 0x00030061, 0x05260aa0,
    0x00000264, 0x00000000, 0x00130061, 0x0b260aa0,
    0x00000264, 0x00000000, 0x00030061, 0x23260aa0,
    0x000002a4, 0x00000000, 0x00130061, 0x0d260aa0,
    0x000002a4, 0x00000000, 0x21031661, 0x00110204,
    0x2a091661, 0x00110204, 0x21051661, 0x00110244,
    0x2a0b1661, 0x00110244, 0x21231661, 0x0011025c,
    0x2a0d1661, 0x0011025c, 0xe10f1268, 0x008e2303,
    0x00030065, 0x11058220, 0x02442306, 0xffffff00,
    0xea101168, 0x008e0d03, 0x00130065, 0x12058220,
    0x02440d06, 0xffffff00, 0xa1131b40, 0x112e2302,
    0x00041b52, 0x19044560, 0x0e8efffe, 0x15050f05,
    0xaa141b40, 0x122e0d02, 0x00041a61, 0x1b052660,
    0x00461905, 0x00000000, 0x00041a65, 0x15058220,
    0x02461305, 0xffffffc0, 0xe0171968, 0x00601503,
    0x00041970, 0x00010220, 0x52461b05, 0x00461705,
    0x01040022, 0x0001c060, 0x00000350, 0x00000350,
    0x00040069, 0x1c05a660, 0x02461905, 0x00000006,
    0xa01e1940, 0x1c001102, 0xe0201968, 0x00601e03,
    0xa1271640, 0x1e0e0302, 0xaa2e1540, 0x1f0e0902,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00041b69, 0x22058660, 0x02462005, 0x00000003,
    0x00031b61, 0x31060220, 0x00342705, 0x00000000,
    0x00131b61, 0x33060220, 0x00342e05, 0x00000000,
    0xa1240b40, 0x220e0502, 0xaa230b40, 0x230e0b02,
    0x00031a70, 0x25050220, 0x52462405, 0x00440506,
    0x00030061, 0x35060220, 0x00342405, 0x00000000,
    0x00030070, 0x28050220, 0x52462705, 0x00440306,
    0x00131c61, 0x37060220, 0x00342305, 0x00000000,
    0x00130070, 0x24050220, 0x52462305, 0x00440b06,
    0x00130070, 0x2f050220, 0x52462e05, 0x00440906,
    0x00031e40, 0x26052660, 0x06462505, 0x00440526,
    0x00031d40, 0x29052660, 0x06462805, 0x00440326,
    0x00131c40, 0x25052660, 0x06462405, 0x00440b26,
    0x00131c40, 0x30052660, 0x06462f05, 0x00440926,
    0x00031c61, 0x35260220, 0x00342605, 0x00000000,
    0x00031c61, 0x31260220, 0x00342905, 0x00000000,
    0x00131c61, 0x37260220, 0x00342505, 0x00000000,
    0x00131c61, 0x33260220, 0x00343005, 0x00000000,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x26240000, 0xfb043524, 0x000c0000,
    0x00032161, 0x2a060220, 0x00342605, 0x00000000,
    0x00132161, 0x2c060220, 0x00342705, 0x00000000,
    0x0003a161, 0x2a260220, 0x00342805, 0x00000000,
    0x0013a161, 0x2c260220, 0x00342905, 0x00000000,
    0xe216004c, 0x00114004, 0x80001969, 0x10018220,
    0x02001604, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000400, 0x80000961, 0x46060660,
    0x00010500, 0x00000000, 0x80000061, 0x46260660,
    0x00010510, 0x00000000, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x36260220,
    0x00004624, 0x00000000, 0x80031961, 0x36060220,
    0x00004604, 0x00000000, 0xe2220061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004231, 0x3b140000, 0xfb00360c, 0x00340000,
    0x80001a61, 0x30010220, 0x00002204, 0x00000000,
    0x80001e69, 0x10018220, 0x02001604, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000600,
    0x80000961, 0x47060660, 0x00010080, 0x00000000,
    0x80000061, 0x47260660, 0x00010090, 0x00000000,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x80031a61, 0x37260220, 0x00004724, 0x00000000,
    0x80031961, 0x37060220, 0x00004704, 0x00000000,
    0x80002201, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004331, 0x00000000, 0xfb08370c, 0x00343b14,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0xe0320068, 0x00801103, 0x00041970, 0x00010220,
    0x42460705, 0x00463205, 0x01040028, 0x0001c660,
    0x00000e88, 0x00000e88, 0x00043169, 0x34058660,
    0x02460705, 0x00000008, 0x80001f01, 0x00000000,
    0x00000000, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0xe0361968, 0x00603403,
    0xa12d1640, 0x340e0302, 0xaa441540, 0x350e0902,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00040069, 0x38058660, 0x02463605, 0x00000003,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x47060220, 0x00342d05, 0x00000000,
    0x80101b01, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x49060220, 0x00344405, 0x00000000,
    0xa12a0b40, 0x380e0502, 0xaa390b40, 0x390e0b02,
    0x00031a70, 0x2b050220, 0x52462a05, 0x00440506,
    0x00030061, 0x4b060220, 0x00342a05, 0x00000000,
    0x00030070, 0x2e050220, 0x52462d05, 0x00440306,
    0x00131c70, 0x3a050220, 0x52463905, 0x00440b06,
    0x00130061, 0x4d060220, 0x00343905, 0x00000000,
    0x00130070, 0x45050220, 0x52464405, 0x00440906,
    0x00031e40, 0x2c052660, 0x06462b05, 0x00440526,
    0x00031d40, 0x2f052660, 0x06462e05, 0x00440326,
    0x80103301, 0x00000000, 0x00000000, 0x00000000,
    0x00131d40, 0x3b052660, 0x06463a05, 0x00440b26,
    0x00131c40, 0x46052660, 0x06464505, 0x00440926,
    0x00031c61, 0x4b260220, 0x00342c05, 0x00000000,
    0x00031c61, 0x47260220, 0x00342f05, 0x00000000,
    0x00131c61, 0x4d260220, 0x00343b05, 0x00000000,
    0x00131c61, 0x49260220, 0x00344605, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00044531, 0x3c240000, 0xfb044b24, 0x000c0000,
    0x00032561, 0x40060220, 0x00343c05, 0x00000000,
    0x00132561, 0x42060220, 0x00343d05, 0x00000000,
    0x0003a561, 0x40260220, 0x00343e05, 0x00000000,
    0x0013a561, 0x42260220, 0x00343f05, 0x00000000,
    0xe217344c, 0x00114004, 0x80001969, 0x10018220,
    0x02001704, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000800, 0x80000961, 0x50060660,
    0x00010000, 0x00000000, 0x80000061, 0x50260660,
    0x00010010, 0x00000000, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x4c260220,
    0x00005024, 0x00000000, 0x80031961, 0x4c060220,
    0x00005004, 0x00000000, 0xe2210061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004631, 0x51140000, 0xfb004c0c, 0x00340000,
    0x80001a61, 0x30010220, 0x00002104, 0x00000000,
    0x80001e69, 0x10018220, 0x02001704, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000800,
    0x80000961, 0x53060660, 0x00010380, 0x00000000,
    0x80000061, 0x53260660, 0x00010390, 0x00000000,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x80031a61, 0x4d260220, 0x00005324, 0x00000000,
    0x80031961, 0x4d060220, 0x00005304, 0x00000000,
    0x80002601, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004731, 0x00000000, 0xfb084d0c, 0x00345114,
    0xa0480040, 0x04003403, 0x80001d01, 0x00000000,
    0x00000000, 0x00000000, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0xe04a1968, 0x00604803,
    0xa1370040, 0x480e0302, 0xaa580040, 0x490e0902,
    0x80003601, 0x00000000, 0x00000000, 0x00000000,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x00041b69, 0x4c058660, 0x02464a05, 0x00000003,
    0x00031b61, 0x5b060220, 0x00343705, 0x00000000,
    0x00131b61, 0x5d060220, 0x00345805, 0x00000000,
    0xa1301b40, 0x4c0e0502, 0xaa4d1c40, 0x4d0e0b02,
    0x00031a61, 0x5f060220, 0x00343005, 0x00000000,
    0x00131a61, 0x61060220, 0x00344d05, 0x00000000,
    0x00130070, 0x4e050220, 0x52464d05, 0x00440b06,
    0x00030070, 0x31050220, 0x52463005, 0x00440506,
    0x00130070, 0x59050220, 0x52465805, 0x00440906,
    0x00030070, 0x38050220, 0x52463705, 0x00440306,
    0x00131c40, 0x4f052660, 0x06464e05, 0x00440b26,
    0x00031c40, 0x36052660, 0x06463105, 0x00440526,
    0x00131c40, 0x5a052660, 0x06465905, 0x00440926,
    0x00031c40, 0x39052660, 0x06463805, 0x00440326,
    0x00131c61, 0x61260220, 0x00344f05, 0x00000000,
    0x00031c61, 0x5f260220, 0x00343605, 0x00000000,
    0x00131c61, 0x5d260220, 0x00345a05, 0x00000000,
    0x00031c61, 0x5b260220, 0x00343905, 0x00000000,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00044831, 0x50240000, 0xfb045f24, 0x000c0000,
    0x00032861, 0x54060220, 0x00345005, 0x00000000,
    0x00132861, 0x56060220, 0x00345105, 0x00000000,
    0x0003a861, 0x54260220, 0x00345205, 0x00000000,
    0x0013a861, 0x56260220, 0x00345305, 0x00000000,
    0xe218004c, 0x00114004, 0x80001969, 0x10018220,
    0x02001804, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000a00, 0x80000961, 0x64060660,
    0x00010200, 0x00000000, 0x80000061, 0x64260660,
    0x00010210, 0x00000000, 0x80003801, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x60260220,
    0x00006424, 0x00000000, 0x80031961, 0x60060220,
    0x00006404, 0x00000000, 0xe2200061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004931, 0x65140000, 0xfb00600c, 0x00340000,
    0x80001a61, 0x30010220, 0x00002004, 0x00000000,
    0x80001e69, 0x10018220, 0x02001804, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000a00,
    0x80000961, 0x67060660, 0x00010580, 0x00000000,
    0x80000061, 0x67260660, 0x00010590, 0x00000000,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0x80031a61, 0x61260220, 0x00006724, 0x00000000,
    0x80031961, 0x61060220, 0x00006704, 0x00000000,
    0x80002901, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004a31, 0x00000000, 0xfb08610c, 0x00346514,
    0xa05c0040, 0x08003403, 0x80001d01, 0x00000000,
    0x00000000, 0x00000000, 0x80003801, 0x00000000,
    0x00000000, 0x00000000, 0xe05e1968, 0x00605c03,
    0xa13d0040, 0x5c0e0302, 0xaa6c0040, 0x5d0e0902,
    0x80003901, 0x00000000, 0x00000000, 0x00000000,
    0x80003a01, 0x00000000, 0x00000000, 0x00000000,
    0x00041b69, 0x60058660, 0x02465e05, 0x00000003,
    0x00031b61, 0x6f060220, 0x00343d05, 0x00000000,
    0x00131b61, 0x71060220, 0x00346c05, 0x00000000,
    0xa13a1b40, 0x600e0502, 0xaa611c40, 0x610e0b02,
    0x00031a61, 0x73060220, 0x00343a05, 0x00000000,
    0x00131a61, 0x75060220, 0x00346105, 0x00000000,
    0x00130070, 0x62050220, 0x52466105, 0x00440b06,
    0x00030070, 0x3b050220, 0x52463a05, 0x00440506,
    0x00130070, 0x6d050220, 0x52466c05, 0x00440906,
    0x00030070, 0x3e050220, 0x52463d05, 0x00440306,
    0x00131c40, 0x63052660, 0x06466205, 0x00440b26,
    0x00031c40, 0x3c052660, 0x06463b05, 0x00440526,
    0x00131c40, 0x6e052660, 0x06466d05, 0x00440926,
    0x00031c40, 0x3f052660, 0x06463e05, 0x00440326,
    0x00131c61, 0x75260220, 0x00346305, 0x00000000,
    0x00031c61, 0x73260220, 0x00343c05, 0x00000000,
    0x00131c61, 0x71260220, 0x00346e05, 0x00000000,
    0x00031c61, 0x6f260220, 0x00343f05, 0x00000000,
    0x80003a01, 0x00000000, 0x00000000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00044b31, 0x64240000, 0xfb047324, 0x000c0000,
    0x00032b61, 0x68060220, 0x00346405, 0x00000000,
    0x00132b61, 0x6a060220, 0x00346505, 0x00000000,
    0x0003ab61, 0x68260220, 0x00346605, 0x00000000,
    0x0013ab61, 0x6a260220, 0x00346705, 0x00000000,
    0xe219004c, 0x00114004, 0x80001969, 0x10018220,
    0x02001904, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000c00, 0x80000961, 0x78060660,
    0x00010400, 0x00000000, 0x80000061, 0x78260660,
    0x00010410, 0x00000000, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x74260220,
    0x00007824, 0x00000000, 0x80031961, 0x74060220,
    0x00007804, 0x00000000, 0xe21f0061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004c31, 0x79140000, 0xfb00740c, 0x00340000,
    0x80001a61, 0x30010220, 0x00001f04, 0x00000000,
    0x80001e69, 0x10018220, 0x02001904, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000c00,
    0x80000961, 0x7b060660, 0x00010780, 0x00000000,
    0x80000061, 0x7b260660, 0x00010790, 0x00000000,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0x80031a61, 0x75260220, 0x00007b24, 0x00000000,
    0x80031961, 0x75060220, 0x00007b04, 0x00000000,
    0x80002c01, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004d31, 0x00000000, 0xfb08750c, 0x00347914,
    0xa0700040, 0x0c003403, 0x80003f01, 0x00000000,
    0x00000000, 0x00000000, 0x80001d01, 0x00000000,
    0x00000000, 0x00000000, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0xe0721968, 0x00607003,
    0xa1430040, 0x700e0302, 0x80102001, 0x00000000,
    0x00000000, 0x00000000, 0xaa010040, 0x710e0902,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x00041b69, 0x74058660, 0x02467205, 0x00000003,
    0x00031b61, 0x11060220, 0x00344305, 0x00000000,
    0x00131b61, 0x13060220, 0x00340105, 0x00000000,
    0xa1401b40, 0x740e0502, 0xaa751c40, 0x750e0b02,
    0x80003e01, 0x00000000, 0x00000000, 0x00000000,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x00031a61, 0x15060220, 0x00344005, 0x00000000,
    0x00131a61, 0x17060220, 0x00347505, 0x00000000,
    0x00130070, 0x76050220, 0x52467505, 0x00440b06,
    0x00030070, 0x41050220, 0x52464005, 0x00440506,
    0x00131170, 0x02050220, 0x52460105, 0x00440906,
    0x00030070, 0x44050220, 0x52464305, 0x00440306,
    0x00131c40, 0x77052660, 0x06467605, 0x00440b26,
    0x00031c40, 0x42052660, 0x06464105, 0x00440526,
    0x00131c40, 0x0d052660, 0x06460205, 0x00440926,
    0x00031c40, 0x45052660, 0x06464405, 0x00440326,
    0x00131c61, 0x17260220, 0x00347705, 0x00000000,
    0x00031c61, 0x15260220, 0x00344205, 0x00000000,
    0x00131c61, 0x13260220, 0x00340d05, 0x00000000,
    0x00031c61, 0x11260220, 0x00344505, 0x00000000,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00044e31, 0x78240000, 0xfb041524, 0x000c0000,
    0x00032e61, 0x7c060220, 0x00347805, 0x00000000,
    0x00132e61, 0x7e060220, 0x00347905, 0x00000000,
    0x0003ae61, 0x7c260220, 0x00347a05, 0x00000000,
    0x0013ae61, 0x7e260220, 0x00347b05, 0x00000000,
    0xe21a004c, 0x00114004, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x80001969, 0x10018220,
    0x02001a04, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000e00, 0x80000961, 0x1f060660,
    0x00010600, 0x00000000, 0x80000061, 0x1f260660,
    0x00010610, 0x00000000, 0x80003e01, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x16260220,
    0x00001f24, 0x00000000, 0x80031961, 0x16060220,
    0x00001f04, 0x00000000, 0xe21e0061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004f31, 0x1c140000, 0xfb00160c, 0x00340000,
    0x80001a61, 0x30010220, 0x00001e04, 0x00000000,
    0x80001e69, 0x10018220, 0x02001a04, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000200,
    0x80000961, 0x20060660, 0x00010080, 0x00000000,
    0x80000061, 0x20260660, 0x00010090, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003e01, 0x00000000, 0x00000000, 0x00000000,
    0x80003e01, 0x00000000, 0x00000000, 0x00000000,
    0x80031a61, 0x17260220, 0x00002024, 0x00000000,
    0x80031961, 0x17060220, 0x00002004, 0x00000000,
    0x80002f01, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004431, 0x00000000, 0xfb08170c, 0x00341c14,
    0x00040052, 0x07044160, 0x0e0e0003, 0x07050f05,
    0x00040027, 0x00014060, 0x00000000, 0xfffff168,
    0x80030061, 0x7e050220, 0x00460005, 0x00000000,
    0x80040931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_misc_copy_instance_ptrs = {
   .prog_data = {
      .base.nr_params = 14,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 5024,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_misc_copy_instance_ptrs_relocs,
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
      .push.cross_thread.dwords = 14,
      .push.cross_thread.regs = 2,
      .push.cross_thread.size = 64,
   },
   .args_size = 24,
   .arg_count = 3,
   .args = gfx125_misc_copy_instance_ptrs_args,
   .code = gfx125_misc_copy_instance_ptrs_code,
};
const char *gfx125_misc_copy_instance_ptrs_sha1 = "2b766c6d4673b3a312f602d88beede6376785e86";
