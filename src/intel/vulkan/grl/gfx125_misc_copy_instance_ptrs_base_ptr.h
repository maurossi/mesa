#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_misc_copy_instance_ptrs_base_ptr_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_misc_copy_instance_ptrs_base_ptr_args[] = {
   { 0, 8 },
   { 8, 8 },
   { 16, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g109<1>UD       g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g41<1>UD        g0.1<0,1,0>UD                   { align1 1H };
add(1)          g110<1>UD       g109<0,1,0>UD   0x00000000UD    { align1 WE_all 1N I@2 compacted };
mov(16)         g5<1>UD         g41<8,8,1>UD                    { align1 1H I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(1)         g1UD            g110UD          nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g59.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g7.1<2>F        g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g3.1<2>F        g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g9.1<2>F        g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g61.1<2>F       g2.5<0,1,0>F                    { align1 1Q };
mov(8)          g11.1<2>F       g2.5<0,1,0>F                    { align1 2Q };
mov(8)          g59<2>F         g2<0,1,0>F                      { align1 1Q F@6 compacted };
mov(8)          g7<2>F          g2<0,1,0>F                      { align1 2Q F@6 compacted };
mov(8)          g3<2>F          g2.2<0,1,0>F                    { align1 1Q F@6 compacted };
mov(8)          g9<2>F          g2.2<0,1,0>F                    { align1 2Q F@6 compacted };
mov(8)          g61<2>F         g2.4<0,1,0>F                    { align1 1Q F@6 compacted };
mov(8)          g11<2>F         g2.4<0,1,0>F                    { align1 2Q F@6 compacted };
add(8)          g13<1>D         g59<8,4,2>D     152D            { align1 1Q F@6 compacted };
add(8)          g14<1>D         g7<8,4,2>D      152D            { align1 2Q F@5 compacted };
shr(8)          g24<1>UD        g61<8,4,2>UD    0x00000008UD    { align1 1Q F@2 compacted };
and(8)          g26<1>UD        g61<8,4,2>UD    0xffffff00UD    { align1 1Q };
shr(8)          g25<1>UD        g11<8,4,2>UD    0x00000008UD    { align1 2Q F@1 compacted };
and(8)          g27<1>UD        g11<8,4,2>UD    0xffffff00UD    { align1 2Q };
mov(8)          g45<2>UD        g13<4,4,1>UD                    { align1 1Q I@6 };
cmp.l.f0.0(16)  g15<1>UD        g13<1,1,0>UD    0x00000098UD    { align1 1H I@6 compacted };
mov(8)          g47<2>UD        g14<4,4,1>UD                    { align1 2Q };
add(8)          g28<1>D         g61<8,4,2>D     -g26<1,1,0>D    { align1 1Q I@6 compacted };
add3(16)        g34<1>D         65534W          -g24<8,8,1>D    g41<1,1,1>D { align1 1H I@6 };
add(8)          g29<1>D         g11<8,4,2>D     -g27<1,1,0>D    { align1 2Q I@6 compacted };
add(8)          g62<1>D         -g15<8,8,1>D    g59.1<8,4,2>D   { align1 1Q I@5 };
add(8)          g16<1>D         -g16<8,8,1>D    g7.1<8,4,2>D    { align1 2Q I@6 };
mov(16)         g36<1>D         -g34<8,8,1>D                    { align1 1H I@4 };
and(16)         g30<1>UD        g28<8,8,1>UD    0xffffffc0UD    { align1 1H I@4 };
mov(8)          g45.1<2>UD      g62<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g47.1<2>UD      g16<4,4,1>UD                    { align1 2Q I@4 };
shr(16)         g32<1>UD        g30<1,1,0>UD    0x00000006UD    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g17UD           g45UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
add(8)          g19<1>D         g59<8,4,2>D     g17<1,1,0>D     { align1 1Q $1.dst compacted };
add(8)          g20<1>D         g7<8,4,2>D      g18<1,1,0>D     { align1 2Q $1.dst compacted };
cmp.l.f0.0(8)   g63<1>UD        g19<8,8,1>UD    g59<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g21<1>UD        g20<8,8,1>UD    g7<8,4,2>UD     { align1 2Q I@2 };
cmp.l.f0.0(16)  null<1>UD       g36<8,8,1>UD    g32<8,8,1>UD    { align1 1H I@5 };
add(8)          g22<1>D         -g63<8,8,1>D    g59.1<8,4,2>D   { align1 1Q I@3 };
add(8)          g23<1>D         -g21<8,8,1>D    g7.1<8,4,2>D    { align1 2Q I@3 };
(+f0.0) if(16)  JIP:  LABEL0          UIP:  LABEL0              { align1 1H };
shl(16)         g37<1>D         -g34<8,8,1>D    0x00000006UD    { align1 1H };
add(16)         g39<1>D         g26<1,1,0>D     g37<1,1,0>D     { align1 1H I@1 compacted };
shr(16)         g41<1>UD        g39<1,1,0>UD    0x00000006UD    { align1 1H I@1 compacted };
add(16)         g55<1>D         g19<1,1,0>D     g39<1,1,0>D     { align1 1H compacted };
shl(16)         g43<1>D         g41<8,8,1>D     0x00000003UD    { align1 1H I@2 };
mov(8)          g61<2>UD        g55<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g63<2>UD        g56<4,4,1>UD                    { align1 2Q I@3 };
add(8)          g71<1>D         g3<8,4,2>D      g43<1,1,0>D     { align1 1Q A@3 compacted };
add(8)          g44<1>D         g9<8,4,2>D      g44<1,1,0>D     { align1 2Q A@3 compacted };
cmp.l.f0.0(8)   g72<1>UD        g71<8,8,1>UD    g3<8,4,2>UD     { align1 1Q I@2 };
mov(8)          g67<2>UD        g71<4,4,1>UD                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $1.src };
cmp.l.f0.0(8)   g45<1>UD        g44<8,8,1>UD    g9<8,4,2>UD     { align1 2Q I@3 };
mov(8)          g69<2>UD        g44<4,4,1>UD                    { align1 2Q };
cmp.l.f0.0(16)  g57<1>UD        g55<1,1,0>UD    g19<1,1,0>UD    { align1 1H compacted };
add(8)          g73<1>D         -g72<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $1.src };
add(8)          g46<1>D         -g45<8,8,1>D    g9.1<8,4,2>D    { align1 2Q I@4 };
add(16)         g59<1>D         -g57<1,1,0>D    g22<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g67.1<2>UD      g73<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g69.1<2>UD      g46<4,4,1>UD                    { align1 2Q I@3 };
mov(8)          g61.1<2>UD      g59<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g63.1<2>UD      g60<4,4,1>UD                    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g47UD           g67UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
mov(8)          g51<2>UD        g47<4,4,1>UD                    { align1 1Q $2.dst };
mov(8)          g53<2>UD        g48<4,4,1>UD                    { align1 2Q $2.dst };
mov(8)          g51.1<2>UD      g49<4,4,1>UD                    { align1 1Q @2 $2.dst };
mov(8)          g53.1<2>UD      g50<4,4,1>UD                    { align1 2Q @2 $2.dst };
fbl(1)          g65<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g65<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000600UD    { align1 WE_all 1N A@1 };
mov(1)          g94<2>D         g[a0 96]<0,1,0>D                { align1 WE_all 1N A@1 };
mov(1)          g94.1<2>D       g[a0 100]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g68.1<2>UD      g94.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g68<2>UD        g94<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g58<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g73UD    g68UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $3 };
mov(1)          f0<1>UD         g58<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g65<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000600UD    { align1 WE_all 1N A@1 };
mov(1)          g95<2>D         g[a0 416]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g95.1<2>D       g[a0 420]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g69.1<2>UD      g95.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g69<2>UD        g95<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g69UD           g73UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $4 };

LABEL0:
endif(16)       JIP:  LABEL1                                    { align1 1H };

LABEL1:
shr(16)         g62<1>UD        g26<1,1,0>UD    0x00000008UD    { align1 1H compacted };

LABEL3:
cmp.ge.f0.0(16) null<1>UD       g5<8,8,1>UD     g62<8,8,1>UD    { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL2        UIP:  LABEL2              { align1 1H };
shl(16)         g64<1>D         g5<8,8,1>D      0x00000008UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
shr(16)         g66<1>UD        g64<1,1,0>UD    0x00000006UD    { align1 1H I@1 compacted };
add(16)         g80<1>D         g19<1,1,0>D     g64<1,1,0>D     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
shl(16)         g68<1>D         g66<8,8,1>D     0x00000003UD    { align1 1H I@2 };
mov(8)          g86<2>UD        g80<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g88<2>UD        g81<4,4,1>UD                    { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add(8)          g74<1>D         g3<8,4,2>D      g68<1,1,0>D     { align1 1Q A@3 compacted };
add(8)          g69<1>D         g9<8,4,2>D      g69<1,1,0>D     { align1 2Q A@3 compacted };
cmp.l.f0.0(8)   g75<1>UD        g74<8,8,1>UD    g3<8,4,2>UD     { align1 1Q I@2 };
mov(8)          g92<2>UD        g74<4,4,1>UD                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@3 };
cmp.l.f0.0(8)   g70<1>UD        g69<8,8,1>UD    g9<8,4,2>UD     { align1 2Q };
mov(8)          g94<2>UD        g69<4,4,1>UD                    { align1 2Q };
cmp.l.f0.0(16)  g82<1>UD        g80<1,1,0>UD    g19<1,1,0>UD    { align1 1H compacted };
add(8)          g84<1>D         -g75<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@5 };
add(8)          g71<1>D         -g70<8,8,1>D    g9.1<8,4,2>D    { align1 2Q I@4 };
mov(8)          g92.1<2>UD      g84<4,4,1>UD                    { align1 1Q I@2 };
add(16)         g84<1>D         -g82<1,1,0>D    g22<1,1,0>D     { align1 1H I@4 compacted };
mov(8)          g94.1<2>UD      g71<4,4,1>UD                    { align1 2Q I@3 };
mov(8)          g86.1<2>UD      g84<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g88.1<2>UD      g85<4,4,1>UD                    { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g72UD           g92UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $5 };
mov(8)          g76<2>UD        g72<4,4,1>UD                    { align1 1Q $5.dst };
mov(8)          g78<2>UD        g73<4,4,1>UD                    { align1 2Q $5.dst };
mov(8)          g76.1<2>UD      g74<4,4,1>UD                    { align1 1Q @2 $5.dst };
mov(8)          g78.1<2>UD      g75<4,4,1>UD                    { align1 2Q @2 $5.dst };
fbl(1)          g90<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g90<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000800UD    { align1 WE_all 1N A@1 };
mov(1)          g97<2>D         g[a0 384]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g97.1<2>D       g[a0 388]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(8)          g93.1<2>UD      g97.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g93<2>UD        g97<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g57<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g98UD    g93UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $6 };
mov(1)          f0<1>UD         g57<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g90<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000a00UD    { align1 WE_all 1N A@1 };
mov(1)          g100<2>D        g[a0 192]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g100.1<2>D      g[a0 196]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(8)          g94.1<2>UD      g100.1<0,1,0>UD                 { align1 WE_all 1Q I@2 };
mov(8)          g94<2>UD        g100<0,1,0>UD                   { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g94UD           g98UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $7 };
add(16)         g87<1>D         g64<1,1,0>D     64D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
shr(16)         g89<1>UD        g87<1,1,0>UD    0x00000006UD    { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@6 };
add(16)         g103<1>D        g19<1,1,0>D     g87<1,1,0>D     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
shl(16)         g91<1>D         g89<8,8,1>D     0x00000003UD    { align1 1H I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
mov(8)          g109<2>UD       g103<4,4,1>UD                   { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@3 };
mov(8)          g111<2>UD       g104<4,4,1>UD                   { align1 2Q };
add(8)          g85<1>D         g3<8,4,2>D      g91<1,1,0>D     { align1 1Q I@3 compacted };
add(8)          g92<1>D         g9<8,4,2>D      g92<1,1,0>D     { align1 2Q I@4 compacted };
mov(8)          g115<2>UD       g85<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g117<2>UD       g92<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $6.src };
cmp.l.f0.0(8)   g93<1>UD        g92<8,8,1>UD    g9<8,4,2>UD     { align1 2Q };
cmp.l.f0.0(8)   g86<1>UD        g85<8,8,1>UD    g3<8,4,2>UD     { align1 1Q };
cmp.l.f0.0(16)  g105<1>UD       g103<1,1,0>UD   g19<1,1,0>UD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $7.src };
add(8)          g94<1>D         -g93<8,8,1>D    g9.1<8,4,2>D    { align1 2Q I@3 };
add(8)          g87<1>D         -g86<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@3 };
add(16)         g107<1>D        -g105<1,1,0>D   g22<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g117.1<2>UD     g94<4,4,1>UD                    { align1 2Q I@3 };
mov(8)          g115.1<2>UD     g87<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g109.1<2>UD     g107<4,4,1>UD                   { align1 1Q I@3 };
mov(8)          g111.1<2>UD     g108<4,4,1>UD                   { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g95UD           g115UD          nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $8 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
mov(8)          g99<2>UD        g95<4,4,1>UD                    { align1 1Q $8.dst };
mov(8)          g101<2>UD       g96<4,4,1>UD                    { align1 2Q $8.dst };
mov(8)          g99.1<2>UD      g97<4,4,1>UD                    { align1 1Q @2 $8.dst };
mov(8)          g101.1<2>UD     g98<4,4,1>UD                    { align1 2Q @2 $8.dst };
fbl(1)          g113<1>UD       mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g113<0,1,0>UD   0x00000003UD    { align1 WE_all 1N I@1 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000c00UD    { align1 WE_all 1N A@1 };
mov(1)          g103<2>D        g[a0 96]<0,1,0>D                { align1 WE_all 1N A@1 };
mov(1)          g103.1<2>D      g[a0 100]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
mov(8)          g116.1<2>UD     g103.1<0,1,0>UD                 { align1 WE_all 1Q I@1 };
mov(8)          g116<2>UD       g103<0,1,0>UD                   { align1 WE_all 1Q I@1 };
mov(1)          g56<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g121UD   g116UD          nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $9 };
mov(1)          f0<1>UD         g56<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g113<0,1,0>UD   0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000c00UD    { align1 WE_all 1N A@1 };
mov(1)          g104<2>D        g[a0 416]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g104.1<2>D      g[a0 420]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
mov(8)          g117.1<2>UD     g104.1<0,1,0>UD                 { align1 WE_all 1Q I@2 };
mov(8)          g117<2>UD       g104<0,1,0>UD                   { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g117UD          g121UD          0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $10 };
add(16)         g110<1>D        g64<1,1,0>D     128D            { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
shr(16)         g112<1>UD       g110<1,1,0>UD   0x00000006UD    { align1 1H I@1 compacted };
add(16)         g126<1>D        g19<1,1,0>D     g110<1,1,0>D    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
shl(16)         g114<1>D        g112<8,8,1>D    0x00000003UD    { align1 1H I@2 };
mov(8)          g11<2>UD        g126<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g13<2>UD        g127<4,4,1>UD                   { align1 2Q I@3 };
add(8)          g88<1>D         g3<8,4,2>D      g114<1,1,0>D    { align1 1Q I@3 compacted };
add(8)          g115<1>D        g9<8,4,2>D      g115<1,1,0>D    { align1 2Q I@4 compacted };
mov(8)          g26<2>UD        g88<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g28<2>UD        g115<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $9.src };
cmp.l.f0.0(8)   g116<1>UD       g115<8,8,1>UD   g9<8,4,2>UD     { align1 2Q };
cmp.l.f0.0(8)   g89<1>UD        g88<8,8,1>UD    g3<8,4,2>UD     { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
cmp.l.f0.0(16)  g1<1>UD         g126<1,1,0>UD   g19<1,1,0>UD    { align1 1H F@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $10.src };
add(8)          g117<1>D        -g116<8,8,1>D   g9.1<8,4,2>D    { align1 2Q I@3 };
add(8)          g90<1>D         -g89<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@3 };
add(16)         g7<1>D          -g1<1,1,0>D     g22<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g28.1<2>UD      g117<4,4,1>UD                   { align1 2Q I@3 };
mov(8)          g26.1<2>UD      g90<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g11.1<2>UD      g7<4,4,1>UD                     { align1 1Q I@3 };
mov(8)          g13.1<2>UD      g8<4,4,1>UD                     { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g118UD          g26UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $11 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
mov(8)          g122<2>UD       g118<4,4,1>UD                   { align1 1Q $11.dst };
mov(8)          g124<2>UD       g119<4,4,1>UD                   { align1 2Q $11.dst };
mov(8)          g122.1<2>UD     g120<4,4,1>UD                   { align1 1Q @2 $11.dst };
mov(8)          g124.1<2>UD     g121<4,4,1>UD                   { align1 2Q @2 $11.dst };
fbl(1)          g15<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g15<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000e00UD    { align1 WE_all 1N A@1 };
mov(1)          g105<2>D        g[a0 320]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g105.1<2>D      g[a0 324]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mov(8)          g27.1<2>UD      g105.1<0,1,0>UD                 { align1 WE_all 1Q I@1 };
mov(8)          g27<2>UD        g105<0,1,0>UD                   { align1 WE_all 1Q I@1 };
mov(1)          g55<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g32UD    g27UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $12 };
mov(1)          f0<1>UD         g55<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g15<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
mov(1)          g106<2>D        g[a0 352]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g106.1<2>D      g[a0 356]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mov(8)          g28.1<2>UD      g106.1<0,1,0>UD                 { align1 WE_all 1Q I@2 };
mov(8)          g28<2>UD        g106<0,1,0>UD                   { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g28UD           g32UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $13 };
add(16)         g12<1>D         g64<1,1,0>D     192D            { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
shr(16)         g14<1>UD        g12<1,1,0>UD    0x00000006UD    { align1 1H I@1 compacted };
add(16)         g34<1>D         g19<1,1,0>D     g12<1,1,0>D     { align1 1H compacted };
shl(16)         g16<1>D         g14<8,8,1>D     0x00000003UD    { align1 1H I@2 };
mov(8)          g40<2>UD        g34<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g42<2>UD        g35<4,4,1>UD                    { align1 2Q I@3 };
add(8)          g91<1>D         g3<8,4,2>D      g16<1,1,0>D     { align1 1Q I@3 compacted };
add(8)          g17<1>D         g9<8,4,2>D      g17<1,1,0>D     { align1 2Q I@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(8)          g46<2>UD        g91<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $1.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@2 };
mov(8)          g48<2>UD        g17<4,4,1>UD                    { align1 2Q };
cmp.l.f0.0(8)   g18<1>UD        g17<8,8,1>UD    g9<8,4,2>UD     { align1 2Q };
cmp.l.f0.0(8)   g92<1>UD        g91<8,8,1>UD    g3<8,4,2>UD     { align1 1Q };
cmp.l.f0.0(16)  g36<1>UD        g34<1,1,0>UD    g19<1,1,0>UD    { align1 1H compacted };
add(8)          g21<1>D         -g18<8,8,1>D    g9.1<8,4,2>D    { align1 2Q I@3 };
add(8)          g93<1>D         -g92<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@3 };
add(16)         g38<1>D         -g36<1,1,0>D    g22<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g48.1<2>UD      g21<4,4,1>UD                    { align1 2Q I@3 };
mov(8)          g46.1<2>UD      g93<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g40.1<2>UD      g38<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g42.1<2>UD      g39<4,4,1>UD                    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g26UD           g46UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
mov(8)          g30<2>UD        g26<4,4,1>UD                    { align1 1Q $1.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $13.src };
mov(8)          g32<2>UD        g27<4,4,1>UD                    { align1 2Q $1.dst };
mov(8)          g30.1<2>UD      g28<4,4,1>UD                    { align1 1Q @2 $1.dst };
mov(8)          g32.1<2>UD      g29<4,4,1>UD                    { align1 2Q @2 $1.dst };
fbl(1)          g44<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g44<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000200UD    { align1 WE_all 1N A@1 };
mov(1)          g107<2>D        g[a0 448]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g107.1<2>D      g[a0 452]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(8)          g47.1<2>UD      g107.1<0,1,0>UD                 { align1 WE_all 1Q I@1 };
mov(8)          g47<2>UD        g107<0,1,0>UD                   { align1 WE_all 1Q I@1 };
mov(1)          g54<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g52UD    g47UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $1 };
mov(1)          f0<1>UD         g54<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g44<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000400UD    { align1 WE_all 1N A@1 };
mov(1)          g108<2>D        g[a0 256]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g108.1<2>D      g[a0 260]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(8)          g48.1<2>UD      g108.1<0,1,0>UD                 { align1 WE_all 1Q I@2 };
mov(8)          g48<2>UD        g108<0,1,0>UD                   { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g48UD           g52UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $1 };
add3(16)        g5<1>D          0x0003UW        g24<8,8,1>D     g5<1,1,1>D { align1 1H };

LABEL2:
while(16)       JIP:  LABEL3                                    { align1 1H };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_misc_copy_instance_ptrs_base_ptr_code[] = {
    0x80000065, 0x6d058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x29050220, 0x00000024, 0x00000000,
    0xe26e1a40, 0x00016d03, 0x00041a61, 0x05050220,
    0x00462905, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa006e0c, 0x00340000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x213b0061, 0x001102cc,
    0x2a070061, 0x001102cc, 0x00030061, 0x03260aa0,
    0x00000264, 0x00000000, 0x00130061, 0x09260aa0,
    0x00000264, 0x00000000, 0x00030061, 0x3d260aa0,
    0x000002a4, 0x00000000, 0x00130061, 0x0b260aa0,
    0x000002a4, 0x00000000, 0x213b1661, 0x00110204,
    0x2a071661, 0x00110204, 0x21031661, 0x00110244,
    0x2a091661, 0x00110244, 0x213d1661, 0x0011025c,
    0x2a0b1661, 0x0011025c, 0xa10d1640, 0x098e3b03,
    0xaa0e1540, 0x098e0703, 0xe1181268, 0x008e3d03,
    0x00030065, 0x1a058220, 0x02443d06, 0xffffff00,
    0xea191168, 0x008e0b03, 0x00130065, 0x1b058220,
    0x02440b06, 0xffffff00, 0x00031e61, 0x2d060220,
    0x00340d05, 0x00000000, 0xe70f1e70, 0x09800d03,
    0x00130061, 0x2f060220, 0x00340e05, 0x00000000,
    0xa11c1e40, 0x1a2e3d02, 0x00041e52, 0x22044560,
    0x0e8efffe, 0x29051805, 0xaa1d1e40, 0x1b2e0b02,
    0x00031d40, 0x3e052660, 0x06460f05, 0x00443b26,
    0x00131e40, 0x10052660, 0x06461005, 0x00440726,
    0x00041c61, 0x24052660, 0x00462205, 0x00000000,
    0x00041c65, 0x1e058220, 0x02461c05, 0xffffffc0,
    0x00031c61, 0x2d260220, 0x00343e05, 0x00000000,
    0x00131c61, 0x2f260220, 0x00341005, 0x00000000,
    0xe0201b68, 0x00601e03, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x00044131, 0x11140000,
    0xfb042d24, 0x00040000, 0xa1132140, 0x110e3b02,
    0xaa142140, 0x120e0702, 0x00031a70, 0x3f050220,
    0x52461305, 0x00443b06, 0x00131a70, 0x15050220,
    0x52461405, 0x00440706, 0x00041d70, 0x00010220,
    0x52462405, 0x00462005, 0x00031b40, 0x16052660,
    0x06463f05, 0x00443b26, 0x00131b40, 0x17052660,
    0x06461505, 0x00440726, 0x01040022, 0x0001c060,
    0x00000338, 0x00000338, 0x00040069, 0x2505a660,
    0x02462205, 0x00000006, 0xa0271940, 0x25001a02,
    0xe0291968, 0x00602703, 0xa0370040, 0x27001302,
    0x00041a69, 0x2b058660, 0x02462905, 0x00000003,
    0x00031a61, 0x3d060220, 0x00343705, 0x00000000,
    0x00131b61, 0x3f060220, 0x00343805, 0x00000000,
    0xa1470b40, 0x2b0e0302, 0xaa2c0b40, 0x2c0e0902,
    0x00031a70, 0x48050220, 0x52464705, 0x00440306,
    0x00030061, 0x43060220, 0x00344705, 0x00000000,
    0x80103101, 0x00000000, 0x00000000, 0x00000000,
    0x00131b70, 0x2d050220, 0x52462c05, 0x00440906,
    0x00130061, 0x45060220, 0x00342c05, 0x00000000,
    0x27390070, 0x13003703, 0x00031d40, 0x49052660,
    0x06464805, 0x00440326, 0x80103101, 0x00000000,
    0x00000000, 0x00000000, 0x00131c40, 0x2e052660,
    0x06462d05, 0x00440926, 0xa03b1b40, 0x16023902,
    0x00031b61, 0x43260220, 0x00344905, 0x00000000,
    0x00131b61, 0x45260220, 0x00342e05, 0x00000000,
    0x00031b61, 0x3d260220, 0x00343b05, 0x00000000,
    0x00131c61, 0x3f260220, 0x00343c05, 0x00000000,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x2f240000, 0xfb044324, 0x000c0000,
    0x00032261, 0x33060220, 0x00342f05, 0x00000000,
    0x00132261, 0x35060220, 0x00343005, 0x00000000,
    0x0003a261, 0x33260220, 0x00343105, 0x00000000,
    0x0013a261, 0x35260220, 0x00343205, 0x00000000,
    0xe241004c, 0x00114004, 0x80001969, 0x10018220,
    0x02004104, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000600, 0x80000961, 0x5e060660,
    0x00010180, 0x00000000, 0x80000061, 0x5e260660,
    0x00010190, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x44260220,
    0x00005e24, 0x00000000, 0x80031961, 0x44060220,
    0x00005e04, 0x00000000, 0xe23a0061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004331, 0x49140000, 0xfb00440c, 0x00340000,
    0x80001a61, 0x30010220, 0x00003a04, 0x00000000,
    0x80001e69, 0x10018220, 0x02004104, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000600,
    0x80000961, 0x5f060660, 0x00010680, 0x00000000,
    0x80000061, 0x5f260660, 0x00010690, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80031a61, 0x45260220, 0x00005f24, 0x00000000,
    0x80031961, 0x45060220, 0x00005f04, 0x00000000,
    0x80002301, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004431, 0x00000000, 0xfb08450c, 0x00344914,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0xe03e0068, 0x00801a03, 0x00041970, 0x00010220,
    0x42460505, 0x00463e05, 0x01040028, 0x0001c660,
    0x00000dc8, 0x00000dc8, 0x00040069, 0x40058660,
    0x02460505, 0x00000008, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0xe0421968, 0x00604003,
    0xa0500040, 0x40001302, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x00041a69, 0x44058660,
    0x02464205, 0x00000003, 0x00031a61, 0x56060220,
    0x00345005, 0x00000000, 0x00131b61, 0x58060220,
    0x00345105, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0xa14a0b40, 0x440e0302,
    0xaa450b40, 0x450e0902, 0x00031a70, 0x4b050220,
    0x52464a05, 0x00440306, 0x00030061, 0x5c060220,
    0x00344a05, 0x00000000, 0x80101b01, 0x00000000,
    0x00000000, 0x00000000, 0x00130070, 0x46050220,
    0x52464505, 0x00440906, 0x00130061, 0x5e060220,
    0x00344505, 0x00000000, 0x27520070, 0x13005003,
    0x00031d40, 0x54052660, 0x06464b05, 0x00440326,
    0x00131c40, 0x47052660, 0x06464605, 0x00440926,
    0x00031a61, 0x5c260220, 0x00345405, 0x00000000,
    0xa0541c40, 0x16025202, 0x00131b61, 0x5e260220,
    0x00344705, 0x00000000, 0x00031a61, 0x56260220,
    0x00345405, 0x00000000, 0x00131b61, 0x58260220,
    0x00345505, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x00044531, 0x48240000,
    0xfb045c24, 0x000c0000, 0x00032561, 0x4c060220,
    0x00344805, 0x00000000, 0x00132561, 0x4e060220,
    0x00344905, 0x00000000, 0x0003a561, 0x4c260220,
    0x00344a05, 0x00000000, 0x0013a561, 0x4e260220,
    0x00344b05, 0x00000000, 0xe25a004c, 0x00114004,
    0x80001969, 0x10018220, 0x02005a04, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000800,
    0x80000961, 0x61060660, 0x00010600, 0x00000000,
    0x80000061, 0x61260660, 0x00010610, 0x00000000,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x80031961, 0x5d260220, 0x00006124, 0x00000000,
    0x80031961, 0x5d060220, 0x00006104, 0x00000000,
    0xe2390061, 0x00113004, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x8a004631, 0x62140000,
    0xfb005d0c, 0x00340000, 0x80001a61, 0x30010220,
    0x00003904, 0x00000000, 0x80001e69, 0x10018220,
    0x02005a04, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000a00, 0x80000961, 0x64060660,
    0x00010300, 0x00000000, 0x80000061, 0x64260660,
    0x00010310, 0x00000000, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x80031a61, 0x5e260220,
    0x00006424, 0x00000000, 0x80031961, 0x5e060220,
    0x00006404, 0x00000000, 0x80002601, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x8a004731, 0x00000000,
    0xfb085e0c, 0x00346214, 0xa0570040, 0x04004003,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x80001d01, 0x00000000, 0x00000000, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0xe0591968, 0x00605703, 0x80001e01, 0x00000000,
    0x00000000, 0x00000000, 0xa0670040, 0x57001302,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x00041a69, 0x5b058660, 0x02465905, 0x00000003,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x6d060220, 0x00346705, 0x00000000,
    0x80101b01, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x6f060220, 0x00346805, 0x00000000,
    0xa1551b40, 0x5b0e0302, 0xaa5c1c40, 0x5c0e0902,
    0x00031a61, 0x73060220, 0x00345505, 0x00000000,
    0x00131a61, 0x75060220, 0x00345c05, 0x00000000,
    0x80103601, 0x00000000, 0x00000000, 0x00000000,
    0x00130070, 0x5d050220, 0x52465c05, 0x00440906,
    0x00030070, 0x56050220, 0x52465505, 0x00440306,
    0x27690070, 0x13006703, 0x80103701, 0x00000000,
    0x00000000, 0x00000000, 0x00131b40, 0x5e052660,
    0x06465d05, 0x00440926, 0x00031b40, 0x57052660,
    0x06465605, 0x00440326, 0xa06b1b40, 0x16026902,
    0x00131b61, 0x75260220, 0x00345e05, 0x00000000,
    0x00031b61, 0x73260220, 0x00345705, 0x00000000,
    0x00031b61, 0x6d260220, 0x00346b05, 0x00000000,
    0x00131c61, 0x6f260220, 0x00346c05, 0x00000000,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00044831, 0x5f240000, 0xfb047324, 0x000c0000,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x00032861, 0x63060220, 0x00345f05, 0x00000000,
    0x00132861, 0x65060220, 0x00346005, 0x00000000,
    0x0003a861, 0x63260220, 0x00346105, 0x00000000,
    0x0013a861, 0x65260220, 0x00346205, 0x00000000,
    0xe271004c, 0x00114004, 0x80001969, 0x10018220,
    0x02007104, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000c00, 0x80000961, 0x67060660,
    0x00010180, 0x00000000, 0x80000061, 0x67260660,
    0x00010190, 0x00000000, 0x80003801, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x74260220,
    0x00006724, 0x00000000, 0x80031961, 0x74060220,
    0x00006704, 0x00000000, 0xe2380061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004931, 0x79140000, 0xfb00740c, 0x00340000,
    0x80001a61, 0x30010220, 0x00003804, 0x00000000,
    0x80001e69, 0x10018220, 0x02007104, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000c00,
    0x80000961, 0x68060660, 0x00010680, 0x00000000,
    0x80000061, 0x68260660, 0x00010690, 0x00000000,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0x80031a61, 0x75260220, 0x00006824, 0x00000000,
    0x80031961, 0x75060220, 0x00006804, 0x00000000,
    0x80002901, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004a31, 0x00000000, 0xfb08750c, 0x00347914,
    0xa06e0040, 0x08004003, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0xe0701968, 0x00606e03,
    0xa07e0040, 0x6e001302, 0x80003801, 0x00000000,
    0x00000000, 0x00000000, 0x00041a69, 0x72058660,
    0x02467005, 0x00000003, 0x00031a61, 0x0b060220,
    0x00347e05, 0x00000000, 0x00131b61, 0x0d060220,
    0x00347f05, 0x00000000, 0xa1581b40, 0x720e0302,
    0xaa731c40, 0x730e0902, 0x00031a61, 0x1a060220,
    0x00345805, 0x00000000, 0x00131a61, 0x1c060220,
    0x00347305, 0x00000000, 0x80103901, 0x00000000,
    0x00000000, 0x00000000, 0x00130070, 0x74050220,
    0x52467305, 0x00440906, 0x00030070, 0x59050220,
    0x52465805, 0x00440306, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x27011170, 0x13007e03,
    0x80103a01, 0x00000000, 0x00000000, 0x00000000,
    0x00131b40, 0x75052660, 0x06467405, 0x00440926,
    0x00031b40, 0x5a052660, 0x06465905, 0x00440326,
    0xa0071b40, 0x16020102, 0x00131b61, 0x1c260220,
    0x00347505, 0x00000000, 0x00031b61, 0x1a260220,
    0x00345a05, 0x00000000, 0x00031b61, 0x0b260220,
    0x00340705, 0x00000000, 0x00131c61, 0x0d260220,
    0x00340805, 0x00000000, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x00044b31, 0x76240000,
    0xfb041a24, 0x000c0000, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x00032b61, 0x7a060220,
    0x00347605, 0x00000000, 0x00132b61, 0x7c060220,
    0x00347705, 0x00000000, 0x0003ab61, 0x7a260220,
    0x00347805, 0x00000000, 0x0013ab61, 0x7c260220,
    0x00347905, 0x00000000, 0xe20f004c, 0x00114004,
    0x80001969, 0x10018220, 0x02000f04, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000e00,
    0x80000961, 0x69060660, 0x00010500, 0x00000000,
    0x80000061, 0x69260660, 0x00010510, 0x00000000,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0x80031961, 0x1b260220, 0x00006924, 0x00000000,
    0x80031961, 0x1b060220, 0x00006904, 0x00000000,
    0xe2370061, 0x00113004, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x8a004c31, 0x20140000,
    0xfb001b0c, 0x00340000, 0x80001a61, 0x30010220,
    0x00003704, 0x00000000, 0x80001e69, 0x10018220,
    0x02000f04, 0x00000003, 0x80000961, 0x6a060660,
    0x00010580, 0x00000000, 0x80000061, 0x6a260660,
    0x00010590, 0x00000000, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x80031a61, 0x1c260220,
    0x00006a24, 0x00000000, 0x80031961, 0x1c060220,
    0x00006a04, 0x00000000, 0x80002c01, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x8a004d31, 0x00000000,
    0xfb081c0c, 0x00342014, 0xa00c0040, 0x0c004003,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0xe00e1968, 0x00600c03, 0xa0220040, 0x0c001302,
    0x00041a69, 0x10058660, 0x02460e05, 0x00000003,
    0x00031a61, 0x28060220, 0x00342205, 0x00000000,
    0x00131b61, 0x2a060220, 0x00342305, 0x00000000,
    0xa15b1b40, 0x100e0302, 0xaa111c40, 0x110e0902,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x00031a61, 0x2e060220, 0x00345b05, 0x00000000,
    0x80103101, 0x00000000, 0x00000000, 0x00000000,
    0x80101a01, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x30060220, 0x00341105, 0x00000000,
    0x00130070, 0x12050220, 0x52461105, 0x00440906,
    0x00030070, 0x5c050220, 0x52465b05, 0x00440306,
    0x27240070, 0x13002203, 0x00131b40, 0x15052660,
    0x06461205, 0x00440926, 0x00031b40, 0x5d052660,
    0x06465c05, 0x00440326, 0xa0261b40, 0x16022402,
    0x00131b61, 0x30260220, 0x00341505, 0x00000000,
    0x00031b61, 0x2e260220, 0x00345d05, 0x00000000,
    0x00031b61, 0x28260220, 0x00342605, 0x00000000,
    0x00131c61, 0x2a260220, 0x00342705, 0x00000000,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x1a240000, 0xfb042e24, 0x000c0000,
    0x00032161, 0x1e060220, 0x00341a05, 0x00000000,
    0x80103d01, 0x00000000, 0x00000000, 0x00000000,
    0x00132161, 0x20060220, 0x00341b05, 0x00000000,
    0x0003a161, 0x1e260220, 0x00341c05, 0x00000000,
    0x0013a161, 0x20260220, 0x00341d05, 0x00000000,
    0xe22c004c, 0x00114004, 0x80001969, 0x10018220,
    0x02002c04, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000200, 0x80000961, 0x6b060660,
    0x00010700, 0x00000000, 0x80000061, 0x6b260660,
    0x00010710, 0x00000000, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x2f260220,
    0x00006b24, 0x00000000, 0x80031961, 0x2f060220,
    0x00006b04, 0x00000000, 0xe2360061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004131, 0x34140000, 0xfb002f0c, 0x00340000,
    0x80001a61, 0x30010220, 0x00003604, 0x00000000,
    0x80001e69, 0x10018220, 0x02002c04, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000400,
    0x80000961, 0x6c060660, 0x00010400, 0x00000000,
    0x80000061, 0x6c260660, 0x00010410, 0x00000000,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x80031a61, 0x30260220, 0x00006c24, 0x00000000,
    0x80031961, 0x30060220, 0x00006c04, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004131, 0x00000000, 0xfb08300c, 0x00343414,
    0x00040052, 0x05044160, 0x0e0e0003, 0x05051805,
    0x00040027, 0x00014060, 0x00000000, 0xfffff228,
    0x80030061, 0x7e050220, 0x00460005, 0x00000000,
    0x80040931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_misc_copy_instance_ptrs_base_ptr = {
   .prog_data = {
      .base.nr_params = 14,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 5040,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_misc_copy_instance_ptrs_base_ptr_relocs,
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
   .args = gfx125_misc_copy_instance_ptrs_base_ptr_args,
   .code = gfx125_misc_copy_instance_ptrs_base_ptr_code,
};
const char *gfx125_misc_copy_instance_ptrs_base_ptr_sha1 = "885aee4de3bdfdac3ece9c5297fe919ebe0266f5";
