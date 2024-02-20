#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_build_leaf_create_HW_instance_nodes_pointers_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_build_leaf_create_HW_instance_nodes_pointers_args[] = {
   { 0, 8 },
   { 8, 8 },
   { 16, 8 },
   { 24, 8 },
   { 32, 8 },
   { 40, 4 },
   { 44, 4 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g74<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g11<1>UD        g0.1<0,1,0>UD                   { align1 1H };
add(1)          g75<1>UD        g74<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@2 compacted };
add(1)          g76<1>UD        g74<0,1,0>UD    0x00000040UD    { align1 WE_all 1N compacted };
mov(8)          g8<1>UW         0x76543210V                     { align1 WE_all 1Q };
shl(16)         g35<1>D         g11<8,8,1>D     0x00000004UD    { align1 1H I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
send(1)         g1UD            g75UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(1)         g3UD            g76UD           nullUD          0x0210c500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V8, transpose, L1STATE_L3MOCS dst_len = 1, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $1 };
add(8)          g8.8<1>UW       g8<1,1,0>UW     0x0008UW        { align1 WE_all 1Q I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         g37<1>D         g8<8,8,1>UW                     { align1 1H };
add(16)         g39<1>D         g35<1,1,0>D     g37<1,1,0>D     { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g60.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g9.1<2>F        g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g107.1<2>F      g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g31.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g109.1<2>F      g2.5<0,1,0>F                    { align1 1Q };
mov(8)          g33.1<2>F       g2.5<0,1,0>F                    { align1 2Q };
mov(8)          g4.1<2>F        g2.7<0,1,0>F                    { align1 1Q };
mov(8)          g6.1<2>F        g2.7<0,1,0>F                    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.dst };
mov(8)          g22.1<2>F       g3.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g24.1<2>F       g3.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g60<2>F         g2<0,1,0>F                      { align1 1Q F@7 compacted };
mov(8)          g9<2>F          g2<0,1,0>F                      { align1 2Q F@7 compacted };
mov(8)          g107<2>F        g2.2<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g31<2>F         g2.2<0,1,0>F                    { align1 2Q F@7 compacted };
mov(8)          g109<2>F        g2.4<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g33<2>F         g2.4<0,1,0>F                    { align1 2Q F@7 compacted };
mov(8)          g4<2>F          g2.6<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g6<2>F          g2.6<0,1,0>F                    { align1 2Q F@7 compacted };
mov(8)          g22<2>F         g3<0,1,0>F                      { align1 1Q F@7 compacted };
mov(8)          g24<2>F         g3<0,1,0>F                      { align1 2Q F@7 compacted };
add(8)          g62<1>D         g60<8,4,2>D     36D             { align1 1Q F@7 compacted };
add(8)          g41<1>D         g9<8,4,2>D      36D             { align1 2Q F@7 compacted };
cmp.l.f0.0(8)   g63<1>UD        g62<8,8,1>UD    g60<8,4,2>UD    { align1 1Q I@2 };
mov(8)          g13<2>UD        g62<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g42<1>UD        g41<8,8,1>UD    g9<8,4,2>UD     { align1 2Q I@3 };
mov(8)          g15<2>UD        g41<4,4,1>UD                    { align1 2Q };
add(8)          g64<1>D         -g63<8,8,1>D    g60.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g43<1>D         -g42<8,8,1>D    g9.1<8,4,2>D    { align1 2Q I@3 };
mov(8)          g13.1<2>UD      g64<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g15.1<2>UD      g43<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g44UD           g13UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
cmp.l.f0.0(16)  null<1>UD       g39<8,8,1>UD    g44<8,8,1>UD    { align1 1H $2.dst };
(+f0.0) if(16)  JIP:  LABEL0          UIP:  LABEL0              { align1 1H };
cmp.z.f0.0(16)  null<1>D        g39<8,8,1>D     0D              { align1 1H };
(+f0.0) if(16)  JIP:  LABEL1          UIP:  LABEL1              { align1 1H };
shl(16)         g46<1>D         g44<8,8,1>D     0x00000001UD    { align1 1H };
add(8)          g65<1>D         g4<8,4,2>D      36D             { align1 1Q F@4 compacted };
add(8)          g48<1>D         g6<8,4,2>D      36D             { align1 2Q F@3 compacted };
cmp.l.f0.0(8)   g66<1>UD        g65<8,8,1>UD    g4<8,4,2>UD     { align1 1Q I@2 };
cmp.l.f0.0(8)   g49<1>UD        g48<8,8,1>UD    g6<8,4,2>UD     { align1 2Q I@2 };
mov(8)          g14<2>UD        g65<4,4,1>UD                    { align1 1Q $2.src };
mov(8)          g16<2>UD        g48<4,4,1>UD                    { align1 2Q $2.src };
add(8)          g67<1>D         -g66<8,8,1>D    g4.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g50<1>D         -g49<8,8,1>D    g6.1<8,4,2>D    { align1 2Q I@4 };
mov(8)          g14.1<2>UD      g67<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g16.1<2>UD      g50<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g51UD           g14UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
add(16)         g18<1>D         g51<1,1,0>D     g46<1,1,0>D     { align1 1H $3.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g14UD           g18UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $2 };

LABEL1:
endif(16)       JIP:  LABEL0                                    { align1 1H };
mul(16)         g52<1>D         g39<8,8,1>D     g3.4<0,1,0>UW   { align1 1H };
mul(16)         g73<1>D         g39<8,8,1>D     g3.5<0,1,0>UW   { align1 1H };
mov(8)          g58<1>UD        g107.1<8,4,2>UD                 { align1 1Q F@7 };
mov(8)          g59<1>UD        g31.1<8,4,2>UD                  { align1 2Q F@7 };
mov(8)          g26<1>UD        g109.1<8,4,2>UD                 { align1 1Q F@6 };
mov(8)          g27<1>UD        g33.1<8,4,2>UD                  { align1 2Q F@5 };
mov(8)          g28<1>UD        g22.1<8,4,2>UD                  { align1 1Q F@2 };
mov(8)          g29<1>UD        g24.1<8,4,2>UD                  { align1 2Q F@1 };
mov(8)          g8<1>UD         g4.1<8,4,2>UD                   { align1 1Q F@4 };
mov(8)          g9<1>UD         g6.1<8,4,2>UD                   { align1 2Q F@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
add(8)          g1<1>D          g4<8,4,2>D      16D             { align1 1Q compacted };
add(8)          g30<1>D         g6<8,4,2>D      16D             { align1 2Q compacted };
shl(16)         g10<1>D         g39<8,8,1>D     0x00000007UD    { align1 1H };
shr(16)         g12<1>UD        g39<1,1,0>UD    0x00000019UD    { align1 1H $2.src compacted };
add(16)         g52.1<2>UW      g52.1<16,8,2>UW g73<16,8,2>UW   { align1 1H };
mov(8)          g124<2>UD       g1<4,4,1>UD                     { align1 1Q I@5 };
mov(8)          g126<2>UD       g30<4,4,1>UD                    { align1 2Q I@5 };
add(16)         g54<1>D         g52<1,1,0>D     g3.3<0,1,0>D    { align1 1H I@3 compacted };
cmp.l.f0.0(16)  g56<1>UD        g54<1,1,0>UD    g52<1,1,0>UD    { align1 1H I@1 compacted };
add(8)          g69<1>D         g107<8,4,2>D    g54<1,1,0>D     { align1 1Q compacted };
add(8)          g60<1>D         g31<8,4,2>D     g55<1,1,0>D     { align1 2Q compacted };
cmp.l.f0.0(8)   g61<1>UD        g69<8,8,1>UD    g107<8,4,2>UD   { align1 1Q I@2 };
mov(8)          g14<2>UD        g69<4,4,1>UD                    { align1 1Q $2.src };
cmp.l.f0.0(8)   g62<1>UD        g60<8,8,1>UD    g31<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g16<2>UD        g60<4,4,1>UD                    { align1 2Q $2.src };
add3(16)        g63<1>D         g58<8,8,1>D     -g56<8,8,1>D    -g61<1,1,1>D { align1 1H I@2 };
mov(8)          g14.1<2>UD      g63<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g16.1<2>UD      g64<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g69UD           g14UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
shl(16)         g71<1>D         g69<8,8,1>D     0x00000005UD    { align1 1H $4.dst };
shr(16)         g73<1>UD        g69<1,1,0>UD    0x0000001bUD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(8)          g75<1>D         g109<8,4,2>D    g71<1,1,0>D     { align1 1Q I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $1.src };
add(8)          g76<1>D         g33<8,4,2>D     g72<1,1,0>D     { align1 2Q I@3 compacted };
cmp.l.f0.0(8)   g77<1>UD        g75<8,8,1>UD    g109<8,4,2>UD   { align1 1Q I@2 };
mov(8)          g18<2>UD        g75<4,4,1>UD                    { align1 1Q $2.src };
cmp.l.f0.0(8)   g78<1>UD        g76<8,8,1>UD    g33<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g20<2>UD        g76<4,4,1>UD                    { align1 2Q };
add(16)         g85<1>D         g75<1,1,0>D     16D             { align1 1H compacted };
add3(16)        g83<1>D         g26<8,8,1>D     g73<8,8,1>D     -g77<1,1,1>D { align1 1H I@3 };
cmp.l.f0.0(16)  g107<1>UD       g85<1,1,0>UD    g75<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g116<2>UD       g85<4,4,1>UD                    { align1 1Q };
mov(8)          g118<2>UD       g86<4,4,1>UD                    { align1 2Q };
mov(8)          g18.1<2>UD      g83<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g20.1<2>UD      g84<4,4,1>UD                    { align1 2Q I@5 };
add(16)         g2<1>D          -g107<1,1,0>D   g83<1,1,0>D     { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g107UD          g18UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $5 };
mov(8)          g116.1<2>UD     g2<4,4,1>UD                     { align1 1Q I@1 };
mov(8)          g118.1<2>UD     g3<4,4,1>UD                     { align1 2Q I@2 };
and(16)         g63<1>UD        g113<8,8,1>UD   0x00ffffffUD    { align1 1H $5.dst };
mov(16)         g85<1>UD        g113.3<32,8,4>UB                { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g108UD          g116UD          nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
shl(16)         g14<1>D         g63<8,8,1>D     0x00000003UD    { align1 1H I@2 };
shr(16)         g16<1>UD        g63<1,1,0>UD    0x0000001dUD    { align1 1H $4.src compacted };
add(8)          g70<1>D         g22<8,4,2>D     g14<1,1,0>D     { align1 1Q I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $5.src };
add(8)          g18<1>D         g24<8,4,2>D     g15<1,1,0>D     { align1 2Q I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
cmp.l.f0.0(8)   g19<1>UD        g70<8,8,1>UD    g22<8,4,2>UD    { align1 1Q I@2 };
mov(8)          g120<2>UD       g70<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g71<1>UD        g1<8,8,1>UD     g4<8,4,2>UD     { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $5.src };
cmp.l.f0.0(8)   g20<1>UD        g18<8,8,1>UD    g24<8,4,2>UD    { align1 2Q I@4 };
mov(8)          g122<2>UD       g18<4,4,1>UD                    { align1 2Q };
cmp.l.f0.0(8)   g23<1>UD        g30<8,8,1>UD    g6<8,4,2>UD     { align1 2Q };
add(8)          g72<1>D         -g71<8,8,1>D    g4.1<8,4,2>D    { align1 1Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
add3(16)        g21<1>D         g28<8,8,1>D     g16<8,8,1>D     -g19<1,1,1>D { align1 1H I@4 };
add(8)          g24<1>D         -g23<8,8,1>D    g6.1<8,4,2>D    { align1 2Q I@3 };
mov(8)          g124.1<2>UD     g72<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g120.1<2>UD     g21<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g122.1<2>UD     g22<4,4,1>UD                    { align1 2Q I@4 };
mov(8)          g126.1<2>UD     g24<4,4,1>UD                    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g69UD           g120UD          nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g25UD           g124UD          nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $8 };
and(16)         g73<1>UD        g114<8,8,1>UD   0x7fffffffUD    { align1 1H $6.dst };
mov(8)          g87<2>UD        g69<4,4,1>UD                    { align1 1Q $7.dst };
mov(8)          g89<2>UD        g70<4,4,1>UD                    { align1 2Q $7.dst };
add(16)         g49<1>D         g69<1,1,0>D     16D             { align1 1H compacted };
add(16)         g54<1>D         g69<1,1,0>D     32D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.dst };
add(16)         g107<1>D        g69<1,1,0>D     56D             { align1 1H $5.dst compacted };
add(16)         g31<1>D         g69<1,1,0>D     48D             { align1 1H compacted };
shl(16)         g27<1>D         g25<8,8,1>D     0x00000006UD    { align1 1H $8.dst };
shr(16)         g29<1>UD        g25<1,1,0>UD    0x0000001aUD    { align1 1H compacted };
mov(8)          g87.1<2>UD      g71<4,4,1>UD                    { align1 1Q @7 $7.dst };
mov(8)          g89.1<2>UD      g72<4,4,1>UD                    { align1 2Q @7 $7.dst };
mov(8)          g91<2>UD        g49<4,4,1>UD                    { align1 1Q I@7 };
mov(8)          g93<2>UD        g50<4,4,1>UD                    { align1 2Q I@7 };
mov(8)          g95<2>UD        g54<4,4,1>UD                    { align1 1Q I@7 };
mov(8)          g97<2>UD        g55<4,4,1>UD                    { align1 2Q I@7 };
mov(8)          g99<2>UD        g107<4,4,1>UD                   { align1 1Q I@7 };
mov(8)          g101<2>UD       g108<4,4,1>UD                   { align1 2Q };
mov(8)          g103<2>UD       g31<4,4,1>UD                    { align1 1Q };
mov(8)          g105<2>UD       g32<4,4,1>UD                    { align1 2Q };
add(8)          g41<1>D         g4<8,4,2>D      g27<1,1,0>D     { align1 1Q compacted };
add(8)          g42<1>D         g6<8,4,2>D      g28<1,1,0>D     { align1 2Q compacted };
send(16)        g33UD           g87UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $9 };
cmp.l.f0.0(8)   g43<1>UD        g41<8,8,1>UD    g4<8,4,2>UD     { align1 1Q I@2 };
cmp.l.f0.0(8)   g44<1>UD        g42<8,8,1>UD    g6<8,4,2>UD     { align1 2Q I@2 };
add(16)         g75<1>D         g41<1,1,0>D     g10<1,1,0>D     { align1 1H compacted };
add3(16)        g45<1>D         g8<8,8,1>D      g29<8,8,1>D     -g43<1,1,1>D { align1 1H I@2 };
cmp.l.f0.0(16)  g47<1>UD        g75<1,1,0>UD    g41<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g65<2>UD        g75<4,4,1>UD                    { align1 1Q };
mov(8)          g67<2>UD        g76<4,4,1>UD                    { align1 2Q };
add(16)         g115<1>D        g75<1,1,0>D     64D             { align1 1H $6.src compacted };
cmp.l.f0.0(16)  g51<1>UD        g49<1,1,0>UD    g69<1,1,0>UD    { align1 1H compacted };
cmp.l.f0.0(16)  g83<1>UD        g54<1,1,0>UD    g69<1,1,0>UD    { align1 1H compacted };
add3(16)        g77<1>D         g45<8,8,1>D     g12<8,8,1>D     -g47<1,1,1>D { align1 1H I@6 };
cmp.l.f0.0(16)  g109<1>UD       g107<1,1,0>UD   g69<1,1,0>UD    { align1 1H $6.dst compacted };
mov(8)          g79<2>UD        g115<4,4,1>UD                   { align1 1Q I@5 };
mov(8)          g81<2>UD        g116<4,4,1>UD                   { align1 2Q I@6 };
cmp.l.f0.0(16)  g113<1>UD       g31<1,1,0>UD    g69<1,1,0>UD    { align1 1H $6.dst compacted };
add(16)         g53<1>D         -g51<1,1,0>D    g71<1,1,0>D     { align1 1H I@7 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
add(16)         g87<1>D         -g83<1,1,0>D    g71<1,1,0>D     { align1 1H I@7 compacted };
cmp.l.f0.0(16)  g117<1>UD       g115<1,1,0>UD   g75<1,1,0>UD    { align1 1H $6.src compacted };
mov(8)          g65.1<2>UD      g77<4,4,1>UD                    { align1 1Q I@7 };
mov(8)          g67.1<2>UD      g78<4,4,1>UD                    { align1 2Q I@7 };
add(16)         g111<1>D        -g109<1,1,0>D   g71<1,1,0>D     { align1 1H @7 $6.dst compacted };
add(16)         g115<1>D        -g113<1,1,0>D   g71<1,1,0>D     { align1 1H I@7 compacted };
mov(8)          g91.1<2>UD      g53<4,4,1>UD                    { align1 1Q I@7 };
mov(8)          g93.1<2>UD      g54<4,4,1>UD                    { align1 2Q I@7 };
mov(8)          g95.1<2>UD      g87<4,4,1>UD                    { align1 1Q I@7 };
mov(8)          g97.1<2>UD      g88<4,4,1>UD                    { align1 2Q I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
add(16)         g124<1>D        -g117<1,1,0>D   g77<1,1,0>D     { align1 1H I@7 compacted };
mov(8)          g99.1<2>UD      g111<4,4,1>UD                   { align1 1Q I@7 };
mov(8)          g101.1<2>UD     g112<4,4,1>UD                   { align1 2Q I@7 };
mov(8)          g103.1<2>UD     g115<4,4,1>UD                   { align1 1Q I@7 };
mov(8)          g105.1<2>UD     g116<4,4,1>UD                   { align1 2Q I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
send(16)        g41UD           g91UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $10 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@6 };
send(16)        g49UD           g95UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $11 };
mov(8)          g79.1<2>UD      g124<4,4,1>UD                   { align1 1Q I@5 };
mov(8)          g81.1<2>UD      g125<4,4,1>UD                   { align1 2Q I@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@5 };
send(16)        g57UD           g99UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $12 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g116UD          g103UD          nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $13 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
mul(16)         g122<1>F        -g43<1,1,0>F    g37<1,1,0>F     { align1 1H $10.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.dst };
mul(16)         g12<1>F         -g45<1,1,0>F    g33<1,1,0>F     { align1 1H $10.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.dst };
mul(16)         g31<1>F         -g41<1,1,0>F    g35<1,1,0>F     { align1 1H $10.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
mul(16)         g118<1>F        -g51<1,1,0>F    g45<1,1,0>F     { align1 1H I@7 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
mul(16)         g120<1>F        -g35<1,1,0>F    g53<1,1,0>F     { align1 1H $11.dst compacted };
mul(16)         g8<1>F          -g53<1,1,0>F    g41<1,1,0>F     { align1 1H compacted };
mul(16)         g10<1>F         -g37<1,1,0>F    g49<1,1,0>F     { align1 1H $11.dst compacted };
mul(16)         g27<1>F         -g49<1,1,0>F    g43<1,1,0>F     { align1 1H compacted };
mul(16)         g29<1>F         -g33<1,1,0>F    g51<1,1,0>F     { align1 1H compacted };
and(16)         g61<1>UD        g116<8,8,1>UD   0x00ffffffUD    { align1 1H $13.dst };
mad(16)         g1<1>F          g122<8,8,1>F    g45<8,8,1>F     g35<1,1,1>F { align1 1H F@7 };
mad(16)         g83<1>F         g12<8,8,1>F     g41<8,8,1>F     g37<1,1,1>F { align1 1H F@7 };
mad(16)         g87<1>F         g31<8,8,1>F     g43<8,8,1>F     g33<1,1,1>F { align1 1H A@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
mad(16)         g125<1>F        g120<8,8,1>F    g37<8,8,1>F     g51<1,1,1>F { align1 1H A@2 };
mad(16)         g17<1>F         g8<8,8,1>F      g49<8,8,1>F     g45<1,1,1>F { align1 1H F@7 };
mad(16)         g25<1>F         g10<8,8,1>F     g33<8,8,1>F     g53<1,1,1>F { align1 1H F@7 };
mad(16)         g123<1>F        g118<8,8,1>F    g53<8,8,1>F     g43<1,1,1>F { align1 1H I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
mad(16)         g89<1>F         g27<8,8,1>F     g51<8,8,1>F     g41<1,1,1>F { align1 1H F@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
mad(16)         g91<1>F         g29<8,8,1>F     g35<8,8,1>F     g49<1,1,1>F { align1 1H F@7 };
mul(16)         g3<1>F          g49<1,1,0>F     g1<1,1,0>F      { align1 1H F@7 compacted };
mad(16)         g5<1>F          g3<8,8,1>F      g125<8,8,1>F    g41<1,1,1>F { align1 1H F@1 };
mad(16)         g7<1>F          g5<8,8,1>F      g123<8,8,1>F    g33<1,1,1>F { align1 1H F@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
math inv(16)    g93<1>F         g7<8,8,1>F      null<8,8,1>F    { align1 1H @1 $14 };
mul(16)         g9<1>F          g123<1,1,0>F    g93<1,1,0>F     { align1 1H $14.dst compacted };
mul(16)         g11<1>F         g17<1,1,0>F     g93<1,1,0>F     { align1 1H F@7 compacted };
mul(16)         g13<1>F         g89<1,1,0>F     g93<1,1,0>F     { align1 1H F@7 compacted };
mul(16)         g15<1>F         g125<1,1,0>F    g93<1,1,0>F     { align1 1H compacted };
mul(16)         g19<1>F         g91<1,1,0>F     g93<1,1,0>F     { align1 1H F@7 compacted };
mul(16)         g21<1>F         g1<1,1,0>F      g93<1,1,0>F     { align1 1H compacted };
mul(16)         g23<1>F         g83<1,1,0>F     g93<1,1,0>F     { align1 1H compacted };
mul(16)         g17<1>F         g25<1,1,0>F     g93<1,1,0>F     { align1 1H compacted };
mul(16)         g25<1>F         g87<1,1,0>F     g93<1,1,0>F     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mul(16)         g98<1>F         -g19<1,1,0>F    g47<1,1,0>F     { align1 1H @5 $10.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mul(16)         g94<1>F         -g15<1,1,0>F    g47<1,1,0>F     { align1 1H F@7 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mul(16)         g96<1>F         -g17<1,1,0>F    g47<1,1,0>F     { align1 1H F@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
mad(16)         g104<1>F        g98<8,8,1>F     g39<8,8,1>F     -g13<1,1,1>F { align1 1H @3 $9.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mad(16)         g100<1>F        g94<8,8,1>F     g39<8,8,1>F     -g9<1,1,1>F { align1 1H F@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
mad(16)         g102<1>F        g96<8,8,1>F     g39<8,8,1>F     -g11<1,1,1>F { align1 1H F@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
mad(16)         g7<1>F          g104<8,8,1>F    g55<8,8,1>F     -g25<1,1,1>F { align1 1H @3 $11.dst };
mad(16)         g3<1>F          g100<8,8,1>F    g55<8,8,1>F     -g21<1,1,1>F { align1 1H F@3 };
mad(16)         g5<1>F          g102<8,8,1>F    g55<8,8,1>F     -g23<1,1,1>F { align1 1H F@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g79UD           g57UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $15 };
add(16)         g125<1>D        g75<1,1,0>D     80D             { align1 1H compacted };
mov(16)         g57<1>D         g33<8,8,1>D                     { align1 1H $15.src };
mov(16)         g59<1>D         g41<8,8,1>D                     { align1 1H $15.src };
mov(16)         g61<1>D         g49<8,8,1>D                     { align1 1H $15.src };
mov(16)         g63<1>D         g35<8,8,1>D                     { align1 1H $15.src };
cmp.l.f0.0(16)  g87<1>UD        g125<1,1,0>UD   g75<1,1,0>UD    { align1 1H A@5 compacted };
mov(8)          g80<2>UD        g125<4,4,1>UD                   { align1 1Q $15.src };
mov(8)          g82<2>UD        g126<4,4,1>UD                   { align1 2Q $15.src };
add(16)         g89<1>D         -g87<1,1,0>D    g77<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g80.1<2>UD      g89<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g82.1<2>UD      g90<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g80UD           g57UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $0 };
add(16)         g90<1>D         g75<1,1,0>D     96D             { align1 1H compacted };
mov(16)         g57<1>D         g43<8,8,1>D                     { align1 1H $0.src };
mov(16)         g59<1>D         g51<8,8,1>D                     { align1 1H $0.src };
mov(16)         g61<1>D         g37<8,8,1>D                     { align1 1H $0.src };
mov(16)         g63<1>D         g45<8,8,1>D                     { align1 1H $0.src };
cmp.l.f0.0(16)  g92<1>UD        g90<1,1,0>UD    g75<1,1,0>UD    { align1 1H A@5 compacted };
mov(8)          g81<2>UD        g90<4,4,1>UD                    { align1 1Q $0.src };
mov(8)          g83<2>UD        g91<4,4,1>UD                    { align1 2Q $0.src };
add(16)         g94<1>D         -g92<1,1,0>D    g77<1,1,0>D     { align1 1H A@3 compacted };
mov(8)          g81.1<2>UD      g94<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g83.1<2>UD      g95<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g81UD           g57UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $1 };
add(16)         g95<1>D         g75<1,1,0>D     112D            { align1 1H F@4 compacted };
mov(16)         g1<1>D          g53<8,8,1>D                     { align1 1H };
cmp.l.f0.0(16)  g97<1>UD        g95<1,1,0>UD    g75<1,1,0>UD    { align1 1H A@2 compacted };
mov(8)          g88<2>UD        g95<4,4,1>UD                    { align1 1Q };
mov(8)          g90<2>UD        g96<4,4,1>UD                    { align1 2Q };
add(16)         g99<1>D         -g97<1,1,0>D    g77<1,1,0>D     { align1 1H A@2 compacted };
mov(8)          g88.1<2>UD      g99<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g90.1<2>UD      g100<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        nullUD          g88UD           g1UD            0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $2 };
add(16)         g100<1>D        g69<1,1,0>D     52D             { align1 1H F@2 compacted };
shl(16)         g107<1>D        g85<8,8,1>D     0x00000018UD    { align1 1H };
cmp.l.f0.0(16)  g102<1>UD       g100<1,1,0>UD   g69<1,1,0>UD    { align1 1H A@1 compacted };
mov(8)          g89<2>UD        g100<4,4,1>UD                   { align1 1Q $2.src };
mov(8)          g91<2>UD        g101<4,4,1>UD                   { align1 2Q $2.src };
add(16)         g104<1>D        -g102<1,1,0>D   g71<1,1,0>D     { align1 1H A@3 compacted };
mov(8)          g89.1<2>UD      g104<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g91.1<2>UD      g105<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g57UD           g89UD           nullUD          0x08607582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyz, L1STATE_L3MOCS dst_len = 6, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
and(16)         g105<1>UD       g57<8,8,1>UD    0x00ffffffUD    { align1 1H $3.dst };
add(16)         g109<1>D        g59<1,1,0>D     g73<1,1,0>D     { align1 1H $3.dst compacted };
mov(16)         g116<1>UD       g57.3<32,8,4>UB                 { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
or(16)          g1<1>UD         g105<1,1,0>UD   g107<1,1,0>UD   { align1 1H I@3 compacted };
or(16)          g3<1>UD         g105<8,8,1>UD   0x20000000UD    { align1 1H $2.src };
cmp.l.f0.0(16)  g111<1>UD       g109<1,1,0>UD   g59<1,1,0>UD    { align1 1H I@4 compacted };
shl(16)         g119<1>D        g116<8,8,1>D    0x00000010UD    { align1 1H I@4 };
cmp.z.f0.0(16)  null<1>D        g73<8,8,1>D     0D              { align1 1H };
add(16)         g113<1>D        -g111<1,1,0>D   g61<1,1,0>D     { align1 1H @3 $3.dst compacted };
(-f0.0) sel(16) g5<1>UD         g109<8,8,1>UD   0x00000000UD    { align1 1H $2.src };
(-f0.0) sel(16) g115<1>UD       g113<8,8,1>UD   0x00000000UD    { align1 1H I@2 };
mov(16)         g117<1>UD       g115<16,8,2>UW                  { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
or(16)          g7<1>UD         g117<1,1,0>UD   g119<1,1,0>UD   { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g65UD           g1UD            0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $4 };
add(16)         g120<1>D        g75<1,1,0>D     16D             { align1 1H compacted };
cmp.l.f0.0(16)  g122<1>UD       g120<1,1,0>UD   g75<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g57<2>UD        g120<4,4,1>UD                   { align1 1Q };
mov(8)          g59<2>UD        g121<4,4,1>UD                   { align1 2Q };
add(16)         g124<1>D        -g122<1,1,0>D   g77<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g57.1<2>UD      g124<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g59.1<2>UD      g125<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g57UD           g9UD            0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $2 };
add(16)         g125<1>D        g75<1,1,0>D     32D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
cmp.l.f0.0(16)  g1<1>UD         g125<1,1,0>UD   g75<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g58<2>UD        g125<4,4,1>UD                   { align1 1Q $2.src };
mov(8)          g60<2>UD        g126<4,4,1>UD                   { align1 2Q $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add(16)         g3<1>D          -g1<1,1,0>D     g77<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g58.1<2>UD      g3<4,4,1>UD                     { align1 1Q I@1 };
mov(8)          g60.1<2>UD      g4<4,4,1>UD                     { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g58UD           g17UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $5 };
add(16)         g4<1>D          g75<1,1,0>D     48D             { align1 1H $4.src compacted };
mov(16)         g27<1>D         g39<8,8,1>D                     { align1 1H };
mov(16)         g29<1>D         g47<8,8,1>D                     { align1 1H };
mov(16)         g31<1>D         g55<8,8,1>D                     { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
cmp.l.f0.0(16)  g6<1>UD         g4<1,1,0>UD     g75<1,1,0>UD    { align1 1H I@4 compacted };
mov(8)          g59<2>UD        g4<4,4,1>UD                     { align1 1Q $5.src };
mov(8)          g61<2>UD        g5<4,4,1>UD                     { align1 2Q $5.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g8<1>D          -g6<1,1,0>D     g77<1,1,0>D     { align1 1H A@3 compacted };
mov(8)          g59.1<2>UD      g8<4,4,1>UD                     { align1 1Q I@1 };
mov(8)          g61.1<2>UD      g9<4,4,1>UD                     { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g59UD           g25UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $6 };

LABEL0:
endif(16)       JIP:  LABEL2                                    { align1 1H };

LABEL2:
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_build_leaf_create_HW_instance_nodes_pointers_code[] = {
    0x80000065, 0x4a058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x0b050220, 0x00000024, 0x00000000,
    0xe24b1a40, 0x00014a03, 0xe24c0040, 0x04014a03,
    0x80030061, 0x08054410, 0x00000000, 0x76543210,
    0x00041c69, 0x23058660, 0x02460b05, 0x00000004,
    0x80001c01, 0x00000000, 0x00000000, 0x00000000,
    0x80004031, 0x01140000, 0xfa004b0c, 0x00340000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x80004131, 0x030c0000, 0xfa004c0c, 0x00300000,
    0x64081a40, 0x00800895, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00040061, 0x25050160,
    0x00460805, 0x00000000, 0xa0271940, 0x25002302,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x213c0061, 0x001102cc, 0x2a090061, 0x001102cc,
    0x00030061, 0x6b260aa0, 0x00000264, 0x00000000,
    0x00130061, 0x1f260aa0, 0x00000264, 0x00000000,
    0x00030061, 0x6d260aa0, 0x000002a4, 0x00000000,
    0x00130061, 0x21260aa0, 0x000002a4, 0x00000000,
    0x00030061, 0x04260aa0, 0x000002e4, 0x00000000,
    0x00130061, 0x06260aa0, 0x000002e4, 0x00000000,
    0x80002101, 0x00000000, 0x00000000, 0x00000000,
    0x21160061, 0x001103cc, 0x2a180061, 0x001103cc,
    0x213c1761, 0x00110204, 0x2a091761, 0x00110204,
    0x216b1761, 0x00110244, 0x2a1f1761, 0x00110244,
    0x216d1761, 0x0011025c, 0x2a211761, 0x0011025c,
    0x21041761, 0x0011026c, 0x2a061761, 0x0011026c,
    0x21161761, 0x00110304, 0x2a181761, 0x00110304,
    0xa13e1740, 0x024e3c03, 0xaa291740, 0x024e0903,
    0x00031a70, 0x3f050220, 0x52463e05, 0x00443c06,
    0x00030061, 0x0d060220, 0x00343e05, 0x00000000,
    0x00131b70, 0x2a050220, 0x52462905, 0x00440906,
    0x00130061, 0x0f060220, 0x00342905, 0x00000000,
    0x00031c40, 0x40052660, 0x06463f05, 0x00443c26,
    0x00131b40, 0x2b052660, 0x06462a05, 0x00440926,
    0x00031a61, 0x0d260220, 0x00344005, 0x00000000,
    0x00131a61, 0x0f260220, 0x00342b05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x2c140000, 0xfb040d24, 0x00040000,
    0x00042270, 0x00010220, 0x52462705, 0x00462c05,
    0x01040022, 0x0001c060, 0x00001238, 0x00001238,
    0x00040070, 0x00018660, 0x16462705, 0x00000000,
    0x01040022, 0x0001c060, 0x00000108, 0x00000108,
    0x00040069, 0x2e058660, 0x02462c05, 0x00000001,
    0xa1411440, 0x024e0403, 0xaa301340, 0x024e0603,
    0x00031a70, 0x42050220, 0x52464105, 0x00440406,
    0x00131a70, 0x31050220, 0x52463005, 0x00440606,
    0x00033261, 0x0e060220, 0x00344105, 0x00000000,
    0x00133261, 0x10060220, 0x00343005, 0x00000000,
    0x00031c40, 0x43052660, 0x06464205, 0x00440426,
    0x00131c40, 0x32052660, 0x06463105, 0x00440626,
    0x00031a61, 0x0e260220, 0x00344305, 0x00000000,
    0x00131a61, 0x10260220, 0x00343205, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x33140000, 0xfb040e24, 0x00040000,
    0xa0122340, 0x2e003302, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb0c0e24, 0x00041214, 0x00040025, 0x00004600,
    0x00000000, 0x00001110, 0x00040041, 0x34050660,
    0x01462705, 0x00000344, 0x00040041, 0x49050660,
    0x01462705, 0x00000354, 0x00031761, 0x3a050220,
    0x00446b26, 0x00000000, 0x00131761, 0x3b050220,
    0x00441f26, 0x00000000, 0x00031661, 0x1a050220,
    0x00446d26, 0x00000000, 0x00131561, 0x1b050220,
    0x00442126, 0x00000000, 0x00031261, 0x1c050220,
    0x00441626, 0x00000000, 0x00131161, 0x1d050220,
    0x00441826, 0x00000000, 0x00031461, 0x08050220,
    0x00440426, 0x00000000, 0x00131361, 0x09050220,
    0x00440626, 0x00000000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0xa1010040, 0x010e0403,
    0xaa1e0040, 0x010e0603, 0x00040069, 0x0a058660,
    0x02462705, 0x00000007, 0xe00c3268, 0x01902703,
    0x00040040, 0x34160110, 0x01563416, 0x00564906,
    0x00031d61, 0x7c060220, 0x00340105, 0x00000000,
    0x00131d61, 0x7e060220, 0x00341e05, 0x00000000,
    0xa0361b40, 0x0310341a, 0x27381970, 0x34003603,
    0xa1450040, 0x360e6b02, 0xaa3c0040, 0x370e1f02,
    0x00031a70, 0x3d050220, 0x52464505, 0x00446b06,
    0x00033261, 0x0e060220, 0x00344505, 0x00000000,
    0x00131b70, 0x3e050220, 0x52463c05, 0x00441f06,
    0x00133261, 0x10060220, 0x00343c05, 0x00000000,
    0x00041a52, 0x3f040e68, 0x0eae3a05, 0x3d053805,
    0x00031961, 0x0e260220, 0x00343f05, 0x00000000,
    0x00131a61, 0x10260220, 0x00344005, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x45140000, 0xfb040e24, 0x00040000,
    0x00042469, 0x47058660, 0x02464505, 0x00000005,
    0xe0490068, 0x01b04503, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0xa14b1a40, 0x470e6d02,
    0x80103101, 0x00000000, 0x00000000, 0x00000000,
    0xaa4c1b40, 0x480e2102, 0x00031a70, 0x4d050220,
    0x52464b05, 0x00446d06, 0x00033261, 0x12060220,
    0x00344b05, 0x00000000, 0x00131b70, 0x4e050220,
    0x52464c05, 0x00442106, 0x00130061, 0x14060220,
    0x00344c05, 0x00000000, 0xa0550040, 0x01004b03,
    0x00041b52, 0x53040e68, 0x0e2e1a05, 0x4d054905,
    0x276b1a70, 0x4b005503, 0x00030061, 0x74060220,
    0x00345505, 0x00000000, 0x00130061, 0x76060220,
    0x00345605, 0x00000000, 0x00031c61, 0x12260220,
    0x00345305, 0x00000000, 0x00131d61, 0x14260220,
    0x00345405, 0x00000000, 0xa0020940, 0x53026b02,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044531, 0x6b440000, 0xfb041224, 0x003c0000,
    0x00031961, 0x74260220, 0x00340205, 0x00000000,
    0x00131a61, 0x76260220, 0x00340305, 0x00000000,
    0x00042565, 0x3f058220, 0x02467105, 0x00ffffff,
    0x00040061, 0x55050020, 0x0066711f, 0x00000000,
    0x80002501, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044631, 0x6c440000, 0xfb047424, 0x003c0000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x00041a69, 0x0e058660, 0x02463f05, 0x00000003,
    0xe0103468, 0x01d03f03, 0xa1461a40, 0x0e0e1602,
    0x80103501, 0x00000000, 0x00000000, 0x00000000,
    0xaa121b40, 0x0f0e1802, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x00031a70, 0x13050220,
    0x52464605, 0x00441606, 0x00030061, 0x78060220,
    0x00344605, 0x00000000, 0x00030070, 0x47050220,
    0x52460105, 0x00440406, 0x80103501, 0x00000000,
    0x00000000, 0x00000000, 0x00131c70, 0x14050220,
    0x52461205, 0x00441806, 0x00130061, 0x7a060220,
    0x00341205, 0x00000000, 0x00130070, 0x17050220,
    0x52461e05, 0x00440606, 0x00031c40, 0x48052660,
    0x06464705, 0x00440426, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x00041c52, 0x15040e68,
    0x0e2e1c05, 0x13051005, 0x00131b40, 0x18052660,
    0x06461705, 0x00440626, 0x00031b61, 0x7c260220,
    0x00344805, 0x00000000, 0x00031b61, 0x78260220,
    0x00341505, 0x00000000, 0x00131c61, 0x7a260220,
    0x00341605, 0x00000000, 0x00131c61, 0x7e260220,
    0x00341805, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x00044731, 0x45240000,
    0xfb047824, 0x000c0000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044831, 0x19140000,
    0xfb047c24, 0x00040000, 0x00042665, 0x49058220,
    0x02467205, 0x7fffffff, 0x00032761, 0x57060220,
    0x00344505, 0x00000000, 0x00132761, 0x59060220,
    0x00344605, 0x00000000, 0xa0310040, 0x01004503,
    0xa0360040, 0x02004503, 0x80002601, 0x00000000,
    0x00000000, 0x00000000, 0xa06b2540, 0x03804503,
    0xa01f0040, 0x03004503, 0x00042869, 0x1b058660,
    0x02461905, 0x00000006, 0xe01d0068, 0x01a01903,
    0x0003f761, 0x57260220, 0x00344705, 0x00000000,
    0x0013f761, 0x59260220, 0x00344805, 0x00000000,
    0x00031f61, 0x5b060220, 0x00343105, 0x00000000,
    0x00131f61, 0x5d060220, 0x00343205, 0x00000000,
    0x00031f61, 0x5f060220, 0x00343605, 0x00000000,
    0x00131f61, 0x61060220, 0x00343705, 0x00000000,
    0x00031f61, 0x63060220, 0x00346b05, 0x00000000,
    0x00130061, 0x65060220, 0x00346c05, 0x00000000,
    0x00030061, 0x67060220, 0x00341f05, 0x00000000,
    0x00130061, 0x69060220, 0x00342005, 0x00000000,
    0xa1290040, 0x1b0e0402, 0xaa2a0040, 0x1c0e0602,
    0x00044931, 0x21440000, 0xfb045724, 0x003c0000,
    0x00031a70, 0x2b050220, 0x52462905, 0x00440406,
    0x00131a70, 0x2c050220, 0x52462a05, 0x00440606,
    0xa04b0040, 0x0a002902, 0x00041a52, 0x2d040e68,
    0x0e2e0805, 0x2b051d05, 0x272f1a70, 0x29004b03,
    0x00030061, 0x41060220, 0x00344b05, 0x00000000,
    0x00130061, 0x43060220, 0x00344c05, 0x00000000,
    0xa0733640, 0x04004b03, 0x27330070, 0x45003103,
    0x27530070, 0x45003603, 0x00041e52, 0x4d040e68,
    0x0e2e2d05, 0x2f050c05, 0x276d2670, 0x45006b03,
    0x00031d61, 0x4f060220, 0x00347305, 0x00000000,
    0x00131e61, 0x51060220, 0x00347405, 0x00000000,
    0x27712670, 0x45001f03, 0xa0351f40, 0x47023302,
    0x80003901, 0x00000000, 0x00000000, 0x00000000,
    0xa0571f40, 0x47025302, 0x27753670, 0x4b007303,
    0x00031f61, 0x41260220, 0x00344d05, 0x00000000,
    0x00131f61, 0x43260220, 0x00344e05, 0x00000000,
    0xa06ff640, 0x47026d02, 0xa0731f40, 0x47027102,
    0x00031f61, 0x5b260220, 0x00343505, 0x00000000,
    0x00131f61, 0x5d260220, 0x00343605, 0x00000000,
    0x00031f61, 0x5f260220, 0x00345705, 0x00000000,
    0x00131f61, 0x61260220, 0x00345805, 0x00000000,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0xa07c1f40, 0x4d027502, 0x00031f61, 0x63260220,
    0x00346f05, 0x00000000, 0x00131f61, 0x65260220,
    0x00347005, 0x00000000, 0x00031f61, 0x67260220,
    0x00347305, 0x00000000, 0x00131f61, 0x69260220,
    0x00347405, 0x00000000, 0x80001f01, 0x00000000,
    0x00000000, 0x00000000, 0x00044a31, 0x29440000,
    0xfb045b24, 0x003c0000, 0x80001e01, 0x00000000,
    0x00000000, 0x00000000, 0x00044b31, 0x31440000,
    0xfb045f24, 0x003c0000, 0x00031d61, 0x4f260220,
    0x00347c05, 0x00000000, 0x00131e61, 0x51260220,
    0x00347d05, 0x00000000, 0x80001d01, 0x00000000,
    0x00000000, 0x00000000, 0x00044c31, 0x39240000,
    0xfb046324, 0x000c0000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x00044d31, 0x74140000,
    0xfb046724, 0x00040000, 0x80002901, 0x00000000,
    0x00000000, 0x00000000, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0x207a2a41, 0x25022b00,
    0x80002901, 0x00000000, 0x00000000, 0x00000000,
    0x200c2a41, 0x21022d00, 0x80002901, 0x00000000,
    0x00000000, 0x00000000, 0x201f2a41, 0x23022900,
    0x80002b01, 0x00000000, 0x00000000, 0x00000000,
    0x80003601, 0x00000000, 0x00000000, 0x00000000,
    0x20761f41, 0x2d023300, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0x20782b41, 0x35022300,
    0x20080041, 0x29023500, 0x200a2b41, 0x31022500,
    0x201b0041, 0x2b023100, 0x201d0041, 0x33022100,
    0x00042d65, 0x3d058220, 0x02467405, 0x00ffffff,
    0x0004175b, 0x01040aa8, 0x0a0a7a05, 0x23052d05,
    0x0004175b, 0x53040aa8, 0x0a0a0c05, 0x25052905,
    0x00040f5b, 0x57040aa8, 0x0a0a1f05, 0x21052b05,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0x00040a5b, 0x7d040aa8, 0x0a0a7805, 0x33052505,
    0x0004175b, 0x11040aa8, 0x0a0a0805, 0x2d053105,
    0x0004175b, 0x19040aa8, 0x0a0a0a05, 0x35052105,
    0x00041b5b, 0x7b040aa8, 0x0a0a7605, 0x2b053505,
    0x80003901, 0x00000000, 0x00000000, 0x00000000,
    0x0004175b, 0x59040aa8, 0x0a0a1b05, 0x29053305,
    0x80003a01, 0x00000000, 0x00000000, 0x00000000,
    0x0004175b, 0x5b040aa8, 0x0a0a1d05, 0x31052305,
    0x20031741, 0x01003100, 0x0004115b, 0x05040aa8,
    0x0a0a0305, 0x29057d05, 0x0004115b, 0x07040aa8,
    0x0a0a0505, 0x21057b05, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x00049e38, 0x5d050aa0,
    0x1a460705, 0x00460001, 0x20092e41, 0x5d007b00,
    0x200b1741, 0x5d001100, 0x200d1741, 0x5d005900,
    0x200f0041, 0x5d007d00, 0x20131741, 0x5d005b00,
    0x20150041, 0x5d000100, 0x20170041, 0x5d005300,
    0x20110041, 0x5d001900, 0x20190041, 0x5d005700,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x2062da41, 0x2f021300, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x205e1741, 0x2f020f00,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0x20601441, 0x2f021100, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x0004b95b, 0x68040aa8,
    0x0a2a6205, 0x0d052705, 0x80003c01, 0x00000000,
    0x00000000, 0x00000000, 0x0004135b, 0x64040aa8,
    0x0a2a5e05, 0x09052705, 0x80003c01, 0x00000000,
    0x00000000, 0x00000000, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x0004135b, 0x66040aa8,
    0x0a2a6005, 0x0b052705, 0x80003e01, 0x00000000,
    0x00000000, 0x00000000, 0x0004bb5b, 0x07040aa8,
    0x0a2a6805, 0x19053705, 0x0004135b, 0x03040aa8,
    0x0a2a6405, 0x15053705, 0x0004135b, 0x05040aa8,
    0x0a2a6605, 0x17053705, 0x80002c01, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044f31, 0x00000000,
    0xfb0c4f24, 0x003c3944, 0xa07d0040, 0x05004b03,
    0x00043f61, 0x39050660, 0x00462105, 0x00000000,
    0x00043f61, 0x3b050660, 0x00462905, 0x00000000,
    0x00043f61, 0x3d050660, 0x00463105, 0x00000000,
    0x00043f61, 0x3f050660, 0x00462305, 0x00000000,
    0x27570d70, 0x4b007d03, 0x00033f61, 0x50060220,
    0x00347d05, 0x00000000, 0x00133f61, 0x52060220,
    0x00347e05, 0x00000000, 0xa0591b40, 0x4d025702,
    0x00031961, 0x50260220, 0x00345905, 0x00000000,
    0x00131a61, 0x52260220, 0x00345a05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x00000000, 0xfb0c5024, 0x003c3944,
    0xa05a0040, 0x06004b03, 0x00043061, 0x39050660,
    0x00462b05, 0x00000000, 0x00043061, 0x3b050660,
    0x00463305, 0x00000000, 0x00043061, 0x3d050660,
    0x00462505, 0x00000000, 0x00043061, 0x3f050660,
    0x00462d05, 0x00000000, 0x275c0d70, 0x4b005a03,
    0x00033061, 0x51060220, 0x00345a05, 0x00000000,
    0x00133061, 0x53060220, 0x00345b05, 0x00000000,
    0xa05e0b40, 0x4d025c02, 0x00031961, 0x51260220,
    0x00345e05, 0x00000000, 0x00131a61, 0x53260220,
    0x00345f05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044131, 0x00000000,
    0xfb0c5124, 0x003c3944, 0xa05f1440, 0x07004b03,
    0x00040061, 0x01050660, 0x00463505, 0x00000000,
    0x27610a70, 0x4b005f03, 0x00030061, 0x58060220,
    0x00345f05, 0x00000000, 0x00130061, 0x5a060220,
    0x00346005, 0x00000000, 0xa0630a40, 0x4d026102,
    0x00031961, 0x58260220, 0x00346305, 0x00000000,
    0x00131a61, 0x5a260220, 0x00346405, 0x00000000,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x00000000, 0xfb0c5824, 0x003c0144,
    0xa0641240, 0x03404503, 0x00040069, 0x6b058660,
    0x02465505, 0x00000018, 0x27660970, 0x45006403,
    0x00033261, 0x59060220, 0x00346405, 0x00000000,
    0x00133261, 0x5b060220, 0x00346505, 0x00000000,
    0xa0680b40, 0x47026602, 0x00031961, 0x59260220,
    0x00346805, 0x00000000, 0x00131a61, 0x5b260220,
    0x00346905, 0x00000000, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044331, 0x39340000,
    0xfb045924, 0x001c0000, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x00042365, 0x69058220,
    0x02463905, 0x00ffffff, 0xa06d2340, 0x49003b02,
    0x00040061, 0x74050020, 0x0066391f, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x20011b66, 0x6b006903, 0x00043266, 0x03058220,
    0x02466905, 0x20000000, 0x276f1c70, 0x3b006d03,
    0x00041c69, 0x77058660, 0x02467405, 0x00000010,
    0x00040070, 0x00018660, 0x16464905, 0x00000000,
    0xa071b340, 0x3d026f02, 0x11043262, 0x05058220,
    0x02466d05, 0x00000000, 0x11041a62, 0x73058220,
    0x02467105, 0x00000000, 0x00041961, 0x75050120,
    0x00567306, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x20071966, 0x77007503,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x00000000, 0xfb0c4124, 0x003c0144,
    0xa0780040, 0x01004b03, 0x277a1970, 0x4b007803,
    0x00030061, 0x39060220, 0x00347805, 0x00000000,
    0x00130061, 0x3b060220, 0x00347905, 0x00000000,
    0xa07c1b40, 0x4d027a02, 0x00031961, 0x39260220,
    0x00347c05, 0x00000000, 0x00131a61, 0x3b260220,
    0x00347d05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb0c3924, 0x003c0944, 0xa07d0040, 0x02004b03,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x27011970, 0x4b007d03, 0x00033261, 0x3a060220,
    0x00347d05, 0x00000000, 0x00133261, 0x3c060220,
    0x00347e05, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0xa0031b40, 0x4d020102,
    0x00031961, 0x3a260220, 0x00340305, 0x00000000,
    0x00131a61, 0x3c260220, 0x00340405, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044531, 0x00000000, 0xfb0c3a24, 0x003c1144,
    0xa0043440, 0x03004b03, 0x00040061, 0x1b050660,
    0x00462705, 0x00000000, 0x00040061, 0x1d050660,
    0x00462f05, 0x00000000, 0x00040061, 0x1f050660,
    0x00463705, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x27061c70, 0x4b000403,
    0x00033561, 0x3b060220, 0x00340405, 0x00000000,
    0x00133561, 0x3d060220, 0x00340505, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0xa0080b40, 0x4d020602, 0x00031961, 0x3b260220,
    0x00340805, 0x00000000, 0x00131a61, 0x3d260220,
    0x00340905, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044631, 0x00000000,
    0xfb0c3b24, 0x003c1944, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x80030061, 0x7e050220,
    0x00460005, 0x00000000, 0x80040931, 0x00000004,
    0x30007e0c, 0x00000000, 0x20000060, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_build_leaf_create_HW_instance_nodes_pointers = {
   .prog_data = {
      .base.nr_params = 20,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 3,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 5344,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_build_leaf_create_HW_instance_nodes_pointers_relocs,
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
   .args = gfx125_bvh_build_leaf_create_HW_instance_nodes_pointers_args,
   .code = gfx125_bvh_build_leaf_create_HW_instance_nodes_pointers_code,
};
const char *gfx125_bvh_build_leaf_create_HW_instance_nodes_pointers_sha1 = "ca1350ebff57f4b95867321413647d635e737bf0";
