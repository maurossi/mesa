#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_misc_copy_instances_indirect_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_misc_copy_instances_indirect_args[] = {
   { 0, 8 },
   { 8, 8 },
   { 16, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g48<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g56<1>UD        g0.1<0,1,0>UD                   { align1 1H };
add(1)          g49<1>UD        g48<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@2 compacted };
mov(8)          g7<1>UW         0x76543210V                     { align1 WE_all 1Q };
shl(16)         g29<1>D         g56<8,8,1>D     0x00000004UD    { align1 1H I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(1)         g1UD            g49UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
add(8)          g7.8<1>UW       g7<1,1,0>UW     0x0008UW        { align1 WE_all 1Q I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         g27<1>D         g7<8,8,1>UW                     { align1 1H };
add.z.f0.0(16)  null<1>D        g27<8,8,1>D     g29<8,8,1>D     { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g34.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g18.1<2>F       g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g5.1<2>F        g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g20.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g62.1<2>F       g2.5<0,1,0>F                    { align1 1Q };
mov(8)          g64.1<2>F       g2.5<0,1,0>F                    { align1 2Q };
mov(8)          g34<2>F         g2<0,1,0>F                      { align1 1Q F@6 compacted };
mov(8)          g18<2>F         g2<0,1,0>F                      { align1 2Q F@6 compacted };
mov(8)          g5<2>F          g2.2<0,1,0>F                    { align1 1Q F@6 compacted };
mov(8)          g20<2>F         g2.2<0,1,0>F                    { align1 2Q F@6 compacted };
mov(8)          g62<2>F         g2.4<0,1,0>F                    { align1 1Q F@6 compacted };
mov(8)          g64<2>F         g2.4<0,1,0>F                    { align1 2Q F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
send(16)        g22UD           g62UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
shl(16)         g26<1>D         g22<8,8,1>D     0x00000006UD    { align1 1H $1.dst };
(+f0.0) if(16)  JIP:  LABEL0          UIP:  LABEL0              { align1 1H };
add(8)          g30<1>D         g34<8,4,2>D     156D            { align1 1Q F@6 compacted };
add(8)          g31<1>D         g18<8,4,2>D     156D            { align1 2Q F@5 compacted };
cmp.l.f0.0(16)  g32<1>UD        g30<1,1,0>UD    0x0000009cUD    { align1 1H I@1 compacted };
mov(8)          g63<2>UD        g30<4,4,1>UD                    { align1 1Q $1.src };
mov(8)          g65<2>UD        g31<4,4,1>UD                    { align1 2Q $1.src };
add(8)          g118<1>D        -g32<8,8,1>D    g34.1<8,4,2>D   { align1 1Q I@3 };
add(8)          g33<1>D         -g33<8,8,1>D    g18.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g63.1<2>UD      g118<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g65.1<2>UD      g33<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g63UD           g22UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $1 };

LABEL0:
endif(16)       JIP:  LABEL1                                    { align1 1H };

LABEL1:
shr(16)         g3<1>UD         g26<1,1,0>UD    0x00000008UD    { align1 1H I@3 compacted };
mov(16)         g16<1>UD        g56<8,8,1>UD                    { align1 1H };
and(16)         g36<1>UD        g26<8,8,1>UD    0xffffff00UD    { align1 1H };
add3(16)        g44<1>D         65534W          -g3<8,8,1>D     g56<1,1,1>D { align1 1H I@3 };
add(16)         g38<1>D         g26<1,1,0>D     -g36<1,1,0>D    { align1 1H I@2 compacted };
mov(16)         g46<1>D         -g44<8,8,1>D                    { align1 1H I@2 };
and(16)         g40<1>UD        g38<8,8,1>UD    0xffffffc0UD    { align1 1H I@2 };
shr(16)         g42<1>UD        g40<1,1,0>UD    0x00000006UD    { align1 1H I@1 compacted };
cmp.l.f0.0(16)  null<1>UD       g46<8,8,1>UD    g42<8,8,1>UD    { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL2          UIP:  LABEL2              { align1 1H };
shl(16)         g47<1>D         -g44<8,8,1>D    0x00000006UD    { align1 1H };
mov(8)          g51<1>UD        g5.1<8,4,2>UD                   { align1 1Q F@4 };
mov(8)          g52<1>UD        g20.1<8,4,2>UD                  { align1 2Q F@3 };
add(8)          g53<1>D         g5<8,4,2>D      g24<1,1,0>D     { align1 1Q $1.dst compacted };
add(8)          g54<1>D         g20<8,4,2>D     g25<1,1,0>D     { align1 2Q $1.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(16)         g49<1>D         g36<1,1,0>D     g47<1,1,0>D     { align1 1H I@5 compacted };
cmp.l.f0.0(8)   g119<1>UD       g53<8,8,1>UD    g5<8,4,2>UD     { align1 1Q I@3 };
cmp.l.f0.0(8)   g55<1>UD        g54<8,8,1>UD    g20<8,4,2>UD    { align1 2Q I@3 };
add(8)          g56<1>D         -g119<8,8,1>D   g5.1<8,4,2>D    { align1 1Q I@2 };
add(8)          g57<1>D         -g55<8,8,1>D    g20.1<8,4,2>D   { align1 2Q I@2 };
or.z.f0.0(16)   null<1>UD       g53<8,8,1>UD    g56<8,8,1>UD    { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL4          UIP:  LABEL3              { align1 1H };
shr(16)         g57<1>UD        g49<1,1,0>UD    0x00000006UD    { align1 1H I@7 compacted };
shl(16)         g59<1>D         g57<8,8,1>D     0x00000003UD    { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(8)          g64<2>UD        g59<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $1.src };
mov(8)          g66<2>UD        g60<4,4,1>UD                    { align1 2Q I@2 };
mov(8)          g64.1<2>UD      0x00000000UD                    { align1 1Q I@2 };
mov(8)          g66.1<2>UD      0x00000000UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g60UD           g64UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
mov(8)          g8<2>UD         g60<4,4,1>UD                    { align1 1Q $1.dst };
mov(8)          g10<2>UD        g61<4,4,1>UD                    { align1 2Q $1.dst };
mov(8)          g8.1<2>UD       g62<4,4,1>UD                    { align1 1Q @2 $1.dst };
mov(8)          g10.1<2>UD      g63<4,4,1>UD                    { align1 2Q @2 $1.dst };
else(16)        JIP:  LABEL3          UIP:  LABEL3              { align1 1H };

LABEL4:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
add(16)         g61<1>D         g24<1,1,0>D     g49<1,1,0>D     { align1 1H I@7 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
cmp.l.f0.0(16)  g63<1>UD        g61<1,1,0>UD    g24<1,1,0>UD    { align1 1H I@1 compacted };
add(8)          g120<1>D        g5<8,4,2>D      g61<1,1,0>D     { align1 1Q compacted };
add(8)          g65<1>D         g20<8,4,2>D     g62<1,1,0>D     { align1 2Q $1.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
cmp.l.f0.0(8)   g66<1>UD        g120<8,8,1>UD   g5<8,4,2>UD     { align1 1Q I@2 };
mov(8)          g8<2>UD         g120<4,4,1>UD                   { align1 1Q I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $1.src };
cmp.l.f0.0(8)   g67<1>UD        g65<8,8,1>UD    g20<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g10<2>UD        g65<4,4,1>UD                    { align1 2Q I@7 };
add3(16)        g68<1>D         g51<8,8,1>D     -g63<8,8,1>D    -g66<1,1,1>D { align1 1H I@2 };
mov(8)          g8.1<2>UD       g68<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g10.1<2>UD      g69<4,4,1>UD                    { align1 2Q I@2 };

LABEL3:
endif(16)       JIP:  LABEL2                                    { align1 1H };
add(8)          g121<1>D        g34<8,4,2>D     g49<1,1,0>D     { align1 1Q F@6 compacted };
add(8)          g69<1>D         g18<8,4,2>D     g50<1,1,0>D     { align1 2Q F@5 compacted };
cmp.l.f0.0(8)   g122<1>UD       g121<8,8,1>UD   g34<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g70<1>UD        g69<8,8,1>UD    g18<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g72<2>UD        g121<4,4,1>UD                   { align1 1Q };
mov(8)          g74<2>UD        g69<4,4,1>UD                    { align1 2Q };
add(8)          g123<1>D        -g122<8,8,1>D   g34.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g71<1>D         -g70<8,8,1>D    g18.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g72.1<2>UD      g123<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g74.1<2>UD      g71<4,4,1>UD                    { align1 2Q I@2 };
fbl(1)          g57<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g57<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g38<2>D         g[a0 256]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g38.1<2>D       g[a0 260]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(8)          g65.1<2>UD      g38.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g65<2>UD        g38<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g117<1>UD       f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g70UD    g65UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $1 };
mov(1)          f0<1>UD         g117<0,1,0>UD                   { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g57<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000800UD    { align1 WE_all 1N A@1 };
mov(1)          g39<2>D         g[a0 256]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g39.1<2>D       g[a0 260]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(8)          g66.1<2>UD      g39.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g66<2>UD        g39<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g66UD           g70UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $1 };

LABEL2:
endif(16)       JIP:  LABEL5                                    { align1 1H };

LABEL5:
shr(16)         g73<1>UD        g36<1,1,0>UD    0x00000008UD    { align1 1H compacted };
mov(8)          g75<1>UD        g5.1<8,4,2>UD                   { align1 1Q F@4 };
mov(8)          g76<1>UD        g20.1<8,4,2>UD                  { align1 2Q F@3 };
add(8)          g77<1>D         g5<8,4,2>D      g24<1,1,0>D     { align1 1Q $1.dst compacted };
add(8)          g78<1>D         g20<8,4,2>D     g25<1,1,0>D     { align1 2Q $1.dst compacted };
cmp.l.f0.0(8)   g124<1>UD       g77<8,8,1>UD    g5<8,4,2>UD     { align1 1Q I@2 };
cmp.l.f0.0(8)   g79<1>UD        g78<8,8,1>UD    g20<8,4,2>UD    { align1 2Q I@2 };
add(8)          g80<1>D         -g124<8,8,1>D   g5.1<8,4,2>D    { align1 1Q I@2 };
add(8)          g81<1>D         -g79<8,8,1>D    g20.1<8,4,2>D   { align1 2Q I@2 };
or(16)          g82<1>UD        g77<1,1,0>UD    g80<1,1,0>UD    { align1 1H I@1 compacted };

LABEL15:
cmp.ge.f0.0(16) null<1>UD       g16<8,8,1>UD    g73<8,8,1>UD    { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL6        UIP:  LABEL6              { align1 1H };
shl(16)         g84<1>D         g16<8,8,1>D     0x00000008UD    { align1 1H };
cmp.z.f0.0(16)  null<1>D        g82<8,8,1>D     0D              { align1 1H I@4 };
(+f0.0) if(16)  JIP:  LABEL8          UIP:  LABEL7              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
shr(16)         g86<1>UD        g84<1,1,0>UD    0x00000006UD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
shl(16)         g88<1>D         g86<8,8,1>D     0x00000003UD    { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(8)          g77<2>UD        g88<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@2 };
mov(8)          g79<2>UD        g89<4,4,1>UD                    { align1 2Q $3.src };
mov(8)          g77.1<2>UD      0x00000000UD                    { align1 1Q I@2 };
mov(8)          g79.1<2>UD      0x00000000UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g89UD           g77UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
mov(8)          g9<2>UD         g89<4,4,1>UD                    { align1 1Q $3.dst };
mov(8)          g11<2>UD        g90<4,4,1>UD                    { align1 2Q $3.dst };
mov(8)          g9.1<2>UD       g91<4,4,1>UD                    { align1 1Q @2 $3.dst };
mov(8)          g11.1<2>UD      g92<4,4,1>UD                    { align1 2Q @2 $3.dst };
else(16)        JIP:  LABEL7          UIP:  LABEL7              { align1 1H };

LABEL8:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g90<1>D         g24<1,1,0>D     g84<1,1,0>D     { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
cmp.l.f0.0(16)  g92<1>UD        g90<1,1,0>UD    g24<1,1,0>UD    { align1 1H I@1 compacted };
add(8)          g125<1>D        g5<8,4,2>D      g90<1,1,0>D     { align1 1Q compacted };
add(8)          g94<1>D         g20<8,4,2>D     g91<1,1,0>D     { align1 2Q compacted };
cmp.l.f0.0(8)   g95<1>UD        g125<8,8,1>UD   g5<8,4,2>UD     { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
mov(8)          g9<2>UD         g125<4,4,1>UD                   { align1 1Q };
cmp.l.f0.0(8)   g96<1>UD        g94<8,8,1>UD    g20<8,4,2>UD    { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@7 };
mov(8)          g11<2>UD        g94<4,4,1>UD                    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add3(16)        g97<1>D         g75<8,8,1>D     -g92<8,8,1>D    -g95<1,1,1>D { align1 1H I@2 };
mov(8)          g9.1<2>UD       g97<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g11.1<2>UD      g98<4,4,1>UD                    { align1 2Q I@2 };

LABEL7:
endif(16)       JIP:  LABEL6                                    { align1 1H };
add(8)          g126<1>D        g34<8,4,2>D     g84<1,1,0>D     { align1 1Q F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $2.src };
add(8)          g98<1>D         g18<8,4,2>D     g85<1,1,0>D     { align1 2Q F@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
cmp.l.f0.0(8)   g127<1>UD       g126<8,8,1>UD   g34<8,4,2>UD    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $6.src };
cmp.l.f0.0(8)   g99<1>UD        g98<8,8,1>UD    g18<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g101<2>UD       g126<4,4,1>UD                   { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $5.src };
mov(8)          g103<2>UD       g98<4,4,1>UD                    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
add(8)          g1<1>D          -g127<8,8,1>D   g34.1<8,4,2>D   { align1 1Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $5.src };
add(8)          g100<1>D        -g99<8,8,1>D    g18.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g101.1<2>UD     g1<4,4,1>UD                     { align1 1Q I@2 };
mov(8)          g103.1<2>UD     g100<4,4,1>UD                   { align1 2Q I@2 };
fbl(1)          g58<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g58<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g40<2>D         g[a0 288]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g40.1<2>D       g[a0 292]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(8)          g78.1<2>UD      g40.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g78<2>UD        g40<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g116<1>UD       f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g90UD    g78UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $3 };
mov(1)          f0<1>UD         g116<0,1,0>UD                   { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g58<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000c00UD    { align1 WE_all 1N A@1 };
mov(1)          g41<2>D         g[a0 160]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g41.1<2>D       g[a0 164]<0,1,0>D               { align1 WE_all 1N };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g86.1<2>UD      g41.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g86<2>UD        g41<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g86UD           g90UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $2 };
add(16)         g102<1>D        g84<1,1,0>D     64D             { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g82<8,8,1>D     0D              { align1 1H };
(+f0.0) if(16)  JIP:  LABEL10         UIP:  LABEL9              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
shr(16)         g104<1>UD       g102<1,1,0>UD   0x00000006UD    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
shl(16)         g106<1>D        g104<8,8,1>D    0x00000003UD    { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g91<2>UD        g106<4,4,1>UD                   { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@2 };
mov(8)          g93<2>UD        g107<4,4,1>UD                   { align1 2Q };
mov(8)          g91.1<2>UD      0x00000000UD                    { align1 1Q I@2 };
mov(8)          g93.1<2>UD      0x00000000UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g107UD          g91UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
mov(8)          g10<2>UD        g107<4,4,1>UD                   { align1 1Q $2.dst };
mov(8)          g12<2>UD        g108<4,4,1>UD                   { align1 2Q $2.dst };
mov(8)          g10.1<2>UD      g109<4,4,1>UD                   { align1 1Q @2 $2.dst };
mov(8)          g12.1<2>UD      g110<4,4,1>UD                   { align1 2Q @2 $2.dst };
else(16)        JIP:  LABEL9          UIP:  LABEL9              { align1 1H };

LABEL10:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
add(16)         g108<1>D        g24<1,1,0>D     g102<1,1,0>D    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
cmp.l.f0.0(16)  g110<1>UD       g108<1,1,0>UD   g24<1,1,0>UD    { align1 1H I@1 compacted };
add(8)          g2<1>D          g5<8,4,2>D      g108<1,1,0>D    { align1 1Q F@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $5.src };
add(8)          g112<1>D        g20<8,4,2>D     g109<1,1,0>D    { align1 2Q compacted };
cmp.l.f0.0(8)   g113<1>UD       g2<8,8,1>UD     g5<8,4,2>UD     { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
mov(8)          g10<2>UD        g2<4,4,1>UD                     { align1 1Q };
cmp.l.f0.0(8)   g114<1>UD       g112<8,8,1>UD   g20<8,4,2>UD    { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@7 };
mov(8)          g12<2>UD        g112<4,4,1>UD                   { align1 2Q };
add3(16)        g115<1>D        g75<8,8,1>D     -g110<8,8,1>D   -g113<1,1,1>D { align1 1H I@2 };
mov(8)          g10.1<2>UD      g115<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g12.1<2>UD      g116<4,4,1>UD                   { align1 2Q I@2 };

LABEL9:
endif(16)       JIP:  LABEL6                                    { align1 1H };
add(8)          g7<1>D          g34<8,4,2>D     g102<1,1,0>D    { align1 1Q compacted };
add(8)          g116<1>D        g18<8,4,2>D     g103<1,1,0>D    { align1 2Q compacted };
cmp.l.f0.0(8)   g8<1>UD         g7<8,8,1>UD     g34<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g117<1>UD       g116<8,8,1>UD   g18<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g119<2>UD       g7<4,4,1>UD                     { align1 1Q };
mov(8)          g121<2>UD       g116<4,4,1>UD                   { align1 2Q };
add(8)          g9<1>D          -g8<8,8,1>D     g34.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g118<1>D        -g117<8,8,1>D   g18.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g119.1<2>UD     g9<4,4,1>UD                     { align1 1Q I@2 };
mov(8)          g121.1<2>UD     g118<4,4,1>UD                   { align1 2Q I@2 };
fbl(1)          g59<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g59<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g42<2>D         g[a0 320]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g42.1<2>D       g[a0 324]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g92.1<2>UD      g42.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g92<2>UD        g42<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g115<1>UD       f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g97UD    g92UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $4 };
mov(1)          f0<1>UD         g115<0,1,0>UD                   { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g59<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000e00UD    { align1 WE_all 1N A@1 };
mov(1)          g43<2>D         g[a0 224]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g43.1<2>D       g[a0 228]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g93.1<2>UD      g43.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g93<2>UD        g43<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g93UD           g97UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $2 };
add(16)         g120<1>D        g84<1,1,0>D     128D            { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g82<8,8,1>D     0D              { align1 1H };
(+f0.0) if(16)  JIP:  LABEL12         UIP:  LABEL11             { align1 1H };
shr(16)         g122<1>UD       g120<1,1,0>UD   0x00000006UD    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
shl(16)         g124<1>D        g122<8,8,1>D    0x00000003UD    { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g98<2>UD        g124<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g100<2>UD       g125<4,4,1>UD                   { align1 2Q I@2 };
mov(8)          g98.1<2>UD      0x00000000UD                    { align1 1Q I@2 };
mov(8)          g100.1<2>UD     0x00000000UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g7UD            g98UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
mov(8)          g11<2>UD        g7<4,4,1>UD                     { align1 1Q $2.dst };
mov(8)          g13<2>UD        g8<4,4,1>UD                     { align1 2Q $2.dst };
mov(8)          g11.1<2>UD      g9<4,4,1>UD                     { align1 1Q @2 $2.dst };
mov(8)          g13.1<2>UD      g10<4,4,1>UD                    { align1 2Q @2 $2.dst };
else(16)        JIP:  LABEL11         UIP:  LABEL11             { align1 1H };

LABEL12:
add(16)         g8<1>D          g24<1,1,0>D     g120<1,1,0>D    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
cmp.l.f0.0(16)  g26<1>UD        g8<1,1,0>UD     g24<1,1,0>UD    { align1 1H I@1 compacted };
add(8)          g10<1>D         g5<8,4,2>D      g8<1,1,0>D      { align1 1Q compacted };
add(8)          g28<1>D         g20<8,4,2>D     g9<1,1,0>D      { align1 2Q compacted };
cmp.l.f0.0(8)   g29<1>UD        g10<8,8,1>UD    g5<8,4,2>UD     { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
mov(8)          g11<2>UD        g10<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g30<1>UD        g28<8,8,1>UD    g20<8,4,2>UD    { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@7 };
mov(8)          g13<2>UD        g28<4,4,1>UD                    { align1 2Q };
add3(16)        g31<1>D         g75<8,8,1>D     -g26<8,8,1>D    -g29<1,1,1>D { align1 1H I@2 };
mov(8)          g11.1<2>UD      g31<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g13.1<2>UD      g32<4,4,1>UD                    { align1 2Q I@2 };

LABEL11:
endif(16)       JIP:  LABEL6                                    { align1 1H };
add(8)          g15<1>D         g34<8,4,2>D     g120<1,1,0>D    { align1 1Q compacted };
add(8)          g32<1>D         g18<8,4,2>D     g121<1,1,0>D    { align1 2Q compacted };
cmp.l.f0.0(8)   g26<1>UD        g15<8,8,1>UD    g34<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g33<1>UD        g32<8,8,1>UD    g18<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g37<2>UD        g15<4,4,1>UD                    { align1 1Q };
mov(8)          g39<2>UD        g32<4,4,1>UD                    { align1 2Q };
add(8)          g27<1>D         -g26<8,8,1>D    g34.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g36<1>D         -g33<8,8,1>D    g18.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g37.1<2>UD      g27<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g39.1<2>UD      g36<4,4,1>UD                    { align1 2Q I@2 };
fbl(1)          g60<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g60<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g44<2>D         g[a0 352]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g44.1<2>D       g[a0 356]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g99.1<2>UD      g44.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g99<2>UD        g44<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g114<1>UD       f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g104UD   g99UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $6 };
mov(1)          f0<1>UD         g114<0,1,0>UD                   { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g60<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000400UD    { align1 WE_all 1N A@1 };
mov(1)          g45<2>D         g[a0 160]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g45.1<2>D       g[a0 164]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g100.1<2>UD     g45.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g100<2>UD       g45<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g100UD          g104UD          0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $5 };
add(16)         g38<1>D         g84<1,1,0>D     192D            { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g82<8,8,1>D     0D              { align1 1H };
(+f0.0) if(16)  JIP:  LABEL14         UIP:  LABEL13             { align1 1H };
shr(16)         g40<1>UD        g38<1,1,0>UD    0x00000006UD    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
shl(16)         g42<1>D         g40<8,8,1>D     0x00000003UD    { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(8)          g105<2>UD       g42<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $5.src };
mov(8)          g107<2>UD       g43<4,4,1>UD                    { align1 2Q I@2 };
mov(8)          g105.1<2>UD     0x00000000UD                    { align1 1Q I@2 };
mov(8)          g107.1<2>UD     0x00000000UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g43UD           g105UD          nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $5 };
mov(8)          g12<2>UD        g43<4,4,1>UD                    { align1 1Q $5.dst };
mov(8)          g14<2>UD        g44<4,4,1>UD                    { align1 2Q $5.dst };
mov(8)          g12.1<2>UD      g45<4,4,1>UD                    { align1 1Q @2 $5.dst };
mov(8)          g14.1<2>UD      g46<4,4,1>UD                    { align1 2Q @2 $5.dst };
else(16)        JIP:  LABEL13         UIP:  LABEL13             { align1 1H };

LABEL14:
add(16)         g44<1>D         g24<1,1,0>D     g38<1,1,0>D     { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
cmp.l.f0.0(16)  g46<1>UD        g44<1,1,0>UD    g24<1,1,0>UD    { align1 1H compacted };
add(8)          g28<1>D         g5<8,4,2>D      g44<1,1,0>D     { align1 1Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@7 };
add(8)          g48<1>D         g20<8,4,2>D     g45<1,1,0>D     { align1 2Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
cmp.l.f0.0(8)   g49<1>UD        g28<8,8,1>UD    g5<8,4,2>UD     { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
mov(8)          g12<2>UD        g28<4,4,1>UD                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@3 };
cmp.l.f0.0(8)   g50<1>UD        g48<8,8,1>UD    g20<8,4,2>UD    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@7 };
mov(8)          g14<2>UD        g48<4,4,1>UD                    { align1 2Q };
add3(16)        g51<1>D         g75<8,8,1>D     -g46<8,8,1>D    -g49<1,1,1>D { align1 1H I@2 };
mov(8)          g12.1<2>UD      g51<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g14.1<2>UD      g52<4,4,1>UD                    { align1 2Q I@2 };

LABEL13:
endif(16)       JIP:  LABEL6                                    { align1 1H };
add(8)          g29<1>D         g34<8,4,2>D     g38<1,1,0>D     { align1 1Q compacted };
add(8)          g52<1>D         g18<8,4,2>D     g39<1,1,0>D     { align1 2Q I@3 compacted };
cmp.l.f0.0(8)   g30<1>UD        g29<8,8,1>UD    g34<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g53<1>UD        g52<8,8,1>UD    g18<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g55<2>UD        g29<4,4,1>UD                    { align1 1Q };
mov(8)          g57<2>UD        g52<4,4,1>UD                    { align1 2Q };
add(8)          g31<1>D         -g30<8,8,1>D    g34.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g54<1>D         -g53<8,8,1>D    g18.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g55.1<2>UD      g31<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g57.1<2>UD      g54<4,4,1>UD                    { align1 2Q I@2 };
fbl(1)          g61<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
shl(1)          a0<1>UD         g61<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g46<2>D         g[a0 384]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g46.1<2>D       g[a0 388]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(8)          g106.1<2>UD     g46.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g106<2>UD       g46<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g113<1>UD       f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g111UD   g106UD          nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $5 };
mov(1)          f0<1>UD         g113<0,1,0>UD                   { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g61<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000600UD    { align1 WE_all 1N A@1 };
mov(1)          g47<2>D         g[a0 224]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g47.1<2>D       g[a0 228]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(8)          g107.1<2>UD     g47.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g107<2>UD       g47<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g107UD          g111UD          0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $5 };
add3(16)        g16<1>D         0x0003UW        g3<8,8,1>D      g16<1,1,1>D { align1 1H };

LABEL6:
while(16)       JIP:  LABEL15                                   { align1 1H };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_misc_copy_instances_indirect_code[] = {
    0x80000065, 0x30058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x38050220, 0x00000024, 0x00000000,
    0xe2311a40, 0x00013003, 0x80030061, 0x07054410,
    0x00000000, 0x76543210, 0x00041b69, 0x1d058660,
    0x02463805, 0x00000004, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa00310c, 0x00340000, 0x64071a40, 0x00800795,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00040061, 0x1b050160, 0x00460705, 0x00000000,
    0x00041940, 0x00010660, 0x16461b05, 0x00461d05,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x21220061, 0x001102cc, 0x2a120061, 0x001102cc,
    0x00030061, 0x05260aa0, 0x00000264, 0x00000000,
    0x00130061, 0x14260aa0, 0x00000264, 0x00000000,
    0x00030061, 0x3e260aa0, 0x000002a4, 0x00000000,
    0x00130061, 0x40260aa0, 0x000002a4, 0x00000000,
    0x21221661, 0x00110204, 0x2a121661, 0x00110204,
    0x21051661, 0x00110244, 0x2a141661, 0x00110244,
    0x213e1661, 0x0011025c, 0x2a401661, 0x0011025c,
    0x80001101, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x16240000, 0xfb043e24, 0x000c0000,
    0x00042169, 0x1a058660, 0x02461605, 0x00000006,
    0x01040022, 0x0001c060, 0x000000a8, 0x000000a8,
    0xa11e1640, 0x09ce2203, 0xaa1f1540, 0x09ce1203,
    0xe7201970, 0x09c01e03, 0x00033161, 0x3f060220,
    0x00341e05, 0x00000000, 0x00133161, 0x41060220,
    0x00341f05, 0x00000000, 0x00031b40, 0x76052660,
    0x06462005, 0x00442226, 0x00131c40, 0x21052660,
    0x06462105, 0x00441226, 0x00031a61, 0x3f260220,
    0x00347605, 0x00000000, 0x00131a61, 0x41260220,
    0x00342105, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044131, 0x00000000,
    0xfb0c3f24, 0x00041614, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0xe0031b68, 0x00801a03,
    0x00040061, 0x10050220, 0x00463805, 0x00000000,
    0x00040065, 0x24058220, 0x02461a05, 0xffffff00,
    0x00041b52, 0x2c044560, 0x0e8efffe, 0x38050305,
    0xa0261a40, 0x24201a02, 0x00041a61, 0x2e052660,
    0x00462c05, 0x00000000, 0x00041a65, 0x28058220,
    0x02462605, 0xffffffc0, 0xe02a1968, 0x00602803,
    0x00041970, 0x00010220, 0x52462e05, 0x00462a05,
    0x01040022, 0x0001c060, 0x00000480, 0x00000480,
    0x00040069, 0x2f05a660, 0x02462c05, 0x00000006,
    0x00031461, 0x33050220, 0x00440526, 0x00000000,
    0x00131361, 0x34050220, 0x00441426, 0x00000000,
    0xa1352140, 0x180e0502, 0xaa362140, 0x190e1402,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0xa0311d40, 0x2f002402, 0x00031b70, 0x77050220,
    0x52463505, 0x00440506, 0x00131b70, 0x37050220,
    0x52463605, 0x00441406, 0x00031a40, 0x38052660,
    0x06467705, 0x00440526, 0x00131a40, 0x39052660,
    0x06463705, 0x00441426, 0x00041966, 0x00010220,
    0x12463505, 0x00463805, 0x01040022, 0x0001c060,
    0x000001c8, 0x000000f8, 0xe0391f68, 0x00603103,
    0x00041969, 0x3b058660, 0x02463905, 0x00000003,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x00031961, 0x40060220, 0x00343b05, 0x00000000,
    0x80103101, 0x00000000, 0x00000000, 0x00000000,
    0x00131a61, 0x42060220, 0x00343c05, 0x00000000,
    0x00031a61, 0x40264220, 0x00000000, 0x00000000,
    0x00131a61, 0x42264220, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x3c240000, 0xfb044024, 0x000c0000,
    0x00032161, 0x08060220, 0x00343c05, 0x00000000,
    0x00132161, 0x0a060220, 0x00343d05, 0x00000000,
    0x0003a161, 0x08260220, 0x00343e05, 0x00000000,
    0x0013a161, 0x0a260220, 0x00343f05, 0x00000000,
    0x00040024, 0x0001c060, 0x000000e0, 0x000000e0,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0xa03d1f40, 0x31001802, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x273f1970, 0x18003d03,
    0xa1780040, 0x3d0e0502, 0xaa413140, 0x3e0e1402,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x00031a70, 0x42050220, 0x52467805, 0x00440506,
    0x00031f61, 0x08060220, 0x00347805, 0x00000000,
    0x80103101, 0x00000000, 0x00000000, 0x00000000,
    0x00131b70, 0x43050220, 0x52464105, 0x00441406,
    0x00131f61, 0x0a060220, 0x00344105, 0x00000000,
    0x00041a52, 0x44040e68, 0x0eae3305, 0x42053f05,
    0x00031961, 0x08260220, 0x00344405, 0x00000000,
    0x00131a61, 0x0a260220, 0x00344505, 0x00000000,
    0x00040025, 0x00004600, 0x00000000, 0x00000200,
    0xa1791640, 0x310e2202, 0xaa451540, 0x320e1202,
    0x00031a70, 0x7a050220, 0x52467905, 0x00442206,
    0x00131a70, 0x46050220, 0x52464505, 0x00441206,
    0x00030061, 0x48060220, 0x00347905, 0x00000000,
    0x00130061, 0x4a060220, 0x00344505, 0x00000000,
    0x00031c40, 0x7b052660, 0x06467a05, 0x00442226,
    0x00131c40, 0x47052660, 0x06464605, 0x00441226,
    0x00031a61, 0x48260220, 0x00347b05, 0x00000000,
    0x00131a61, 0x4a260220, 0x00344705, 0x00000000,
    0xe239004c, 0x00114004, 0x80001969, 0x10018220,
    0x02003904, 0x00000003, 0x80000961, 0x26060660,
    0x00010400, 0x00000000, 0x80000061, 0x26260660,
    0x00010410, 0x00000000, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x41260220,
    0x00002624, 0x00000000, 0x80031961, 0x41060220,
    0x00002604, 0x00000000, 0xe2750061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004131, 0x46140000, 0xfb00410c, 0x00340000,
    0x80001a61, 0x30010220, 0x00007504, 0x00000000,
    0x80001e69, 0x10018220, 0x02003904, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000800,
    0x80000961, 0x27060660, 0x00010400, 0x00000000,
    0x80000061, 0x27260660, 0x00010410, 0x00000000,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x80031a61, 0x42260220, 0x00002724, 0x00000000,
    0x80031961, 0x42060220, 0x00002704, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004131, 0x00000000, 0xfb08420c, 0x00344614,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0xe0490068, 0x00802403, 0x00031461, 0x4b050220,
    0x00440526, 0x00000000, 0x00131361, 0x4c050220,
    0x00441426, 0x00000000, 0xa14d2140, 0x180e0502,
    0xaa4e2140, 0x190e1402, 0x00031a70, 0x7c050220,
    0x52464d05, 0x00440506, 0x00131a70, 0x4f050220,
    0x52464e05, 0x00441406, 0x00031a40, 0x50052660,
    0x06467c05, 0x00440526, 0x00131a40, 0x51052660,
    0x06464f05, 0x00441426, 0x20521966, 0x50004d03,
    0x00041a70, 0x00010220, 0x42461005, 0x00464905,
    0x01040028, 0x0001c660, 0x00001188, 0x00001188,
    0x00040069, 0x54058660, 0x02461005, 0x00000008,
    0x00041c70, 0x00018660, 0x16465205, 0x00000000,
    0x01040022, 0x0001c060, 0x00000258, 0x00000158,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0xe0560068, 0x00605403, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x80001f01, 0x00000000,
    0x00000000, 0x00000000, 0x00041969, 0x58058660,
    0x02465605, 0x00000003, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x00031961, 0x4d060220,
    0x00345805, 0x00000000, 0x80101a01, 0x00000000,
    0x00000000, 0x00000000, 0x00133361, 0x4f060220,
    0x00345905, 0x00000000, 0x00031a61, 0x4d264220,
    0x00000000, 0x00000000, 0x00131a61, 0x4f264220,
    0x00000000, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044331, 0x59240000,
    0xfb044d24, 0x000c0000, 0x00032361, 0x09060220,
    0x00345905, 0x00000000, 0x00132361, 0x0b060220,
    0x00345a05, 0x00000000, 0x0003a361, 0x09260220,
    0x00345b05, 0x00000000, 0x0013a361, 0x0b260220,
    0x00345c05, 0x00000000, 0x00040024, 0x0001c060,
    0x00000110, 0x00000110, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0xa05a1b40, 0x54001802,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x275c1970, 0x18005a03, 0xa17d0040, 0x5a0e0502,
    0xaa5e0040, 0x5b0e1402, 0x00031a70, 0x5f050220,
    0x52467d05, 0x00440506, 0x80001f01, 0x00000000,
    0x00000000, 0x00000000, 0x00030061, 0x09060220,
    0x00347d05, 0x00000000, 0x00131b70, 0x60050220,
    0x52465e05, 0x00441406, 0x80101f01, 0x00000000,
    0x00000000, 0x00000000, 0x00130061, 0x0b060220,
    0x00345e05, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x00041a52, 0x61040e68,
    0x0eae4b05, 0x5f055c05, 0x00031961, 0x09260220,
    0x00346105, 0x00000000, 0x00131a61, 0x0b260220,
    0x00346205, 0x00000000, 0x00040025, 0x00004600,
    0x00000000, 0x00000f00, 0xa17e1640, 0x540e2202,
    0x80103201, 0x00000000, 0x00000000, 0x00000000,
    0xaa621540, 0x550e1202, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x00031a70, 0x7f050220,
    0x52467e05, 0x00442206, 0x80103601, 0x00000000,
    0x00000000, 0x00000000, 0x00131a70, 0x63050220,
    0x52466205, 0x00441206, 0x00030061, 0x65060220,
    0x00347e05, 0x00000000, 0x80103501, 0x00000000,
    0x00000000, 0x00000000, 0x00130061, 0x67060220,
    0x00346205, 0x00000000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x00031c40, 0x01052660,
    0x06467f05, 0x00442226, 0x80103501, 0x00000000,
    0x00000000, 0x00000000, 0x00131c40, 0x64052660,
    0x06466305, 0x00441226, 0x00031a61, 0x65260220,
    0x00340105, 0x00000000, 0x00131a61, 0x67260220,
    0x00346405, 0x00000000, 0xe23a004c, 0x00114004,
    0x80001969, 0x10018220, 0x02003a04, 0x00000003,
    0x80000961, 0x28060660, 0x00010480, 0x00000000,
    0x80000061, 0x28260660, 0x00010490, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x80031961, 0x4e260220, 0x00002824, 0x00000000,
    0x80031961, 0x4e060220, 0x00002804, 0x00000000,
    0xe2740061, 0x00113004, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x8a004331, 0x5a140000,
    0xfb004e0c, 0x00340000, 0x80001a61, 0x30010220,
    0x00007404, 0x00000000, 0x80001e69, 0x10018220,
    0x02003a04, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000c00, 0x80000961, 0x29060660,
    0x00010280, 0x00000000, 0x80000061, 0x29260660,
    0x00010290, 0x00000000, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80031a61, 0x56260220,
    0x00002924, 0x00000000, 0x80031961, 0x56060220,
    0x00002904, 0x00000000, 0x80002301, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x8a004231, 0x00000000,
    0xfb08560c, 0x00345a14, 0xa0660040, 0x04005403,
    0x00040070, 0x00018660, 0x16465205, 0x00000000,
    0x01040022, 0x0001c060, 0x00000228, 0x00000138,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0xe0681b68, 0x00606603, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x00041969, 0x6a058660,
    0x02466805, 0x00000003, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x00031961, 0x5b060220,
    0x00346a05, 0x00000000, 0x80103201, 0x00000000,
    0x00000000, 0x00000000, 0x80101a01, 0x00000000,
    0x00000000, 0x00000000, 0x00130061, 0x5d060220,
    0x00346b05, 0x00000000, 0x00031a61, 0x5b264220,
    0x00000000, 0x00000000, 0x00131a61, 0x5d264220,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x6b240000,
    0xfb045b24, 0x000c0000, 0x00032261, 0x0a060220,
    0x00346b05, 0x00000000, 0x00132261, 0x0c060220,
    0x00346c05, 0x00000000, 0x0003a261, 0x0a260220,
    0x00346d05, 0x00000000, 0x0013a261, 0x0c260220,
    0x00346e05, 0x00000000, 0x00040024, 0x0001c060,
    0x00000100, 0x00000100, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0xa06c0040, 0x66001802,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x276e1970, 0x18006c03, 0xa1021140, 0x6c0e0502,
    0x80103501, 0x00000000, 0x00000000, 0x00000000,
    0xaa700040, 0x6d0e1402, 0x00031a70, 0x71050220,
    0x52460205, 0x00440506, 0x80001f01, 0x00000000,
    0x00000000, 0x00000000, 0x00030061, 0x0a060220,
    0x00340205, 0x00000000, 0x00131b70, 0x72050220,
    0x52467005, 0x00441406, 0x80101f01, 0x00000000,
    0x00000000, 0x00000000, 0x00130061, 0x0c060220,
    0x00347005, 0x00000000, 0x00041a52, 0x73040e68,
    0x0eae4b05, 0x71056e05, 0x00031961, 0x0a260220,
    0x00347305, 0x00000000, 0x00131a61, 0x0c260220,
    0x00347405, 0x00000000, 0x00040025, 0x00004600,
    0x00000000, 0x00000a60, 0xa1070040, 0x660e2202,
    0xaa740040, 0x670e1202, 0x00031a70, 0x08050220,
    0x52460705, 0x00442206, 0x00131a70, 0x75050220,
    0x52467405, 0x00441206, 0x00030061, 0x77060220,
    0x00340705, 0x00000000, 0x00130061, 0x79060220,
    0x00347405, 0x00000000, 0x00031c40, 0x09052660,
    0x06460805, 0x00442226, 0x00131c40, 0x76052660,
    0x06467505, 0x00441226, 0x00031a61, 0x77260220,
    0x00340905, 0x00000000, 0x00131a61, 0x79260220,
    0x00347605, 0x00000000, 0xe23b004c, 0x00114004,
    0x80001969, 0x10018220, 0x02003b04, 0x00000003,
    0x80000961, 0x2a060660, 0x00010500, 0x00000000,
    0x80000061, 0x2a260660, 0x00010510, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80031961, 0x5c260220, 0x00002a24, 0x00000000,
    0x80031961, 0x5c060220, 0x00002a04, 0x00000000,
    0xe2730061, 0x00113004, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x8a004431, 0x61140000,
    0xfb005c0c, 0x00340000, 0x80001a61, 0x30010220,
    0x00007304, 0x00000000, 0x80001e69, 0x10018220,
    0x02003b04, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000e00, 0x80000961, 0x2b060660,
    0x00010380, 0x00000000, 0x80000061, 0x2b260660,
    0x00010390, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80031a61, 0x5d260220,
    0x00002b24, 0x00000000, 0x80031961, 0x5d060220,
    0x00002b04, 0x00000000, 0x80002401, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x8a004231, 0x00000000,
    0xfb085d0c, 0x00346114, 0xa0780040, 0x08005403,
    0x00040070, 0x00018660, 0x16465205, 0x00000000,
    0x01040022, 0x0001c060, 0x000001b8, 0x000000f8,
    0xe07a1b68, 0x00607803, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x00041969, 0x7c058660,
    0x02467a05, 0x00000003, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x00031961, 0x62060220,
    0x00347c05, 0x00000000, 0x00131a61, 0x64060220,
    0x00347d05, 0x00000000, 0x00031a61, 0x62264220,
    0x00000000, 0x00000000, 0x00131a61, 0x64264220,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x07240000,
    0xfb046224, 0x000c0000, 0x00032261, 0x0b060220,
    0x00340705, 0x00000000, 0x00132261, 0x0d060220,
    0x00340805, 0x00000000, 0x0003a261, 0x0b260220,
    0x00340905, 0x00000000, 0x0013a261, 0x0d260220,
    0x00340a05, 0x00000000, 0x00040024, 0x0001c060,
    0x000000d0, 0x000000d0, 0xa0081b40, 0x78001802,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x271a1970, 0x18000803, 0xa10a0040, 0x080e0502,
    0xaa1c0040, 0x090e1402, 0x00031a70, 0x1d050220,
    0x52460a05, 0x00440506, 0x80001f01, 0x00000000,
    0x00000000, 0x00000000, 0x00030061, 0x0b060220,
    0x00340a05, 0x00000000, 0x00131b70, 0x1e050220,
    0x52461c05, 0x00441406, 0x80101f01, 0x00000000,
    0x00000000, 0x00000000, 0x00130061, 0x0d060220,
    0x00341c05, 0x00000000, 0x00041a52, 0x1f040e68,
    0x0eae4b05, 0x1d051a05, 0x00031961, 0x0b260220,
    0x00341f05, 0x00000000, 0x00131a61, 0x0d260220,
    0x00342005, 0x00000000, 0x00040025, 0x00004600,
    0x00000000, 0x00000670, 0xa10f0040, 0x780e2202,
    0xaa200040, 0x790e1202, 0x00031a70, 0x1a050220,
    0x52460f05, 0x00442206, 0x00131a70, 0x21050220,
    0x52462005, 0x00441206, 0x00030061, 0x25060220,
    0x00340f05, 0x00000000, 0x00130061, 0x27060220,
    0x00342005, 0x00000000, 0x00031c40, 0x1b052660,
    0x06461a05, 0x00442226, 0x00131c40, 0x24052660,
    0x06462105, 0x00441226, 0x00031a61, 0x25260220,
    0x00341b05, 0x00000000, 0x00131a61, 0x27260220,
    0x00342405, 0x00000000, 0xe23c004c, 0x00114004,
    0x80001969, 0x10018220, 0x02003c04, 0x00000003,
    0x80000961, 0x2c060660, 0x00010580, 0x00000000,
    0x80000061, 0x2c260660, 0x00010590, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80031961, 0x63260220, 0x00002c24, 0x00000000,
    0x80031961, 0x63060220, 0x00002c04, 0x00000000,
    0xe2720061, 0x00113004, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x8a004631, 0x68140000,
    0xfb00630c, 0x00340000, 0x80001a61, 0x30010220,
    0x00007204, 0x00000000, 0x80001e69, 0x10018220,
    0x02003c04, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000400, 0x80000961, 0x2d060660,
    0x00010280, 0x00000000, 0x80000061, 0x2d260660,
    0x00010290, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80031a61, 0x64260220,
    0x00002d24, 0x00000000, 0x80031961, 0x64060220,
    0x00002d04, 0x00000000, 0x80002601, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x8a004531, 0x00000000,
    0xfb08640c, 0x00346814, 0xa0260040, 0x0c005403,
    0x00040070, 0x00018660, 0x16465205, 0x00000000,
    0x01040022, 0x0001c060, 0x00000218, 0x00000108,
    0xe0281b68, 0x00602603, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x00041969, 0x2a058660,
    0x02462805, 0x00000003, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x00031961, 0x69060220,
    0x00342a05, 0x00000000, 0x80103501, 0x00000000,
    0x00000000, 0x00000000, 0x00131a61, 0x6b060220,
    0x00342b05, 0x00000000, 0x00031a61, 0x69264220,
    0x00000000, 0x00000000, 0x00131a61, 0x6b264220,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044531, 0x2b240000,
    0xfb046924, 0x000c0000, 0x00032561, 0x0c060220,
    0x00342b05, 0x00000000, 0x00132561, 0x0e060220,
    0x00342c05, 0x00000000, 0x0003a561, 0x0c260220,
    0x00342d05, 0x00000000, 0x0013a561, 0x0e260220,
    0x00342e05, 0x00000000, 0x00040024, 0x0001c060,
    0x00000120, 0x00000120, 0xa02c1b40, 0x26001802,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x272e0070, 0x18002c03, 0xa11c0040, 0x2c0e0502,
    0x80101f01, 0x00000000, 0x00000000, 0x00000000,
    0xaa300040, 0x2d0e1402, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x00030070, 0x31050220,
    0x52461c05, 0x00440506, 0x80001f01, 0x00000000,
    0x00000000, 0x00000000, 0x00030061, 0x0c060220,
    0x00341c05, 0x00000000, 0x80101b01, 0x00000000,
    0x00000000, 0x00000000, 0x00130070, 0x32050220,
    0x52463005, 0x00441406, 0x80101f01, 0x00000000,
    0x00000000, 0x00000000, 0x00130061, 0x0e060220,
    0x00343005, 0x00000000, 0x00041a52, 0x33040e68,
    0x0eae4b05, 0x31052e05, 0x00031961, 0x0c260220,
    0x00343305, 0x00000000, 0x00131a61, 0x0e260220,
    0x00343405, 0x00000000, 0x00040025, 0x00004600,
    0x00000000, 0x00000220, 0xa11d0040, 0x260e2202,
    0xaa341b40, 0x270e1202, 0x00031a70, 0x1e050220,
    0x52461d05, 0x00442206, 0x00131a70, 0x35050220,
    0x52463405, 0x00441206, 0x00030061, 0x37060220,
    0x00341d05, 0x00000000, 0x00130061, 0x39060220,
    0x00343405, 0x00000000, 0x00031c40, 0x1f052660,
    0x06461e05, 0x00442226, 0x00131c40, 0x36052660,
    0x06463505, 0x00441226, 0x00031a61, 0x37260220,
    0x00341f05, 0x00000000, 0x00131a61, 0x39260220,
    0x00343605, 0x00000000, 0xe23d004c, 0x00114004,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80001969, 0x10018220, 0x02003d04, 0x00000003,
    0x80000961, 0x2e060660, 0x00010600, 0x00000000,
    0x80000061, 0x2e260660, 0x00010610, 0x00000000,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x80031961, 0x6a260220, 0x00002e24, 0x00000000,
    0x80031961, 0x6a060220, 0x00002e04, 0x00000000,
    0xe2710061, 0x00113004, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x8a004531, 0x6f140000,
    0xfb006a0c, 0x00340000, 0x80001a61, 0x30010220,
    0x00007104, 0x00000000, 0x80001e69, 0x10018220,
    0x02003d04, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000600, 0x80000961, 0x2f060660,
    0x00010380, 0x00000000, 0x80000061, 0x2f260660,
    0x00010390, 0x00000000, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x80031a61, 0x6b260220,
    0x00002f24, 0x00000000, 0x80031961, 0x6b060220,
    0x00002f04, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x8a004531, 0x00000000,
    0xfb086b0c, 0x00346f14, 0x00040052, 0x10044160,
    0x0e0e0003, 0x10050305, 0x00040027, 0x00014060,
    0x00000000, 0xffffee68, 0x80030061, 0x7e050220,
    0x00460005, 0x00000000, 0x80040931, 0x00000004,
    0x30007e0c, 0x00000000, 0x20000060, 0x00000000,
};
static const struct brw_kernel gfx125_misc_copy_instances_indirect = {
   .prog_data = {
      .base.nr_params = 14,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 6512,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_misc_copy_instances_indirect_relocs,
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
   .args = gfx125_misc_copy_instances_indirect_args,
   .code = gfx125_misc_copy_instances_indirect_code,
};
const char *gfx125_misc_copy_instances_indirect_sha1 = "bd3dc37aa4174aec98a2deda48f7aa363fb704b6";
