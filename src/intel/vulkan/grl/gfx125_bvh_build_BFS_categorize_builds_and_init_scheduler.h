#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_build_BFS_categorize_builds_and_init_scheduler_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_build_BFS_categorize_builds_and_init_scheduler_args[] = {
   { 0, 8 },
   { 8, 8 },
   { 16, 8 },
   { 24, 8 },
   { 32, 4 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g115<1>UD       g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g83<1>D         0D                              { align1 1H };
mov.nz.f0.0(16) null<1>UD       g0.1<0,1,0>UD                   { align1 1H };
and(16)         g70<1>UD        g0.2<0,1,0>UD   0x000000ffUD    { align1 1H compacted };
add(1)          g116<1>UD       g115<0,1,0>UD   0x00000000UD    { align1 WE_all 1N I@4 compacted };
add(1)          g117<1>UD       g115<0,1,0>UD   0x00000040UD    { align1 WE_all 1N compacted };
mov(8)          g55<1>UW        0x76543210V                     { align1 WE_all 1Q };
shl(16)         g85<1>D         g70<8,8,1>D     0x00000004UD    { align1 1H I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
send(1)         g1UD            g116UD          nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(1)         g3UD            g117UD          nullUD          0x0210c500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V8, transpose, L1STATE_L3MOCS dst_len = 1, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $1 };
add(8)          g55.8<1>UW      g55<1,1,0>UW    0x0008UW        { align1 WE_all 1Q I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         g87<1>D         g55<8,8,1>UW                    { align1 1H };
add(16)         g89<1>D         g87<1,1,0>D     g85<1,1,0>D     { align1 1H I@1 compacted };
and(16)         g72<1>UD        g89<1,1,0>UD    0x000001ffUD    { align1 1H I@1 compacted };
mov(16)         g74<1>UD        g72<8,8,1>UD                    { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g60.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g98.1<2>F       g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g62.1<2>F       g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g18.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g64.1<2>F       g2.5<0,1,0>F                    { align1 1Q };
mov(8)          g20.1<2>F       g2.5<0,1,0>F                    { align1 2Q };
mov(8)          g66.1<2>F       g2.7<0,1,0>F                    { align1 1Q };
mov(8)          g22.1<2>F       g2.7<0,1,0>F                    { align1 2Q };
mov(8)          g60<2>F         g2<0,1,0>F                      { align1 1Q F@7 compacted };
mov(8)          g98<2>F         g2<0,1,0>F                      { align1 2Q F@7 compacted };
mov(8)          g62<2>F         g2.2<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g18<2>F         g2.2<0,1,0>F                    { align1 2Q F@7 compacted };
mov(8)          g64<2>F         g2.4<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g20<2>F         g2.4<0,1,0>F                    { align1 2Q F@7 compacted };
mov(8)          g66<2>F         g2.6<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g22<2>F         g2.6<0,1,0>F                    { align1 2Q F@7 compacted };
(+f0.0) if(16)  JIP:  LABEL1          UIP:  LABEL0              { align1 1H };
mov(8)          g90<1>UD        g60.1<8,4,2>UD                  { align1 1Q F@7 };
mov(8)          g91<1>UD        g98.1<8,4,2>UD                  { align1 2Q F@7 };
add(8)          g92<1>D         g60<8,4,2>D     40D             { align1 1Q compacted };
add(8)          g93<1>D         g98<8,4,2>D     40D             { align1 2Q compacted };
cmp.l.f0.0(8)   g94<1>UD        g92<8,8,1>UD    g60<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g95<1>UD        g93<8,8,1>UD    g98<8,4,2>UD    { align1 2Q I@2 };

LABEL3:
cmp.ge.f0.0(16) null<1>UD       g72<8,8,1>UD    0x00000100UD    { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL2        UIP:  LABEL2              { align1 1H };
cmp.nz.f0.0(16) g96<1>D         g72<1,1,0>D     0D              { align1 1H compacted };
shl(16)         g16<1>D         g72<8,8,1>D     0x00000002UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(16)         g8<1>D          -g96<8,8,1>D                    { align1 1H I@2 };
add(16)         g100<1>D        g92<1,1,0>D     g16<1,1,0>D     { align1 1H I@2 compacted };
cmp.l.f0.0(16)  g102<1>UD       g100<1,1,0>UD   g92<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g4<2>UD         g100<4,4,1>UD                   { align1 1Q $2.src };
mov(8)          g6<2>UD         g101<4,4,1>UD                   { align1 2Q $2.src };
add3(16)        g104<1>D        -g94<8,8,1>D    g90<8,8,1>D     -g102<1,1,1>D { align1 1H I@3 };
mov(8)          g4.1<2>UD       g104<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g6.1<2>UD       g105<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g4UD            g8UD            0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $2 };
add(16)         g72<1>D         g72<1,1,0>D     512D            { align1 1H compacted };

LABEL2:
while(16)       JIP:  LABEL3                                    { align1 1H };
shl(16)         g58<1>D         g87<8,8,1>D     0x00000002UD    { align1 1H };
mov(16)         g105<1>D        2139095040D                     { align1 1H };
add(8)          g107<1>D        g60<8,4,2>D     11308D          { align1 1Q };
add(8)          g108<1>D        g98<8,4,2>D     11308D          { align1 2Q };
cmp.l.f0.0(8)   g54<1>UD        g107<8,8,1>UD   g60<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g109<1>UD       g108<8,8,1>UD   g98<8,4,2>UD    { align1 2Q I@2 };
add(8)          g110<1>D        -g54<8,8,1>D    g60.1<8,4,2>D   { align1 1Q I@2 };
add(8)          g111<1>D        -g109<8,8,1>D   g98.1<8,4,2>D   { align1 2Q I@2 };

LABEL9:
cmp.ge.f0.0(16) null<1>UD       g70<8,8,1>UD    0x00000100UD    { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL4        UIP:  LABEL4              { align1 1H };
mul(8)          acc0<1>UD       g70<8,8,1>UD    0x05ccUW        { align1 1Q };
mul(16)         g114<1>D        g70<1,1,0>D     1484W           { align1 1H compacted };
mov(16)         g56<1>UD        g58<8,8,1>UD                    { align1 1H I@7 };
mach(8)         g112<1>UD       g70<1,1,0>UD    0x000005ccUD    { align1 1Q compacted AccWrEnable };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
add(16)         g116<1>D        g107<1,1,0>D    g114<1,1,0>D    { align1 1H I@3 compacted };
mul(8)          acc0<1>UD       g71<8,8,1>UD    0x05ccUW        { align1 2Q };
cmp.l.f0.0(16)  g118<1>UD       g116<1,1,0>UD   g107<1,1,0>UD   { align1 1H I@2 compacted };
add(16)         g122<1>D        g116<1,1,0>D    44D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
add(16)         g1<1>D          g116<1,1,0>D    1196D           { align1 1H F@1 compacted };
mach(8)         g113<1>UD       g71<8,8,1>UD    0x000005ccUD    { align1 2Q AccWrEnable };
cmp.l.f0.0(16)  g124<1>UD       g122<1,1,0>UD   g116<1,1,0>UD   { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
cmp.l.f0.0(16)  g4<1>UD         g1<1,1,0>UD     0x000004acUD    { align1 1H I@3 compacted };
add3(16)        g120<1>D        g110<8,8,1>D    g112<8,8,1>D    -g118<1,1,1>D { align1 1H I@3 };
add(16)         g126<1>D        -g124<1,1,0>D   g120<1,1,0>D    { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g6<1>D          -g4<1,1,0>D     g120<1,1,0>D    { align1 1H I@3 compacted };

LABEL6:
cmp.ge.f0.0(16) null<1>UD       g56<8,8,1>UD    0x00000030UD    { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL5        UIP:  LABEL5              { align1 1H };
shl(16)         g8<1>D          g56<8,8,1>D     0x00000002UD    { align1 1H $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g10<1>D         g1<1,1,0>D      g8<1,1,0>D      { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
cmp.l.f0.0(16)  g12<1>UD        g10<1,1,0>UD    g1<1,1,0>UD     { align1 1H I@1 compacted };
mov(8)          g40<2>UD        g10<4,4,1>UD                    { align1 1Q $0.src };
mov(8)          g42<2>UD        g11<4,4,1>UD                    { align1 2Q $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g14<1>D         -g12<1,1,0>D    g6<1,1,0>D      { align1 1H I@3 compacted };
mov(8)          g40.1<2>UD      g14<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g42.1<2>UD      g15<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g40UD           g83UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };
add3(16)        g24<1>D         0x0004UW        g8<8,8,1>D      g1<1,1,1>D { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
cmp.l.f0.0(16)  g26<1>UD        g24<1,1,0>UD    g1<1,1,0>UD     { align1 1H I@1 compacted };
mov(8)          g41<2>UD        g24<4,4,1>UD                    { align1 1Q $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $2.src };
mov(8)          g43<2>UD        g25<4,4,1>UD                    { align1 2Q $0.src };
add(16)         g28<1>D         -g26<1,1,0>D    g6<1,1,0>D      { align1 1H I@3 compacted };
mov(8)          g41.1<2>UD      g28<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g43.1<2>UD      g29<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g41UD           g83UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };
add3(16)        g29<1>D         0x0008UW        g8<8,8,1>D      g1<1,1,1>D { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
cmp.l.f0.0(16)  g31<1>UD        g29<1,1,0>UD    g1<1,1,0>UD     { align1 1H I@1 compacted };
mov(8)          g42<2>UD        g29<4,4,1>UD                    { align1 1Q $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $2.src };
mov(8)          g44<2>UD        g30<4,4,1>UD                    { align1 2Q $0.src };
add(16)         g33<1>D         -g31<1,1,0>D    g6<1,1,0>D      { align1 1H I@3 compacted };
mov(8)          g42.1<2>UD      g33<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g44.1<2>UD      g34<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g42UD           g83UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $2 };
add3(16)        g34<1>D         0x000cUW        g8<8,8,1>D      g1<1,1,1>D { align1 1H };
cmp.l.f0.0(16)  g36<1>UD        g34<1,1,0>UD    g1<1,1,0>UD     { align1 1H I@1 compacted };
mov(8)          g43<2>UD        g34<4,4,1>UD                    { align1 1Q $2.src };
mov(8)          g45<2>UD        g35<4,4,1>UD                    { align1 2Q $2.src };
add(16)         g38<1>D         -g36<1,1,0>D    g6<1,1,0>D      { align1 1H I@3 compacted };
mov(8)          g43.1<2>UD      g38<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g45.1<2>UD      g39<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g43UD           g83UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $2 };
add(16)         g56<1>D         g56<1,1,0>D     64D             { align1 1H compacted };

LABEL5:
while(16)       JIP:  LABEL6                                    { align1 1H };
mov(16)         g68<1>UD        g58<8,8,1>UD                    { align1 1H };

LABEL8:
cmp.ge.f0.0(16) null<1>UD       g68<8,8,1>UD    0x00000120UD    { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL7        UIP:  LABEL7              { align1 1H };
shl(16)         g39<1>D         g68<8,8,1>D     0x00000002UD    { align1 1H $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g41<1>D         g122<1,1,0>D    g39<1,1,0>D     { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
cmp.l.f0.0(16)  g43<1>UD        g41<1,1,0>UD    g122<1,1,0>UD   { align1 1H I@1 compacted };
mov(8)          g9<2>UD         g41<4,4,1>UD                    { align1 1Q $2.src };
mov(8)          g11<2>UD        g42<4,4,1>UD                    { align1 2Q $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g45<1>D         -g43<1,1,0>D    g126<1,1,0>D    { align1 1H I@3 compacted };
mov(8)          g9.1<2>UD       g45<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g11.1<2>UD      g46<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g9UD            g105UD          0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $2 };
add3(16)        g46<1>D         0x0004UW        g39<8,8,1>D     g122<1,1,1>D { align1 1H };
cmp.l.f0.0(16)  g48<1>UD        g46<1,1,0>UD    g122<1,1,0>UD   { align1 1H I@1 compacted };
mov(8)          g10<2>UD        g46<4,4,1>UD                    { align1 1Q $2.src };
mov(8)          g12<2>UD        g47<4,4,1>UD                    { align1 2Q $2.src };
add(16)         g50<1>D         -g48<1,1,0>D    g126<1,1,0>D    { align1 1H I@3 compacted };
mov(8)          g10.1<2>UD      g50<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g12.1<2>UD      g51<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g10UD           g105UD          0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $2 };
add3(16)        g51<1>D         0x0008UW        g39<8,8,1>D     g122<1,1,1>D { align1 1H };
cmp.l.f0.0(16)  g53<1>UD        g51<1,1,0>UD    g122<1,1,0>UD   { align1 1H I@1 compacted };
mov(8)          g11<2>UD        g51<4,4,1>UD                    { align1 1Q $2.src };
mov(8)          g13<2>UD        g52<4,4,1>UD                    { align1 2Q $2.src };
add(16)         g55<1>D         -g53<1,1,0>D    g126<1,1,0>D    { align1 1H I@3 compacted };
mov(8)          g11.1<2>UD      g55<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g13.1<2>UD      g56<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g11UD           g105UD          0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $2 };
add3(16)        g56<1>D         0x000cUW        g39<8,8,1>D     g122<1,1,1>D { align1 1H };
cmp.l.f0.0(16)  g76<1>UD        g56<1,1,0>UD    g122<1,1,0>UD   { align1 1H I@1 compacted };
mov(8)          g12<2>UD        g56<4,4,1>UD                    { align1 1Q $2.src };
mov(8)          g14<2>UD        g57<4,4,1>UD                    { align1 2Q $2.src };
add(16)         g78<1>D         -g76<1,1,0>D    g126<1,1,0>D    { align1 1H I@3 compacted };
mov(8)          g12.1<2>UD      g78<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g14.1<2>UD      g79<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g12UD           g105UD          0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $2 };
add(16)         g68<1>D         g68<1,1,0>D     64D             { align1 1H compacted };

LABEL7:
while(16)       JIP:  LABEL8                                    { align1 1H };
add(16)         g70<1>D         g70<1,1,0>D     32D             { align1 1H compacted };

LABEL4:
while(16)       JIP:  LABEL9                                    { align1 1H };
cmp.z.f0.0(16)  null<1>D        g74<8,8,1>D     0D              { align1 1H };
(+f0.0) if(16)  JIP:  LABEL10         UIP:  LABEL10             { align1 1H };
add(8)          g55<1>D         g60<8,4,2>D     391212D         { align1 1Q };
add(8)          g79<1>D         g98<8,4,2>D     391212D         { align1 2Q };
cmp.l.f0.0(8)   g56<1>UD        g55<8,8,1>UD    g60<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g80<1>UD        g79<8,8,1>UD    g98<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g24<2>UD        g55<4,4,1>UD                    { align1 1Q };
mov(8)          g26<2>UD        g79<4,4,1>UD                    { align1 2Q };
add(8)          g57<1>D         -g56<8,8,1>D    g60.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g81<1>D         -g80<8,8,1>D    g98.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g24.1<2>UD      g57<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g26.1<2>UD      g81<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g24UD           g83UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $2 };

LABEL10:
endif(16)       JIP:  LABEL11                                   { align1 1H };

LABEL11:
else(16)        JIP:  LABEL0          UIP:  LABEL0              { align1 1H };

LABEL1:
mov(16)         g76<1>D         1D                              { align1 1H };
cmp.z.f0.0(16)  null<1>D        g74<8,8,1>D     0D              { align1 1H I@3 };
(+f0.0) if(16)  JIP:  LABEL12         UIP:  LABEL12             { align1 1H };
mov(16)         g25<1>D         0D                              { align1 1H $2.src };
mov(16)         g27<1>UD        0x00000000UD                    { align1 1H $2.src };
mov(16)         g29<1>UD        0x00000000UD                    { align1 1H };
mov(16)         g31<1>UD        0x00000000UD                    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g25UD           g27UD           0x04007506                0x00000180
                            slm MsgDesc: ( store_cmask, a32, d32, xyz, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 6 flat )  base_offset 0  { align1 1H $2 };

LABEL12:
endif(16)       JIP:  LABEL0                                    { align1 1H };
sync allwr(1)                   null<0,1,0>UB                   { align1 WE_all 1N };
sync allwr(1)                   null<0,1,0>UB                   { align1 WE_all 1N };
send(1)         g82UD           g0UD            nullUD          0x0210011f                0x00000000
                            slm MsgDesc: ( fence, a32, threadgroup, none, normal_routing dst_len = 1, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.dst };
mov(8)          g83<1>UD        0x00000000UD                    { align1 WE_all 1Q $2.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(2)          g83.10<1>UB     g0.11<0,1,0>UB                  { align1 WE_all 1N I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(16) nullUD  g83UD           nullUD          0x02000004                0x00000000
                            gateway MsgDesc: (barrier msg)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H $2 };
(+f0.0.any16h) sync bar(16)     null<0,1,0>UB                   { align1 WE_all 1H };
mov(8)          g78<1>UD        g62.1<8,4,2>UD                  { align1 1Q F@6 };
mov(8)          g79<1>UD        g18.1<8,4,2>UD                  { align1 2Q F@5 };
mov(16)         g30<1>UD        0x00000000UD                    { align1 1H $2.src };
mov(16)         g33<1>UD        0x00000000UD                    { align1 1H };
mov(16)         g36<1>UD        0x00000000UD                    { align1 1H };
mov(16)         g80<1>UD        g72<8,8,1>UD                    { align1 1H };

LABEL14:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.dst };
cmp.ge.f0.0(16) null<1>UD       g80<8,8,1>UD    g3<0,1,0>UD     { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL13       UIP:  LABEL13             { align1 1H };
shl(16)         g84<1>D         g80<8,8,1>D     0x00000003UD    { align1 1H $2.src };
shr(16)         g86<1>UD        g80<1,1,0>UD    0x0000001dUD    { align1 1H compacted };
add(16)         g103<1>D        g36<1,1,0>D     1D              { align1 1H I@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g105<1>D        g33<1,1,0>D     1D              { align1 1H I@7 compacted };
add(16)         g111<1>D        g30<1,1,0>D     1D              { align1 1H I@7 compacted };
add(16)         g80<1>D         g80<1,1,0>D     512D            { align1 1H compacted };
add(8)          g58<1>D         g62<8,4,2>D     g84<1,1,0>D     { align1 1Q I@6 compacted };
add(8)          g88<1>D         g18<8,4,2>D     g85<1,1,0>D     { align1 2Q I@7 compacted };
cmp.l.f0.0(8)   g89<1>UD        g58<8,8,1>UD    g62<8,4,2>UD    { align1 1Q I@2 };
mov(8)          g44<2>UD        g58<4,4,1>UD                    { align1 1Q $2.src };
cmp.l.f0.0(8)   g90<1>UD        g88<8,8,1>UD    g18<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g46<2>UD        g88<4,4,1>UD                    { align1 2Q $2.src };
add3(16)        g91<1>D         g78<8,8,1>D     g86<8,8,1>D     -g89<1,1,1>D { align1 1H I@2 };
mov(8)          g44.1<2>UD      g91<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g46.1<2>UD      g92<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g92UD           g44UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
add(16)         g96<1>D         g92<1,1,0>D     36D             { align1 1H $2.dst compacted };
cmp.l.f0.0(16)  g16<1>UD        g96<1,1,0>UD    g92<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g48<2>UD        g96<4,4,1>UD                    { align1 1Q $2.src };
mov(8)          g50<2>UD        g97<4,4,1>UD                    { align1 2Q $2.src };
add(16)         g100<1>D        -g16<1,1,0>D    g94<1,1,0>D     { align1 1H @3 $2.dst compacted };
mov(8)          g48.1<2>UD      g100<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g50.1<2>UD      g101<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g101UD          g48UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
cmp.le.f0.0(16) g107<1>UD       g101<8,8,1>UD   0x00000100UD    { align1 1H $2.dst };
(+f0.0) sel(16) g109<1>UD       g36<1,1,0>UD    g103<1,1,0>UD   { align1 1H compacted };
cmp.g.f0.0(16)  g113<1>UD       g101<1,1,0>UD   0x00000006UD    { align1 1H compacted };
(+f0.0) sel(16) g36<1>UD        g109<1,1,0>UD   g36<1,1,0>UD    { align1 1H I@2 compacted };
and.nz.f0.0(16) null<1>UD       g113<8,8,1>UD   g107<8,8,1>UD   { align1 1H I@2 };
(+f0.0) sel(16) g33<1>UD        g105<1,1,0>UD   g33<1,1,0>UD    { align1 1H compacted };
cmp.nz.f0.0(16) null<1>D        g113<8,8,1>D    0D              { align1 1H };
(+f0.0) sel(16) g30<1>UD        g30<1,1,0>UD    g111<1,1,0>UD   { align1 1H compacted };

LABEL13:
while(16)       JIP:  LABEL14                                   { align1 1H };
mov(16)         g28<1>UD        0x00000000UD                    { align1 1H $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        g1UD            g28UD           g30UD           0x0424050c                0x00000080
                            slm MsgDesc: ( atomic_add, a32, d32, V1, L1UC_L3WB dst_len = 2, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(16)         g31<1>UD        0x00000004UD                    { align1 1H $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g68UD           g31UD           g33UD           0x0424050c                0x00000080
                            slm MsgDesc: ( atomic_add, a32, d32, V1, L1UC_L3WB dst_len = 2, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };
mov(16)         g34<1>UD        0x00000008UD                    { align1 1H $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g70UD           g34UD           g36UD           0x0424050c                0x00000080
                            slm MsgDesc: ( atomic_add, a32, d32, V1, L1UC_L3WB dst_len = 2, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };
sync allwr(1)                   null<0,1,0>UB                   { align1 WE_all 1N };
sync allwr(1)                   null<0,1,0>UB                   { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
send(1)         g114UD          g0UD            nullUD          0x0210011f                0x00000000
                            slm MsgDesc: ( fence, a32, threadgroup, none, normal_routing dst_len = 1, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.dst };
mov(8)          g115<1>UD       0x00000000UD                    { align1 WE_all 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(2)          g115.10<1>UB    g0.11<0,1,0>UB                  { align1 WE_all 1N I@1 };
mov(1)          g53<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(16) nullUD  g115UD          nullUD          0x02000004                0x00000000
                            gateway MsgDesc: (barrier msg)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H $5 };
(+f0.0.any16h) sync bar(16)     null<0,1,0>UB                   { align1 WE_all 1H };
mov(1)          f0<1>UD         g53<0,1,0>UD                    { align1 WE_all 1N I@2 };
mov(8)          g116<1>UD       0x00000000UD                    { align1 WE_all 1Q $0.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
(+f0.0.any16h) send(1) g86UD    g116UD          nullUD          0x0210c500                0x00000000
                            slm MsgDesc: ( load, a32, d32, V8, transpose, L1STATE_L3MOCS dst_len = 1, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
add(16)         g14<1>D         g86<0,1,0>D     g86.1<0,1,0>D   { align1 1H $2.src compacted };
cmp.l.f0.0(16)  g80<1>UD        g14<1,1,0>UD    g86<0,1,0>UD    { align1 1H I@1 compacted };

LABEL19:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.dst };
cmp.ge.f0.0(16) null<1>UD       g72<8,8,1>UD    g3<0,1,0>UD     { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL15       UIP:  LABEL15             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
shl(16)         g117<1>D        g72<8,8,1>D     0x00000003UD    { align1 1H };
shr(16)         g119<1>UD       g72<1,1,0>UD    0x0000001dUD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(8)          g34<1>UD        g66.1<8,4,2>UD                  { align1 1Q F@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
mov(8)          g35<1>UD        g22.1<8,4,2>UD                  { align1 2Q F@1 };
add(8)          g82<1>D         g62<8,4,2>D     g117<1,1,0>D    { align1 1Q I@4 compacted };
add(8)          g121<1>D        g18<8,4,2>D     g118<1,1,0>D    { align1 2Q I@5 compacted };
cmp.l.f0.0(8)   g122<1>UD       g82<8,8,1>UD    g62<8,4,2>UD    { align1 1Q I@2 };
mov(8)          g49<2>UD        g82<4,4,1>UD                    { align1 1Q $2.src };
cmp.l.f0.0(8)   g123<1>UD       g121<8,8,1>UD   g18<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g51<2>UD        g121<4,4,1>UD                   { align1 2Q $2.src };
add3(16)        g124<1>D        g78<8,8,1>D     g119<8,8,1>D    -g122<1,1,1>D { align1 1H I@2 };
mov(8)          g49.1<2>UD      g124<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g51.1<2>UD      g125<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g4UD            g49UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g56<2>UD        g4<4,4,1>UD                     { align1 1Q $6.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
mov(8)          g58<2>UD        g5<4,4,1>UD                     { align1 2Q $6.dst };
add(16)         g125<1>D        g4<1,1,0>D      36D             { align1 1H compacted };
mov(8)          g56.1<2>UD      g6<4,4,1>UD                     { align1 1Q @3 $6.dst };
mov(8)          g58.1<2>UD      g7<4,4,1>UD                     { align1 2Q @3 $6.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
cmp.l.f0.0(16)  g24<1>UD        g125<1,1,0>UD   g4<1,1,0>UD     { align1 1H I@3 compacted };
mov(8)          g39<2>UD        g125<4,4,1>UD                   { align1 1Q $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $2.src };
mov(8)          g41<2>UD        g126<4,4,1>UD                   { align1 2Q $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g26<1>D         -g24<1,1,0>D    g6<1,1,0>D      { align1 1H I@3 compacted };
mov(8)          g39.1<2>UD      g26<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g41.1<2>UD      g27<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g24UD           g39UD           nullUD          0x08607582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyz, L1STATE_L3MOCS dst_len = 6, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $7 };
cmp.g.f0.0(16)  null<1>UD       g24<8,8,1>UD    0x00000006UD    { align1 1H $7.dst };
(+f0.0) if(16)  JIP:  LABEL17         UIP:  LABEL16             { align1 1H };
add(16)         g36<1>D         g70<1,1,0>D     1D              { align1 1H $0.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
add(16)         g38<1>D         g14<1,1,0>D     g70<1,1,0>D     { align1 1H $0.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g44<1>D         g68<1,1,0>D     1D              { align1 1H $0.dst compacted };
add(16)         g46<1>D         g86<0,1,0>D     g68<1,1,0>D     { align1 1H $2.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
cmp.l.f0.0(16)  g40<1>UD        g38<1,1,0>UD    g14<1,1,0>UD    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
cmp.l.f0.0(16)  g48<1>UD        g46<1,1,0>UD    g86<0,1,0>UD    { align1 1H I@2 compacted };
cmp.le.f0.0(16) g50<1>UD        g24<8,8,1>UD    0x00000100UD    { align1 1H $6.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g42<1>D         -g40<1,1,0>D    -g80<1,1,0>D    { align1 1H I@3 compacted };
(+f0.0) sel(16) g54<1>UD        g46<1,1,0>UD    g38<1,1,0>UD    { align1 1H $2.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
(+f0.0) sel(16) g52<1>D         -g48<1,1,0>D    g42<1,1,0>D     { align1 1H $6.src compacted };
mul(8)          acc0<1>UD       g54<8,8,1>UD    0x0058UW        { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mul(16)         g84<1>D         g54<1,1,0>D     88W             { align1 1H I@3 compacted };
mul(16)         g87<1>D         g52<1,1,0>D     88W             { align1 1H I@3 compacted };
mach(8)         g82<1>UD        g54<1,1,0>UD    0x00000058UD    { align1 1Q compacted AccWrEnable };
add(8)          g89<1>D         g66<8,4,2>D     g84<1,1,0>D     { align1 1Q I@3 compacted };
add(8)          g90<1>D         g22<8,4,2>D     g85<1,1,0>D     { align1 2Q I@4 compacted };
mul(8)          acc0<1>UD       g55<8,8,1>UD    0x0058UW        { align1 2Q };
cmp.l.f0.0(8)   g91<1>UD        g89<8,8,1>UD    g66<8,4,2>UD    { align1 1Q I@3 };
mov(8)          g30<2>UD        g89<4,4,1>UD                    { align1 1Q $0.dst };
cmp.l.f0.0(8)   g92<1>UD        g90<8,8,1>UD    g22<8,4,2>UD    { align1 2Q I@4 };
mov(8)          g32<2>UD        g90<4,4,1>UD                    { align1 2Q $0.src };
cmp.nz.f0.0(16) null<1>D        g50<8,8,1>D     0D              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $2.src };
mach(8)         g83<1>UD        g55<8,8,1>UD    0x00000058UD    { align1 2Q AccWrEnable };
(+f0.0) sel(16) g70<1>UD        g70<1,1,0>UD    g36<1,1,0>UD    { align1 1H compacted };
(+f0.0) sel(16) g68<1>UD        g44<1,1,0>UD    g68<1,1,0>UD    { align1 1H compacted };
add(16)         g89<1>D         g82<1,1,0>D     g87<1,1,0>D     { align1 1H I@3 compacted };
add3(16)        g93<1>D         g34<8,8,1>D     g89<8,8,1>D     -g91<1,1,1>D { align1 1H I@1 };
mov(8)          g30.1<2>UD      g93<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g32.1<2>UD      g94<4,4,1>UD                    { align1 2Q I@2 };
else(16)        JIP:  LABEL16         UIP:  LABEL16             { align1 1H };

LABEL17:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g82<1>D         g1<1,1,0>D      1D              { align1 1H $0.dst compacted };
mul(8)          acc0<1>UD       g1<8,8,1>UD     0x0058UW        { align1 1Q };
mul(16)         g96<1>D         g1<1,1,0>D      88W             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mach(8)         g94<1>UD        g1<1,1,0>UD     0x00000058UD    { align1 1Q I@5 compacted AccWrEnable };
add(8)          g90<1>D         g66<8,4,2>D     g96<1,1,0>D     { align1 1Q I@2 compacted };
add(8)          g97<1>D         g22<8,4,2>D     g97<1,1,0>D     { align1 2Q I@3 compacted };
mul(8)          acc0<1>UD       g2<8,8,1>UD     0x0058UW        { align1 2Q };
cmp.l.f0.0(8)   g16<1>UD        g90<8,8,1>UD    g66<8,4,2>UD    { align1 1Q I@3 };
mov(8)          g30<2>UD        g90<4,4,1>UD                    { align1 1Q $0.dst };
cmp.l.f0.0(8)   g17<1>UD        g97<8,8,1>UD    g22<8,4,2>UD    { align1 2Q I@4 };
mov(8)          g32<2>UD        g97<4,4,1>UD                    { align1 2Q $0.src };
mach(8)         g95<1>UD        g2<8,8,1>UD     0x00000058UD    { align1 2Q AccWrEnable };
mov(16)         g1<1>UD         g82<8,8,1>UD                    { align1 1H };
add3(16)        g100<1>D        g34<8,8,1>D     g94<8,8,1>D     -g16<1,1,1>D { align1 1H I@2 };
mov(8)          g30.1<2>UD      g100<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g32.1<2>UD      g101<4,4,1>UD                   { align1 2Q I@2 };

LABEL16:
endif(16)       JIP:  LABEL15                                   { align1 1H };
shl(16)         g101<1>D        g72<8,8,1>D     0x00000006UD    { align1 1H };
shr(16)         g103<1>UD       g72<1,1,0>UD    0x0000001aUD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g105<1>UD       g64.1<8,4,2>UD                  { align1 1Q F@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $2.src };
mov(8)          g106<1>UD       g20.1<8,4,2>UD                  { align1 2Q F@3 };
add(8)          g82<1>D         g64<8,4,2>D     g101<1,1,0>D    { align1 1Q I@4 compacted };
add(8)          g83<1>D         g20<8,4,2>D     g102<1,1,0>D    { align1 2Q I@5 compacted };
cmp.l.f0.0(8)   g107<1>UD       g82<8,8,1>UD    g64<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g108<1>UD       g83<8,8,1>UD    g20<8,4,2>UD    { align1 2Q I@2 };
add(16)         g109<1>D        g82<1,1,0>D     8D              { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add3(16)        g84<1>D         g105<8,8,1>D    g103<8,8,1>D    -g107<1,1,1>D { align1 1H I@2 };
cmp.l.f0.0(16)  g111<1>UD       g109<1,1,0>UD   g82<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g34<2>UD        g109<4,4,1>UD                   { align1 1Q };
mov(8)          g36<2>UD        g110<4,4,1>UD                   { align1 2Q $0.src };
add(16)         g113<1>D        -g111<1,1,0>D   g84<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g34.1<2>UD      g113<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g36.1<2>UD      g114<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g38UD           g34UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $8 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.dst };
send(16)        nullUD          g30UD           g38UD           0x08003586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 4 flat )  base_offset 0  { align1 1H $9 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
add(16)         g114<1>D        g82<1,1,0>D     16D             { align1 1H compacted };
add(8)          g91<1>D         g30<8,4,2>D     8D              { align1 1Q $9.src compacted };
add(8)          g119<1>D        g32<8,4,2>D     8D              { align1 2Q $9.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
cmp.l.f0.0(16)  g116<1>UD       g114<1,1,0>UD   g82<1,1,0>UD    { align1 1H I@3 compacted };
mov(8)          g34<2>UD        g114<4,4,1>UD                   { align1 1Q $8.src };
mov(8)          g36<2>UD        g115<4,4,1>UD                   { align1 2Q $8.src };
cmp.l.f0.0(8)   g92<1>UD        g91<8,8,1>UD    g30<8,4,2>UD    { align1 1Q I@5 };
cmp.l.f0.0(8)   g120<1>UD       g119<8,8,1>UD   g32<8,4,2>UD    { align1 2Q I@5 };
mov(8)          g38<2>UD        g91<4,4,1>UD                    { align1 1Q $9.src };
mov(8)          g40<2>UD        g119<4,4,1>UD                   { align1 2Q $9.src };
add(16)         g118<1>D        -g116<1,1,0>D   g84<1,1,0>D     { align1 1H I@7 compacted };
add(8)          g93<1>D         -g92<8,8,1>D    g30.1<8,4,2>D   { align1 1Q I@5 };
add(8)          g121<1>D        -g120<8,8,1>D   g32.1<8,4,2>D   { align1 2Q I@5 };
mov(8)          g34.1<2>UD      g118<4,4,1>UD                   { align1 1Q I@3 };
mov(8)          g36.1<2>UD      g119<4,4,1>UD                   { align1 2Q I@4 };
mov(8)          g38.1<2>UD      g93<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g40.1<2>UD      g121<4,4,1>UD                   { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g42UD           g34UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $10 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g38UD           g42UD           0x08003586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 4 flat )  base_offset 0  { align1 1H $11 };
add(16)         g122<1>D        g82<1,1,0>D     24D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(8)          g94<1>D         g30<8,4,2>D     16D             { align1 1Q compacted };
add(8)          g127<1>D        g32<8,4,2>D     16D             { align1 2Q compacted };
cmp.l.f0.0(16)  g124<1>UD       g122<1,1,0>UD   g82<1,1,0>UD    { align1 1H I@3 compacted };
mov(8)          g34<2>UD        g122<4,4,1>UD                   { align1 1Q $10.src };
mov(8)          g36<2>UD        g123<4,4,1>UD                   { align1 2Q $10.src };
cmp.l.f0.0(8)   g95<1>UD        g94<8,8,1>UD    g30<8,4,2>UD    { align1 1Q I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $11.src };
cmp.l.f0.0(8)   g42<1>UD        g127<8,8,1>UD   g32<8,4,2>UD    { align1 2Q I@5 };
mov(8)          g38<2>UD        g94<4,4,1>UD                    { align1 1Q $11.src };
mov(8)          g40<2>UD        g127<4,4,1>UD                   { align1 2Q $11.src };
add(16)         g126<1>D        -g124<1,1,0>D   g84<1,1,0>D     { align1 1H I@7 compacted };
add(8)          g96<1>D         -g95<8,8,1>D    g30.1<8,4,2>D   { align1 1Q I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $11.src };
add(8)          g43<1>D         -g42<8,8,1>D    g32.1<8,4,2>D   { align1 2Q I@5 };
mov(8)          g34.1<2>UD      g126<4,4,1>UD                   { align1 1Q I@3 };
mov(8)          g36.1<2>UD      g127<4,4,1>UD                   { align1 2Q I@4 };
mov(8)          g38.1<2>UD      g96<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g40.1<2>UD      g43<4,4,1>UD                    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g42UD           g34UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $12 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g52<2>UD        g42<4,4,1>UD                    { align1 1Q $12.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $2.src };
mov(8)          g54<2>UD        g43<4,4,1>UD                    { align1 2Q $12.dst };
mov(8)          g52.1<2>UD      g44<4,4,1>UD                    { align1 1Q @2 $12.dst };
mov(8)          g54.1<2>UD      g45<4,4,1>UD                    { align1 2Q @2 $12.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@5 };
send(16)        nullUD          g38UD           g42UD           0x08003586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 4 flat )  base_offset 0  { align1 1H $2 };
add(16)         g46<1>D         g82<1,1,0>D     32D             { align1 1H $2.src compacted };
add(8)          g97<1>D         g30<8,4,2>D     24D             { align1 1Q compacted };
add(8)          g51<1>D         g32<8,4,2>D     24D             { align1 2Q $6.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
cmp.l.f0.0(16)  g48<1>UD        g46<1,1,0>UD    g82<1,1,0>UD    { align1 1H I@3 compacted };
mov(8)          g34<2>UD        g46<4,4,1>UD                    { align1 1Q $12.src };
mov(8)          g36<2>UD        g47<4,4,1>UD                    { align1 2Q $12.src };
cmp.l.f0.0(8)   g16<1>UD        g97<8,8,1>UD    g30<8,4,2>UD    { align1 1Q I@5 };
cmp.l.f0.0(8)   g87<1>UD        g51<8,8,1>UD    g32<8,4,2>UD    { align1 2Q I@5 };
mov(8)          g38<2>UD        g97<4,4,1>UD                    { align1 1Q $2.src };
mov(8)          g40<2>UD        g51<4,4,1>UD                    { align1 2Q $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
add(16)         g50<1>D         -g48<1,1,0>D    g84<1,1,0>D     { align1 1H I@7 compacted };
add(8)          g17<1>D         -g16<8,8,1>D    g30.1<8,4,2>D   { align1 1Q I@5 };
add(8)          g88<1>D         -g87<8,8,1>D    g32.1<8,4,2>D   { align1 2Q I@5 };
mov(8)          g34.1<2>UD      g50<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g36.1<2>UD      g51<4,4,1>UD                    { align1 2Q I@4 };
mov(8)          g38.1<2>UD      g17<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g40.1<2>UD      g88<4,4,1>UD                    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g8UD            g34UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $13 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g38UD           g4UD            0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $2 };
add(8)          g100<1>D        g30<8,4,2>D     52D             { align1 1Q compacted };
add(8)          g89<1>D         g32<8,4,2>D     52D             { align1 2Q compacted };
mov(16)         g46<1>D         g24<8,8,1>D                     { align1 1H };
mov(16)         g48<1>D         g28<8,8,1>D                     { align1 1H $7.dst };
mov(16)         g50<1>D         g26<8,8,1>D                     { align1 1H $7.dst };
cmp.l.f0.0(8)   g101<1>UD       g100<8,8,1>UD   g30<8,4,2>UD    { align1 1Q I@5 };
cmp.l.f0.0(8)   g90<1>UD        g89<8,8,1>UD    g32<8,4,2>UD    { align1 2Q I@5 };
mov(8)          g34<2>UD        g100<4,4,1>UD                   { align1 1Q $13.src };
mov(8)          g36<2>UD        g89<4,4,1>UD                    { align1 2Q $13.src };
add(8)          g102<1>D        -g101<8,8,1>D   g30.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g91<1>D         -g90<8,8,1>D    g32.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g34.1<2>UD      g102<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g36.1<2>UD      g91<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g34UD           g46UD           0x08007586                0x00000180
                            ugm MsgDesc: ( store_cmask, a64, d32, xyz, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 6 flat )  base_offset 0  { align1 1H $2 };
add(16)         g92<1>D         g82<1,1,0>D     48D             { align1 1H compacted };
add(8)          g103<1>D        g30<8,4,2>D     48D             { align1 1Q compacted };
add(8)          g97<1>D         g32<8,4,2>D     48D             { align1 2Q compacted };
cmp.l.f0.0(16)  g94<1>UD        g92<1,1,0>UD    g82<1,1,0>UD    { align1 1H I@3 compacted };
mov(8)          g34<2>UD        g92<4,4,1>UD                    { align1 1Q $2.src };
mov(8)          g36<2>UD        g93<4,4,1>UD                    { align1 2Q $2.src };
cmp.l.f0.0(8)   g104<1>UD       g103<8,8,1>UD   g30<8,4,2>UD    { align1 1Q I@5 };
cmp.l.f0.0(8)   g16<1>UD        g97<8,8,1>UD    g32<8,4,2>UD    { align1 2Q I@5 };
mov(8)          g38<2>UD        g103<4,4,1>UD                   { align1 1Q $2.src };
mov(8)          g40<2>UD        g97<4,4,1>UD                    { align1 2Q $2.src };
add(16)         g96<1>D         -g94<1,1,0>D    g84<1,1,0>D     { align1 1H I@7 compacted };
add(8)          g105<1>D        -g104<8,8,1>D   g30.1<8,4,2>D   { align1 1Q I@5 };
add(8)          g17<1>D         -g16<8,8,1>D    g32.1<8,4,2>D   { align1 2Q I@5 };
mov(8)          g34.1<2>UD      g96<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g36.1<2>UD      g97<4,4,1>UD                    { align1 2Q I@4 };
mov(8)          g38.1<2>UD      g105<4,4,1>UD                   { align1 1Q I@4 };
mov(8)          g40.1<2>UD      g17<4,4,1>UD                    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g46UD           g34UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $14 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g38UD           g46UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $2 };
add(16)         g100<1>D        g82<1,1,0>D     40D             { align1 1H compacted };
add(8)          g106<1>D        g30<8,4,2>D     40D             { align1 1Q compacted };
add(8)          g105<1>D        g32<8,4,2>D     40D             { align1 2Q compacted };
cmp.l.f0.0(16)  g102<1>UD       g100<1,1,0>UD   g82<1,1,0>UD    { align1 1H I@3 compacted };
mov(8)          g34<2>UD        g100<4,4,1>UD                   { align1 1Q $14.src };
mov(8)          g36<2>UD        g101<4,4,1>UD                   { align1 2Q $14.src };
cmp.l.f0.0(8)   g107<1>UD       g106<8,8,1>UD   g30<8,4,2>UD    { align1 1Q I@5 };
mov(8)          g38<2>UD        g106<4,4,1>UD                   { align1 1Q $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $2.src };
mov(8)          g40<2>UD        g105<4,4,1>UD                   { align1 2Q I@6 };
cmp.l.f0.0(8)   g106<1>UD       g105<8,8,1>UD   g32<8,4,2>UD    { align1 2Q };
add(16)         g104<1>D        -g102<1,1,0>D   g84<1,1,0>D     { align1 1H I@7 compacted };
add(8)          g108<1>D        -g107<8,8,1>D   g30.1<8,4,2>D   { align1 1Q I@5 };
add(8)          g107<1>D        -g106<8,8,1>D   g32.1<8,4,2>D   { align1 2Q I@3 };
mov(8)          g34.1<2>UD      g104<4,4,1>UD                   { align1 1Q I@3 };
mov(8)          g36.1<2>UD      g105<4,4,1>UD                   { align1 2Q I@4 };
mov(8)          g38.1<2>UD      g108<4,4,1>UD                   { align1 1Q I@4 };
mov(8)          g40.1<2>UD      g107<4,4,1>UD                   { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g30UD           g34UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g38UD           g30UD           0x08003586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 4 flat )  base_offset 0  { align1 1H $0 };
cmp.g.f0.0(16)  null<1>UD       g24<8,8,1>UD    0x00000100UD    { align1 1H };
(+f0.0) if(16)  JIP:  LABEL18         UIP:  LABEL18             { align1 1H };
send(16)        nullUD          g52UD           g76UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $15 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g24UD           g56UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $0 };
add(16)         g108<1>D        g4<1,1,0>D      16D             { align1 1H $2.src compacted };
add(16)         g113<1>D        g42<1,1,0>D     32D             { align1 1H $2.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
cmp.l.f0.0(16)  g110<1>UD       g108<1,1,0>UD   g4<1,1,0>UD     { align1 1H I@2 compacted };
mov(8)          g46<2>UD        g108<4,4,1>UD                   { align1 1Q $2.src };
mov(8)          g48<2>UD        g109<4,4,1>UD                   { align1 2Q $2.src };
cmp.l.f0.0(16)  g115<1>UD       g113<1,1,0>UD   g42<1,1,0>UD    { align1 1H I@4 compacted };
mov(8)          g50<2>UD        g113<4,4,1>UD                   { align1 1Q $2.src };
mov(8)          g52<2>UD        g114<4,4,1>UD                   { align1 2Q $15.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g112<1>D        -g110<1,1,0>D   g6<1,1,0>D      { align1 1H I@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g117<1>D        -g115<1,1,0>D   g44<1,1,0>D     { align1 1H I@4 compacted };
mov(8)          g46.1<2>UD      g112<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g48.1<2>UD      g113<4,4,1>UD                   { align1 2Q I@3 };
mov(8)          g50.1<2>UD      g117<4,4,1>UD                   { align1 1Q I@3 };
mov(8)          g52.1<2>UD      g118<4,4,1>UD                   { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g4UD            g46UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
mov(16)         g32<1>D         g24<8,8,1>D                     { align1 1H $0.dst };
mov(16)         g34<1>D         g26<8,8,1>D                     { align1 1H $0.dst };
mov(16)         g36<1>D         g28<8,8,1>D                     { align1 1H $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(16)         g38<1>D         g4<8,8,1>D                      { align1 1H $2.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g50UD           g32UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $0 };
add(16)         g118<1>D        g42<1,1,0>D     48D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
mov(16)         g54<1>D         g6<8,8,1>D                      { align1 1H $2.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(16)         g56<1>D         g8<8,8,1>D                      { align1 1H $2.dst };
cmp.l.f0.0(16)  g120<1>UD       g118<1,1,0>UD   0x00000030UD    { align1 1H I@3 compacted };
mov(8)          g50<2>UD        g118<4,4,1>UD                   { align1 1Q $0.src };
mov(8)          g52<2>UD        g119<4,4,1>UD                   { align1 2Q $0.src };
add(16)         g122<1>D        -g120<1,1,0>D   g44<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g50.1<2>UD      g122<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g52.1<2>UD      g123<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g50UD           g54UD           0x08003586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 4 flat )  base_offset 0  { align1 1H $2 };

LABEL18:
endif(16)       JIP:  LABEL15                                   { align1 1H };
add(16)         g72<1>D         g72<1,1,0>D     512D            { align1 1H compacted };

LABEL15:
while(16)       JIP:  LABEL19                                   { align1 1H };
cmp.z.f0.0(16)  null<1>D        g74<8,8,1>D     0D              { align1 1H };
(+f0.0) if(16)  JIP:  LABEL20         UIP:  LABEL20             { align1 1H };
add(8)          g109<1>D        g60<8,4,2>D     16D             { align1 1Q F@7 compacted };
add(8)          g123<1>D        g98<8,4,2>D     16D             { align1 2Q F@7 compacted };
mov(16)         g51<1>D         g86<0,1,0>D                     { align1 1H $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
mov(16)         g53<1>D         g86.1<0,1,0>D                   { align1 1H };
cmp.l.f0.0(8)   g110<1>UD       g109<8,8,1>UD   g60<8,4,2>UD    { align1 1Q I@4 };
cmp.l.f0.0(8)   g124<1>UD       g123<8,8,1>UD   g98<8,4,2>UD    { align1 2Q I@4 };
mov(8)          g47<2>UD        g109<4,4,1>UD                   { align1 1Q $2.src };
mov(8)          g49<2>UD        g123<4,4,1>UD                   { align1 2Q $2.src };
add(8)          g111<1>D        -g110<8,8,1>D   g60.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g125<1>D        -g124<8,8,1>D   g98.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g47.1<2>UD      g111<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g49.1<2>UD      g125<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g47UD           g51UD           0x08003586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 4 flat )  base_offset 0  { align1 1H $2 };
mov(8)          g127<1>UD       0x00000008UD                    { align1 WE_all 1Q };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
add(8)          g112<1>D        g60<8,4,2>D     32D             { align1 1Q compacted };
add(8)          g1<1>D          g98<8,4,2>D     32D             { align1 2Q $0.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
(+f0.0.any16h) send(1) g126UD   g127UD          nullUD          0x0210c500                0x00000000
                            slm MsgDesc: ( load, a32, d32, V8, transpose, L1STATE_L3MOCS dst_len = 1, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g52<2>UD        g112<4,4,1>UD                   { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $2.src };
mov(8)          g54<2>UD        g1<4,4,1>UD                     { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(16)         g12<1>D         g126<0,1,0>D                    { align1 1H $2.src };
cmp.l.f0.0(8)   g113<1>UD       g112<8,8,1>UD   g60<8,4,2>UD    { align1 1Q };
cmp.l.f0.0(8)   g2<1>UD         g1<8,8,1>UD     g98<8,4,2>UD    { align1 2Q $0.dst };
add(8)          g114<1>D        -g113<8,8,1>D   g60.1<8,4,2>D   { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $1.dst };
add(8)          g3<1>D          -g2<8,8,1>D     g98.1<8,4,2>D   { align1 2Q I@2 };
mov(8)          g52.1<2>UD      g114<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g54.1<2>UD      g3<4,4,1>UD                     { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g52UD           g12UD           0x08003586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 4 flat )  base_offset 0  { align1 1H $2 };

LABEL20:
endif(16)       JIP:  LABEL0                                    { align1 1H };

LABEL0:
endif(16)       JIP:  LABEL21                                   { align1 1H };

LABEL21:
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q I@7 };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_build_BFS_categorize_builds_and_init_scheduler_code[] = {
    0x80000065, 0x73058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x53054660, 0x00000000, 0x00000000,
    0x00040061, 0x00010220, 0x20000024, 0x00000000,
    0xe0460065, 0x0ff10043, 0xe2741c40, 0x00017303,
    0xe2750040, 0x04017303, 0x80030061, 0x37054410,
    0x00000000, 0x76543210, 0x00041c69, 0x55058660,
    0x02464605, 0x00000004, 0x80001c01, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa00740c, 0x00340000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x80004131, 0x030c0000,
    0xfa00750c, 0x00300000, 0x64371a40, 0x00803795,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00040061, 0x57050160, 0x00463705, 0x00000000,
    0xa0591940, 0x55005702, 0xe0481965, 0x1ff05903,
    0x00041961, 0x4a050220, 0x00464805, 0x00000000,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x213c0061, 0x001102cc, 0x2a620061, 0x001102cc,
    0x00030061, 0x3e260aa0, 0x00000264, 0x00000000,
    0x00130061, 0x12260aa0, 0x00000264, 0x00000000,
    0x00030061, 0x40260aa0, 0x000002a4, 0x00000000,
    0x00130061, 0x14260aa0, 0x000002a4, 0x00000000,
    0x00030061, 0x42260aa0, 0x000002e4, 0x00000000,
    0x00130061, 0x16260aa0, 0x000002e4, 0x00000000,
    0x213c1761, 0x00110204, 0x2a621761, 0x00110204,
    0x213e1761, 0x00110244, 0x2a121761, 0x00110244,
    0x21401761, 0x0011025c, 0x2a141761, 0x0011025c,
    0x21421761, 0x0011026c, 0x2a161761, 0x0011026c,
    0x01040022, 0x0001c060, 0x00002330, 0x00000968,
    0x00031761, 0x5a050220, 0x00443c26, 0x00000000,
    0x00131761, 0x5b050220, 0x00446226, 0x00000000,
    0xa15c0040, 0x028e3c03, 0xaa5d0040, 0x028e6203,
    0x00031a70, 0x5e050220, 0x52465c05, 0x00443c06,
    0x00131a70, 0x5f050220, 0x52465d05, 0x00446206,
    0x00041a70, 0x00018220, 0x42464805, 0x00000100,
    0x01040028, 0x0001c660, 0x000000d0, 0x000000d0,
    0xae600070, 0x00004803, 0x00040069, 0x10058660,
    0x02464805, 0x00000002, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x00041a61, 0x08052660,
    0x00466005, 0x00000000, 0xa0641a40, 0x10005c02,
    0x27661970, 0x5c006403, 0x00033261, 0x04060220,
    0x00346405, 0x00000000, 0x00133261, 0x06060220,
    0x00346505, 0x00000000, 0x00041b52, 0x68042e68,
    0x0e2e5e05, 0x66055a05, 0x00031961, 0x04260220,
    0x00346805, 0x00000000, 0x00131a61, 0x06260220,
    0x00346905, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb0c0424, 0x00040814, 0xa0480040, 0x20004803,
    0x00040027, 0x00014060, 0x00000000, 0xffffff20,
    0x00040069, 0x3a058660, 0x02465705, 0x00000002,
    0x00040061, 0x69054660, 0x00000000, 0x7f800000,
    0x00030040, 0x6b058660, 0x06443c06, 0x00002c2c,
    0x00130040, 0x6c058660, 0x06446206, 0x00002c2c,
    0x00031a70, 0x36050220, 0x52466b05, 0x00443c06,
    0x00131a70, 0x6d050220, 0x52466c05, 0x00446206,
    0x00031a40, 0x6e052660, 0x06463605, 0x00443c26,
    0x00131a40, 0x6f052660, 0x06466d05, 0x00446226,
    0x00041a70, 0x00018220, 0x42464605, 0x00000100,
    0x01040028, 0x0001c660, 0x00000678, 0x00000678,
    0x00030041, 0x20018220, 0x01464605, 0x05cc05cc,
    0x60720041, 0x5cc04602, 0x00041f61, 0x38050220,
    0x00463a05, 0x00000000, 0xfe700049, 0x5cc04603,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0xa0741b40, 0x72006b02, 0x00130041, 0x20018220,
    0x01464705, 0x05cc05cc, 0x27761a70, 0x6b007403,
    0xa07a0040, 0x02c07403, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0xa0011140, 0x4ac07403,
    0x00130049, 0x71058222, 0x02464705, 0x000005cc,
    0x277c1b70, 0x74007a03, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0xe7041b70, 0x4ac00103,
    0x00041b52, 0x78040e68, 0x0e2e6e05, 0x76057005,
    0xa07e1940, 0x78027c02, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0xa0061b40, 0x78020402,
    0x00041a70, 0x00018220, 0x42463805, 0x00000030,
    0x01040028, 0x0001c660, 0x000002c0, 0x000002c0,
    0x00043269, 0x08058660, 0x02463805, 0x00000002,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0xa00a1940, 0x08000102, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x270c1970, 0x01000a03,
    0x00033061, 0x28060220, 0x00340a05, 0x00000000,
    0x00133261, 0x2a060220, 0x00340b05, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0xa00e1b40, 0x06020c02, 0x00031961, 0x28260220,
    0x00340e05, 0x00000000, 0x00131a61, 0x2a260220,
    0x00340f05, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044031, 0x00000000,
    0xfb0c2824, 0x00045314, 0x00040052, 0x18044160,
    0x0e0e0004, 0x01050805, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x271a1970, 0x01001803,
    0x00033061, 0x29060220, 0x00341805, 0x00000000,
    0x80103201, 0x00000000, 0x00000000, 0x00000000,
    0x00133061, 0x2b060220, 0x00341905, 0x00000000,
    0xa01c1b40, 0x06021a02, 0x00031961, 0x29260220,
    0x00341c05, 0x00000000, 0x00131a61, 0x2b260220,
    0x00341d05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044031, 0x00000000,
    0xfb0c2924, 0x00045314, 0x00040052, 0x1d044160,
    0x0e0e0008, 0x01050805, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x271f1970, 0x01001d03,
    0x00033061, 0x2a060220, 0x00341d05, 0x00000000,
    0x80103201, 0x00000000, 0x00000000, 0x00000000,
    0x00133061, 0x2c060220, 0x00341e05, 0x00000000,
    0xa0211b40, 0x06021f02, 0x00031961, 0x2a260220,
    0x00342105, 0x00000000, 0x00131a61, 0x2c260220,
    0x00342205, 0x00000000, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb0c2a24, 0x00045314, 0x00040052, 0x22044160,
    0x0e0e000c, 0x01050805, 0x27241970, 0x01002203,
    0x00033261, 0x2b060220, 0x00342205, 0x00000000,
    0x00133261, 0x2d060220, 0x00342305, 0x00000000,
    0xa0261b40, 0x06022402, 0x00031961, 0x2b260220,
    0x00342605, 0x00000000, 0x00131a61, 0x2d260220,
    0x00342705, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb0c2b24, 0x00045314, 0xa0380040, 0x04003803,
    0x00040027, 0x00014060, 0x00000000, 0xfffffd30,
    0x00040061, 0x44050220, 0x00463a05, 0x00000000,
    0x00041970, 0x00018220, 0x42464405, 0x00000120,
    0x01040028, 0x0001c660, 0x00000260, 0x00000260,
    0x00043069, 0x27058660, 0x02464405, 0x00000002,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0xa0291940, 0x27007a02, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x272b1970, 0x7a002903,
    0x00033261, 0x09060220, 0x00342905, 0x00000000,
    0x00133261, 0x0b060220, 0x00342a05, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0xa02d1b40, 0x7e022b02, 0x00031961, 0x09260220,
    0x00342d05, 0x00000000, 0x00131a61, 0x0b260220,
    0x00342e05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb0c0924, 0x00046914, 0x00040052, 0x2e044160,
    0x0e0e0004, 0x7a052705, 0x27301970, 0x7a002e03,
    0x00033261, 0x0a060220, 0x00342e05, 0x00000000,
    0x00133261, 0x0c060220, 0x00342f05, 0x00000000,
    0xa0321b40, 0x7e023002, 0x00031961, 0x0a260220,
    0x00343205, 0x00000000, 0x00131a61, 0x0c260220,
    0x00343305, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb0c0a24, 0x00046914, 0x00040052, 0x33044160,
    0x0e0e0008, 0x7a052705, 0x27351970, 0x7a003303,
    0x00033261, 0x0b060220, 0x00343305, 0x00000000,
    0x00133261, 0x0d060220, 0x00343405, 0x00000000,
    0xa0371b40, 0x7e023502, 0x00031961, 0x0b260220,
    0x00343705, 0x00000000, 0x00131a61, 0x0d260220,
    0x00343805, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb0c0b24, 0x00046914, 0x00040052, 0x38044160,
    0x0e0e000c, 0x7a052705, 0x274c1970, 0x7a003803,
    0x00033261, 0x0c060220, 0x00343805, 0x00000000,
    0x00133261, 0x0e060220, 0x00343905, 0x00000000,
    0xa04e1b40, 0x7e024c02, 0x00031961, 0x0c260220,
    0x00344e05, 0x00000000, 0x00131a61, 0x0e260220,
    0x00344f05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb0c0c24, 0x00046914, 0xa0440040, 0x04004403,
    0x00040027, 0x00014060, 0x00000000, 0xfffffd90,
    0xa0460040, 0x02004603, 0x00040027, 0x00014060,
    0x00000000, 0xfffff978, 0x00040070, 0x00018660,
    0x16464a05, 0x00000000, 0x01040022, 0x0001c060,
    0x000000d0, 0x000000d0, 0x00030040, 0x37058660,
    0x06443c06, 0x0005f82c, 0x00130040, 0x4f058660,
    0x06446206, 0x0005f82c, 0x00031a70, 0x38050220,
    0x52463705, 0x00443c06, 0x00131a70, 0x50050220,
    0x52464f05, 0x00446206, 0x00030061, 0x18060220,
    0x00343705, 0x00000000, 0x00130061, 0x1a060220,
    0x00344f05, 0x00000000, 0x00031c40, 0x39052660,
    0x06463805, 0x00443c26, 0x00131c40, 0x51052660,
    0x06465005, 0x00446226, 0x00031a61, 0x18260220,
    0x00343905, 0x00000000, 0x00131a61, 0x1a260220,
    0x00345105, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb0c1824, 0x00045314, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x00040024, 0x0001c060,
    0x000019d8, 0x000019d8, 0x00040061, 0x4c054660,
    0x00000000, 0x00000001, 0x00041b70, 0x00018660,
    0x16464a05, 0x00000000, 0x01040022, 0x0001c060,
    0x00000070, 0x00000070, 0x00043261, 0x19054660,
    0x00000000, 0x00000000, 0x00043261, 0x1b054220,
    0x00000000, 0x00000000, 0x00040061, 0x1d054220,
    0x00000000, 0x00000000, 0x00040061, 0x1f054220,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xea0c1914, 0x001c1b34, 0x00040025, 0x00004600,
    0x00000000, 0x00001938, 0x80000001, 0x00000000,
    0x30000000, 0x00000000, 0x80000001, 0x00000000,
    0x30000000, 0x00000000, 0x80004331, 0x520c0000,
    0xe23e000c, 0x00000000, 0x80002301, 0x00000000,
    0x00000000, 0x00000000, 0x80033261, 0x53054220,
    0x00000000, 0x00000000, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x80011a61, 0x53550000,
    0x0000005c, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x8a044231, 0x00000000,
    0x3008530c, 0x00000000, 0x8a040001, 0x00000000,
    0xe0000000, 0x00000000, 0x00031661, 0x4e050220,
    0x00443e26, 0x00000000, 0x00131561, 0x4f050220,
    0x00441226, 0x00000000, 0x00043261, 0x1e054220,
    0x00000000, 0x00000000, 0x00040061, 0x21054220,
    0x00000000, 0x00000000, 0x00040061, 0x24054220,
    0x00000000, 0x00000000, 0x00040061, 0x50050220,
    0x00464805, 0x00000000, 0x80002101, 0x00000000,
    0x00000000, 0x00000000, 0x00041970, 0x00010220,
    0x42465005, 0x00000304, 0x01040028, 0x0001c660,
    0x000001c8, 0x000001c8, 0x00043269, 0x54058660,
    0x02465005, 0x00000003, 0xe0560068, 0x01d05003,
    0xa0671e40, 0x00102403, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0xa0691f40, 0x00102103,
    0xa06f1f40, 0x00101e03, 0xa0500040, 0x20005003,
    0xa13a1e40, 0x540e3e02, 0xaa581f40, 0x550e1202,
    0x00031a70, 0x59050220, 0x52463a05, 0x00443e06,
    0x00033261, 0x2c060220, 0x00343a05, 0x00000000,
    0x00131b70, 0x5a050220, 0x52465805, 0x00441206,
    0x00133261, 0x2e060220, 0x00345805, 0x00000000,
    0x00041a52, 0x5b040e68, 0x0e2e4e05, 0x59055605,
    0x00031961, 0x2c260220, 0x00345b05, 0x00000000,
    0x00131a61, 0x2e260220, 0x00345c05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x5c240000, 0xfb042c24, 0x000c0000,
    0xa0602240, 0x02405c03, 0x27101970, 0x5c006003,
    0x00033261, 0x30060220, 0x00346005, 0x00000000,
    0x00133261, 0x32060220, 0x00346105, 0x00000000,
    0xa064b240, 0x5e021002, 0x00031961, 0x30260220,
    0x00346405, 0x00000000, 0x00131a61, 0x32260220,
    0x00346505, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x65140000,
    0xfb043024, 0x00040000, 0x00042270, 0x6b058220,
    0x62466505, 0x00000100, 0x2f6d0062, 0x67002403,
    0xeb710070, 0x00606503, 0x2f241a62, 0x24006d03,
    0x00041a65, 0x00010220, 0x22467105, 0x00466b05,
    0x2f210062, 0x21006903, 0x00040070, 0x00018660,
    0x26467105, 0x00000000, 0x2f1e0062, 0x6f001e03,
    0x00040027, 0x00014060, 0x00000000, 0xfffffe18,
    0x00043261, 0x1c054220, 0x00000000, 0x00000000,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x01140000, 0xea181c14, 0x01001e14,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x00043061, 0x1f054220, 0x00000000, 0x00000004,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x44140000, 0xea181f14, 0x01002114,
    0x00043061, 0x22054220, 0x00000000, 0x00000008,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x46140000, 0xea182214, 0x01002414,
    0x80000001, 0x00000000, 0x30000000, 0x00000000,
    0x80000001, 0x00000000, 0x30000000, 0x00000000,
    0x80001f01, 0x00000000, 0x00000000, 0x00000000,
    0x80004431, 0x720c0000, 0xe23e000c, 0x00000000,
    0x80002401, 0x00000000, 0x00000000, 0x00000000,
    0x80030061, 0x73054220, 0x00000000, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80011961, 0x73550000, 0x0000005c, 0x00000000,
    0xe2350061, 0x00113004, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x8a044531, 0x00000000,
    0x3008730c, 0x00000000, 0x8a040001, 0x00000000,
    0xe0000000, 0x00000000, 0x80001a61, 0x30010220,
    0x00003504, 0x00000000, 0x80033061, 0x74054220,
    0x00000000, 0x00000000, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x8a004031, 0x560c0000,
    0xea00740c, 0x00300000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0xa00e3240, 0x56115612,
    0x27501970, 0x56100e03, 0x80002101, 0x00000000,
    0x00000000, 0x00000000, 0x00041a70, 0x00010220,
    0x42464805, 0x00000304, 0x01040028, 0x0001c660,
    0x000011a8, 0x000011a8, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x00040069, 0x75058660,
    0x02464805, 0x00000003, 0xe0770068, 0x01d04803,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00031261, 0x22050220, 0x00444226, 0x00000000,
    0x80103001, 0x00000000, 0x00000000, 0x00000000,
    0x00131161, 0x23050220, 0x00441626, 0x00000000,
    0xa1521c40, 0x750e3e02, 0xaa791d40, 0x760e1202,
    0x00031a70, 0x7a050220, 0x52465205, 0x00443e06,
    0x00033261, 0x31060220, 0x00345205, 0x00000000,
    0x00131b70, 0x7b050220, 0x52467905, 0x00441206,
    0x00133261, 0x33060220, 0x00347905, 0x00000000,
    0x00041a52, 0x7c040e68, 0x0e2e4e05, 0x7a057705,
    0x00031961, 0x31260220, 0x00347c05, 0x00000000,
    0x00131a61, 0x33260220, 0x00347d05, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044631, 0x04240000, 0xfb043124, 0x000c0000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x00032661, 0x38060220, 0x00340405, 0x00000000,
    0x80103001, 0x00000000, 0x00000000, 0x00000000,
    0x00132661, 0x3a060220, 0x00340505, 0x00000000,
    0xa07d0040, 0x02400403, 0x0003b661, 0x38260220,
    0x00340605, 0x00000000, 0x0013b661, 0x3a260220,
    0x00340705, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x27181b70, 0x04007d03,
    0x00033061, 0x27060220, 0x00347d05, 0x00000000,
    0x80103201, 0x00000000, 0x00000000, 0x00000000,
    0x00133061, 0x29060220, 0x00347e05, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0xa01a1b40, 0x06021802, 0x00031961, 0x27260220,
    0x00341a05, 0x00000000, 0x00131a61, 0x29260220,
    0x00341b05, 0x00000000, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044731, 0x18340000,
    0xfb042724, 0x001c0000, 0x00042770, 0x00018220,
    0x32461805, 0x00000006, 0x01040022, 0x0001c060,
    0x00000310, 0x00000218, 0xa0242040, 0x00104603,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0xa0263040, 0x46000e02, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0xa02c2040, 0x00104403,
    0xa02e3240, 0x44015602, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0x27281b70, 0x0e002603,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80003601, 0x00000000, 0x00000000, 0x00000000,
    0x27301a70, 0x56102e03, 0x00043670, 0x32058220,
    0x62461805, 0x00000100, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0xa02a1b40, 0x50222802,
    0x2f363262, 0x26002e03, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0xaf343662, 0x2a023002,
    0x00031a41, 0x20018220, 0x01463605, 0x00580058,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x60541b41, 0x05803602, 0x60571b41, 0x05803402,
    0xfe520049, 0x05803603, 0xa1591b40, 0x540e4202,
    0xaa5a1c40, 0x550e1602, 0x00130041, 0x20018220,
    0x01463705, 0x00580058, 0x00031b70, 0x5b050220,
    0x52465905, 0x00444206, 0x00032061, 0x1e060220,
    0x00345905, 0x00000000, 0x00131c70, 0x5c050220,
    0x52465a05, 0x00441606, 0x00133061, 0x20060220,
    0x00345a05, 0x00000000, 0x00040070, 0x00018660,
    0x26463205, 0x00000000, 0x80103201, 0x00000000,
    0x00000000, 0x00000000, 0x00130049, 0x53058222,
    0x02463705, 0x00000058, 0x2f460062, 0x24004603,
    0x2f440062, 0x44002c03, 0xa0591b40, 0x57005202,
    0x00041952, 0x5d040e68, 0x0e2e2205, 0x5b055905,
    0x00031961, 0x1e260220, 0x00345d05, 0x00000000,
    0x00131a61, 0x20260220, 0x00345e05, 0x00000000,
    0x00040024, 0x0001c060, 0x00000108, 0x00000108,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0xa0522040, 0x00100103, 0x00030041, 0x20018220,
    0x01460105, 0x00580058, 0x60600041, 0x05800102,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0xfe5e1d49, 0x05800103, 0xa15a1a40, 0x600e4202,
    0xaa611b40, 0x610e1602, 0x00130041, 0x20018220,
    0x01460205, 0x00580058, 0x00031b70, 0x10050220,
    0x52465a05, 0x00444206, 0x00032061, 0x1e060220,
    0x00345a05, 0x00000000, 0x00131c70, 0x11050220,
    0x52466105, 0x00441606, 0x00133061, 0x20060220,
    0x00346105, 0x00000000, 0x00130049, 0x5f058222,
    0x02460205, 0x00000058, 0x00040061, 0x01050220,
    0x00465205, 0x00000000, 0x00041a52, 0x64040e68,
    0x0e2e2205, 0x10055e05, 0x00031961, 0x1e260220,
    0x00346405, 0x00000000, 0x00131a61, 0x20260220,
    0x00346505, 0x00000000, 0x00040025, 0x00004600,
    0x00000000, 0x00000c28, 0x00040069, 0x65058660,
    0x02464805, 0x00000006, 0xe0670068, 0x01a04803,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x00031461, 0x69050220, 0x00444026, 0x00000000,
    0x80103201, 0x00000000, 0x00000000, 0x00000000,
    0x00131361, 0x6a050220, 0x00441426, 0x00000000,
    0xa1521c40, 0x650e4002, 0xaa531d40, 0x660e1402,
    0x00031a70, 0x6b050220, 0x52465205, 0x00444006,
    0x00131a70, 0x6c050220, 0x52465305, 0x00441406,
    0xa06d0040, 0x00805203, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x00041a52, 0x54040e68,
    0x0e2e6905, 0x6b056705, 0x276f1a70, 0x52006d03,
    0x00030061, 0x22060220, 0x00346d05, 0x00000000,
    0x00133061, 0x24060220, 0x00346e05, 0x00000000,
    0xa0711b40, 0x54026f02, 0x00031961, 0x22260220,
    0x00347105, 0x00000000, 0x00131a61, 0x24260220,
    0x00347205, 0x00000000, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044831, 0x26240000,
    0xfb042224, 0x000c0000, 0x80002801, 0x00000000,
    0x00000000, 0x00000000, 0x00044931, 0x00000000,
    0xfb0c1e24, 0x000c2624, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0xa0720040, 0x01005203,
    0xa15b3940, 0x008e1e03, 0xaa773940, 0x008e2003,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x27741b70, 0x52007203, 0x00033861, 0x22060220,
    0x00347205, 0x00000000, 0x00133861, 0x24060220,
    0x00347305, 0x00000000, 0x00031d70, 0x5c050220,
    0x52465b05, 0x00441e06, 0x00131d70, 0x78050220,
    0x52467705, 0x00442006, 0x00033961, 0x26060220,
    0x00345b05, 0x00000000, 0x00133961, 0x28060220,
    0x00347705, 0x00000000, 0xa0761f40, 0x54027402,
    0x00031d40, 0x5d052660, 0x06465c05, 0x00441e26,
    0x00131d40, 0x79052660, 0x06467805, 0x00442026,
    0x00031b61, 0x22260220, 0x00347605, 0x00000000,
    0x00131c61, 0x24260220, 0x00347705, 0x00000000,
    0x00031c61, 0x26260220, 0x00345d05, 0x00000000,
    0x00131c61, 0x28260220, 0x00347905, 0x00000000,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00044a31, 0x2a240000, 0xfb042224, 0x000c0000,
    0x80002a01, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044b31, 0x00000000, 0xfb0c2624, 0x000c2a24,
    0xa07a0040, 0x01805203, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0xa15e0040, 0x010e1e03,
    0xaa7f0040, 0x010e2003, 0x277c1b70, 0x52007a03,
    0x00033a61, 0x22060220, 0x00347a05, 0x00000000,
    0x00133a61, 0x24060220, 0x00347b05, 0x00000000,
    0x00031d70, 0x5f050220, 0x52465e05, 0x00441e06,
    0x80103b01, 0x00000000, 0x00000000, 0x00000000,
    0x00131d70, 0x2a050220, 0x52467f05, 0x00442006,
    0x00033b61, 0x26060220, 0x00345e05, 0x00000000,
    0x00133b61, 0x28060220, 0x00347f05, 0x00000000,
    0xa07e1f40, 0x54027c02, 0x00031d40, 0x60052660,
    0x06465f05, 0x00441e26, 0x80103b01, 0x00000000,
    0x00000000, 0x00000000, 0x00131d40, 0x2b052660,
    0x06462a05, 0x00442026, 0x00031b61, 0x22260220,
    0x00347e05, 0x00000000, 0x00131c61, 0x24260220,
    0x00347f05, 0x00000000, 0x00031c61, 0x26260220,
    0x00346005, 0x00000000, 0x00131c61, 0x28260220,
    0x00342b05, 0x00000000, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044c31, 0x2a240000,
    0xfb042224, 0x000c0000, 0x80003601, 0x00000000,
    0x00000000, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x00032c61, 0x34060220,
    0x00342a05, 0x00000000, 0x80103201, 0x00000000,
    0x00000000, 0x00000000, 0x00132c61, 0x36060220,
    0x00342b05, 0x00000000, 0x0003ac61, 0x34260220,
    0x00342c05, 0x00000000, 0x0013ac61, 0x36260220,
    0x00342d05, 0x00000000, 0x80001d01, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb0c2624, 0x000c2a24, 0xa02e3240, 0x02005203,
    0xa1610040, 0x018e1e03, 0xaa333640, 0x018e2003,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80003601, 0x00000000, 0x00000000, 0x00000000,
    0x27301b70, 0x52002e03, 0x00033c61, 0x22060220,
    0x00342e05, 0x00000000, 0x00133c61, 0x24060220,
    0x00342f05, 0x00000000, 0x00031d70, 0x10050220,
    0x52466105, 0x00441e06, 0x00131d70, 0x57050220,
    0x52463305, 0x00442006, 0x00033261, 0x26060220,
    0x00346105, 0x00000000, 0x00133261, 0x28060220,
    0x00343305, 0x00000000, 0x80003601, 0x00000000,
    0x00000000, 0x00000000, 0xa0321f40, 0x54023002,
    0x00031d40, 0x11052660, 0x06461005, 0x00441e26,
    0x00131d40, 0x58052660, 0x06465705, 0x00442026,
    0x00031b61, 0x22260220, 0x00343205, 0x00000000,
    0x00131c61, 0x24260220, 0x00343305, 0x00000000,
    0x00031c61, 0x26260220, 0x00341105, 0x00000000,
    0x00131c61, 0x28260220, 0x00345805, 0x00000000,
    0x80002201, 0x00000000, 0x00000000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00044d31, 0x08240000, 0xfb042224, 0x000c0000,
    0x80002d01, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x00000000, 0xfb0c2624, 0x003c0444,
    0xa1640040, 0x034e1e03, 0xaa590040, 0x034e2003,
    0x00040061, 0x2e050660, 0x00461805, 0x00000000,
    0x00042761, 0x30050660, 0x00461c05, 0x00000000,
    0x00042761, 0x32050660, 0x00461a05, 0x00000000,
    0x00031d70, 0x65050220, 0x52466405, 0x00441e06,
    0x00131d70, 0x5a050220, 0x52465905, 0x00442006,
    0x00033d61, 0x22060220, 0x00346405, 0x00000000,
    0x00133d61, 0x24060220, 0x00345905, 0x00000000,
    0x00031c40, 0x66052660, 0x06466505, 0x00441e26,
    0x00131c40, 0x5b052660, 0x06465a05, 0x00442026,
    0x00031a61, 0x22260220, 0x00346605, 0x00000000,
    0x00131a61, 0x24260220, 0x00345b05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x00000000, 0xfb0c2224, 0x001c2e34,
    0xa05c0040, 0x03005203, 0xa1670040, 0x030e1e03,
    0xaa610040, 0x030e2003, 0x275e1b70, 0x52005c03,
    0x00033261, 0x22060220, 0x00345c05, 0x00000000,
    0x00133261, 0x24060220, 0x00345d05, 0x00000000,
    0x00031d70, 0x68050220, 0x52466705, 0x00441e06,
    0x00131d70, 0x10050220, 0x52466105, 0x00442006,
    0x00033261, 0x26060220, 0x00346705, 0x00000000,
    0x00133261, 0x28060220, 0x00346105, 0x00000000,
    0xa0601f40, 0x54025e02, 0x00031d40, 0x69052660,
    0x06466805, 0x00441e26, 0x00131d40, 0x11052660,
    0x06461005, 0x00442026, 0x00031b61, 0x22260220,
    0x00346005, 0x00000000, 0x00131c61, 0x24260220,
    0x00346105, 0x00000000, 0x00031c61, 0x26260220,
    0x00346905, 0x00000000, 0x00131c61, 0x28260220,
    0x00341105, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x00044e31, 0x2e140000,
    0xfb042224, 0x00040000, 0x80002e01, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb0c2624, 0x00042e14, 0xa0640040, 0x02805203,
    0xa16a0040, 0x028e1e03, 0xaa690040, 0x028e2003,
    0x27661b70, 0x52006403, 0x00033e61, 0x22060220,
    0x00346405, 0x00000000, 0x00133e61, 0x24060220,
    0x00346505, 0x00000000, 0x00031d70, 0x6b050220,
    0x52466a05, 0x00441e06, 0x00033261, 0x26060220,
    0x00346a05, 0x00000000, 0x80103201, 0x00000000,
    0x00000000, 0x00000000, 0x00131e61, 0x28060220,
    0x00346905, 0x00000000, 0x00130070, 0x6a050220,
    0x52466905, 0x00442006, 0xa0681f40, 0x54026602,
    0x00031d40, 0x6c052660, 0x06466b05, 0x00441e26,
    0x00131b40, 0x6b052660, 0x06466a05, 0x00442026,
    0x00031b61, 0x22260220, 0x00346805, 0x00000000,
    0x00131c61, 0x24260220, 0x00346905, 0x00000000,
    0x00031c61, 0x26260220, 0x00346c05, 0x00000000,
    0x00131c61, 0x28260220, 0x00346b05, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x1e240000, 0xfb042224, 0x000c0000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x00000000, 0xfb0c2624, 0x000c1e24,
    0x00040070, 0x00018220, 0x32461805, 0x00000100,
    0x01040022, 0x0001c060, 0x00000268, 0x00000268,
    0x00044f31, 0x00000000, 0xfb0c3424, 0x00044c14,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x18440000, 0xfb043824, 0x003c0000,
    0xa06c3240, 0x01000403, 0xa0713240, 0x02002a03,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x276e1a70, 0x04006c03, 0x00033261, 0x2e060220,
    0x00346c05, 0x00000000, 0x00133261, 0x30060220,
    0x00346d05, 0x00000000, 0x27731c70, 0x2a007103,
    0x00033261, 0x32060220, 0x00347105, 0x00000000,
    0x00133f61, 0x34060220, 0x00347205, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0xa0701e40, 0x06026e02, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0xa0751c40, 0x2c027302,
    0x00031a61, 0x2e260220, 0x00347005, 0x00000000,
    0x00131b61, 0x30260220, 0x00347105, 0x00000000,
    0x00031b61, 0x32260220, 0x00347505, 0x00000000,
    0x00131c61, 0x34260220, 0x00347605, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x04440000, 0xfb042e24, 0x003c0000,
    0x00042061, 0x20050660, 0x00461805, 0x00000000,
    0x00042061, 0x22050660, 0x00461a05, 0x00000000,
    0x00042061, 0x24050660, 0x00461c05, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00042261, 0x26050660, 0x00460405, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x00000000, 0xfb0c3224, 0x003c2044,
    0xa0760040, 0x03002a03, 0x80003f01, 0x00000000,
    0x00000000, 0x00000000, 0x00042261, 0x36050660,
    0x00460605, 0x00000000, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x00042261, 0x38050660,
    0x00460805, 0x00000000, 0xe7781b70, 0x03007603,
    0x00033061, 0x32060220, 0x00347605, 0x00000000,
    0x00133061, 0x34060220, 0x00347705, 0x00000000,
    0xa07a1b40, 0x2c027802, 0x00031961, 0x32260220,
    0x00347a05, 0x00000000, 0x00131a61, 0x34260220,
    0x00347b05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb0c3224, 0x000c3624, 0x00040025, 0x00004600,
    0x00000000, 0x00000018, 0xa0480040, 0x20004803,
    0x00040027, 0x00014060, 0x00000000, 0xffffee38,
    0x00040070, 0x00018660, 0x16464a05, 0x00000000,
    0x01040022, 0x0001c060, 0x00000240, 0x00000240,
    0xa16d1740, 0x010e3c03, 0xaa7b1740, 0x010e6203,
    0x00043261, 0x33050660, 0x00005604, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80001f01, 0x00000000, 0x00000000, 0x00000000,
    0x00040061, 0x35050660, 0x00005624, 0x00000000,
    0x00031c70, 0x6e050220, 0x52466d05, 0x00443c06,
    0x00131c70, 0x7c050220, 0x52467b05, 0x00446206,
    0x00033261, 0x2f060220, 0x00346d05, 0x00000000,
    0x00133261, 0x31060220, 0x00347b05, 0x00000000,
    0x00031c40, 0x6f052660, 0x06466e05, 0x00443c26,
    0x00131c40, 0x7d052660, 0x06467c05, 0x00446226,
    0x00031a61, 0x2f260220, 0x00346f05, 0x00000000,
    0x00131a61, 0x31260220, 0x00347d05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x00000000, 0xfb0c2f24, 0x000c3324,
    0x80030061, 0x7f054220, 0x00000000, 0x00000008,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0xa1700040, 0x020e3c03, 0xaa012040, 0x020e6203,
    0x80001c01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004031, 0x7e0c0000, 0xea007f0c, 0x00300000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x00031a61, 0x34060220, 0x00347005, 0x00000000,
    0x80103201, 0x00000000, 0x00000000, 0x00000000,
    0x00131a61, 0x36060220, 0x00340105, 0x00000000,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x00043261, 0x0c050660, 0x00007e04, 0x00000000,
    0x00030070, 0x71050220, 0x52467005, 0x00443c06,
    0x00132070, 0x02050220, 0x52460105, 0x00446206,
    0x00031a40, 0x72052660, 0x06467105, 0x00443c26,
    0x80102101, 0x00000000, 0x00000000, 0x00000000,
    0x00131a40, 0x03052660, 0x06460205, 0x00446226,
    0x00031a61, 0x34260220, 0x00347205, 0x00000000,
    0x00131a61, 0x36260220, 0x00340305, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x00000000, 0xfb0c3424, 0x000c0c24,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x80031f61, 0x7e050220, 0x00460005, 0x00000000,
    0x80040931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_build_BFS_categorize_builds_and_init_scheduler = {
   .prog_data = {
      .base.nr_params = 17,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 3,
      .base.total_scratch = 0,
      .base.total_shared = 12,
      .base.program_size = 9488,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_build_BFS_categorize_builds_and_init_scheduler_relocs,
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
      .push.cross_thread.dwords = 17,
      .push.cross_thread.regs = 3,
      .push.cross_thread.size = 96,
   },
   .args_size = 36,
   .arg_count = 5,
   .args = gfx125_bvh_build_BFS_categorize_builds_and_init_scheduler_args,
   .code = gfx125_bvh_build_BFS_categorize_builds_and_init_scheduler_code,
};
const char *gfx125_bvh_build_BFS_categorize_builds_and_init_scheduler_sha1 = "f514667a80fab6af7b1e48e46e287299c1a86e11";
