#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_misc_copy_instances_base_ptr_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_misc_copy_instances_base_ptr_args[] = {
   { 0, 8 },
   { 8, 8 },
   { 16, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g122<1>UD       g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g6<1>UD         g0.1<0,1,0>UD                   { align1 1H };
add(1)          g123<1>UD       g122<0,1,0>UD   0x00000000UD    { align1 WE_all 1N I@2 compacted };
mov(16)         g13<1>UD        g6<8,8,1>UD                     { align1 1H I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(1)         g1UD            g123UD          nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g83.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g15.1<2>F       g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g3.1<2>F        g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g17.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g85.1<2>F       g2.5<0,1,0>F                    { align1 1Q };
mov(8)          g19.1<2>F       g2.5<0,1,0>F                    { align1 2Q };
mov(8)          g83<2>F         g2<0,1,0>F                      { align1 1Q F@6 compacted };
mov(8)          g15<2>F         g2<0,1,0>F                      { align1 2Q F@6 compacted };
mov(8)          g3<2>F          g2.2<0,1,0>F                    { align1 1Q F@6 compacted };
mov(8)          g17<2>F         g2.2<0,1,0>F                    { align1 2Q F@6 compacted };
mov(8)          g85<2>F         g2.4<0,1,0>F                    { align1 1Q F@6 compacted };
mov(8)          g19<2>F         g2.4<0,1,0>F                    { align1 2Q F@6 compacted };
add(8)          g21<1>D         g83<8,4,2>D     152D            { align1 1Q F@6 compacted };
add(8)          g22<1>D         g15<8,4,2>D     152D            { align1 2Q F@5 compacted };
shr(8)          g67<1>UD        g85<8,4,2>UD    0x00000008UD    { align1 1Q F@2 compacted };
and(8)          g34<1>UD        g85<8,4,2>UD    0xffffff00UD    { align1 1Q };
shr(8)          g68<1>UD        g19<8,4,2>UD    0x00000008UD    { align1 2Q F@1 compacted };
and(8)          g35<1>UD        g19<8,4,2>UD    0xffffff00UD    { align1 2Q };
mov(8)          g27<2>UD        g21<4,4,1>UD                    { align1 1Q I@6 };
cmp.l.f0.0(16)  g23<1>UD        g21<1,1,0>UD    0x00000098UD    { align1 1H I@6 compacted };
mov(8)          g29<2>UD        g22<4,4,1>UD                    { align1 2Q };
add(8)          g36<1>D         g85<8,4,2>D     -g34<1,1,0>D    { align1 1Q I@6 compacted };
add3(16)        g42<1>D         65534W          -g67<8,8,1>D    g6<1,1,1>D { align1 1H I@6 };
add(8)          g37<1>D         g19<8,4,2>D     -g35<1,1,0>D    { align1 2Q I@6 compacted };
add(8)          g86<1>D         -g23<8,8,1>D    g83.1<8,4,2>D   { align1 1Q I@5 };
add(8)          g24<1>D         -g24<8,8,1>D    g15.1<8,4,2>D   { align1 2Q I@6 };
mov(16)         g44<1>D         -g42<8,8,1>D                    { align1 1H I@4 };
and(16)         g38<1>UD        g36<8,8,1>UD    0xffffffc0UD    { align1 1H I@4 };
mov(8)          g27.1<2>UD      g86<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g29.1<2>UD      g24<4,4,1>UD                    { align1 2Q I@4 };
shr(16)         g40<1>UD        g38<1,1,0>UD    0x00000006UD    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g25UD           g27UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
add(8)          g27<1>D         g83<8,4,2>D     g25<1,1,0>D     { align1 1Q $1.dst compacted };
add(8)          g28<1>D         g15<8,4,2>D     g26<1,1,0>D     { align1 2Q $1.dst compacted };
cmp.l.f0.0(8)   g87<1>UD        g27<8,8,1>UD    g83<8,4,2>UD    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $1.src };
cmp.l.f0.0(8)   g29<1>UD        g28<8,8,1>UD    g15<8,4,2>UD    { align1 2Q I@2 };
cmp.l.f0.0(16)  null<1>UD       g44<8,8,1>UD    g40<8,8,1>UD    { align1 1H I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
add(8)          g30<1>D         -g87<8,8,1>D    g83.1<8,4,2>D   { align1 1Q I@3 };
add(8)          g31<1>D         -g29<8,8,1>D    g15.1<8,4,2>D   { align1 2Q I@3 };
(+f0.0) if(16)  JIP:  LABEL0          UIP:  LABEL0              { align1 1H };
shl(16)         g45<1>D         -g42<8,8,1>D    0x00000006UD    { align1 1H };
or.z.f0.0(8)    null<1>UD       g3<8,4,2>UD     g3.1<8,4,2>UD   { align1 1Q F@4 };
or.z.f0.0(8)    null<1>UD       g17<8,4,2>UD    g17.1<8,4,2>UD  { align1 2Q F@3 };
add(16)         g47<1>D         g34<1,1,0>D     g45<1,1,0>D     { align1 1H I@3 compacted };
(+f0.0) if(16)  JIP:  LABEL2          UIP:  LABEL1              { align1 1H };
shr(16)         g49<1>UD        g47<1,1,0>UD    0x00000006UD    { align1 1H I@2 compacted };
shl(16)         g51<1>D         g49<8,8,1>D     0x00000003UD    { align1 1H I@1 };
mov(8)          g36<2>UD        g51<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g38<2>UD        g52<4,4,1>UD                    { align1 2Q I@2 };
mov(8)          g36.1<2>UD      0x00000000UD                    { align1 1Q I@2 };
mov(8)          g38.1<2>UD      0x00000000UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g52UD           g36UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
mov(8)          g5<2>UD         g52<4,4,1>UD                    { align1 1Q $2.dst };
mov(8)          g7<2>UD         g53<4,4,1>UD                    { align1 2Q $2.dst };
mov(8)          g5.1<2>UD       g54<4,4,1>UD                    { align1 1Q @2 $2.dst };
mov(8)          g7.1<2>UD       g55<4,4,1>UD                    { align1 2Q @2 $2.dst };
else(16)        JIP:  LABEL1          UIP:  LABEL1              { align1 1H };

LABEL2:
add(8)          g88<1>D         g3<8,4,2>D      g47<1,1,0>D     { align1 1Q I@2 compacted };
add(8)          g53<1>D         g17<8,4,2>D     g48<1,1,0>D     { align1 2Q I@3 compacted };
cmp.l.f0.0(8)   g89<1>UD        g88<8,8,1>UD    g3<8,4,2>UD     { align1 1Q I@2 };
cmp.l.f0.0(8)   g54<1>UD        g53<8,8,1>UD    g17<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g5<2>UD         g88<4,4,1>UD                    { align1 1Q I@7 };
mov(8)          g7<2>UD         g53<4,4,1>UD                    { align1 2Q I@7 };
add(8)          g90<1>D         -g89<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g55<1>D         -g54<8,8,1>D    g17.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g5.1<2>UD       g90<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g7.1<2>UD       g55<4,4,1>UD                    { align1 2Q I@2 };

LABEL1:
endif(16)       JIP:  LABEL0                                    { align1 1H };
add(16)         g56<1>D         g27<1,1,0>D     g47<1,1,0>D     { align1 1H compacted };
cmp.l.f0.0(16)  g58<1>UD        g56<1,1,0>UD    g27<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g62<2>UD        g56<4,4,1>UD                    { align1 1Q };
mov(8)          g64<2>UD        g57<4,4,1>UD                    { align1 2Q };
add(16)         g60<1>D         -g58<1,1,0>D    g30<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g62.1<2>UD      g60<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g64.1<2>UD      g61<4,4,1>UD                    { align1 2Q I@2 };
fbl(1)          g9<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g9<0,1,0>UD     0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g105<2>D        g[a0 160]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g105.1<2>D      g[a0 164]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g37.1<2>UD      g105.1<0,1,0>UD                 { align1 WE_all 1Q I@1 };
mov(8)          g37<2>UD        g105<0,1,0>UD                   { align1 WE_all 1Q I@1 };
mov(1)          g82<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g42UD    g37UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $3 };
mov(1)          f0<1>UD         g82<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g9<0,1,0>UD     0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000600UD    { align1 WE_all 1N A@1 };
mov(1)          g106<2>D        g[a0 448]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g106.1<2>D      g[a0 452]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g38.1<2>UD      g106.1<0,1,0>UD                 { align1 WE_all 1Q I@2 };
mov(8)          g38<2>UD        g106<0,1,0>UD                   { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g38UD           g42UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $4 };

LABEL0:
endif(16)       JIP:  LABEL3                                    { align1 1H };

LABEL3:
shr(16)         g63<1>UD        g34<1,1,0>UD    0x00000008UD    { align1 1H compacted };
or(8)           g65<1>UD        g3<8,4,2>UD     g3.1<8,4,2>UD   { align1 1Q F@4 };
or(8)           g66<1>UD        g17<8,4,2>UD    g17.1<8,4,2>UD  { align1 2Q F@3 };

LABEL13:
cmp.ge.f0.0(16) null<1>UD       g13<8,8,1>UD    g63<8,8,1>UD    { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL4        UIP:  LABEL4              { align1 1H };
shl(16)         g32<1>D         g13<8,8,1>D     0x00000008UD    { align1 1H };
cmp.z.f0.0(16)  null<1>D        g65<8,8,1>D     0D              { align1 1H I@4 };
(+f0.0) if(16)  JIP:  LABEL6          UIP:  LABEL5              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
shr(16)         g69<1>UD        g32<1,1,0>UD    0x00000006UD    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
shl(16)         g71<1>D         g69<8,8,1>D     0x00000003UD    { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g43<2>UD        g71<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@2 };
mov(8)          g45<2>UD        g72<4,4,1>UD                    { align1 2Q };
mov(8)          g43.1<2>UD      0x00000000UD                    { align1 1Q I@2 };
mov(8)          g45.1<2>UD      0x00000000UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g72UD           g43UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
mov(8)          g6<2>UD         g72<4,4,1>UD                    { align1 1Q $4.dst };
mov(8)          g8<2>UD         g73<4,4,1>UD                    { align1 2Q $4.dst };
mov(8)          g6.1<2>UD       g74<4,4,1>UD                    { align1 1Q @2 $4.dst };
mov(8)          g8.1<2>UD       g75<4,4,1>UD                    { align1 2Q @2 $4.dst };
else(16)        JIP:  LABEL5          UIP:  LABEL5              { align1 1H };

LABEL6:
add(8)          g91<1>D         g3<8,4,2>D      g32<1,1,0>D     { align1 1Q I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@4 };
add(8)          g73<1>D         g17<8,4,2>D     g33<1,1,0>D     { align1 2Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
cmp.l.f0.0(8)   g92<1>UD        g91<8,8,1>UD    g3<8,4,2>UD     { align1 1Q I@2 };
cmp.l.f0.0(8)   g74<1>UD        g73<8,8,1>UD    g17<8,4,2>UD    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
mov(8)          g6<2>UD         g91<4,4,1>UD                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@7 };
mov(8)          g8<2>UD         g73<4,4,1>UD                    { align1 2Q };
add(8)          g93<1>D         -g92<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g75<1>D         -g74<8,8,1>D    g17.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g6.1<2>UD       g93<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g8.1<2>UD       g75<4,4,1>UD                    { align1 2Q I@2 };

LABEL5:
endif(16)       JIP:  LABEL4                                    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add(16)         g76<1>D         g27<1,1,0>D     g32<1,1,0>D     { align1 1H compacted };
cmp.l.f0.0(16)  g78<1>UD        g76<1,1,0>UD    g27<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g82<2>UD        g76<4,4,1>UD                    { align1 1Q };
mov(8)          g84<2>UD        g77<4,4,1>UD                    { align1 2Q };
add(16)         g80<1>D         -g78<1,1,0>D    g30<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g82.1<2>UD      g80<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g84.1<2>UD      g81<4,4,1>UD                    { align1 2Q I@2 };
fbl(1)          g10<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g10<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g107<2>D        g[a0 192]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g107.1<2>D      g[a0 196]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g44.1<2>UD      g107.1<0,1,0>UD                 { align1 WE_all 1Q I@1 };
mov(8)          g44<2>UD        g107<0,1,0>UD                   { align1 WE_all 1Q I@1 };
mov(1)          g81<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g49UD    g44UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $4 };
mov(1)          f0<1>UD         g81<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g10<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000a00UD    { align1 WE_all 1N A@1 };
mov(1)          g108<2>D        g[a0 64]<0,1,0>D                { align1 WE_all 1N A@1 };
mov(1)          g108.1<2>D      g[a0 68]<0,1,0>D                { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g45.1<2>UD      g108.1<0,1,0>UD                 { align1 WE_all 1Q I@2 };
mov(8)          g45<2>UD        g108<0,1,0>UD                   { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g45UD           g49UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $4 };
add(16)         g83<1>D         g32<1,1,0>D     64D             { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g65<8,8,1>D     0D              { align1 1H };
(+f0.0) if(16)  JIP:  LABEL8          UIP:  LABEL7              { align1 1H };
shr(16)         g85<1>UD        g83<1,1,0>UD    0x00000006UD    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
shl(16)         g87<1>D         g85<8,8,1>D     0x00000003UD    { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g50<2>UD        g87<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@2 };
mov(8)          g52<2>UD        g88<4,4,1>UD                    { align1 2Q };
mov(8)          g50.1<2>UD      0x00000000UD                    { align1 1Q I@2 };
mov(8)          g52.1<2>UD      0x00000000UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g88UD           g50UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
mov(8)          g7<2>UD         g88<4,4,1>UD                    { align1 1Q $4.dst };
mov(8)          g9<2>UD         g89<4,4,1>UD                    { align1 2Q $4.dst };
mov(8)          g7.1<2>UD       g90<4,4,1>UD                    { align1 1Q @2 $4.dst };
mov(8)          g9.1<2>UD       g91<4,4,1>UD                    { align1 2Q @2 $4.dst };
else(16)        JIP:  LABEL7          UIP:  LABEL7              { align1 1H };

LABEL8:
add(8)          g94<1>D         g3<8,4,2>D      g83<1,1,0>D     { align1 1Q I@3 compacted };
add(8)          g89<1>D         g17<8,4,2>D     g84<1,1,0>D     { align1 2Q I@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
cmp.l.f0.0(8)   g95<1>UD        g94<8,8,1>UD    g3<8,4,2>UD     { align1 1Q I@2 };
cmp.l.f0.0(8)   g90<1>UD        g89<8,8,1>UD    g17<8,4,2>UD    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
mov(8)          g7<2>UD         g94<4,4,1>UD                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@7 };
mov(8)          g9<2>UD         g89<4,4,1>UD                    { align1 2Q };
add(8)          g96<1>D         -g95<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g91<1>D         -g90<8,8,1>D    g17.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g7.1<2>UD       g96<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g9.1<2>UD       g91<4,4,1>UD                    { align1 2Q I@2 };

LABEL7:
endif(16)       JIP:  LABEL4                                    { align1 1H };
add(16)         g92<1>D         g27<1,1,0>D     g83<1,1,0>D     { align1 1H compacted };
cmp.l.f0.0(16)  g94<1>UD        g92<1,1,0>UD    g27<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g98<2>UD        g92<4,4,1>UD                    { align1 1Q };
mov(8)          g100<2>UD       g93<4,4,1>UD                    { align1 2Q };
add(16)         g96<1>D         -g94<1,1,0>D    g30<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g98.1<2>UD      g96<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g100.1<2>UD     g97<4,4,1>UD                    { align1 2Q I@2 };
fbl(1)          g11<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g11<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g109<2>D        g[a0 224]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g109.1<2>D      g[a0 228]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g51.1<2>UD      g109.1<0,1,0>UD                 { align1 WE_all 1Q I@1 };
mov(8)          g51<2>UD        g109<0,1,0>UD                   { align1 WE_all 1Q I@1 };
mov(1)          g80<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g56UD    g51UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $4 };
mov(1)          f0<1>UD         g80<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g11<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000c00UD    { align1 WE_all 1N A@1 };
mov(1)          g110<2>D        g[a0 64]<0,1,0>D                { align1 WE_all 1N A@1 };
mov(1)          g110.1<2>D      g[a0 68]<0,1,0>D                { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g52.1<2>UD      g110.1<0,1,0>UD                 { align1 WE_all 1Q I@2 };
mov(8)          g52<2>UD        g110<0,1,0>UD                   { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g52UD           g56UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $4 };
add(16)         g99<1>D         g32<1,1,0>D     128D            { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g65<8,8,1>D     0D              { align1 1H };
(+f0.0) if(16)  JIP:  LABEL10         UIP:  LABEL9              { align1 1H };
shr(16)         g101<1>UD       g99<1,1,0>UD    0x00000006UD    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
shl(16)         g103<1>D        g101<8,8,1>D    0x00000003UD    { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g57<2>UD        g103<4,4,1>UD                   { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@2 };
mov(8)          g59<2>UD        g104<4,4,1>UD                   { align1 2Q };
mov(8)          g57.1<2>UD      0x00000000UD                    { align1 1Q I@2 };
mov(8)          g59.1<2>UD      0x00000000UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g104UD          g57UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
mov(8)          g8<2>UD         g104<4,4,1>UD                   { align1 1Q $4.dst };
mov(8)          g10<2>UD        g105<4,4,1>UD                   { align1 2Q $4.dst };
mov(8)          g8.1<2>UD       g106<4,4,1>UD                   { align1 1Q @2 $4.dst };
mov(8)          g10.1<2>UD      g107<4,4,1>UD                   { align1 2Q @2 $4.dst };
else(16)        JIP:  LABEL9          UIP:  LABEL9              { align1 1H };

LABEL10:
add(8)          g97<1>D         g3<8,4,2>D      g99<1,1,0>D     { align1 1Q I@3 compacted };
add(8)          g105<1>D        g17<8,4,2>D     g100<1,1,0>D    { align1 2Q I@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
cmp.l.f0.0(8)   g98<1>UD        g97<8,8,1>UD    g3<8,4,2>UD     { align1 1Q I@2 };
cmp.l.f0.0(8)   g106<1>UD       g105<8,8,1>UD   g17<8,4,2>UD    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
mov(8)          g8<2>UD         g97<4,4,1>UD                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@7 };
mov(8)          g10<2>UD        g105<4,4,1>UD                   { align1 2Q };
add(8)          g101<1>D        -g98<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g107<1>D        -g106<8,8,1>D   g17.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g8.1<2>UD       g101<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g10.1<2>UD      g107<4,4,1>UD                   { align1 2Q I@2 };

LABEL9:
endif(16)       JIP:  LABEL4                                    { align1 1H };
add(16)         g108<1>D        g27<1,1,0>D     g99<1,1,0>D     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
cmp.l.f0.0(16)  g110<1>UD       g108<1,1,0>UD   g27<1,1,0>UD    { align1 1H compacted };
mov(8)          g114<2>UD       g108<4,4,1>UD                   { align1 1Q };
mov(8)          g116<2>UD       g109<4,4,1>UD                   { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
add(16)         g112<1>D        -g110<1,1,0>D   g30<1,1,0>D     { align1 1H compacted };
mov(8)          g114.1<2>UD     g112<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g116.1<2>UD     g113<4,4,1>UD                   { align1 2Q I@2 };
fbl(1)          g15<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g15<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g118<2>D        g[a0 256]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g118.1<2>D      g[a0 260]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g58.1<2>UD      g118.1<0,1,0>UD                 { align1 WE_all 1Q I@1 };
mov(8)          g58<2>UD        g118<0,1,0>UD                   { align1 WE_all 1Q I@1 };
mov(1)          g79<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g69UD    g58UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $4 };
mov(1)          f0<1>UD         g79<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g15<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000e00UD    { align1 WE_all 1N A@1 };
mov(1)          g119<2>D        g[a0 64]<0,1,0>D                { align1 WE_all 1N A@1 };
mov(1)          g119.1<2>D      g[a0 68]<0,1,0>D                { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g59.1<2>UD      g119.1<0,1,0>UD                 { align1 WE_all 1Q I@2 };
mov(8)          g59<2>UD        g119<0,1,0>UD                   { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g59UD           g69UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $4 };
add(16)         g115<1>D        g32<1,1,0>D     192D            { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g65<8,8,1>D     0D              { align1 1H };
(+f0.0) if(16)  JIP:  LABEL12         UIP:  LABEL11             { align1 1H };
shr(16)         g117<1>UD       g115<1,1,0>UD   0x00000006UD    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
shl(16)         g119<1>D        g117<8,8,1>D    0x00000003UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g70<2>UD        g119<4,4,1>UD                   { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $4.src };
mov(8)          g72<2>UD        g120<4,4,1>UD                   { align1 2Q I@2 };
mov(8)          g70.1<2>UD      0x00000000UD                    { align1 1Q I@2 };
mov(8)          g72.1<2>UD      0x00000000UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g120UD          g70UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
mov(8)          g9<2>UD         g120<4,4,1>UD                   { align1 1Q $4.dst };
mov(8)          g11<2>UD        g121<4,4,1>UD                   { align1 2Q $4.dst };
mov(8)          g9.1<2>UD       g122<4,4,1>UD                   { align1 1Q @2 $4.dst };
mov(8)          g11.1<2>UD      g123<4,4,1>UD                   { align1 2Q @2 $4.dst };
else(16)        JIP:  LABEL11         UIP:  LABEL11             { align1 1H };

LABEL12:
add(8)          g102<1>D        g3<8,4,2>D      g115<1,1,0>D    { align1 1Q I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@4 };
add(8)          g121<1>D        g17<8,4,2>D     g116<1,1,0>D    { align1 2Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
cmp.l.f0.0(8)   g103<1>UD       g102<8,8,1>UD   g3<8,4,2>UD     { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@2 };
cmp.l.f0.0(8)   g122<1>UD       g121<8,8,1>UD   g17<8,4,2>UD    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
mov(8)          g9<2>UD         g102<4,4,1>UD                   { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@7 };
mov(8)          g11<2>UD        g121<4,4,1>UD                   { align1 2Q };
add(8)          g104<1>D        -g103<8,8,1>D   g3.1<8,4,2>D    { align1 1Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@4 };
add(8)          g123<1>D        -g122<8,8,1>D   g17.1<8,4,2>D   { align1 2Q };
mov(8)          g9.1<2>UD       g104<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g11.1<2>UD      g123<4,4,1>UD                   { align1 2Q I@2 };

LABEL11:
endif(16)       JIP:  LABEL4                                    { align1 1H };
add(16)         g124<1>D        g27<1,1,0>D     g115<1,1,0>D    { align1 1H compacted };
cmp.l.f0.0(16)  g126<1>UD       g124<1,1,0>UD   g27<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g5<2>UD         g124<4,4,1>UD                   { align1 1Q };
mov(8)          g7<2>UD         g125<4,4,1>UD                   { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
add(16)         g1<1>D          -g126<1,1,0>D   g30<1,1,0>D     { align1 1H A@1 compacted };
mov(8)          g5.1<2>UD       g1<4,4,1>UD                     { align1 1Q I@1 };
mov(8)          g7.1<2>UD       g2<4,4,1>UD                     { align1 2Q I@2 };
fbl(1)          g19<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g19<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g120<2>D        g[a0 288]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g120.1<2>D      g[a0 292]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g71.1<2>UD      g120.1<0,1,0>UD                 { align1 WE_all 1Q I@1 };
mov(8)          g71<2>UD        g120<0,1,0>UD                   { align1 WE_all 1Q I@1 };
mov(1)          g78<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g76UD    g71UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $4 };
mov(1)          f0<1>UD         g78<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g19<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
mov(1)          g121<2>D        g[a0 160]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g121.1<2>D      g[a0 164]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g72.1<2>UD      g121.1<0,1,0>UD                 { align1 WE_all 1Q I@2 };
mov(8)          g72<2>UD        g121<0,1,0>UD                   { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g72UD           g76UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $4 };
add3(16)        g13<1>D         0x0003UW        g67<8,8,1>D     g13<1,1,1>D { align1 1H };

LABEL4:
while(16)       JIP:  LABEL13                                   { align1 1H };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_misc_copy_instances_base_ptr_code[] = {
    0x80000065, 0x7a058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x06050220, 0x00000024, 0x00000000,
    0xe27b1a40, 0x00017a03, 0x00041a61, 0x0d050220,
    0x00460605, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa007b0c, 0x00340000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x21530061, 0x001102cc,
    0x2a0f0061, 0x001102cc, 0x00030061, 0x03260aa0,
    0x00000264, 0x00000000, 0x00130061, 0x11260aa0,
    0x00000264, 0x00000000, 0x00030061, 0x55260aa0,
    0x000002a4, 0x00000000, 0x00130061, 0x13260aa0,
    0x000002a4, 0x00000000, 0x21531661, 0x00110204,
    0x2a0f1661, 0x00110204, 0x21031661, 0x00110244,
    0x2a111661, 0x00110244, 0x21551661, 0x0011025c,
    0x2a131661, 0x0011025c, 0xa1151640, 0x098e5303,
    0xaa161540, 0x098e0f03, 0xe1431268, 0x008e5503,
    0x00030065, 0x22058220, 0x02445506, 0xffffff00,
    0xea441168, 0x008e1303, 0x00130065, 0x23058220,
    0x02441306, 0xffffff00, 0x00031e61, 0x1b060220,
    0x00341505, 0x00000000, 0xe7171e70, 0x09801503,
    0x00130061, 0x1d060220, 0x00341605, 0x00000000,
    0xa1241e40, 0x222e5502, 0x00041e52, 0x2a044560,
    0x0e8efffe, 0x06054305, 0xaa251e40, 0x232e1302,
    0x00031d40, 0x56052660, 0x06461705, 0x00445326,
    0x00131e40, 0x18052660, 0x06461805, 0x00440f26,
    0x00041c61, 0x2c052660, 0x00462a05, 0x00000000,
    0x00041c65, 0x26058220, 0x02462405, 0xffffffc0,
    0x00031c61, 0x1b260220, 0x00345605, 0x00000000,
    0x00131c61, 0x1d260220, 0x00341805, 0x00000000,
    0xe0281b68, 0x00602603, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x00044131, 0x19140000,
    0xfb041b24, 0x00040000, 0xa11b2140, 0x190e5302,
    0xaa1c2140, 0x1a0e0f02, 0x00031a70, 0x57050220,
    0x52461b05, 0x00445306, 0x80103101, 0x00000000,
    0x00000000, 0x00000000, 0x00131a70, 0x1d050220,
    0x52461c05, 0x00440f06, 0x00041d70, 0x00010220,
    0x52462c05, 0x00462805, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x00031b40, 0x1e052660,
    0x06465705, 0x00445326, 0x00131b40, 0x1f052660,
    0x06461d05, 0x00440f26, 0x01040022, 0x0001c060,
    0x00000388, 0x00000388, 0x00040069, 0x2d05a660,
    0x02462a05, 0x00000006, 0x00031466, 0x00010220,
    0x12440306, 0x00440326, 0x00131366, 0x00010220,
    0x12441106, 0x00441126, 0xa02f1b40, 0x2d002202,
    0x01040022, 0x0001c060, 0x00000168, 0x000000d8,
    0xe0311a68, 0x00602f03, 0x00041969, 0x33058660,
    0x02463105, 0x00000003, 0x00031961, 0x24060220,
    0x00343305, 0x00000000, 0x00131a61, 0x26060220,
    0x00343405, 0x00000000, 0x00031a61, 0x24264220,
    0x00000000, 0x00000000, 0x00131a61, 0x26264220,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x34240000,
    0xfb042424, 0x000c0000, 0x00032261, 0x05060220,
    0x00343405, 0x00000000, 0x00132261, 0x07060220,
    0x00343505, 0x00000000, 0x0003a261, 0x05260220,
    0x00343605, 0x00000000, 0x0013a261, 0x07260220,
    0x00343705, 0x00000000, 0x00040024, 0x0001c060,
    0x000000a0, 0x000000a0, 0xa1581a40, 0x2f0e0302,
    0xaa351b40, 0x300e1102, 0x00031a70, 0x59050220,
    0x52465805, 0x00440306, 0x00131a70, 0x36050220,
    0x52463505, 0x00441106, 0x00031f61, 0x05060220,
    0x00345805, 0x00000000, 0x00131f61, 0x07060220,
    0x00343505, 0x00000000, 0x00031c40, 0x5a052660,
    0x06465905, 0x00440326, 0x00131c40, 0x37052660,
    0x06463605, 0x00441126, 0x00031a61, 0x05260220,
    0x00345a05, 0x00000000, 0x00131a61, 0x07260220,
    0x00343705, 0x00000000, 0x00040025, 0x00004600,
    0x00000000, 0x000001d8, 0xa0380040, 0x2f001b02,
    0x273a1970, 0x1b003803, 0x00030061, 0x3e060220,
    0x00343805, 0x00000000, 0x00130061, 0x40060220,
    0x00343905, 0x00000000, 0xa03c1b40, 0x1e023a02,
    0x00031961, 0x3e260220, 0x00343c05, 0x00000000,
    0x00131a61, 0x40260220, 0x00343d05, 0x00000000,
    0xe209004c, 0x00114004, 0x80001969, 0x10018220,
    0x02000904, 0x00000003, 0x80000961, 0x69060660,
    0x00010280, 0x00000000, 0x80000061, 0x69260660,
    0x00010290, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x25260220,
    0x00006924, 0x00000000, 0x80031961, 0x25060220,
    0x00006904, 0x00000000, 0xe2520061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004331, 0x2a140000, 0xfb00250c, 0x00340000,
    0x80001a61, 0x30010220, 0x00005204, 0x00000000,
    0x80001e69, 0x10018220, 0x02000904, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000600,
    0x80000961, 0x6a060660, 0x00010700, 0x00000000,
    0x80000061, 0x6a260660, 0x00010710, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80031a61, 0x26260220, 0x00006a24, 0x00000000,
    0x80031961, 0x26060220, 0x00006a04, 0x00000000,
    0x80002301, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004431, 0x00000000, 0xfb08260c, 0x00342a14,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0xe03f0068, 0x00802203, 0x00031466, 0x41050220,
    0x02440306, 0x00440326, 0x00131366, 0x42050220,
    0x02441106, 0x00441126, 0x00041a70, 0x00010220,
    0x42460d05, 0x00463f05, 0x01040028, 0x0001c660,
    0x00000fb8, 0x00000fb8, 0x00040069, 0x20058660,
    0x02460d05, 0x00000008, 0x00041c70, 0x00018660,
    0x16464105, 0x00000000, 0x01040022, 0x0001c060,
    0x00000208, 0x00000138, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0xe0451b68, 0x00602003,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x00041969, 0x47058660, 0x02464505, 0x00000003,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x00031961, 0x2b060220, 0x00344705, 0x00000000,
    0x80103401, 0x00000000, 0x00000000, 0x00000000,
    0x80101a01, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x2d060220, 0x00344805, 0x00000000,
    0x00031a61, 0x2b264220, 0x00000000, 0x00000000,
    0x00131a61, 0x2d264220, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x48240000, 0xfb042b24, 0x000c0000,
    0x00032461, 0x06060220, 0x00344805, 0x00000000,
    0x00132461, 0x08060220, 0x00344905, 0x00000000,
    0x0003a461, 0x06260220, 0x00344a05, 0x00000000,
    0x0013a461, 0x08260220, 0x00344b05, 0x00000000,
    0x00040024, 0x0001c060, 0x000000e0, 0x000000e0,
    0xa15b1b40, 0x200e0302, 0x80101c01, 0x00000000,
    0x00000000, 0x00000000, 0xaa490040, 0x210e1102,
    0x80001c01, 0x00000000, 0x00000000, 0x00000000,
    0x00031a70, 0x5c050220, 0x52465b05, 0x00440306,
    0x00131a70, 0x4a050220, 0x52464905, 0x00441106,
    0x80001f01, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x06060220, 0x00345b05, 0x00000000,
    0x80101f01, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x08060220, 0x00344905, 0x00000000,
    0x00031c40, 0x5d052660, 0x06465c05, 0x00440326,
    0x00131c40, 0x4b052660, 0x06464a05, 0x00441126,
    0x00031a61, 0x06260220, 0x00345d05, 0x00000000,
    0x00131a61, 0x08260220, 0x00344b05, 0x00000000,
    0x00040025, 0x00004600, 0x00000000, 0x00000d80,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0xa04c0040, 0x20001b02, 0x274e1970, 0x1b004c03,
    0x00030061, 0x52060220, 0x00344c05, 0x00000000,
    0x00130061, 0x54060220, 0x00344d05, 0x00000000,
    0xa0501b40, 0x1e024e02, 0x00031961, 0x52260220,
    0x00345005, 0x00000000, 0x00131a61, 0x54260220,
    0x00345105, 0x00000000, 0xe20a004c, 0x00114004,
    0x80001969, 0x10018220, 0x02000a04, 0x00000003,
    0x80000961, 0x6b060660, 0x00010300, 0x00000000,
    0x80000061, 0x6b260660, 0x00010310, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80031961, 0x2c260220, 0x00006b24, 0x00000000,
    0x80031961, 0x2c060220, 0x00006b04, 0x00000000,
    0xe2510061, 0x00113004, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x8a004431, 0x31140000,
    0xfb002c0c, 0x00340000, 0x80001a61, 0x30010220,
    0x00005104, 0x00000000, 0x80001e69, 0x10018220,
    0x02000a04, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000a00, 0x80000961, 0x6c060660,
    0x00010100, 0x00000000, 0x80000061, 0x6c260660,
    0x00010110, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80031a61, 0x2d260220,
    0x00006c24, 0x00000000, 0x80031961, 0x2d060220,
    0x00006c04, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x8a004431, 0x00000000,
    0xfb082d0c, 0x00343114, 0xa0530040, 0x04002003,
    0x00040070, 0x00018660, 0x16464105, 0x00000000,
    0x01040022, 0x0001c060, 0x000001d8, 0x00000118,
    0xe0551b68, 0x00605303, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x00041969, 0x57058660,
    0x02465505, 0x00000003, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x00031961, 0x32060220,
    0x00345705, 0x00000000, 0x80103401, 0x00000000,
    0x00000000, 0x00000000, 0x80101a01, 0x00000000,
    0x00000000, 0x00000000, 0x00130061, 0x34060220,
    0x00345805, 0x00000000, 0x00031a61, 0x32264220,
    0x00000000, 0x00000000, 0x00131a61, 0x34264220,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044431, 0x58240000,
    0xfb043224, 0x000c0000, 0x00032461, 0x07060220,
    0x00345805, 0x00000000, 0x00132461, 0x09060220,
    0x00345905, 0x00000000, 0x0003a461, 0x07260220,
    0x00345a05, 0x00000000, 0x0013a461, 0x09260220,
    0x00345b05, 0x00000000, 0x00040024, 0x0001c060,
    0x000000d0, 0x000000d0, 0xa15e1b40, 0x530e0302,
    0xaa591c40, 0x540e1102, 0x80001c01, 0x00000000,
    0x00000000, 0x00000000, 0x00031a70, 0x5f050220,
    0x52465e05, 0x00440306, 0x00131a70, 0x5a050220,
    0x52465905, 0x00441106, 0x80001f01, 0x00000000,
    0x00000000, 0x00000000, 0x00030061, 0x07060220,
    0x00345e05, 0x00000000, 0x80101f01, 0x00000000,
    0x00000000, 0x00000000, 0x00130061, 0x09060220,
    0x00345905, 0x00000000, 0x00031c40, 0x60052660,
    0x06465f05, 0x00440326, 0x00131c40, 0x5b052660,
    0x06465a05, 0x00441126, 0x00031a61, 0x07260220,
    0x00346005, 0x00000000, 0x00131a61, 0x09260220,
    0x00345b05, 0x00000000, 0x00040025, 0x00004600,
    0x00000000, 0x000009b8, 0xa05c0040, 0x53001b02,
    0x275e1970, 0x1b005c03, 0x00030061, 0x62060220,
    0x00345c05, 0x00000000, 0x00130061, 0x64060220,
    0x00345d05, 0x00000000, 0xa0601b40, 0x1e025e02,
    0x00031961, 0x62260220, 0x00346005, 0x00000000,
    0x00131a61, 0x64260220, 0x00346105, 0x00000000,
    0xe20b004c, 0x00114004, 0x80001969, 0x10018220,
    0x02000b04, 0x00000003, 0x80000961, 0x6d060660,
    0x00010380, 0x00000000, 0x80000061, 0x6d260660,
    0x00010390, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x33260220,
    0x00006d24, 0x00000000, 0x80031961, 0x33060220,
    0x00006d04, 0x00000000, 0xe2500061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004431, 0x38140000, 0xfb00330c, 0x00340000,
    0x80001a61, 0x30010220, 0x00005004, 0x00000000,
    0x80001e69, 0x10018220, 0x02000b04, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000c00,
    0x80000961, 0x6e060660, 0x00010100, 0x00000000,
    0x80000061, 0x6e260660, 0x00010110, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80031a61, 0x34260220, 0x00006e24, 0x00000000,
    0x80031961, 0x34060220, 0x00006e04, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004431, 0x00000000, 0xfb08340c, 0x00343814,
    0xa0630040, 0x08002003, 0x00040070, 0x00018660,
    0x16464105, 0x00000000, 0x01040022, 0x0001c060,
    0x000001d8, 0x00000118, 0xe0651b68, 0x00606303,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x00041969, 0x67058660, 0x02466505, 0x00000003,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x00031961, 0x39060220, 0x00346705, 0x00000000,
    0x80103401, 0x00000000, 0x00000000, 0x00000000,
    0x80101a01, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x3b060220, 0x00346805, 0x00000000,
    0x00031a61, 0x39264220, 0x00000000, 0x00000000,
    0x00131a61, 0x3b264220, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x68240000, 0xfb043924, 0x000c0000,
    0x00032461, 0x08060220, 0x00346805, 0x00000000,
    0x00132461, 0x0a060220, 0x00346905, 0x00000000,
    0x0003a461, 0x08260220, 0x00346a05, 0x00000000,
    0x0013a461, 0x0a260220, 0x00346b05, 0x00000000,
    0x00040024, 0x0001c060, 0x000000d0, 0x000000d0,
    0xa1611b40, 0x630e0302, 0xaa691c40, 0x640e1102,
    0x80001c01, 0x00000000, 0x00000000, 0x00000000,
    0x00031a70, 0x62050220, 0x52466105, 0x00440306,
    0x00131a70, 0x6a050220, 0x52466905, 0x00441106,
    0x80001f01, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x08060220, 0x00346105, 0x00000000,
    0x80101f01, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x0a060220, 0x00346905, 0x00000000,
    0x00031c40, 0x65052660, 0x06466205, 0x00440326,
    0x00131c40, 0x6b052660, 0x06466a05, 0x00441126,
    0x00031a61, 0x08260220, 0x00346505, 0x00000000,
    0x00131a61, 0x0a260220, 0x00346b05, 0x00000000,
    0x00040025, 0x00004600, 0x00000000, 0x00000600,
    0xa06c0040, 0x63001b02, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x276e0070, 0x1b006c03,
    0x00030061, 0x72060220, 0x00346c05, 0x00000000,
    0x00130061, 0x74060220, 0x00346d05, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0xa0700040, 0x1e026e02, 0x00031961, 0x72260220,
    0x00347005, 0x00000000, 0x00131a61, 0x74260220,
    0x00347105, 0x00000000, 0xe20f004c, 0x00114004,
    0x80001969, 0x10018220, 0x02000f04, 0x00000003,
    0x80000961, 0x76060660, 0x00010400, 0x00000000,
    0x80000061, 0x76260660, 0x00010410, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80031961, 0x3a260220, 0x00007624, 0x00000000,
    0x80031961, 0x3a060220, 0x00007604, 0x00000000,
    0xe24f0061, 0x00113004, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x8a004431, 0x45140000,
    0xfb003a0c, 0x00340000, 0x80001a61, 0x30010220,
    0x00004f04, 0x00000000, 0x80001e69, 0x10018220,
    0x02000f04, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000e00, 0x80000961, 0x77060660,
    0x00010100, 0x00000000, 0x80000061, 0x77260660,
    0x00010110, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80031a61, 0x3b260220,
    0x00007724, 0x00000000, 0x80031961, 0x3b060220,
    0x00007704, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x8a004431, 0x00000000,
    0xfb083b0c, 0x00344514, 0xa0730040, 0x0c002003,
    0x00040070, 0x00018660, 0x16464105, 0x00000000,
    0x01040022, 0x0001c060, 0x00000228, 0x00000128,
    0xe0751b68, 0x00607303, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00040069, 0x77058660,
    0x02467505, 0x00000003, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x00031961, 0x46060220,
    0x00347705, 0x00000000, 0x80103401, 0x00000000,
    0x00000000, 0x00000000, 0x00131a61, 0x48060220,
    0x00347805, 0x00000000, 0x00031a61, 0x46264220,
    0x00000000, 0x00000000, 0x00131a61, 0x48264220,
    0x00000000, 0x00000000, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044431, 0x78240000,
    0xfb044624, 0x000c0000, 0x00032461, 0x09060220,
    0x00347805, 0x00000000, 0x00132461, 0x0b060220,
    0x00347905, 0x00000000, 0x0003a461, 0x09260220,
    0x00347a05, 0x00000000, 0x0013a461, 0x0b260220,
    0x00347b05, 0x00000000, 0x00040024, 0x0001c060,
    0x00000110, 0x00000110, 0xa1661b40, 0x730e0302,
    0x80101c01, 0x00000000, 0x00000000, 0x00000000,
    0xaa790040, 0x740e1102, 0x80001c01, 0x00000000,
    0x00000000, 0x00000000, 0x00031a70, 0x67050220,
    0x52466605, 0x00440306, 0x80101a01, 0x00000000,
    0x00000000, 0x00000000, 0x00130070, 0x7a050220,
    0x52467905, 0x00441106, 0x80001f01, 0x00000000,
    0x00000000, 0x00000000, 0x00030061, 0x09060220,
    0x00346605, 0x00000000, 0x80101f01, 0x00000000,
    0x00000000, 0x00000000, 0x00130061, 0x0b060220,
    0x00347905, 0x00000000, 0x00031c40, 0x68052660,
    0x06466705, 0x00440326, 0x80103001, 0x00000000,
    0x00000000, 0x00000000, 0x80101c01, 0x00000000,
    0x00000000, 0x00000000, 0x00130040, 0x7b052660,
    0x06467a05, 0x00441126, 0x00031a61, 0x09260220,
    0x00346805, 0x00000000, 0x00131a61, 0x0b260220,
    0x00347b05, 0x00000000, 0x00040025, 0x00004600,
    0x00000000, 0x000001d8, 0xa07c0040, 0x73001b02,
    0x277e1970, 0x1b007c03, 0x00030061, 0x05060220,
    0x00347c05, 0x00000000, 0x00130061, 0x07060220,
    0x00347d05, 0x00000000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0xa0010940, 0x1e027e02,
    0x00031961, 0x05260220, 0x00340105, 0x00000000,
    0x00131a61, 0x07260220, 0x00340205, 0x00000000,
    0xe213004c, 0x00114004, 0x80001969, 0x10018220,
    0x02001304, 0x00000003, 0x80000961, 0x78060660,
    0x00010480, 0x00000000, 0x80000061, 0x78260660,
    0x00010490, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x47260220,
    0x00007824, 0x00000000, 0x80031961, 0x47060220,
    0x00007804, 0x00000000, 0xe24e0061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004431, 0x4c140000, 0xfb00470c, 0x00340000,
    0x80001a61, 0x30010220, 0x00004e04, 0x00000000,
    0x80001e69, 0x10018220, 0x02001304, 0x00000003,
    0x80000961, 0x79060660, 0x00010280, 0x00000000,
    0x80000061, 0x79260660, 0x00010290, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80031a61, 0x48260220, 0x00007924, 0x00000000,
    0x80031961, 0x48060220, 0x00007904, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004431, 0x00000000, 0xfb08480c, 0x00344c14,
    0x00040052, 0x0d044160, 0x0e0e0003, 0x0d054305,
    0x00040027, 0x00014060, 0x00000000, 0xfffff038,
    0x80030061, 0x7e050220, 0x00460005, 0x00000000,
    0x80040931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_misc_copy_instances_base_ptr = {
   .prog_data = {
      .base.nr_params = 14,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 5680,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_misc_copy_instances_base_ptr_relocs,
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
   .args = gfx125_misc_copy_instances_base_ptr_args,
   .code = gfx125_misc_copy_instances_base_ptr_code,
};
const char *gfx125_misc_copy_instances_base_ptr_sha1 = "9974b65708ac20bc1498e7de4ef033a2a89cfbe4";
