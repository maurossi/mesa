#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_build_leaf_primref_to_procedurals_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_build_leaf_primref_to_procedurals_args[] = {
   { 0, 8 },
   { 8, 8 },
   { 16, 8 },
   { 24, 8 },
   { 32, 8 },
   { 40, 4 },
   { 44, 4 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g83<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g115<1>UD       g0.1<0,1,0>UD                   { align1 1H };
add(1)          g84<1>UD        g83<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@2 compacted };
add(1)          g85<1>UD        g83<0,1,0>UD    0x00000040UD    { align1 WE_all 1N compacted };
mov(8)          g89<1>UW        0x76543210V                     { align1 WE_all 1Q };
shl(16)         g92<1>D         g115<8,8,1>D    0x00000004UD    { align1 1H I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
send(1)         g1UD            g84UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(1)         g3UD            g85UD           nullUD          0x0210c500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V8, transpose, L1STATE_L3MOCS dst_len = 1, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
add(8)          g89.8<1>UW      g89<1,1,0>UW    0x0008UW        { align1 WE_all 1Q I@2 compacted };
add(16)         g109<1>D        g92<1,1,0>D     16D             { align1 1H I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g98.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g100.1<2>F      g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g33.1<2>F       g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g35.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g37.1<2>F       g2.5<0,1,0>F                    { align1 1Q };
mov(8)          g39.1<2>F       g2.5<0,1,0>F                    { align1 2Q };
mov(8)          g41.1<2>F       g2.7<0,1,0>F                    { align1 1Q };
mov(8)          g43.1<2>F       g2.7<0,1,0>F                    { align1 2Q };
mov(8)          g98<2>F         g2<0,1,0>F                      { align1 1Q F@7 compacted };
mov(8)          g100<2>F        g2<0,1,0>F                      { align1 2Q F@7 compacted };
mov(8)          g33<2>F         g2.2<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g35<2>F         g2.2<0,1,0>F                    { align1 2Q F@7 compacted };
mov(8)          g37<2>F         g2.4<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g39<2>F         g2.4<0,1,0>F                    { align1 2Q F@7 compacted };
mov(8)          g41<2>F         g2.6<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g43<2>F         g2.6<0,1,0>F                    { align1 2Q F@7 compacted };
add(8)          g102<1>D        g98<8,4,2>D     36D             { align1 1Q F@7 compacted };
add(8)          g103<1>D        g100<8,4,2>D    36D             { align1 2Q F@7 compacted };
cmp.l.f0.0(8)   g104<1>UD       g102<8,8,1>UD   g98<8,4,2>UD    { align1 1Q I@2 };
mov(8)          g117<2>UD       g102<4,4,1>UD                   { align1 1Q };
cmp.l.f0.0(8)   g105<1>UD       g103<8,8,1>UD   g100<8,4,2>UD   { align1 2Q I@3 };
mov(8)          g119<2>UD       g103<4,4,1>UD                   { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
cmp.z.f0.0(16)  null<1>D        g3.2<0,1,0>D    8D              { align1 1H };
add(8)          g106<1>D        -g104<8,8,1>D   g98.1<8,4,2>D   { align1 1Q I@5 };
add(8)          g107<1>D        -g105<8,8,1>D   g100.1<8,4,2>D  { align1 2Q I@4 };
mov(8)          g117.1<2>UD     g106<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g119.1<2>UD     g107<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g107UD          g117UD          nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
sel.l(16)       g25<1>UD        g109<1,1,0>UD   g107<1,1,0>UD   { align1 1H $1.dst compacted };
mul(16)         g73<1>D         g3.2<0,1,0>D    g107.1<16,8,2>UW { align1 1H };
mul(16)         g110<1>D        g3.2<0,1,0>D    g107<16,8,2>UW  { align1 1H };
add(16)         g110.1<2>UW     g110.1<16,8,2>UW g73<16,8,2>UW  { align1 1H I@1 };
(-f0.0) sel(16) g45<1>UD        g110<8,8,1>UD   0x00000004UD    { align1 1H I@1 };
cmp.nz.f0.0(16) null<1>D        g92<8,8,1>D     0D              { align1 1H };
(+f0.0) if(16)  JIP:  LABEL1          UIP:  LABEL0              { align1 1H };
add(16)         g111<1>D        g92<1,1,0>D     -1D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(8)          g119<1>UD       g37.1<8,4,2>UD                  { align1 1Q F@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $1.src };
mov(8)          g120<1>UD       g39.1<8,4,2>UD                  { align1 2Q F@3 };
mul(16)         g113<1>D        g111<8,8,1>D    g3.4<0,1,0>UW   { align1 1H I@3 };
mul(16)         g74<1>D         g111<8,8,1>D    g3.5<0,1,0>UW   { align1 1H };
add(16)         g113.1<2>UW     g113.1<16,8,2>UW g74<16,8,2>UW  { align1 1H I@1 };
add(16)         g115<1>D        g113<1,1,0>D    g45<1,1,0>D     { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
cmp.l.f0.0(16)  g117<1>UD       g115<1,1,0>UD   g113<1,1,0>UD   { align1 1H I@1 compacted };
add(8)          g121<1>D        g37<8,4,2>D     g115<1,1,0>D    { align1 1Q compacted };
add(8)          g122<1>D        g39<8,4,2>D     g116<1,1,0>D    { align1 2Q compacted };
cmp.l.f0.0(8)   g123<1>UD       g121<8,8,1>UD   g37<8,4,2>UD    { align1 1Q I@2 };
mov(8)          g4<2>UD         g121<4,4,1>UD                   { align1 1Q };
cmp.l.f0.0(8)   g124<1>UD       g122<8,8,1>UD   g39<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g6<2>UD         g122<4,4,1>UD                   { align1 2Q };
add3(16)        g125<1>D        g119<8,8,1>D    -g117<8,8,1>D   -g123<1,1,1>D { align1 1H I@2 };
mov(8)          g4.1<2>UD       g125<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g6.1<2>UD       g126<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g90UD           g4UD            nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $0 };
else(16)        JIP:  LABEL0          UIP:  LABEL0              { align1 1H };

LABEL1:
mov(16)         g90<1>UD        0xffffffffUD                    { align1 1H $0.dst };

LABEL0:
endif(16)       JIP:  LABEL2                                    { align1 1H };

LABEL2:
mov(16)         g47<1>D         1D                              { align1 1H };
mov(8)          g49<1>UD        g37.1<8,4,2>UD                  { align1 1Q A@4 };
mov(8)          g50<1>UD        g39.1<8,4,2>UD                  { align1 2Q A@3 };

LABEL4:
cmp.ge.f0.0(16) null<1>UD       g92<8,8,1>UD    g107<8,8,1>UD   { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL3        UIP:  LABEL3              { align1 1H };
mul(16)         g126<1>D        g92<8,8,1>D     g3.4<0,1,0>UW   { align1 1H };
mul(16)         g75<1>D         g92<8,8,1>D     g3.5<0,1,0>UW   { align1 1H };
add(16)         g126.1<2>UW     g126.1<16,8,2>UW g75<16,8,2>UW  { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
add(16)         g1<1>D          g126<1,1,0>D    g45<1,1,0>D     { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
cmp.l.f0.0(16)  g4<1>UD         g1<1,1,0>UD     g126<1,1,0>UD   { align1 1H I@1 compacted };
add(8)          g6<1>D          g37<8,4,2>D     g1<1,1,0>D      { align1 1Q $0.src compacted };
add(8)          g7<1>D          g39<8,4,2>D     g2<1,1,0>D      { align1 2Q $0.src compacted };
cmp.l.f0.0(8)   g8<1>UD         g6<8,8,1>UD     g37<8,4,2>UD    { align1 1Q I@2 };
mov(8)          g13<2>UD        g6<4,4,1>UD                     { align1 1Q $2.src };
cmp.l.f0.0(8)   g9<1>UD         g7<8,8,1>UD     g39<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g15<2>UD        g7<4,4,1>UD                     { align1 2Q $2.src };
add3(16)        g10<1>D         g49<8,8,1>D     -g4<8,8,1>D     -g8<1,1,1>D { align1 1H I@2 };
mov(8)          g13.1<2>UD      g10<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g15.1<2>UD      g11<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g11UD           g13UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
cmp.nz.f0.0(16) null<1>D        g11<8,8,1>D     g90<8,8,1>D     { align1 1H $2.dst };
(+f0.0) break(16) JIP:  LABEL3        UIP:  LABEL3              { align1 1H };
add(16)         g92<1>D         g92<1,1,0>D     1D              { align1 1H compacted };

LABEL3:
while(16)       JIP:  LABEL4                                    { align1 1H };
add(16)         g12<1>D         g25<1,1,0>D     -1D             { align1 1H $2.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mul(16)         g14<1>D         g12<8,8,1>D     g3.4<0,1,0>UW   { align1 1H I@1 };
mul(16)         g76<1>D         g12<8,8,1>D     g3.5<0,1,0>UW   { align1 1H };
add(16)         g14.1<2>UW      g14.1<16,8,2>UW g76<16,8,2>UW   { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g16<1>D         g14<1,1,0>D     g45<1,1,0>D     { align1 1H I@1 compacted };
cmp.l.f0.0(16)  g18<1>UD        g16<1,1,0>UD    g14<1,1,0>UD    { align1 1H I@1 compacted };
add(8)          g20<1>D         g37<8,4,2>D     g16<1,1,0>D     { align1 1Q compacted };
add(8)          g21<1>D         g39<8,4,2>D     g17<1,1,0>D     { align1 2Q compacted };
cmp.l.f0.0(8)   g22<1>UD        g20<8,8,1>UD    g37<8,4,2>UD    { align1 1Q I@2 };
mov(8)          g51<2>UD        g20<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g23<1>UD        g21<8,8,1>UD    g39<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g53<2>UD        g21<4,4,1>UD                    { align1 2Q };
add3(16)        g27<1>D         g49<8,8,1>D     -g18<8,8,1>D    -g22<1,1,1>D { align1 1H I@2 };
mov(8)          g51.1<2>UD      g27<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g53.1<2>UD      g28<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g28UD           g51UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };

LABEL6:
cmp.ge.f0.0(16) null<1>UD       g25<8,8,1>UD    g107<8,8,1>UD   { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL5        UIP:  LABEL5              { align1 1H };
mul(16)         g30<1>D         g25<8,8,1>D     g3.4<0,1,0>UW   { align1 1H };
mul(16)         g77<1>D         g25<8,8,1>D     g3.5<0,1,0>UW   { align1 1H };
add(16)         g30.1<2>UW      g30.1<16,8,2>UW g77<16,8,2>UW   { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
add(16)         g51<1>D         g30<1,1,0>D     g45<1,1,0>D     { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
cmp.l.f0.0(16)  g53<1>UD        g51<1,1,0>UD    g30<1,1,0>UD    { align1 1H I@1 compacted };
add(8)          g55<1>D         g37<8,4,2>D     g51<1,1,0>D     { align1 1Q compacted };
add(8)          g56<1>D         g39<8,4,2>D     g52<1,1,0>D     { align1 2Q compacted };
cmp.l.f0.0(8)   g57<1>UD        g55<8,8,1>UD    g37<8,4,2>UD    { align1 1Q I@2 };
mov(8)          g62<2>UD        g55<4,4,1>UD                    { align1 1Q $3.src };
cmp.l.f0.0(8)   g58<1>UD        g56<8,8,1>UD    g39<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g64<2>UD        g56<4,4,1>UD                    { align1 2Q $3.src };
add3(16)        g59<1>D         g49<8,8,1>D     -g53<8,8,1>D    -g57<1,1,1>D { align1 1H I@2 };
mov(8)          g62.1<2>UD      g59<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g64.1<2>UD      g60<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g60UD           g62UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
cmp.nz.f0.0(16) null<1>D        g60<8,8,1>D     g28<8,8,1>D     { align1 1H $3.dst };
(+f0.0) break(16) JIP:  LABEL5        UIP:  LABEL5              { align1 1H };
add(16)         g25<1>D         g25<1,1,0>D     1D              { align1 1H compacted };

LABEL5:
while(16)       JIP:  LABEL6                                    { align1 1H };
mov(16)         g51<1>D         g89<8,8,1>UW                    { align1 1H $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
and(16)         g53<1>UD        g51<1,1,0>UD    0x0000000fUD    { align1 1H I@1 compacted };
mov(16)         g88<2>B         3W                              { align1 1H };
add(16)         g31<1>D         g92<1,1,0>D     g53<1,1,0>D     { align1 1H I@2 compacted };

LABEL18:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
add(16)         g61<1>D         g25<1,1,0>D     g53<1,1,0>D     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
cmp.l.f0.0(16)  g63<1>UD        g61<1,1,0>UD    g25<1,1,0>UD    { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
cmp.l.f0.0(16)  g67<1>UD        g31<1,1,0>UD    g61<1,1,0>UD    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(16)         g65<1>D         -g63<8,8,1>D                    { align1 1H I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
and(16)         g69<1>UD        ~g63<8,8,1>D    g67<8,8,1>UD    { align1 1H I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
cmp.g.f0.0(16)  g71<1>UD        g65<1,1,0>UD    0x00000000UD    { align1 1H I@2 compacted };
or.nz.f0.0(16)  null<1>UD       g71<8,8,1>UD    g69<8,8,1>UD    { align1 1H I@1 };
(-f0.0) break(16) JIP:  LABEL7        UIP:  LABEL7              { align1 1H };
mul(16)         g73<1>D         g31<8,8,1>D     g3.4<0,1,0>UW   { align1 1H $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mul(16)         g78<1>D         g31<8,8,1>D     g3.5<0,1,0>UW   { align1 1H };
add(16)         g73.1<2>UW      g73.1<16,8,2>UW g78<16,8,2>UW   { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(16)         g75<1>D         g73<1,1,0>D     g3.3<0,1,0>D    { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(16)         g84<1>D         g73<1,1,0>D     g45<1,1,0>D     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
cmp.l.f0.0(16)  g77<1>UD        g75<1,1,0>UD    g73<1,1,0>UD    { align1 1H I@2 compacted };
add(8)          g79<1>D         g37<8,4,2>D     g75<1,1,0>D     { align1 1Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
add(8)          g80<1>D         g39<8,4,2>D     g76<1,1,0>D     { align1 2Q compacted };
add(8)          g89<1>D         g37<8,4,2>D     g84<1,1,0>D     { align1 1Q I@4 compacted };
add(8)          g90<1>D         g39<8,4,2>D     g85<1,1,0>D     { align1 2Q @5 $0.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
cmp.l.f0.0(8)   g81<1>UD        g79<8,8,1>UD    g37<8,4,2>UD    { align1 1Q I@4 };
mov(8)          g65<2>UD        g79<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g82<1>UD        g80<8,8,1>UD    g39<8,4,2>UD    { align1 2Q I@5 };
mov(8)          g67<2>UD        g80<4,4,1>UD                    { align1 2Q };
mov(8)          g69<2>UD        g89<4,4,1>UD                    { align1 1Q I@6 };
cmp.l.f0.0(16)  g86<1>UD        g84<1,1,0>UD    g73<1,1,0>UD    { align1 1H $0.src compacted };
mov(8)          g71<2>UD        g90<4,4,1>UD                    { align1 2Q I@7 };
cmp.l.f0.0(8)   g91<1>UD        g89<8,8,1>UD    g37<8,4,2>UD    { align1 1Q $0.dst };
cmp.l.f0.0(8)   g92<1>UD        g90<8,8,1>UD    g39<8,4,2>UD    { align1 2Q };
cmp.l.f0.0(16)  g95<1>UD        g31<1,1,0>UD    g25<1,1,0>UD    { align1 1H compacted };
add3(16)        g83<1>D         g49<8,8,1>D     -g77<8,8,1>D    -g81<1,1,1>D { align1 1H I@7 };
add3(16)        g93<1>D         g49<8,8,1>D     -g86<8,8,1>D    -g91<1,1,1>D { align1 1H I@3 };
mov(8)          g65.1<2>UD      g83<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g67.1<2>UD      g84<4,4,1>UD                    { align1 2Q I@3 };
mov(8)          g69.1<2>UD      g93<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g71.1<2>UD      g94<4,4,1>UD                    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g29UD           g65UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g55UD           g69UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $0 };
cmp.z.f0.0(16)  g97<1>D         g55<1,1,0>D     g55<0,1,0>D     { align1 1H $0.dst compacted };
and.nz.f0.0(16) null<1>UD       g95<8,8,1>UD    g97<8,8,1>UD    { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL9          UIP:  LABEL8              { align1 1H };
mov(16)         g98<1>D         0D                              { align1 WE_all 1H };
mov(16)         g98<1>D         g47<8,8,1>D                     { align1 1H };
add(8)          g98.1<2>D       g98<8,4,2>D     g98.1<8,4,2>D   { align1 WE_all 1Q I@1 };
add(4)          g98.2<4>D       g98.1<8,2,4>D   g98.2<8,2,4>D   { align1 WE_all 1N I@1 };
add(4)          g98.3<4>D       g98.1<8,2,4>D   g98.3<8,2,4>D   { align1 WE_all 1N I@1 };
add(4)          g98.4<1>D       g98.3<0,1,0>D   g98.4<4,4,1>D   { align1 WE_all 1N I@1 };
add(4)          g99.4<1>D       g99.3<0,1,0>D   g99.4<4,4,1>D   { align1 WE_all 1N I@2 };
add(8)          g99<1>D         g98.7<0,1,0>D   g99<1,1,0>D     { align1 WE_all 1Q I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         g27<1>UD        g99.7<0,1,0>UD                  { align1 1H $3.dst };
else(16)        JIP:  LABEL8          UIP:  LABEL8              { align1 1H };

LABEL9:
mov(16)         g27<1>UD        0x00000000UD                    { align1 1H @2 $3.dst };

LABEL8:
endif(16)       JIP:  LABEL7                                    { align1 1H };
cmp.l.f0.0(16)  null<1>UD       g53<8,8,1>UD    0x00000008UD    { align1 1H };
(+f0.0) if(16)  JIP:  LABEL10         UIP:  LABEL10             { align1 1H };
cmp.l.f0.0(16)  null<1>UD       g53<8,8,1>UD    g27<0,1,0>UD    { align1 1H I@4 };
(+f0.0) if(16)  JIP:  LABEL12         UIP:  LABEL11             { align1 1H };
shl(16)         g99<1>D         g29<8,8,1>D     0x00000005UD    { align1 1H $3.dst };
shr(16)         g101<1>UD       g29<1,1,0>UD    0x0000001bUD    { align1 1H compacted };
mov(8)          g103<1>UD       g33.1<8,4,2>UD                  { align1 1Q F@6 };
mov(8)          g104<1>UD       g35.1<8,4,2>UD                  { align1 2Q F@5 };
add(8)          g105<1>D        g33<8,4,2>D     g99<1,1,0>D     { align1 1Q I@4 compacted };
add(8)          g106<1>D        g35<8,4,2>D     g100<1,1,0>D    { align1 2Q I@5 compacted };
cmp.l.f0.0(8)   g107<1>UD       g105<8,8,1>UD   g33<8,4,2>UD    { align1 1Q I@2 };
mov(8)          g70<2>UD        g105<4,4,1>UD                   { align1 1Q $0.src };
cmp.l.f0.0(8)   g108<1>UD       g106<8,8,1>UD   g35<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g72<2>UD        g106<4,4,1>UD                   { align1 2Q $0.src };
add(16)         g111<1>D        g105<1,1,0>D    16D             { align1 1H compacted };
add3(16)        g109<1>D        g103<8,8,1>D    g101<8,8,1>D    -g107<1,1,1>D { align1 1H I@3 };
cmp.l.f0.0(16)  g113<1>UD       g111<1,1,0>UD   g105<1,1,0>UD   { align1 1H I@2 compacted };
mov(8)          g74<2>UD        g111<4,4,1>UD                   { align1 1Q };
mov(8)          g76<2>UD        g112<4,4,1>UD                   { align1 2Q };
mov(8)          g70.1<2>UD      g109<4,4,1>UD                   { align1 1Q I@4 };
mov(8)          g72.1<2>UD      g110<4,4,1>UD                   { align1 2Q I@5 };
add(16)         g115<1>D        -g113<1,1,0>D   g109<1,1,0>D    { align1 1H I@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g4UD            g70UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $0 };
mov(8)          g74.1<2>UD      g115<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g76.1<2>UD      g116<4,4,1>UD                   { align1 2Q I@2 };
and(16)         g97<1>UD        g10<8,8,1>UD    0x07ffffffUD    { align1 1H $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g5UD            g74UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $0 };
shr(16)         g95<1>UD        g11<1,1,0>UD    0x0000001dUD    { align1 1H $0.dst compacted };
else(16)        JIP:  LABEL11         UIP:  LABEL11             { align1 1H };

LABEL12:
mov(16)         g29<1>UD        0xffffffffUD                    { align1 1H $3.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
mov(16)         g97<1>UD        0xffffffffUD                    { align1 1H };
mov(16)         g95<1>UD        0x00000000UD                    { align1 1H I@4 };

LABEL11:
endif(16)       JIP:  LABEL10                                   { align1 1H };
add(16)         g116<1>D        g51<1,1,0>D     15D             { align1 1H $1.src compacted };
add(16)         g118<1>D        g51<1,1,0>D     -1D             { align1 1H $1.src compacted };
add(16)         g124<1>D        g51<1,1,0>D     1D              { align1 1H compacted };
cmp.l.f0.0(16)  null<1>D        g116<8,8,1>D    16D             { align1 1H I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(16)         a0<1>UW         0x0c20UW                        { align1 WE_all 1H I@3 };
shl(16)         a0<1>UW         g118<16,8,2>W   0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0c20UW        { align1 1H A@1 };
mov(16)         g120<1>UD       g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0c20UW                        { align1 WE_all 1H I@3 };
shl(16)         a0<1>UW         g124<16,8,2>W   0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0c20UW        { align1 1H A@1 };
mov(16)         g126<1>UD       g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
(-f0.0) sel(16) g122<1>UD       g120<8,8,1>UD   0xffffffffUD    { align1 1H I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
cmp.l.f0.0(16)  g1<1>D          g124<1,1,0>D    16D             { align1 1H F@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0) sel(16) g4<1>UD         g126<8,8,1>UD   0xffffffffUD    { align1 1H $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
cmp.nz.f0.0(16) g6<1>D          g122<1,1,0>D    g97<1,1,0>D     { align1 1H I@3 compacted };
cmp.nz.f0.0(16) null<1>D        g1<8,8,1>D      0D              { align1 1H I@3 };
mov(16)         g8<1>D          -g6<8,8,1>D                     { align1 1H @2 $0.dst };
mov(16)         a0<1>UW         0x0100UW                        { align1 WE_all 1H @1 $0.dst };
shl(16)         a0<1>UW         g124<16,8,2>W   0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0100UW        { align1 1H A@1 };
mov(16)         g10<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
(+f0.0) sel(16) g57<1>UD        g10<1,1,0>UD    0x00000000UD    { align1 1H I@1 compacted };
cmp.nz.f0.0(16) null<1>D        g6<8,8,1>D      0D              { align1 1H };
(+f0.0) sel(16) g11<1>UD        g53<1,1,0>UD    0x00000000UD    { align1 1H compacted };
mov(8)          g59<2>UD        g11<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g61<2>UD        g12<4,4,1>UD                    { align1 2Q I@2 };
mov(8)          g59.1<2>UD      0x00000000UD                    { align1 1Q I@2 };
mov(8)          g61.1<2>UD      0x00000000UD                    { align1 2Q I@2 };
mov(8)          g79<2>UD        0x00000000UD                    { align1 WE_all 1Q I@2 };
mov(8)          g79<2>UD        g59<8,4,2>UD                    { align1 1Q };
mov(8)          g81<2>UD        0x00000000UD                    { align1 WE_all 2Q I@2 };
mov(8)          g81<2>UD        g61<8,4,2>UD                    { align1 2Q };
mov(8)          g13<2>UD        g79<8,4,2>UD                    { align1 WE_all 1Q I@2 };
mov(8)          g79.1<2>UD      0x00000000UD                    { align1 WE_all 1Q };
mov(8)          g79.1<2>UD      g59.1<8,4,2>UD                  { align1 1Q };
mov(8)          g15<2>UD        g81<8,4,2>UD                    { align1 WE_all 2Q I@3 };
mov(8)          g81.1<2>UD      0x00000000UD                    { align1 WE_all 2Q };
mov(8)          g81.1<2>UD      g61.1<8,4,2>UD                  { align1 2Q };
mov(8)          g13.1<2>UD      g79.1<8,4,2>UD                  { align1 WE_all 1Q I@3 };
mov(8)          g15.1<2>UD      g81.1<8,4,2>UD                  { align1 WE_all 2Q I@2 };
cmp.g.f0.0(4)   null<1>UD       g13<8,2,4>UD    g13.2<8,2,4>UD  { align1 WE_all 1N I@2 };
(+f0.0) cmp.z.f0.0(4) null<1>UD g13.1<8,2,4>UD  g13.3<8,2,4>UD  { align1 WE_all 1N };
(-f0.0) cmp.g.f0.0(4) null<1>UD g13.1<8,2,4>UD  g13.3<8,2,4>UD  { align1 WE_all 1N };
(+f0.0) mov(4)  g13.2<4>UD      g13<8,2,4>UD                    { align1 WE_all 1N };
(+f0.0) mov(4)  g13.3<4>UD      g13.1<8,2,4>UD                  { align1 WE_all 1N I@1 };
cmp.g.f0.0(2)   null<1>UD       g13.2<0,1,0>UD  g13.4<4,2,2>UD  { align1 WE_all 1N I@1 };
(+f0.0) cmp.z.f0.0(2) null<1>UD g13.3<0,1,0>UD  g13.5<4,2,2>UD  { align1 WE_all 1N };
(-f0.0) cmp.g.f0.0(2) null<1>UD g13.3<0,1,0>UD  g13.5<4,2,2>UD  { align1 WE_all 1N };
(+f0.0) mov(2)  g13.4<2>UD      g13.2<0,1,0>UD                  { align1 WE_all 1N };
(+f0.0) mov(2)  g13.5<2>UD      g13.3<0,1,0>UD                  { align1 WE_all 1N I@1 };
cmp.g.f0.0(2)   null<1>UD       g14.2<0,1,0>UD  g14.4<4,2,2>UD  { align1 WE_all 1N I@6 };
(+f0.0) cmp.z.f0.0(2) null<1>UD g14.3<0,1,0>UD  g14.5<4,2,2>UD  { align1 WE_all 1N };
(-f0.0) cmp.g.f0.0(2) null<1>UD g14.3<0,1,0>UD  g14.5<4,2,2>UD  { align1 WE_all 1N };
(+f0.0) mov(2)  g14.4<2>UD      g14.2<0,1,0>UD                  { align1 WE_all 1N };
(+f0.0) mov(2)  g14.5<2>UD      g14.3<0,1,0>UD                  { align1 WE_all 1N I@1 };
cmp.g.f0.0(4)   null<1>UD       g13.6<0,1,0>UD  g14<8,4,2>UD    { align1 WE_all 1N I@1 };
(+f0.0) cmp.z.f0.0(4) null<1>UD g13.7<0,1,0>UD  g14.1<8,4,2>UD  { align1 WE_all 1N };
(-f0.0) cmp.g.f0.0(4) null<1>UD g13.7<0,1,0>UD  g14.1<8,4,2>UD  { align1 WE_all 1N };
(+f0.0) mov(4)  g14<2>UD        g13.6<0,1,0>UD                  { align1 WE_all 1N };
(+f0.0) mov(4)  g14.1<2>UD      g13.7<0,1,0>UD                  { align1 WE_all 1N I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(8)          g63.1<2>UD      g13.1<8,4,2>UD                  { align1 1Q };
cmp.g.f0.0(4)   null<1>UD       g15<8,2,4>UD    g15.2<8,2,4>UD  { align1 WE_all 1N };
mov(8)          g63<2>UD        g13<8,4,2>UD                    { align1 1Q I@2 };
(+f0.0) cmp.z.f0.0(4) null<1>UD g15.1<8,2,4>UD  g15.3<8,2,4>UD  { align1 WE_all 1N };
add(8)          g59<1>D         g53<8,8,1>D     -g63<8,4,2>D    { align1 1Q I@2 };
(-f0.0) cmp.g.f0.0(4) null<1>UD g15.1<8,2,4>UD  g15.3<8,2,4>UD  { align1 WE_all 1N };
(+f0.0) mov(4)  g15.2<4>UD      g15<8,2,4>UD                    { align1 WE_all 1N };
(+f0.0) mov(4)  g15.3<4>UD      g15.1<8,2,4>UD                  { align1 WE_all 1N I@1 };
cmp.g.f0.0(2)   null<1>UD       g15.2<0,1,0>UD  g15.4<4,2,2>UD  { align1 WE_all 1N I@1 };
(+f0.0) cmp.z.f0.0(2) null<1>UD g15.3<0,1,0>UD  g15.5<4,2,2>UD  { align1 WE_all 1N };
(-f0.0) cmp.g.f0.0(2) null<1>UD g15.3<0,1,0>UD  g15.5<4,2,2>UD  { align1 WE_all 1N };
(+f0.0) mov(2)  g15.4<2>UD      g15.2<0,1,0>UD                  { align1 WE_all 1N };
(+f0.0) mov(2)  g15.5<2>UD      g15.3<0,1,0>UD                  { align1 WE_all 1N I@1 };
cmp.g.f0.0(2)   null<1>UD       g16.2<0,1,0>UD  g16.4<4,2,2>UD  { align1 WE_all 1N I@6 };
(+f0.0) cmp.z.f0.0(2) null<1>UD g16.3<0,1,0>UD  g16.5<4,2,2>UD  { align1 WE_all 1N };
(-f0.0) cmp.g.f0.0(2) null<1>UD g16.3<0,1,0>UD  g16.5<4,2,2>UD  { align1 WE_all 1N };
(+f0.0) mov(2)  g16.4<2>UD      g16.2<0,1,0>UD                  { align1 WE_all 1N };
(+f0.0) mov(2)  g16.5<2>UD      g16.3<0,1,0>UD                  { align1 WE_all 1N I@1 };
cmp.g.f0.0(4)   null<1>UD       g15.6<0,1,0>UD  g16<8,4,2>UD    { align1 WE_all 1N I@1 };
(+f0.0) cmp.z.f0.0(4) null<1>UD g15.7<0,1,0>UD  g16.1<8,4,2>UD  { align1 WE_all 1N };
(-f0.0) cmp.g.f0.0(4) null<1>UD g15.7<0,1,0>UD  g16.1<8,4,2>UD  { align1 WE_all 1N };
(+f0.0) mov(4)  g16<2>UD        g15.6<0,1,0>UD                  { align1 WE_all 1N };
(+f0.0) mov(4)  g16.1<2>UD      g15.7<0,1,0>UD                  { align1 WE_all 1N I@1 };
cmp.g.f0.0(8)   null<1>UD       g14.6<0,1,0>UD  g15<8,4,2>UD    { align1 WE_all 1Q I@1 };
(+f0.0) cmp.z.f0.0(8) null<1>UD g14.7<0,1,0>UD  g15.1<8,4,2>UD  { align1 WE_all 1Q };
(-f0.0) cmp.g.f0.0(8) null<1>UD g14.7<0,1,0>UD  g15.1<8,4,2>UD  { align1 WE_all 1Q };
(+f0.0) mov(8)  g15<2>UD        g14.6<0,1,0>UD                  { align1 WE_all 1Q };
(+f0.0) mov(8)  g15.1<2>UD      g14.7<0,1,0>UD                  { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@1 };
mov(8)          g65.1<2>UD      g15.1<8,4,2>UD                  { align1 2Q $3.src };
cmp.l.f0.0(16)  null<1>UD       g53<8,8,1>UD    g27<0,1,0>UD    { align1 1H };
mov(8)          g65<2>UD        g15<8,4,2>UD                    { align1 2Q I@2 };
add(8)          g60<1>D         g54<8,8,1>D     -g65<8,4,2>D    { align1 2Q I@1 };
(+f0.0) if(16)  JIP:  LABEL13         UIP:  LABEL13             { align1 1H };
mov(16)         g61<1>D         0D                              { align1 WE_all 1H };
mov(16)         g61<1>D         -g6<8,8,1>D                     { align1 1H };
cmp.z.f0.0(16)  null<1>D        g53<8,8,1>D     0D              { align1 1H };
add(8)          g61.1<2>D       g61<8,4,2>D     g61.1<8,4,2>D   { align1 WE_all 1Q I@2 };
add(4)          g61.2<4>D       g61.1<8,2,4>D   g61.2<8,2,4>D   { align1 WE_all 1N I@1 };
add(4)          g61.3<4>D       g61.1<8,2,4>D   g61.3<8,2,4>D   { align1 WE_all 1N I@1 };
add(4)          g61.4<1>D       g61.3<0,1,0>D   g61.4<4,4,1>D   { align1 WE_all 1N I@1 };
add(4)          g62.4<1>D       g62.3<0,1,0>D   g62.4<4,4,1>D   { align1 WE_all 1N I@2 };
add(8)          g62<1>D         g61.7<0,1,0>D   g62<1,1,0>D     { align1 WE_all 1Q I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         g68<1>D         g62.7<0,1,0>D                   { align1 1H $3.src };
(+f0.0) if(16)  JIP:  LABEL15         UIP:  LABEL14             { align1 1H };
add(8)          g62<1>D         g41<8,4,2>D     28D             { align1 1Q F@2 compacted };
add(8)          g63<1>D         g43<8,4,2>D     28D             { align1 2Q F@1 compacted };
cmp.l.f0.0(8)   g64<1>UD        g62<8,8,1>UD    g41<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g65<1>UD        g63<8,8,1>UD    g43<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g75<2>UD        g62<4,4,1>UD                    { align1 1Q $0.src };
mov(8)          g77<2>UD        g63<4,4,1>UD                    { align1 2Q $0.src };
add(8)          g66<1>D         -g64<8,8,1>D    g41.1<8,4,2>D   { align1 1Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $3.src };
add(8)          g67<1>D         -g65<8,8,1>D    g43.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g75.1<2>UD      g66<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g77.1<2>UD      g67<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g93UD           g75UD           g68UD           0x0824058c                0x00000080
                            ugm MsgDesc: ( atomic_add, a64, d32, V1, L1UC_L3WB dst_len = 2, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };
else(16)        JIP:  LABEL14         UIP:  LABEL14             { align1 1H };

LABEL15:
mov(16)         g93<1>UD        0x00000000UD                    { align1 1H $0.dst };

LABEL14:
endif(16)       JIP:  LABEL13                                   { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(16)         g67<1>D         0D                              { align1 WE_all 1H I@4 };
mov(16)         g67<1>D         -g6<8,8,1>D                     { align1 1H };
cmp.nz.f0.0(16) null<1>D        g4<8,8,1>D      g97<8,8,1>D     { align1 1H };
add(8)          g67.1<2>D       g67<8,4,2>D     g67.1<8,4,2>D   { align1 WE_all 1Q I@2 };
add(4)          g67.2<4>D       g67.1<8,2,4>D   g67.2<8,2,4>D   { align1 WE_all 1N I@1 };
add(4)          g67.3<4>D       g67.1<8,2,4>D   g67.3<8,2,4>D   { align1 WE_all 1N I@1 };
add(4)          g67.4<1>D       g67.3<0,1,0>D   g67.4<4,4,1>D   { align1 WE_all 1N I@1 };
add(4)          g68.4<1>D       g68.3<0,1,0>D   g68.4<4,4,1>D   { align1 WE_all 1N I@2 };
add(8)          g68<1>D         g67.7<0,1,0>D   g68<1,1,0>D     { align1 WE_all 1Q I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
add3(16)        g69<1>D         65535W          g93.0<0,1,0>D   g67<1,1,1>D { align1 1H };
(+f0.0) if(16)  JIP:  LABEL16         UIP:  LABEL16             { align1 1H };
and(16)         g71<1>UD        g97<8,8,1>UD    0x00ffffffUD    { align1 1H $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
shl(16)         g73<1>D         g69<8,8,1>D     0x00000006UD    { align1 1H I@3 };
shr(16)         g80<1>UD        g69<1,1,0>UD    0x0000001aUD    { align1 1H compacted };
mov(8)          g82<1>UD        g41.1<8,4,2>UD                  { align1 1Q F@2 };
mov(8)          g83<1>UD        g43.1<8,4,2>UD                  { align1 2Q F@1 };
shl(16)         g90<1>D         g95<8,8,1>D     0x0000001eUD    { align1 1H };
add(16)         g92<1>D         g59<1,1,0>D     1D              { align1 1H compacted };
or(16)          g18<1>UD        g71<8,8,1>UD    0xff000000UD    { align1 1H @7 $0.dst };
add(8)          g84<1>D         g41<8,4,2>D     g73<1,1,0>D     { align1 1Q I@7 compacted };
add(8)          g85<1>D         g43<8,4,2>D     g74<1,1,0>D     { align1 2Q I@7 compacted };
or(16)          g20<1>UD        g97<1,1,0>UD    g90<1,1,0>UD    { align1 1H @5 $0.dst compacted };
or(16)          g22<1>UD        g92<8,8,1>UD    0xfffffff0UD    { align1 1H @5 $0.dst };
cmp.l.f0.0(8)   g86<1>UD        g84<8,8,1>UD    g41<8,4,2>UD    { align1 1Q I@4 };
mov(8)          g76<2>UD        g84<4,4,1>UD                    { align1 1Q $0.src };
cmp.l.f0.0(8)   g87<1>UD        g85<8,8,1>UD    g43<8,4,2>UD    { align1 2Q I@5 };
mov(8)          g78<2>UD        g85<4,4,1>UD                    { align1 2Q $0.src };
add3(16)        g89<1>D         g82<8,8,1>D     g80<8,8,1>D     -g86<1,1,1>D { align1 1H I@2 };
mov(8)          g76.1<2>UD      g89<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g78.1<2>UD      g90<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g76UD           g18UD           0x08007586                0x00000180
                            ugm MsgDesc: ( store_cmask, a64, d32, xyz, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 6 flat )  base_offset 0  { align1 1H $0 };

LABEL16:
endif(16)       JIP:  LABEL13                                   { align1 1H };
shl(16)         g93<1>D         g29<8,8,1>D     0x00000005UD    { align1 1H $0.dst };
shr(16)         g95<1>UD        g29<1,1,0>UD    0x0000001bUD    { align1 1H compacted };
add(8)          g97<1>D         g33<8,4,2>D     16D             { align1 1Q F@6 compacted };
add(8)          g98<1>D         g35<8,4,2>D     16D             { align1 2Q F@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
shl(16)         g108<1>D        g69<8,8,1>D     0x00000006UD    { align1 1H I@7 };
shr(16)         g110<1>UD       g69<1,1,0>UD    0x0000001aUD    { align1 1H compacted };
mov(8)          g112<1>UD       g41.1<8,4,2>UD                  { align1 1Q F@2 };
mov(8)          g113<1>UD       g43.1<8,4,2>UD                  { align1 2Q F@1 };
shl(16)         g64<1>D         g59<8,8,1>D     0x00000002UD    { align1 1H };
cmp.l.f0.0(8)   g99<1>UD        g97<8,8,1>UD    g33<8,4,2>UD    { align1 1Q I@7 };
cmp.l.f0.0(8)   g100<1>UD       g98<8,8,1>UD    g35<8,4,2>UD    { align1 2Q I@7 };
add(16)         g103<1>D        g97<1,1,0>D     g93<1,1,0>D     { align1 1H compacted };
add(8)          g114<1>D        g41<8,4,2>D     g108<1,1,0>D    { align1 1Q I@7 compacted };
add(8)          g115<1>D        g43<8,4,2>D     g109<1,1,0>D    { align1 2Q I@7 compacted };
add(8)          g101<1>D        -g99<8,8,1>D    g33.1<8,4,2>D   { align1 1Q I@5 };
add(8)          g102<1>D        -g100<8,8,1>D   g35.1<8,4,2>D   { align1 2Q I@5 };
cmp.l.f0.0(16)  g105<1>UD       g103<1,1,0>UD   g97<1,1,0>UD    { align1 1H I@5 compacted };
mov(8)          g77<2>UD        g103<4,4,1>UD                   { align1 1Q $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
mov(8)          g79<2>UD        g104<4,4,1>UD                   { align1 2Q };
cmp.l.f0.0(8)   g116<1>UD       g114<8,8,1>UD   g41<8,4,2>UD    { align1 1Q I@7 };
cmp.l.f0.0(8)   g117<1>UD       g115<8,8,1>UD   g43<8,4,2>UD    { align1 2Q I@7 };
add(16)         g120<1>D        g114<1,1,0>D    12D             { align1 1H compacted };
add3(16)        g107<1>D        g101<8,8,1>D    g95<8,8,1>D     -g105<1,1,1>D { align1 1H I@6 };
add3(16)        g118<1>D        g112<8,8,1>D    g110<8,8,1>D    -g116<1,1,1>D { align1 1H I@3 };
add(16)         g124<1>D        g120<1,1,0>D    g64<1,1,0>D     { align1 1H I@3 compacted };
cmp.l.f0.0(16)  g122<1>UD       g120<1,1,0>UD   g114<1,1,0>UD   { align1 1H compacted };
mov(8)          g77.1<2>UD      g107<4,4,1>UD                   { align1 1Q I@4 };
mov(8)          g79.1<2>UD      g108<4,4,1>UD                   { align1 2Q I@5 };
cmp.l.f0.0(16)  g126<1>UD       g124<1,1,0>UD   g120<1,1,0>UD   { align1 1H I@4 compacted };
mov(8)          g81<2>UD        g124<4,4,1>UD                   { align1 1Q };
mov(8)          g83<2>UD        g125<4,4,1>UD                   { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
send(16)        g17UD           g77UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $0 };
add3(16)        g1<1>D          -g122<8,8,1>D   g118<8,8,1>D    -g126<1,1,1>D { align1 1H A@1 };
mov(8)          g81.1<2>UD      g1<4,4,1>UD                     { align1 1Q I@1 };
mov(8)          g83.1<2>UD      g2<4,4,1>UD                     { align1 2Q I@2 };
and(16)         g69<1>UD        g23<8,8,1>UD    0x1fffffffUD    { align1 1H $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g81UD           g69UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };
cmp.z.f0.0(16)  null<1>D        g53<8,8,1>D     0D              { align1 1H };
(+f0.0) if(16)  JIP:  LABEL17         UIP:  LABEL17             { align1 1H };
shl(16)         g4<1>D          g55<8,8,1>D     0x00000006UD    { align1 1H };
shr(16)         g6<1>UD         g55<1,1,0>UD    0x0000001aUD    { align1 1H compacted };
add(8)          g8<1>D          g41<8,4,2>D     g4<1,1,0>D      { align1 1Q I@2 compacted };
add(8)          g9<1>D          g43<8,4,2>D     g5<1,1,0>D      { align1 2Q I@3 compacted };
cmp.l.f0.0(8)   g10<1>UD        g8<8,8,1>UD     g41<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g11<1>UD        g9<8,8,1>UD     g43<8,4,2>UD    { align1 2Q I@2 };
add(16)         g23<1>D         g114<1,1,0>D    -g8<1,1,0>D     { align1 1H compacted };
add(16)         g78<1>D         g8<1,1,0>D      12D             { align1 1H $0.src compacted };
cmp.g.f0.0(16)  g19<1>UD        g8<1,1,0>UD     0x00000000UD    { align1 1H $0.dst compacted };
add3(16)        g17<1>D         g112<8,8,1>D    g6<8,8,1>D      -g10<1,1,1>D { align1 1H @4 $0.dst };
cmp.l.f0.0(16)  g29<1>UD        g23<1,1,0>UD    g114<1,1,0>UD   { align1 1H I@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(8)          g82<2>UD        g78<4,4,1>UD                    { align1 1Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
mov(8)          g84<2>UD        g79<4,4,1>UD                    { align1 2Q I@5 };
cmp.g.f0.0(16)  g61<1>UD        g23<1,1,0>UD    0x00000000UD    { align1 1H compacted };
add(16)         g21<1>D         -g17<1,1,0>D    g19<1,1,0>D     { align1 1H @5 $0.dst compacted };
add3(16)        g59<1>D         g118<8,8,1>D    g21<8,8,1>D     -g29<1,1,1>D { align1 1H I@1 };
add(16)         g66<1>D         -g59<1,1,0>D    g61<1,1,0>D     { align1 1H I@1 compacted };
cmp.l.f0.0(16)  null<1>D        g59<8,8,1>D     0D              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
(+f0.0) sel(16) g68<1>UD        g66<1,1,0>UD    g59<1,1,0>UD    { align1 1H I@2 compacted };
(+f0.0) sel(16) g70<1>D         -g23<1,1,0>D    g23<1,1,0>D     { align1 1H $0.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
shl(16)         g74<1>D         g68<8,8,1>D     0x0000001aUD    { align1 1H I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
shr(16)         g72<1>UD        g70<1,1,0>UD    0x00000006UD    { align1 1H I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
or(16)          g76<1>UD        g72<1,1,0>UD    g74<1,1,0>UD    { align1 1H I@1 compacted };
(+f0.0) sel(16) g70<1>D         -g76<1,1,0>D    g76<1,1,0>D     { align1 1H I@1 compacted };
cmp.l.f0.0(16)  g80<1>UD        g78<1,1,0>UD    g8<1,1,0>UD     { align1 1H $0.src compacted };
add(16)         g86<1>D         -g80<1,1,0>D    g17<1,1,0>D     { align1 1H I@1 compacted };
mov(8)          g82.1<2>UD      g86<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g84.1<2>UD      g87<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g82UD           g70UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };
add(16)         g89<1>D         g8<1,1,0>D      16D             { align1 1H compacted };
mov(16)         g71<1>UD        g88<16,8,2>UB                   { align1 1H $0.src };
cmp.l.f0.0(16)  g91<1>UD        g89<1,1,0>UD    g8<1,1,0>UD     { align1 1H I@2 compacted };
mov(8)          g83<2>UD        g89<4,4,1>UD                    { align1 1Q $0.src };
mov(8)          g85<2>UD        g90<4,4,1>UD                    { align1 2Q $0.src };
add(16)         g93<1>D         -g91<1,1,0>D    g17<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g83.1<2>UD      g93<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g85.1<2>UD      g94<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g83UD           g71UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };

LABEL17:
endif(16)       JIP:  LABEL13                                   { align1 1H };
or(16)          g94<1>UD        g57<1,1,0>UD    g64<1,1,0>UD    { align1 1H compacted };
shl(16)         g96<1>D         g55<8,8,1>D     0x00000006UD    { align1 1H };
shr(16)         g98<1>UD        g55<1,1,0>UD    0x0000001aUD    { align1 1H compacted };
add(8)          g100<1>D        g41<8,4,2>D     22D             { align1 1Q compacted };
add(8)          g101<1>D        g43<8,4,2>D     22D             { align1 2Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(16)         g82<4>UB        g94<8,8,1>UD                    { align1 1H I@5 };
cmp.l.f0.0(8)   g102<1>UD       g100<8,8,1>UD   g41<8,4,2>UD    { align1 1Q I@3 };
cmp.l.f0.0(8)   g103<1>UD       g101<8,8,1>UD   g43<8,4,2>UD    { align1 2Q I@3 };
add(16)         g104<1>D        g100<1,1,0>D    g96<1,1,0>D     { align1 1H I@7 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(16)         g72<1>UD        g82<32,8,4>UB                   { align1 1H I@4 };
add3(16)        g108<1>D        -g102<8,8,1>D   g112<8,8,1>D    g98<1,1,1>D { align1 1H I@3 };
cmp.l.f0.0(16)  g106<1>UD       g104<1,1,0>UD   g100<1,1,0>UD   { align1 1H I@3 compacted };
add(16)         g110<1>D        g104<1,1,0>D    g53<1,1,0>D     { align1 1H compacted };
cmp.l.f0.0(16)  g112<1>UD       g110<1,1,0>UD   g104<1,1,0>UD   { align1 1H I@1 compacted };
mov(8)          g84<2>UD        g110<4,4,1>UD                   { align1 1Q $0.src };
mov(8)          g86<2>UD        g111<4,4,1>UD                   { align1 2Q $0.src };
add3(16)        g114<1>D        -g106<8,8,1>D   g108<8,8,1>D    -g112<1,1,1>D { align1 1H I@3 };
mov(8)          g84.1<2>UD      g114<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g86.1<2>UD      g115<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g84UD           g72UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };

LABEL13:
endif(16)       JIP:  LABEL10                                   { align1 1H };

LABEL10:
endif(16)       JIP:  LABEL7                                    { align1 1H };
add(16)         g31<1>D         g31<1,1,0>D     g27<0,1,0>D     { align1 1H I@5 compacted };

LABEL7:
while(16)       JIP:  LABEL18                                   { align1 1H };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_build_leaf_primref_to_procedurals_code[] = {
    0x80000065, 0x53058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x73050220, 0x00000024, 0x00000000,
    0xe2541a40, 0x00015303, 0xe2550040, 0x04015303,
    0x80030061, 0x59054410, 0x00000000, 0x76543210,
    0x00041c69, 0x5c058660, 0x02467305, 0x00000004,
    0x80001c01, 0x00000000, 0x00000000, 0x00000000,
    0x80004031, 0x01140000, 0xfa00540c, 0x00340000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x80004031, 0x030c0000, 0xfa00550c, 0x00300000,
    0x64591a40, 0x00805995, 0xa06d1a40, 0x01005c03,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x21620061, 0x001102cc, 0x2a640061, 0x001102cc,
    0x00030061, 0x21260aa0, 0x00000264, 0x00000000,
    0x00130061, 0x23260aa0, 0x00000264, 0x00000000,
    0x00030061, 0x25260aa0, 0x000002a4, 0x00000000,
    0x00130061, 0x27260aa0, 0x000002a4, 0x00000000,
    0x00030061, 0x29260aa0, 0x000002e4, 0x00000000,
    0x00130061, 0x2b260aa0, 0x000002e4, 0x00000000,
    0x21621761, 0x00110204, 0x2a641761, 0x00110204,
    0x21211761, 0x00110244, 0x2a231761, 0x00110244,
    0x21251761, 0x0011025c, 0x2a271761, 0x0011025c,
    0x21291761, 0x0011026c, 0x2a2b1761, 0x0011026c,
    0xa1661740, 0x024e6203, 0xaa671740, 0x024e6403,
    0x00031a70, 0x68050220, 0x52466605, 0x00446206,
    0x00030061, 0x75060220, 0x00346605, 0x00000000,
    0x00131b70, 0x69050220, 0x52466705, 0x00446406,
    0x00130061, 0x77060220, 0x00346705, 0x00000000,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x00040070, 0x00018660, 0x16000344, 0x00000008,
    0x00031d40, 0x6a052660, 0x06466805, 0x00446226,
    0x00131c40, 0x6b052660, 0x06466905, 0x00446426,
    0x00031a61, 0x75260220, 0x00346a05, 0x00000000,
    0x00131a61, 0x77260220, 0x00346b05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x6b140000, 0xfb047524, 0x00040000,
    0x27192162, 0x6b006d03, 0x00040041, 0x49050660,
    0x01000344, 0x00566b16, 0x00040041, 0x6e050660,
    0x01000344, 0x00566b06, 0x00041940, 0x6e160110,
    0x01566e16, 0x00564906, 0x11041962, 0x2d058220,
    0x02466e05, 0x00000004, 0x00040070, 0x00018660,
    0x26465c05, 0x00000000, 0x01040022, 0x0001c060,
    0x00000168, 0x00000158, 0xa06f0040, 0xfff05c03,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x00031461, 0x77050220, 0x00442526, 0x00000000,
    0x80103101, 0x00000000, 0x00000000, 0x00000000,
    0x00131361, 0x78050220, 0x00442726, 0x00000000,
    0x00041b41, 0x71050660, 0x01466f05, 0x00000344,
    0x00040041, 0x4a050660, 0x01466f05, 0x00000354,
    0x00041940, 0x71160110, 0x01567116, 0x00564a06,
    0xa0731940, 0x2d007102, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x27751970, 0x71007303,
    0xa1790040, 0x730e2502, 0xaa7a0040, 0x740e2702,
    0x00031a70, 0x7b050220, 0x52467905, 0x00442506,
    0x00030061, 0x04060220, 0x00347905, 0x00000000,
    0x00131b70, 0x7c050220, 0x52467a05, 0x00442706,
    0x00130061, 0x06060220, 0x00347a05, 0x00000000,
    0x00041a52, 0x7d040e68, 0x0eae7705, 0x7b057505,
    0x00031961, 0x04260220, 0x00347d05, 0x00000000,
    0x00131a61, 0x06260220, 0x00347e05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x5a140000, 0xfb040424, 0x00040000,
    0x00040024, 0x0001c060, 0x00000020, 0x00000020,
    0x00042061, 0x5a054220, 0x00000000, 0xffffffff,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x00040061, 0x2f054660, 0x00000000, 0x00000001,
    0x00030c61, 0x31050220, 0x00442526, 0x00000000,
    0x00130b61, 0x32050220, 0x00442726, 0x00000000,
    0x00041a70, 0x00010220, 0x42465c05, 0x00466b05,
    0x01040028, 0x0001c660, 0x00000148, 0x00000148,
    0x00040041, 0x7e050660, 0x01465c05, 0x00000344,
    0x00040041, 0x4b050660, 0x01465c05, 0x00000354,
    0x00041940, 0x7e160110, 0x01567e16, 0x00564b06,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0xa0010940, 0x2d007e02, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x27041970, 0x7e000103,
    0xa1063040, 0x010e2502, 0xaa073040, 0x020e2702,
    0x00031a70, 0x08050220, 0x52460605, 0x00442506,
    0x00033261, 0x0d060220, 0x00340605, 0x00000000,
    0x00131b70, 0x09050220, 0x52460705, 0x00442706,
    0x00133261, 0x0f060220, 0x00340705, 0x00000000,
    0x00041a52, 0x0a040e68, 0x0eae3105, 0x08050405,
    0x00031961, 0x0d260220, 0x00340a05, 0x00000000,
    0x00131a61, 0x0f260220, 0x00340b05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x0b140000, 0xfb040d24, 0x00040000,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x00042270, 0x00010660, 0x26460b05, 0x00465a05,
    0x01040028, 0x0001c660, 0x00000018, 0x00000018,
    0xa05c0040, 0x00105c03, 0x00040027, 0x00014060,
    0x00000000, 0xfffffea8, 0xa00c3240, 0xfff01903,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x00041941, 0x0e050660, 0x01460c05, 0x00000344,
    0x00040041, 0x4c050660, 0x01460c05, 0x00000354,
    0x00041940, 0x0e160110, 0x01560e16, 0x00564c06,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0xa0101940, 0x2d000e02, 0x27121970, 0x0e001003,
    0xa1140040, 0x100e2502, 0xaa150040, 0x110e2702,
    0x00031a70, 0x16050220, 0x52461405, 0x00442506,
    0x00030061, 0x33060220, 0x00341405, 0x00000000,
    0x00131b70, 0x17050220, 0x52461505, 0x00442706,
    0x00130061, 0x35060220, 0x00341505, 0x00000000,
    0x00041a52, 0x1b040e68, 0x0eae3105, 0x16051205,
    0x00031961, 0x33260220, 0x00341b05, 0x00000000,
    0x00131a61, 0x35260220, 0x00341c05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x1c140000, 0xfb043324, 0x00040000,
    0x00041a70, 0x00010220, 0x42461905, 0x00466b05,
    0x01040028, 0x0001c660, 0x00000138, 0x00000138,
    0x00040041, 0x1e050660, 0x01461905, 0x00000344,
    0x00040041, 0x4d050660, 0x01461905, 0x00000354,
    0x00041940, 0x1e160110, 0x01561e16, 0x00564d06,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0xa0331940, 0x2d001e02, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x27351970, 0x1e003303,
    0xa1370040, 0x330e2502, 0xaa380040, 0x340e2702,
    0x00031a70, 0x39050220, 0x52463705, 0x00442506,
    0x00033361, 0x3e060220, 0x00343705, 0x00000000,
    0x00131b70, 0x3a050220, 0x52463805, 0x00442706,
    0x00133361, 0x40060220, 0x00343805, 0x00000000,
    0x00041a52, 0x3b040e68, 0x0eae3105, 0x39053505,
    0x00031961, 0x3e260220, 0x00343b05, 0x00000000,
    0x00131a61, 0x40260220, 0x00343c05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x3c140000, 0xfb043e24, 0x00040000,
    0x00042370, 0x00010660, 0x26463c05, 0x00461c05,
    0x01040028, 0x0001c660, 0x00000018, 0x00000018,
    0xa0190040, 0x00101903, 0x00040027, 0x00014060,
    0x00000000, 0xfffffeb8, 0x00043361, 0x33050160,
    0x00465905, 0x00000000, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0xe0351965, 0x00f03303,
    0x00040061, 0x58064540, 0x00000000, 0x00030003,
    0xa01f1a40, 0x35005c02, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0xa03d0040, 0x35001902,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x273f1970, 0x19003d03, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x27431b70, 0x3d001f03,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x00041a61, 0x41052660, 0x00463f05, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00041a65, 0x45052620, 0x02463f05, 0x00464305,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0xeb471a70, 0x00004103, 0x00041966, 0x00010220,
    0x22464705, 0x00464505, 0x11040028, 0x0001c660,
    0x000014e0, 0x000014e0, 0x00043041, 0x49050660,
    0x01461f05, 0x00000344, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x00040041, 0x4e050660,
    0x01461f05, 0x00000354, 0x00041940, 0x49160110,
    0x01564916, 0x00564e06, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0xa04b1940, 0x0310491a,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0xa0540040, 0x2d004902, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x274d1a70, 0x49004b03,
    0xa14f0040, 0x4b0e2502, 0x80103001, 0x00000000,
    0x00000000, 0x00000000, 0xaa500040, 0x4c0e2702,
    0xa1591c40, 0x540e2502, 0xaa5ad040, 0x550e2702,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00031c70, 0x51050220, 0x52464f05, 0x00442506,
    0x00030061, 0x41060220, 0x00344f05, 0x00000000,
    0x00131d70, 0x52050220, 0x52465005, 0x00442706,
    0x00130061, 0x43060220, 0x00345005, 0x00000000,
    0x00031e61, 0x45060220, 0x00345905, 0x00000000,
    0x27563070, 0x49005403, 0x00131f61, 0x47060220,
    0x00345a05, 0x00000000, 0x00032070, 0x5b050220,
    0x52465905, 0x00442506, 0x00130070, 0x5c050220,
    0x52465a05, 0x00442706, 0x275f0070, 0x19001f03,
    0x00041f52, 0x53040e68, 0x0eae3105, 0x51054d05,
    0x00041b52, 0x5d040e68, 0x0eae3105, 0x5b055605,
    0x00031a61, 0x41260220, 0x00345305, 0x00000000,
    0x00131b61, 0x43260220, 0x00345405, 0x00000000,
    0x00031b61, 0x45260220, 0x00345d05, 0x00000000,
    0x00131c61, 0x47260220, 0x00345e05, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x1d140000, 0xfb044124, 0x00040000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x37140000, 0xfb044524, 0x00040000,
    0xac612070, 0x37103702, 0x00041965, 0x00010220,
    0x22465f05, 0x00466105, 0x01040022, 0x0001c060,
    0x000000c8, 0x000000b8, 0x80040061, 0x62054660,
    0x00000000, 0x00000000, 0x00040061, 0x62050660,
    0x00462f05, 0x00000000, 0x80031940, 0x62260660,
    0x06446206, 0x00446226, 0x80021940, 0x62470660,
    0x06426227, 0x00426247, 0x80021940, 0x62670660,
    0x06426227, 0x00426267, 0x80021940, 0x62850660,
    0x06006264, 0x00346285, 0x80021a40, 0x63850660,
    0x06006364, 0x00346385, 0xa4631940, 0x63016282,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00042361, 0x1b050220, 0x000063e4, 0x00000000,
    0x00040024, 0x0001c060, 0x00000020, 0x00000020,
    0x0004a361, 0x1b054220, 0x00000000, 0x00000000,
    0x00040025, 0x00004600, 0x00000000, 0x000011f8,
    0x00040070, 0x00018220, 0x52463505, 0x00000008,
    0x01040022, 0x0001c060, 0x000011c0, 0x000011c0,
    0x00041c70, 0x00010220, 0x52463505, 0x00001b04,
    0x01040022, 0x0001c060, 0x000001d8, 0x00000198,
    0x00042369, 0x63058660, 0x02461d05, 0x00000005,
    0xe0650068, 0x01b01d03, 0x00031661, 0x67050220,
    0x00442126, 0x00000000, 0x00131561, 0x68050220,
    0x00442326, 0x00000000, 0xa1691c40, 0x630e2102,
    0xaa6a1d40, 0x640e2302, 0x00031a70, 0x6b050220,
    0x52466905, 0x00442106, 0x00033061, 0x46060220,
    0x00346905, 0x00000000, 0x00131b70, 0x6c050220,
    0x52466a05, 0x00442306, 0x00133061, 0x48060220,
    0x00346a05, 0x00000000, 0xa06f0040, 0x01006903,
    0x00041b52, 0x6d040e68, 0x0e2e6705, 0x6b056505,
    0x27711a70, 0x69006f03, 0x00030061, 0x4a060220,
    0x00346f05, 0x00000000, 0x00130061, 0x4c060220,
    0x00347005, 0x00000000, 0x00031c61, 0x46260220,
    0x00346d05, 0x00000000, 0x00131d61, 0x48260220,
    0x00346e05, 0x00000000, 0xa0731d40, 0x6d027102,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x04440000, 0xfb044624, 0x003c0000,
    0x00031961, 0x4a260220, 0x00347305, 0x00000000,
    0x00131a61, 0x4c260220, 0x00347405, 0x00000000,
    0x00042065, 0x61058220, 0x02460a05, 0x07ffffff,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x05440000, 0xfb044a24, 0x003c0000,
    0xe05f2068, 0x01d00b03, 0x00040024, 0x0001c060,
    0x00000050, 0x00000050, 0x00042361, 0x1d054220,
    0x00000000, 0xffffffff, 0x80001c01, 0x00000000,
    0x00000000, 0x00000000, 0x00040061, 0x61054220,
    0x00000000, 0xffffffff, 0x00041c61, 0x5f054220,
    0x00000000, 0x00000000, 0x00040025, 0x00004600,
    0x00000000, 0x00000fc8, 0xa0743140, 0x00f03303,
    0xa0763140, 0xfff03303, 0xa07c0040, 0x00103303,
    0x00041b70, 0x00018660, 0x56467405, 0x00000010,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x80041b61, 0x10014110, 0x00000000, 0x0c200c20,
    0x00040069, 0x10018510, 0x01567606, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0c200c20,
    0xe0780961, 0x001b0004, 0x80041b61, 0x10014110,
    0x00000000, 0x0c200c20, 0x00040069, 0x10018510,
    0x01567c06, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0c200c20, 0xe07e0961, 0x001b0004,
    0x11041a62, 0x7a058220, 0x02467805, 0xffffffff,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0xa7011170, 0x01007c03, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x01042062, 0x04058220,
    0x02467e05, 0xffffffff, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0xae061b70, 0x61007a02,
    0x00041b70, 0x00018660, 0x26460105, 0x00000000,
    0x0004a061, 0x08052660, 0x00460605, 0x00000000,
    0x80049061, 0x10014110, 0x00000000, 0x01000100,
    0x00040069, 0x10018510, 0x01567c06, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x01000100,
    0xe00a0961, 0x001b0004, 0xef391962, 0x00000a03,
    0x00040070, 0x00018660, 0x26460605, 0x00000000,
    0xef0b0062, 0x00003503, 0x00031961, 0x3b060220,
    0x00340b05, 0x00000000, 0x00131a61, 0x3d060220,
    0x00340c05, 0x00000000, 0x00031a61, 0x3b264220,
    0x00000000, 0x00000000, 0x00131a61, 0x3d264220,
    0x00000000, 0x00000000, 0x80031a61, 0x4f064220,
    0x00000000, 0x00000000, 0x00030061, 0x4f060220,
    0x00443b06, 0x00000000, 0x80131a61, 0x51064220,
    0x00000000, 0x00000000, 0x00130061, 0x51060220,
    0x00443d06, 0x00000000, 0x80031a61, 0x0d060220,
    0x00444f06, 0x00000000, 0x80030061, 0x4f264220,
    0x00000000, 0x00000000, 0x00030061, 0x4f260220,
    0x00443b26, 0x00000000, 0x80131b61, 0x0f060220,
    0x00445106, 0x00000000, 0x80130061, 0x51264220,
    0x00000000, 0x00000000, 0x00130061, 0x51260220,
    0x00443d26, 0x00000000, 0x80031b61, 0x0d260220,
    0x00444f26, 0x00000000, 0x80131a61, 0x0f260220,
    0x00445126, 0x00000000, 0x80021a70, 0x00010220,
    0x32420d07, 0x00420d47, 0x81020070, 0x00010220,
    0x12420d27, 0x00420d67, 0x91020070, 0x00010220,
    0x32420d27, 0x00420d67, 0x81020061, 0x0d470220,
    0x00420d07, 0x00000000, 0x81021961, 0x0d670220,
    0x00420d27, 0x00000000, 0x80011970, 0x00010220,
    0x32000d44, 0x00320d86, 0x81010070, 0x00010220,
    0x12000d64, 0x00320da6, 0x91010070, 0x00010220,
    0x32000d64, 0x00320da6, 0x81010061, 0x0d860220,
    0x00000d44, 0x00000000, 0x81011961, 0x0da60220,
    0x00000d64, 0x00000000, 0x80011e70, 0x00010220,
    0x32000e44, 0x00320e86, 0x81010070, 0x00010220,
    0x12000e64, 0x00320ea6, 0x91010070, 0x00010220,
    0x32000e64, 0x00320ea6, 0x81010061, 0x0e860220,
    0x00000e44, 0x00000000, 0x81011961, 0x0ea60220,
    0x00000e64, 0x00000000, 0x80021970, 0x00010220,
    0x32000dc4, 0x00440e06, 0x81020070, 0x00010220,
    0x12000de4, 0x00440e26, 0x91020070, 0x00010220,
    0x32000de4, 0x00440e26, 0x81020061, 0x0e060220,
    0x00000dc4, 0x00000000, 0x81021961, 0x0e260220,
    0x00000de4, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00030061, 0x3f260220,
    0x00440d26, 0x00000000, 0x80020070, 0x00010220,
    0x32420f07, 0x00420f47, 0x00031a61, 0x3f060220,
    0x00440d06, 0x00000000, 0x81020070, 0x00010220,
    0x12420f27, 0x00420f67, 0x00031a40, 0x3b050660,
    0x06463505, 0x02443f06, 0x91020070, 0x00010220,
    0x32420f27, 0x00420f67, 0x81020061, 0x0f470220,
    0x00420f07, 0x00000000, 0x81021961, 0x0f670220,
    0x00420f27, 0x00000000, 0x80011970, 0x00010220,
    0x32000f44, 0x00320f86, 0x81010070, 0x00010220,
    0x12000f64, 0x00320fa6, 0x91010070, 0x00010220,
    0x32000f64, 0x00320fa6, 0x81010061, 0x0f860220,
    0x00000f44, 0x00000000, 0x81011961, 0x0fa60220,
    0x00000f64, 0x00000000, 0x80011e70, 0x00010220,
    0x32001044, 0x00321086, 0x81010070, 0x00010220,
    0x12001064, 0x003210a6, 0x91010070, 0x00010220,
    0x32001064, 0x003210a6, 0x81010061, 0x10860220,
    0x00001044, 0x00000000, 0x81011961, 0x10a60220,
    0x00001064, 0x00000000, 0x80021970, 0x00010220,
    0x32000fc4, 0x00441006, 0x81020070, 0x00010220,
    0x12000fe4, 0x00441026, 0x91020070, 0x00010220,
    0x32000fe4, 0x00441026, 0x81020061, 0x10060220,
    0x00000fc4, 0x00000000, 0x81021961, 0x10260220,
    0x00000fe4, 0x00000000, 0x80031970, 0x00010220,
    0x32000ec4, 0x00440f06, 0x81030070, 0x00010220,
    0x12000ee4, 0x00440f26, 0x91030070, 0x00010220,
    0x32000ee4, 0x00440f26, 0x81030061, 0x0f060220,
    0x00000ec4, 0x00000000, 0x81031961, 0x0f260220,
    0x00000ee4, 0x00000000, 0x80101901, 0x00000000,
    0x00000000, 0x00000000, 0x00133361, 0x41260220,
    0x00440f26, 0x00000000, 0x00040070, 0x00010220,
    0x52463505, 0x00001b04, 0x00131a61, 0x41060220,
    0x00440f06, 0x00000000, 0x00131940, 0x3c050660,
    0x06463605, 0x02444106, 0x01040022, 0x0001c060,
    0x000009c8, 0x000009c8, 0x80040061, 0x3d054660,
    0x00000000, 0x00000000, 0x00040061, 0x3d052660,
    0x00460605, 0x00000000, 0x00040070, 0x00018660,
    0x16463505, 0x00000000, 0x80031a40, 0x3d260660,
    0x06443d06, 0x00443d26, 0x80021940, 0x3d470660,
    0x06423d27, 0x00423d47, 0x80021940, 0x3d670660,
    0x06423d27, 0x00423d67, 0x80021940, 0x3d850660,
    0x06003d64, 0x00343d85, 0x80021a40, 0x3e850660,
    0x06003e64, 0x00343e85, 0xa43e1940, 0x3e013d82,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00043361, 0x44050660, 0x00003ee4, 0x00000000,
    0x01040022, 0x0001c060, 0x000000f0, 0x000000e0,
    0xa13e1240, 0x01ce2903, 0xaa3f1140, 0x01ce2b03,
    0x00031a70, 0x40050220, 0x52463e05, 0x00442906,
    0x00131a70, 0x41050220, 0x52463f05, 0x00442b06,
    0x00033061, 0x4b060220, 0x00343e05, 0x00000000,
    0x00133061, 0x4d060220, 0x00343f05, 0x00000000,
    0x00031c40, 0x42052660, 0x06464005, 0x00442926,
    0x80103301, 0x00000000, 0x00000000, 0x00000000,
    0x00131c40, 0x43052660, 0x06464105, 0x00442b26,
    0x00031a61, 0x4b260220, 0x00344205, 0x00000000,
    0x00131a61, 0x4d260220, 0x00344305, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x5d140000, 0xfb184b24, 0x01004414,
    0x00040024, 0x0001c060, 0x00000020, 0x00000020,
    0x00042061, 0x5d054220, 0x00000000, 0x00000000,
    0x00040025, 0x00004600, 0x00000000, 0x00000810,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80041c61, 0x43054660, 0x00000000, 0x00000000,
    0x00040061, 0x43052660, 0x00460605, 0x00000000,
    0x00040070, 0x00010660, 0x26460405, 0x00466105,
    0x80031a40, 0x43260660, 0x06444306, 0x00444326,
    0x80021940, 0x43470660, 0x06424327, 0x00424347,
    0x80021940, 0x43670660, 0x06424327, 0x00424367,
    0x80021940, 0x43850660, 0x06004364, 0x00344385,
    0x80021a40, 0x44850660, 0x06004464, 0x00344485,
    0xa4441940, 0x44014382, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00040052, 0x45044560,
    0x0606ffff, 0x43055d04, 0x01040022, 0x0001c060,
    0x00000148, 0x00000148, 0x00043065, 0x47058220,
    0x02466105, 0x00ffffff, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x00041b69, 0x49058660,
    0x02464505, 0x00000006, 0xe0500068, 0x01a04503,
    0x00031261, 0x52050220, 0x00442926, 0x00000000,
    0x00131161, 0x53050220, 0x00442b26, 0x00000000,
    0x00040069, 0x5a058660, 0x02465f05, 0x0000001e,
    0xa05c0040, 0x00103b03, 0x0004f066, 0x12058220,
    0x02464705, 0xff000000, 0xa1541f40, 0x490e2902,
    0xaa551f40, 0x4a0e2b02, 0x2014d066, 0x5a006103,
    0x0004d066, 0x16058220, 0x02465c05, 0xfffffff0,
    0x00031c70, 0x56050220, 0x52465405, 0x00442906,
    0x00033061, 0x4c060220, 0x00345405, 0x00000000,
    0x00131d70, 0x57050220, 0x52465505, 0x00442b06,
    0x00133061, 0x4e060220, 0x00345505, 0x00000000,
    0x00041a52, 0x59040e68, 0x0e2e5205, 0x56055005,
    0x00031961, 0x4c260220, 0x00345905, 0x00000000,
    0x00131a61, 0x4e260220, 0x00345a05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x00000000, 0xfb0c4c24, 0x001c1234,
    0x00040025, 0x00004600, 0x00000000, 0x000005e0,
    0x00042069, 0x5d058660, 0x02461d05, 0x00000005,
    0xe05f0068, 0x01b01d03, 0xa1611640, 0x010e2103,
    0xaa621540, 0x010e2303, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x00041f69, 0x6c058660,
    0x02464505, 0x00000006, 0xe06e0068, 0x01a04503,
    0x00031261, 0x70050220, 0x00442926, 0x00000000,
    0x00131161, 0x71050220, 0x00442b26, 0x00000000,
    0x00040069, 0x40058660, 0x02463b05, 0x00000002,
    0x00031f70, 0x63050220, 0x52466105, 0x00442106,
    0x00131f70, 0x64050220, 0x52466205, 0x00442306,
    0xa0670040, 0x5d006102, 0xa1721f40, 0x6c0e2902,
    0xaa731f40, 0x6d0e2b02, 0x00031d40, 0x65052660,
    0x06466305, 0x00442126, 0x00131d40, 0x66052660,
    0x06466405, 0x00442326, 0x27691d70, 0x61006703,
    0x00033061, 0x4d060220, 0x00346705, 0x00000000,
    0x80103001, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x4f060220, 0x00346805, 0x00000000,
    0x00031f70, 0x74050220, 0x52467205, 0x00442906,
    0x00131f70, 0x75050220, 0x52467305, 0x00442b06,
    0xa0780040, 0x00c07203, 0x00041e52, 0x6b040e68,
    0x0e2e6505, 0x69055f05, 0x00041b52, 0x76040e68,
    0x0e2e7005, 0x74056e05, 0xa07c1b40, 0x40007802,
    0x277a0070, 0x72007803, 0x00031c61, 0x4d260220,
    0x00346b05, 0x00000000, 0x00131d61, 0x4f260220,
    0x00346c05, 0x00000000, 0x277e1c70, 0x78007c03,
    0x00030061, 0x51060220, 0x00347c05, 0x00000000,
    0x00130061, 0x53060220, 0x00347d05, 0x00000000,
    0x80001c01, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x11440000, 0xfb044d24, 0x003c0000,
    0x00040952, 0x01042e68, 0x0e2e7a05, 0x7e057605,
    0x00031961, 0x51260220, 0x00340105, 0x00000000,
    0x00131a61, 0x53260220, 0x00340205, 0x00000000,
    0x00042065, 0x45058220, 0x02461705, 0x1fffffff,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x00000000, 0xfb0c5124, 0x00044514,
    0x00040070, 0x00018660, 0x16463505, 0x00000000,
    0x01040022, 0x0001c060, 0x00000250, 0x00000250,
    0x00040069, 0x04058660, 0x02463705, 0x00000006,
    0xe0060068, 0x01a03703, 0xa1081a40, 0x040e2902,
    0xaa091b40, 0x050e2b02, 0x00031a70, 0x0a050220,
    0x52460805, 0x00442906, 0x00131a70, 0x0b050220,
    0x52460905, 0x00442b06, 0xa0170040, 0x08207202,
    0xa04e3040, 0x00c00803, 0xeb132070, 0x00000803,
    0x0004c052, 0x11040e68, 0x0e2e7005, 0x0a050605,
    0x271d1c70, 0x72001703, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x00031c61, 0x52060220,
    0x00344e05, 0x00000000, 0x80103001, 0x00000000,
    0x00000000, 0x00000000, 0x00131d61, 0x54060220,
    0x00344f05, 0x00000000, 0xeb3d0070, 0x00001703,
    0xa015d040, 0x13021102, 0x00041952, 0x3b040e68,
    0x0e2e7605, 0x1d051505, 0xa0421940, 0x3d023b02,
    0x00040070, 0x00018660, 0x56463b05, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x2f441a62, 0x3b004203, 0xaf463062, 0x17021702,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00041a69, 0x4a058660, 0x02464405, 0x0000001a,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0xe0481a68, 0x00604603, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x204c1966, 0x4a004803,
    0xaf461962, 0x4c024c02, 0x27503070, 0x08004e03,
    0xa0561940, 0x11025002, 0x00031961, 0x52260220,
    0x00345605, 0x00000000, 0x00131a61, 0x54260220,
    0x00345705, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044031, 0x00000000,
    0xfb0c5224, 0x00044614, 0xa0590040, 0x01000803,
    0x00043061, 0x47050020, 0x00565806, 0x00000000,
    0x275b1a70, 0x08005903, 0x00033061, 0x53060220,
    0x00345905, 0x00000000, 0x00133061, 0x55060220,
    0x00345a05, 0x00000000, 0xa05d1b40, 0x11025b02,
    0x00031961, 0x53260220, 0x00345d05, 0x00000000,
    0x00131a61, 0x55260220, 0x00345e05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x00000000, 0xf3085324, 0x00024714,
    0x00040025, 0x00004600, 0x00000000, 0x00000140,
    0x205e0066, 0x40003903, 0x00040069, 0x60058660,
    0x02463705, 0x00000006, 0xe0620068, 0x01a03703,
    0xa1640040, 0x016e2903, 0xaa650040, 0x016e2b03,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00041d61, 0x52070200, 0x00465e05, 0x00000000,
    0x00031b70, 0x66050220, 0x52466405, 0x00442906,
    0x00131b70, 0x67050220, 0x52466505, 0x00442b06,
    0xa0681f40, 0x60006402, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x00041c61, 0x48050020,
    0x00665207, 0x00000000, 0x00041b52, 0x6c042e68,
    0x0e0e6605, 0x62057005, 0x276a1b70, 0x64006803,
    0xa06e0040, 0x35006802, 0x27701970, 0x68006e03,
    0x00033061, 0x54060220, 0x00346e05, 0x00000000,
    0x00133061, 0x56060220, 0x00346f05, 0x00000000,
    0x00041b52, 0x72042e68, 0x0e2e6a05, 0x70056c05,
    0x00031961, 0x54260220, 0x00347205, 0x00000000,
    0x00131a61, 0x56260220, 0x00347305, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x00000000, 0xf3085424, 0x00024814,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x00040025, 0x00004600, 0x00000000, 0x00000018,
    0xa01f1d40, 0x1b101f02, 0x00040027, 0x00014060,
    0x00000000, 0xffffea70, 0x80030061, 0x7e050220,
    0x00460005, 0x00000000, 0x80040931, 0x00000004,
    0x30007e0c, 0x00000000, 0x20000060, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_build_leaf_primref_to_procedurals = {
   .prog_data = {
      .base.nr_params = 20,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 3,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 7680,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_build_leaf_primref_to_procedurals_relocs,
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
      .push.cross_thread.dwords = 20,
      .push.cross_thread.regs = 3,
      .push.cross_thread.size = 96,
   },
   .args_size = 48,
   .arg_count = 7,
   .args = gfx125_bvh_build_leaf_primref_to_procedurals_args,
   .code = gfx125_bvh_build_leaf_primref_to_procedurals_code,
};
const char *gfx125_bvh_build_leaf_primref_to_procedurals_sha1 = "75fef0a8a72306104b6a1832f4fd347b7eabbad0";
