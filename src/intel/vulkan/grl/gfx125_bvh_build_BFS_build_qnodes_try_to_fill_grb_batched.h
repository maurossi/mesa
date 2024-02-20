#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_build_BFS_build_qnodes_try_to_fill_grb_batched_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_build_BFS_build_qnodes_try_to_fill_grb_batched_args[] = {
   { 0, 8 },
   { 8, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g46<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g11<1>UD        g0.1<0,1,0>UD                   { align1 1H };
add(1)          g47<1>UD        g46<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@2 compacted };
mov(8)          g30<1>UW        0x76543210V                     { align1 WE_all 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(1)         g1UD            g47UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
add(8)          g30.8<1>UW      g30<1,1,0>UW    0x0008UW        { align1 WE_all 1Q I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g34.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g36.1<2>F       g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g38.1<2>F       g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g40.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g34<2>F         g2<0,1,0>F                      { align1 1Q F@4 compacted };
mov(8)          g36<2>F         g2<0,1,0>F                      { align1 2Q F@4 compacted };
mov(8)          g38<2>F         g2.2<0,1,0>F                    { align1 1Q F@4 compacted };
mov(8)          g40<2>F         g2.2<0,1,0>F                    { align1 2Q F@4 compacted };
mov(8)          g54<1>UD        g34.1<8,4,2>UD                  { align1 1Q F@4 };
mov(8)          g55<1>UD        g36.1<8,4,2>UD                  { align1 2Q F@3 };
mov(8)          g42<1>UD        g38.1<8,4,2>UD                  { align1 1Q F@2 };
add(8)          g44<1>D         g38<8,4,2>D     36D             { align1 1Q compacted };
mov(8)          g43<1>UD        g40.1<8,4,2>UD                  { align1 2Q F@1 };
add(8)          g45<1>D         g40<8,4,2>D     36D             { align1 2Q compacted };
cmp.l.f0.0(8)   g46<1>UD        g44<8,8,1>UD    g38<8,4,2>UD    { align1 1Q I@3 };
mov(8)          g13<2>UD        g44<4,4,1>UD                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
cmp.l.f0.0(8)   g47<1>UD        g45<8,8,1>UD    g40<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g15<2>UD        g45<4,4,1>UD                    { align1 2Q };
add(8)          g48<1>D         -g46<8,8,1>D    g38.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g60<1>D         -g47<8,8,1>D    g40.1<8,4,2>D   { align1 2Q I@3 };
mov(8)          g13.1<2>UD      g48<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g15.1<2>UD      g60<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g60UD           g13UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
add(16)         g24<1>D         g60<1,1,0>D     g11<1,1,0>D     { align1 1H $1.dst compacted };
mul(8)          acc0<1>UD       g24<8,8,1>UD    0x0058UW        { align1 1Q I@1 };
mul(16)         g52<1>D         g24<1,1,0>D     88W             { align1 1H I@2 compacted };
mach(8)         g61<1>UD        g24<1,1,0>UD    0x00000058UD    { align1 1Q compacted AccWrEnable };
add(8)          g56<1>D         g34<8,4,2>D     g52<1,1,0>D     { align1 1Q I@2 compacted };
add(8)          g57<1>D         g36<8,4,2>D     g53<1,1,0>D     { align1 2Q I@3 compacted };
mul(8)          acc0<1>UD       g25<8,8,1>UD    0x0058UW        { align1 2Q };
cmp.l.f0.0(8)   g58<1>UD        g56<8,8,1>UD    g34<8,4,2>UD    { align1 1Q I@3 };
cmp.l.f0.0(8)   g59<1>UD        g57<8,8,1>UD    g36<8,4,2>UD    { align1 2Q I@3 };
add(16)         g50<1>D         g56<1,1,0>D     40D             { align1 1H compacted };
add(16)         g67<1>D         g56<1,1,0>D     64D             { align1 1H compacted };
add(16)         g73<1>D         g56<1,1,0>D     72D             { align1 1H compacted };
mach(8)         g62<1>UD        g25<8,8,1>UD    0x00000058UD    { align1 2Q AccWrEnable };
cmp.l.f0.0(16)  g63<1>UD        g50<1,1,0>UD    g56<1,1,0>UD    { align1 1H I@4 compacted };
mov(8)          g3<2>UD         g50<4,4,1>UD                    { align1 1Q };
mov(8)          g5<2>UD         g51<4,4,1>UD                    { align1 2Q };
cmp.l.f0.0(16)  g69<1>UD        g67<1,1,0>UD    g56<1,1,0>UD    { align1 1H I@6 compacted };
mov(8)          g7<2>UD         g67<4,4,1>UD                    { align1 1Q };
mov(8)          g9<2>UD         g68<4,4,1>UD                    { align1 2Q };
mov(8)          g26<2>UD        g73<4,4,1>UD                    { align1 1Q I@7 };
mov(8)          g28<2>UD        g74<4,4,1>UD                    { align1 2Q I@7 };
add3(16)        g49<1>D         g54<8,8,1>D     g61<8,8,1>D     -g58<1,1,1>D { align1 1H I@7 };
cmp.l.f0.0(16)  g75<1>UD        g73<1,1,0>UD    g56<1,1,0>UD    { align1 1H compacted };
add(16)         g65<1>D         -g63<1,1,0>D    g49<1,1,0>D     { align1 1H I@2 compacted };
add(16)         g71<1>D         -g69<1,1,0>D    g49<1,1,0>D     { align1 1H I@7 compacted };
add(16)         g77<1>D         -g75<1,1,0>D    g49<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g3.1<2>UD       g65<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g5.1<2>UD       g66<4,4,1>UD                    { align1 2Q I@4 };
mov(8)          g7.1<2>UD       g71<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g9.1<2>UD       g72<4,4,1>UD                    { align1 2Q I@5 };
mov(8)          g26.1<2>UD      g77<4,4,1>UD                    { align1 1Q I@5 };
mov(8)          g28.1<2>UD      g78<4,4,1>UD                    { align1 2Q I@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@5 };
send(16)        g66UD           g3UD            nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g72UD           g7UD            nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g78UD           g26UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.dst };
add.nz.f0.0(16) g18<1>D         g72<1,1,0>D     -g78<1,1,0>D    { align1 1H $3.dst compacted };
(+f0.0) if(16)  JIP:  LABEL0          UIP:  LABEL0              { align1 1H };
mov(16)         g80<1>D         g30<8,8,1>UW                    { align1 1H };
and.z.f0.0(16)  g33<1>UD        g80<1,1,0>UD    0x0000000fUD    { align1 1H I@1 compacted };
mov(16)         g81<1>UD        g33<8,8,1>UD                    { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL1          UIP:  LABEL1              { align1 1H };
add(8)          g83<1>D         g38<8,4,2>D     4D              { align1 1Q compacted };
add(8)          g84<1>D         g40<8,4,2>D     4D              { align1 2Q compacted };
cmp.l.f0.0(8)   g85<1>UD        g83<8,8,1>UD    g38<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g86<1>UD        g84<8,8,1>UD    g40<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g14<2>UD        g83<4,4,1>UD                    { align1 1Q $1.src };
mov(8)          g16<2>UD        g84<4,4,1>UD                    { align1 2Q $1.src };
add(8)          g87<1>D         -g85<8,8,1>D    g38.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g88<1>D         -g86<8,8,1>D    g40.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g14.1<2>UD      g87<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g16.1<2>UD      g88<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g115UD          g14UD           g18UD           0x0824058c                0x00000080
                            ugm MsgDesc: ( atomic_add, a64, d32, V1, L1UC_L3WB dst_len = 2, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $1 };

LABEL1:
endif(16)       JIP:  LABEL0                                    { align1 1H };
add(8)          g88<1>D         g38<8,4,2>D     28D             { align1 1Q compacted };
add(8)          g89<1>D         g40<8,4,2>D     28D             { align1 2Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
add(16)         g95<1>D         g115<0,1,0>D    g18<1,1,0>D     { align1 1H $1.dst compacted };
cmp.l.f0.0(8)   g90<1>UD        g88<8,8,1>UD    g38<8,4,2>UD    { align1 1Q I@3 };
cmp.l.f0.0(8)   g91<1>UD        g89<8,8,1>UD    g40<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g44<2>UD        g88<4,4,1>UD                    { align1 1Q };
mov(8)          g46<2>UD        g89<4,4,1>UD                    { align1 2Q };
add(8)          g92<1>D         -g90<8,8,1>D    g38.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g93<1>D         -g91<8,8,1>D    g40.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g44.1<2>UD      g92<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g46.1<2>UD      g93<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g93UD           g44UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
add(16)         g97<1>D         g93<1,1,0>D     -g115<0,1,0>D   { align1 1H $4.dst compacted };
cmp.ge.f0.0(16) null<1>UD       g95<8,8,1>UD    g93<8,8,1>UD    { align1 1H I@7 };
(+f0.0) sel(16) g99<1>UD        g97<1,1,0>UD    g18<1,1,0>UD    { align1 1H I@2 compacted };
cmp.l.f0.0(16)  null<1>UD       g115<0,1,0>UD   g93<8,8,1>UD    { align1 1H };
(+f0.0) if(16)  JIP:  LABEL2          UIP:  LABEL2              { align1 1H };
add(8)          g101<1>D        g38<8,4,2>D     40D             { align1 1Q compacted };
add(8)          g102<1>D        g40<8,4,2>D     40D             { align1 2Q compacted };
add(8)          g107<1>D        g38<8,4,2>D     44D             { align1 1Q compacted };
add(8)          g108<1>D        g40<8,4,2>D     44D             { align1 2Q compacted };
cmp.l.f0.0(8)   g103<1>UD       g101<8,8,1>UD   g38<8,4,2>UD    { align1 1Q I@4 };
cmp.l.f0.0(8)   g104<1>UD       g102<8,8,1>UD   g40<8,4,2>UD    { align1 2Q I@4 };
mov(8)          g12<2>UD        g101<4,4,1>UD                   { align1 1Q $1.src };
mov(8)          g14<2>UD        g102<4,4,1>UD                   { align1 2Q $1.src };
cmp.l.f0.0(16)  g109<1>UD       g107<1,1,0>UD   0x0000002cUD    { align1 1H I@5 compacted };
add(8)          g105<1>D        -g103<8,8,1>D   g38.1<8,4,2>D   { align1 1Q I@5 };
add(8)          g106<1>D        -g104<8,8,1>D   g40.1<8,4,2>D   { align1 2Q I@5 };
mov(8)          g12.1<2>UD      g105<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g14.1<2>UD      g106<4,4,1>UD                   { align1 2Q I@2 };

LABEL4:
cmp.ge.f0.0(16) null<1>UD       g33<8,8,1>UD    g99<8,8,1>UD    { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL3        UIP:  LABEL3              { align1 1H };
add(16)         g111<1>D        g78<1,1,0>D     g33<1,1,0>D     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
send(16)        g122UD          g12UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
shl(16)         g113<1>D        g111<8,8,1>D    0x00000003UD    { align1 1H I@1 };
shr(16)         g31<1>UD        g111<1,1,0>UD   0x0000001dUD    { align1 1H compacted };
add(16)         g117<1>D        g66<1,1,0>D     g113<1,1,0>D    { align1 1H @2 $2.dst compacted };
cmp.l.f0.0(16)  g119<1>UD       g117<1,1,0>UD   g66<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g8<2>UD         g117<4,4,1>UD                   { align1 1Q $3.src };
mov(8)          g10<2>UD        g118<4,4,1>UD                   { align1 2Q $3.src };
cmp.nz.f0.0(16) null<1>D        g122<8,8,1>D    0D              { align1 1H $1.dst };
add3(16)        g121<1>D        g68<8,8,1>D     g31<8,8,1>D     -g119<1,1,1>D { align1 1H @4 $2.dst };
(-f0.0) sel(16) g123<1>UD       g93<8,8,1>UD    0x00000000UD    { align1 1H };
mov(8)          g8.1<2>UD       g121<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g10.1<2>UD      g122<4,4,1>UD                   { align1 2Q I@3 };
add3(16)        g125<1>D        g123<8,8,1>D    g115.0<0,1,0>D  g33<1,1,1>D { align1 1H I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g20UD           g8UD            nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
shl(16)         g1<1>D          g125<8,8,1>D    0x00000004UD    { align1 1H A@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g3<1>D          g107<1,1,0>D    g1<1,1,0>D      { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
cmp.l.f0.0(16)  g5<1>UD         g3<1,1,0>UD     g107<1,1,0>UD   { align1 1H I@1 compacted };
mov(8)          g16<2>UD        g3<4,4,1>UD                     { align1 1Q $1.src };
mov(8)          g18<2>UD        g4<4,4,1>UD                     { align1 2Q $1.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
add3(16)        g7<1>D          -g109<8,8,1>D   g42<8,8,1>D     -g5<1,1,1>D { align1 1H I@3 };
mov(8)          g16.1<2>UD      g7<4,4,1>UD                     { align1 1Q I@1 };
mov(8)          g18.1<2>UD      g8<4,4,1>UD                     { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g16UD           g20UD           0x08007586                0x00000180
                            ugm MsgDesc: ( store_cmask, a64, d32, xyz, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 6 flat )  base_offset 0  { align1 1H $1 };
add(16)         g33<1>D         g33<1,1,0>D     16D             { align1 1H compacted };

LABEL3:
while(16)       JIP:  LABEL4                                    { align1 1H };
cmp.z.f0.0(16)  null<1>D        g81<8,8,1>D     0D              { align1 1H };
(+f0.0) if(16)  JIP:  LABEL5          UIP:  LABEL5              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
send(16)        g8UD            g26UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add(16)         g45<1>D         g8<1,1,0>D      g99<1,1,0>D     { align1 1H @4 $5.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g26UD           g45UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $4 };

LABEL5:
endif(16)       JIP:  LABEL2                                    { align1 1H };

LABEL2:
endif(16)       JIP:  LABEL0                                    { align1 1H };

LABEL0:
endif(16)       JIP:  LABEL6                                    { align1 1H };

LABEL6:
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_build_BFS_build_qnodes_try_to_fill_grb_batched_code[] = {
    0x80000065, 0x2e058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x0b050220, 0x00000024, 0x00000000,
    0xe22f1a40, 0x00012e03, 0x80030061, 0x1e054410,
    0x00000000, 0x76543210, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa002f0c, 0x00340000, 0x641e1940, 0x00801e95,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x21220061, 0x001102cc, 0x2a240061, 0x001102cc,
    0x00030061, 0x26260aa0, 0x00000264, 0x00000000,
    0x00130061, 0x28260aa0, 0x00000264, 0x00000000,
    0x21221461, 0x00110204, 0x2a241461, 0x00110204,
    0x21261461, 0x00110244, 0x2a281461, 0x00110244,
    0x00031461, 0x36050220, 0x00442226, 0x00000000,
    0x00131361, 0x37050220, 0x00442426, 0x00000000,
    0x00031261, 0x2a050220, 0x00442626, 0x00000000,
    0xa12c0040, 0x024e2603, 0x00131161, 0x2b050220,
    0x00442826, 0x00000000, 0xaa2d0040, 0x024e2803,
    0x00031b70, 0x2e050220, 0x52462c05, 0x00442606,
    0x00030061, 0x0d060220, 0x00342c05, 0x00000000,
    0x80103001, 0x00000000, 0x00000000, 0x00000000,
    0x00131b70, 0x2f050220, 0x52462d05, 0x00442806,
    0x00130061, 0x0f060220, 0x00342d05, 0x00000000,
    0x00031c40, 0x30052660, 0x06462e05, 0x00442626,
    0x00131b40, 0x3c052660, 0x06462f05, 0x00442826,
    0x00031a61, 0x0d260220, 0x00343005, 0x00000000,
    0x00131a61, 0x0f260220, 0x00343c05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x3c140000, 0xfb040d24, 0x00040000,
    0xa0182140, 0x0b003c02, 0x00031941, 0x20018220,
    0x01461805, 0x00580058, 0x60341a41, 0x05801802,
    0xfe3d0049, 0x05801803, 0xa1381a40, 0x340e2202,
    0xaa391b40, 0x350e2402, 0x00130041, 0x20018220,
    0x01461905, 0x00580058, 0x00031b70, 0x3a050220,
    0x52463805, 0x00442206, 0x00131b70, 0x3b050220,
    0x52463905, 0x00442406, 0xa0320040, 0x02803803,
    0xa0430040, 0x04003803, 0xa0490040, 0x04803803,
    0x00130049, 0x3e058222, 0x02461905, 0x00000058,
    0x273f1c70, 0x38003203, 0x00030061, 0x03060220,
    0x00343205, 0x00000000, 0x00130061, 0x05060220,
    0x00343305, 0x00000000, 0x27451e70, 0x38004303,
    0x00030061, 0x07060220, 0x00344305, 0x00000000,
    0x00130061, 0x09060220, 0x00344405, 0x00000000,
    0x00031f61, 0x1a060220, 0x00344905, 0x00000000,
    0x00131f61, 0x1c060220, 0x00344a05, 0x00000000,
    0x00041f52, 0x31040e68, 0x0e2e3605, 0x3a053d05,
    0x274b0070, 0x38004903, 0xa0411a40, 0x31023f02,
    0xa0471f40, 0x31024502, 0xa04d1b40, 0x31024b02,
    0x00031b61, 0x03260220, 0x00344105, 0x00000000,
    0x00131c61, 0x05260220, 0x00344205, 0x00000000,
    0x00031c61, 0x07260220, 0x00344705, 0x00000000,
    0x00131d61, 0x09260220, 0x00344805, 0x00000000,
    0x00031d61, 0x1a260220, 0x00344d05, 0x00000000,
    0x00131e61, 0x1c260220, 0x00344e05, 0x00000000,
    0x80001d01, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x42240000, 0xfb040324, 0x000c0000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x48140000, 0xfb040724, 0x00040000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x4e140000, 0xfb041a24, 0x00040000,
    0x80002401, 0x00000000, 0x00000000, 0x00000000,
    0xae122340, 0x4e204802, 0x01040022, 0x0001c060,
    0x00000598, 0x00000598, 0x00040061, 0x50050160,
    0x00461e05, 0x00000000, 0xec211965, 0x00f05003,
    0x00041961, 0x51050220, 0x00462105, 0x00000000,
    0x01040022, 0x0001c060, 0x000000c0, 0x000000c0,
    0xa1530040, 0x004e2603, 0xaa540040, 0x004e2803,
    0x00031a70, 0x55050220, 0x52465305, 0x00442606,
    0x00131a70, 0x56050220, 0x52465405, 0x00442806,
    0x00033161, 0x0e060220, 0x00345305, 0x00000000,
    0x00133161, 0x10060220, 0x00345405, 0x00000000,
    0x00031c40, 0x57052660, 0x06465505, 0x00442626,
    0x00131c40, 0x58052660, 0x06465605, 0x00442826,
    0x00031a61, 0x0e260220, 0x00345705, 0x00000000,
    0x00131a61, 0x10260220, 0x00345805, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x73140000, 0xfb180e24, 0x01001214,
    0x00040025, 0x00004600, 0x00000000, 0x000004a0,
    0xa1580040, 0x01ce2603, 0xaa590040, 0x01ce2803,
    0x80001f01, 0x00000000, 0x00000000, 0x00000000,
    0xa05f2140, 0x12017302, 0x00031b70, 0x5a050220,
    0x52465805, 0x00442606, 0x00131b70, 0x5b050220,
    0x52465905, 0x00442806, 0x00030061, 0x2c060220,
    0x00345805, 0x00000000, 0x00130061, 0x2e060220,
    0x00345905, 0x00000000, 0x00031c40, 0x5c052660,
    0x06465a05, 0x00442626, 0x00131c40, 0x5d052660,
    0x06465b05, 0x00442826, 0x00031a61, 0x2c260220,
    0x00345c05, 0x00000000, 0x00131a61, 0x2e260220,
    0x00345d05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044431, 0x5d140000,
    0xfb042c24, 0x00040000, 0xa0612440, 0x73305d02,
    0x00041f70, 0x00010220, 0x42465f05, 0x00465d05,
    0x2f631a62, 0x12006103, 0x00040070, 0x00010220,
    0x52007304, 0x00465d05, 0x01040022, 0x0001c060,
    0x00000388, 0x00000388, 0xa1650040, 0x028e2603,
    0xaa660040, 0x028e2803, 0xa16b0040, 0x02ce2603,
    0xaa6c0040, 0x02ce2803, 0x00031c70, 0x67050220,
    0x52466505, 0x00442606, 0x00131c70, 0x68050220,
    0x52466605, 0x00442806, 0x00033161, 0x0c060220,
    0x00346505, 0x00000000, 0x00133161, 0x0e060220,
    0x00346605, 0x00000000, 0xe76d1d70, 0x02c06b03,
    0x00031d40, 0x69052660, 0x06466705, 0x00442626,
    0x00131d40, 0x6a052660, 0x06466805, 0x00442826,
    0x00031a61, 0x0c260220, 0x00346905, 0x00000000,
    0x00131a61, 0x0e260220, 0x00346a05, 0x00000000,
    0x00041a70, 0x00010220, 0x42462105, 0x00466305,
    0x01040028, 0x0001c660, 0x000001f8, 0x000001f8,
    0xa06f0040, 0x21004e02, 0x80001c01, 0x00000000,
    0x00000000, 0x00000000, 0x00044131, 0x7a140000,
    0xfb040c24, 0x00040000, 0x00041969, 0x71058660,
    0x02466f05, 0x00000003, 0xe01f0068, 0x01d06f03,
    0xa075a240, 0x71004202, 0x27771970, 0x42007503,
    0x00033361, 0x08060220, 0x00347505, 0x00000000,
    0x00133361, 0x0a060220, 0x00347605, 0x00000000,
    0x00042170, 0x00018660, 0x26467a05, 0x00000000,
    0x0004c252, 0x79040e68, 0x0e2e4405, 0x77051f05,
    0x11040062, 0x7b058220, 0x02465d05, 0x00000000,
    0x00031a61, 0x08260220, 0x00347905, 0x00000000,
    0x00131b61, 0x0a260220, 0x00347a05, 0x00000000,
    0x00041b52, 0x7d040e68, 0x06067b05, 0x21057304,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x14240000, 0xfb040824, 0x000c0000,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x00040969, 0x01058660, 0x02467d05, 0x00000004,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0xa0031940, 0x01006b02, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x27051970, 0x6b000303,
    0x00033161, 0x10060220, 0x00340305, 0x00000000,
    0x00133161, 0x12060220, 0x00340405, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x00041b52, 0x07042e68, 0x0e2e6d05, 0x05052a05,
    0x00031961, 0x10260220, 0x00340705, 0x00000000,
    0x00131a61, 0x12260220, 0x00340805, 0x00000000,
    0x80002301, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x00000000, 0xfb0c1024, 0x001c1434,
    0xa0210040, 0x01002103, 0x00040027, 0x00014060,
    0x00000000, 0xfffffdf8, 0x00040070, 0x00018660,
    0x16465105, 0x00000000, 0x01040022, 0x0001c060,
    0x00000098, 0x00000098, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x80001f01, 0x00000000,
    0x00000000, 0x00000000, 0x00044531, 0x08140000,
    0xfb041a24, 0x00040000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0xa02dc540, 0x63000802,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x00000000, 0xfb0c1a24, 0x00042d14,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x80030061, 0x7e050220, 0x00460005, 0x00000000,
    0x80040931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_build_BFS_build_qnodes_try_to_fill_grb_batched = {
   .prog_data = {
      .base.nr_params = 12,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 2448,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_build_BFS_build_qnodes_try_to_fill_grb_batched_relocs,
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
   .args = gfx125_bvh_build_BFS_build_qnodes_try_to_fill_grb_batched_args,
   .code = gfx125_bvh_build_BFS_build_qnodes_try_to_fill_grb_batched_code,
};
const char *gfx125_bvh_build_BFS_build_qnodes_try_to_fill_grb_batched_sha1 = "ce091e166d897fee5beb330a5f3db6ab5de8148c";
