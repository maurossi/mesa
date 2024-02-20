#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_build_BFS_begin_batchable_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_build_BFS_begin_batchable_args[] = {
   { 0, 8 },
   { 8, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g73<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g43<1>D         0D                              { align1 1H };
and(16)         g41<1>UD        g0.2<0,1,0>UD   0x000000ffUD    { align1 1H compacted };
add(1)          g74<1>UD        g73<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@3 compacted };
mov(8)          g31<1>UW        0x76543210V                     { align1 WE_all 1Q };
shl(16)         g60<1>D         g41<8,8,1>D     0x00000004UD    { align1 1H I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(1)         g1UD            g74UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
add(8)          g31.8<1>UW      g31<1,1,0>UW    0x0008UW        { align1 WE_all 1Q I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         g62<1>D         g31<8,8,1>UW                    { align1 1H };
add(16)         g64<1>D         g62<1,1,0>D     g60<1,1,0>D     { align1 1H I@1 compacted };
mov(16)         g66<1>UD        g64<32,8,4>UB                   { align1 1H I@1 };
mov(16)         g68<1>UW        g64<32,8,4>UB                   { align1 1H };
mov(16)         g69<1>UD        g66<16,8,2>UW                   { align1 1H I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g28.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g45.1<2>F       g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g17.1<2>F       g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g47.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g28<2>F         g2<0,1,0>F                      { align1 1Q F@4 compacted };
mov(8)          g45<2>F         g2<0,1,0>F                      { align1 2Q F@4 compacted };
mov(8)          g17<2>F         g2.2<0,1,0>F                    { align1 1Q F@4 compacted };
mov(8)          g47<2>F         g2.2<0,1,0>F                    { align1 2Q F@4 compacted };
mov(8)          g49<1>UD        g28.1<8,4,2>UD                  { align1 1Q F@4 };
add(8)          g51<1>D         g28<8,4,2>D     32D             { align1 1Q compacted };
mov(8)          g50<1>UD        g45.1<8,4,2>UD                  { align1 2Q F@3 };
add(8)          g52<1>D         g45<8,4,2>D     32D             { align1 2Q compacted };
mov(8)          g32<2>UD        g51<4,4,1>UD                    { align1 1Q I@3 };
cmp.l.f0.0(16)  g53<1>UD        g51<1,1,0>UD    0x00000020UD    { align1 1H I@2 compacted };
mov(8)          g34<2>UD        g52<4,4,1>UD                    { align1 2Q };
add(8)          g19<1>D         -g53<8,8,1>D    g28.1<8,4,2>D   { align1 1Q I@2 };
add(8)          g54<1>D         -g54<8,8,1>D    g45.1<8,4,2>D   { align1 2Q I@3 };
mov(8)          g32.1<2>UD      g19<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g34.1<2>UD      g54<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g55UD           g32UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
mov(16)         g71<2>UW        g55<8,8,1>UD                    { align1 1H $1.dst };
sel.l(16)       g59<1>UW        g71<16,8,2>UW   0x0100UW        { align1 1H I@1 };
mov(16)         g26<1>UD        g59<8,8,1>UW                    { align1 1H I@1 };
cmp.l.f0.0(16)  null<1>D        g69<8,8,1>D     g26<8,8,1>D     { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL1          UIP:  LABEL0              { align1 1H };
add(16)         g71<1>D         g66<1,1,0>D     g57<1,1,0>D     { align1 1H $1.dst compacted };
add(8)          g76<1>D         g17<8,4,2>D     52D             { align1 1Q F@2 compacted };
add(8)          g77<1>D         g47<8,4,2>D     52D             { align1 2Q F@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
add(8)          g87<1>D         g28<8,4,2>D     11308D          { align1 1Q };
add(8)          g88<1>D         g45<8,4,2>D     11308D          { align1 2Q };
mul(16)         g90<1>D         g69<1,1,0>D     1484W           { align1 1H compacted };
mov(16)         g9<1>D          0D                              { align1 1H };
mov(16)         g13<1>D         0D                              { align1 1H };
mov(16)         g15<1>D         0D                              { align1 1H };
mov(16)         g37<1>UD        g71<16,8,2>UW                   { align1 1H I@7 };
mov(16)         g30<2>UW        g71<8,8,1>UD                    { align1 1H };
cmp.l.f0.0(8)   g20<1>UD        g76<8,8,1>UD    g17<8,4,2>UD    { align1 1Q I@7 };
cmp.l.f0.0(8)   g78<1>UD        g77<8,8,1>UD    g47<8,4,2>UD    { align1 2Q I@7 };
add(16)         g92<1>D         g87<1,1,0>D     g90<1,1,0>D     { align1 1H I@7 compacted };
mul(8)          acc0<1>UD       g37<8,8,1>UD    0x0058UW        { align1 1Q I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mul(16)         g74<1>D         g37<1,1,0>D     88W             { align1 1H I@6 compacted };
add(8)          g79<1>D         -g20<8,8,1>D    g17.1<8,4,2>D   { align1 1Q I@5 };
add(8)          g80<1>D         -g78<8,8,1>D    g47.1<8,4,2>D   { align1 2Q I@5 };
mov(8)          g5<2>UD         g92<4,4,1>UD                    { align1 1Q I@5 };
mov(8)          g7<2>UD         g93<4,4,1>UD                    { align1 2Q I@6 };
mach(8)         g72<1>UD        g37<1,1,0>UD    0x00000058UD    { align1 1Q compacted AccWrEnable };
add(16)         g81<1>D         g76<1,1,0>D     g74<1,1,0>D     { align1 1H I@6 compacted };
mul(8)          acc0<1>UD       g38<8,8,1>UD    0x0058UW        { align1 2Q };
cmp.l.f0.0(16)  g83<1>UD        g81<1,1,0>UD    g76<1,1,0>UD    { align1 1H I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g1<2>UD         g81<4,4,1>UD                    { align1 1Q F@1 };
mov(8)          g3<2>UD         g82<4,4,1>UD                    { align1 2Q };
cmp.l.f0.0(8)   g89<1>UD        g87<8,8,1>UD    g28<8,4,2>UD    { align1 1Q };
cmp.l.f0.0(8)   g90<1>UD        g88<8,8,1>UD    g45<8,4,2>UD    { align1 2Q };
mach(8)         g73<1>UD        g38<8,8,1>UD    0x00000058UD    { align1 2Q AccWrEnable };
cmp.l.f0.0(16)  g94<1>UD        g92<1,1,0>UD    g87<1,1,0>UD    { align1 1H compacted };
add3(16)        g85<1>D         g79<8,8,1>D     g72<8,8,1>D     -g83<1,1,1>D { align1 1H I@2 };
add3(16)        g96<1>D         -g89<8,8,1>D    g49<8,8,1>D     -g94<1,1,1>D { align1 1H I@2 };
mov(8)          g1.1<2>UD       g85<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g3.1<2>UD       g86<4,4,1>UD                    { align1 2Q I@3 };
mov(8)          g5.1<2>UD       g96<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g7.1<2>UD       g97<4,4,1>UD                    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g11UD           g1UD            nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g5UD            g9UD            0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $3 };
add(16)         g98<1>D         g92<1,1,0>D     28D             { align1 1H compacted };
cmp.l.f0.0(16)  g100<1>UD       g98<1,1,0>UD    g92<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g33<2>UD        g98<4,4,1>UD                    { align1 1Q $1.src };
mov(8)          g35<2>UD        g99<4,4,1>UD                    { align1 2Q $1.src };
add(16)         g102<1>D        -g100<1,1,0>D   g96<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g33.1<2>UD      g102<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g35.1<2>UD      g103<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g33UD           g37UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $1 };
shl(16)         g103<1>D        g69<8,8,1>D     0x00000002UD    { align1 1H };
add(8)          g105<1>D        g28<8,4,2>D     g103<1,1,0>D    { align1 1Q I@1 compacted };
add(8)          g106<1>D        g45<8,4,2>D     g104<1,1,0>D    { align1 2Q I@2 compacted };
cmp.l.f0.0(8)   g21<1>UD        g105<8,8,1>UD   g28<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g107<1>UD       g106<8,8,1>UD   g45<8,4,2>UD    { align1 2Q I@2 };
add(16)         g110<1>D        g105<1,1,0>D    40D             { align1 1H compacted };
add(8)          g108<1>D        -g21<8,8,1>D    g28.1<8,4,2>D   { align1 1Q I@3 };
add(8)          g109<1>D        -g107<8,8,1>D   g45.1<8,4,2>D   { align1 2Q I@3 };
cmp.l.f0.0(16)  g112<1>UD       g110<1,1,0>UD   g105<1,1,0>UD   { align1 1H I@3 compacted };
mov(8)          g51<2>UD        g110<4,4,1>UD                   { align1 1Q };
mov(8)          g53<2>UD        g111<4,4,1>UD                   { align1 2Q };
add(16)         g114<1>D        -g112<1,1,0>D   g108<1,1,0>D    { align1 1H I@3 compacted };
mov(8)          g51.1<2>UD      g114<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g53.1<2>UD      g115<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g51UD           g43UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $4 };
add(16)         g115<1>D        g92<1,1,0>D     16D             { align1 1H compacted };
mov(16)         g17<1>D         0D                              { align1 1H };
mov(16)         g19<1>D         0D                              { align1 1H };
mov(16)         g21<1>D         0D                              { align1 1H };
cmp.l.f0.0(16)  g117<1>UD       g115<1,1,0>UD   g92<1,1,0>UD    { align1 1H I@4 compacted };
mov(8)          g64<2>UD        g115<4,4,1>UD                   { align1 1Q };
mov(8)          g66<2>UD        g116<4,4,1>UD                   { align1 2Q };
add(16)         g119<1>D        -g117<1,1,0>D   g96<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g64.1<2>UD      g119<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g66.1<2>UD      g120<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g64UD           g17UD           0x08007586                0x00000180
                            ugm MsgDesc: ( store_cmask, a64, d32, xyz, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 6 flat )  base_offset 0  { align1 1H $5 };
add(16)         g120<1>D        g11<1,1,0>D     511D            { align1 1H $3.src compacted };
add(16)         g122<1>D        g105<1,1,0>D    1068D           { align1 1H compacted };
shr(16)         g73<1>UD        g120<1,1,0>UD   0x00000009UD    { align1 1H I@2 compacted };
cmp.l.f0.0(16)  g124<1>UD       g122<1,1,0>UD   g105<1,1,0>UD   { align1 1H I@2 compacted };
mov(8)          g69<2>UD        g122<4,4,1>UD                   { align1 1Q };
mov(8)          g71<2>UD        g123<4,4,1>UD                   { align1 2Q };
add(16)         g126<1>D        -g124<1,1,0>D   g108<1,1,0>D    { align1 1H I@3 compacted };
mov(8)          g69.1<2>UD      g126<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g71.1<2>UD      g127<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g69UD           g73UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };
add(8)          g1<1>D          g28<8,4,2>D     2092D           { align1 1Q $2.src };
add(8)          g2<1>D          g45<8,4,2>D     2092D           { align1 2Q $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
cmp.l.f0.0(16)  g3<1>UD         g1<1,1,0>UD     0x0000082cUD    { align1 1H I@1 compacted };
add(16)         g5<1>D          g1<1,1,0>D      g103<1,1,0>D    { align1 1H $3.src compacted };
mov(16)         g79<2>UW        g30<16,8,2>UW                   { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
cmp.l.f0.0(16)  g7<1>UD         g5<1,1,0>UD     g1<1,1,0>UD     { align1 1H I@2 compacted };
mov(8)          g75<2>UD        g5<4,4,1>UD                     { align1 1Q };
mov(8)          g77<2>UD        g6<4,4,1>UD                     { align1 2Q };
mov(16)         g79.1<2>UW      g68<8,8,1>UW                    { align1 1H I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
add3(16)        g9<1>D          -g3<8,8,1>D     g49<8,8,1>D     -g7<1,1,1>D { align1 1H I@4 };
mov(8)          g75.1<2>UD      g9<4,4,1>UD                     { align1 1Q I@1 };
mov(8)          g77.1<2>UD      g10<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g75UD           g79UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $5 };
else(16)        JIP:  LABEL0          UIP:  LABEL0              { align1 1H };

LABEL1:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(16)         g73<1>UD        0x00000000UD                    { align1 1H };

LABEL0:
endif(16)       JIP:  LABEL2                                    { align1 1H };

LABEL2:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(16)         g10<1>D         0D                              { align1 WE_all 1H I@2 };
mov(16)         g10<1>D         g73<8,8,1>D                     { align1 1H };
cmp.z.f0.0(16)  null<1>W        g68<16,16,1>W   0W              { align1 1H I@7 };
add(8)          g10.1<2>D       g10<8,4,2>D     g10.1<8,4,2>D   { align1 WE_all 1Q I@2 };
add(4)          g10.2<4>D       g10.1<8,2,4>D   g10.2<8,2,4>D   { align1 WE_all 1N I@1 };
add(4)          g10.3<4>D       g10.1<8,2,4>D   g10.3<8,2,4>D   { align1 WE_all 1N I@1 };
add(4)          g10.4<1>D       g10.3<0,1,0>D   g10.4<4,4,1>D   { align1 WE_all 1N I@1 };
add(4)          g11.4<1>D       g11.3<0,1,0>D   g11.4<4,4,1>D   { align1 WE_all 1N I@2 };
add(8)          g11<1>D         g10.7<0,1,0>D   g11<1,1,0>D     { align1 WE_all 1Q I@1 compacted };
(+f0.0) if(16)  JIP:  LABEL3          UIP:  LABEL3              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(16)         g12<1>UD        g57<16,8,2>UW                   { align1 1H $1.dst };
mov(16)         g14<1>UD        g55<16,8,2>UW                   { align1 1H $3.src };
add(8)          g30<1>D         g28<8,4,2>D     24D             { align1 1Q compacted };
add(8)          g16<1>D         g45<8,4,2>D     24D             { align1 2Q $3.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@2 };
mov(16)         g18<1>D         g11.7<0,1,0>D                   { align1 1H $5.src };
add(16)         g24<1>D         g12<1,1,0>D     g26<1,1,0>D     { align1 1H I@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
add(16)         g22<1>D         g14<1,1,0>D     -g26<1,1,0>D    { align1 1H I@5 compacted };
cmp.l.f0.0(8)   g31<1>UD        g30<8,8,1>UD    g28<8,4,2>UD    { align1 1Q I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $5.src };
cmp.l.f0.0(8)   g17<1>UD        g16<8,8,1>UD    g45<8,4,2>UD    { align1 2Q A@2 };
mov(8)          g80<2>UD        g30<4,4,1>UD                    { align1 1Q $5.src };
mov(8)          g82<2>UD        g16<4,4,1>UD                    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
add(8)          g33<1>D         -g31<8,8,1>D    g28.1<8,4,2>D   { align1 1Q I@4 };
cmp.l.f0.0(16)  g31<1>D         g26<1,1,0>D     g14<1,1,0>D     { align1 1H $1.src compacted };
add(8)          g30<1>D         -g17<8,8,1>D    g45.1<8,4,2>D   { align1 2Q I@5 };
mov(8)          g80.1<2>UD      g33<4,4,1>UD                    { align1 1Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(16)         g20<1>D         -g31<8,8,1>D                    { align1 1H I@3 };
mov(8)          g82.1<2>UD      g30<4,4,1>UD                    { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g80UD           g18UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $5 };
add(8)          g34<1>D         g28<8,4,2>D     1064D           { align1 1Q $1.src compacted };
add(8)          g32<1>D         g45<8,4,2>D     1064D           { align1 2Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
cmp.l.f0.0(8)   g35<1>UD        g34<8,8,1>UD    g28<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g33<1>UD        g32<8,8,1>UD    g45<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g81<2>UD        g34<4,4,1>UD                    { align1 1Q $5.src };
mov(8)          g83<2>UD        g32<4,4,1>UD                    { align1 2Q $5.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
add(8)          g36<1>D         -g35<8,8,1>D    g28.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g34<1>D         -g33<8,8,1>D    g45.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g81.1<2>UD      g36<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g83.1<2>UD      g34<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g81UD           g26UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $6 };

LABEL3:
endif(16)       JIP:  LABEL4                                    { align1 1H };

LABEL4:
add(8)          g47<1>D         g28<8,4,2>D     11308D          { align1 1Q F@1 };
add(8)          g48<1>D         g45<8,4,2>D     11308D          { align1 2Q F@1 };
shl(16)         g37<1>D         g62<8,8,1>D     0x00000002UD    { align1 1H $1.src };
mov(16)         g52<1>D         2139095040D                     { align1 1H $4.src };
mov(16)         g35<1>UD        g41<8,8,1>UD                    { align1 1H $1.src };
cmp.l.f0.0(8)   g39<1>UD        g47<8,8,1>UD    g28<8,4,2>UD    { align1 1Q I@5 };
cmp.l.f0.0(8)   g49<1>UD        g48<8,8,1>UD    g45<8,4,2>UD    { align1 2Q I@5 };
add(8)          g50<1>D         -g39<8,8,1>D    g28.1<8,4,2>D   { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $4.src };
add(8)          g51<1>D         -g49<8,8,1>D    g45.1<8,4,2>D   { align1 2Q I@2 };

LABEL10:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
cmp.ge.f0.0(16) null<1>UD       g35<8,8,1>UD    g26<8,8,1>UD    { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL5        UIP:  LABEL5              { align1 1H };
mul(16)         g54<1>D         g35<1,1,0>D     1484W           { align1 1H $4.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(16)         g32<1>UD        g37<8,8,1>UD                    { align1 1H I@6 };
add(16)         g56<1>D         g47<1,1,0>D     g54<1,1,0>D     { align1 1H @2 $1.dst compacted };
cmp.l.f0.0(16)  g58<1>UD        g56<1,1,0>UD    g47<1,1,0>UD    { align1 1H @1 $1.dst compacted };
add(16)         g62<1>D         g56<1,1,0>D     44D             { align1 1H compacted };
add(16)         g68<1>D         g56<1,1,0>D     1196D           { align1 1H $0.src compacted };
add(16)         g60<1>D         -g58<1,1,0>D    g50<1,1,0>D     { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
cmp.l.f0.0(16)  g64<1>UD        g62<1,1,0>UD    g56<1,1,0>UD    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
cmp.l.f0.0(16)  g70<1>UD        g68<1,1,0>UD    0x000004acUD    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
add(16)         g66<1>D         -g64<1,1,0>D    g60<1,1,0>D     { align1 1H I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
add(16)         g72<1>D         -g70<1,1,0>D    g60<1,1,0>D     { align1 1H $0.src compacted };

LABEL7:
cmp.ge.f0.0(16) null<1>UD       g32<8,8,1>UD    0x00000030UD    { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL6        UIP:  LABEL6              { align1 1H };
shl(16)         g74<1>D         g32<8,8,1>D     0x00000002UD    { align1 1H $5.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
add(16)         g76<1>D         g68<1,1,0>D     g74<1,1,0>D     { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
cmp.l.f0.0(16)  g78<1>UD        g76<1,1,0>UD    g68<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g82<2>UD        g76<4,4,1>UD                    { align1 1Q $6.src };
mov(8)          g84<2>UD        g77<4,4,1>UD                    { align1 2Q $6.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
add(16)         g80<1>D         -g78<1,1,0>D    g72<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g82.1<2>UD      g80<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g84.1<2>UD      g81<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g82UD           g43UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $7 };
add3(16)        g81<1>D         0x0004UW        g74<8,8,1>D     g68<1,1,1>D { align1 1H $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
cmp.l.f0.0(16)  g83<1>UD        g81<1,1,0>UD    g68<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g87<2>UD        g81<4,4,1>UD                    { align1 1Q };
mov(8)          g89<2>UD        g82<4,4,1>UD                    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
add(16)         g85<1>D         -g83<1,1,0>D    g72<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g87.1<2>UD      g85<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g89.1<2>UD      g86<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g87UD           g43UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $8 };
add3(16)        g86<1>D         0x0008UW        g74<8,8,1>D     g68<1,1,1>D { align1 1H $8.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
cmp.l.f0.0(16)  g88<1>UD        g86<1,1,0>UD    g68<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g92<2>UD        g86<4,4,1>UD                    { align1 1Q };
mov(8)          g94<2>UD        g87<4,4,1>UD                    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
add(16)         g90<1>D         -g88<1,1,0>D    g72<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g92.1<2>UD      g90<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g94.1<2>UD      g91<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g92UD           g43UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $9 };
add3(16)        g91<1>D         0x000cUW        g74<8,8,1>D     g68<1,1,1>D { align1 1H $9.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
cmp.l.f0.0(16)  g93<1>UD        g91<1,1,0>UD    g68<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g97<2>UD        g91<4,4,1>UD                    { align1 1Q $4.src };
mov(8)          g99<2>UD        g92<4,4,1>UD                    { align1 2Q $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
add(16)         g95<1>D         -g93<1,1,0>D    g72<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g97.1<2>UD      g95<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g99.1<2>UD      g96<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g97UD           g43UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $4 };
add(16)         g32<1>D         g32<1,1,0>D     64D             { align1 1H compacted };

LABEL6:
while(16)       JIP:  LABEL7                                    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(16)         g33<1>UD        g37<8,8,1>UD                    { align1 1H I@2 };

LABEL9:
cmp.ge.f0.0(16) null<1>UD       g33<8,8,1>UD    0x00000120UD    { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL8        UIP:  LABEL8              { align1 1H };
shl(16)         g96<1>D         g33<8,8,1>D     0x00000002UD    { align1 1H $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add(16)         g98<1>D         g62<1,1,0>D     g96<1,1,0>D     { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
cmp.l.f0.0(16)  g100<1>UD       g98<1,1,0>UD    g62<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g104<2>UD       g98<4,4,1>UD                    { align1 1Q };
mov(8)          g106<2>UD       g99<4,4,1>UD                    { align1 2Q };
add(16)         g102<1>D        -g100<1,1,0>D   g66<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g104.1<2>UD     g102<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g106.1<2>UD     g103<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g104UD          g52UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $11 };
add3(16)        g103<1>D        0x0004UW        g96<8,8,1>D     g62<1,1,1>D { align1 1H $11.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
cmp.l.f0.0(16)  g105<1>UD       g103<1,1,0>UD   g62<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g109<2>UD       g103<4,4,1>UD                   { align1 1Q };
mov(8)          g111<2>UD       g104<4,4,1>UD                   { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
add(16)         g107<1>D        -g105<1,1,0>D   g66<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g109.1<2>UD     g107<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g111.1<2>UD     g108<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g109UD          g52UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $12 };
add3(16)        g108<1>D        0x0008UW        g96<8,8,1>D     g62<1,1,1>D { align1 1H $12.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
cmp.l.f0.0(16)  g110<1>UD       g108<1,1,0>UD   g62<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g114<2>UD       g108<4,4,1>UD                   { align1 1Q };
mov(8)          g116<2>UD       g109<4,4,1>UD                   { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
add(16)         g112<1>D        -g110<1,1,0>D   g66<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g114.1<2>UD     g112<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g116.1<2>UD     g113<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g114UD          g52UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $13 };
add3(16)        g113<1>D        0x000cUW        g96<8,8,1>D     g62<1,1,1>D { align1 1H $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
cmp.l.f0.0(16)  g115<1>UD       g113<1,1,0>UD   g62<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g119<2>UD       g113<4,4,1>UD                   { align1 1Q $10.src };
mov(8)          g121<2>UD       g114<4,4,1>UD                   { align1 2Q $10.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
add(16)         g117<1>D        -g115<1,1,0>D   g66<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g119.1<2>UD     g117<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g121.1<2>UD     g118<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g119UD          g52UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $10 };
add(16)         g33<1>D         g33<1,1,0>D     64D             { align1 1H compacted };

LABEL8:
while(16)       JIP:  LABEL9                                    { align1 1H };
add(16)         g35<1>D         g35<1,1,0>D     16D             { align1 1H compacted };

LABEL5:
while(16)       JIP:  LABEL10                                   { align1 1H };

LABEL14:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
cmp.ge.f0.0(16) null<1>UD       g41<8,8,1>UD    g26<8,8,1>UD    { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL11       UIP:  LABEL11             { align1 1H };
mul(16)         g118<1>D        g41<1,1,0>D     1484W           { align1 1H $10.src compacted };
mov(16)         g39<1>UD        g37<8,8,1>UD                    { align1 1H I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
add(16)         g120<1>D        g47<1,1,0>D     g118<1,1,0>D    { align1 1H I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
cmp.l.f0.0(16)  g122<1>UD       g120<1,1,0>UD   g47<1,1,0>UD    { align1 1H I@1 compacted };
add(16)         g124<1>D        g120<1,1,0>D    1388D           { align1 1H compacted };
cmp.l.f0.0(16)  g126<1>UD       g124<1,1,0>UD   g120<1,1,0>UD   { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
add3(16)        g1<1>D          -g122<8,8,1>D   g50<8,8,1>D     -g126<1,1,1>D { align1 1H };

LABEL13:
cmp.ge.f0.0(16) null<1>UD       g39<8,8,1>UD    0x00000018UD    { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL12       UIP:  LABEL12             { align1 1H };
shl(16)         g3<1>D          g39<8,8,1>D     0x00000002UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
add(16)         g5<1>D          g124<1,1,0>D    g3<1,1,0>D      { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
cmp.l.f0.0(16)  g7<1>UD         g5<1,1,0>UD     g124<1,1,0>UD   { align1 1H I@1 compacted };
mov(8)          g120<2>UD       g5<4,4,1>UD                     { align1 1Q $10.src };
mov(8)          g122<2>UD       g6<4,4,1>UD                     { align1 2Q $10.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
add(16)         g9<1>D          -g7<1,1,0>D     g1<1,1,0>D      { align1 1H I@3 compacted };
mov(8)          g120.1<2>UD     g9<4,4,1>UD                     { align1 1Q I@1 };
mov(8)          g122.1<2>UD     g10<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g120UD          g52UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $10 };
add3(16)        g10<1>D         0x0004UW        g3<8,8,1>D      g124<1,1,1>D { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
cmp.l.f0.0(16)  g12<1>UD        g10<1,1,0>UD    g124<1,1,0>UD   { align1 1H I@1 compacted };
mov(8)          g5<2>UD         g10<4,4,1>UD                    { align1 1Q };
mov(8)          g7<2>UD         g11<4,4,1>UD                    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
add(16)         g14<1>D         -g12<1,1,0>D    g1<1,1,0>D      { align1 1H I@3 compacted };
mov(8)          g5.1<2>UD       g14<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g7.1<2>UD       g15<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g5UD            g52UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $14 };
add3(16)        g15<1>D         0x0008UW        g3<8,8,1>D      g124<1,1,1>D { align1 1H $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
cmp.l.f0.0(16)  g17<1>UD        g15<1,1,0>UD    g124<1,1,0>UD   { align1 1H A@1 compacted };
mov(8)          g6<2>UD         g15<4,4,1>UD                    { align1 1Q $14.src };
mov(8)          g8<2>UD         g16<4,4,1>UD                    { align1 2Q $14.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
add(16)         g19<1>D         -g17<1,1,0>D    g1<1,1,0>D      { align1 1H I@3 compacted };
mov(8)          g6.1<2>UD       g19<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g8.1<2>UD       g20<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g6UD            g52UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $15 };
add3(16)        g20<1>D         0x000cUW        g3<8,8,1>D      g124<1,1,1>D { align1 1H $5.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
cmp.l.f0.0(16)  g22<1>UD        g20<1,1,0>UD    g124<1,1,0>UD   { align1 1H I@1 compacted };
mov(8)          g7<2>UD         g20<4,4,1>UD                    { align1 1Q $15.src };
mov(8)          g9<2>UD         g21<4,4,1>UD                    { align1 2Q $15.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
add(16)         g24<1>D         -g22<1,1,0>D    g1<1,1,0>D      { align1 1H I@3 compacted };
mov(8)          g7.1<2>UD       g24<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g9.1<2>UD       g25<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g7UD            g52UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $10 };
add(16)         g39<1>D         g39<1,1,0>D     64D             { align1 1H compacted };

LABEL12:
while(16)       JIP:  LABEL13                                   { align1 1H };
add(16)         g41<1>D         g41<1,1,0>D     16D             { align1 1H compacted };

LABEL11:
while(16)       JIP:  LABEL14                                   { align1 1H };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_build_BFS_begin_batchable_code[] = {
    0x80000065, 0x49058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x2b054660, 0x00000000, 0x00000000,
    0xe0290065, 0x0ff10043, 0xe24a1b40, 0x00014903,
    0x80030061, 0x1f054410, 0x00000000, 0x76543210,
    0x00041b69, 0x3c058660, 0x02462905, 0x00000004,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x80004031, 0x01140000, 0xfa004a0c, 0x00340000,
    0x641f1a40, 0x00801f95, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00040061, 0x3e050160,
    0x00461f05, 0x00000000, 0xa0401940, 0x3c003e02,
    0x00041961, 0x42050020, 0x00664007, 0x00000000,
    0x00040061, 0x44050010, 0x00664007, 0x00000000,
    0x00041a61, 0x45050120, 0x00564206, 0x00000000,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x211c0061, 0x001102cc, 0x2a2d0061, 0x001102cc,
    0x00030061, 0x11260aa0, 0x00000264, 0x00000000,
    0x00130061, 0x2f260aa0, 0x00000264, 0x00000000,
    0x211c1461, 0x00110204, 0x2a2d1461, 0x00110204,
    0x21111461, 0x00110244, 0x2a2f1461, 0x00110244,
    0x00031461, 0x31050220, 0x00441c26, 0x00000000,
    0xa1330040, 0x020e1c03, 0x00131361, 0x32050220,
    0x00442d26, 0x00000000, 0xaa340040, 0x020e2d03,
    0x00031b61, 0x20060220, 0x00343305, 0x00000000,
    0xe7351a70, 0x02003303, 0x00130061, 0x22060220,
    0x00343405, 0x00000000, 0x00031a40, 0x13052660,
    0x06463505, 0x00441c26, 0x00131b40, 0x36052660,
    0x06463605, 0x00442d26, 0x00031a61, 0x20260220,
    0x00341305, 0x00000000, 0x00131a61, 0x22260220,
    0x00343605, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044131, 0x37240000,
    0xfb042024, 0x000c0000, 0x00042161, 0x47060210,
    0x00463705, 0x00000000, 0x00041962, 0x3b058110,
    0x51564706, 0x01000100, 0x00041961, 0x1a050120,
    0x00463b05, 0x00000000, 0x00041970, 0x00010660,
    0x56464505, 0x00461a05, 0x01040022, 0x0001c060,
    0x00000628, 0x00000608, 0xa0472140, 0x39004202,
    0xa14c1240, 0x034e1103, 0xaa4d1140, 0x034e2f03,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x00030040, 0x57058660, 0x06441c06, 0x00002c2c,
    0x00130040, 0x58058660, 0x06442d06, 0x00002c2c,
    0x605a0041, 0x5cc04502, 0x00040061, 0x09054660,
    0x00000000, 0x00000000, 0x00040061, 0x0d054660,
    0x00000000, 0x00000000, 0x00040061, 0x0f054660,
    0x00000000, 0x00000000, 0x00041f61, 0x25050120,
    0x00564706, 0x00000000, 0x00040061, 0x1e060210,
    0x00464705, 0x00000000, 0x00031f70, 0x14050220,
    0x52464c05, 0x00441106, 0x00131f70, 0x4e050220,
    0x52464d05, 0x00442f06, 0xa05c1f40, 0x5a005702,
    0x00031d41, 0x20018220, 0x01462505, 0x00580058,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x604a1e41, 0x05802502, 0x00031d40, 0x4f052660,
    0x06461405, 0x00441126, 0x00131d40, 0x50052660,
    0x06464e05, 0x00442f26, 0x00031d61, 0x05060220,
    0x00345c05, 0x00000000, 0x00131e61, 0x07060220,
    0x00345d05, 0x00000000, 0xfe480049, 0x05802503,
    0xa0511e40, 0x4a004c02, 0x00130041, 0x20018220,
    0x01462605, 0x00580058, 0x27531a70, 0x4c005103,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x00031161, 0x01060220, 0x00345105, 0x00000000,
    0x00130061, 0x03060220, 0x00345205, 0x00000000,
    0x00030070, 0x59050220, 0x52465705, 0x00441c06,
    0x00130070, 0x5a050220, 0x52465805, 0x00442d06,
    0x00130049, 0x49058222, 0x02462605, 0x00000058,
    0x275e0070, 0x57005c03, 0x00041a52, 0x55040e68,
    0x0e2e4f05, 0x53054805, 0x00041a52, 0x60042e68,
    0x0e2e5905, 0x5e053105, 0x00031a61, 0x01260220,
    0x00345505, 0x00000000, 0x00131b61, 0x03260220,
    0x00345605, 0x00000000, 0x00031b61, 0x05260220,
    0x00346005, 0x00000000, 0x00131c61, 0x07260220,
    0x00346105, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x0b140000,
    0xfb040124, 0x00040000, 0x80002201, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044331, 0x00000000,
    0xfb0c0524, 0x003c0944, 0xa0620040, 0x01c05c03,
    0x27641970, 0x5c006203, 0x00033161, 0x21060220,
    0x00346205, 0x00000000, 0x00133161, 0x23060220,
    0x00346305, 0x00000000, 0xa0661b40, 0x60026402,
    0x00031961, 0x21260220, 0x00346605, 0x00000000,
    0x00131a61, 0x23260220, 0x00346705, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x00000000, 0xfb0c2124, 0x00042514,
    0x00040069, 0x67058660, 0x02464505, 0x00000002,
    0xa1691940, 0x670e1c02, 0xaa6a1a40, 0x680e2d02,
    0x00031a70, 0x15050220, 0x52466905, 0x00441c06,
    0x00131a70, 0x6b050220, 0x52466a05, 0x00442d06,
    0xa06e0040, 0x02806903, 0x00031b40, 0x6c052660,
    0x06461505, 0x00441c26, 0x00131b40, 0x6d052660,
    0x06466b05, 0x00442d26, 0x27701b70, 0x69006e03,
    0x00030061, 0x33060220, 0x00346e05, 0x00000000,
    0x00130061, 0x35060220, 0x00346f05, 0x00000000,
    0xa0721b40, 0x6c027002, 0x00031961, 0x33260220,
    0x00347205, 0x00000000, 0x00131a61, 0x35260220,
    0x00347305, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044431, 0x00000000,
    0xfb0c3324, 0x00042b14, 0xa0730040, 0x01005c03,
    0x00040061, 0x11054660, 0x00000000, 0x00000000,
    0x00040061, 0x13054660, 0x00000000, 0x00000000,
    0x00040061, 0x15054660, 0x00000000, 0x00000000,
    0x27751c70, 0x5c007303, 0x00030061, 0x40060220,
    0x00347305, 0x00000000, 0x00130061, 0x42060220,
    0x00347405, 0x00000000, 0xa0771b40, 0x60027502,
    0x00031961, 0x40260220, 0x00347705, 0x00000000,
    0x00131a61, 0x42260220, 0x00347805, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044531, 0x00000000, 0xfb0c4024, 0x001c1134,
    0xa0783340, 0x1ff00b03, 0xa07a0040, 0x42c06903,
    0xe0491a68, 0x00907803, 0x277c1a70, 0x69007a03,
    0x00030061, 0x45060220, 0x00347a05, 0x00000000,
    0x00130061, 0x47060220, 0x00347b05, 0x00000000,
    0xa07e1b40, 0x6c027c02, 0x00031961, 0x45260220,
    0x00347e05, 0x00000000, 0x00131a61, 0x47260220,
    0x00347f05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044031, 0x00000000,
    0xfb0c4524, 0x00044914, 0x00033240, 0x01058660,
    0x06441c06, 0x0000082c, 0x00133240, 0x02058660,
    0x06442d06, 0x0000082c, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0xe7031970, 0x82c00103,
    0xa0053340, 0x67000102, 0x00040061, 0x4f060110,
    0x00561e06, 0x00000000, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x27071a70, 0x01000503,
    0x00030061, 0x4b060220, 0x00340505, 0x00000000,
    0x00130061, 0x4d060220, 0x00340605, 0x00000000,
    0x00041c61, 0x4f160110, 0x00464405, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x00041c52, 0x09042e68, 0x0e2e0305, 0x07053105,
    0x00031961, 0x4b260220, 0x00340905, 0x00000000,
    0x00131a61, 0x4d260220, 0x00340a05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044531, 0x00000000, 0xfb0c4b24, 0x00044f14,
    0x00040024, 0x0001c060, 0x00000030, 0x00000030,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00040061, 0x49054220, 0x00000000, 0x00000000,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80041a61, 0x0a054660, 0x00000000, 0x00000000,
    0x00040061, 0x0a050660, 0x00464905, 0x00000000,
    0x00041f70, 0x00018550, 0x15584405, 0x00000000,
    0x80031a40, 0x0a260660, 0x06440a06, 0x00440a26,
    0x80021940, 0x0a470660, 0x06420a27, 0x00420a47,
    0x80021940, 0x0a670660, 0x06420a27, 0x00420a67,
    0x80021940, 0x0a850660, 0x06000a64, 0x00340a85,
    0x80021a40, 0x0b850660, 0x06000b64, 0x00340b85,
    0xa40b1940, 0x0b010a82, 0x01040022, 0x0001c060,
    0x00000248, 0x00000248, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x00042161, 0x0c050120,
    0x00563906, 0x00000000, 0x00043361, 0x0e050120,
    0x00563706, 0x00000000, 0xa11e0040, 0x018e1c03,
    0xaa103340, 0x018e2d03, 0x80000a01, 0x00000000,
    0x00000000, 0x00000000, 0x00043561, 0x12050660,
    0x00000be4, 0x00000000, 0xa0181d40, 0x1a000c02,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0xa0161d40, 0x1a200e02, 0x00031d70, 0x1f050220,
    0x52461e05, 0x00441c06, 0x80103501, 0x00000000,
    0x00000000, 0x00000000, 0x00130a70, 0x11050220,
    0x52461005, 0x00442d06, 0x00033561, 0x50060220,
    0x00341e05, 0x00000000, 0x00130061, 0x52060220,
    0x00341005, 0x00000000, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x00031c40, 0x21052660,
    0x06461f05, 0x00441c26, 0xa71f3170, 0x0e001a02,
    0x00131d40, 0x1e052660, 0x06461105, 0x00442d26,
    0x00031b61, 0x50260220, 0x00342105, 0x00000000,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x00041b61, 0x14052660, 0x00461f05, 0x00000000,
    0x00131b61, 0x52260220, 0x00341e05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044531, 0x00000000, 0xfb0c5024, 0x003c1244,
    0xa1223140, 0x428e1c03, 0xaa200040, 0x428e2d03,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x00031a70, 0x23050220, 0x52462205, 0x00441c06,
    0x00131a70, 0x21050220, 0x52462005, 0x00442d06,
    0x00033561, 0x51060220, 0x00342205, 0x00000000,
    0x00133561, 0x53060220, 0x00342005, 0x00000000,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x00031c40, 0x24052660, 0x06462305, 0x00441c26,
    0x00131c40, 0x22052660, 0x06462105, 0x00442d26,
    0x00031a61, 0x51260220, 0x00342405, 0x00000000,
    0x00131a61, 0x53260220, 0x00342205, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044631, 0x00000000, 0xfb0c5124, 0x00041a14,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x00031140, 0x2f058660, 0x06441c06, 0x00002c2c,
    0x00131140, 0x30058660, 0x06442d06, 0x00002c2c,
    0x00043169, 0x25058660, 0x02463e05, 0x00000002,
    0x00043461, 0x34054660, 0x00000000, 0x7f800000,
    0x00043161, 0x23050220, 0x00462905, 0x00000000,
    0x00031d70, 0x27050220, 0x52462f05, 0x00441c06,
    0x00131d70, 0x31050220, 0x52463005, 0x00442d06,
    0x00031a40, 0x32052660, 0x06462705, 0x00441c26,
    0x80103401, 0x00000000, 0x00000000, 0x00000000,
    0x00131a40, 0x33052660, 0x06463105, 0x00442d26,
    0x80003601, 0x00000000, 0x00000000, 0x00000000,
    0x00041a70, 0x00010220, 0x42462305, 0x00461a05,
    0x01040028, 0x0001c660, 0x00000708, 0x00000708,
    0x60363441, 0x5cc02302, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x00041e61, 0x20050220,
    0x00462505, 0x00000000, 0xa038a140, 0x36002f02,
    0x273a9170, 0x2f003803, 0xa03e0040, 0x02c03803,
    0xa0443040, 0x4ac03803, 0xa03c1b40, 0x32023a02,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x27401b70, 0x38003e03, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0xe7461b70, 0x4ac04403,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0xa0421a40, 0x3c024002, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0xa0483040, 0x3c024602,
    0x00041a70, 0x00018220, 0x42462005, 0x00000030,
    0x01040028, 0x0001c660, 0x00000300, 0x00000300,
    0x00043569, 0x4a058660, 0x02462005, 0x00000002,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0xa04c1940, 0x4a004402, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x274e1970, 0x44004c03,
    0x00033661, 0x52060220, 0x00344c05, 0x00000000,
    0x00133661, 0x54060220, 0x00344d05, 0x00000000,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x80003601, 0x00000000, 0x00000000, 0x00000000,
    0xa0501b40, 0x48024e02, 0x00031961, 0x52260220,
    0x00345005, 0x00000000, 0x00131a61, 0x54260220,
    0x00345105, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044731, 0x00000000,
    0xfb0c5224, 0x00042b14, 0x00043752, 0x51044160,
    0x0e0e0004, 0x44054a05, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0x27531970, 0x44005103,
    0x00030061, 0x57060220, 0x00345105, 0x00000000,
    0x00130061, 0x59060220, 0x00345205, 0x00000000,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0xa0551b40, 0x48025302, 0x00031961, 0x57260220,
    0x00345505, 0x00000000, 0x00131a61, 0x59260220,
    0x00345605, 0x00000000, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044831, 0x00000000,
    0xfb0c5724, 0x00042b14, 0x00043852, 0x56044160,
    0x0e0e0008, 0x44054a05, 0x80003801, 0x00000000,
    0x00000000, 0x00000000, 0x27581970, 0x44005603,
    0x00030061, 0x5c060220, 0x00345605, 0x00000000,
    0x00130061, 0x5e060220, 0x00345705, 0x00000000,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0xa05a1b40, 0x48025802, 0x00031961, 0x5c260220,
    0x00345a05, 0x00000000, 0x00131a61, 0x5e260220,
    0x00345b05, 0x00000000, 0x80003801, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044931, 0x00000000,
    0xfb0c5c24, 0x00042b14, 0x00043952, 0x5b044160,
    0x0e0e000c, 0x44054a05, 0x80003901, 0x00000000,
    0x00000000, 0x00000000, 0x275d1970, 0x44005b03,
    0x00033461, 0x61060220, 0x00345b05, 0x00000000,
    0x00133461, 0x63060220, 0x00345c05, 0x00000000,
    0x80003901, 0x00000000, 0x00000000, 0x00000000,
    0xa05f1b40, 0x48025d02, 0x00031961, 0x61260220,
    0x00345f05, 0x00000000, 0x00131a61, 0x63260220,
    0x00346005, 0x00000000, 0x80003901, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044431, 0x00000000,
    0xfb0c6124, 0x00042b14, 0xa0200040, 0x04002003,
    0x00040027, 0x00014060, 0x00000000, 0xfffffcf0,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x00041a61, 0x21050220, 0x00462505, 0x00000000,
    0x00041970, 0x00018220, 0x42462105, 0x00000120,
    0x01040028, 0x0001c660, 0x000002e0, 0x000002e0,
    0x00043469, 0x60058660, 0x02462105, 0x00000002,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0xa0621940, 0x60003e02, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x27641970, 0x3e006203,
    0x00030061, 0x68060220, 0x00346205, 0x00000000,
    0x00130061, 0x6a060220, 0x00346305, 0x00000000,
    0xa0661b40, 0x42026402, 0x00031961, 0x68260220,
    0x00346605, 0x00000000, 0x00131a61, 0x6a260220,
    0x00346705, 0x00000000, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044b31, 0x00000000,
    0xfb0c6824, 0x00043414, 0x00043b52, 0x67044160,
    0x0e0e0004, 0x3e056005, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x27691970, 0x3e006703,
    0x00030061, 0x6d060220, 0x00346705, 0x00000000,
    0x00130061, 0x6f060220, 0x00346805, 0x00000000,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0xa06b1b40, 0x42026902, 0x00031961, 0x6d260220,
    0x00346b05, 0x00000000, 0x00131a61, 0x6f260220,
    0x00346c05, 0x00000000, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044c31, 0x00000000,
    0xfb0c6d24, 0x00043414, 0x00043c52, 0x6c044160,
    0x0e0e0008, 0x3e056005, 0x80003c01, 0x00000000,
    0x00000000, 0x00000000, 0x276e1970, 0x3e006c03,
    0x00030061, 0x72060220, 0x00346c05, 0x00000000,
    0x00130061, 0x74060220, 0x00346d05, 0x00000000,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0xa0701b40, 0x42026e02, 0x00031961, 0x72260220,
    0x00347005, 0x00000000, 0x00131a61, 0x74260220,
    0x00347105, 0x00000000, 0x80003c01, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044d31, 0x00000000,
    0xfb0c7224, 0x00043414, 0x00043d52, 0x71044160,
    0x0e0e000c, 0x3e056005, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x27731970, 0x3e007103,
    0x00033a61, 0x77060220, 0x00347105, 0x00000000,
    0x00133a61, 0x79060220, 0x00347205, 0x00000000,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0xa0751b40, 0x42027302, 0x00031961, 0x77260220,
    0x00347505, 0x00000000, 0x00131a61, 0x79260220,
    0x00347605, 0x00000000, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044a31, 0x00000000,
    0xfb0c7724, 0x00043414, 0xa0210040, 0x04002103,
    0x00040027, 0x00014060, 0x00000000, 0xfffffd10,
    0xa0230040, 0x01002303, 0x00040027, 0x00014060,
    0x00000000, 0xfffff8d8, 0x80003601, 0x00000000,
    0x00000000, 0x00000000, 0x00041a70, 0x00010220,
    0x42462905, 0x00461a05, 0x01040028, 0x0001c660,
    0x000003b0, 0x000003b0, 0x60763a41, 0x5cc02902,
    0x00041d61, 0x27050220, 0x00462505, 0x00000000,
    0x80003a01, 0x00000000, 0x00000000, 0x00000000,
    0xa0781a40, 0x76002f02, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x277a1970, 0x2f007803,
    0xa07c0040, 0x56c07803, 0x277e1970, 0x78007c03,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00040052, 0x01042e68, 0x0e2e7a05, 0x7e053205,
    0x00041a70, 0x00018220, 0x42462705, 0x00000018,
    0x01040028, 0x0001c660, 0x000002f0, 0x000002f0,
    0x00040069, 0x03058660, 0x02462705, 0x00000002,
    0x80003e01, 0x00000000, 0x00000000, 0x00000000,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0xa0051940, 0x03007c02, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x27071970, 0x7c000503,
    0x00033a61, 0x78060220, 0x00340505, 0x00000000,
    0x00133a61, 0x7a060220, 0x00340605, 0x00000000,
    0x80003a01, 0x00000000, 0x00000000, 0x00000000,
    0xa0091b40, 0x01020702, 0x00031961, 0x78260220,
    0x00340905, 0x00000000, 0x00131a61, 0x7a260220,
    0x00340a05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044a31, 0x00000000,
    0xfb0c7824, 0x00043414, 0x00040052, 0x0a044160,
    0x0e0e0004, 0x7c050305, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x270c1970, 0x7c000a03,
    0x00030061, 0x05060220, 0x00340a05, 0x00000000,
    0x00130061, 0x07060220, 0x00340b05, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0xa00e1b40, 0x01020c02, 0x00031961, 0x05260220,
    0x00340e05, 0x00000000, 0x00131a61, 0x07260220,
    0x00340f05, 0x00000000, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044e31, 0x00000000,
    0xfb0c0524, 0x00043414, 0x00043352, 0x0f044160,
    0x0e0e0008, 0x7c050305, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x27110970, 0x7c000f03,
    0x00033e61, 0x06060220, 0x00340f05, 0x00000000,
    0x00133e61, 0x08060220, 0x00341005, 0x00000000,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0xa0131b40, 0x01021102, 0x00031961, 0x06260220,
    0x00341305, 0x00000000, 0x00131a61, 0x08260220,
    0x00341405, 0x00000000, 0x80003e01, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044f31, 0x00000000,
    0xfb0c0624, 0x00043414, 0x00043552, 0x14044160,
    0x0e0e000c, 0x7c050305, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x27161970, 0x7c001403,
    0x00033f61, 0x07060220, 0x00341405, 0x00000000,
    0x00133f61, 0x09060220, 0x00341505, 0x00000000,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0xa0181b40, 0x01021602, 0x00031961, 0x07260220,
    0x00341805, 0x00000000, 0x00131a61, 0x09260220,
    0x00341905, 0x00000000, 0x80003f01, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044a31, 0x00000000,
    0xfb0c0724, 0x00043414, 0xa0270040, 0x04002703,
    0x00040027, 0x00014060, 0x00000000, 0xfffffd00,
    0xa0290040, 0x01002903, 0x00040027, 0x00014060,
    0x00000000, 0xfffffc30, 0x80030061, 0x7e050220,
    0x00460005, 0x00000000, 0x80040931, 0x00000004,
    0x30007e0c, 0x00000000, 0x20000060, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_build_BFS_begin_batchable = {
   .prog_data = {
      .base.nr_params = 12,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 5936,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_build_BFS_begin_batchable_relocs,
      .base.uses_atomic_load_store = false,
      .local_size = { 256, 1, 1 },
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
   .args = gfx125_bvh_build_BFS_begin_batchable_args,
   .code = gfx125_bvh_build_BFS_begin_batchable_code,
};
const char *gfx125_bvh_build_BFS_begin_batchable_sha1 = "e08d39f5bbbd1b4fb1f53da77160ac0ace30486f";
