#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_misc_batched_init_globals_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_misc_batched_init_globals_args[] = {
   { 0, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g41<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(8)          g9<1>D          0D                              { align1 1Q };
mov(8)          g85<1>UD        g0.1<0,1,0>UD                   { align1 1Q };
add(1)          g42<1>UD        g41<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@3 compacted };
mul(8)          acc0<1>UD       g85<8,8,1>UD    0x0050UW        { align1 1Q I@2 };
mul(8)          g13<1>D         g85<1,1,0>D     80W             { align1 1Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(1)         g1UD            g42UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
mach(8)         g12<1>UD        g85<1,1,0>UD    0x00000050UD    { align1 1Q compacted AccWrEnable };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g10.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g10<2>F         g2<0,1,0>F                      { align1 1Q F@1 compacted };
mov(8)          g14<1>UD        g10.1<8,4,2>UD                  { align1 1Q F@1 };
add(8)          g15<1>D         g10<8,4,2>D     g13<1,1,0>D     { align1 1Q I@3 compacted };
cmp.l.f0.0(8)   g16<1>UD        g15<8,8,1>UD    g10<8,4,2>UD    { align1 1Q I@1 };
mov(8)          g86<2>UD        g15<4,4,1>UD                    { align1 1Q };
add(8)          g19<1>D         g15<1,1,0>D     16D             { align1 1Q compacted };
add(8)          g24<1>D         g15<1,1,0>D     32D             { align1 1Q compacted };
add(8)          g28<1>D         g15<1,1,0>D     48D             { align1 1Q compacted };
add(8)          g32<1>D         g15<1,1,0>D     64D             { align1 1Q compacted };
add3(8)         g17<1>D         g14<8,8,1>D     g12<8,8,1>D     -g16<1,1,1>D { align1 1Q I@6 };
cmp.l.f0.0(8)   g20<1>UD        g19<1,1,0>UD    g15<1,1,0>UD    { align1 1Q I@5 compacted };
mov(8)          g88<2>UD        g19<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g25<1>UD        g24<1,1,0>UD    g15<1,1,0>UD    { align1 1Q I@6 compacted };
mov(8)          g90<2>UD        g24<4,4,1>UD                    { align1 1Q };
mov(8)          g92<2>UD        g28<4,4,1>UD                    { align1 1Q I@7 };
cmp.l.f0.0(8)   g29<1>UD        g28<1,1,0>UD    g15<1,1,0>UD    { align1 1Q compacted };
mov(8)          g94<2>UD        g32<4,4,1>UD                    { align1 1Q I@7 };
cmp.l.f0.0(8)   g33<1>UD        g32<1,1,0>UD    g15<1,1,0>UD    { align1 1Q compacted };
mov(8)          g86.1<2>UD      g17<4,4,1>UD                    { align1 1Q I@7 };
add(8)          g22<1>D         -g20<1,1,0>D    g17<1,1,0>D     { align1 1Q I@7 compacted };
add(8)          g26<1>D         -g25<1,1,0>D    g17<1,1,0>D     { align1 1Q I@7 compacted };
add(8)          g30<1>D         -g29<1,1,0>D    g17<1,1,0>D     { align1 1Q I@6 compacted };
add(8)          g34<1>D         -g33<1,1,0>D    g17<1,1,0>D     { align1 1Q I@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
send(8)         g18UD           g86UD           nullUD          0x0440f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 4, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $1 };
mov(8)          g88.1<2>UD      g22<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g90.1<2>UD      g26<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g92.1<2>UD      g30<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g94.1<2>UD      g34<4,4,1>UD                    { align1 1Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(8)         g23UD           g88UD           nullUD          0x0440f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 4, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(8)         g27UD           g90UD           nullUD          0x0440f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 4, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g31UD           g92UD           nullUD          0x0440f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 4, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $4 };
mov(8)          g7<2>UD         g18<4,4,1>UD                    { align1 1Q $1.dst };
mov(8)          g3<2>UD         g20<4,4,1>UD                    { align1 1Q $1.dst };
add(8)          g36<1>D         g20<1,1,0>D     128D            { align1 1Q compacted };
add(8)          g39<1>D         g20<1,1,0>D     152D            { align1 1Q compacted };
mov(8)          g7.1<2>UD       g19<4,4,1>UD                    { align1 1Q @4 $1.dst };
mov(8)          g3.1<2>UD       g21<4,4,1>UD                    { align1 1Q @4 $1.dst };
mov(8)          g98<1>D         g26<8,8,1>D                     { align1 1Q $2.dst };
mov(8)          g99<1>D         g25<8,8,1>D                     { align1 1Q $2.dst };
mov(8)          g101<1>D        g24<8,8,1>D                     { align1 1Q $2.dst };
mov(8)          g5<2>UD         g36<4,4,1>UD                    { align1 1Q I@7 };
cmp.l.f0.0(8)   g37<1>UD        g36<1,1,0>UD    g20<1,1,0>UD    { align1 1Q compacted };
mov(8)          g100<1>D        g27<8,8,1>D                     { align1 1Q $3.dst };
mov(8)          g96<2>UD        g39<4,4,1>UD                    { align1 1Q I@7 };
cmp.l.f0.0(8)   g40<1>UD        g39<1,1,0>UD    0x00000098UD    { align1 1Q compacted };
add(8)          g38<1>D         -g37<1,1,0>D    g21<1,1,0>D     { align1 1Q I@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g35UD           g94UD           nullUD          0x04307582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyz, L1STATE_L3MOCS dst_len = 3, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $5 };
add(8)          g41<1>D         -g40<1,1,0>D    g21<1,1,0>D     { align1 1Q I@2 compacted };
mov(8)          g5.1<2>UD       g38<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g96.1<2>UD      g41<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g96UD           g98UD           0x0400f586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 4 flat )  base_offset 0  { align1 1Q $6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(8)          g42<1>D         g20<1,1,0>D     168D            { align1 1Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
mov(8)          g101<1>D        g32<8,8,1>D                     { align1 1Q $4.dst };
mov(8)          g102<1>D        0D                              { align1 1Q };
mov(8)          g103<1>D        0D                              { align1 1Q };
mov(8)          g104<1>D        0D                              { align1 1Q };
cmp.l.f0.0(8)   g43<1>UD        g42<1,1,0>UD    0x000000a8UD    { align1 1Q I@5 compacted };
mov(8)          g99<2>UD        g42<4,4,1>UD                    { align1 1Q $6.src };
add(8)          g44<1>D         -g43<1,1,0>D    g21<1,1,0>D     { align1 1Q I@2 compacted };
mov(8)          g99.1<2>UD      g44<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g99UD           g101UD          0x0400f586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 4 flat )  base_offset 0  { align1 1Q $7 };
add(8)          g45<1>D         g20<1,1,0>D     184D            { align1 1Q compacted };
mov(8)          g104<1>D        0D                              { align1 1Q $7.src };
mov(8)          g105<1>D        -1431586134D                    { align1 1Q };
cmp.l.f0.0(8)   g46<1>UD        g45<1,1,0>UD    0x000000b8UD    { align1 1Q I@3 compacted };
mov(8)          g102<2>UD       g45<4,4,1>UD                    { align1 1Q $7.src };
add(8)          g47<1>D         -g46<1,1,0>D    g21<1,1,0>D     { align1 1Q I@2 compacted };
mov(8)          g102.1<2>UD     g47<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g102UD          g104UD          0x04003586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1Q $8 };
shr(8)          g107<1>UD       g28<1,1,0>UD    0x00000006UD    { align1 1Q $3.dst compacted };
add(8)          g48<1>D         g20<1,1,0>D     12D             { align1 1Q compacted };
shr(8)          g108<1>UD       g29<1,1,0>UD    0x00000006UD    { align1 1Q $3.dst compacted };
shr(8)          g110<1>UD       g30<1,1,0>UD    0x00000006UD    { align1 1Q $3.dst compacted };
cmp.l.f0.0(8)   g49<1>UD        g48<1,1,0>UD    g20<1,1,0>UD    { align1 1Q I@3 compacted };
mov(8)          g105<2>UD       g48<4,4,1>UD                    { align1 1Q $8.src };
mov(8)          g109<1>D        g108<8,8,1>D                    { align1 1Q I@4 };
add(8)          g50<1>D         -g49<1,1,0>D    g21<1,1,0>D     { align1 1Q I@3 compacted };
mov(8)          g105.1<2>UD     g50<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g105UD          g107UD          0x0400f586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 4 flat )  base_offset 0  { align1 1Q $9 };
add(8)          g51<1>D         g20<1,1,0>D     28D             { align1 1Q compacted };
shr(8)          g116<1>UD       g31<1,1,0>UD    0x00000006UD    { align1 1Q $4.dst compacted };
mov(8)          g113<1>D        g110<8,8,1>D                    { align1 1Q $9.src };
mov(8)          g114<1>D        g108<8,8,1>D                    { align1 1Q $9.src };
mov(8)          g115<1>D        g108<8,8,1>D                    { align1 1Q };
cmp.l.f0.0(8)   g52<1>UD        g51<1,1,0>UD    g20<1,1,0>UD    { align1 1Q I@5 compacted };
mov(8)          g111<2>UD       g51<4,4,1>UD                    { align1 1Q };
add(8)          g53<1>D         -g52<1,1,0>D    g21<1,1,0>D     { align1 1Q I@2 compacted };
mov(8)          g111.1<2>UD     g53<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g111UD          g113UD          0x0400f586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 4 flat )  base_offset 0  { align1 1Q $10 };
shr(8)          g118<1>UD       g32<1,1,0>UD    0x00000006UD    { align1 1Q compacted };
add(8)          g54<1>D         g20<1,1,0>D     44D             { align1 1Q compacted };
mov(8)          g119<1>D        0D                              { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
mov(8)          g116<1>D        g118<8,8,1>D                    { align1 1Q I@3 };
mov(8)          g117<1>D        g118<8,8,1>D                    { align1 1Q };
cmp.l.f0.0(8)   g55<1>UD        g54<1,1,0>UD    g20<1,1,0>UD    { align1 1Q I@4 compacted };
mov(8)          g114<2>UD       g54<4,4,1>UD                    { align1 1Q $10.src };
add(8)          g56<1>D         -g55<1,1,0>D    g21<1,1,0>D     { align1 1Q I@2 compacted };
mov(8)          g114.1<2>UD     g56<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g114UD          g116UD          0x0400f586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 4 flat )  base_offset 0  { align1 1Q $11 };
add(8)          g57<1>D         g20<1,1,0>D     60D             { align1 1Q compacted };
mov(8)          g117<1>D        g28<8,8,1>D                     { align1 1Q $11.src };
mov(8)          g118<1>D        0D                              { align1 1Q $11.src };
cmp.l.f0.0(8)   g58<1>UD        g57<1,1,0>UD    g20<1,1,0>UD    { align1 1Q I@3 compacted };
mov(8)          g119<2>UD       g57<4,4,1>UD                    { align1 1Q $11.src };
add(8)          g59<1>D         -g58<1,1,0>D    g21<1,1,0>D     { align1 1Q I@2 compacted };
mov(8)          g119.1<2>UD     g59<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@5 };
send(8)         nullUD          g3UD            g117UD          0x04003586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1Q $12 };
shr(8)          g124<1>UD       g35<1,1,0>UD    0x00000006UD    { align1 1Q $5.dst compacted };
mov(8)          g121<1>D        0D                              { align1 1Q };
mov(8)          g122<1>D        0D                              { align1 1Q };
mov(8)          g123<1>D        0D                              { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g119UD          g121UD          0x0400f586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 4 flat )  base_offset 0  { align1 1Q $13 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
shr(8)          g124<1>UD       g36<1,1,0>UD    0x00000006UD    { align1 1Q $5.dst compacted };
add(8)          g60<1>D         g20<1,1,0>D     76D             { align1 1Q compacted };
shr(8)          g127<1>UD       g37<1,1,0>UD    0x00000006UD    { align1 1Q $5.dst compacted };
mov(8)          g125<1>D        0D                              { align1 1Q };
mov(8)          g126<1>D        0D                              { align1 1Q };
cmp.l.f0.0(8)   g61<1>UD        g60<1,1,0>UD    g20<1,1,0>UD    { align1 1Q I@4 compacted };
mov(8)          g122<2>UD       g60<4,4,1>UD                    { align1 1Q $13.src };
add(8)          g62<1>D         -g61<1,1,0>D    g21<1,1,0>D     { align1 1Q I@2 compacted };
mov(8)          g122.1<2>UD     g62<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g122UD          g124UD          0x0400f586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 4 flat )  base_offset 0  { align1 1Q $14 };
cmp.nz.f0.0(8)  null<1>D        g31<8,8,1>D     g32<8,8,1>D     { align1 1Q };
(+f0.0) if(8)   JIP:  LABEL0          UIP:  LABEL0              { align1 1Q };
and(8)          g63<1>UD        g31<8,8,1>UD    0xffffffc0UD    { align1 1Q };
mov(8)          g127<1>D        -64D                            { align1 1Q $14.src };
add(8)          g64<1>D         g20<1,1,0>D     g63<1,1,0>D     { align1 1Q I@2 compacted };
cmp.l.f0.0(8)   g65<1>UD        g64<1,1,0>UD    g20<1,1,0>UD    { align1 1Q I@1 compacted };
mov(8)          g125<2>UD       g64<4,4,1>UD                    { align1 1Q $14.src };
add(8)          g66<1>D         -g65<1,1,0>D    g21<1,1,0>D     { align1 1Q I@2 compacted };
mov(8)          g125.1<2>UD     g66<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g125UD          g127UD          0x04001586                0x00000040
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 1 flat )  base_offset 0  { align1 1Q $14 };

LABEL0:
endif(8)        JIP:  LABEL1                                    { align1 1Q };

LABEL1:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g1<1>D          2139095040D                     { align1 1Q };
mov(8)          g2<1>D          2139095040D                     { align1 1Q F@1 };
mov(8)          g3<1>D          2139095040D                     { align1 1Q $12.src };
mov(8)          g4<1>D          -8388608D                       { align1 1Q $12.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g5UD            g1UD            0x0400f586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 4 flat )  base_offset 0  { align1 1Q $15 };
add(8)          g67<1>D         g20<1,1,0>D     144D            { align1 1Q compacted };
mov(8)          g4<1>D          -8388608D                       { align1 1Q $15.src };
mov(8)          g5<1>D          -8388608D                       { align1 1Q $15.src };
cmp.l.f0.0(8)   g68<1>UD        g67<1,1,0>UD    0x00000090UD    { align1 1Q I@3 compacted };
mov(8)          g2<2>UD         g67<4,4,1>UD                    { align1 1Q $15.src };
add(8)          g69<1>D         -g68<1,1,0>D    g21<1,1,0>D     { align1 1Q I@2 compacted };
mov(8)          g2.1<2>UD       g69<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g2UD            g4UD            0x04003586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1Q $0 };
mov(8)          g10<1>D         2139095040D                     { align1 1Q };
mov(8)          g11<1>D         2139095040D                     { align1 1Q };
mov(8)          g12<1>D         2139095040D                     { align1 1Q };
mov(8)          g13<1>D         0D                              { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g7UD            g10UD           0x0400f586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 4 flat )  base_offset 0  { align1 1Q $1 };
add(8)          g70<1>D         g18<1,1,0>D     16D             { align1 1Q compacted };
mov(8)          g13<1>D         -8388608D                       { align1 1Q $1.src };
mov(8)          g14<1>D         -8388608D                       { align1 1Q };
mov(8)          g15<1>D         -8388608D                       { align1 1Q };
mov(8)          g16<1>D         -2147483648D                    { align1 1Q };
cmp.l.f0.0(8)   g71<1>UD        g70<1,1,0>UD    g18<1,1,0>UD    { align1 1Q I@5 compacted };
mov(8)          g11<2>UD        g70<4,4,1>UD                    { align1 1Q $1.src };
add(8)          g72<1>D         -g71<1,1,0>D    g19<1,1,0>D     { align1 1Q I@2 compacted };
mov(8)          g11.1<2>UD      g72<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g11UD           g13UD           0x0400f586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 4 flat )  base_offset 0  { align1 1Q $2 };
add(8)          g73<1>D         g18<1,1,0>D     32D             { align1 1Q compacted };
add(8)          g76<1>D         g18<1,1,0>D     72D             { align1 1Q compacted };
add(8)          g79<1>D         g18<1,1,0>D     48D             { align1 1Q compacted };
mov(8)          g35<1>D         0D                              { align1 1Q };
mov(8)          g36<1>D         0D                              { align1 1Q };
cmp.l.f0.0(8)   g74<1>UD        g73<1,1,0>UD    g18<1,1,0>UD    { align1 1Q I@5 compacted };
mov(8)          g38<2>UD        g73<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g77<1>UD        g76<1,1,0>UD    g18<1,1,0>UD    { align1 1Q I@6 compacted };
mov(8)          g16<2>UD        g76<4,4,1>UD                    { align1 1Q $2.src };
cmp.l.f0.0(8)   g80<1>UD        g79<1,1,0>UD    g18<1,1,0>UD    { align1 1Q I@7 compacted };
mov(8)          g14<2>UD        g79<4,4,1>UD                    { align1 1Q $2.src };
add(8)          g75<1>D         -g74<1,1,0>D    g19<1,1,0>D     { align1 1Q I@6 compacted };
add(8)          g78<1>D         -g77<1,1,0>D    g19<1,1,0>D     { align1 1Q I@5 compacted };
add(8)          g81<1>D         -g80<1,1,0>D    g19<1,1,0>D     { align1 1Q I@4 compacted };
mov(8)          g38.1<2>UD      g75<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g16.1<2>UD      g78<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g14.1<2>UD      g81<4,4,1>UD                    { align1 1Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g14UD           g35UD           0x04003586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1Q $3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(8)         nullUD          g16UD           g35UD           0x04003586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1Q $4 };
add(8)          g82<1>D         g18<1,1,0>D     64D             { align1 1Q compacted };
cmp.l.f0.0(8)   g83<1>UD        g82<1,1,0>UD    g18<1,1,0>UD    { align1 1Q I@1 compacted };
mov(8)          g36<2>UD        g82<4,4,1>UD                    { align1 1Q $4.src };
add(8)          g84<1>D         -g83<1,1,0>D    g19<1,1,0>D     { align1 1Q I@2 compacted };
mov(8)          g36.1<2>UD      g84<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g36UD           g9UD            0x04001586                0x00000040
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 1 flat )  base_offset 0  { align1 1Q $5 };
mov(8)          g40<1>D         g29<8,8,1>D                     { align1 1Q };
mov(8)          g41<1>D         g23<8,8,1>D                     { align1 1Q $2.dst };
mov(8)          g42<1>D         g33<8,8,1>D                     { align1 1Q $4.dst };
mov(8)          g43<1>D         g34<8,8,1>D                     { align1 1Q $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         nullUD          g38UD           g40UD           0x0400f586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 4 flat )  base_offset 0  { align1 1Q $6 };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q $14.src };
send(8)         nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1Q A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_misc_batched_init_globals_code[] = {
    0x80000065, 0x29058220, 0x02000004, 0xffffffc0,
    0x00030061, 0x09054660, 0x00000000, 0x00000000,
    0x00030061, 0x55050220, 0x00000024, 0x00000000,
    0xe22a1b40, 0x00012903, 0x00031a41, 0x20018220,
    0x01465505, 0x00500050, 0x610d0041, 0x05005502,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x80004031, 0x01140000, 0xfa002a0c, 0x00340000,
    0xfe0c0049, 0x05005503, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x210a0061, 0x001102cc,
    0x210a1161, 0x00110204, 0x00031161, 0x0e050220,
    0x00440a26, 0x00000000, 0xa10f1b40, 0x0d0e0a02,
    0x00031970, 0x10050220, 0x52460f05, 0x00440a06,
    0x00030061, 0x56060220, 0x00340f05, 0x00000000,
    0xa1130040, 0x01000f03, 0xa1180040, 0x02000f03,
    0xa11c0040, 0x03000f03, 0xa1200040, 0x04000f03,
    0x00031e52, 0x11040e68, 0x0e2e0e05, 0x10050c05,
    0x30141d70, 0x0f001303, 0x00030061, 0x58060220,
    0x00341305, 0x00000000, 0x30191e70, 0x0f001803,
    0x00030061, 0x5a060220, 0x00341805, 0x00000000,
    0x00031f61, 0x5c060220, 0x00341c05, 0x00000000,
    0x301d0070, 0x0f001c03, 0x00031f61, 0x5e060220,
    0x00342005, 0x00000000, 0x30210070, 0x0f002003,
    0x00031f61, 0x56260220, 0x00341105, 0x00000000,
    0xa1161f40, 0x11021402, 0xa11a1f40, 0x11021902,
    0xa11e1e40, 0x11021d02, 0xa1221d40, 0x11022102,
    0x80001c01, 0x00000000, 0x00000000, 0x00000000,
    0x00034131, 0x12240000, 0xfb045614, 0x003c0000,
    0x00031c61, 0x58260220, 0x00341605, 0x00000000,
    0x00031c61, 0x5a260220, 0x00341a05, 0x00000000,
    0x00031c61, 0x5c260220, 0x00341e05, 0x00000000,
    0x00031c61, 0x5e260220, 0x00342205, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00034231, 0x17240000, 0xfb045814, 0x003c0000,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00034331, 0x1b240000, 0xfb045a14, 0x003c0000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034431, 0x1f240000, 0xfb045c14, 0x003c0000,
    0x00032161, 0x07060220, 0x00341205, 0x00000000,
    0x00032161, 0x03060220, 0x00341405, 0x00000000,
    0xa1240040, 0x08001403, 0xa1270040, 0x09801403,
    0x0003c161, 0x07260220, 0x00341305, 0x00000000,
    0x0003c161, 0x03260220, 0x00341505, 0x00000000,
    0x00032261, 0x62050660, 0x00461a05, 0x00000000,
    0x00032261, 0x63050660, 0x00461905, 0x00000000,
    0x00032261, 0x65050660, 0x00461805, 0x00000000,
    0x00031f61, 0x05060220, 0x00342405, 0x00000000,
    0x30250070, 0x14002403, 0x00032361, 0x64050660,
    0x00461b05, 0x00000000, 0x00031f61, 0x60060220,
    0x00342705, 0x00000000, 0xf0280070, 0x09802703,
    0xa1261c40, 0x15022502, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00034531, 0x231c0000,
    0xfb045e14, 0x001c0000, 0xa1291a40, 0x15022802,
    0x00031a61, 0x05260220, 0x00342605, 0x00000000,
    0x00031a61, 0x60260220, 0x00342905, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034631, 0x00000000, 0xfb0c6014, 0x003c6224,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0xa12a0040, 0x0a801403, 0x80003601, 0x00000000,
    0x00000000, 0x00000000, 0x00032461, 0x65050660,
    0x00462005, 0x00000000, 0x00030061, 0x66054660,
    0x00000000, 0x00000000, 0x00030061, 0x67054660,
    0x00000000, 0x00000000, 0x00030061, 0x68054660,
    0x00000000, 0x00000000, 0xf02b1d70, 0x0a802a03,
    0x00033661, 0x63060220, 0x00342a05, 0x00000000,
    0xa12c1a40, 0x15022b02, 0x00031961, 0x63260220,
    0x00342c05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00034731, 0x00000000,
    0xfb0c6314, 0x003c6524, 0xa12d0040, 0x0b801403,
    0x00033761, 0x68054660, 0x00000000, 0x00000000,
    0x00030061, 0x69054660, 0x00000000, 0xaaabbaaa,
    0xf02e1b70, 0x0b802d03, 0x00033761, 0x66060220,
    0x00342d05, 0x00000000, 0xa12f1a40, 0x15022e02,
    0x00031961, 0x66260220, 0x00342f05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034831, 0x00000000, 0xfb0c6614, 0x000c6814,
    0xe16b2368, 0x00601c03, 0xa1300040, 0x00c01403,
    0xe16c2368, 0x00601d03, 0xe16e2368, 0x00601e03,
    0x30311b70, 0x14003003, 0x00033861, 0x69060220,
    0x00343005, 0x00000000, 0x00031c61, 0x6d050660,
    0x00466c05, 0x00000000, 0xa1321b40, 0x15023102,
    0x00031961, 0x69260220, 0x00343205, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034931, 0x00000000, 0xfb0c6914, 0x003c6b24,
    0xa1330040, 0x01c01403, 0xe1742468, 0x00601f03,
    0x00033961, 0x71050660, 0x00466e05, 0x00000000,
    0x00033961, 0x72050660, 0x00466c05, 0x00000000,
    0x00030061, 0x73050660, 0x00466c05, 0x00000000,
    0x30341d70, 0x14003303, 0x00030061, 0x6f060220,
    0x00343305, 0x00000000, 0xa1351a40, 0x15023402,
    0x00031961, 0x6f260220, 0x00343505, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034a31, 0x00000000, 0xfb0c6f14, 0x003c7124,
    0xe1760068, 0x00602003, 0xa1360040, 0x02c01403,
    0x00030061, 0x77054660, 0x00000000, 0x00000000,
    0x80003a01, 0x00000000, 0x00000000, 0x00000000,
    0x00031b61, 0x74050660, 0x00467605, 0x00000000,
    0x00030061, 0x75050660, 0x00467605, 0x00000000,
    0x30371c70, 0x14003603, 0x00033a61, 0x72060220,
    0x00343605, 0x00000000, 0xa1381a40, 0x15023702,
    0x00031961, 0x72260220, 0x00343805, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034b31, 0x00000000, 0xfb0c7214, 0x003c7424,
    0xa1390040, 0x03c01403, 0x00033b61, 0x75050660,
    0x00461c05, 0x00000000, 0x00033b61, 0x76054660,
    0x00000000, 0x00000000, 0x303a1b70, 0x14003903,
    0x00033b61, 0x77060220, 0x00343905, 0x00000000,
    0xa13b1a40, 0x15023a02, 0x00031961, 0x77260220,
    0x00343b05, 0x00000000, 0x80001d01, 0x00000000,
    0x00000000, 0x00000000, 0x00034c31, 0x00000000,
    0xfb0c0314, 0x000c7514, 0xe17c2568, 0x00602303,
    0x00030061, 0x79054660, 0x00000000, 0x00000000,
    0x00030061, 0x7a054660, 0x00000000, 0x00000000,
    0x00030061, 0x7b054660, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034d31, 0x00000000, 0xfb0c7714, 0x003c7924,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0xe17c2568, 0x00602403, 0xa13c0040, 0x04c01403,
    0xe17f2568, 0x00602503, 0x00030061, 0x7d054660,
    0x00000000, 0x00000000, 0x00030061, 0x7e054660,
    0x00000000, 0x00000000, 0x303d1c70, 0x14003c03,
    0x00033d61, 0x7a060220, 0x00343c05, 0x00000000,
    0xa13e1a40, 0x15023d02, 0x00031961, 0x7a260220,
    0x00343e05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00034e31, 0x00000000,
    0xfb0c7a14, 0x003c7c24, 0x00030070, 0x00010660,
    0x26461f05, 0x00462005, 0x01030022, 0x0001c060,
    0x00000088, 0x00000088, 0x00030065, 0x3f058220,
    0x02461f05, 0xffffffc0, 0x00033e61, 0x7f054660,
    0x00000000, 0xffffffc0, 0xa1401a40, 0x3f001402,
    0x30411970, 0x14004003, 0x00033e61, 0x7d060220,
    0x00344005, 0x00000000, 0xa1421a40, 0x15024102,
    0x00031961, 0x7d260220, 0x00344205, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034e31, 0x00000000, 0xfb0c7d14, 0x00047f0c,
    0x00030025, 0x00004600, 0x00000000, 0x00000010,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x01054660, 0x00000000, 0x7f800000,
    0x00031161, 0x02054660, 0x00000000, 0x7f800000,
    0x00033c61, 0x03054660, 0x00000000, 0x7f800000,
    0x00033c61, 0x04054660, 0x00000000, 0xff800000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034f31, 0x00000000, 0xfb0c0514, 0x003c0124,
    0xa1430040, 0x09001403, 0x00033f61, 0x04054660,
    0x00000000, 0xff800000, 0x00033f61, 0x05054660,
    0x00000000, 0xff800000, 0xf0441b70, 0x09004303,
    0x00033f61, 0x02060220, 0x00344305, 0x00000000,
    0xa1451a40, 0x15024402, 0x00031961, 0x02260220,
    0x00344505, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00034031, 0x00000000,
    0xfb0c0214, 0x000c0414, 0x00030061, 0x0a054660,
    0x00000000, 0x7f800000, 0x00030061, 0x0b054660,
    0x00000000, 0x7f800000, 0x00030061, 0x0c054660,
    0x00000000, 0x7f800000, 0x00030061, 0x0d054660,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00034131, 0x00000000,
    0xfb0c0714, 0x003c0a24, 0xa1460040, 0x01001203,
    0x00033161, 0x0d054660, 0x00000000, 0xff800000,
    0x00030061, 0x0e054660, 0x00000000, 0xff800000,
    0x00030061, 0x0f054660, 0x00000000, 0xff800000,
    0x00030061, 0x10054660, 0x00000000, 0x80000000,
    0x30471d70, 0x12004603, 0x00033161, 0x0b060220,
    0x00344605, 0x00000000, 0xa1481a40, 0x13024702,
    0x00031961, 0x0b260220, 0x00344805, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034231, 0x00000000, 0xfb0c0b14, 0x003c0d24,
    0xa1490040, 0x02001203, 0xa14c0040, 0x04801203,
    0xa14f0040, 0x03001203, 0x00030061, 0x23054660,
    0x00000000, 0x00000000, 0x00030061, 0x24054660,
    0x00000000, 0x00000000, 0x304a1d70, 0x12004903,
    0x00030061, 0x26060220, 0x00344905, 0x00000000,
    0x304d1e70, 0x12004c03, 0x00033261, 0x10060220,
    0x00344c05, 0x00000000, 0x30501f70, 0x12004f03,
    0x00033261, 0x0e060220, 0x00344f05, 0x00000000,
    0xa14b1e40, 0x13024a02, 0xa14e1d40, 0x13024d02,
    0xa1511c40, 0x13025002, 0x00031b61, 0x26260220,
    0x00344b05, 0x00000000, 0x00031b61, 0x10260220,
    0x00344e05, 0x00000000, 0x00031b61, 0x0e260220,
    0x00345105, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00034331, 0x00000000,
    0xfb0c0e14, 0x000c2314, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x00034431, 0x00000000,
    0xfb0c1014, 0x000c2314, 0xa1520040, 0x04001203,
    0x30531970, 0x12005203, 0x00033461, 0x24060220,
    0x00345205, 0x00000000, 0xa1541a40, 0x13025302,
    0x00031961, 0x24260220, 0x00345405, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034531, 0x00000000, 0xfb0c2414, 0x0004090c,
    0x00030061, 0x28050660, 0x00461d05, 0x00000000,
    0x00032261, 0x29050660, 0x00461705, 0x00000000,
    0x00032461, 0x2a050660, 0x00462105, 0x00000000,
    0x00032461, 0x2b050660, 0x00462205, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034631, 0x00000000, 0xfb0c2614, 0x003c2824,
    0x80033e61, 0x7e050220, 0x00460005, 0x00000000,
    0x80030931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_misc_batched_init_globals = {
   .prog_data = {
      .base.nr_params = 10,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 3136,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_misc_batched_init_globals_relocs,
      .base.uses_atomic_load_store = false,
      .local_size = { 1, 1, 1 },
      .prog_offset = { 0, 0, 0 },
      .prog_mask = 1,
      .prog_spilled = 0,
      .uses_barrier = false,
      .uses_num_work_groups = false,
      .push.per_thread.dwords = 0,
      .push.per_thread.regs = 0,
      .push.per_thread.size = 0,
      .push.cross_thread.dwords = 10,
      .push.cross_thread.regs = 2,
      .push.cross_thread.size = 64,
   },
   .args_size = 8,
   .arg_count = 1,
   .args = gfx125_misc_batched_init_globals_args,
   .code = gfx125_misc_batched_init_globals_code,
};
const char *gfx125_misc_batched_init_globals_sha1 = "d3d2be8ec322a13e38960e1b4d334b7c9a3de7da";
