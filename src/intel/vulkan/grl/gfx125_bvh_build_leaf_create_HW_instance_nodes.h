#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_build_leaf_create_HW_instance_nodes_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_build_leaf_create_HW_instance_nodes_args[] = {
   { 0, 8 },
   { 8, 8 },
   { 16, 8 },
   { 24, 8 },
   { 32, 8 },
   { 40, 4 },
   { 44, 4 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g82<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g6<1>UD         g0.1<0,1,0>UD                   { align1 1H };
add(1)          g83<1>UD        g82<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@2 compacted };
add(1)          g84<1>UD        g82<0,1,0>UD    0x00000040UD    { align1 WE_all 1N compacted };
mov(8)          g126<1>UW       0x76543210V                     { align1 WE_all 1Q };
shl(16)         g39<1>D         g6<8,8,1>D      0x00000004UD    { align1 1H I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
send(1)         g1UD            g83UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(1)         g3UD            g84UD           nullUD          0x0210c500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V8, transpose, L1STATE_L3MOCS dst_len = 1, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $1 };
add(8)          g126.8<1>UW     g126<1,1,0>UW   0x0008UW        { align1 WE_all 1Q I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         g41<1>D         g126<8,8,1>UW                   { align1 1H };
add(16)         g43<1>D         g39<1,1,0>D     g41<1,1,0>D     { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g60.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g4.1<2>F        g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g101.1<2>F      g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g35.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g103.1<2>F      g2.5<0,1,0>F                    { align1 1Q };
mov(8)          g37.1<2>F       g2.5<0,1,0>F                    { align1 2Q };
mov(8)          g22.1<2>F       g2.7<0,1,0>F                    { align1 1Q };
mov(8)          g26.1<2>F       g2.7<0,1,0>F                    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.dst };
mov(8)          g24.1<2>F       g3.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g28.1<2>F       g3.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g60<2>F         g2<0,1,0>F                      { align1 1Q F@7 compacted };
mov(8)          g4<2>F          g2<0,1,0>F                      { align1 2Q F@7 compacted };
mov(8)          g101<2>F        g2.2<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g35<2>F         g2.2<0,1,0>F                    { align1 2Q F@7 compacted };
mov(8)          g103<2>F        g2.4<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g37<2>F         g2.4<0,1,0>F                    { align1 2Q F@7 compacted };
mov(8)          g22<2>F         g2.6<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g26<2>F         g2.6<0,1,0>F                    { align1 2Q F@7 compacted };
mov(8)          g24<2>F         g3<0,1,0>F                      { align1 1Q F@7 compacted };
mov(8)          g28<2>F         g3<0,1,0>F                      { align1 2Q F@7 compacted };
add(8)          g62<1>D         g60<8,4,2>D     36D             { align1 1Q F@7 compacted };
add(8)          g45<1>D         g4<8,4,2>D      36D             { align1 2Q F@7 compacted };
cmp.l.f0.0(8)   g63<1>UD        g62<8,8,1>UD    g60<8,4,2>UD    { align1 1Q I@2 };
mov(8)          g8<2>UD         g62<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g46<1>UD        g45<8,8,1>UD    g4<8,4,2>UD     { align1 2Q I@3 };
mov(8)          g10<2>UD        g45<4,4,1>UD                    { align1 2Q };
add(8)          g64<1>D         -g63<8,8,1>D    g60.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g47<1>D         -g46<8,8,1>D    g4.1<8,4,2>D    { align1 2Q I@3 };
mov(8)          g8.1<2>UD       g64<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g10.1<2>UD      g47<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g48UD           g8UD            nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
cmp.l.f0.0(16)  null<1>UD       g43<8,8,1>UD    g48<8,8,1>UD    { align1 1H $2.dst };
(+f0.0) if(16)  JIP:  LABEL0          UIP:  LABEL0              { align1 1H };
cmp.z.f0.0(16)  null<1>D        g43<8,8,1>D     0D              { align1 1H };
(+f0.0) if(16)  JIP:  LABEL1          UIP:  LABEL1              { align1 1H };
shl(16)         g50<1>D         g48<8,8,1>D     0x00000001UD    { align1 1H };
add(8)          g65<1>D         g22<8,4,2>D     36D             { align1 1Q F@4 compacted };
add(8)          g52<1>D         g26<8,4,2>D     36D             { align1 2Q F@3 compacted };
cmp.l.f0.0(8)   g66<1>UD        g65<8,8,1>UD    g22<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g53<1>UD        g52<8,8,1>UD    g26<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g9<2>UD         g65<4,4,1>UD                    { align1 1Q $2.src };
mov(8)          g11<2>UD        g52<4,4,1>UD                    { align1 2Q $2.src };
add(8)          g67<1>D         -g66<8,8,1>D    g22.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g54<1>D         -g53<8,8,1>D    g26.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g9.1<2>UD       g67<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g11.1<2>UD      g54<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g55UD           g9UD            nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
add(16)         g13<1>D         g55<1,1,0>D     g50<1,1,0>D     { align1 1H $3.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g9UD            g13UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $2 };

LABEL1:
endif(16)       JIP:  LABEL0                                    { align1 1H };
mul(16)         g56<1>D         g43<8,8,1>D     g3.4<0,1,0>UW   { align1 1H };
mul(16)         g81<1>D         g43<8,8,1>D     g3.5<0,1,0>UW   { align1 1H };
mov(8)          g62<1>UD        g101.1<8,4,2>UD                 { align1 1Q F@7 };
mov(8)          g63<1>UD        g35.1<8,4,2>UD                  { align1 2Q F@7 };
mov(8)          g30<1>UD        g103.1<8,4,2>UD                 { align1 1Q F@6 };
mov(8)          g31<1>UD        g37.1<8,4,2>UD                  { align1 2Q F@5 };
mov(8)          g32<1>UD        g24.1<8,4,2>UD                  { align1 1Q F@2 };
mov(8)          g33<1>UD        g28.1<8,4,2>UD                  { align1 2Q F@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g8<1>UD         g22.1<8,4,2>UD                  { align1 1Q F@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $2.src };
mov(8)          g9<1>UD         g26.1<8,4,2>UD                  { align1 2Q F@3 };
add(8)          g125<1>D        g22<8,4,2>D     16D             { align1 1Q compacted };
add(8)          g34<1>D         g26<8,4,2>D     16D             { align1 2Q compacted };
shl(16)         g10<1>D         g43<8,8,1>D     0x00000007UD    { align1 1H $2.src };
shr(16)         g12<1>UD        g43<1,1,0>UD    0x00000019UD    { align1 1H $2.src compacted };
add(16)         g56.1<2>UW      g56.1<16,8,2>UW g81<16,8,2>UW   { align1 1H };
mov(8)          g109<2>UD       g125<4,4,1>UD                   { align1 1Q I@5 };
mov(8)          g111<2>UD       g34<4,4,1>UD                    { align1 2Q I@5 };
add(16)         g58<1>D         g56<1,1,0>D     g3.3<0,1,0>D    { align1 1H I@3 compacted };
cmp.l.f0.0(16)  g60<1>UD        g58<1,1,0>UD    g56<1,1,0>UD    { align1 1H I@1 compacted };
add(8)          g75<1>D         g101<8,4,2>D    g58<1,1,0>D     { align1 1Q compacted };
add(8)          g64<1>D         g35<8,4,2>D     g59<1,1,0>D     { align1 2Q compacted };
cmp.l.f0.0(8)   g73<1>UD        g75<8,8,1>UD    g101<8,4,2>UD   { align1 1Q I@2 };
mov(8)          g14<2>UD        g75<4,4,1>UD                    { align1 1Q $2.src };
cmp.l.f0.0(8)   g74<1>UD        g64<8,8,1>UD    g35<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g16<2>UD        g64<4,4,1>UD                    { align1 2Q };
add3(16)        g75<1>D         g62<8,8,1>D     -g60<8,8,1>D    -g73<1,1,1>D { align1 1H I@2 };
mov(8)          g14.1<2>UD      g75<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g16.1<2>UD      g76<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g76UD           g14UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
shl(16)         g78<1>D         g76<8,8,1>D     0x00000005UD    { align1 1H $4.dst };
shr(16)         g80<1>UD        g76<1,1,0>UD    0x0000001bUD    { align1 1H compacted };
add(8)          g82<1>D         g103<8,4,2>D    g78<1,1,0>D     { align1 1Q I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
add(8)          g83<1>D         g37<8,4,2>D     g79<1,1,0>D     { align1 2Q I@3 compacted };
cmp.l.f0.0(8)   g101<1>UD       g82<8,8,1>UD    g103<8,4,2>UD   { align1 1Q I@2 };
mov(8)          g18<2>UD        g82<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g102<1>UD       g83<8,8,1>UD    g37<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g20<2>UD        g83<4,4,1>UD                    { align1 2Q };
add(16)         g105<1>D        g82<1,1,0>D     16D             { align1 1H compacted };
add3(16)        g103<1>D        g30<8,8,1>D     g80<8,8,1>D     -g101<1,1,1>D { align1 1H I@3 };
cmp.l.f0.0(16)  g107<1>UD       g105<1,1,0>UD   g82<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g4<2>UD         g105<4,4,1>UD                   { align1 1Q };
mov(8)          g6<2>UD         g106<4,4,1>UD                   { align1 2Q };
mov(8)          g18.1<2>UD      g103<4,4,1>UD                   { align1 1Q I@4 };
mov(8)          g20.1<2>UD      g104<4,4,1>UD                   { align1 2Q I@5 };
add(16)         g126<1>D        -g107<1,1,0>D   g103<1,1,0>D    { align1 1H I@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g117UD          g18UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $5 };
mov(8)          g4.1<2>UD       g126<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g6.1<2>UD       g127<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g101UD          g4UD            nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $6 };
and(16)         g63<1>UD        g123<8,8,1>UD   0x00ffffffUD    { align1 1H $5.dst };
mov(16)         g75<1>UD        g123.3<32,8,4>UB                { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
shl(16)         g1<1>D          g63<8,8,1>D     0x00000006UD    { align1 1H A@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
shr(16)         g3<1>UD         g63<1,1,0>UD    0x0000001aUD    { align1 1H F@1 compacted };
and(16)         g73<1>UD        g107<8,8,1>UD   0x7fffffffUD    { align1 1H $6.dst };
add(8)          g77<1>D         g24<8,4,2>D     g1<1,1,0>D      { align1 1Q I@3 compacted };
add(8)          g78<1>D         g28<8,4,2>D     g2<1,1,0>D      { align1 2Q I@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
cmp.l.f0.0(8)   g5<1>UD         g77<8,8,1>UD    g24<8,4,2>UD    { align1 1Q I@2 };
mov(8)          g113<2>UD       g77<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g79<1>UD        g125<8,8,1>UD   g22<8,4,2>UD    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $6.src };
cmp.l.f0.0(8)   g6<1>UD         g78<8,8,1>UD    g28<8,4,2>UD    { align1 2Q I@4 };
mov(8)          g115<2>UD       g78<4,4,1>UD                    { align1 2Q };
add(16)         g41<1>D         g77<1,1,0>D     16D             { align1 1H compacted };
add(16)         g43<1>D         g77<1,1,0>D     32D             { align1 1H compacted };
add(16)         g45<1>D         g77<1,1,0>D     56D             { align1 1H compacted };
add(16)         g47<1>D         g77<1,1,0>D     48D             { align1 1H compacted };
cmp.l.f0.0(8)   g7<1>UD         g34<8,8,1>UD    g26<8,4,2>UD    { align1 2Q $6.src };
add(8)          g80<1>D         -g79<8,8,1>D    g22.1<8,4,2>D   { align1 1Q I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
add3(16)        g83<1>D         g32<8,8,1>D     g3<8,8,1>D      -g5<1,1,1>D { align1 1H I@7 };
mov(8)          g85<2>UD        g41<4,4,1>UD                    { align1 1Q I@7 };
mov(8)          g87<2>UD        g42<4,4,1>UD                    { align1 2Q I@7 };
mov(8)          g89<2>UD        g43<4,4,1>UD                    { align1 1Q I@7 };
mov(8)          g91<2>UD        g44<4,4,1>UD                    { align1 2Q I@7 };
mov(8)          g93<2>UD        g45<4,4,1>UD                    { align1 1Q I@7 };
mov(8)          g95<2>UD        g46<4,4,1>UD                    { align1 2Q I@7 };
mov(8)          g97<2>UD        g47<4,4,1>UD                    { align1 1Q I@7 };
mov(8)          g99<2>UD        g48<4,4,1>UD                    { align1 2Q };
add(8)          g14<1>D         -g7<8,8,1>D     g26.1<8,4,2>D   { align1 2Q $4.src };
mov(8)          g109.1<2>UD     g80<4,4,1>UD                    { align1 1Q };
mov(8)          g113.1<2>UD     g83<4,4,1>UD                    { align1 1Q };
mov(8)          g115.1<2>UD     g84<4,4,1>UD                    { align1 2Q };
mov(8)          g111.1<2>UD     g14<4,4,1>UD                    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g33UD           g113UD          nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g15UD           g109UD          nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $8 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
shl(16)         g17<1>D         g15<8,8,1>D     0x00000006UD    { align1 1H $8.dst };
shr(16)         g19<1>UD        g15<1,1,0>UD    0x0000001aUD    { align1 1H $5.src compacted };
add(8)          g24<1>D         g22<8,4,2>D     g17<1,1,0>D     { align1 1Q I@2 compacted };
add(8)          g25<1>D         g26<8,4,2>D     g18<1,1,0>D     { align1 2Q I@3 compacted };
cmp.l.f0.0(8)   g28<1>UD        g24<8,8,1>UD    g22<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g29<1>UD        g25<8,8,1>UD    g26<8,4,2>UD    { align1 2Q I@2 };
add(16)         g79<1>D         g24<1,1,0>D     g10<1,1,0>D     { align1 1H compacted };
add3(16)        g30<1>D         g8<8,8,1>D      g19<8,8,1>D     -g28<1,1,1>D { align1 1H I@2 };
cmp.l.f0.0(16)  g49<1>UD        g79<1,1,0>UD    g24<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g65<2>UD        g79<4,4,1>UD                    { align1 1Q };
mov(8)          g67<2>UD        g80<4,4,1>UD                    { align1 2Q };
add(16)         g109<1>D        g79<1,1,0>D     64D             { align1 1H $8.src compacted };
cmp.l.f0.0(16)  g51<1>UD        g41<1,1,0>UD    g77<1,1,0>UD    { align1 1H compacted };
cmp.l.f0.0(16)  g55<1>UD        g43<1,1,0>UD    g77<1,1,0>UD    { align1 1H compacted };
add3(16)        g81<1>D         g30<8,8,1>D     g12<8,8,1>D     -g49<1,1,1>D { align1 1H I@6 };
cmp.l.f0.0(16)  g103<1>UD       g45<1,1,0>UD    g77<1,1,0>UD    { align1 1H $6.dst compacted };
mov(8)          g69<2>UD        g109<4,4,1>UD                   { align1 1Q I@5 };
mov(8)          g71<2>UD        g110<4,4,1>UD                   { align1 2Q I@6 };
cmp.l.f0.0(16)  g107<1>UD       g47<1,1,0>UD    g77<1,1,0>UD    { align1 1H compacted };
add(16)         g53<1>D         -g51<1,1,0>D    g83<1,1,0>D     { align1 1H I@7 compacted };
add(16)         g101<1>D        -g55<1,1,0>D    g83<1,1,0>D     { align1 1H @7 $6.dst compacted };
cmp.l.f0.0(16)  g111<1>UD       g109<1,1,0>UD   g79<1,1,0>UD    { align1 1H $8.src compacted };
mov(8)          g65.1<2>UD      g81<4,4,1>UD                    { align1 1Q I@7 };
mov(8)          g67.1<2>UD      g82<4,4,1>UD                    { align1 2Q I@7 };
add(16)         g105<1>D        -g103<1,1,0>D   g83<1,1,0>D     { align1 1H @7 $6.dst compacted };
add(16)         g109<1>D        -g107<1,1,0>D   g83<1,1,0>D     { align1 1H I@7 compacted };
mov(8)          g85.1<2>UD      g53<4,4,1>UD                    { align1 1Q I@7 };
mov(8)          g87.1<2>UD      g54<4,4,1>UD                    { align1 2Q I@7 };
mov(8)          g89.1<2>UD      g101<4,4,1>UD                   { align1 1Q I@7 };
mov(8)          g91.1<2>UD      g102<4,4,1>UD                   { align1 2Q I@7 };
add(16)         g118<1>D        -g111<1,1,0>D   g81<1,1,0>D     { align1 1H @7 $5.dst compacted };
mov(8)          g93.1<2>UD      g105<4,4,1>UD                   { align1 1Q I@7 };
mov(8)          g95.1<2>UD      g106<4,4,1>UD                   { align1 2Q I@7 };
mov(8)          g97.1<2>UD      g109<4,4,1>UD                   { align1 1Q I@7 };
mov(8)          g99.1<2>UD      g110<4,4,1>UD                   { align1 2Q I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
send(16)        g41UD           g85UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $9 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@6 };
send(16)        g49UD           g89UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $10 };
mov(8)          g69.1<2>UD      g118<4,4,1>UD                   { align1 1Q I@5 };
mov(8)          g71.1<2>UD      g119<4,4,1>UD                   { align1 2Q I@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@5 };
send(16)        g57UD           g93UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $11 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g110UD          g97UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $12 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.dst };
mul(16)         g116<1>F        -g43<1,1,0>F    g37<1,1,0>F     { align1 1H $9.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.dst };
mul(16)         g6<1>F          -g45<1,1,0>F    g33<1,1,0>F     { align1 1H $9.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.dst };
mul(16)         g31<1>F         -g41<1,1,0>F    g35<1,1,0>F     { align1 1H $9.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
mul(16)         g112<1>F        -g51<1,1,0>F    g45<1,1,0>F     { align1 1H I@7 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
mul(16)         g114<1>F        -g35<1,1,0>F    g53<1,1,0>F     { align1 1H $10.dst compacted };
mul(16)         g2<1>F          -g53<1,1,0>F    g41<1,1,0>F     { align1 1H compacted };
mul(16)         g4<1>F          -g37<1,1,0>F    g49<1,1,0>F     { align1 1H $10.dst compacted };
mul(16)         g27<1>F         -g49<1,1,0>F    g43<1,1,0>F     { align1 1H compacted };
mul(16)         g29<1>F         -g33<1,1,0>F    g51<1,1,0>F     { align1 1H compacted };
and(16)         g61<1>UD        g110<8,8,1>UD   0x00ffffffUD    { align1 1H $12.dst };
mad(16)         g121<1>F        g116<8,8,1>F    g45<8,8,1>F     g35<1,1,1>F { align1 1H @7 $5.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
mad(16)         g85<1>F         g6<8,8,1>F      g41<8,8,1>F     g37<1,1,1>F { align1 1H F@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
mad(16)         g87<1>F         g31<8,8,1>F     g43<8,8,1>F     g33<1,1,1>F { align1 1H F@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.dst };
mad(16)         g119<1>F        g114<8,8,1>F    g37<8,8,1>F     g51<1,1,1>F { align1 1H A@2 };
mad(16)         g25<1>F         g4<8,8,1>F      g33<8,8,1>F     g53<1,1,1>F { align1 1H F@7 };
mad(16)         g117<1>F        g112<8,8,1>F    g53<8,8,1>F     g43<1,1,1>F { align1 1H I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
mad(16)         g89<1>F         g27<8,8,1>F     g51<8,8,1>F     g41<1,1,1>F { align1 1H F@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
mad(16)         g91<1>F         g29<8,8,1>F     g35<8,8,1>F     g49<1,1,1>F { align1 1H F@7 };
mad(16)         g7<1>F          g2<8,8,1>F      g49<8,8,1>F     g45<1,1,1>F { align1 1H };
mul(16)         g123<1>F        g49<1,1,0>F     g121<1,1,0>F    { align1 1H F@7 compacted };
mad(16)         g125<1>F        g123<8,8,1>F    g119<8,8,1>F    g41<1,1,1>F { align1 1H F@1 };
mad(16)         g1<1>F          g125<8,8,1>F    g117<8,8,1>F    g33<1,1,1>F { align1 1H F@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
math inv(16)    g93<1>F         g1<8,8,1>F      null<8,8,1>F    { align1 1H @1 $13 };
mul(16)         g9<1>F          g117<1,1,0>F    g93<1,1,0>F     { align1 1H $13.dst compacted };
mul(16)         g11<1>F         g7<1,1,0>F      g93<1,1,0>F     { align1 1H F@5 compacted };
mul(16)         g13<1>F         g89<1,1,0>F     g93<1,1,0>F     { align1 1H F@7 compacted };
mul(16)         g15<1>F         g119<1,1,0>F    g93<1,1,0>F     { align1 1H compacted };
mul(16)         g17<1>F         g25<1,1,0>F     g93<1,1,0>F     { align1 1H compacted };
mul(16)         g19<1>F         g91<1,1,0>F     g93<1,1,0>F     { align1 1H F@7 compacted };
mul(16)         g21<1>F         g121<1,1,0>F    g93<1,1,0>F     { align1 1H $5.src compacted };
mul(16)         g23<1>F         g85<1,1,0>F     g93<1,1,0>F     { align1 1H compacted };
mul(16)         g25<1>F         g87<1,1,0>F     g93<1,1,0>F     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mul(16)         g96<1>F         -g17<1,1,0>F    g47<1,1,0>F     { align1 1H @5 $9.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mul(16)         g94<1>F         -g15<1,1,0>F    g47<1,1,0>F     { align1 1H F@7 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mul(16)         g98<1>F         -g19<1,1,0>F    g47<1,1,0>F     { align1 1H F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.dst };
mad(16)         g102<1>F        g96<8,8,1>F     g39<8,8,1>F     -g11<1,1,1>F { align1 1H A@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mad(16)         g100<1>F        g94<8,8,1>F     g39<8,8,1>F     -g9<1,1,1>F { align1 1H A@3 };
mad(16)         g104<1>F        g98<8,8,1>F     g39<8,8,1>F     -g13<1,1,1>F { align1 1H A@3 };
mad(16)         g5<1>F          g102<8,8,1>F    g55<8,8,1>F     -g23<1,1,1>F { align1 1H @3 $10.dst };
mad(16)         g3<1>F          g100<8,8,1>F    g55<8,8,1>F     -g21<1,1,1>F { align1 1H F@3 };
mad(16)         g7<1>F          g104<8,8,1>F    g55<8,8,1>F     -g25<1,1,1>F { align1 1H F@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g69UD           g57UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $14 };
add(16)         g119<1>D        g79<1,1,0>D     80D             { align1 1H compacted };
mov(16)         g57<1>D         g33<8,8,1>D                     { align1 1H $14.src };
mov(16)         g59<1>D         g41<8,8,1>D                     { align1 1H $14.src };
mov(16)         g61<1>D         g49<8,8,1>D                     { align1 1H $14.src };
mov(16)         g63<1>D         g35<8,8,1>D                     { align1 1H $14.src };
cmp.l.f0.0(16)  g121<1>UD       g119<1,1,0>UD   g79<1,1,0>UD    { align1 1H I@5 compacted };
mov(8)          g69<2>UD        g119<4,4,1>UD                   { align1 1Q $14.src };
mov(8)          g71<2>UD        g120<4,4,1>UD                   { align1 2Q $14.src };
add(16)         g123<1>D        -g121<1,1,0>D   g81<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g69.1<2>UD      g123<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g71.1<2>UD      g124<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g69UD           g57UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $15 };
add(16)         g124<1>D        g79<1,1,0>D     96D             { align1 1H compacted };
mov(16)         g57<1>D         g43<8,8,1>D                     { align1 1H $15.src };
mov(16)         g59<1>D         g51<8,8,1>D                     { align1 1H $15.src };
mov(16)         g61<1>D         g37<8,8,1>D                     { align1 1H $15.src };
mov(16)         g63<1>D         g45<8,8,1>D                     { align1 1H $15.src };
cmp.l.f0.0(16)  g126<1>UD       g124<1,1,0>UD   g79<1,1,0>UD    { align1 1H I@5 compacted };
mov(8)          g69<2>UD        g124<4,4,1>UD                   { align1 1Q $15.src };
mov(8)          g71<2>UD        g125<4,4,1>UD                   { align1 2Q $15.src };
add(16)         g85<1>D         -g126<1,1,0>D   g81<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g69.1<2>UD      g85<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g71.1<2>UD      g86<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g69UD           g57UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $0 };
add(16)         g86<1>D         g79<1,1,0>D     112D            { align1 1H F@7 compacted };
mov(16)         g1<1>D          g53<8,8,1>D                     { align1 1H $13.src };
cmp.l.f0.0(16)  g92<1>UD        g86<1,1,0>UD    g79<1,1,0>UD    { align1 1H A@2 compacted };
mov(8)          g88<2>UD        g86<4,4,1>UD                    { align1 1Q F@7 };
mov(8)          g90<2>UD        g87<4,4,1>UD                    { align1 2Q };
add(16)         g94<1>D         -g92<1,1,0>D    g81<1,1,0>D     { align1 1H A@3 compacted };
mov(8)          g88.1<2>UD      g94<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g90.1<2>UD      g95<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        nullUD          g88UD           g1UD            0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $1 };
add(16)         g95<1>D         g77<1,1,0>D     52D             { align1 1H F@6 compacted };
shl(16)         g102<1>D        g75<8,8,1>D     0x00000018UD    { align1 1H F@3 };
cmp.l.f0.0(16)  g97<1>UD        g95<1,1,0>UD    g77<1,1,0>UD    { align1 1H A@2 compacted };
mov(8)          g90<2>UD        g95<4,4,1>UD                    { align1 1Q $1.src };
mov(8)          g92<2>UD        g96<4,4,1>UD                    { align1 2Q };
add(16)         g99<1>D         -g97<1,1,0>D    g83<1,1,0>D     { align1 1H A@2 compacted };
mov(8)          g90.1<2>UD      g99<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g92.1<2>UD      g100<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g84UD           g90UD           nullUD          0x08607582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyz, L1STATE_L3MOCS dst_len = 6, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.dst };
and(16)         g100<1>UD       g84<8,8,1>UD    0x00ffffffUD    { align1 1H F@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.dst };
add(16)         g104<1>D        g86<1,1,0>D     g73<1,1,0>D     { align1 1H F@1 compacted };
mov(16)         g111<1>UD       g84.3<32,8,4>UB                 { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
or(16)          g1<1>UD         g100<1,1,0>UD   g102<1,1,0>UD   { align1 1H I@3 compacted };
or(16)          g3<1>UD         g100<8,8,1>UD   0x20000000UD    { align1 1H $1.src };
cmp.l.f0.0(16)  g106<1>UD       g104<1,1,0>UD   g86<1,1,0>UD    { align1 1H I@4 compacted };
shl(16)         g114<1>D        g111<8,8,1>D    0x00000010UD    { align1 1H I@4 };
cmp.z.f0.0(16)  null<1>D        g73<8,8,1>D     0D              { align1 1H };
add(16)         g108<1>D        -g106<1,1,0>D   g88<1,1,0>D     { align1 1H @3 $2.dst compacted };
(-f0.0) sel(16) g5<1>UD         g104<8,8,1>UD   0x00000000UD    { align1 1H $1.src };
(-f0.0) sel(16) g110<1>UD       g108<8,8,1>UD   0x00000000UD    { align1 1H I@2 };
mov(16)         g112<1>UD       g110<16,8,2>UW                  { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
or(16)          g7<1>UD         g112<1,1,0>UD   g114<1,1,0>UD   { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g65UD           g1UD            0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $2 };
add(16)         g115<1>D        g79<1,1,0>D     16D             { align1 1H compacted };
cmp.l.f0.0(16)  g117<1>UD       g115<1,1,0>UD   g79<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g57<2>UD        g115<4,4,1>UD                   { align1 1Q $0.src };
mov(8)          g59<2>UD        g116<4,4,1>UD                   { align1 2Q $0.src };
add(16)         g119<1>D        -g117<1,1,0>D   g81<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g57.1<2>UD      g119<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g59.1<2>UD      g120<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g57UD           g9UD            0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $2 };
add(16)         g120<1>D        g79<1,1,0>D     32D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
cmp.l.f0.0(16)  g122<1>UD       g120<1,1,0>UD   g79<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g58<2>UD        g120<4,4,1>UD                   { align1 1Q $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
mov(8)          g60<2>UD        g121<4,4,1>UD                   { align1 2Q $2.src };
add(16)         g124<1>D        -g122<1,1,0>D   g81<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g58.1<2>UD      g124<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g60.1<2>UD      g125<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g58UD           g17UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $3 };
add(16)         g125<1>D        g79<1,1,0>D     48D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(16)         g27<1>D         g39<8,8,1>D                     { align1 1H };
mov(16)         g29<1>D         g47<8,8,1>D                     { align1 1H };
mov(16)         g31<1>D         g55<8,8,1>D                     { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
cmp.l.f0.0(16)  g1<1>UD         g125<1,1,0>UD   g79<1,1,0>UD    { align1 1H I@4 compacted };
mov(8)          g59<2>UD        g125<4,4,1>UD                   { align1 1Q $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
mov(8)          g61<2>UD        g126<4,4,1>UD                   { align1 2Q $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g3<1>D          -g1<1,1,0>D     g81<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g59.1<2>UD      g3<4,4,1>UD                     { align1 1Q I@1 };
mov(8)          g61.1<2>UD      g4<4,4,1>UD                     { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g59UD           g25UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $4 };

LABEL0:
endif(16)       JIP:  LABEL2                                    { align1 1H };

LABEL2:
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_build_leaf_create_HW_instance_nodes_code[] = {
    0x80000065, 0x52058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x06050220, 0x00000024, 0x00000000,
    0xe2531a40, 0x00015203, 0xe2540040, 0x04015203,
    0x80030061, 0x7e054410, 0x00000000, 0x76543210,
    0x00041c69, 0x27058660, 0x02460605, 0x00000004,
    0x80001c01, 0x00000000, 0x00000000, 0x00000000,
    0x80004031, 0x01140000, 0xfa00530c, 0x00340000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x80004131, 0x030c0000, 0xfa00540c, 0x00300000,
    0x647e1a40, 0x00807e95, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00040061, 0x29050160,
    0x00467e05, 0x00000000, 0xa02b1940, 0x29002702,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x213c0061, 0x001102cc, 0x2a040061, 0x001102cc,
    0x00030061, 0x65260aa0, 0x00000264, 0x00000000,
    0x00130061, 0x23260aa0, 0x00000264, 0x00000000,
    0x00030061, 0x67260aa0, 0x000002a4, 0x00000000,
    0x00130061, 0x25260aa0, 0x000002a4, 0x00000000,
    0x00030061, 0x16260aa0, 0x000002e4, 0x00000000,
    0x00130061, 0x1a260aa0, 0x000002e4, 0x00000000,
    0x80002101, 0x00000000, 0x00000000, 0x00000000,
    0x21180061, 0x001103cc, 0x2a1c0061, 0x001103cc,
    0x213c1761, 0x00110204, 0x2a041761, 0x00110204,
    0x21651761, 0x00110244, 0x2a231761, 0x00110244,
    0x21671761, 0x0011025c, 0x2a251761, 0x0011025c,
    0x21161761, 0x0011026c, 0x2a1a1761, 0x0011026c,
    0x21181761, 0x00110304, 0x2a1c1761, 0x00110304,
    0xa13e1740, 0x024e3c03, 0xaa2d1740, 0x024e0403,
    0x00031a70, 0x3f050220, 0x52463e05, 0x00443c06,
    0x00030061, 0x08060220, 0x00343e05, 0x00000000,
    0x00131b70, 0x2e050220, 0x52462d05, 0x00440406,
    0x00130061, 0x0a060220, 0x00342d05, 0x00000000,
    0x00031c40, 0x40052660, 0x06463f05, 0x00443c26,
    0x00131b40, 0x2f052660, 0x06462e05, 0x00440426,
    0x00031a61, 0x08260220, 0x00344005, 0x00000000,
    0x00131a61, 0x0a260220, 0x00342f05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x30140000, 0xfb040824, 0x00040000,
    0x00042270, 0x00010220, 0x52462b05, 0x00463005,
    0x01040022, 0x0001c060, 0x000011e8, 0x000011e8,
    0x00040070, 0x00018660, 0x16462b05, 0x00000000,
    0x01040022, 0x0001c060, 0x00000108, 0x00000108,
    0x00040069, 0x32058660, 0x02463005, 0x00000001,
    0xa1411440, 0x024e1603, 0xaa341340, 0x024e1a03,
    0x00031a70, 0x42050220, 0x52464105, 0x00441606,
    0x00131a70, 0x35050220, 0x52463405, 0x00441a06,
    0x00033261, 0x09060220, 0x00344105, 0x00000000,
    0x00133261, 0x0b060220, 0x00343405, 0x00000000,
    0x00031c40, 0x43052660, 0x06464205, 0x00441626,
    0x00131c40, 0x36052660, 0x06463505, 0x00441a26,
    0x00031a61, 0x09260220, 0x00344305, 0x00000000,
    0x00131a61, 0x0b260220, 0x00343605, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x37140000, 0xfb040924, 0x00040000,
    0xa00d2340, 0x32003702, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb0c0924, 0x00040d14, 0x00040025, 0x00004600,
    0x00000000, 0x000010c0, 0x00040041, 0x38050660,
    0x01462b05, 0x00000344, 0x00040041, 0x51050660,
    0x01462b05, 0x00000354, 0x00031761, 0x3e050220,
    0x00446526, 0x00000000, 0x00131761, 0x3f050220,
    0x00442326, 0x00000000, 0x00031661, 0x1e050220,
    0x00446726, 0x00000000, 0x00131561, 0x1f050220,
    0x00442526, 0x00000000, 0x00031261, 0x20050220,
    0x00441826, 0x00000000, 0x00131161, 0x21050220,
    0x00441c26, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x00031461, 0x08050220,
    0x00441626, 0x00000000, 0x80103201, 0x00000000,
    0x00000000, 0x00000000, 0x00131361, 0x09050220,
    0x00441a26, 0x00000000, 0xa17d0040, 0x010e1603,
    0xaa220040, 0x010e1a03, 0x00043269, 0x0a058660,
    0x02462b05, 0x00000007, 0xe00c3268, 0x01902b03,
    0x00040040, 0x38160110, 0x01563816, 0x00565106,
    0x00031d61, 0x6d060220, 0x00347d05, 0x00000000,
    0x00131d61, 0x6f060220, 0x00342205, 0x00000000,
    0xa03a1b40, 0x0310381a, 0x273c1970, 0x38003a03,
    0xa14b0040, 0x3a0e6502, 0xaa400040, 0x3b0e2302,
    0x00031a70, 0x49050220, 0x52464b05, 0x00446506,
    0x00033261, 0x0e060220, 0x00344b05, 0x00000000,
    0x00131b70, 0x4a050220, 0x52464005, 0x00442306,
    0x00130061, 0x10060220, 0x00344005, 0x00000000,
    0x00041a52, 0x4b040e68, 0x0eae3e05, 0x49053c05,
    0x00031961, 0x0e260220, 0x00344b05, 0x00000000,
    0x00131a61, 0x10260220, 0x00344c05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x4c140000, 0xfb040e24, 0x00040000,
    0x00042469, 0x4e058660, 0x02464c05, 0x00000005,
    0xe0500068, 0x01b04c03, 0xa1521a40, 0x4e0e6702,
    0x80103001, 0x00000000, 0x00000000, 0x00000000,
    0xaa531b40, 0x4f0e2502, 0x00031a70, 0x65050220,
    0x52465205, 0x00446706, 0x00030061, 0x12060220,
    0x00345205, 0x00000000, 0x00131b70, 0x66050220,
    0x52465305, 0x00442506, 0x00130061, 0x14060220,
    0x00345305, 0x00000000, 0xa0690040, 0x01005203,
    0x00041b52, 0x67040e68, 0x0e2e1e05, 0x65055005,
    0x276b1a70, 0x52006903, 0x00030061, 0x04060220,
    0x00346905, 0x00000000, 0x00130061, 0x06060220,
    0x00346a05, 0x00000000, 0x00031c61, 0x12260220,
    0x00346705, 0x00000000, 0x00131d61, 0x14260220,
    0x00346805, 0x00000000, 0xa07e1d40, 0x67026b02,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00044531, 0x75440000, 0xfb041224, 0x003c0000,
    0x00031961, 0x04260220, 0x00347e05, 0x00000000,
    0x00131a61, 0x06260220, 0x00347f05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044631, 0x65440000, 0xfb040424, 0x003c0000,
    0x00042565, 0x3f058220, 0x02467b05, 0x00ffffff,
    0x00040061, 0x4b050020, 0x00667b1f, 0x00000000,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x00040a69, 0x01058660, 0x02463f05, 0x00000006,
    0x80003601, 0x00000000, 0x00000000, 0x00000000,
    0xe0031168, 0x01a03f03, 0x00042665, 0x49058220,
    0x02466b05, 0x7fffffff, 0xa14d1b40, 0x010e1802,
    0xaa4e1c40, 0x020e1c02, 0x80003601, 0x00000000,
    0x00000000, 0x00000000, 0x00031a70, 0x05050220,
    0x52464d05, 0x00441806, 0x00030061, 0x71060220,
    0x00344d05, 0x00000000, 0x00030070, 0x4f050220,
    0x52467d05, 0x00441606, 0x80103601, 0x00000000,
    0x00000000, 0x00000000, 0x00131c70, 0x06050220,
    0x52464e05, 0x00441c06, 0x00130061, 0x73060220,
    0x00344e05, 0x00000000, 0xa0290040, 0x01004d03,
    0xa02b0040, 0x02004d03, 0xa02d0040, 0x03804d03,
    0xa02f0040, 0x03004d03, 0x00133670, 0x07050220,
    0x52462205, 0x00441a06, 0x00031f40, 0x50052660,
    0x06464f05, 0x00441626, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x00041f52, 0x53040e68,
    0x0e2e2005, 0x05050305, 0x00031f61, 0x55060220,
    0x00342905, 0x00000000, 0x00131f61, 0x57060220,
    0x00342a05, 0x00000000, 0x00031f61, 0x59060220,
    0x00342b05, 0x00000000, 0x00131f61, 0x5b060220,
    0x00342c05, 0x00000000, 0x00031f61, 0x5d060220,
    0x00342d05, 0x00000000, 0x00131f61, 0x5f060220,
    0x00342e05, 0x00000000, 0x00031f61, 0x61060220,
    0x00342f05, 0x00000000, 0x00130061, 0x63060220,
    0x00343005, 0x00000000, 0x00133440, 0x0e052660,
    0x06460705, 0x00441a26, 0x00030061, 0x6d260220,
    0x00345005, 0x00000000, 0x00030061, 0x71260220,
    0x00345305, 0x00000000, 0x00130061, 0x73260220,
    0x00345405, 0x00000000, 0x00131c61, 0x6f260220,
    0x00340e05, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x00044731, 0x21440000,
    0xfb047124, 0x003c0000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044831, 0x0f140000,
    0xfb046d24, 0x00040000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x00042869, 0x11058660,
    0x02460f05, 0x00000006, 0xe0133568, 0x01a00f03,
    0xa1181a40, 0x110e1602, 0xaa191b40, 0x120e1a02,
    0x00031a70, 0x1c050220, 0x52461805, 0x00441606,
    0x00131a70, 0x1d050220, 0x52461905, 0x00441a06,
    0xa04f0040, 0x0a001802, 0x00041a52, 0x1e040e68,
    0x0e2e0805, 0x1c051305, 0x27311a70, 0x18004f03,
    0x00030061, 0x41060220, 0x00344f05, 0x00000000,
    0x00130061, 0x43060220, 0x00345005, 0x00000000,
    0xa06d3840, 0x04004f03, 0x27330070, 0x4d002903,
    0x27370070, 0x4d002b03, 0x00041e52, 0x51040e68,
    0x0e2e1e05, 0x31050c05, 0x27672670, 0x4d002d03,
    0x00031d61, 0x45060220, 0x00346d05, 0x00000000,
    0x00131e61, 0x47060220, 0x00346e05, 0x00000000,
    0x276b0070, 0x4d002f03, 0xa0351f40, 0x53023302,
    0xa065f640, 0x53023702, 0x276f3870, 0x4f006d03,
    0x00031f61, 0x41260220, 0x00345105, 0x00000000,
    0x00131f61, 0x43260220, 0x00345205, 0x00000000,
    0xa069f640, 0x53026702, 0xa06d1f40, 0x53026b02,
    0x00031f61, 0x55260220, 0x00343505, 0x00000000,
    0x00131f61, 0x57260220, 0x00343605, 0x00000000,
    0x00031f61, 0x59260220, 0x00346505, 0x00000000,
    0x00131f61, 0x5b260220, 0x00346605, 0x00000000,
    0xa076f540, 0x51026f02, 0x00031f61, 0x5d260220,
    0x00346905, 0x00000000, 0x00131f61, 0x5f260220,
    0x00346a05, 0x00000000, 0x00031f61, 0x61260220,
    0x00346d05, 0x00000000, 0x00131f61, 0x63260220,
    0x00346e05, 0x00000000, 0x80001f01, 0x00000000,
    0x00000000, 0x00000000, 0x00044931, 0x29440000,
    0xfb045524, 0x003c0000, 0x80001e01, 0x00000000,
    0x00000000, 0x00000000, 0x00044a31, 0x31440000,
    0xfb045924, 0x003c0000, 0x00031d61, 0x45260220,
    0x00347605, 0x00000000, 0x00131e61, 0x47260220,
    0x00347705, 0x00000000, 0x80001d01, 0x00000000,
    0x00000000, 0x00000000, 0x00044b31, 0x39240000,
    0xfb045d24, 0x000c0000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x00044c31, 0x6e140000,
    0xfb046124, 0x00040000, 0x80002701, 0x00000000,
    0x00000000, 0x00000000, 0x80002501, 0x00000000,
    0x00000000, 0x00000000, 0x20742941, 0x25022b00,
    0x80002701, 0x00000000, 0x00000000, 0x00000000,
    0x20062941, 0x21022d00, 0x80002701, 0x00000000,
    0x00000000, 0x00000000, 0x201f2941, 0x23022900,
    0x80002a01, 0x00000000, 0x00000000, 0x00000000,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x20701f41, 0x2d023300, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0x20722a41, 0x35022300,
    0x20020041, 0x29023500, 0x20042a41, 0x31022500,
    0x201b0041, 0x2b023100, 0x201d0041, 0x33022100,
    0x00042c65, 0x3d058220, 0x02466e05, 0x00ffffff,
    0x0004f55b, 0x79040aa8, 0x0a0a7405, 0x23052d05,
    0x80003901, 0x00000000, 0x00000000, 0x00000000,
    0x0004175b, 0x55040aa8, 0x0a0a0605, 0x25052905,
    0x80003901, 0x00000000, 0x00000000, 0x00000000,
    0x0004175b, 0x57040aa8, 0x0a0a1f05, 0x21052b05,
    0x80002501, 0x00000000, 0x00000000, 0x00000000,
    0x00040a5b, 0x77040aa8, 0x0a0a7205, 0x33052505,
    0x0004175b, 0x19040aa8, 0x0a0a0405, 0x35052105,
    0x00041b5b, 0x75040aa8, 0x0a0a7005, 0x2b053505,
    0x80003a01, 0x00000000, 0x00000000, 0x00000000,
    0x0004175b, 0x59040aa8, 0x0a0a1b05, 0x29053305,
    0x80003a01, 0x00000000, 0x00000000, 0x00000000,
    0x0004175b, 0x5b040aa8, 0x0a0a1d05, 0x31052305,
    0x0004005b, 0x07040aa8, 0x0a0a0205, 0x2d053105,
    0x207b1741, 0x79003100, 0x0004115b, 0x7d040aa8,
    0x0a0a7b05, 0x29057705, 0x0004115b, 0x01040aa8,
    0x0a0a7d05, 0x21057505, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x00049d38, 0x5d050aa0,
    0x1a460105, 0x00460001, 0x20092d41, 0x5d007500,
    0x200b1541, 0x5d000700, 0x200d1741, 0x5d005900,
    0x200f0041, 0x5d007700, 0x20110041, 0x5d001900,
    0x20131741, 0x5d005b00, 0x20153541, 0x5d007900,
    0x20170041, 0x5d005500, 0x20190041, 0x5d005700,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x2060d941, 0x2f021100, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x205e1741, 0x2f020f00,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x20621641, 0x2f021300, 0x80002701, 0x00000000,
    0x00000000, 0x00000000, 0x00040b5b, 0x66040aa8,
    0x0a2a6005, 0x0b052705, 0x80003c01, 0x00000000,
    0x00000000, 0x00000000, 0x00040b5b, 0x64040aa8,
    0x0a2a5e05, 0x09052705, 0x00040b5b, 0x68040aa8,
    0x0a2a6205, 0x0d052705, 0x0004ba5b, 0x05040aa8,
    0x0a2a6605, 0x17053705, 0x0004135b, 0x03040aa8,
    0x0a2a6405, 0x15053705, 0x0004135b, 0x07040aa8,
    0x0a2a6805, 0x19053705, 0x80002b01, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044e31, 0x00000000,
    0xfb0c4524, 0x003c3944, 0xa0770040, 0x05004f03,
    0x00043e61, 0x39050660, 0x00462105, 0x00000000,
    0x00043e61, 0x3b050660, 0x00462905, 0x00000000,
    0x00043e61, 0x3d050660, 0x00463105, 0x00000000,
    0x00043e61, 0x3f050660, 0x00462305, 0x00000000,
    0x27791d70, 0x4f007703, 0x00033e61, 0x45060220,
    0x00347705, 0x00000000, 0x00133e61, 0x47060220,
    0x00347805, 0x00000000, 0xa07b1b40, 0x51027902,
    0x00031961, 0x45260220, 0x00347b05, 0x00000000,
    0x00131a61, 0x47260220, 0x00347c05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044f31, 0x00000000, 0xfb0c4524, 0x003c3944,
    0xa07c0040, 0x06004f03, 0x00043f61, 0x39050660,
    0x00462b05, 0x00000000, 0x00043f61, 0x3b050660,
    0x00463305, 0x00000000, 0x00043f61, 0x3d050660,
    0x00462505, 0x00000000, 0x00043f61, 0x3f050660,
    0x00462d05, 0x00000000, 0x277e1d70, 0x4f007c03,
    0x00033f61, 0x45060220, 0x00347c05, 0x00000000,
    0x00133f61, 0x47060220, 0x00347d05, 0x00000000,
    0xa0551b40, 0x51027e02, 0x00031961, 0x45260220,
    0x00345505, 0x00000000, 0x00131a61, 0x47260220,
    0x00345605, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044031, 0x00000000,
    0xfb0c4524, 0x003c3944, 0xa0561740, 0x07004f03,
    0x00043d61, 0x01050660, 0x00463505, 0x00000000,
    0x275c0a70, 0x4f005603, 0x00031761, 0x58060220,
    0x00345605, 0x00000000, 0x00130061, 0x5a060220,
    0x00345705, 0x00000000, 0xa05e0b40, 0x51025c02,
    0x00031961, 0x58260220, 0x00345e05, 0x00000000,
    0x00131a61, 0x5a260220, 0x00345f05, 0x00000000,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x00000000, 0xfb0c5824, 0x003c0144,
    0xa05f1640, 0x03404d03, 0x00041369, 0x66058660,
    0x02464b05, 0x00000018, 0x27610a70, 0x4d005f03,
    0x00033161, 0x5a060220, 0x00345f05, 0x00000000,
    0x00130061, 0x5c060220, 0x00346005, 0x00000000,
    0xa0630a40, 0x53026102, 0x00031961, 0x5a260220,
    0x00346305, 0x00000000, 0x00131a61, 0x5c260220,
    0x00346405, 0x00000000, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x54340000,
    0xfb045a24, 0x001c0000, 0x80002201, 0x00000000,
    0x00000000, 0x00000000, 0x00041265, 0x64058220,
    0x02465405, 0x00ffffff, 0x80002201, 0x00000000,
    0x00000000, 0x00000000, 0xa0681140, 0x49005602,
    0x00040061, 0x6f050020, 0x0066541f, 0x00000000,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x20011b66, 0x66006403, 0x00043166, 0x03058220,
    0x02466405, 0x20000000, 0x276a1c70, 0x56006803,
    0x00041c69, 0x72058660, 0x02466f05, 0x00000010,
    0x00040070, 0x00018660, 0x16464905, 0x00000000,
    0xa06cb240, 0x58026a02, 0x11043162, 0x05058220,
    0x02466805, 0x00000000, 0x11041a62, 0x6e058220,
    0x02466c05, 0x00000000, 0x00041961, 0x70050120,
    0x00566e06, 0x00000000, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x20071966, 0x72007003,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x00000000, 0xfb0c4124, 0x003c0144,
    0xa0730040, 0x01004f03, 0x27751970, 0x4f007303,
    0x00033061, 0x39060220, 0x00347305, 0x00000000,
    0x00133061, 0x3b060220, 0x00347405, 0x00000000,
    0xa0771b40, 0x51027502, 0x00031961, 0x39260220,
    0x00347705, 0x00000000, 0x00131a61, 0x3b260220,
    0x00347805, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb0c3924, 0x003c0944, 0xa0780040, 0x02004f03,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x277a1970, 0x4f007803, 0x00033261, 0x3a060220,
    0x00347805, 0x00000000, 0x80103001, 0x00000000,
    0x00000000, 0x00000000, 0x00133261, 0x3c060220,
    0x00347905, 0x00000000, 0xa07c1b40, 0x51027a02,
    0x00031961, 0x3a260220, 0x00347c05, 0x00000000,
    0x00131a61, 0x3c260220, 0x00347d05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x00000000, 0xfb0c3a24, 0x003c1144,
    0xa07d0040, 0x03004f03, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x00040061, 0x1b050660,
    0x00462705, 0x00000000, 0x00040061, 0x1d050660,
    0x00462f05, 0x00000000, 0x00040061, 0x1f050660,
    0x00463705, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x27011c70, 0x4f007d03,
    0x00033361, 0x3b060220, 0x00347d05, 0x00000000,
    0x80103001, 0x00000000, 0x00000000, 0x00000000,
    0x00133361, 0x3d060220, 0x00347e05, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0xa0031b40, 0x51020102, 0x00031961, 0x3b260220,
    0x00340305, 0x00000000, 0x00131a61, 0x3d260220,
    0x00340405, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044431, 0x00000000,
    0xfb0c3b24, 0x003c1944, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x80030061, 0x7e050220,
    0x00460005, 0x00000000, 0x80040931, 0x00000004,
    0x30007e0c, 0x00000000, 0x20000060, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_build_leaf_create_HW_instance_nodes = {
   .prog_data = {
      .base.nr_params = 20,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 3,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 5264,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_build_leaf_create_HW_instance_nodes_relocs,
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
   .args = gfx125_bvh_build_leaf_create_HW_instance_nodes_args,
   .code = gfx125_bvh_build_leaf_create_HW_instance_nodes_code,
};
const char *gfx125_bvh_build_leaf_create_HW_instance_nodes_sha1 = "ea1f14ef74239b0630b6e8d0d23881163c35c42a";
