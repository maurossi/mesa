#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_build_BFS_BFS_pass1_initial_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_build_BFS_BFS_pass1_initial_args[] = {
   { 0, 8 },
   { 8, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g104<1>UD       g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
and(16)         g73<1>UD        g0.2<0,1,0>UD   0x000000ffUD    { align1 1H compacted };
add(1)          g105<1>UD       g104<0,1,0>UD   0x00000000UD    { align1 WE_all 1N I@2 compacted };
mov(8)          g39<1>UW        0x76543210V                     { align1 WE_all 1Q };
shl(16)         g75<1>D         g73<8,8,1>D     0x00000004UD    { align1 1H I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(1)         g1UD            g105UD          nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
add(8)          g39.8<1>UW      g39<1,1,0>UW    0x0008UW        { align1 WE_all 1Q I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         g77<1>D         g39<8,8,1>UW                    { align1 1H };
add(16)         g79<1>D         g77<1,1,0>D     g75<1,1,0>D     { align1 1H I@1 compacted };
and(16)         g48<1>UD        g79<1,1,0>UD    0x000001ffUD    { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g38.1<2>F       g2.1<0,1,0>F                    { align1 1Q I@3 compacted };
mov(8)          g51.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g90.1<2>F       g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g38<2>F         g2<0,1,0>F                      { align1 1Q F@3 compacted };
mov(8)          g51<2>F         g2.2<0,1,0>F                    { align1 2Q F@3 compacted };
mov(8)          g90<2>F         g2.2<0,1,0>F                    { align1 1Q F@3 compacted };
add(8)          g53<1>D         g51<8,4,2>D     52D             { align1 2Q F@2 compacted };
add(8)          g57<1>D         g51<8,4,2>D     8D              { align1 2Q compacted };
add(8)          g108<1>D        g90<8,4,2>D     52D             { align1 1Q F@1 compacted };
add(8)          g119<1>D        g90<8,4,2>D     8D              { align1 1Q compacted };
cmp.l.f0.0(8)   g54<1>UD        g53<8,8,1>UD    g51<8,4,2>UD    { align1 2Q I@4 };
cmp.l.f0.0(8)   g58<1>UD        g57<8,8,1>UD    g51<8,4,2>UD    { align1 2Q I@4 };
mov(8)          g44<2>UD        g57<4,4,1>UD                    { align1 2Q };
cmp.l.f0.0(8)   g109<1>UD       g108<8,8,1>UD   g90<8,4,2>UD    { align1 1Q I@5 };
mov(8)          g42<2>UD        g119<4,4,1>UD                   { align1 1Q I@5 };
cmp.l.f0.0(8)   g120<1>UD       g119<8,8,1>UD   g90<8,4,2>UD    { align1 1Q };
add(8)          g55<1>D         -g54<8,8,1>D    g51.1<8,4,2>D   { align1 2Q I@6 };
add(8)          g59<1>D         -g58<8,8,1>D    g51.1<8,4,2>D   { align1 2Q I@6 };
add(8)          g118<1>D        -g109<8,8,1>D   g90.1<8,4,2>D   { align1 1Q I@5 };
add(8)          g121<1>D        -g120<8,8,1>D   g90.1<8,4,2>D   { align1 1Q I@4 };
mov(8)          g44.1<2>UD      g59<4,4,1>UD                    { align1 2Q I@3 };
mov(8)          g50.1<2>F       g2.1<0,1,0>F                    { align1 2Q I@4 compacted };
mov(8)          g42.1<2>UD      g121<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g50<2>F         g2<0,1,0>F                      { align1 2Q F@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g60UD           g42UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
add(16)         g81<1>D         g64<1,1,0>D     32D             { align1 1H $1.dst compacted };
cmp.l.f0.0(16)  g83<1>UD        g81<1,1,0>UD    g64<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g43<2>UD        g81<4,4,1>UD                    { align1 1Q $1.src };
mov(8)          g45<2>UD        g82<4,4,1>UD                    { align1 2Q $1.src };
add(16)         g85<1>D         -g83<1,1,0>D    g66<1,1,0>D     { align1 1H @3 $1.dst compacted };
mov(8)          g43.1<2>UD      g85<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g45.1<2>UD      g86<4,4,1>UD                    { align1 2Q I@2 };
add(16)         g86<1>D         g64<1,1,0>D     48D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g3UD            g43UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
cmp.l.f0.0(16)  g88<1>UD        g86<1,1,0>UD    0x00000030UD    { align1 1H I@1 compacted };
mov(8)          g44<2>UD        g86<4,4,1>UD                    { align1 1Q $2.src };
mov(8)          g46<2>UD        g87<4,4,1>UD                    { align1 2Q $2.src };
add(16)         g90<1>D         -g88<1,1,0>D    g66<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g44.1<2>UD      g90<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g46.1<2>UD      g91<4,4,1>UD                    { align1 2Q I@2 };
add(16)         g92<1>F         g9<1,1,0>F      -g3<1,1,0>F     { align1 1H $2.dst compacted };
sel.ge(16)      g98<1>F         g92<8,8,1>F     0x704ec3dF  /* 1e-34F */ { align1 1H F@1 };
cmp.l.f0.0(16)  g110<1>F        g98<8,8,1>F     0x77f684dfF  /* 1e+34F */ { align1 1H F@1 };
math inv(16)    g101<1>F        g98<8,8,1>F     null<8,8,1>F    { align1 1H $3 };
cmp.g.f0.0(16)  g112<1>F        g92<8,8,1>F     0x704ec3dF  /* 1e-34F */ { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        g91UD           g44UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mul(16)         g103<1>F        g101<8,8,1>F    0x417d70a4F  /* 15.84F */ { align1 1H $3.dst };
and.nz.f0.0(16) null<1>UD       g110<8,8,1>UD   g112<8,8,1>UD   { align1 1H F@2 };
mov(8)          g41<2>UD        g108<4,4,1>UD                   { align1 1Q $1.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $4.src };
mov(8)          g43<2>UD        g53<4,4,1>UD                    { align1 2Q $2.src };
(+f0.0) sel(16) g11<1>UD        g103<1,1,0>UD   0x00000000UD    { align1 1H F@1 compacted };
mov(8)          g41.1<2>UD      g118<4,4,1>UD                   { align1 1Q I@3 };
mov(8)          g43.1<2>UD      g55<4,4,1>UD                    { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g56UD           g41UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
mov(16)         g40<1>UD        g0.1<0,1,0>UD                   { align1 1H $4.src };
shl(16)         g68<1>D         g40<8,8,1>D     0x00000009UD    { align1 1H I@1 };
add(16)         g70<1>D         g68<1,1,0>D     512D            { align1 1H I@1 compacted };
add(16)         g80<1>D         g68<1,1,0>D     g48<1,1,0>D     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.dst };
add(16)         g95<1>F         g91<1,1,0>F     -g5<1,1,0>F     { align1 1H $4.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
add(16)         g97<1>F         g93<1,1,0>F     -g7<1,1,0>F     { align1 1H $4.dst compacted };
cmp.g.f0.0(16)  g113<1>F        g95<8,8,1>F     0x704ec3dF  /* 1e-34F */ { align1 1H A@2 };
sel.ge(16)      g99<1>F         g95<8,8,1>F     0x704ec3dF  /* 1e-34F */ { align1 1H $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
math inv(16)    g104<1>F        g99<8,8,1>F     null<8,8,1>F    { align1 1H $5 };
cmp.l.f0.0(16)  g115<1>F        g99<8,8,1>F     0x77f684dfF  /* 1e+34F */ { align1 1H $5.src };
sel.ge(16)      g100<1>F        g97<8,8,1>F     0x704ec3dF  /* 1e-34F */ { align1 1H F@4 };
sel.l(16)       g72<1>UD        g70<1,1,0>UD    g56<1,1,0>UD    { align1 1H @2 $4.dst compacted };
mul(16)         g106<1>F        g104<8,8,1>F    0x417d70a4F  /* 15.84F */ { align1 1H $5.dst };
and.nz.f0.0(16) null<1>UD       g115<8,8,1>UD   g113<8,8,1>UD   { align1 1H F@3 };
(+f0.0) sel(16) g116<1>UD       g106<1,1,0>UD   0x00000000UD    { align1 1H F@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
math inv(16)    g107<1>F        g100<8,8,1>F    null<8,8,1>F    { align1 1H $6 };
cmp.g.f0.0(16)  g118<1>F        g97<8,8,1>F     0x704ec3dF  /* 1e-34F */ { align1 1H I@7 };
cmp.l.f0.0(16)  g120<1>F        g100<8,8,1>F    0x77f684dfF  /* 1e+34F */ { align1 1H $6.src };
mul(16)         g109<1>F        g107<8,8,1>F    0x417d70a4F  /* 15.84F */ { align1 1H $6.dst };
and.nz.f0.0(16) null<1>UD       g120<8,8,1>UD   g118<8,8,1>UD   { align1 1H F@2 };
(+f0.0) sel(16) g121<1>UD       g109<1,1,0>UD   0x00000000UD    { align1 1H F@1 compacted };
cmp.l.f0.0(16)  g123<1>UD       g80<1,1,0>UD    g72<1,1,0>UD    { align1 1H I@5 compacted };
(+f0.0) if(16)  JIP:  LABEL0          UIP:  LABEL0              { align1 1H };
shl(16)         g125<1>D        g80<8,8,1>D     0x00000005UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
add(16)         g1<1>D          g60<1,1,0>D     g125<1,1,0>D    { align1 1H @1 $1.dst compacted };
shr(16)         g126<1>UD       g80<1,1,0>UD    0x0000001bUD    { align1 1H compacted };
cmp.l.f0.0(16)  g40<1>UD        g1<1,1,0>UD     g60<1,1,0>UD    { align1 1H I@2 compacted };
add(16)         g44<1>D         g1<1,1,0>D      16D             { align1 1H $4.src compacted };
mov(8)          g23<2>UD        g1<4,4,1>UD                     { align1 1Q };
mov(8)          g25<2>UD        g2<4,4,1>UD                     { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add3(16)        g42<1>D         g62<8,8,1>D     g126<8,8,1>D    -g40<1,1,1>D { align1 1H @4 $1.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
cmp.l.f0.0(16)  g46<1>UD        g44<1,1,0>UD    g1<1,1,0>UD     { align1 1H I@4 compacted };
mov(8)          g29<2>UD        g44<4,4,1>UD                    { align1 1Q };
mov(8)          g31<2>UD        g45<4,4,1>UD                    { align1 2Q };
mov(8)          g25.1<2>UD      g43<4,4,1>UD                    { align1 2Q I@4 };
mov(8)          g23.1<2>UD      g42<4,4,1>UD                    { align1 1Q I@5 };
add(16)         g21<1>D         -g46<1,1,0>D    g42<1,1,0>D     { align1 1H I@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g40UD           g23UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
mov(8)          g29.1<2>UD      g21<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g31.1<2>UD      g22<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g21UD           g29UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $7 };

LABEL0:
endif(16)       JIP:  LABEL1                                    { align1 1H };

LABEL1:
shl(16)         g15<1>D         g48<8,8,1>D     0x00000002UD    { align1 1H };
mov(16)         g13<1>UD        g15<8,8,1>UD                    { align1 1H I@1 };

LABEL3:
cmp.ge.f0.0(16) null<1>UD       g13<8,8,1>UD    0x00000120UD    { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL2        UIP:  LABEL2              { align1 1H };
shl(16)         g17<1>D         g13<8,8,1>D     0x00000002UD    { align1 1H $7.src };
mov(16)         g29<1>UD        0x7f800000UD                    { align1 1H $7.src };
mov(16)         g31<1>UD        0x7f800000UD                    { align1 1H $7.src };
mov(16)         g33<1>UD        0x7f800000UD                    { align1 1H $7.src };
mov(16)         g35<1>UD        0x7f800000UD                    { align1 1H $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g17UD           g29UD           0x0400f506                0x00000200
                            slm MsgDesc: ( store_cmask, a32, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 8 flat )  base_offset 0  { align1 1H $7 };
add(16)         g13<1>D         g13<8,8,1>D     2048D           { align1 1H };

LABEL2:
while(16)       JIP:  LABEL3                                    { align1 1H };

LABEL5:
cmp.ge.f0.0(16) null<1>UD       g15<8,8,1>UD    0x00000030UD    { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL4        UIP:  LABEL4              { align1 1H };
shl(16)         g29<1>D         g15<8,8,1>D     0x00000002UD    { align1 1H $7.src };
mov(16)         g32<1>UD        0x00000000UD                    { align1 1H $7.src };
mov(16)         g34<1>UD        0x00000000UD                    { align1 1H $7.src };
mov(16)         g36<1>UD        0x00000000UD                    { align1 1H $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
add(16)         g18<1>D         g29<1,1,0>D     1152D           { align1 1H I@4 compacted };
mov(16)         g30<1>UD        0x00000000UD                    { align1 1H $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g18UD           g30UD           0x0400f506                0x00000200
                            slm MsgDesc: ( store_cmask, a32, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 8 flat )  base_offset 0  { align1 1H $7 };
add(16)         g15<1>D         g15<8,8,1>D     2048D           { align1 1H };

LABEL4:
while(16)       JIP:  LABEL5                                    { align1 1H };
sync allwr(1)                   null<0,1,0>UB                   { align1 WE_all 1N };
sync allwr(1)                   null<0,1,0>UB                   { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
send(1)         g30UD           g0UD            nullUD          0x0210011f                0x00000000
                            slm MsgDesc: ( fence, a32, threadgroup, none, normal_routing dst_len = 1, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $8 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.dst };
mov(8)          g31<1>UD        0x00000000UD                    { align1 WE_all 1Q $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
mov(2)          g31.10<1>UB     g0.11<0,1,0>UB                  { align1 WE_all 1N I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g31UD           nullUD          0x02000004                0x00000000
                            gateway MsgDesc: (barrier msg)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H $9 };
sync bar(16)                    null<0,1,0>UB                   { align1 WE_all 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.dst };
add(16)         g32<1>F         g40<1,1,0>F     g21<1,1,0>F     { align1 1H $4.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.dst };
add(16)         g34<1>F         g42<1,1,0>F     g23<1,1,0>F     { align1 1H $4.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.dst };
add(16)         g36<1>F         g44<1,1,0>F     g25<1,1,0>F     { align1 1H $4.dst compacted };
add(16)         g13<1>F         g32<1,1,0>F     -g3<1,1,0>F     { align1 1H A@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
add(16)         g16<1>F         g36<1,1,0>F     -g7<1,1,0>F     { align1 1H A@2 compacted };
mov(1)          g125<2>UW       0x00000000UD                    { align1 WE_all 1N };
mul(16)         g52<1>F         g16<1,1,0>F     g121<1,1,0>F    { align1 1H A@1 compacted };
add(16)         g15<1>F         g34<1,1,0>F     -g5<1,1,0>F     { align1 1H A@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
mul(16)         g17<1>F         g13<1,1,0>F     g11<1,1,0>F     { align1 1H F@4 compacted };
mov(16)         g58<1>UD        g52<8,8,1>F                     { align1 1H F@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
mul(16)         g19<1>F         g15<1,1,0>F     g116<1,1,0>F    { align1 1H F@2 compacted };
mov(16)         g54<1>UD        g17<8,8,1>F                     { align1 1H F@2 };
mov(16)         g56<1>UD        g19<8,8,1>F                     { align1 1H F@1 };
mov(1)          f0<1>UW         g125<0,1,0>UW                   { align1 WE_all 1N I@4 };
cmp.nz.f0.0(16) null<1>UD       g123<8,8,1>UD   0x00000000UD    { align1 1H };
mov(16)         g16<1>UD        f0<0,1,0>UW                     { align1 1H F@1 };
mov(1)          g122<1>D        1D                              { align1 WE_all 1N F@4 };
and(16)         g60<1>UD        g77<1,1,0>UD    0x00000003UD    { align1 1H $1.dst compacted };
shr(16)         g62<1>UD        g77<1,1,0>UD    0x00000002UD    { align1 1H $1.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
shl(16)         g64<1>D         g122<0,1,0>D    g77<8,8,1>UD    { align1 1H };

LABEL8:
cmp.z.f0.0(16)  null<1>D        g16<8,8,1>D     0D              { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL6        UIP:  LABEL6              { align1 1H };
fbl(16)         g66<1>UD        g16<8,8,1>UD                    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
mov(16)         a0<1>UW         0x06c0UW                        { align1 WE_all 1H I@1 };
shl(16)         a0<1>UW         g66<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x06c0UW        { align1 1H A@1 };
mov(16)         g68<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
cmp.z.f0.0(16)  g70<1>D         g68<1,1,0>D     g54<1,1,0>D     { align1 1H I@1 compacted };
and.nz.f0.0(16) null<1>UD       g70<8,8,1>UD    g123<8,8,1>UD   { align1 1H I@1 };
(+f0.0) sel(16) g72<1>UD        g21<8,8,1>UD    0xff800000UD    { align1 1H };
(+f0.0) sel(16) g74<1>UD        g23<8,8,1>UD    0xff800000UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
(+f0.0) sel(16) g76<1>UD        g40<8,8,1>UD    0x7f800000UD    { align1 1H };
(+f0.0) sel(16) g78<1>UD        g42<8,8,1>UD    0x7f800000UD    { align1 1H };
mov(16)         g87<1>F         0xff800000F      /* -infF */    { align1 WE_all 1H I@4 compacted };
mov(16)         g87<1>F         g72<1,1,0>F                     { align1 1H compacted };
mov(16)         g89<1>F         0xff800000F      /* -infF */    { align1 WE_all 1H I@3 compacted };
mov(16)         g89<1>F         g74<1,1,0>F                     { align1 1H compacted };
mov(16)         g81<1>F         0x7f800000F      /* infF */     { align1 WE_all 1H A@2 compacted };
mov(16)         g81<1>F         g76<1,1,0>F                     { align1 1H compacted };
mov(16)         g83<1>F         0x7f800000F      /* infF */     { align1 WE_all 1H I@1 compacted };
mov(16)         g83<1>F         g78<1,1,0>F                     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@2 };
(+f0.0) sel(16) g75<1>UD        g25<8,8,1>UD    0xff800000UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
(+f0.0) sel(16) g79<1>UD        g44<8,8,1>UD    0x7f800000UD    { align1 1H };
mov(8)          g98<2>UD        g87.1<8,4,2>UD                  { align1 WE_all 1Q F@4 };
mov(8)          g111<2>UD       g89.1<8,4,2>UD                  { align1 WE_all 1Q F@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g1<2>UD         g81.1<8,4,2>UD                  { align1 WE_all 1Q F@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
mov(8)          g31<2>UD        g83.1<8,4,2>UD                  { align1 WE_all 1Q F@1 };
mov(16)         g91<1>F         0xff800000F      /* -infF */    { align1 WE_all 1H I@6 compacted };
mov(16)         g91<1>F         g75<1,1,0>F                     { align1 1H compacted };
mov(16)         g85<1>F         0x7f800000F      /* infF */     { align1 WE_all 1H A@5 compacted };
mov(16)         g85<1>F         g79<1,1,0>F                     { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g60<8,8,1>D     1D              { align1 1H };
sel.ge(8)       g96<2>F         g87<8,4,2>F     g98<8,4,2>F     { align1 WE_all 1Q I@5 };
sel.ge(8)       g109<2>F        g89<8,4,2>F     g111<8,4,2>F    { align1 WE_all 1Q I@4 };
sel.l(8)        g126<2>F        g81<8,4,2>F     g1<8,4,2>F      { align1 WE_all 1Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
sel.l(8)        g29<2>F         g83<8,4,2>F     g31<8,4,2>F     { align1 WE_all 1Q I@2 };
mov(8)          g72<2>UD        g85.1<8,4,2>UD                  { align1 WE_all 1Q F@5 };
mov(8)          g87.1<2>UD      g96<8,4,2>UD                    { align1 WE_all 1Q A@4 };
mov(8)          g89.1<2>UD      g109<8,4,2>UD                   { align1 WE_all 1Q A@3 };
mov(8)          g81.1<2>UD      g126<8,4,2>UD                   { align1 WE_all 1Q A@2 };
mov(8)          g83.1<2>UD      g29<8,4,2>UD                    { align1 WE_all 1Q A@1 };
mov(8)          g1<2>UD         g91.1<8,4,2>UD                  { align1 WE_all 1Q F@2 };
sel.l(8)        g66<2>F         g85<8,4,2>F     g72<8,4,2>F     { align1 WE_all 1Q I@6 };
mov(4)          g103<4>UD       g87.2<8,2,4>UD                  { align1 WE_all 1N I@5 };
mov(4)          g101<4>UD       g87.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g116<4>UD       g89.2<8,2,4>UD                  { align1 WE_all 1N I@6 };
mov(4)          g114<4>UD       g89.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g6<4>UD         g81.2<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g4<4>UD         g81.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g36<4>UD        g83.2<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g34<4>UD        g83.1<8,2,4>UD                  { align1 WE_all 1N };
sel.ge(8)       g125<2>F        g91<8,4,2>F     g1<8,4,2>F      { align1 WE_all 1Q I@7 };
mov(8)          g85.1<2>UD      g66<8,4,2>UD                    { align1 WE_all 1Q F@2 };
sel.ge(4)       g99<4>F         g101<8,2,4>F    g103<8,2,4>F    { align1 WE_all 1N I@7 };
sel.ge(4)       g112<4>F        g114<8,2,4>F    g116<8,2,4>F    { align1 WE_all 1N I@6 };
sel.l(4)        g32<4>F         g34<8,2,4>F     g36<8,2,4>F     { align1 WE_all 1N I@2 };
mov(8)          g91.1<2>UD      g125<8,4,2>UD                   { align1 WE_all 1Q A@4 };
mov(4)          g77<4>UD        g85.2<8,2,4>UD                  { align1 WE_all 1N I@2 };
mov(4)          g75<4>UD        g85.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g87.2<4>UD      g99<8,2,4>UD                    { align1 WE_all 1N F@3 };
sel.l(4)        g2<4>F          g4<8,2,4>F      g6<8,2,4>F      { align1 WE_all 1N I@7 };
mov(4)          g89.2<4>UD      g112<8,2,4>UD                   { align1 WE_all 1N F@3 };
mov(4)          g83.2<4>UD      g32<8,2,4>UD                    { align1 WE_all 1N F@2 };
sel.l(4)        g73<4>F         g75<8,2,4>F     g77<8,2,4>F     { align1 WE_all 1N I@4 };
mov(4)          g108<4>UD       g87.3<8,2,4>UD                  { align1 WE_all 1N I@3 };
mov(4)          g106<4>UD       g87.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g81.2<4>UD      g2<8,2,4>UD                     { align1 WE_all 1N F@2 };
mov(4)          g4<4>UD         g91.1<8,2,4>UD                  { align1 WE_all 1N A@2 };
mov(4)          g6<4>UD         g91.2<8,2,4>UD                  { align1 WE_all 1N F@2 };
mov(4)          g121<4>UD       g89.3<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g119<4>UD       g89.1<8,2,4>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
mov(4)          g52<4>UD        g83.3<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g18<4>UD        g83.1<8,2,4>UD                  { align1 WE_all 1N $10.src };
mov(4)          g85.2<4>UD      g73<8,2,4>UD                    { align1 WE_all 1N F@1 };
sel.ge(4)       g104<4>F        g106<8,2,4>F    g108<8,2,4>F    { align1 WE_all 1N I@7 };
mov(4)          g11<4>UD        g81.3<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g9<4>UD         g81.1<8,2,4>UD                  { align1 WE_all 1N };
sel.ge(4)       g2<4>F          g4<8,2,4>F      g6<8,2,4>F      { align1 WE_all 1N I@7 };
sel.ge(4)       g117<4>F        g119<8,2,4>F    g121<8,2,4>F    { align1 WE_all 1N I@6 };
sel.l(4)        g13<4>F         g18<8,2,4>F     g52<8,2,4>F     { align1 WE_all 1N I@4 };
mov(4)          g95<4>UD        g85.3<8,2,4>UD                  { align1 WE_all 1N I@3 };
mov(4)          g93<4>UD        g85.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g87.3<4>UD      g104<8,2,4>UD                   { align1 WE_all 1N F@4 };
mov(4)          g91.2<4>UD      g2<8,2,4>UD                     { align1 WE_all 1N F@3 };
sel.l(4)        g7<4>F          g9<8,2,4>F      g11<8,2,4>F     { align1 WE_all 1N I@5 };
mov(4)          g89.3<4>UD      g117<8,2,4>UD                   { align1 WE_all 1N F@3 };
mov(4)          g83.3<4>UD      g13<8,2,4>UD                    { align1 WE_all 1N F@2 };
sel.l(4)        g78<4>F         g93<8,2,4>F     g95<8,2,4>F     { align1 WE_all 1N I@5 };
sel.ge(4)       g87.4<1>F       g87.3<0,1,0>F   g87.4<4,4,1>F   { align1 WE_all 1N I@4 };
sel.ge(4)       g88.4<1>F       g88.3<0,1,0>F   g88.4<4,4,1>F   { align1 WE_all 1N I@4 };
mov(4)          g81.3<4>UD      g7<8,2,4>UD                     { align1 WE_all 1N F@4 };
mov(4)          g9<4>UD         g91.1<8,2,4>UD                  { align1 WE_all 1N A@4 };
mov(4)          g11<4>UD        g91.3<8,2,4>UD                  { align1 WE_all 1N F@4 };
sel.ge(4)       g89.4<1>F       g89.3<0,1,0>F   g89.4<4,4,1>F   { align1 WE_all 1N I@5 };
sel.ge(4)       g90.4<1>F       g90.3<0,1,0>F   g90.4<4,4,1>F   { align1 WE_all 1N I@5 };
sel.l(4)        g83.4<1>F       g83.3<0,1,0>F   g83.4<4,4,1>F   { align1 WE_all 1N I@4 };
sel.l(4)        g84.4<1>F       g84.3<0,1,0>F   g84.4<4,4,1>F   { align1 WE_all 1N I@4 };
mov(4)          g85.3<4>UD      g78<8,2,4>UD                    { align1 WE_all 1N F@7 };
sel.ge(8)       g88<1>F         g87.7<0,1,0>F   g88<8,8,1>F     { align1 WE_all 1Q F@5 };
sel.l(4)        g81.4<1>F       g81.3<0,1,0>F   g81.4<4,4,1>F   { align1 WE_all 1N I@4 };
sel.l(4)        g82.4<1>F       g82.3<0,1,0>F   g82.4<4,4,1>F   { align1 WE_all 1N I@4 };
sel.ge(4)       g7<4>F          g9<8,2,4>F      g11<8,2,4>F     { align1 WE_all 1N I@2 };
sel.ge(8)       g90<1>F         g89.7<0,1,0>F   g90<8,8,1>F     { align1 WE_all 1Q F@7 };
sel.l(8)        g84<1>F         g83.7<0,1,0>F   g84<8,8,1>F     { align1 WE_all 1Q F@6 };
sel.l(4)        g85.4<1>F       g85.3<0,1,0>F   g85.4<4,4,1>F   { align1 WE_all 1N I@1 };
sel.l(4)        g86.4<1>F       g86.3<0,1,0>F   g86.4<4,4,1>F   { align1 WE_all 1N I@1 };
sel.l(8)        g82<1>F         g81.7<0,1,0>F   g82<8,8,1>F     { align1 WE_all 1Q F@6 };
mov(4)          g91.3<4>UD      g7<8,2,4>UD                     { align1 WE_all 1N F@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@5 };
(+f0.0) sel(16) g95<1>UD        g90.7<0,1,0>UD  g88.7<0,1,0>UD  { align1 1H };
sel.l(8)        g86<1>F         g85.7<0,1,0>F   g86<8,8,1>F     { align1 WE_all 1Q F@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@2 };
(+f0.0) sel(16) g93<1>UD        g84.7<0,1,0>UD  g82.7<0,1,0>UD  { align1 1H };
sel.ge(4)       g91.4<1>F       g91.3<0,1,0>F   g91.4<4,4,1>F   { align1 WE_all 1N I@3 };
sel.ge(4)       g92.4<1>F       g92.3<0,1,0>F   g92.4<4,4,1>F   { align1 WE_all 1N I@3 };
cmp.z.f0.0(16)  null<1>D        g60<8,8,1>D     2D              { align1 1H };
sel.ge(8)       g92<1>F         g91.7<0,1,0>F   g92<8,8,1>F     { align1 WE_all 1Q F@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@2 };
(+f0.0) sel(16) g97<1>UD        g86.7<0,1,0>UD  g93<1,1,0>UD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
(+f0.0) sel(16) g99<1>UD        g92.7<0,1,0>UD  g95<1,1,0>UD    { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g62<8,8,1>D     0D              { align1 1H };
(+f0.0) sel(16) g52<1>F         g97<1,1,0>F     -g99<1,1,0>F    { align1 1H I@2 compacted };
and.nz.f0.0(16) null<1>UD       g64<8,8,1>UD    0x00000077UD    { align1 1H };
(+f0.0) if(16)  JIP:  LABEL7          UIP:  LABEL7              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
mul(16)         g100<1>D        g68<1,1,0>D     6W              { align1 1H compacted };
mul(16)         g102<1>D        g62<1,1,0>D     3W              { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
add3(16)        g104<1>D        g100<8,8,1>D    g60<8,8,1>D     g102<1,1,1>D { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
shl(16)         g19<1>D         g104<8,8,1>D    0x00000002UD    { align1 1H $10.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        nullUD          g19UD           g52UD           0x04040515                0x00000080
                            slm MsgDesc: ( atomic_fmin, a32, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1H $10 };

LABEL7:
endif(16)       JIP:  LABEL6                                    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
or(16)          g105<1>D        ~g70<1,1,0>D    ~g123<1,1,0>D   { align1 1H compacted };
mov(1)          g12<2>UW        0x00000000UD                    { align1 WE_all 1N };
mov(1)          f0<1>UW         g12<0,1,0>UW                    { align1 WE_all 1N I@1 };
cmp.nz.f0.0(16) null<1>UD       g105<8,8,1>UD   0x00000000UD    { align1 1H I@3 };
mov(16)         g106<1>UD       f0<0,1,0>UW                     { align1 1H };
and(16)         g16<1>UD        g16<1,1,0>UD    g106<1,1,0>UD   { align1 1H I@1 compacted };

LABEL6:
while(16)       JIP:  LABEL8                                    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@4 };
add(16)         g107<1>D        g56<1,1,0>D     16D             { align1 1H compacted };
mov(1)          g29<2>UW        0x00000000UD                    { align1 WE_all 1N $7.src };
mov(1)          f0<1>UW         g29<0,1,0>UW                    { align1 WE_all 1N I@1 };
cmp.nz.f0.0(16) null<1>UD       g123<8,8,1>UD   0x00000000UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@6 };
mov(16)         g17<1>UD        f0<0,1,0>UW                     { align1 1H };

LABEL11:
cmp.z.f0.0(16)  null<1>D        g17<8,8,1>D     0D              { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL9        UIP:  LABEL9              { align1 1H };
fbl(16)         g109<1>UD       g17<8,8,1>UD                    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         a0<1>UW         0x0d60UW                        { align1 WE_all 1H };
shl(16)         a0<1>UW         g109<16,8,2>W   0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0d60UW        { align1 1H A@1 };
mov(16)         g111<1>UD       g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
cmp.z.f0.0(16)  g113<1>D        g111<1,1,0>D    g107<1,1,0>D    { align1 1H compacted };
and.nz.f0.0(16) null<1>UD       g113<8,8,1>UD   g123<8,8,1>UD   { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
(+f0.0) sel(16) g115<1>UD       g21<8,8,1>UD    0xff800000UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
(+f0.0) sel(16) g117<1>UD       g23<8,8,1>UD    0xff800000UD    { align1 1H };
(+f0.0) sel(16) g119<1>UD       g40<8,8,1>UD    0x7f800000UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@4 };
(+f0.0) sel(16) g121<1>UD       g42<8,8,1>UD    0x7f800000UD    { align1 1H };
(+f0.0) sel(16) g125<1>UD       g44<8,8,1>UD    0x7f800000UD    { align1 1H };
mov(16)         g7<1>F          0xff800000F      /* -infF */    { align1 WE_all 1H A@5 compacted };
mov(16)         g7<1>F          g115<1,1,0>F                    { align1 1H compacted };
mov(16)         g9<1>F          0xff800000F      /* -infF */    { align1 WE_all 1H I@4 compacted };
mov(16)         g9<1>F          g117<1,1,0>F                    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(16)         g1<1>F          0x7f800000F      /* infF */     { align1 WE_all 1H A@3 compacted };
mov(16)         g1<1>F          g119<1,1,0>F                    { align1 1H compacted };
mov(16)         g3<1>F          0x7f800000F      /* infF */     { align1 WE_all 1H A@2 compacted };
mov(16)         g3<1>F          g121<1,1,0>F                    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@2 };
(+f0.0) sel(16) g118<1>UD       g25<8,8,1>UD    0xff800000UD    { align1 1H };
mov(16)         g5<1>F          0x7f800000F      /* infF */     { align1 WE_all 1H A@2 compacted };
mov(16)         g5<1>F          g125<1,1,0>F                    { align1 1H compacted };
mov(8)          g94<2>UD        g7.1<8,4,2>UD                   { align1 WE_all 1Q F@5 };
mov(8)          g109<2>UD       g9.1<8,4,2>UD                   { align1 WE_all 1Q F@4 };
mov(8)          g32<2>UD        g1.1<8,4,2>UD                   { align1 WE_all 1Q F@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mov(8)          g68<2>UD        g3.1<8,4,2>UD                   { align1 WE_all 1Q F@2 };
mov(16)         g11<1>F         0xff800000F      /* -infF */    { align1 WE_all 1H A@5 compacted };
mov(16)         g11<1>F         g118<1,1,0>F                    { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g60<8,8,1>D     1D              { align1 1H };
mov(8)          g81<2>UD        g5.1<8,4,2>UD                   { align1 WE_all 1Q F@2 };
sel.ge(8)       g92<2>F         g7<8,4,2>F      g94<8,4,2>F     { align1 WE_all 1Q I@6 };
sel.ge(8)       g105<2>F        g9<8,4,2>F      g109<8,4,2>F    { align1 WE_all 1Q I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
sel.l(8)        g30<2>F         g1<8,4,2>F      g32<8,4,2>F     { align1 WE_all 1Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
sel.l(8)        g66<2>F         g3<8,4,2>F      g68<8,4,2>F     { align1 WE_all 1Q I@3 };
sel.l(8)        g79<2>F         g5<8,4,2>F      g81<8,4,2>F     { align1 WE_all 1Q I@1 };
mov(8)          g7.1<2>UD       g92<8,4,2>UD                    { align1 WE_all 1Q A@5 };
mov(8)          g9.1<2>UD       g105<8,4,2>UD                   { align1 WE_all 1Q A@4 };
mov(8)          g1.1<2>UD       g30<8,4,2>UD                    { align1 WE_all 1Q A@3 };
mov(8)          g3.1<2>UD       g66<8,4,2>UD                    { align1 WE_all 1Q A@2 };
mov(8)          g32<2>UD        g11.1<8,4,2>UD                  { align1 WE_all 1Q F@3 };
mov(8)          g5.1<2>UD       g79<8,4,2>UD                    { align1 WE_all 1Q A@1 };
mov(4)          g99<4>UD        g7.2<8,2,4>UD                   { align1 WE_all 1N I@6 };
mov(4)          g97<4>UD        g7.1<8,2,4>UD                   { align1 WE_all 1N };
mov(4)          g119<4>UD       g9.2<8,2,4>UD                   { align1 WE_all 1N A@6 };
mov(4)          g117<4>UD       g9.1<8,2,4>UD                   { align1 WE_all 1N F@6 };
mov(4)          g13<4>UD        g1.2<8,2,4>UD                   { align1 WE_all 1N I@7 };
mov(4)          g35<4>UD        g1.1<8,2,4>UD                   { align1 WE_all 1N };
mov(4)          g73<4>UD        g3.2<8,2,4>UD                   { align1 WE_all 1N I@7 };
mov(4)          g71<4>UD        g3.1<8,2,4>UD                   { align1 WE_all 1N };
sel.ge(8)       g30<2>F         g11<8,4,2>F     g32<8,4,2>F     { align1 WE_all 1Q I@7 };
mov(4)          g86<4>UD        g5.2<8,2,4>UD                   { align1 WE_all 1N I@7 };
mov(4)          g84<4>UD        g5.1<8,2,4>UD                   { align1 WE_all 1N };
sel.ge(4)       g95<4>F         g97<8,2,4>F     g99<8,2,4>F     { align1 WE_all 1N I@7 };
sel.ge(4)       g115<4>F        g117<8,2,4>F    g119<8,2,4>F    { align1 WE_all 1N I@7 };
sel.l(4)        g69<4>F         g71<8,2,4>F     g73<8,2,4>F     { align1 WE_all 1N I@3 };
mov(8)          g11.1<2>UD      g30<8,4,2>UD                    { align1 WE_all 1Q F@4 };
sel.l(4)        g82<4>F         g84<8,2,4>F     g86<8,2,4>F     { align1 WE_all 1N I@2 };
mov(4)          g7.2<4>UD       g95<8,2,4>UD                    { align1 WE_all 1N F@4 };
sel.l(4)        g33<4>F         g35<8,2,4>F     g13<8,2,4>F     { align1 WE_all 1N I@7 };
mov(4)          g9.2<4>UD       g115<8,2,4>UD                   { align1 WE_all 1N F@4 };
mov(4)          g3.2<4>UD       g69<8,2,4>UD                    { align1 WE_all 1N F@3 };
mov(4)          g5.2<4>UD       g82<8,2,4>UD                    { align1 WE_all 1N F@2 };
mov(4)          g104<4>UD       g7.3<8,2,4>UD                   { align1 WE_all 1N I@4 };
mov(4)          g102<4>UD       g7.1<8,2,4>UD                   { align1 WE_all 1N };
mov(4)          g1.2<4>UD       g33<8,2,4>UD                    { align1 WE_all 1N F@1 };
mov(4)          g29<4>UD        g9.3<8,2,4>UD                   { align1 WE_all 1N I@6 };
mov(4)          g125<4>UD       g9.1<8,2,4>UD                   { align1 WE_all 1N };
mov(4)          g35<4>UD        g11.1<8,2,4>UD                  { align1 WE_all 1N A@1 };
mov(4)          g13<4>UD        g11.2<8,2,4>UD                  { align1 WE_all 1N F@1 };
mov(4)          g78<4>UD        g3.3<8,2,4>UD                   { align1 WE_all 1N I@7 };
mov(4)          g76<4>UD        g3.1<8,2,4>UD                   { align1 WE_all 1N };
mov(4)          g91<4>UD        g5.3<8,2,4>UD                   { align1 WE_all 1N I@7 };
mov(4)          g89<4>UD        g5.1<8,2,4>UD                   { align1 WE_all 1N };
sel.ge(4)       g100<4>F        g102<8,2,4>F    g104<8,2,4>F    { align1 WE_all 1N I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
mov(4)          g52<4>UD        g1.3<8,2,4>UD                   { align1 WE_all 1N I@7 };
mov(4)          g19<4>UD        g1.1<8,2,4>UD                   { align1 WE_all 1N $10.src };
sel.ge(4)       g120<4>F        g125<8,2,4>F    g29<8,2,4>F     { align1 WE_all 1N I@7 };
sel.ge(4)       g33<4>F         g35<8,2,4>F     g13<8,2,4>F     { align1 WE_all 1N I@7 };
sel.l(4)        g74<4>F         g76<8,2,4>F     g78<8,2,4>F     { align1 WE_all 1N I@5 };
sel.l(4)        g87<4>F         g89<8,2,4>F     g91<8,2,4>F     { align1 WE_all 1N I@3 };
mov(4)          g7.3<4>UD       g100<8,2,4>UD                   { align1 WE_all 1N F@5 };
mov(4)          g9.3<4>UD       g120<8,2,4>UD                   { align1 WE_all 1N F@4 };
mov(4)          g11.2<4>UD      g33<8,2,4>UD                    { align1 WE_all 1N F@3 };
sel.l(4)        g14<4>F         g19<8,2,4>F     g52<8,2,4>F     { align1 WE_all 1N I@4 };
mov(4)          g3.3<4>UD       g74<8,2,4>UD                    { align1 WE_all 1N F@3 };
mov(4)          g5.3<4>UD       g87<8,2,4>UD                    { align1 WE_all 1N F@2 };
sel.ge(4)       g7.4<1>F        g7.3<0,1,0>F    g7.4<4,4,1>F    { align1 WE_all 1N I@5 };
sel.ge(4)       g8.4<1>F        g8.3<0,1,0>F    g8.4<4,4,1>F    { align1 WE_all 1N I@5 };
sel.ge(4)       g9.4<1>F        g9.3<0,1,0>F    g9.4<4,4,1>F    { align1 WE_all 1N I@4 };
sel.ge(4)       g10.4<1>F       g10.3<0,1,0>F   g10.4<4,4,1>F   { align1 WE_all 1N I@4 };
mov(4)          g1.3<4>UD       g14<8,2,4>UD                    { align1 WE_all 1N F@5 };
mov(4)          g19<4>UD        g11.1<8,2,4>UD                  { align1 WE_all 1N A@4 };
mov(4)          g52<4>UD        g11.3<8,2,4>UD                  { align1 WE_all 1N F@5 };
sel.l(4)        g3.4<1>F        g3.3<0,1,0>F    g3.4<4,4,1>F    { align1 WE_all 1N I@5 };
sel.l(4)        g4.4<1>F        g4.3<0,1,0>F    g4.4<4,4,1>F    { align1 WE_all 1N I@5 };
sel.l(4)        g5.4<1>F        g5.3<0,1,0>F    g5.4<4,4,1>F    { align1 WE_all 1N I@4 };
sel.l(4)        g6.4<1>F        g6.3<0,1,0>F    g6.4<4,4,1>F    { align1 WE_all 1N I@4 };
sel.ge(8)       g8<1>F          g7.7<0,1,0>F    g8<8,8,1>F      { align1 WE_all 1Q F@7 };
sel.ge(8)       g10<1>F         g9.7<0,1,0>F    g10<8,8,1>F     { align1 WE_all 1Q F@6 };
sel.l(4)        g1.4<1>F        g1.3<0,1,0>F    g1.4<4,4,1>F    { align1 WE_all 1N I@3 };
sel.l(4)        g2.4<1>F        g2.3<0,1,0>F    g2.4<4,4,1>F    { align1 WE_all 1N I@3 };
sel.ge(4)       g14<4>F         g19<8,2,4>F     g52<8,2,4>F     { align1 WE_all 1N I@1 };
sel.l(8)        g4<1>F          g3.7<0,1,0>F    g4<8,8,1>F      { align1 WE_all 1Q F@7 };
sel.l(8)        g6<1>F          g5.7<0,1,0>F    g6<8,8,1>F      { align1 WE_all 1Q F@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@6 };
(+f0.0) sel(16) g31<1>UD        g10.7<0,1,0>UD  g8.7<0,1,0>UD   { align1 1H };
sel.l(8)        g2<1>F          g1.7<0,1,0>F    g2<8,8,1>F      { align1 WE_all 1Q F@4 };
mov(4)          g11.3<4>UD      g14<8,2,4>UD                    { align1 WE_all 1N F@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
(+f0.0) sel(16) g29<1>UD        g4.7<0,1,0>UD   g2.7<0,1,0>UD   { align1 1H };
sel.ge(4)       g11.4<1>F       g11.3<0,1,0>F   g11.4<4,4,1>F   { align1 WE_all 1N I@2 };
sel.ge(4)       g12.4<1>F       g12.3<0,1,0>F   g12.4<4,4,1>F   { align1 WE_all 1N I@2 };
cmp.z.f0.0(16)  null<1>D        g60<8,8,1>D     2D              { align1 1H };
sel.ge(8)       g12<1>F         g11.7<0,1,0>F   g12<8,8,1>F     { align1 WE_all 1Q F@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@2 };
(+f0.0) sel(16) g33<1>UD        g6.7<0,1,0>UD   g29<1,1,0>UD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
(+f0.0) sel(16) g35<1>UD        g12.7<0,1,0>UD  g31<1,1,0>UD    { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g62<8,8,1>D     0D              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
(+f0.0) sel(16) g68<1>F         g33<1,1,0>F     -g35<1,1,0>F    { align1 1H compacted };
and.nz.f0.0(16) null<1>UD       g64<8,8,1>UD    0x00000077UD    { align1 1H };
(+f0.0) if(16)  JIP:  LABEL10         UIP:  LABEL10             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
mul(16)         g36<1>D         g111<1,1,0>D    6W              { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
mul(16)         g13<1>D         g62<1,1,0>D     3W              { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
add3(16)        g15<1>D         g36<8,8,1>D     g60<8,8,1>D     g13<1,1,1>D { align1 1H };
shl(16)         g66<1>D         g15<8,8,1>D     0x00000002UD    { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        nullUD          g66UD           g68UD           0x04040515                0x00000080
                            slm MsgDesc: ( atomic_fmin, a32, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1H $11 };

LABEL10:
endif(16)       JIP:  LABEL9                                    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@7 };
or(16)          g19<1>D         ~g113<1,1,0>D   ~g123<1,1,0>D   { align1 1H compacted };
mov(1)          g53<2>UW        0x00000000UD                    { align1 WE_all 1N F@7 };
mov(1)          f0<1>UW         g53<0,1,0>UW                    { align1 WE_all 1N I@1 };
cmp.nz.f0.0(16) null<1>UD       g19<8,8,1>UD    0x00000000UD    { align1 1H I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@7 };
mov(16)         g52<1>UD        f0<0,1,0>UW                     { align1 1H };
and(16)         g17<1>UD        g17<1,1,0>UD    g52<1,1,0>UD    { align1 1H I@1 compacted };

LABEL9:
while(16)       JIP:  LABEL11                                   { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
add(16)         g66<1>D         g58<1,1,0>D     32D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mov(1)          g68<2>UW        0x00000000UD                    { align1 WE_all 1N F@1 };
mov(1)          f0<1>UW         g68<0,1,0>UW                    { align1 WE_all 1N I@1 };
cmp.nz.f0.0(16) null<1>UD       g123<8,8,1>UD   0x00000000UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@5 };
mov(16)         g18<1>UD        f0<0,1,0>UW                     { align1 1H };

LABEL14:
cmp.z.f0.0(16)  null<1>D        g18<8,8,1>D     0D              { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL12       UIP:  LABEL12             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
fbl(16)         g68<1>UD        g18<8,8,1>UD                    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         a0<1>UW         0x0840UW                        { align1 WE_all 1H };
shl(16)         a0<1>UW         g68<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0840UW        { align1 1H A@1 };
mov(16)         g70<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
cmp.z.f0.0(16)  g72<1>D         g70<1,1,0>D     g66<1,1,0>D     { align1 1H compacted };
and.nz.f0.0(16) null<1>UD       g72<8,8,1>UD    g123<8,8,1>UD   { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
(+f0.0) sel(16) g74<1>UD        g21<8,8,1>UD    0xff800000UD    { align1 1H };
(+f0.0) sel(16) g76<1>UD        g23<8,8,1>UD    0xff800000UD    { align1 1H };
(+f0.0) sel(16) g78<1>UD        g40<8,8,1>UD    0x7f800000UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@6 };
(+f0.0) sel(16) g80<1>UD        g42<8,8,1>UD    0x7f800000UD    { align1 1H };
mov(16)         g89<1>F         0xff800000F      /* -infF */    { align1 WE_all 1H A@4 compacted };
mov(16)         g89<1>F         g74<1,1,0>F                     { align1 1H compacted };
mov(16)         g91<1>F         0xff800000F      /* -infF */    { align1 WE_all 1H I@3 compacted };
mov(16)         g91<1>F         g76<1,1,0>F                     { align1 1H compacted };
mov(16)         g83<1>F         0x7f800000F      /* infF */     { align1 WE_all 1H A@2 compacted };
mov(16)         g83<1>F         g78<1,1,0>F                     { align1 1H compacted };
mov(16)         g85<1>F         0x7f800000F      /* infF */     { align1 WE_all 1H A@1 compacted };
mov(16)         g85<1>F         g80<1,1,0>F                     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@2 };
(+f0.0) sel(16) g77<1>UD        g25<8,8,1>UD    0xff800000UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
(+f0.0) sel(16) g81<1>UD        g44<8,8,1>UD    0x7f800000UD    { align1 1H };
mov(8)          g6<2>UD         g89.1<8,4,2>UD                  { align1 WE_all 1Q F@4 };
mov(8)          g36<2>UD        g91.1<8,4,2>UD                  { align1 WE_all 1Q F@3 };
mov(8)          g105<2>UD       g85.1<8,4,2>UD                  { align1 WE_all 1Q F@1 };
mov(16)         g93<1>F         0xff800000F      /* -infF */    { align1 WE_all 1H A@5 compacted };
mov(16)         g93<1>F         g77<1,1,0>F                     { align1 1H compacted };
mov(8)          g79<2>UD        g83.1<8,4,2>UD                  { align1 WE_all 1Q F@2 };
mov(16)         g87<1>F         0x7f800000F      /* infF */     { align1 WE_all 1H A@5 compacted };
mov(16)         g87<1>F         g81<1,1,0>F                     { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g60<8,8,1>D     1D              { align1 1H };
sel.ge(8)       g4<2>F          g89<8,4,2>F     g6<8,4,2>F      { align1 WE_all 1Q A@5 };
sel.ge(8)       g34<2>F         g91<8,4,2>F     g36<8,4,2>F     { align1 WE_all 1Q A@4 };
sel.l(8)        g103<2>F        g85<8,4,2>F     g105<8,4,2>F    { align1 WE_all 1Q I@3 };
sel.l(8)        g74<2>F         g83<8,4,2>F     g79<8,4,2>F     { align1 WE_all 1Q I@2 };
mov(8)          g118<2>UD       g87.1<8,4,2>UD                  { align1 WE_all 1Q F@5 };
mov(8)          g89.1<2>UD      g4<8,4,2>UD                     { align1 WE_all 1Q A@4 };
mov(8)          g91.1<2>UD      g34<8,4,2>UD                    { align1 WE_all 1Q A@3 };
mov(8)          g85.1<2>UD      g103<8,4,2>UD                   { align1 WE_all 1Q A@2 };
mov(8)          g83.1<2>UD      g74<8,4,2>UD                    { align1 WE_all 1Q A@1 };
mov(8)          g79<2>UD        g93.1<8,4,2>UD                  { align1 WE_all 1Q F@1 };
sel.l(8)        g116<2>F        g87<8,4,2>F     g118<8,4,2>F    { align1 WE_all 1Q I@6 };
mov(4)          g11<4>UD        g89.2<8,2,4>UD                  { align1 WE_all 1N I@5 };
mov(4)          g9<4>UD         g89.1<8,2,4>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
mov(4)          g52<4>UD        g91.2<8,2,4>UD                  { align1 WE_all 1N I@6 };
mov(4)          g15<4>UD        g91.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g110<4>UD       g85.2<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g108<4>UD       g85.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g97<4>UD        g83.2<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g95<4>UD        g83.1<8,2,4>UD                  { align1 WE_all 1N };
sel.ge(8)       g77<2>F         g93<8,4,2>F     g79<8,4,2>F     { align1 WE_all 1Q I@7 };
mov(8)          g87.1<2>UD      g116<8,4,2>UD                   { align1 WE_all 1Q F@2 };
sel.ge(4)       g7<4>F          g9<8,2,4>F      g11<8,2,4>F     { align1 WE_all 1N I@7 };
sel.ge(4)       g13<4>F         g15<8,2,4>F     g52<8,2,4>F     { align1 WE_all 1N I@6 };
sel.l(4)        g106<4>F        g108<8,2,4>F    g110<8,2,4>F    { align1 WE_all 1N I@4 };
mov(8)          g93.1<2>UD      g77<8,4,2>UD                    { align1 WE_all 1Q A@4 };
mov(4)          g125<4>UD       g87.2<8,2,4>UD                  { align1 WE_all 1N I@2 };
mov(4)          g121<4>UD       g87.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g89.2<4>UD      g7<8,2,4>UD                     { align1 WE_all 1N F@3 };
sel.l(4)        g80<4>F         g95<8,2,4>F     g97<8,2,4>F     { align1 WE_all 1N I@6 };
mov(4)          g91.2<4>UD      g13<8,2,4>UD                    { align1 WE_all 1N F@3 };
mov(4)          g85.2<4>UD      g106<8,2,4>UD                   { align1 WE_all 1N F@2 };
sel.l(4)        g119<4>F        g121<8,2,4>F    g125<8,2,4>F    { align1 WE_all 1N I@4 };
mov(4)          g33<4>UD        g89.3<8,2,4>UD                  { align1 WE_all 1N I@3 };
mov(4)          g31<4>UD        g89.1<8,2,4>UD                  { align1 WE_all 1N $9.src };
mov(4)          g83.2<4>UD      g80<8,2,4>UD                    { align1 WE_all 1N F@2 };
mov(4)          g76<4>UD        g91.3<8,2,4>UD                  { align1 WE_all 1N I@5 };
mov(4)          g74<4>UD        g91.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g95<4>UD        g93.1<8,2,4>UD                  { align1 WE_all 1N F@2 };
mov(4)          g97<4>UD        g93.2<8,2,4>UD                  { align1 WE_all 1N F@2 };
mov(4)          g115<4>UD       g85.3<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g113<4>UD       g85.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g87.2<4>UD      g119<8,2,4>UD                   { align1 WE_all 1N F@1 };
sel.ge(4)       g29<4>F         g31<8,2,4>F     g33<8,2,4>F     { align1 WE_all 1N I@7 };
mov(4)          g102<4>UD       g83.3<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g100<4>UD       g83.1<8,2,4>UD                  { align1 WE_all 1N };
sel.ge(4)       g68<4>F         g74<8,2,4>F     g76<8,2,4>F     { align1 WE_all 1N I@7 };
sel.ge(4)       g80<4>F         g95<8,2,4>F     g97<8,2,4>F     { align1 WE_all 1N I@6 };
sel.l(4)        g111<4>F        g113<8,2,4>F    g115<8,2,4>F    { align1 WE_all 1N I@4 };
mov(4)          g3<4>UD         g87.3<8,2,4>UD                  { align1 WE_all 1N I@3 };
mov(4)          g1<4>UD         g87.1<8,2,4>UD                  { align1 WE_all 1N $0.dst };
mov(4)          g89.3<4>UD      g29<8,2,4>UD                    { align1 WE_all 1N F@4 };
mov(4)          g91.3<4>UD      g68<8,2,4>UD                    { align1 WE_all 1N F@3 };
mov(4)          g93.2<4>UD      g80<8,2,4>UD                    { align1 WE_all 1N F@2 };
sel.l(4)        g98<4>F         g100<8,2,4>F    g102<8,2,4>F    { align1 WE_all 1N I@6 };
mov(4)          g85.3<4>UD      g111<8,2,4>UD                   { align1 WE_all 1N F@2 };
sel.l(4)        g126<4>F        g1<8,2,4>F      g3<8,2,4>F      { align1 WE_all 1N I@5 };
sel.ge(4)       g89.4<1>F       g89.3<0,1,0>F   g89.4<4,4,1>F   { align1 WE_all 1N I@4 };
sel.ge(4)       g90.4<1>F       g90.3<0,1,0>F   g90.4<4,4,1>F   { align1 WE_all 1N I@4 };
sel.ge(4)       g91.4<1>F       g91.3<0,1,0>F   g91.4<4,4,1>F   { align1 WE_all 1N I@3 };
sel.ge(4)       g92.4<1>F       g92.3<0,1,0>F   g92.4<4,4,1>F   { align1 WE_all 1N I@3 };
mov(4)          g83.3<4>UD      g98<8,2,4>UD                    { align1 WE_all 1N F@6 };
mov(4)          g100<4>UD       g93.1<8,2,4>UD                  { align1 WE_all 1N A@3 };
mov(4)          g102<4>UD       g93.3<8,2,4>UD                  { align1 WE_all 1N F@6 };
sel.l(4)        g85.4<1>F       g85.3<0,1,0>F   g85.4<4,4,1>F   { align1 WE_all 1N I@4 };
sel.l(4)        g86.4<1>F       g86.3<0,1,0>F   g86.4<4,4,1>F   { align1 WE_all 1N I@4 };
mov(4)          g87.3<4>UD      g126<8,2,4>UD                   { align1 WE_all 1N F@7 };
sel.ge(8)       g90<1>F         g89.7<0,1,0>F   g90<8,8,1>F     { align1 WE_all 1Q F@5 };
sel.ge(8)       g92<1>F         g91.7<0,1,0>F   g92<8,8,1>F     { align1 WE_all 1Q F@4 };
sel.l(4)        g83.4<1>F       g83.3<0,1,0>F   g83.4<4,4,1>F   { align1 WE_all 1N I@4 };
sel.l(4)        g84.4<1>F       g84.3<0,1,0>F   g84.4<4,4,1>F   { align1 WE_all 1N I@4 };
sel.ge(4)       g98<4>F         g100<8,2,4>F    g102<8,2,4>F    { align1 WE_all 1N I@2 };
sel.l(8)        g86<1>F         g85.7<0,1,0>F   g86<8,8,1>F     { align1 WE_all 1Q F@6 };
sel.l(4)        g87.4<1>F       g87.3<0,1,0>F   g87.4<4,4,1>F   { align1 WE_all 1N I@1 };
sel.l(4)        g88.4<1>F       g88.3<0,1,0>F   g88.4<4,4,1>F   { align1 WE_all 1N I@1 };
sel.l(8)        g84<1>F         g83.7<0,1,0>F   g84<8,8,1>F     { align1 WE_all 1Q F@5 };
mov(4)          g93.3<4>UD      g98<8,2,4>UD                    { align1 WE_all 1N F@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@7 };
(+f0.0) sel(16) g97<1>UD        g92.7<0,1,0>UD  g90.7<0,1,0>UD  { align1 1H };
sel.l(8)        g88<1>F         g87.7<0,1,0>F   g88<8,8,1>F     { align1 WE_all 1Q F@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@2 };
(+f0.0) sel(16) g95<1>UD        g86.7<0,1,0>UD  g84.7<0,1,0>UD  { align1 1H };
sel.ge(4)       g93.4<1>F       g93.3<0,1,0>F   g93.4<4,4,1>F   { align1 WE_all 1N I@3 };
sel.ge(4)       g94.4<1>F       g94.3<0,1,0>F   g94.4<4,4,1>F   { align1 WE_all 1N I@3 };
cmp.z.f0.0(16)  null<1>D        g60<8,8,1>D     2D              { align1 1H };
sel.ge(8)       g94<1>F         g93.7<0,1,0>F   g94<8,8,1>F     { align1 WE_all 1Q F@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@2 };
(+f0.0) sel(16) g99<1>UD        g88.7<0,1,0>UD  g95<1,1,0>UD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
(+f0.0) sel(16) g101<1>UD       g94.7<0,1,0>UD  g97<1,1,0>UD    { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g62<8,8,1>D     0D              { align1 1H };
(+f0.0) sel(16) g74<1>F         g99<1,1,0>F     -g101<1,1,0>F   { align1 1H I@2 compacted };
and.nz.f0.0(16) null<1>UD       g64<8,8,1>UD    0x00000077UD    { align1 1H };
(+f0.0) if(16)  JIP:  LABEL13         UIP:  LABEL13             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
mul(16)         g102<1>D        g70<1,1,0>D     6W              { align1 1H compacted };
mul(16)         g104<1>D        g62<1,1,0>D     3W              { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
add3(16)        g106<1>D        g102<8,8,1>D    g60<8,8,1>D     g104<1,1,1>D { align1 1H };
shl(16)         g69<1>D         g106<8,8,1>D    0x00000002UD    { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        nullUD          g69UD           g74UD           0x04040515                0x00000080
                            slm MsgDesc: ( atomic_fmin, a32, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1H $11 };

LABEL13:
endif(16)       JIP:  LABEL12                                   { align1 1H };
or(16)          g107<1>D        ~g72<1,1,0>D    ~g123<1,1,0>D   { align1 1H compacted };
mov(1)          g103<2>UW       0x00000000UD                    { align1 WE_all 1N A@4 };
mov(1)          f0<1>UW         g103<0,1,0>UW                   { align1 WE_all 1N I@1 };
cmp.nz.f0.0(16) null<1>UD       g107<8,8,1>UD   0x00000000UD    { align1 1H I@3 };
mov(16)         g108<1>UD       f0<0,1,0>UW                     { align1 1H };
and(16)         g18<1>UD        g18<1,1,0>UD    g108<1,1,0>UD   { align1 1H I@1 compacted };

LABEL12:
while(16)       JIP:  LABEL14                                   { align1 1H };
mov.nz.f0.0(16) null<1>D        g123<8,8,1>D                    { align1 1H };
(+f0.0) if(16)  JIP:  LABEL15         UIP:  LABEL15             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@6 };
shl(16)         g109<1>D        g54<8,8,1>D     0x00000002UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
add(16)         g75<1>D         g109<1,1,0>D    1152D           { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g75UD           nullUD          0x04040508                0x00000000
                            slm MsgDesc: ( atomic_inc, a32, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1H $11 };
shl(16)         g110<1>D        g56<8,8,1>D     0x00000002UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
add(16)         g76<1>D         g110<1,1,0>D    1216D           { align1 1H $11.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g76UD           nullUD          0x04040508                0x00000000
                            slm MsgDesc: ( atomic_inc, a32, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1H $12 };
shl(16)         g111<1>D        g58<8,8,1>D     0x00000002UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
add(16)         g77<1>D         g111<1,1,0>D    1280D           { align1 1H $12.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g77UD           nullUD          0x04040508                0x00000000
                            slm MsgDesc: ( atomic_inc, a32, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1H $13 };

LABEL15:
endif(16)       JIP:  LABEL16                                   { align1 1H };

LABEL16:
sync allwr(1)                   null<0,1,0>UB                   { align1 WE_all 1N };
sync allwr(1)                   null<0,1,0>UB                   { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(1)         g112UD          g0UD            nullUD          0x0210011f                0x00000000
                            slm MsgDesc: ( fence, a32, threadgroup, none, normal_routing dst_len = 1, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $14 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.dst };
mov(8)          g113<1>UD       0x00000000UD                    { align1 WE_all 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
mov(2)          g113.10<1>UB    g0.11<0,1,0>UB                  { align1 WE_all 1N I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g113UD          nullUD          0x02000004                0x00000000
                            gateway MsgDesc: (barrier msg)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H $15 };
sync bar(16)                    null<0,1,0>UB                   { align1 WE_all 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
add(8)          g116<1>D        g38<8,4,2>D     11352D          { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N F@1 };
add(8)          g117<1>D        g50<8,4,2>D     11352D          { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
mov(16)         g19<1>UD        g48<8,8,1>UD                    { align1 1H A@7 };
mov(8)          g114<1>UD       g38.1<8,4,2>UD                  { align1 1Q };
mov(8)          g115<1>UD       g50.1<8,4,2>UD                  { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
cmp.l.f0.0(16)  g118<1>UD       g116<8,8,1>UD   0x00002c58UD    { align1 1H };

LABEL19:
cmp.ge.f0.0(16) null<1>UD       g19<8,8,1>UD    0x00000120UD    { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL17       UIP:  LABEL17             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
shl(16)         g78<1>D         g19<8,8,1>D     0x00000002UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        g83UD           g78UD           nullUD          0x04201502                0x00000000
                            slm MsgDesc: ( load_cmask, a32, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1H $13 };
cmp.nz.f0.0(16) null<1>F        g83<1,1,0>F     0x7f800000F  /* infF */ { align1 1H $13.dst compacted };
(+f0.0) if(16)  JIP:  LABEL18         UIP:  LABEL18             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@5 };
add(16)         g120<1>D        g116<1,1,0>D    g78<1,1,0>D     { align1 1H $13.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
cmp.l.f0.0(16)  g122<1>UD       g120<1,1,0>UD   g116<1,1,0>UD   { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
mov(8)          g79<2>UD        g120<4,4,1>UD                   { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N F@7 };
mov(8)          g81<2>UD        g121<4,4,1>UD                   { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
add3(16)        g124<1>D        -g118<8,8,1>D   g114<8,8,1>D    -g122<1,1,1>D { align1 1H };
mov(8)          g79.1<2>UD      g124<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g81.1<2>UD      g125<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g79UD           g83UD           0x08040595                0x00000080
                            ugm MsgDesc: ( atomic_fmin, a64, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $13 };

LABEL18:
endif(16)       JIP:  LABEL17                                   { align1 1H };
add(16)         g19<1>D         g19<1,1,0>D     512D            { align1 1H compacted };

LABEL17:
while(16)       JIP:  LABEL19                                   { align1 1H };

LABEL22:
cmp.ge.f0.0(16) null<1>UD       g48<8,8,1>UD    0x00000030UD    { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL20       UIP:  LABEL20             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
shl(16)         g125<1>D        g48<8,8,1>D     0x00000002UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
add(16)         g84<1>D         g125<1,1,0>D    1152D           { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        g89UD           g84UD           nullUD          0x04201502                0x00000000
                            slm MsgDesc: ( load_cmask, a32, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1H $13 };
cmp.nz.f0.0(16) null<1>D        g89<8,8,1>D     0D              { align1 1H $13.dst };
(+f0.0) if(16)  JIP:  LABEL21         UIP:  LABEL21             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@5 };
add(8)          g1<1>D          g38<8,4,2>D     12504D          { align1 1Q };
add(8)          g2<1>D          g50<8,4,2>D     12504D          { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
cmp.l.f0.0(16)  g3<1>UD         g1<8,8,1>UD     0x000030d8UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@3 };
add(16)         g5<1>D          g1<1,1,0>D      g125<1,1,0>D    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
cmp.l.f0.0(16)  g7<1>UD         g5<1,1,0>UD     g1<1,1,0>UD     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@5 };
mov(8)          g85<2>UD        g5<4,4,1>UD                     { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N F@5 };
mov(8)          g87<2>UD        g6<4,4,1>UD                     { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
add3(16)        g9<1>D          -g3<8,8,1>D     g114<8,8,1>D    -g7<1,1,1>D { align1 1H };
mov(8)          g87.1<2>UD      g10<4,4,1>UD                    { align1 2Q I@1 };
mov(8)          g85.1<2>UD      g9<4,4,1>UD                     { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g85UD           g89UD           0x0804058c                0x00000080
                            ugm MsgDesc: ( atomic_add, a64, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $13 };

LABEL21:
endif(16)       JIP:  LABEL20                                   { align1 1H };
add(16)         g48<1>D         g48<1,1,0>D     512D            { align1 1H compacted };

LABEL20:
while(16)       JIP:  LABEL22                                   { align1 1H };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q I@7 };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_build_BFS_BFS_pass1_initial_code[] = {
    0x80000065, 0x68058220, 0x02000004, 0xffffffc0,
    0xe0490065, 0x0ff10043, 0xe2691a40, 0x00016803,
    0x80030061, 0x27054410, 0x00000000, 0x76543210,
    0x00041b69, 0x4b058660, 0x02464905, 0x00000004,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x80004031, 0x01140000, 0xfa00690c, 0x00340000,
    0x64271a40, 0x00802795, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00040061, 0x4d050160,
    0x00462705, 0x00000000, 0xa04f1940, 0x4b004d02,
    0xe0301965, 0x1ff04f03, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x21261b61, 0x001102cc,
    0x00130061, 0x33260aa0, 0x00000264, 0x00000000,
    0x00030061, 0x5a260aa0, 0x00000264, 0x00000000,
    0x21261361, 0x00110204, 0x2a331361, 0x00110244,
    0x215a1361, 0x00110244, 0xaa351240, 0x034e3303,
    0xaa390040, 0x008e3303, 0xa16c1140, 0x034e5a03,
    0xa1770040, 0x008e5a03, 0x00131c70, 0x36050220,
    0x52463505, 0x00443306, 0x00131c70, 0x3a050220,
    0x52463905, 0x00443306, 0x00130061, 0x2c060220,
    0x00343905, 0x00000000, 0x00031d70, 0x6d050220,
    0x52466c05, 0x00445a06, 0x00031d61, 0x2a060220,
    0x00347705, 0x00000000, 0x00030070, 0x78050220,
    0x52467705, 0x00445a06, 0x00131e40, 0x37052660,
    0x06463605, 0x00443326, 0x00131e40, 0x3b052660,
    0x06463a05, 0x00443326, 0x00031d40, 0x76052660,
    0x06466d05, 0x00445a26, 0x00031c40, 0x79052660,
    0x06467805, 0x00445a26, 0x00131b61, 0x2c260220,
    0x00343b05, 0x00000000, 0x2a321c61, 0x001102cc,
    0x00031a61, 0x2a260220, 0x00347905, 0x00000000,
    0x2a321161, 0x00110204, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044131, 0x3c440000,
    0xfb042a24, 0x003c0000, 0xa0512140, 0x02004003,
    0x27531970, 0x40005103, 0x00033161, 0x2b060220,
    0x00345105, 0x00000000, 0x00133161, 0x2d060220,
    0x00345205, 0x00000000, 0xa055b140, 0x42025302,
    0x00031961, 0x2b260220, 0x00345505, 0x00000000,
    0x00131a61, 0x2d260220, 0x00345605, 0x00000000,
    0xa0560040, 0x03004003, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x03440000,
    0xfb042b24, 0x003c0000, 0xe7581970, 0x03005603,
    0x00033261, 0x2c060220, 0x00345605, 0x00000000,
    0x00133261, 0x2e060220, 0x00345705, 0x00000000,
    0xa05a1b40, 0x42025802, 0x00031961, 0x2c260220,
    0x00345a05, 0x00000000, 0x00131a61, 0x2e260220,
    0x00345b05, 0x00000000, 0x205c2240, 0x03200900,
    0x00041162, 0x62058aa0, 0x4a465c05, 0x0704ec3d,
    0x00041170, 0x6e058aa0, 0x5a466205, 0x77f684df,
    0x00044338, 0x65050aa0, 0x1a466205, 0x00460001,
    0x00040070, 0x70058aa0, 0x3a465c05, 0x0704ec3d,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x5b240000, 0xfb042c24, 0x000c0000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x00042341, 0x67058aa0, 0x0a466505, 0x417d70a4,
    0x00041265, 0x00010220, 0x22466e05, 0x00467005,
    0x00033161, 0x29060220, 0x00346c05, 0x00000000,
    0x80103401, 0x00000000, 0x00000000, 0x00000000,
    0x00133261, 0x2b060220, 0x00343505, 0x00000000,
    0xef0b1162, 0x00006703, 0x00031b61, 0x29260220,
    0x00347605, 0x00000000, 0x00131b61, 0x2b260220,
    0x00343705, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044431, 0x38140000,
    0xfb042924, 0x00040000, 0x00043461, 0x28050220,
    0x00000024, 0x00000000, 0x00041969, 0x44058660,
    0x02462805, 0x00000009, 0xa0461940, 0x20004403,
    0xa0500040, 0x30004402, 0x80002201, 0x00000000,
    0x00000000, 0x00000000, 0x205f2440, 0x05205b00,
    0x80002201, 0x00000000, 0x00000000, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x20612440, 0x07205d00, 0x00040a70, 0x71058aa0,
    0x3a465f05, 0x0704ec3d, 0x00043362, 0x63058aa0,
    0x4a465f05, 0x0704ec3d, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0x00044538, 0x68050aa0,
    0x1a466305, 0x00460001, 0x00043570, 0x73058aa0,
    0x5a466305, 0x77f684df, 0x00041462, 0x64058aa0,
    0x4a466105, 0x0704ec3d, 0x2748a462, 0x38004603,
    0x00042541, 0x6a058aa0, 0x0a466805, 0x417d70a4,
    0x00041365, 0x00010220, 0x22467305, 0x00467105,
    0xef741162, 0x00006a03, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0x00044638, 0x6b050aa0,
    0x1a466405, 0x00460001, 0x00041f70, 0x76058aa0,
    0x3a466105, 0x0704ec3d, 0x00043670, 0x78058aa0,
    0x5a466405, 0x77f684df, 0x00042641, 0x6d058aa0,
    0x0a466b05, 0x417d70a4, 0x00041265, 0x00010220,
    0x22467805, 0x00467605, 0xef791162, 0x00006d03,
    0x277b1d70, 0x48005003, 0x01040022, 0x0001c060,
    0x00000160, 0x00000160, 0x00040069, 0x7d058660,
    0x02465005, 0x00000005, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0xa0019140, 0x7d003c02,
    0xe07e0068, 0x01b05003, 0x27281a70, 0x3c000103,
    0xa02c3440, 0x01000103, 0x00030061, 0x17060220,
    0x00340105, 0x00000000, 0x00130061, 0x19060220,
    0x00340205, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x0004c152, 0x2a040e68,
    0x0e2e3e05, 0x28057e05, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x272e1c70, 0x01002c03,
    0x00030061, 0x1d060220, 0x00342c05, 0x00000000,
    0x00130061, 0x1f060220, 0x00342d05, 0x00000000,
    0x00131c61, 0x19260220, 0x00342b05, 0x00000000,
    0x00031d61, 0x17260220, 0x00342a05, 0x00000000,
    0xa0151d40, 0x2a022e02, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044431, 0x28440000,
    0xfb041724, 0x003c0000, 0x00031961, 0x1d260220,
    0x00341505, 0x00000000, 0x00131a61, 0x1f260220,
    0x00341605, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044731, 0x15440000,
    0xfb041d24, 0x003c0000, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x00040069, 0x0f058660,
    0x02463005, 0x00000002, 0x00041961, 0x0d050220,
    0x00460f05, 0x00000000, 0x00041970, 0x00018220,
    0x42460d05, 0x00000120, 0x01040028, 0x0001c660,
    0x00000090, 0x00000090, 0x00043769, 0x11058660,
    0x02460d05, 0x00000002, 0x00043761, 0x1d054220,
    0x00000000, 0x7f800000, 0x00043761, 0x1f054220,
    0x00000000, 0x7f800000, 0x00043761, 0x21054220,
    0x00000000, 0x7f800000, 0x00043761, 0x23054220,
    0x00000000, 0x7f800000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044731, 0x00000000,
    0xea0c1114, 0x003c1d44, 0x00040040, 0x0d058660,
    0x06460d05, 0x00000800, 0x00040027, 0x00014060,
    0x00000000, 0xffffff60, 0x00041970, 0x00018220,
    0x42460f05, 0x00000030, 0x01040028, 0x0001c660,
    0x000000a8, 0x000000a8, 0x00043769, 0x1d058660,
    0x02460f05, 0x00000002, 0x00043761, 0x20054220,
    0x00000000, 0x00000000, 0x00043761, 0x22054220,
    0x00000000, 0x00000000, 0x00043761, 0x24054220,
    0x00000000, 0x00000000, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0xa0121c40, 0x48001d03,
    0x00043761, 0x1e054220, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044731, 0x00000000, 0xea0c1214, 0x003c1e44,
    0x00040040, 0x0f058660, 0x06460f05, 0x00000800,
    0x00040027, 0x00014060, 0x00000000, 0xffffff48,
    0x80000001, 0x00000000, 0x30000000, 0x00000000,
    0x80000001, 0x00000000, 0x30000000, 0x00000000,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x80004831, 0x1e0c0000, 0xe23e000c, 0x00000000,
    0x80002801, 0x00000000, 0x00000000, 0x00000000,
    0x80033761, 0x1f054220, 0x00000000, 0x00000000,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0x80011961, 0x1f550000, 0x0000005c, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x80044931, 0x00000000, 0x30081f0c, 0x00000000,
    0x80040001, 0x00000000, 0xe0000000, 0x00000000,
    0x80002701, 0x00000000, 0x00000000, 0x00000000,
    0x20202440, 0x15002800, 0x80002701, 0x00000000,
    0x00000000, 0x00000000, 0x20222440, 0x17002a00,
    0x80002701, 0x00000000, 0x00000000, 0x00000000,
    0x20242440, 0x19002c00, 0x200d0b40, 0x03202000,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x20100a40, 0x07202400, 0x80000061, 0x7d064210,
    0x00000000, 0x00000000, 0x20340941, 0x79001000,
    0x200f0c40, 0x05202200, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0x20111441, 0x0b000d00,
    0x00041361, 0x3a050a20, 0x00463405, 0x00000000,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x20131241, 0x74000f00, 0x00041261, 0x36050a20,
    0x00461105, 0x00000000, 0x00041161, 0x38050a20,
    0x00461305, 0x00000000, 0x80001c61, 0x30010110,
    0x00007d04, 0x00000000, 0x00040070, 0x00018220,
    0x22467b05, 0x00000000, 0x00041161, 0x10050120,
    0x00003000, 0x00000000, 0x80001461, 0x7a054660,
    0x00000000, 0x00000001, 0xe03c2165, 0x00304d03,
    0xe03e2168, 0x00204d03, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x00040069, 0x40050660,
    0x02007a04, 0x00464d05, 0x00041a70, 0x00018660,
    0x16461005, 0x00000000, 0x01040028, 0x0001c660,
    0x000008b8, 0x000008b8, 0x0004004c, 0x42050220,
    0x00461005, 0x00000000, 0x80003901, 0x00000000,
    0x00000000, 0x00000000, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x80041961, 0x10014110,
    0x00000000, 0x06c006c0, 0x00040069, 0x10018510,
    0x01564206, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x06c006c0, 0xe0440961, 0x001b0004,
    0xac461970, 0x36004402, 0x00041965, 0x00010220,
    0x22464605, 0x00467b05, 0x01040062, 0x48058220,
    0x02461505, 0xff800000, 0x01040062, 0x4a058220,
    0x02461705, 0xff800000, 0x80001f01, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x4c058220,
    0x02462805, 0x7f800000, 0x01040062, 0x4e058220,
    0x02462a05, 0x7f800000, 0xa3571c61, 0xff810000,
    0x60570061, 0x00104800, 0xa3591b61, 0xff810000,
    0x60590061, 0x00104a00, 0xa3510a61, 0x7f810000,
    0x60510061, 0x00104c00, 0xa3531961, 0x7f810000,
    0x60530061, 0x00104e00, 0x80001201, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x4b058220,
    0x02461905, 0xff800000, 0x80001101, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x4f058220,
    0x02462c05, 0x7f800000, 0x80031461, 0x62060220,
    0x00445726, 0x00000000, 0x80031361, 0x6f060220,
    0x00445926, 0x00000000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x80031261, 0x01060220,
    0x00445126, 0x00000000, 0x80003901, 0x00000000,
    0x00000000, 0x00000000, 0x80031161, 0x1f060220,
    0x00445326, 0x00000000, 0xa35b1e61, 0xff810000,
    0x605b0061, 0x00104b00, 0xa3550d61, 0x7f810000,
    0x60550061, 0x00104f00, 0x00040070, 0x00018660,
    0x16463c05, 0x00000001, 0x80031d62, 0x60060aa0,
    0x4a445706, 0x00446206, 0x80031c62, 0x6d060aa0,
    0x4a445906, 0x00446f06, 0x80031b62, 0x7e060aa0,
    0x5a445106, 0x00440106, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0x80031a62, 0x1d060aa0,
    0x5a445306, 0x00441f06, 0x80031561, 0x48060220,
    0x00445526, 0x00000000, 0x80030c61, 0x57260220,
    0x00446006, 0x00000000, 0x80030b61, 0x59260220,
    0x00446d06, 0x00000000, 0x80030a61, 0x51260220,
    0x00447e06, 0x00000000, 0x80030961, 0x53260220,
    0x00441d06, 0x00000000, 0x80031261, 0x01060220,
    0x00445b26, 0x00000000, 0x80031e62, 0x42060aa0,
    0x5a445506, 0x00444806, 0x80021d61, 0x67070220,
    0x00425747, 0x00000000, 0x80020061, 0x65070220,
    0x00425727, 0x00000000, 0x80021e61, 0x74070220,
    0x00425947, 0x00000000, 0x80020061, 0x72070220,
    0x00425927, 0x00000000, 0x80021f61, 0x06070220,
    0x00425147, 0x00000000, 0x80020061, 0x04070220,
    0x00425127, 0x00000000, 0x80021f61, 0x24070220,
    0x00425347, 0x00000000, 0x80020061, 0x22070220,
    0x00425327, 0x00000000, 0x80031f62, 0x7d060aa0,
    0x4a445b06, 0x00440106, 0x80031261, 0x55260220,
    0x00444206, 0x00000000, 0x80021f62, 0x63070aa0,
    0x4a426507, 0x00426707, 0x80021e62, 0x70070aa0,
    0x4a427207, 0x00427407, 0x80021a62, 0x20070aa0,
    0x5a422207, 0x00422407, 0x80030c61, 0x5b260220,
    0x00447d06, 0x00000000, 0x80021a61, 0x4d070220,
    0x00425547, 0x00000000, 0x80020061, 0x4b070220,
    0x00425527, 0x00000000, 0x80021361, 0x57470220,
    0x00426307, 0x00000000, 0x80021f62, 0x02070aa0,
    0x5a420407, 0x00420607, 0x80021361, 0x59470220,
    0x00427007, 0x00000000, 0x80021261, 0x53470220,
    0x00422007, 0x00000000, 0x80021c62, 0x49070aa0,
    0x5a424b07, 0x00424d07, 0x80021b61, 0x6c070220,
    0x00425767, 0x00000000, 0x80020061, 0x6a070220,
    0x00425727, 0x00000000, 0x80021261, 0x51470220,
    0x00420207, 0x00000000, 0x80020a61, 0x04070220,
    0x00425b27, 0x00000000, 0x80021261, 0x06070220,
    0x00425b47, 0x00000000, 0x80021f61, 0x79070220,
    0x00425967, 0x00000000, 0x80020061, 0x77070220,
    0x00425927, 0x00000000, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x80021f61, 0x34070220,
    0x00425367, 0x00000000, 0x80023a61, 0x12070220,
    0x00425327, 0x00000000, 0x80021161, 0x55470220,
    0x00424907, 0x00000000, 0x80021f62, 0x68070aa0,
    0x4a426a07, 0x00426c07, 0x80021f61, 0x0b070220,
    0x00425167, 0x00000000, 0x80020061, 0x09070220,
    0x00425127, 0x00000000, 0x80021f62, 0x02070aa0,
    0x4a420407, 0x00420607, 0x80021e62, 0x75070aa0,
    0x4a427707, 0x00427907, 0x80021c62, 0x0d070aa0,
    0x5a421207, 0x00423407, 0x80021b61, 0x5f070220,
    0x00425567, 0x00000000, 0x80020061, 0x5d070220,
    0x00425527, 0x00000000, 0x80021461, 0x57670220,
    0x00426807, 0x00000000, 0x80021361, 0x5b470220,
    0x00420207, 0x00000000, 0x80021d62, 0x07070aa0,
    0x5a420907, 0x00420b07, 0x80021361, 0x59670220,
    0x00427507, 0x00000000, 0x80021261, 0x53670220,
    0x00420d07, 0x00000000, 0x80021d62, 0x4e070aa0,
    0x5a425d07, 0x00425f07, 0x80021c62, 0x57850aa0,
    0x4a005764, 0x00345785, 0x80021c62, 0x58850aa0,
    0x4a005864, 0x00345885, 0x80021461, 0x51670220,
    0x00420707, 0x00000000, 0x80020c61, 0x09070220,
    0x00425b27, 0x00000000, 0x80021461, 0x0b070220,
    0x00425b67, 0x00000000, 0x80021d62, 0x59850aa0,
    0x4a005964, 0x00345985, 0x80021d62, 0x5a850aa0,
    0x4a005a64, 0x00345a85, 0x80021c62, 0x53850aa0,
    0x5a005364, 0x00345385, 0x80021c62, 0x54850aa0,
    0x5a005464, 0x00345485, 0x80021761, 0x55670220,
    0x00424e07, 0x00000000, 0x80031562, 0x58050aa0,
    0x4a0057e4, 0x00465805, 0x80021c62, 0x51850aa0,
    0x5a005164, 0x00345185, 0x80021c62, 0x52850aa0,
    0x5a005264, 0x00345285, 0x80021a62, 0x07070aa0,
    0x4a420907, 0x00420b07, 0x80031762, 0x5a050aa0,
    0x4a0059e4, 0x00465a05, 0x80031662, 0x54050aa0,
    0x5a0053e4, 0x00465405, 0x80021962, 0x55850aa0,
    0x5a005564, 0x00345585, 0x80021962, 0x56850aa0,
    0x5a005664, 0x00345685, 0x80031662, 0x52050aa0,
    0x5a0051e4, 0x00465205, 0x80021661, 0x5b670220,
    0x00420707, 0x00000000, 0x80001501, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x5f050220,
    0x02005ae4, 0x000058e4, 0x80031262, 0x56050aa0,
    0x5a0055e4, 0x00465605, 0x80001201, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x5d050220,
    0x020054e4, 0x000052e4, 0x80021b62, 0x5b850aa0,
    0x4a005b64, 0x00345b85, 0x80021b62, 0x5c850aa0,
    0x4a005c64, 0x00345c85, 0x00040070, 0x00018660,
    0x16463c05, 0x00000002, 0x80031162, 0x5c050aa0,
    0x4a005be4, 0x00465c05, 0x80000a01, 0x00000000,
    0x00000000, 0x00000000, 0x2f610062, 0x5d015683,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x2f630062, 0x5f015c83, 0x00040070, 0x00018660,
    0x16463e05, 0x00000000, 0x2f341a62, 0x63206100,
    0x00040065, 0x00018220, 0x22464005, 0x00000077,
    0x01040022, 0x0001c060, 0x00000090, 0x00000090,
    0x80001101, 0x00000000, 0x00000000, 0x00000000,
    0x60640041, 0x00604402, 0x60660041, 0x00303e02,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00040052, 0x68040e68, 0x0e0e6405, 0x66053c05,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00043a69, 0x13058660, 0x02466805, 0x00000002,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00044a31, 0x00000000, 0xea2a1314, 0x01003414,
    0x00040025, 0x00004600, 0x00000000, 0x00000070,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0xa0690066, 0x7b224602, 0x80000061, 0x0c064210,
    0x00000000, 0x00000000, 0x80001961, 0x30010110,
    0x00000c04, 0x00000000, 0x00041b70, 0x00018220,
    0x22466905, 0x00000000, 0x00040061, 0x6a050120,
    0x00003000, 0x00000000, 0x20101965, 0x6a001003,
    0x00040027, 0x00014060, 0x00000000, 0xfffff738,
    0x80000c01, 0x00000000, 0x00000000, 0x00000000,
    0xa06b0040, 0x01003803, 0x80003761, 0x1d064210,
    0x00000000, 0x00000000, 0x80001961, 0x30010110,
    0x00001d04, 0x00000000, 0x00040070, 0x00018220,
    0x22467b05, 0x00000000, 0x80001e01, 0x00000000,
    0x00000000, 0x00000000, 0x00040061, 0x11050120,
    0x00003000, 0x00000000, 0x00041970, 0x00018660,
    0x16461105, 0x00000000, 0x01040028, 0x0001c660,
    0x00000928, 0x00000928, 0x0004004c, 0x6d050220,
    0x00461105, 0x00000000, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x80040061, 0x10014110,
    0x00000000, 0x0d600d60, 0x00040069, 0x10018510,
    0x01566d06, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0d600d60, 0xe06f0961, 0x001b0004,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0xac710070, 0x6b006f02, 0x00041965, 0x00010220,
    0x22467105, 0x00467b05, 0x80001101, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x73058220,
    0x02461505, 0xff800000, 0x80001101, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x75058220,
    0x02461705, 0xff800000, 0x01040062, 0x77058220,
    0x02462805, 0x7f800000, 0x80001401, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x79058220,
    0x02462a05, 0x7f800000, 0x01040062, 0x7d058220,
    0x02462c05, 0x7f800000, 0xa3070d61, 0xff810000,
    0x60070061, 0x00107300, 0xa3091c61, 0xff810000,
    0x60090061, 0x00107500, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0xa3010b61, 0x7f810000,
    0x60010061, 0x00107700, 0xa3030a61, 0x7f810000,
    0x60030061, 0x00107900, 0x80001201, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x76058220,
    0x02461905, 0xff800000, 0xa3050a61, 0x7f810000,
    0x60050061, 0x00107d00, 0x80031561, 0x5e060220,
    0x00440726, 0x00000000, 0x80031461, 0x6d060220,
    0x00440926, 0x00000000, 0x80031361, 0x20060220,
    0x00440126, 0x00000000, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x80031261, 0x44060220,
    0x00440326, 0x00000000, 0xa30b0d61, 0xff810000,
    0x600b0061, 0x00107600, 0x00040070, 0x00018660,
    0x16463c05, 0x00000001, 0x80031261, 0x51060220,
    0x00440526, 0x00000000, 0x80031e62, 0x5c060aa0,
    0x4a440706, 0x00445e06, 0x80031d62, 0x69060aa0,
    0x4a440906, 0x00446d06, 0x80003901, 0x00000000,
    0x00000000, 0x00000000, 0x80031c62, 0x1e060aa0,
    0x5a440106, 0x00442006, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x80031b62, 0x42060aa0,
    0x5a440306, 0x00444406, 0x80031962, 0x4f060aa0,
    0x5a440506, 0x00445106, 0x80030d61, 0x07260220,
    0x00445c06, 0x00000000, 0x80030c61, 0x09260220,
    0x00446906, 0x00000000, 0x80030b61, 0x01260220,
    0x00441e06, 0x00000000, 0x80030a61, 0x03260220,
    0x00444206, 0x00000000, 0x80031361, 0x20060220,
    0x00440b26, 0x00000000, 0x80030961, 0x05260220,
    0x00444f06, 0x00000000, 0x80021e61, 0x63070220,
    0x00420747, 0x00000000, 0x80020061, 0x61070220,
    0x00420727, 0x00000000, 0x80020e61, 0x77070220,
    0x00420947, 0x00000000, 0x80021661, 0x75070220,
    0x00420927, 0x00000000, 0x80021f61, 0x0d070220,
    0x00420147, 0x00000000, 0x80020061, 0x23070220,
    0x00420127, 0x00000000, 0x80021f61, 0x49070220,
    0x00420347, 0x00000000, 0x80020061, 0x47070220,
    0x00420327, 0x00000000, 0x80031f62, 0x1e060aa0,
    0x4a440b06, 0x00442006, 0x80021f61, 0x56070220,
    0x00420547, 0x00000000, 0x80020061, 0x54070220,
    0x00420527, 0x00000000, 0x80021f62, 0x5f070aa0,
    0x4a426107, 0x00426307, 0x80021f62, 0x73070aa0,
    0x4a427507, 0x00427707, 0x80021b62, 0x45070aa0,
    0x5a424707, 0x00424907, 0x80031461, 0x0b260220,
    0x00441e06, 0x00000000, 0x80021a62, 0x52070aa0,
    0x5a425407, 0x00425607, 0x80021461, 0x07470220,
    0x00425f07, 0x00000000, 0x80021f62, 0x21070aa0,
    0x5a422307, 0x00420d07, 0x80021461, 0x09470220,
    0x00427307, 0x00000000, 0x80021361, 0x03470220,
    0x00424507, 0x00000000, 0x80021261, 0x05470220,
    0x00425207, 0x00000000, 0x80021c61, 0x68070220,
    0x00420767, 0x00000000, 0x80020061, 0x66070220,
    0x00420727, 0x00000000, 0x80021161, 0x01470220,
    0x00422107, 0x00000000, 0x80021e61, 0x1d070220,
    0x00420967, 0x00000000, 0x80020061, 0x7d070220,
    0x00420927, 0x00000000, 0x80020961, 0x23070220,
    0x00420b27, 0x00000000, 0x80021161, 0x0d070220,
    0x00420b47, 0x00000000, 0x80021f61, 0x4e070220,
    0x00420367, 0x00000000, 0x80020061, 0x4c070220,
    0x00420327, 0x00000000, 0x80021f61, 0x5b070220,
    0x00420567, 0x00000000, 0x80020061, 0x59070220,
    0x00420527, 0x00000000, 0x80021f62, 0x64070aa0,
    0x4a426607, 0x00426807, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x80021f61, 0x34070220,
    0x00420167, 0x00000000, 0x80023a61, 0x13070220,
    0x00420127, 0x00000000, 0x80021f62, 0x78070aa0,
    0x4a427d07, 0x00421d07, 0x80021f62, 0x21070aa0,
    0x4a422307, 0x00420d07, 0x80021d62, 0x4a070aa0,
    0x5a424c07, 0x00424e07, 0x80021b62, 0x57070aa0,
    0x5a425907, 0x00425b07, 0x80021561, 0x07670220,
    0x00426407, 0x00000000, 0x80021461, 0x09670220,
    0x00427807, 0x00000000, 0x80021361, 0x0b470220,
    0x00422107, 0x00000000, 0x80021c62, 0x0e070aa0,
    0x5a421307, 0x00423407, 0x80021361, 0x03670220,
    0x00424a07, 0x00000000, 0x80021261, 0x05670220,
    0x00425707, 0x00000000, 0x80021d62, 0x07850aa0,
    0x4a000764, 0x00340785, 0x80021d62, 0x08850aa0,
    0x4a000864, 0x00340885, 0x80021c62, 0x09850aa0,
    0x4a000964, 0x00340985, 0x80021c62, 0x0a850aa0,
    0x4a000a64, 0x00340a85, 0x80021561, 0x01670220,
    0x00420e07, 0x00000000, 0x80020c61, 0x13070220,
    0x00420b27, 0x00000000, 0x80021561, 0x34070220,
    0x00420b67, 0x00000000, 0x80021d62, 0x03850aa0,
    0x5a000364, 0x00340385, 0x80021d62, 0x04850aa0,
    0x5a000464, 0x00340485, 0x80021c62, 0x05850aa0,
    0x5a000564, 0x00340585, 0x80021c62, 0x06850aa0,
    0x5a000664, 0x00340685, 0x80031762, 0x08050aa0,
    0x4a0007e4, 0x00460805, 0x80031662, 0x0a050aa0,
    0x4a0009e4, 0x00460a05, 0x80021b62, 0x01850aa0,
    0x5a000164, 0x00340185, 0x80021b62, 0x02850aa0,
    0x5a000264, 0x00340285, 0x80021962, 0x0e070aa0,
    0x4a421307, 0x00423407, 0x80031762, 0x04050aa0,
    0x5a0003e4, 0x00460405, 0x80031762, 0x06050aa0,
    0x5a0005e4, 0x00460605, 0x80001601, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x1f050220,
    0x02000ae4, 0x000008e4, 0x80031462, 0x02050aa0,
    0x5a0001e4, 0x00460205, 0x80021461, 0x0b670220,
    0x00420e07, 0x00000000, 0x80001101, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x1d050220,
    0x020004e4, 0x000002e4, 0x80021a62, 0x0b850aa0,
    0x4a000b64, 0x00340b85, 0x80021a62, 0x0c850aa0,
    0x4a000c64, 0x00340c85, 0x00040070, 0x00018660,
    0x16463c05, 0x00000002, 0x80031162, 0x0c050aa0,
    0x4a000be4, 0x00460c05, 0x80000a01, 0x00000000,
    0x00000000, 0x00000000, 0x2f210062, 0x1d010683,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x2f230062, 0x1f010c83, 0x00040070, 0x00018660,
    0x16463e05, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x2f440062, 0x23202100,
    0x00040065, 0x00018220, 0x22464005, 0x00000077,
    0x01040022, 0x0001c060, 0x00000090, 0x00000090,
    0x80001101, 0x00000000, 0x00000000, 0x00000000,
    0x60240041, 0x00606f02, 0x80001f01, 0x00000000,
    0x00000000, 0x00000000, 0x600d0041, 0x00303e02,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00040052, 0x0f040e68, 0x0e0e2405, 0x0d053c05,
    0x00041969, 0x42058660, 0x02460f05, 0x00000002,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00044b31, 0x00000000, 0xea2a4214, 0x01004414,
    0x00040025, 0x00004600, 0x00000000, 0x00000080,
    0x80001701, 0x00000000, 0x00000000, 0x00000000,
    0xa0130066, 0x7b227102, 0x80001761, 0x35064210,
    0x00000000, 0x00000000, 0x80001961, 0x30010110,
    0x00003504, 0x00000000, 0x00041b70, 0x00018220,
    0x22461305, 0x00000000, 0x80001701, 0x00000000,
    0x00000000, 0x00000000, 0x00040061, 0x34050120,
    0x00003000, 0x00000000, 0x20111965, 0x34001103,
    0x00040027, 0x00014060, 0x00000000, 0xfffff6c8,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0xa0420040, 0x02003a03, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x80001161, 0x44064210,
    0x00000000, 0x00000000, 0x80001961, 0x30010110,
    0x00004404, 0x00000000, 0x00040070, 0x00018220,
    0x22467b05, 0x00000000, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x80000d01, 0x00000000,
    0x00000000, 0x00000000, 0x00040061, 0x12050120,
    0x00003000, 0x00000000, 0x00041970, 0x00018660,
    0x16461205, 0x00000000, 0x01040028, 0x0001c660,
    0x000008d8, 0x000008d8, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x80001101, 0x00000000,
    0x00000000, 0x00000000, 0x0004004c, 0x44050220,
    0x00461205, 0x00000000, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x80003901, 0x00000000,
    0x00000000, 0x00000000, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x80040061, 0x10014110,
    0x00000000, 0x08400840, 0x00040069, 0x10018510,
    0x01564406, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x08400840, 0xe0460961, 0x001b0004,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0xac480070, 0x42004602, 0x00041965, 0x00010220,
    0x22464805, 0x00467b05, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x80001101, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x4a058220,
    0x02461505, 0xff800000, 0x01040062, 0x4c058220,
    0x02461705, 0xff800000, 0x01040062, 0x4e058220,
    0x02462805, 0x7f800000, 0x80001601, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x50058220,
    0x02462a05, 0x7f800000, 0xa3590c61, 0xff810000,
    0x60590061, 0x00104a00, 0xa35b1b61, 0xff810000,
    0x605b0061, 0x00104c00, 0xa3530a61, 0x7f810000,
    0x60530061, 0x00104e00, 0xa3550961, 0x7f810000,
    0x60550061, 0x00105000, 0x80001201, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x4d058220,
    0x02461905, 0xff800000, 0x80001101, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x51058220,
    0x02462c05, 0x7f800000, 0x80031461, 0x06060220,
    0x00445926, 0x00000000, 0x80031361, 0x24060220,
    0x00445b26, 0x00000000, 0x80031161, 0x69060220,
    0x00445526, 0x00000000, 0xa35d0d61, 0xff810000,
    0x605d0061, 0x00104d00, 0x80031261, 0x4f060220,
    0x00445326, 0x00000000, 0xa3570d61, 0x7f810000,
    0x60570061, 0x00105100, 0x00040070, 0x00018660,
    0x16463c05, 0x00000001, 0x80030d62, 0x04060aa0,
    0x4a445906, 0x00440606, 0x80030c62, 0x22060aa0,
    0x4a445b06, 0x00442406, 0x80031b62, 0x67060aa0,
    0x5a445506, 0x00446906, 0x80031a62, 0x4a060aa0,
    0x5a445306, 0x00444f06, 0x80031561, 0x76060220,
    0x00445726, 0x00000000, 0x80030c61, 0x59260220,
    0x00440406, 0x00000000, 0x80030b61, 0x5b260220,
    0x00442206, 0x00000000, 0x80030a61, 0x55260220,
    0x00446706, 0x00000000, 0x80030961, 0x53260220,
    0x00444a06, 0x00000000, 0x80031161, 0x4f060220,
    0x00445d26, 0x00000000, 0x80031e62, 0x74060aa0,
    0x5a445706, 0x00447606, 0x80021d61, 0x0b070220,
    0x00425947, 0x00000000, 0x80020061, 0x09070220,
    0x00425927, 0x00000000, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x80021e61, 0x34070220,
    0x00425b47, 0x00000000, 0x80020061, 0x0f070220,
    0x00425b27, 0x00000000, 0x80021f61, 0x6e070220,
    0x00425547, 0x00000000, 0x80020061, 0x6c070220,
    0x00425527, 0x00000000, 0x80021f61, 0x61070220,
    0x00425347, 0x00000000, 0x80020061, 0x5f070220,
    0x00425327, 0x00000000, 0x80031f62, 0x4d060aa0,
    0x4a445d06, 0x00444f06, 0x80031261, 0x57260220,
    0x00447406, 0x00000000, 0x80021f62, 0x07070aa0,
    0x4a420907, 0x00420b07, 0x80021e62, 0x0d070aa0,
    0x4a420f07, 0x00423407, 0x80021c62, 0x6a070aa0,
    0x5a426c07, 0x00426e07, 0x80030c61, 0x5d260220,
    0x00444d06, 0x00000000, 0x80021a61, 0x7d070220,
    0x00425747, 0x00000000, 0x80020061, 0x79070220,
    0x00425727, 0x00000000, 0x80021361, 0x59470220,
    0x00420707, 0x00000000, 0x80021e62, 0x50070aa0,
    0x5a425f07, 0x00426107, 0x80021361, 0x5b470220,
    0x00420d07, 0x00000000, 0x80021261, 0x55470220,
    0x00426a07, 0x00000000, 0x80021c62, 0x77070aa0,
    0x5a427907, 0x00427d07, 0x80021b61, 0x21070220,
    0x00425967, 0x00000000, 0x80023961, 0x1f070220,
    0x00425927, 0x00000000, 0x80021261, 0x53470220,
    0x00425007, 0x00000000, 0x80021d61, 0x4c070220,
    0x00425b67, 0x00000000, 0x80020061, 0x4a070220,
    0x00425b27, 0x00000000, 0x80021261, 0x5f070220,
    0x00425d27, 0x00000000, 0x80021261, 0x61070220,
    0x00425d47, 0x00000000, 0x80021f61, 0x73070220,
    0x00425567, 0x00000000, 0x80020061, 0x71070220,
    0x00425527, 0x00000000, 0x80021161, 0x57470220,
    0x00427707, 0x00000000, 0x80021f62, 0x1d070aa0,
    0x4a421f07, 0x00422107, 0x80021f61, 0x66070220,
    0x00425367, 0x00000000, 0x80020061, 0x64070220,
    0x00425327, 0x00000000, 0x80021f62, 0x44070aa0,
    0x4a424a07, 0x00424c07, 0x80021e62, 0x50070aa0,
    0x4a425f07, 0x00426107, 0x80021c62, 0x6f070aa0,
    0x5a427107, 0x00427307, 0x80021b61, 0x03070220,
    0x00425767, 0x00000000, 0x80022061, 0x01070220,
    0x00425727, 0x00000000, 0x80021461, 0x59670220,
    0x00421d07, 0x00000000, 0x80021361, 0x5b670220,
    0x00424407, 0x00000000, 0x80021261, 0x5d470220,
    0x00425007, 0x00000000, 0x80021e62, 0x62070aa0,
    0x5a426407, 0x00426607, 0x80021261, 0x55670220,
    0x00426f07, 0x00000000, 0x80021d62, 0x7e070aa0,
    0x5a420107, 0x00420307, 0x80021c62, 0x59850aa0,
    0x4a005964, 0x00345985, 0x80021c62, 0x5a850aa0,
    0x4a005a64, 0x00345a85, 0x80021b62, 0x5b850aa0,
    0x4a005b64, 0x00345b85, 0x80021b62, 0x5c850aa0,
    0x4a005c64, 0x00345c85, 0x80021661, 0x53670220,
    0x00426207, 0x00000000, 0x80020b61, 0x64070220,
    0x00425d27, 0x00000000, 0x80021661, 0x66070220,
    0x00425d67, 0x00000000, 0x80021c62, 0x55850aa0,
    0x5a005564, 0x00345585, 0x80021c62, 0x56850aa0,
    0x5a005664, 0x00345685, 0x80021761, 0x57670220,
    0x00427e07, 0x00000000, 0x80031562, 0x5a050aa0,
    0x4a0059e4, 0x00465a05, 0x80031462, 0x5c050aa0,
    0x4a005be4, 0x00465c05, 0x80021c62, 0x53850aa0,
    0x5a005364, 0x00345385, 0x80021c62, 0x54850aa0,
    0x5a005464, 0x00345485, 0x80021a62, 0x62070aa0,
    0x4a426407, 0x00426607, 0x80031662, 0x56050aa0,
    0x5a0055e4, 0x00465605, 0x80021962, 0x57850aa0,
    0x5a005764, 0x00345785, 0x80021962, 0x58850aa0,
    0x5a005864, 0x00345885, 0x80031562, 0x54050aa0,
    0x5a0053e4, 0x00465405, 0x80021561, 0x5d670220,
    0x00426207, 0x00000000, 0x80001701, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x61050220,
    0x02005ce4, 0x00005ae4, 0x80031262, 0x58050aa0,
    0x5a0057e4, 0x00465805, 0x80001201, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x5f050220,
    0x020056e4, 0x000054e4, 0x80021b62, 0x5d850aa0,
    0x4a005d64, 0x00345d85, 0x80021b62, 0x5e850aa0,
    0x4a005e64, 0x00345e85, 0x00040070, 0x00018660,
    0x16463c05, 0x00000002, 0x80031162, 0x5e050aa0,
    0x4a005de4, 0x00465e05, 0x80000a01, 0x00000000,
    0x00000000, 0x00000000, 0x2f630062, 0x5f015883,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x2f650062, 0x61015e83, 0x00040070, 0x00018660,
    0x16463e05, 0x00000000, 0x2f4a1a62, 0x65206300,
    0x00040065, 0x00018220, 0x22464005, 0x00000077,
    0x01040022, 0x0001c060, 0x00000080, 0x00000080,
    0x80001101, 0x00000000, 0x00000000, 0x00000000,
    0x60660041, 0x00604602, 0x60680041, 0x00303e02,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00040052, 0x6a040e68, 0x0e0e6605, 0x68053c05,
    0x00041969, 0x45058660, 0x02466a05, 0x00000002,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00044b31, 0x00000000, 0xea2a4514, 0x01004a14,
    0x00040025, 0x00004600, 0x00000000, 0x00000060,
    0xa06b0066, 0x7b224802, 0x80000c61, 0x67064210,
    0x00000000, 0x00000000, 0x80001961, 0x30010110,
    0x00006704, 0x00000000, 0x00041b70, 0x00018220,
    0x22466b05, 0x00000000, 0x00040061, 0x6c050120,
    0x00003000, 0x00000000, 0x20121965, 0x6c001203,
    0x00040027, 0x00014060, 0x00000000, 0xfffff718,
    0x00040061, 0x00010660, 0x20467b05, 0x00000000,
    0x01040022, 0x0001c060, 0x00000108, 0x00000108,
    0x80001e01, 0x00000000, 0x00000000, 0x00000000,
    0x00040069, 0x6d058660, 0x02463605, 0x00000002,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0xa04b0040, 0x48006d03, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044b31, 0x00000000,
    0xea104b14, 0x01000000, 0x00040069, 0x6e058660,
    0x02463805, 0x00000002, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0xa04c3b40, 0x4c006e03,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044c31, 0x00000000, 0xea104c14, 0x01000000,
    0x00040069, 0x6f058660, 0x02463a05, 0x00000002,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0xa04d3c40, 0x50006f03, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044d31, 0x00000000,
    0xea104d14, 0x01000000, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x80000001, 0x00000000,
    0x30000000, 0x00000000, 0x80000001, 0x00000000,
    0x30000000, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x80004e31, 0x700c0000,
    0xe23e000c, 0x00000000, 0x80002e01, 0x00000000,
    0x00000000, 0x00000000, 0x80030061, 0x71054220,
    0x00000000, 0x00000000, 0x80003e01, 0x00000000,
    0x00000000, 0x00000000, 0x80011961, 0x71550000,
    0x0000005c, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x80044f31, 0x00000000,
    0x3008710c, 0x00000000, 0x80040001, 0x00000000,
    0xe0000000, 0x00000000, 0x80001101, 0x00000000,
    0x00000000, 0x00000000, 0x00030040, 0x74058660,
    0x06442606, 0x00002c58, 0x80101101, 0x00000000,
    0x00000000, 0x00000000, 0x00130040, 0x75058660,
    0x06443206, 0x00002c58, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x00040f61, 0x13050220,
    0x00463005, 0x00000000, 0x00030061, 0x72050220,
    0x00442626, 0x00000000, 0x00130061, 0x73050220,
    0x00443226, 0x00000000, 0x80001c01, 0x00000000,
    0x00000000, 0x00000000, 0x00040070, 0x76058220,
    0x52467405, 0x00002c58, 0x00041a70, 0x00018220,
    0x42461305, 0x00000120, 0x01040028, 0x0001c660,
    0x000001a0, 0x000001a0, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x00040069, 0x4e058660,
    0x02461305, 0x00000002, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0x00044d31, 0x53140000,
    0xea044e14, 0x00040000, 0xae002d70, 0x7f805301,
    0x01040022, 0x0001c060, 0x00000120, 0x00000120,
    0x80001501, 0x00000000, 0x00000000, 0x00000000,
    0xa0783d40, 0x4e007402, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x277a0070, 0x74007803,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x4f060220, 0x00347805, 0x00000000,
    0x80103d01, 0x00000000, 0x00000000, 0x00000000,
    0x80101701, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x51060220, 0x00347905, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00040052, 0x7c042e68, 0x0e2e7605, 0x7a057205,
    0x00031961, 0x4f260220, 0x00347c05, 0x00000000,
    0x00131a61, 0x51260220, 0x00347d05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044d31, 0x00000000, 0xfb2a4f24, 0x01005314,
    0x00040025, 0x00004600, 0x00000000, 0x00000018,
    0xa0130040, 0x20001303, 0x00040027, 0x00014060,
    0x00000000, 0xfffffe50, 0x00041a70, 0x00018220,
    0x42463005, 0x00000030, 0x01040028, 0x0001c660,
    0x00000230, 0x00000230, 0x80001f01, 0x00000000,
    0x00000000, 0x00000000, 0x00040069, 0x7d058660,
    0x02463005, 0x00000002, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0xa0540040, 0x48007d03,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00044d31, 0x59140000, 0xea045414, 0x00040000,
    0x00042d70, 0x00018660, 0x26465905, 0x00000000,
    0x01040022, 0x0001c060, 0x00000180, 0x00000180,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x80001501, 0x00000000, 0x00000000, 0x00000000,
    0x00030040, 0x01058660, 0x06442606, 0x000030d8,
    0x00130040, 0x02058660, 0x06443206, 0x000030d8,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x80001501, 0x00000000, 0x00000000, 0x00000000,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00040070, 0x03058220, 0x52460105, 0x000030d8,
    0x80001301, 0x00000000, 0x00000000, 0x00000000,
    0xa0050040, 0x7d000102, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0x27070070, 0x01000503,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x80001501, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x55060220, 0x00340505, 0x00000000,
    0x80103d01, 0x00000000, 0x00000000, 0x00000000,
    0x80101501, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x57060220, 0x00340605, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00040052, 0x09042e68, 0x0e2e0305, 0x07057205,
    0x00131961, 0x57260220, 0x00340a05, 0x00000000,
    0x00031a61, 0x55260220, 0x00340905, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044d31, 0x00000000, 0xfb185524, 0x01005914,
    0x00040025, 0x00004600, 0x00000000, 0x00000018,
    0xa0300040, 0x20003003, 0x00040027, 0x00014060,
    0x00000000, 0xfffffdc0, 0x80031f61, 0x7e050220,
    0x00460005, 0x00000000, 0x80040931, 0x00000004,
    0x30007e0c, 0x00000000, 0x20000060, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_build_BFS_BFS_pass1_initial = {
   .prog_data = {
      .base.nr_params = 12,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 1344,
      .base.program_size = 11584,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_build_BFS_BFS_pass1_initial_relocs,
      .base.uses_atomic_load_store = false,
      .local_size = { 512, 1, 1 },
      .prog_offset = { 0, 0, 0 },
      .prog_mask = 2,
      .prog_spilled = 0,
      .uses_barrier = true,
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
   .args = gfx125_bvh_build_BFS_BFS_pass1_initial_args,
   .code = gfx125_bvh_build_BFS_BFS_pass1_initial_code,
};
const char *gfx125_bvh_build_BFS_BFS_pass1_initial_sha1 = "0a9cb76d808434c09f22a18449d01f93d17c47f9";
