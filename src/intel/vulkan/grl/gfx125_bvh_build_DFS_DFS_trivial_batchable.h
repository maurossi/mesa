#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_build_DFS_DFS_trivial_batchable_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_build_DFS_DFS_trivial_batchable_args[] = {
   { 0, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g126<1>UD       g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g28<1>UD        g0.1<0,1,0>UD                   { align1 1H };
add(1)          g127<1>UD       g126<0,1,0>UD   0x00000000UD    { align1 WE_all 1N I@2 compacted };
mov(8)          g62<1>UW        0x76543210V                     { align1 WE_all 1Q };
mul(8)          acc0<1>UD       g28<8,8,1>UD    0x0058UW        { align1 1Q I@3 };
mul(16)         g95<1>D         g28<1,1,0>D     88W             { align1 1H I@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
send(1)         g1UD            g127UD          nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
add(8)          g62.8<1>UW      g62<1,1,0>UW    0x0008UW        { align1 WE_all 1Q I@3 compacted };
mach(8)         g93<1>UD        g28<1,1,0>UD    0x00000058UD    { align1 1Q compacted AccWrEnable };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
mov(16)         g67<1>D         g62<8,8,1>UW                    { align1 1H };
mul(8)          acc0<1>UD       g29<8,8,1>UD    0x0058UW        { align1 2Q };
and(16)         g30<1>UD        g67<1,1,0>UD    0x0000000fUD    { align1 1H I@2 compacted };
mach(8)         g94<1>UD        g29<8,8,1>UD    0x00000058UD    { align1 2Q AccWrEnable };
mov(16)         g123<2>UW       g30<8,8,1>UD                    { align1 1H I@2 };
mov(16)         g105<1>UD       g30<16,8,2>UW                   { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g34.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g91.1<2>F       g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g34<2>F         g2<0,1,0>F                      { align1 1Q F@2 compacted };
mov(8)          g91<2>F         g2<0,1,0>F                      { align1 2Q F@2 compacted };
mov(8)          g97<1>UD        g34.1<8,4,2>UD                  { align1 1Q F@2 };
add(8)          g119<1>D        g34<8,4,2>D     g95<1,1,0>D     { align1 1Q I@7 compacted };
mov(8)          g98<1>UD        g91.1<8,4,2>UD                  { align1 2Q F@1 };
add(8)          g120<1>D        g91<8,4,2>D     g96<1,1,0>D     { align1 2Q compacted };
cmp.l.f0.0(8)   g125<1>UD       g119<8,8,1>UD   g34<8,4,2>UD    { align1 1Q I@3 };
mov(8)          g111<2>UD       g119<4,4,1>UD                   { align1 1Q };
cmp.l.f0.0(8)   g126<1>UD       g120<8,8,1>UD   g91<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g113<2>UD       g120<4,4,1>UD                   { align1 2Q };
add(16)         g10<1>D         g119<1,1,0>D    32D             { align1 1H compacted };
add(16)         g15<1>D         g119<1,1,0>D    60D             { align1 1H compacted };
add(16)         g20<1>D         g119<1,1,0>D    48D             { align1 1H compacted };
cmp.l.f0.0(16)  g12<1>UD        g10<1,1,0>UD    g119<1,1,0>UD   { align1 1H I@3 compacted };
mov(8)          g100<2>UD       g10<4,4,1>UD                    { align1 1Q };
mov(8)          g102<2>UD       g11<4,4,1>UD                    { align1 2Q };
cmp.l.f0.0(16)  g17<1>UD        g15<1,1,0>UD    g119<1,1,0>UD   { align1 1H I@5 compacted };
mov(8)          g107<2>UD       g15<4,4,1>UD                    { align1 1Q };
mov(8)          g109<2>UD       g16<4,4,1>UD                    { align1 2Q };
add3(16)        g9<1>D          g97<8,8,1>D     g93<8,8,1>D     -g125<1,1,1>D { align1 1H };
mov(8)          g115<2>UD       g20<4,4,1>UD                    { align1 1Q I@7 };
mov(8)          g117<2>UD       g21<4,4,1>UD                    { align1 2Q I@7 };
cmp.l.f0.0(16)  g22<1>UD        g20<1,1,0>UD    g119<1,1,0>UD   { align1 1H compacted };
add(16)         g19<1>D         -g17<1,1,0>D    g9<1,1,0>D      { align1 1H I@4 compacted };
mov(8)          g111.1<2>UD     g9<4,4,1>UD                     { align1 1Q };
mov(8)          g113.1<2>UD     g10<4,4,1>UD                    { align1 2Q };
add(16)         g14<1>D         -g12<1,1,0>D    g9<1,1,0>D      { align1 1H compacted };
add(16)         g24<1>D         -g22<1,1,0>D    g9<1,1,0>D      { align1 1H I@5 compacted };
mov(8)          g107.1<2>UD     g19<4,4,1>UD                    { align1 1Q I@5 };
mov(8)          g109.1<2>UD     g20<4,4,1>UD                    { align1 2Q I@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        g1UD            g111UD          nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
mov(8)          g100.1<2>UD     g14<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g102.1<2>UD     g15<4,4,1>UD                    { align1 2Q I@5 };
mov(8)          g115.1<2>UD     g24<4,4,1>UD                    { align1 1Q I@5 };
mov(8)          g117.1<2>UD     g25<4,4,1>UD                    { align1 2Q I@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@5 };
send(16)        g61UD           g107UD          nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g63UD           g100UD          nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g25UD           g115UD          nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
mov(16)         g121<2>UW       g61<8,8,1>UD                    { align1 1H $2.dst };
and(16)         g29<1>UD        g25<1,1,0>UD    0x00000001UD    { align1 1H $4.dst compacted };
mov(16)         g69<1>UD        g27<16,8,2>UW                   { align1 1H $4.dst };
cmp.nz.f0.0(16) g71<1>D         g29<1,1,0>D     0D              { align1 1H I@2 compacted };
cmp.l.f0.0(16)  null<1>D        g105<8,8,1>D    g69<8,8,1>D     { align1 1H I@2 };
(+f0.0) if(16)  JIP:  LABEL1          UIP:  LABEL0              { align1 1H };
shl(16)         g31<1>D         g105<8,8,1>D    0x00000005UD    { align1 1H };
shr(16)         g33<1>UD        g105<1,1,0>UD   0x0000001bUD    { align1 1H compacted };
add(16)         g35<1>D         g5<1,1,0>D      g31<1,1,0>D     { align1 1H @2 $1.dst compacted };
cmp.l.f0.0(16)  g37<1>UD        g35<1,1,0>UD    g5<1,1,0>UD     { align1 1H I@1 compacted };
mov(8)          g47<2>UD        g35<4,4,1>UD                    { align1 1Q };
mov(8)          g49<2>UD        g36<4,4,1>UD                    { align1 2Q };
add(16)         g41<1>D         g35<1,1,0>D     16D             { align1 1H compacted };
add3(16)        g39<1>D         g7<8,8,1>D      g33<8,8,1>D     -g37<1,1,1>D { align1 1H @4 $1.dst };
cmp.l.f0.0(16)  g43<1>UD        g41<1,1,0>UD    g35<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g116<2>UD       g41<4,4,1>UD                    { align1 1Q $4.src };
mov(8)          g118<2>UD       g42<4,4,1>UD                    { align1 2Q $4.src };
mov(8)          g47.1<2>UD      g39<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g49.1<2>UD      g40<4,4,1>UD                    { align1 2Q I@5 };
add(16)         g45<1>D         -g43<1,1,0>D    g39<1,1,0>D     { align1 1H I@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g33UD           g47UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
mov(8)          g116.1<2>UD     g45<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g118.1<2>UD     g46<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g41UD           g116UD          nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
else(16)        JIP:  LABEL0          UIP:  LABEL0              { align1 1H };

LABEL1:
mov(16)         g39<1>UD        0x00000000UD                    { align1 1H $4.dst };
mov(16)         g37<1>UD        0x7f800000UD                    { align1 1H $4.dst };
mov(16)         g35<1>UD        0x7f800000UD                    { align1 1H $4.dst };
mov(16)         g33<1>UD        0x7f800000UD                    { align1 1H $4.dst };
mov(16)         g47<1>UD        0x80000000UD                    { align1 1H $4.dst };
mov(16)         g45<1>UD        0xff800000UD                    { align1 1H $4.dst };
mov(16)         g43<1>UD        0xff800000UD                    { align1 1H $4.dst };
mov(16)         g41<1>UD        0xff800000UD                    { align1 1H $4.dst };

LABEL0:
endif(16)       JIP:  LABEL2                                    { align1 1H };

LABEL2:
cmp.z.f0.0(16)  g49<1>W         g123<16,8,2>W   0W              { align1 1H $4.src };
mov.nz.f0.0(16) g73<1>D         g49<8,8,1>W                     { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL3          UIP:  LABEL3              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add(16)         g50<1>D         g63<1,1,0>D     12D             { align1 1H $3.dst compacted };
cmp.l.f0.0(16)  g52<1>UD        g50<1,1,0>UD    g63<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g57<2>UD        g50<4,4,1>UD                    { align1 1Q };
mov(8)          g59<2>UD        g51<4,4,1>UD                    { align1 2Q };
add(16)         g54<1>D         -g52<1,1,0>D    g65<1,1,0>D     { align1 1H @3 $3.dst compacted };
mov(8)          g57.1<2>UD      g54<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g59.1<2>UD      g55<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g55UD           g57UD           nullUD          0x08240588                0x00000000
                            ugm MsgDesc: ( atomic_inc, a64, d32, V1, L1UC_L3WB dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $5 };
shl(16)         g49<1>D         g55<8,8,1>D     0x00000006UD    { align1 1H $5.dst };

LABEL3:
endif(16)       JIP:  LABEL4                                    { align1 1H };

LABEL4:
add(16)         g75<1>D         g63<1,1,0>D     g49<0,1,0>D     { align1 1H @2 $3.dst compacted };
add(16)         g58<1>D         g63<1,1,0>D     16D             { align1 1H $5.src compacted };
add(16)         g87<1>D         g67<1,1,0>D     4D              { align1 1H compacted };
add(16)         g89<1>D         g67<1,1,0>D     -12D            { align1 1H compacted };
add(16)         g91<1>D         g67<1,1,0>D     2D              { align1 1H compacted };
add(16)         g93<1>D         g67<1,1,0>D     -14D            { align1 1H compacted };
add(16)         g95<1>D         g67<1,1,0>D     1D              { align1 1H compacted };
add(16)         g97<1>D         g67<1,1,0>D     -15D            { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
cmp.l.f0.0(16)  g56<1>UD        g75<1,1,0>UD    g63<1,1,0>UD    { align1 1H I@7 compacted };
mov(8)          g21<2>UD        g75<4,4,1>UD                    { align1 1Q };
mov(8)          g23<2>UD        g76<4,4,1>UD                    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g117<2>UD       g58<4,4,1>UD                    { align1 1Q I@7 };
mov(8)          g119<2>UD       g59<4,4,1>UD                    { align1 2Q $4.src };
cmp.l.f0.0(16)  g79<1>UD        g58<1,1,0>UD    g63<1,1,0>UD    { align1 1H compacted };
cmp.l.f0.0(16)  null<1>D        g87<8,8,1>D     16D             { align1 1H };
add(16)         g77<1>D         -g56<1,1,0>D    g65<1,1,0>D     { align1 1H @7 $3.dst compacted };
add(16)         g81<1>D         -g79<1,1,0>D    g65<1,1,0>D     { align1 1H I@3 compacted };
(+f0.0) sel(16) g25<1>UD        g87<1,1,0>UD    g89<1,1,0>UD    { align1 1H compacted };
mov(8)          g21.1<2>UD      g77<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g23.1<2>UD      g78<4,4,1>UD                    { align1 2Q I@4 };
mov(8)          g117.1<2>UD     g81<4,4,1>UD                    { align1 1Q I@4 };
mov(8)          g119.1<2>UD     g82<4,4,1>UD                    { align1 2Q I@5 };
cmp.l.f0.0(16)  null<1>D        g91<8,8,1>D     16D             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g82UD           g117UD          nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $6 };
(+f0.0) sel(16) g17<1>UD        g91<1,1,0>UD    g93<1,1,0>UD    { align1 1H compacted };
cmp.l.f0.0(16)  null<1>D        g95<8,8,1>D     16D             { align1 1H };
(+f0.0) sel(16) g19<1>UD        g95<1,1,0>UD    g97<1,1,0>UD    { align1 1H compacted };
cmp.z.f0.0(16)  g98<1>W         g121<16,8,2>W   1W              { align1 1H };
mov.nz.f0.0(16) g27<1>D         g98<8,8,1>W                     { align1 1H I@1 };
shl(16)         g84<1>D         g82<8,8,1>D     0x00000006UD    { align1 1H $6.dst };
add3(16)        g86<1>D         g63<8,8,1>D     g84<8,8,1>D     -g75<1,1,1>D { align1 1H I@1 };
asr(16)         g15<1>D         g86<8,8,1>D     0x00000006UD    { align1 1H I@1 };
(-f0.0) if(16)  JIP:  LABEL6          UIP:  LABEL5              { align1 1H };
cmp.l.f0.0(16)  null<1>UD       g67<8,8,1>UD    g69<8,8,1>UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
(+f0.0) sel(16) g99<1>UD        g41<8,8,1>UD    0xff800000UD    { align1 1H $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
(+f0.0) sel(16) g101<1>UD       g43<8,8,1>UD    0xff800000UD    { align1 1H $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
(+f0.0) sel(16) g103<1>UD       g45<8,8,1>UD    0xff800000UD    { align1 1H $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
(+f0.0) sel(16) g109<1>UD       g33<8,8,1>UD    0x7f800000UD    { align1 1H $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
(+f0.0) sel(16) g111<1>UD       g35<8,8,1>UD    0x7f800000UD    { align1 1H $4.dst };
(+f0.0) sel(16) g92<1>UD        g37<8,8,1>UD    0x7f800000UD    { align1 1H $4.dst };
mov(16)         a0<1>UW         0x0c60UW                        { align1 WE_all 1H I@6 };
shl(16)         a0<1>UW         g25<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0c60UW        { align1 1H A@1 };
mov(16)         g53<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0ca0UW                        { align1 WE_all 1H I@6 };
shl(16)         a0<1>UW         g25<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0ca0UW        { align1 1H A@1 };
mov(16)         g55<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0ce0UW                        { align1 WE_all 1H I@6 };
shl(16)         a0<1>UW         g25<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0ce0UW        { align1 1H A@1 };
mov(16)         g30<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(16)         a0<1>UW         0x0da0UW                        { align1 WE_all 1H I@6 };
shl(16)         a0<1>UW         g25<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0da0UW        { align1 1H A@1 };
mov(16)         g113<1>UD       g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0de0UW                        { align1 WE_all 1H I@6 };
shl(16)         a0<1>UW         g25<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0de0UW        { align1 1H A@1 };
mov(16)         g94<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0b80UW                        { align1 WE_all 1H I@6 };
shl(16)         a0<1>UW         g25<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0b80UW        { align1 1H A@1 };
mov(16)         g96<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
cmp.l.f0.0(16)  null<1>UD       g67<8,8,1>UD    0x00000006UD    { align1 1H };
sel.ge(16)      g51<1>F         g99<1,1,0>F     g53<1,1,0>F     { align1 1H I@7 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sel.ge(16)      g107<1>F        g103<1,1,0>F    g30<1,1,0>F     { align1 1H I@5 compacted };
sel.l(16)       g98<1>F         g109<1,1,0>F    g113<1,1,0>F    { align1 1H A@2 compacted };
sel.ge(16)      g53<1>F         g101<1,1,0>F    g55<1,1,0>F     { align1 1H I@6 compacted };
sel.l(16)       g100<1>F        g111<1,1,0>F    g94<1,1,0>F     { align1 1H A@1 compacted };
sel.l(16)       g102<1>F        g92<1,1,0>F     g96<1,1,0>F     { align1 1H A@2 compacted };
mov(16)         a0<1>UW         0x0660UW                        { align1 WE_all 1H F@6 };
shl(16)         a0<1>UW         g17<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0660UW        { align1 1H A@1 };
mov(16)         g57<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0d60UW                        { align1 WE_all 1H F@3 };
shl(16)         a0<1>UW         g17<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0d60UW        { align1 1H A@1 };
mov(16)         g55<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0c40UW                        { align1 WE_all 1H F@4 };
shl(16)         a0<1>UW         g17<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0c40UW        { align1 1H A@1 };
mov(16)         g121<1>UD       g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(16)         a0<1>UW         0x06a0UW                        { align1 WE_all 1H F@3 };
shl(16)         a0<1>UW         g17<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x06a0UW        { align1 1H A@1 };
mov(16)         g59<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(16)         a0<1>UW         0x0c80UW                        { align1 WE_all 1H F@2 };
shl(16)         a0<1>UW         g17<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0c80UW        { align1 1H A@1 };
mov(16)         g115<1>UD       g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
mov(16)         a0<1>UW         0x0cc0UW                        { align1 WE_all 1H F@1 };
shl(16)         a0<1>UW         g17<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0cc0UW        { align1 1H A@1 };
mov(16)         g117<1>UD       g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sel.ge(16)      g109<1>F        g51<1,1,0>F     g57<1,1,0>F     { align1 1H A@4 compacted };
sel.ge(16)      g113<1>F        g107<1,1,0>F    g55<1,1,0>F     { align1 1H I@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
sel.l(16)       g119<1>F        g98<1,1,0>F     g121<1,1,0>F    { align1 1H I@4 compacted };
sel.ge(16)      g111<1>F        g53<1,1,0>F     g59<1,1,0>F     { align1 1H A@3 compacted };
sel.l(16)       g121<1>F        g100<1,1,0>F    g115<1,1,0>F    { align1 1H I@2 compacted };
sel.l(16)       g123<1>F        g102<1,1,0>F    g117<1,1,0>F    { align1 1H I@1 compacted };
mov(16)         a0<1>UW         0x0da0UW                        { align1 WE_all 1H F@6 };
shl(16)         a0<1>UW         g19<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0da0UW        { align1 1H A@1 };
mov(16)         g79<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0e20UW                        { align1 WE_all 1H F@5 };
shl(16)         a0<1>UW         g19<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0e20UW        { align1 1H A@1 };
mov(16)         g56<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0ee0UW                        { align1 WE_all 1H F@4 };
shl(16)         a0<1>UW         g19<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0ee0UW        { align1 1H A@1 };
mov(16)         g31<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0de0UW                        { align1 WE_all 1H F@3 };
shl(16)         a0<1>UW         g19<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0de0UW        { align1 1H A@1 };
mov(16)         g81<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0f20UW                        { align1 WE_all 1H F@2 };
shl(16)         a0<1>UW         g19<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0f20UW        { align1 1H A@1 };
mov(16)         g125<1>UD       g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0f60UW                        { align1 WE_all 1H F@1 };
shl(16)         a0<1>UW         g19<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0f60UW        { align1 1H A@1 };
mov(16)         g29<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sel.ge(16)      g85<1>F         g109<1,1,0>F    g79<1,1,0>F     { align1 1H I@6 compacted };
sel.ge(16)      g89<1>F         g113<1,1,0>F    g56<1,1,0>F     { align1 1H I@5 compacted };
sel.l(16)       g79<1>F         g119<1,1,0>F    g31<1,1,0>F     { align1 1H I@4 compacted };
sel.ge(16)      g87<1>F         g111<1,1,0>F    g81<1,1,0>F     { align1 1H I@3 compacted };
sel.l(16)       g83<1>F         g123<1,1,0>F    g29<1,1,0>F     { align1 1H I@1 compacted };
sel.l(16)       g81<1>F         g121<1,1,0>F    g125<1,1,0>F    { align1 1H I@2 compacted };
(+f0.0) if(16)  JIP:  LABEL7          UIP:  LABEL7              { align1 1H };
sel.ge(16)      g91<1>F         (abs)g79<0,1,0>F (abs)g85<0,1,0>F { align1 1H F@4 compacted };
sel.ge(16)      g93<1>F         (abs)g81<0,1,0>F (abs)g87<0,1,0>F { align1 1H F@2 compacted };
sel.ge(16)      g95<1>F         (abs)g83<0,1,0>F (abs)g89<0,1,0>F { align1 1H F@4 compacted };
add(16)         g55<1>D         g75<1,1,0>D     16D             { align1 1H F@7 compacted };
mov(16)         g104<2>B        -1W                             { align1 1H };
sel.ge(16)      g97<1>F         g93<1,1,0>F     g95<1,1,0>F     { align1 1H F@1 compacted };
mov(16)         g59<4>UB        g61<8,8,1>UD                    { align1 1H };
mov(8)          g51<2>UD        g55<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g53<2>UD        g56<4,4,1>UD                    { align1 2Q I@4 };
sel.ge(16)      g93<1>F         g91<1,1,0>F     g97<1,1,0>F     { align1 1H F@1 compacted };
mov(16)         g57<1>UD        g59<32,8,4>UB                   { align1 1H A@3 };
mul(16)         g95<1>F         g93<1,1,0>F     0x34000000F  /* 1.19209e-07F */ { align1 1H F@1 compacted };
add(16)         g9<1>F          g79<0,1,0>F     -g95<1,1,0>F    { align1 1H F@1 compacted };
add(16)         g11<1>F         g81<0,1,0>F     -g95<1,1,0>F    { align1 1H compacted };
add(16)         g13<1>F         g83<0,1,0>F     -g95<1,1,0>F    { align1 1H compacted };
add(16)         g97<1>F         g85<0,1,0>F     g95<1,1,0>F     { align1 1H compacted };
add(16)         g99<1>F         g87<0,1,0>F     g95<1,1,0>F     { align1 1H compacted };
add(16)         g101<1>F        g89<0,1,0>F     g95<1,1,0>F     { align1 1H compacted };
add(16)         g107<1>F        g97<1,1,0>F     -g9<1,1,0>F     { align1 1H F@3 compacted };
add(16)         g109<1>F        g99<1,1,0>F     -g11<1,1,0>F    { align1 1H F@3 compacted };
add(16)         g111<1>F        g101<1,1,0>F    -g13<1,1,0>F    { align1 1H A@3 compacted };
mul(16)         g113<1>F        g107<8,8,1>F    0x3f800001F  /* 1F */ { align1 1H F@3 };
mul(16)         g59<1>F         g109<8,8,1>F    0x3f800001F  /* 1F */ { align1 1H A@1 };
mul(16)         g123<1>F        g111<8,8,1>F    0x3f800001F  /* 1F */ { align1 1H A@3 };
and(16)         g111<1>UD       g113<8,8,1>UD   0x80000000UD    { align1 1H F@1 };
and(16)         g98<1>UD        g113<8,8,1>UD   0x007fffffUD    { align1 1H F@5 };
and(16)         g117<1>UD       g113<8,8,1>UD   0x7f800000UD    { align1 1H };
and(16)         g100<1>UD       g59<8,8,1>UD    0x007fffffUD    { align1 1H F@2 };
and(16)         g119<1>UD       g59<8,8,1>UD    0x7f800000UD    { align1 1H };
and(16)         g102<1>UD       g123<8,8,1>UD   0x007fffffUD    { align1 1H F@1 };
and(16)         g115<1>UD       g123<8,8,1>UD   0x80000000UD    { align1 1H };
and(16)         g121<1>UD       g123<8,8,1>UD   0x7f800000UD    { align1 1H };
and(16)         g113<1>UD       g59<8,8,1>UD    0x80000000UD    { align1 1H };
add(16)         g107<1>D        g98<8,8,1>D     1056964608D     { align1 1H A@3 };
add(16)         g109<1>D        g100<8,8,1>D    1056964608D     { align1 1H A@2 };
asr(16)         g123<1>D        g117<8,8,1>D    0x00000017UD    { align1 1H I@7 };
asr(16)         g125<1>D        g119<8,8,1>D    0x00000017UD    { align1 1H I@7 };
add(16)         g29<1>D         g102<8,8,1>D    1056964608D     { align1 1H I@7 };
asr(16)         g31<1>D         g121<8,8,1>D    0x00000017UD    { align1 1H I@7 };
cmp.z.f0.0(16)  null<1>F        g107<1,1,0>F    0x3f800000F  /* 1F */ { align1 1H I@6 compacted };
(-f0.0) sel(16) g60<1>UD        g107<8,8,1>UD   0x3f000000UD    { align1 1H };
and(16)         g79<1>UD        g60<8,8,1>UD    0x7fffffffUD    { align1 1H I@1 };
cmp.z.f0.0(16)  null<1>F        g109<1,1,0>F    0x3f800000F  /* 1F */ { align1 1H I@7 compacted };
or(16)          g81<1>UD        g79<1,1,0>UD    g111<1,1,0>UD   { align1 1H I@1 compacted };
(-f0.0) sel(16) g117<1>UD       g109<8,8,1>UD   0x3f000000UD    { align1 1H };
and(16)         g119<1>UD       g117<8,8,1>UD   0x7fffffffUD    { align1 1H I@1 };
cmp.z.f0.0(16)  null<1>F        g29<1,1,0>F     0x3f800000F  /* 1F */ { align1 1H I@7 compacted };
or(16)          g83<1>UD        g119<1,1,0>UD   g113<1,1,0>UD   { align1 1H I@1 compacted };
(-f0.0) sel(16) g118<1>UD       g29<8,8,1>UD    0x3f000000UD    { align1 1H };
and(16)         g120<1>UD       g118<8,8,1>UD   0x7fffffffUD    { align1 1H I@1 };
cmp.g.f0.0(16)  g59<1>F         g81<8,8,1>F     0x3f7f0000F  /* 0.996094F */ { align1 1H I@6 };
cmp.g.f0.0(16)  g61<1>F         g83<8,8,1>F     0x3f7f0000F  /* 0.996094F */ { align1 1H I@3 };
or(16)          g29<1>UD        g120<1,1,0>UD   g115<1,1,0>UD   { align1 1H A@1 compacted };
add3(16)        g84<1>D         65410W          g123<8,8,1>D    -g59<1,1,1>D { align1 1H F@1 };
add3(16)        g86<1>D         65410W          g125<8,8,1>D    -g61<1,1,1>D { align1 1H F@1 };
cmp.g.f0.0(16)  g79<1>F         g29<8,8,1>F     0x3f7f0000F  /* 0.996094F */ { align1 1H I@3 };
cmp.l.f0.0(16)  g81<1>UD        g55<1,1,0>UD    g75<1,1,0>UD    { align1 1H F@3 compacted };
add3(16)        g88<1>D         65410W          g31<8,8,1>D     -g79<1,1,1>D { align1 1H F@1 };
add(16)         g90<1>D         -g81<1,1,0>D    g77<1,1,0>D     { align1 1H I@2 compacted };
mov(8)          g51.1<2>UD      g90<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g53.1<2>UD      g91<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g51UD           g57UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $7 };
send(16)        nullUD          g21UD           g9UD            0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $4 };
add(16)         g91<1>D         g75<1,1,0>D     18D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
mov(16)         g52<4>UB        g84<8,8,1>UD                    { align1 1H I@7 };
cmp.l.f0.0(16)  g93<1>UD        g91<1,1,0>UD    g75<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g54<2>UD        g91<4,4,1>UD                    { align1 1Q $7.src };
mov(8)          g56<2>UD        g92<4,4,1>UD                    { align1 2Q $7.src };
mov(16)         g60<4>UB        g86<8,8,1>UD                    { align1 1H };
mov(16)         g79<4>UB        g88<8,8,1>UD                    { align1 1H I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
mov(16)         g58<4>UB        g52<32,8,4>UB                   { align1 1H I@6 };
add(16)         g95<1>D         -g93<1,1,0>D    g77<1,1,0>D     { align1 1H I@6 compacted };
mov(16)         g58.1<4>UB      g60<32,8,4>UB                   { align1 1H I@2 };
mov(8)          g54.1<2>UD      g95<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g56.1<2>UD      g96<4,4,1>UD                    { align1 2Q I@3 };
mov(16)         g58.2<4>UB      g79<32,8,4>UB                   { align1 1H I@3 };
mov(16)         g58.3<4>UB      g104<16,8,2>UB                  { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g54UD           g58UD           0x08000584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $8 };
cmp.l.f0.0(16)  null<1>UD       g67<8,8,1>UD    g69<8,8,1>UD    { align1 1H };
(+f0.0) if(16)  JIP:  LABEL9          UIP:  LABEL8              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
sel.ge(16)      g51<1>F         (abs)g33<1,1,0>F (abs)g41<1,1,0>F { align1 1H I@7 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
sel.ge(16)      g53<1>F         (abs)g35<1,1,0>F (abs)g43<1,1,0>F { align1 1H I@7 compacted };
sel.ge(16)      g55<1>F         (abs)g37<1,1,0>F (abs)g45<1,1,0>F { align1 1H $8.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
shl(16)         g59<1>D         -g84<8,8,1>D    0x00000017UD    { align1 1H I@7 };
shl(16)         g81<1>D         -g86<8,8,1>D    0x00000017UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
shl(16)         g126<1>D        -g88<8,8,1>D    0x00000017UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
sel.ge(16)      g57<1>F         g53<1,1,0>F     g55<1,1,0>F     { align1 1H F@1 compacted };
add(16)         g61<1>D         g59<8,8,1>D     1132462080D     { align1 1H I@3 };
add(16)         g79<1>D         g81<8,8,1>D     1132462080D     { align1 1H I@3 };
add(16)         g81<1>D         g126<8,8,1>D    1132462080D     { align1 1H I@3 };
sel.ge(16)      g96<1>F         g51<1,1,0>F     g57<1,1,0>F     { align1 1H F@1 compacted };
mul(16)         g29<1>F         g96<1,1,0>F     0x34000000F  /* 1.19209e-07F */ { align1 1H F@1 compacted };
add(16)         g31<1>F         g33<1,1,0>F     -g29<1,1,0>F    { align1 1H F@1 compacted };
add(16)         g51<1>F         g35<1,1,0>F     -g29<1,1,0>F    { align1 1H compacted };
add(16)         g53<1>F         g37<1,1,0>F     -g29<1,1,0>F    { align1 1H compacted };
add(16)         g55<1>F         g41<1,1,0>F     g29<1,1,0>F     { align1 1H compacted };
add(16)         g57<1>F         g43<1,1,0>F     g29<1,1,0>F     { align1 1H compacted };
add(16)         g59<1>F         g45<1,1,0>F     g29<1,1,0>F     { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add(16)         g29<1>F         g31<1,1,0>F     -g9<1,1,0>F     { align1 1H F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add(16)         g31<1>F         g51<1,1,0>F     -g11<1,1,0>F    { align1 1H F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add(16)         g51<1>F         g53<1,1,0>F     -g13<1,1,0>F    { align1 1H F@6 compacted };
add(16)         g53<1>F         g55<1,1,0>F     -g9<1,1,0>F     { align1 1H F@6 compacted };
add(16)         g55<1>F         g57<1,1,0>F     -g11<1,1,0>F    { align1 1H F@6 compacted };
add(16)         g57<1>F         g59<1,1,0>F     -g13<1,1,0>F    { align1 1H F@6 compacted };
mul(16)         g59<1>F         g29<8,8,1>F     0x3f7ffffeF  /* 1F */ { align1 1H F@6 };
mul(16)         g29<1>F         g31<8,8,1>F     0x3f7ffffeF  /* 1F */ { align1 1H F@6 };
mul(16)         g31<1>F         g51<8,8,1>F     0x3f7ffffeF  /* 1F */ { align1 1H F@6 };
mul(16)         g51<1>F         g53<8,8,1>F     0x3f800001F  /* 1F */ { align1 1H F@6 };
mul(16)         g53<1>F         g55<8,8,1>F     0x3f800001F  /* 1F */ { align1 1H F@6 };
mul(16)         g55<1>F         g57<8,8,1>F     0x3f800001F  /* 1F */ { align1 1H F@6 };
mul(16)         g57<1>F         g59<1,1,0>F     g61<1,1,0>F     { align1 1H A@3 compacted };
mul(16)         g59<1>F         g29<1,1,0>F     g79<1,1,0>F     { align1 1H A@2 compacted };
mul(16)         g29<1>F         g31<1,1,0>F     g81<1,1,0>F     { align1 1H A@1 compacted };
mul(16)         g31<1>F         g51<1,1,0>F     g61<1,1,0>F     { align1 1H F@6 compacted };
mul(16)         g51<1>F         g53<1,1,0>F     g79<1,1,0>F     { align1 1H F@6 compacted };
rndd(16)        g95<1>F         g57<1,1,0>F                     { align1 1H F@5 compacted };
mul(16)         g53<1>F         g55<1,1,0>F     g81<1,1,0>F     { align1 1H F@7 compacted };
rndd(16)        g97<1>F         g59<1,1,0>F                     { align1 1H F@6 compacted };
rndd(16)        g99<1>F         g29<1,1,0>F                     { align1 1H F@6 compacted };
rndd(16)        g101<1>F        -g31<1,1,0>F                    { align1 1H F@6 compacted };
rndd(16)        g103<1>F        -g51<1,1,0>F                    { align1 1H A@6 compacted };
sel.ge(16)      g109<1>F        g95<1,1,0>F     0x0F  /* 0F */  { align1 1H F@6 compacted };
rndd(16)        g107<1>F        -g53<1,1,0>F                    { align1 1H F@6 compacted };
sel.ge(16)      g111<1>F        g97<1,1,0>F     0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.ge(16)      g113<1>F        g99<1,1,0>F     0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.ge(16)      g115<1>F        -g101<1,1,0>F   0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.ge(16)      g117<1>F        -g103<1,1,0>F   0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.l(16)       g97<1>F         g109<8,8,1>F    0x437f0000F  /* 255F */ { align1 1H F@6 };
sel.ge(16)      g119<1>F        -g107<1,1,0>F   0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.l(16)       g99<1>F         g111<8,8,1>F    0x437f0000F  /* 255F */ { align1 1H F@6 };
sel.l(16)       g101<1>F        g113<8,8,1>F    0x437f0000F  /* 255F */ { align1 1H F@6 };
sel.l(16)       g103<1>F        g115<8,8,1>F    0x437f0000F  /* 255F */ { align1 1H F@6 };
sel.l(16)       g107<1>F        g117<8,8,1>F    0x437f0000F  /* 255F */ { align1 1H F@6 };
mov(16)         g83<4>UB        g97<8,8,1>F                     { align1 1H F@6 };
sel.l(16)       g109<1>F        g119<8,8,1>F    0x437f0000F  /* 255F */ { align1 1H F@5 };
mov(16)         g85<4>UB        g99<8,8,1>F                     { align1 1H F@5 };
mov(16)         g87<4>UB        g101<8,8,1>F                    { align1 1H F@4 };
mov(16)         g89<4>UB        g103<8,8,1>F                    { align1 1H F@3 };
mov(16)         g91<4>UB        g107<8,8,1>F                    { align1 1H F@2 };
mov(16)         g81<1>UW        g83<32,8,4>UB                   { align1 1H I@5 };
mov(16)         g93<4>UB        g109<8,8,1>F                    { align1 1H F@1 };
mov(16)         g80<1>UW        g85<32,8,4>UB                   { align1 1H I@6 };
mov(16)         g79<1>UW        g87<32,8,4>UB                   { align1 1H I@6 };
mov(16)         g84<1>UW        g89<32,8,4>UB                   { align1 1H I@6 };
mov(16)         g83<1>UW        g91<32,8,4>UB                   { align1 1H I@6 };
mov(16)         g82<1>UW        g93<32,8,4>UB                   { align1 1H I@5 };
else(16)        JIP:  LABEL8          UIP:  LABEL8              { align1 1H };

LABEL9:
mov(16)         g79<1>UW        0x0080UW                        { align1 1H I@4 };
mov(16)         g80<1>UW        0x0080UW                        { align1 1H I@5 };
mov(16)         g81<1>UW        0x0080UW                        { align1 1H I@7 };
mov(16)         g82<1>UW        0x0000UW                        { align1 1H I@5 };
mov(16)         g83<1>UW        0x0000UW                        { align1 1H A@2 };
mov(16)         g84<1>UW        0x0000UW                        { align1 1H I@7 };

LABEL8:
endif(16)       JIP:  LABEL7                                    { align1 1H };
add(16)         g120<1>D        g75<1,1,0>D     g67<1,1,0>D     { align1 1H F@1 compacted };
mov(16)         g85<2>UB        g79<16,16,1>UW                  { align1 1H I@7 };
mov(16)         g86<2>UB        g80<16,16,1>UW                  { align1 1H I@7 };
mov(16)         g87<2>UB        g81<16,16,1>UW                  { align1 1H I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
mov(16)         g57<2>UB        g82<16,16,1>UW                  { align1 1H I@7 };
mov(16)         g60<2>UB        g83<16,16,1>UW                  { align1 1H I@7 };
mov(16)         g88<2>UB        g84<16,16,1>UW                  { align1 1H I@7 };
cmp.l.f0.0(16)  g110<1>UD       g120<1,1,0>UD   g75<1,1,0>UD    { align1 1H A@5 compacted };
add(16)         g112<1>D        g120<1,1,0>D    28D             { align1 1H F@4 compacted };
mov(16)         g61<1>UD        g87<16,8,2>UB                   { align1 1H I@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
add(16)         g58<1>D         -g110<1,1,0>D   g77<1,1,0>D     { align1 1H I@3 compacted };
cmp.l.f0.0(16)  g114<1>UD       g112<1,1,0>UD   g120<1,1,0>UD   { align1 1H A@3 compacted };
mov(8)          g123<2>UD       g112<4,4,1>UD                   { align1 1Q };
mov(8)          g125<2>UD       g113<4,4,1>UD                   { align1 2Q };
add(16)         g116<1>D        -g114<1,1,0>D   g58<1,1,0>D     { align1 1H A@2 compacted };
mov(8)          g123.1<2>UD     g116<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g125.1<2>UD     g117<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g123UD          g61UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $9 };
add(16)         g117<1>D        g120<1,1,0>D    40D             { align1 1H F@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(16)         g79<1>UD        g86<16,8,2>UB                   { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
cmp.l.f0.0(16)  g122<1>UD       g117<1,1,0>UD   g120<1,1,0>UD   { align1 1H I@2 compacted };
mov(8)          g124<2>UD       g117<4,4,1>UD                   { align1 1Q $9.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
mov(8)          g126<2>UD       g118<4,4,1>UD                   { align1 2Q $9.src };
add(16)         g29<1>D         -g122<1,1,0>D   g58<1,1,0>D     { align1 1H A@1 compacted };
mov(8)          g124.1<2>UD     g29<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g126.1<2>UD     g30<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g124UD          g79UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };
add(16)         g51<1>D         g120<1,1,0>D    52D             { align1 1H $7.src compacted };
mov(16)         g80<1>UD        g85<16,8,2>UB                   { align1 1H $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
cmp.l.f0.0(16)  g53<1>UD        g51<1,1,0>UD    g120<1,1,0>UD   { align1 1H I@2 compacted };
mov(8)          g29<2>UD        g51<4,4,1>UD                    { align1 1Q };
mov(8)          g31<2>UD        g52<4,4,1>UD                    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
add(16)         g55<1>D         -g53<1,1,0>D    g58<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g29.1<2>UD      g55<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g31.1<2>UD      g56<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g29UD           g80UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $4 };
add(16)         g61<1>D         g120<1,1,0>D    34D             { align1 1H $9.src compacted };
mov(16)         g83<1>UD        g88<16,8,2>UB                   { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
cmp.l.f0.0(16)  g79<1>UD        g61<1,1,0>UD    g120<1,1,0>UD   { align1 1H I@2 compacted };
mov(8)          g51<2>UD        g61<4,4,1>UD                    { align1 1Q };
mov(8)          g53<2>UD        g62<4,4,1>UD                    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add(16)         g81<1>D         -g79<1,1,0>D    g58<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g51.1<2>UD      g81<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g53.1<2>UD      g82<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g51UD           g83UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $10 };
add(16)         g82<1>D         g120<1,1,0>D    46D             { align1 1H $10.src compacted };
mov(16)         g88<1>UD        g60<16,8,2>UB                   { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
cmp.l.f0.0(16)  g84<1>UD        g82<1,1,0>UD    g120<1,1,0>UD   { align1 1H I@2 compacted };
mov(8)          g52<2>UD        g82<4,4,1>UD                    { align1 1Q $10.src };
mov(8)          g54<2>UD        g83<4,4,1>UD                    { align1 2Q $10.src };
add(16)         g86<1>D         -g84<1,1,0>D    g58<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g52.1<2>UD      g86<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g54.1<2>UD      g87<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g52UD           g88UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $11 };
add(16)         g87<1>D         g120<1,1,0>D    58D             { align1 1H $11.src compacted };
mov(16)         g93<1>UD        g57<16,8,2>UB                   { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
cmp.l.f0.0(16)  g89<1>UD        g87<1,1,0>UD    g120<1,1,0>UD   { align1 1H I@2 compacted };
mov(8)          g53<2>UD        g87<4,4,1>UD                    { align1 1Q $11.src };
mov(8)          g55<2>UD        g88<4,4,1>UD                    { align1 2Q $11.src };
add(16)         g91<1>D         -g89<1,1,0>D    g58<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g53.1<2>UD      g91<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g55.1<2>UD      g92<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g53UD           g93UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $12 };
cmp.nz.f0.0(16) null<1>D        g27<8,8,1>D     0D              { align1 1H };
mov(1)          g51<1>D         2D                              { align1 WE_all 1N $10.src };
add(16)         g94<1>D         g120<1,1,0>D    22D             { align1 1H $12.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
(+f0.0) sel(16) g92<1>UD        g51<0,1,0>UD    0x00000001UD    { align1 1H $12.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mov(8)          g54<2>UD        g94<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $12.src };
mov(8)          g56<2>UD        g95<4,4,1>UD                    { align1 2Q I@3 };
mov(16)         g89<4>UB        g92<8,8,1>UD                    { align1 1H I@3 };
cmp.l.f0.0(16)  g96<1>UD        g94<1,1,0>UD    g120<1,1,0>UD   { align1 1H compacted };
mov(16)         g94<1>UD        g89<32,8,4>UB                   { align1 1H I@2 };
add(16)         g98<1>D         -g96<1,1,0>D    g58<1,1,0>D     { align1 1H I@2 compacted };
mov(8)          g54.1<2>UD      g98<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g56.1<2>UD      g99<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g54UD           g94UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $13 };

LABEL7:
endif(16)       JIP:  LABEL10                                   { align1 1H };

LABEL10:
else(16)        JIP:  LABEL5          UIP:  LABEL5              { align1 1H };

LABEL6:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(16)         g99<1>UD        g39.3<32,8,4>UB                 { align1 1H A@3 };
cmp.l.f0.0(16)  null<1>D        g105<8,8,1>D    g69<8,8,1>D     { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
(+f0.0) sel(16) g79<1>UD        g99<1,1,0>UD    0x00000000UD    { align1 1H A@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(16)         g100<1>UD       g79<32,8,4>UB                   { align1 1H A@1 };
cmp.l.f0.0(16)  null<1>UD       g67<8,8,1>UD    g69<8,8,1>UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(16)         a0<1>UW         0x0c80UW                        { align1 WE_all 1H A@2 };
shl(16)         a0<1>UW         g25<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0c80UW        { align1 1H A@1 };
mov(16)         g102<1>UD       g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
or(16)          g109<1>UD       g79<1,1,0>UD    g102<1,1,0>UD   { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(16)         g111<1>UD       g109<32,8,4>UB                  { align1 1H A@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(16)         a0<1>UW         0x0de0UW                        { align1 WE_all 1H A@1 };
shl(16)         a0<1>UW         g17<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0de0UW        { align1 1H A@1 };
mov(16)         g113<1>UD       g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
or(16)          g115<1>UD       g111<1,1,0>UD   g113<1,1,0>UD   { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
mov(16)         g117<1>UD       g115<32,8,4>UB                  { align1 1H A@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
mov(16)         a0<1>UW         0x0ea0UW                        { align1 WE_all 1H A@1 };
shl(16)         a0<1>UW         g19<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0ea0UW        { align1 1H A@1 };
mov(16)         g119<1>UD       g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
or(16)          g121<1>UD       g115<1,1,0>UD   g119<1,1,0>UD   { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(16)         g107<4>UB       g121<0,1,0>UD                   { align1 1H A@1 };
(+f0.0) if(16)  JIP:  LABEL12         UIP:  LABEL11             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(16)         g123<1>UD       g121<0,1,0>UB                   { align1 1H F@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
cmp.z.f0.0(16)  g125<1>D        g123<1,1,0>D    0D              { align1 1H A@1 compacted };
cmp.z.f0.0(16)  null<1>D        g79<8,8,1>D     0D              { align1 1H };
(+f0.0) sel(16) g27<1>UD        g125<8,8,1>UD   0xffffffffUD    { align1 1H I@2 };
cmp.nz.f0.0(16) null<1>D        g27<8,8,1>D     0D              { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
(+f0.0) sel(16) g57<1>UD        g41<8,8,1>UD    0xff800000UD    { align1 1H F@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
(+f0.0) sel(16) g59<1>UD        g43<8,8,1>UD    0xff800000UD    { align1 1H F@7 };
(+f0.0) sel(16) g61<1>UD        g45<8,8,1>UD    0xff800000UD    { align1 1H $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
(+f0.0) sel(16) g51<1>UD        g33<8,8,1>UD    0x7f800000UD    { align1 1H $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
(+f0.0) sel(16) g53<1>UD        g35<8,8,1>UD    0x7f800000UD    { align1 1H F@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
(+f0.0) sel(16) g55<1>UD        g37<8,8,1>UD    0x7f800000UD    { align1 1H F@5 };
else(16)        JIP:  LABEL11         UIP:  LABEL11             { align1 1H };

LABEL12:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
mov(16)         g55<1>UD        0x7f800000UD                    { align1 1H A@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
mov(16)         g53<1>UD        0x7f800000UD                    { align1 1H A@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mov(16)         g51<1>UD        0x7f800000UD                    { align1 1H I@6 };
mov(16)         g61<1>UD        0xff800000UD                    { align1 1H I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(16)         g59<1>UD        0xff800000UD                    { align1 1H A@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
mov(16)         g57<1>UD        0xff800000UD                    { align1 1H F@5 };

LABEL11:
endif(16)       JIP:  LABEL5                                    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(16)         a0<1>UW         0x0660UW                        { align1 WE_all 1H A@1 };
shl(16)         a0<1>UW         g25<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0660UW        { align1 1H A@1 };
mov(16)         g28<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(16)         a0<1>UW         0x06a0UW                        { align1 WE_all 1H A@1 };
shl(16)         a0<1>UW         g25<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x06a0UW        { align1 1H A@1 };
mov(16)         g30<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x06e0UW                        { align1 WE_all 1H A@1 };
shl(16)         a0<1>UW         g25<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x06e0UW        { align1 1H A@1 };
mov(16)         g81<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0720UW                        { align1 WE_all 1H A@2 };
shl(16)         a0<1>UW         g25<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0720UW        { align1 1H A@1 };
mov(16)         g83<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0760UW                        { align1 WE_all 1H A@5 };
shl(16)         a0<1>UW         g25<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0760UW        { align1 1H A@1 };
mov(16)         g109<1>UD       g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x07a0UW                        { align1 WE_all 1H A@3 };
shl(16)         a0<1>UW         g25<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x07a0UW        { align1 1H A@1 };
mov(16)         g111<1>UD       g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
cmp.l.f0.0(16)  null<1>UD       g67<8,8,1>UD    0x00000006UD    { align1 1H };
sel.l(16)       g25<1>F         g51<1,1,0>F     g28<1,1,0>F     { align1 1H I@2 compacted };
sel.ge(16)      g85<1>F         g57<1,1,0>F     g83<1,1,0>F     { align1 1H A@4 compacted };
sel.l(16)       g27<1>F         g53<1,1,0>F     g30<1,1,0>F     { align1 1H I@6 compacted };
sel.ge(16)      g87<1>F         g59<1,1,0>F     g109<1,1,0>F    { align1 1H A@3 compacted };
sel.ge(16)      g89<1>F         g61<1,1,0>F     g111<1,1,0>F    { align1 1H A@2 compacted };
sel.l(16)       g29<1>F         g55<1,1,0>F     g81<1,1,0>F     { align1 1H I@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(16)         a0<1>UW         0x0320UW                        { align1 WE_all 1H F@4 };
shl(16)         a0<1>UW         g17<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0320UW        { align1 1H A@1 };
mov(16)         g31<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0aa0UW                        { align1 WE_all 1H F@5 };
shl(16)         a0<1>UW         g17<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0aa0UW        { align1 1H A@1 };
mov(16)         g91<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0360UW                        { align1 WE_all 1H F@1 };
shl(16)         a0<1>UW         g17<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0360UW        { align1 1H A@1 };
mov(16)         g81<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
mov(16)         a0<1>UW         0x0ae0UW                        { align1 WE_all 1H F@3 };
shl(16)         a0<1>UW         g17<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0ae0UW        { align1 1H A@1 };
mov(16)         g93<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
mov(16)         a0<1>UW         0x0b20UW                        { align1 WE_all 1H F@2 };
shl(16)         a0<1>UW         g17<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0b20UW        { align1 1H A@1 };
mov(16)         g95<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x03a0UW                        { align1 WE_all 1H F@1 };
shl(16)         a0<1>UW         g17<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x03a0UW        { align1 1H A@1 };
mov(16)         g83<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sel.l(16)       g17<1>F         g25<1,1,0>F     g31<1,1,0>F     { align1 1H I@1 compacted };
sel.l(16)       g25<1>F         g27<1,1,0>F     g81<1,1,0>F     { align1 1H A@1 compacted };
sel.ge(16)      g31<1>F         g87<1,1,0>F     g93<1,1,0>F     { align1 1H I@3 compacted };
sel.l(16)       g27<1>F         g29<1,1,0>F     g83<1,1,0>F     { align1 1H A@1 compacted };
sel.ge(16)      g29<1>F         g85<1,1,0>F     g91<1,1,0>F     { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0220UW                        { align1 WE_all 1H F@2 };
shl(16)         a0<1>UW         g19<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0220UW        { align1 1H A@1 };
mov(16)         g83<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0320UW                        { align1 WE_all 1H A@1 };
shl(16)         a0<1>UW         g19<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0320UW        { align1 1H A@1 };
mov(16)         g85<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sel.ge(16)      g91<1>F         g89<1,1,0>F     g95<1,1,0>F     { align1 1H I@4 compacted };
mov(16)         a0<1>UW         0x03e0UW                        { align1 WE_all 1H F@4 };
shl(16)         a0<1>UW         g19<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x03e0UW        { align1 1H A@1 };
mov(16)         g93<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0360UW                        { align1 WE_all 1H A@3 };
shl(16)         a0<1>UW         g19<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0360UW        { align1 1H A@1 };
mov(16)         g87<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x03a0UW                        { align1 WE_all 1H A@1 };
shl(16)         a0<1>UW         g19<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x03a0UW        { align1 1H A@1 };
mov(16)         g89<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sel.l(16)       g81<1>F         g17<1,1,0>F     g83<1,1,0>F     { align1 1H I@5 compacted };
mov(16)         a0<1>UW         0x0b60UW                        { align1 WE_all 1H F@2 };
shl(16)         a0<1>UW         g19<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0b60UW        { align1 1H A@1 };
mov(16)         g95<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sel.l(16)       g83<1>F         g25<1,1,0>F     g85<1,1,0>F     { align1 1H I@5 compacted };
sel.l(16)       g85<1>F         g27<1,1,0>F     g87<1,1,0>F     { align1 1H I@3 compacted };
sel.ge(16)      g87<1>F         g29<1,1,0>F     g89<1,1,0>F     { align1 1H I@2 compacted };
sel.ge(16)      g89<1>F         g31<1,1,0>F     g93<1,1,0>F     { align1 1H I@4 compacted };
sel.ge(16)      g93<1>F         g91<1,1,0>F     g95<1,1,0>F     { align1 1H I@1 compacted };
(+f0.0) if(16)  JIP:  LABEL13         UIP:  LABEL13             { align1 1H };
sel.ge(16)      g27<1>F         (abs)g81<0,1,0>F (abs)g87<0,1,0>F { align1 1H A@3 compacted };
sel.ge(16)      g25<1>F         (abs)g83<0,1,0>F (abs)g89<0,1,0>F { align1 1H A@3 compacted };
sel.ge(16)      g31<1>F         (abs)g85<0,1,0>F (abs)g93<0,1,0>F { align1 1H A@3 compacted };
add(16)         g101<1>D        g75<1,1,0>D     16D             { align1 1H compacted };
sel.ge(16)      g29<1>F         g25<1,1,0>F     g31<1,1,0>F     { align1 1H A@1 compacted };
mov(16)         g91<2>B         1W                              { align1 1H A@3 };
mov(8)          g17<2>UD        g101<4,4,1>UD                   { align1 1Q A@2 };
mov(8)          g19<2>UD        g102<4,4,1>UD                   { align1 2Q I@3 };
sel.ge(16)      g25<1>F         g27<1,1,0>F     g29<1,1,0>F     { align1 1H F@1 compacted };
mov(16)         g103<1>UD       g91<16,8,2>UB                   { align1 1H I@3 };
mul(16)         g91<1>F         g25<1,1,0>F     0x34000000F  /* 1.19209e-07F */ { align1 1H A@1 compacted };
add(16)         g25<1>F         g81<0,1,0>F     -g91<1,1,0>F    { align1 1H F@1 compacted };
add(16)         g27<1>F         g83<0,1,0>F     -g91<1,1,0>F    { align1 1H compacted };
add(16)         g29<1>F         g85<0,1,0>F     -g91<1,1,0>F    { align1 1H compacted };
add(16)         g81<1>F         g87<0,1,0>F     g91<1,1,0>F     { align1 1H compacted };
add(16)         g83<1>F         g89<0,1,0>F     g91<1,1,0>F     { align1 1H compacted };
add(16)         g85<1>F         g93<0,1,0>F     g91<1,1,0>F     { align1 1H compacted };
add(16)         g87<1>F         g81<1,1,0>F     -g25<1,1,0>F    { align1 1H F@3 compacted };
add(16)         g81<1>F         g83<1,1,0>F     -g27<1,1,0>F    { align1 1H F@3 compacted };
add(16)         g83<1>F         g85<1,1,0>F     -g29<1,1,0>F    { align1 1H F@3 compacted };
mul(16)         g85<1>F         g87<8,8,1>F     0x3f800001F  /* 1F */ { align1 1H F@3 };
mul(16)         g87<1>F         g81<8,8,1>F     0x3f800001F  /* 1F */ { align1 1H F@3 };
mul(16)         g81<1>F         g83<8,8,1>F     0x3f800001F  /* 1F */ { align1 1H F@3 };
and(16)         g83<1>UD        g85<8,8,1>UD    0x007fffffUD    { align1 1H F@1 };
and(16)         g89<1>UD        g85<8,8,1>UD    0x80000000UD    { align1 1H F@7 };
and(16)         g95<1>UD        g85<8,8,1>UD    0x7f800000UD    { align1 1H };
and(16)         g91<1>UD        g87<8,8,1>UD    0x80000000UD    { align1 1H F@2 };
and(16)         g97<1>UD        g87<8,8,1>UD    0x7f800000UD    { align1 1H };
and(16)         g93<1>UD        g81<8,8,1>UD    0x80000000UD    { align1 1H F@1 };
and(16)         g99<1>UD        g81<8,8,1>UD    0x7f800000UD    { align1 1H };
and(16)         g85<1>UD        g87<8,8,1>UD    0x007fffffUD    { align1 1H };
and(16)         g87<1>UD        g81<8,8,1>UD    0x007fffffUD    { align1 1H };
add(16)         g81<1>D         g83<8,8,1>D     1056964608D     { align1 1H I@7 };
asr(16)         g83<1>D         g95<8,8,1>D     0x00000017UD    { align1 1H I@7 };
add(16)         g95<1>D         g85<8,8,1>D     1056964608D     { align1 1H I@4 };
asr(16)         g85<1>D         g97<8,8,1>D     0x00000017UD    { align1 1H I@7 };
add(16)         g97<1>D         g87<8,8,1>D     1056964608D     { align1 1H I@5 };
cmp.z.f0.0(16)  null<1>F        g81<1,1,0>F     0x3f800000F  /* 1F */ { align1 1H I@5 compacted };
asr(16)         g87<1>D         g99<8,8,1>D     0x00000017UD    { align1 1H I@7 };
(-f0.0) sel(16) g99<1>UD        g81<8,8,1>UD    0x3f000000UD    { align1 1H };
and(16)         g81<1>UD        g99<8,8,1>UD    0x7fffffffUD    { align1 1H A@1 };
cmp.z.f0.0(16)  null<1>F        g95<1,1,0>F     0x3f800000F  /* 1F */ { align1 1H I@6 compacted };
or(16)          g99<1>UD        g81<1,1,0>UD    g89<1,1,0>UD    { align1 1H I@1 compacted };
(-f0.0) sel(16) g81<1>UD        g95<8,8,1>UD    0x3f000000UD    { align1 1H };
and(16)         g89<1>UD        g81<8,8,1>UD    0x7fffffffUD    { align1 1H I@1 };
cmp.z.f0.0(16)  null<1>F        g97<1,1,0>F     0x3f800000F  /* 1F */ { align1 1H I@7 compacted };
or(16)          g81<1>UD        g89<1,1,0>UD    g91<1,1,0>UD    { align1 1H I@1 compacted };
(-f0.0) sel(16) g89<1>UD        g97<8,8,1>UD    0x3f000000UD    { align1 1H };
and(16)         g91<1>UD        g89<8,8,1>UD    0x7fffffffUD    { align1 1H I@1 };
cmp.g.f0.0(16)  g89<1>F         g99<8,8,1>F     0x3f7f0000F  /* 0.996094F */ { align1 1H I@1 };
or(16)          g95<1>UD        g91<1,1,0>UD    g93<1,1,0>UD    { align1 1H A@1 compacted };
cmp.g.f0.0(16)  g91<1>F         g81<8,8,1>F     0x3f7f0000F  /* 0.996094F */ { align1 1H I@1 };
add3(16)        g81<1>D         65410W          g83<8,8,1>D     -g89<1,1,1>D { align1 1H F@1 };
cmp.g.f0.0(16)  g89<1>F         g95<8,8,1>F     0x3f7f0000F  /* 0.996094F */ { align1 1H I@1 };
add3(16)        g83<1>D         65410W          g85<8,8,1>D     -g91<1,1,1>D { align1 1H F@2 };
cmp.l.f0.0(16)  g91<1>UD        g101<1,1,0>UD   g75<1,1,0>UD    { align1 1H compacted };
add3(16)        g85<1>D         65410W          g87<8,8,1>D     -g89<1,1,1>D { align1 1H F@1 };
add(16)         g87<1>D         -g91<1,1,0>D    g77<1,1,0>D     { align1 1H I@2 compacted };
mov(8)          g17.1<2>UD      g87<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g19.1<2>UD      g88<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g17UD           g103UD          0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $14 };
mov(16)         g31<1>D         g15<8,8,1>D                     { align1 1H $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g21UD           g25UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $15 };
add(16)         g88<1>D         g75<1,1,0>D     18D             { align1 1H compacted };
mov(16)         g92<4>UB        g81<8,8,1>UD                    { align1 1H I@7 };
cmp.l.f0.0(16)  g90<1>UD        g88<1,1,0>UD    g75<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g9<2>UD         g88<4,4,1>UD                    { align1 1Q $4.src };
mov(8)          g11<2>UD        g89<4,4,1>UD                    { align1 2Q $4.src };
mov(16)         g97<4>UB        g83<8,8,1>UD                    { align1 1H F@4 };
mov(16)         g99<4>UB        g85<8,8,1>UD                    { align1 1H F@3 };
mov(16)         g95<4>UB        g92<32,8,4>UB                   { align1 1H A@1 };
add(16)         g92<1>D         -g90<1,1,0>D    g77<1,1,0>D     { align1 1H I@6 compacted };
mov(16)         g95.1<4>UB      g97<32,8,4>UB                   { align1 1H I@2 };
mov(8)          g9.1<2>UD       g92<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g11.1<2>UD      g93<4,4,1>UD                    { align1 2Q I@3 };
mov(16)         g95.2<4>UB      g99<32,8,4>UB                   { align1 1H I@3 };
mov(16)         g95.3<4>UB      g107<32,8,4>UB                  { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g9UD            g95UD           0x08000584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $13 };
cmp.l.f0.0(16)  null<1>UD       g67<8,8,1>UD    g69<8,8,1>UD    { align1 1H };
(+f0.0) if(16)  JIP:  LABEL15         UIP:  LABEL14             { align1 1H };
sel.ge(16)      g112<1>F        (abs)g51<1,1,0>F (abs)g57<1,1,0>F { align1 1H compacted };
sel.ge(16)      g114<1>F        (abs)g53<1,1,0>F (abs)g59<1,1,0>F { align1 1H compacted };
sel.ge(16)      g116<1>F        (abs)g55<1,1,0>F (abs)g61<1,1,0>F { align1 1H compacted };
shl(16)         g118<1>D        -g81<8,8,1>D    0x00000017UD    { align1 1H };
shl(16)         g120<1>D        -g83<8,8,1>D    0x00000017UD    { align1 1H };
shl(16)         g122<1>D        -g85<8,8,1>D    0x00000017UD    { align1 1H };
cmp.z.f0.0(16)  null<1>D        g79<8,8,1>D     0D              { align1 1H };
sel.ge(16)      g93<1>F         g114<1,1,0>F    g116<1,1,0>F    { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
add(16)         g95<1>D         g118<8,8,1>D    1132462080D     { align1 1H I@4 };
add(16)         g97<1>D         g120<8,8,1>D    1132462080D     { align1 1H I@4 };
add(16)         g99<1>D         g122<8,8,1>D    1132462080D     { align1 1H I@4 };
sel.ge(16)      g101<1>F        g112<1,1,0>F    g93<1,1,0>F     { align1 1H F@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
mul(16)         g103<1>F        g101<1,1,0>F    0x34000000F  /* 1.19209e-07F */ { align1 1H F@1 compacted };
add(16)         g107<1>F        g51<1,1,0>F     -g103<1,1,0>F   { align1 1H A@1 compacted };
add(16)         g109<1>F        g53<1,1,0>F     -g103<1,1,0>F   { align1 1H compacted };
add(16)         g111<1>F        g55<1,1,0>F     -g103<1,1,0>F   { align1 1H compacted };
add(16)         g113<1>F        g57<1,1,0>F     g103<1,1,0>F    { align1 1H compacted };
add(16)         g115<1>F        g59<1,1,0>F     g103<1,1,0>F    { align1 1H compacted };
add(16)         g117<1>F        g61<1,1,0>F     g103<1,1,0>F    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
add(16)         g119<1>F        g107<1,1,0>F    -g25<1,1,0>F    { align1 1H A@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
add(16)         g121<1>F        g109<1,1,0>F    -g27<1,1,0>F    { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(16)         g123<1>F        g111<1,1,0>F    -g29<1,1,0>F    { align1 1H A@1 compacted };
add(16)         g54<1>F         g113<1,1,0>F    -g25<1,1,0>F    { align1 1H F@6 compacted };
add(16)         g56<1>F         g115<1,1,0>F    -g27<1,1,0>F    { align1 1H F@6 compacted };
add(16)         g58<1>F         g117<1,1,0>F    -g29<1,1,0>F    { align1 1H F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mul(16)         g125<1>F        g119<8,8,1>F    0x3f7ffffeF  /* 1F */ { align1 1H F@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
mul(16)         g9<1>F          g121<8,8,1>F    0x3f7ffffeF  /* 1F */ { align1 1H F@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
mul(16)         g11<1>F         g123<8,8,1>F    0x3f7ffffeF  /* 1F */ { align1 1H F@6 };
mul(16)         g60<1>F         g54<8,8,1>F     0x3f800001F  /* 1F */ { align1 1H F@6 };
mul(16)         g79<1>F         g56<8,8,1>F     0x3f800001F  /* 1F */ { align1 1H A@4 };
mul(16)         g93<1>F         g58<8,8,1>F     0x3f800001F  /* 1F */ { align1 1H F@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mul(16)         g13<1>F         g125<1,1,0>F    g95<1,1,0>F     { align1 1H A@3 compacted };
mul(16)         g15<1>F         g9<1,1,0>F      g97<1,1,0>F     { align1 1H A@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
mul(16)         g17<1>F         g11<1,1,0>F     g99<1,1,0>F     { align1 1H A@1 compacted };
mul(16)         g101<1>F        g60<1,1,0>F     g95<1,1,0>F     { align1 1H F@6 compacted };
mul(16)         g103<1>F        g79<1,1,0>F     g97<1,1,0>F     { align1 1H F@6 compacted };
mul(16)         g107<1>F        g93<1,1,0>F     g99<1,1,0>F     { align1 1H F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
rndd(16)        g19<1>F         g13<1,1,0>F                     { align1 1H F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
rndd(16)        g21<1>F         g15<1,1,0>F                     { align1 1H F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
rndd(16)        g23<1>F         g17<1,1,0>F                     { align1 1H F@6 compacted };
rndd(16)        g109<1>F        -g101<1,1,0>F                   { align1 1H F@6 compacted };
rndd(16)        g111<1>F        -g103<1,1,0>F                   { align1 1H F@6 compacted };
rndd(16)        g113<1>F        -g107<1,1,0>F                   { align1 1H F@6 compacted };
sel.ge(16)      g25<1>F         g19<1,1,0>F     0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.ge(16)      g27<1>F         g21<1,1,0>F     0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.ge(16)      g29<1>F         g23<1,1,0>F     0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.ge(16)      g115<1>F        -g109<1,1,0>F   0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.ge(16)      g117<1>F        -g111<1,1,0>F   0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.ge(16)      g119<1>F        -g113<1,1,0>F   0x0F  /* 0F */  { align1 1H F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
sel.l(16)       g31<1>F         g25<8,8,1>F     0x437f0000F  /* 255F */ { align1 1H F@6 };
sel.l(16)       g51<1>F         g27<8,8,1>F     0x437f0000F  /* 255F */ { align1 1H F@6 };
sel.l(16)       g53<1>F         g29<8,8,1>F     0x437f0000F  /* 255F */ { align1 1H F@6 };
sel.l(16)       g121<1>F        g115<8,8,1>F    0x437f0000F  /* 255F */ { align1 1H F@6 };
sel.l(16)       g123<1>F        g117<8,8,1>F    0x437f0000F  /* 255F */ { align1 1H F@6 };
sel.l(16)       g125<1>F        g119<8,8,1>F    0x437f0000F  /* 255F */ { align1 1H F@6 };
mov(16)         g81<4>UB        g31<8,8,1>F                     { align1 1H F@6 };
mov(16)         g83<4>UB        g51<8,8,1>F                     { align1 1H F@5 };
mov(16)         g85<4>UB        g53<8,8,1>F                     { align1 1H F@4 };
mov(16)         g87<4>UB        g121<8,8,1>F                    { align1 1H F@3 };
mov(16)         g89<4>UB        g123<8,8,1>F                    { align1 1H F@2 };
mov(16)         g91<4>UB        g125<8,8,1>F                    { align1 1H F@1 };
mov(16)         g10<1>UW        g81<32,8,4>UB                   { align1 1H I@6 };
mov(16)         g11<1>UW        g83<32,8,4>UB                   { align1 1H I@6 };
mov(16)         g12<1>UW        g85<32,8,4>UB                   { align1 1H I@6 };
mov(16)         g126<1>UW       g87<32,8,4>UB                   { align1 1H I@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(16)         g127<1>UW       g89<32,8,4>UB                   { align1 1H I@6 };
mov(16)         g9<1>UW         g91<32,8,4>UB                   { align1 1H I@6 };
(-f0.0) sel(16) g87<1>UW        g10<16,16,1>UW  0x0000UW        { align1 1H I@6 };
(-f0.0) sel(16) g86<1>UW        g11<16,16,1>UW  0x0000UW        { align1 1H I@6 };
(-f0.0) sel(16) g85<1>UW        g12<16,16,1>UW  0x0000UW        { align1 1H I@6 };
(-f0.0) sel(16) g90<1>UW        g126<16,16,1>UW 0x0000UW        { align1 1H I@6 };
(-f0.0) sel(16) g89<1>UW        g127<16,16,1>UW 0x0000UW        { align1 1H I@6 };
(-f0.0) sel(16) g88<1>UW        g9<16,16,1>UW   0x0000UW        { align1 1H I@6 };
else(16)        JIP:  LABEL14         UIP:  LABEL14             { align1 1H };

LABEL15:
mov(16)         g85<1>UW        0x0080UW                        { align1 1H I@5 };
mov(16)         g86<1>UW        0x0080UW                        { align1 1H I@7 };
mov(16)         g87<1>UW        0x0080UW                        { align1 1H I@7 };
mov(16)         g88<1>UW        0x0000UW                        { align1 1H I@5 };
mov(16)         g89<1>UW        0x0000UW                        { align1 1H I@7 };
mov(16)         g90<1>UW        0x0000UW                        { align1 1H I@7 };

LABEL14:
endif(16)       JIP:  LABEL13                                   { align1 1H };
add(16)         g13<1>D         g75<1,1,0>D     g67<1,1,0>D     { align1 1H $4.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
mov(16)         g103<2>UB       g85<16,16,1>UW                  { align1 1H I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
mov(16)         g104<2>UB       g86<16,16,1>UW                  { align1 1H I@7 };
mov(16)         g107<2>UB       g87<16,16,1>UW                  { align1 1H I@7 };
mov(16)         g61<2>UB        g88<16,16,1>UW                  { align1 1H I@7 };
mov(16)         g123<2>UB       g89<16,16,1>UW                  { align1 1H I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(16)         g124<2>UB       g90<16,16,1>UW                  { align1 1H I@7 };
cmp.l.f0.0(16)  g15<1>UD        g13<1,1,0>UD    g75<1,1,0>UD    { align1 1H I@7 compacted };
add(16)         g19<1>D         g13<1,1,0>D     28D             { align1 1H $14.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
mov(16)         g96<1>UD        g107<16,8,2>UB                  { align1 1H I@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
add(16)         g17<1>D         -g15<1,1,0>D    g77<1,1,0>D     { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
cmp.l.f0.0(16)  g21<1>UD        g19<1,1,0>UD    g13<1,1,0>UD    { align1 1H I@3 compacted };
mov(8)          g118<2>UD       g19<4,4,1>UD                    { align1 1Q F@1 };
mov(8)          g120<2>UD       g20<4,4,1>UD                    { align1 2Q F@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
add(16)         g23<1>D         -g21<1,1,0>D    g17<1,1,0>D     { align1 1H A@3 compacted };
mov(8)          g118.1<2>UD     g23<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g120.1<2>UD     g24<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g118UD          g96UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
add(16)         g24<1>D         g13<1,1,0>D     40D             { align1 1H F@6 compacted };
mov(16)         g101<1>UD       g104<16,8,2>UB                  { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
cmp.l.f0.0(16)  g26<1>UD        g24<1,1,0>UD    g13<1,1,0>UD    { align1 1H A@2 compacted };
mov(8)          g97<2>UD        g24<4,4,1>UD                    { align1 1Q $0.src };
mov(8)          g99<2>UD        g25<4,4,1>UD                    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
add(16)         g28<1>D         -g26<1,1,0>D    g17<1,1,0>D     { align1 1H A@3 compacted };
mov(8)          g97.1<2>UD      g28<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g99.1<2>UD      g29<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g97UD           g101UD          0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
add(16)         g29<1>D         g13<1,1,0>D     52D             { align1 1H F@4 compacted };
mov(16)         g111<1>UD       g103<16,8,2>UB                  { align1 1H F@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
cmp.l.f0.0(16)  g31<1>UD        g29<1,1,0>UD    g13<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g107<2>UD       g29<4,4,1>UD                    { align1 1Q };
mov(8)          g109<2>UD       g30<4,4,1>UD                    { align1 2Q F@7 };
add(16)         g51<1>D         -g31<1,1,0>D    g17<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g107.1<2>UD     g51<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g109.1<2>UD     g52<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g107UD          g111UD          0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $2 };
add(16)         g52<1>D         g13<1,1,0>D     34D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(16)         g116<1>UD       g124<16,8,2>UB                  { align1 1H F@2 };
cmp.l.f0.0(16)  g54<1>UD        g52<1,1,0>UD    g13<1,1,0>UD    { align1 1H I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g112<2>UD       g52<4,4,1>UD                    { align1 1Q F@7 };
mov(8)          g114<2>UD       g53<4,4,1>UD                    { align1 2Q F@3 };
add(16)         g56<1>D         -g54<1,1,0>D    g17<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g112.1<2>UD     g56<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g114.1<2>UD     g57<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g112UD          g116UD          0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $3 };
add(16)         g57<1>D         g13<1,1,0>D     46D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(16)         g121<1>UD       g123<16,8,2>UB                  { align1 1H $0.src };
cmp.l.f0.0(16)  g59<1>UD        g57<1,1,0>UD    g13<1,1,0>UD    { align1 1H I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(8)          g117<2>UD       g57<4,4,1>UD                    { align1 1Q $3.src };
mov(8)          g119<2>UD       g58<4,4,1>UD                    { align1 2Q $0.src };
add(16)         g67<1>D         -g59<1,1,0>D    g17<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g117.1<2>UD     g67<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g119.1<2>UD     g68<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g117UD          g121UD          0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $4 };
add(16)         g75<1>D         g13<1,1,0>D     58D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(16)         g126<1>UD       g61<16,8,2>UB                   { align1 1H };
cmp.l.f0.0(16)  g77<1>UD        g75<1,1,0>UD    g13<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g122<2>UD       g75<4,4,1>UD                    { align1 1Q $4.src };
mov(8)          g124<2>UD       g76<4,4,1>UD                    { align1 2Q $0.src };
add(16)         g79<1>D         -g77<1,1,0>D    g17<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g122.1<2>UD     g79<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g124.1<2>UD     g80<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g122UD          g126UD          0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };
add(16)         g80<1>D         g13<1,1,0>D     22D             { align1 1H compacted };
mov(16)         g125<2>B        2W                              { align1 1H $0.src };
cmp.l.f0.0(16)  g82<1>UD        g80<1,1,0>UD    g13<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g9<2>UD         g80<4,4,1>UD                    { align1 1Q $13.src };
mov(8)          g11<2>UD        g81<4,4,1>UD                    { align1 2Q $13.src };
mov(16)         g13<1>UD        g125<16,8,2>UB                  { align1 1H I@4 };
add(16)         g84<1>D         -g82<1,1,0>D    g17<1,1,0>D     { align1 1H I@4 compacted };
mov(8)          g9.1<2>UD       g84<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g11.1<2>UD      g85<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g9UD            g13UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $4 };

LABEL13:
endif(16)       JIP:  LABEL5                                    { align1 1H };

LABEL5:
endif(16)       JIP:  LABEL16                                   { align1 1H };

LABEL16:
cmp.l.f0.0(16)  null<1>D        g105<8,8,1>D    g69<8,8,1>D     { align1 1H };
(+f0.0) if(16)  JIP:  LABEL17         UIP:  LABEL17             { align1 1H };
asr(16)         g85<1>D         g69<8,8,1>D     0x0000001fUD    { align1 1H A@4 };
shl(16)         g87<1>D         g105<8,8,1>D    0x00000005UD    { align1 1H F@3 };
shr(16)         g89<1>UD        g105<1,1,0>UD   0x0000001bUD    { align1 1H F@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.dst };
add(16)         g91<1>D         g5<1,1,0>D      g87<1,1,0>D     { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
cmp.l.f0.0(16)  g93<1>UD        g91<1,1,0>UD    g5<1,1,0>UD     { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g14<2>UD        g91<4,4,1>UD                    { align1 1Q F@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $4.src };
mov(8)          g16<2>UD        g92<4,4,1>UD                    { align1 2Q F@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add3(16)        g95<1>D         g7<8,8,1>D      g89<8,8,1>D     -g93<1,1,1>D { align1 1H A@1 };
mov(8)          g14.1<2>UD      g95<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g16.1<2>UD      g96<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g14UD           g33UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
add(16)         g97<1>D         g91<1,1,0>D     16D             { align1 1H F@7 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
cmp.l.f0.0(16)  g99<1>UD        g97<1,1,0>UD    g91<1,1,0>UD    { align1 1H A@1 compacted };
mov(8)          g15<2>UD        g97<4,4,1>UD                    { align1 1Q $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $4.src };
mov(8)          g17<2>UD        g98<4,4,1>UD                    { align1 2Q F@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
add(16)         g101<1>D        -g99<1,1,0>D    g95<1,1,0>D     { align1 1H A@3 compacted };
mov(8)          g15.1<2>UD      g101<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g17.1<2>UD      g102<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g15UD           g41UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $4 };
shl(16)         g102<1>D        g105<8,8,1>D    0x00000002UD    { align1 1H F@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
shr(16)         g107<1>UD       g105<1,1,0>UD   0x0000001eUD    { align1 1H F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g109<1>D        g1<1,1,0>D      g102<1,1,0>D    { align1 1H A@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
cmp.l.f0.0(16)  g111<1>UD       g109<1,1,0>UD   g1<1,1,0>UD     { align1 1H A@1 compacted };
mov(8)          g16<2>UD        g109<4,4,1>UD                   { align1 1Q $4.src };
mov(8)          g18<2>UD        g110<4,4,1>UD                   { align1 2Q $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
add3(16)        g113<1>D        g3<8,8,1>D      g107<8,8,1>D    -g111<1,1,1>D { align1 1H A@3 };
mov(8)          g16.1<2>UD      g113<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g18.1<2>UD      g114<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g16UD           g105UD          0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $4 };
shr(16)         g21<1>UD        g49<0,1,0>UD    0x00000006UD    { align1 1H $4.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
add(16)         g114<1>D        g69<1,1,0>D     g105<1,1,0>D    { align1 1H F@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
cmp.l.f0.0(16)  g116<1>UD       g114<1,1,0>UD   g69<1,1,0>UD    { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
shl(16)         g120<1>D        g114<8,8,1>D    0x00000002UD    { align1 1H F@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
shr(16)         g123<1>UD       g114<1,1,0>UD   0x0000001eUD    { align1 1H F@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add(16)         g118<1>D        -g116<1,1,0>D   g85<1,1,0>D     { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(16)         g126<1>D        g1<1,1,0>D      g120<1,1,0>D    { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
shl(16)         g121<1>D        g118<8,8,1>D    0x00000002UD    { align1 1H A@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
cmp.l.f0.0(16)  g9<1>UD         g126<1,1,0>UD   g1<1,1,0>UD     { align1 1H I@2 compacted };
mov(8)          g17<2>UD        g126<4,4,1>UD                   { align1 1Q $4.src };
mov(8)          g19<2>UD        g127<4,4,1>UD                   { align1 2Q $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
or(16)          g125<1>UD       g121<1,1,0>UD   g123<1,1,0>UD   { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add3(16)        g11<1>D         g3<8,8,1>D      g125<8,8,1>D    -g9<1,1,1>D { align1 1H I@1 };
mov(8)          g17.1<2>UD      g11<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g19.1<2>UD      g12<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g17UD           g21UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $4 };
and.nz.f0.0(16) null<1>UD       g71<8,8,1>UD    g73<8,8,1>UD    { align1 1H };
(+f0.0) if(16)  JIP:  LABEL18         UIP:  LABEL18             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
shl(16)         g12<1>D         g69<8,8,1>D     0x00000003UD    { align1 1H A@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add(16)         g14<1>D         g63<1,1,0>D     40D             { align1 1H F@7 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
or(16)          g33<1>UD        g12<8,8,1>UD    0xffffffc0UD    { align1 1H I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
cmp.l.f0.0(16)  g16<1>UD        g14<1,1,0>UD    g63<1,1,0>UD    { align1 1H A@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g22<2>UD        g14<4,4,1>UD                    { align1 1Q F@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $4.src };
mov(8)          g24<2>UD        g15<4,4,1>UD                    { align1 2Q F@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add(16)         g18<1>D         -g16<1,1,0>D    g65<1,1,0>D     { align1 1H A@3 compacted };
mov(8)          g22.1<2>UD      g18<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g24.1<2>UD      g19<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g19UD           g22UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
shl(16)         g21<1>D         g19<8,8,1>D     0x00000006UD    { align1 1H $5.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
add(16)         g23<1>D         g63<1,1,0>D     g21<1,1,0>D     { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
cmp.l.f0.0(16)  g25<1>UD        g23<1,1,0>UD    g63<1,1,0>UD    { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g29<2>UD        g23<4,4,1>UD                    { align1 1Q F@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $4.src };
mov(8)          g31<2>UD        g24<4,4,1>UD                    { align1 2Q F@2 };
add(16)         g27<1>D         -g25<1,1,0>D    g65<1,1,0>D     { align1 1H A@3 compacted };
mov(8)          g29.1<2>UD      g27<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g31.1<2>UD      g28<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g29UD           g33UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $4 };

LABEL18:
endif(16)       JIP:  LABEL17                                   { align1 1H };

LABEL17:
endif(16)       JIP:  LABEL19                                   { align1 1H };

LABEL19:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q A@1 };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_build_DFS_DFS_trivial_batchable_code[] = {
    0x80000065, 0x7e058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x1c050220, 0x00000024, 0x00000000,
    0xe27f1a40, 0x00017e03, 0x80030061, 0x3e054410,
    0x00000000, 0x76543210, 0x00031b41, 0x20018220,
    0x01461c05, 0x00580058, 0x605f1c41, 0x05801c02,
    0x80001c01, 0x00000000, 0x00000000, 0x00000000,
    0x80004031, 0x01140000, 0xfa007f0c, 0x00340000,
    0x643e1b40, 0x00803e95, 0xfe5d0049, 0x05801c03,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00040061, 0x43050160, 0x00463e05, 0x00000000,
    0x00130041, 0x20018220, 0x01461d05, 0x00580058,
    0xe01e1a65, 0x00f04303, 0x00130049, 0x5e058222,
    0x02461d05, 0x00000058, 0x00041a61, 0x7b060210,
    0x00461e05, 0x00000000, 0x00040061, 0x69050120,
    0x00561e06, 0x00000000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x21220061, 0x001102cc,
    0x2a5b0061, 0x001102cc, 0x21221261, 0x00110204,
    0x2a5b1261, 0x00110204, 0x00031261, 0x61050220,
    0x00442226, 0x00000000, 0xa1771f40, 0x5f0e2202,
    0x00131161, 0x62050220, 0x00445b26, 0x00000000,
    0xaa780040, 0x600e5b02, 0x00031b70, 0x7d050220,
    0x52467705, 0x00442206, 0x00030061, 0x6f060220,
    0x00347705, 0x00000000, 0x00131b70, 0x7e050220,
    0x52467805, 0x00445b06, 0x00130061, 0x71060220,
    0x00347805, 0x00000000, 0xa00a0040, 0x02007703,
    0xa00f0040, 0x03c07703, 0xa0140040, 0x03007703,
    0x270c1b70, 0x77000a03, 0x00030061, 0x64060220,
    0x00340a05, 0x00000000, 0x00130061, 0x66060220,
    0x00340b05, 0x00000000, 0x27111d70, 0x77000f03,
    0x00030061, 0x6b060220, 0x00340f05, 0x00000000,
    0x00130061, 0x6d060220, 0x00341005, 0x00000000,
    0x00040052, 0x09040e68, 0x0e2e6105, 0x7d055d05,
    0x00031f61, 0x73060220, 0x00341405, 0x00000000,
    0x00131f61, 0x75060220, 0x00341505, 0x00000000,
    0x27160070, 0x77001403, 0xa0131c40, 0x09021102,
    0x00030061, 0x6f260220, 0x00340905, 0x00000000,
    0x00130061, 0x71260220, 0x00340a05, 0x00000000,
    0xa00e0040, 0x09020c02, 0xa0181d40, 0x09021602,
    0x00031d61, 0x6b260220, 0x00341305, 0x00000000,
    0x00131e61, 0x6d260220, 0x00341405, 0x00000000,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x01440000, 0xfb046f24, 0x003c0000,
    0x00031c61, 0x64260220, 0x00340e05, 0x00000000,
    0x00131d61, 0x66260220, 0x00340f05, 0x00000000,
    0x00031d61, 0x73260220, 0x00341805, 0x00000000,
    0x00131e61, 0x75260220, 0x00341905, 0x00000000,
    0x80001d01, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x3d140000, 0xfb046b24, 0x00040000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x3f240000, 0xfb046424, 0x000c0000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x19240000, 0xfb047324, 0x000c0000,
    0x00042261, 0x79060210, 0x00463d05, 0x00000000,
    0xe01d2465, 0x00101903, 0x00042461, 0x45050120,
    0x00561b06, 0x00000000, 0xae471a70, 0x00001d03,
    0x00041a70, 0x00010660, 0x56466905, 0x00464505,
    0x01040022, 0x0001c060, 0x000001b0, 0x00000130,
    0x00040069, 0x1f058660, 0x02466905, 0x00000005,
    0xe0210068, 0x01b06903, 0xa023a140, 0x1f000502,
    0x27251970, 0x05002303, 0x00030061, 0x2f060220,
    0x00342305, 0x00000000, 0x00130061, 0x31060220,
    0x00342405, 0x00000000, 0xa0290040, 0x01002303,
    0x0004c152, 0x27040e68, 0x0e2e0705, 0x25052105,
    0x272b1a70, 0x23002903, 0x00033461, 0x74060220,
    0x00342905, 0x00000000, 0x00133461, 0x76060220,
    0x00342a05, 0x00000000, 0x00031c61, 0x2f260220,
    0x00342705, 0x00000000, 0x00131d61, 0x31260220,
    0x00342805, 0x00000000, 0xa02d1d40, 0x27022b02,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x21440000, 0xfb042f24, 0x003c0000,
    0x00031961, 0x74260220, 0x00342d05, 0x00000000,
    0x00131a61, 0x76260220, 0x00342e05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x29440000, 0xfb047424, 0x003c0000,
    0x00040024, 0x0001c060, 0x00000090, 0x00000090,
    0x00042461, 0x27054220, 0x00000000, 0x00000000,
    0x00042461, 0x25054220, 0x00000000, 0x7f800000,
    0x00042461, 0x23054220, 0x00000000, 0x7f800000,
    0x00042461, 0x21054220, 0x00000000, 0x7f800000,
    0x00042461, 0x2f054220, 0x00000000, 0x80000000,
    0x00042461, 0x2d054220, 0x00000000, 0xff800000,
    0x00042461, 0x2b054220, 0x00000000, 0xff800000,
    0x00042461, 0x29054220, 0x00000000, 0xff800000,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x00043470, 0x31058550, 0x15567b06, 0x00000000,
    0x00041961, 0x49050560, 0x20463105, 0x00000000,
    0x01040022, 0x0001c060, 0x000000a8, 0x000000a8,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0xa0322340, 0x00c03f03, 0x27341970, 0x3f003203,
    0x00030061, 0x39060220, 0x00343205, 0x00000000,
    0x00130061, 0x3b060220, 0x00343305, 0x00000000,
    0xa036b340, 0x41023402, 0x00031961, 0x39260220,
    0x00343605, 0x00000000, 0x00131a61, 0x3b260220,
    0x00343705, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044531, 0x37140000,
    0xfb103924, 0x01000000, 0x00042569, 0x31058660,
    0x02463705, 0x00000006, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0xa04ba340, 0x31103f02,
    0xa03a3540, 0x01003f03, 0xa0570040, 0x00404303,
    0xa0590040, 0xff404303, 0xa05b0040, 0x00204303,
    0xa05d0040, 0xff204303, 0xa05f0040, 0x00104303,
    0xa0610040, 0xff104303, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x27381f70, 0x3f004b03,
    0x00030061, 0x15060220, 0x00344b05, 0x00000000,
    0x00130061, 0x17060220, 0x00344c05, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x00031f61, 0x75060220, 0x00343a05, 0x00000000,
    0x00133461, 0x77060220, 0x00343b05, 0x00000000,
    0x274f0070, 0x3f003a03, 0x00040070, 0x00018660,
    0x56465705, 0x00000010, 0xa04df340, 0x41023802,
    0xa0511b40, 0x41024f02, 0x2f190062, 0x59005703,
    0x00031b61, 0x15260220, 0x00344d05, 0x00000000,
    0x00131c61, 0x17260220, 0x00344e05, 0x00000000,
    0x00031c61, 0x75260220, 0x00345105, 0x00000000,
    0x00131d61, 0x77260220, 0x00345205, 0x00000000,
    0x00040070, 0x00018660, 0x56465b05, 0x00000010,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00044631, 0x52140000, 0xfb047524, 0x00040000,
    0x2f110062, 0x5d005b03, 0x00040070, 0x00018660,
    0x56465f05, 0x00000010, 0x2f130062, 0x61005f03,
    0x00040070, 0x62058550, 0x15567906, 0x00010001,
    0x00041961, 0x1b050560, 0x20466205, 0x00000000,
    0x00042669, 0x54058660, 0x02465205, 0x00000006,
    0x00041952, 0x56040e68, 0x0e2e3f05, 0x4b055405,
    0x0004196c, 0x0f058660, 0x02465605, 0x00000006,
    0x11040022, 0x0001c060, 0x00002bb8, 0x000013b0,
    0x00040070, 0x00010220, 0x52464305, 0x00464505,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x01042462, 0x63058220, 0x02462905, 0xff800000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x01042462, 0x65058220, 0x02462b05, 0xff800000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x01042462, 0x67058220, 0x02462d05, 0xff800000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x01042462, 0x6d058220, 0x02462105, 0x7f800000,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x01042462, 0x6f058220, 0x02462305, 0x7f800000,
    0x01042462, 0x5c058220, 0x02462505, 0x7f800000,
    0x80041e61, 0x10014110, 0x00000000, 0x0c600c60,
    0x00040069, 0x10018510, 0x01561906, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0c600c60,
    0xe0350961, 0x001b0004, 0x80041e61, 0x10014110,
    0x00000000, 0x0ca00ca0, 0x00040069, 0x10018510,
    0x01561906, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0ca00ca0, 0xe0370961, 0x001b0004,
    0x80041e61, 0x10014110, 0x00000000, 0x0ce00ce0,
    0x00040069, 0x10018510, 0x01561906, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0ce00ce0,
    0xe01e0961, 0x001b0004, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x80041e61, 0x10014110,
    0x00000000, 0x0da00da0, 0x00040069, 0x10018510,
    0x01561906, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0da00da0, 0xe0710961, 0x001b0004,
    0x80041e61, 0x10014110, 0x00000000, 0x0de00de0,
    0x00040069, 0x10018510, 0x01561906, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0de00de0,
    0xe05e0961, 0x001b0004, 0x80041e61, 0x10014110,
    0x00000000, 0x0b800b80, 0x00040069, 0x10018510,
    0x01561906, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0b800b80, 0xe0600961, 0x001b0004,
    0x00040070, 0x00018220, 0x52464305, 0x00000006,
    0x25331f62, 0x35006300, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x256b1d62, 0x1e006700,
    0x27620a62, 0x71006d00, 0x25351e62, 0x37006500,
    0x27640962, 0x5e006f00, 0x27660a62, 0x60005c00,
    0x80041661, 0x10014110, 0x00000000, 0x06600660,
    0x00040069, 0x10018510, 0x01561106, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x06600660,
    0xe0390961, 0x001b0004, 0x80041361, 0x10014110,
    0x00000000, 0x0d600d60, 0x00040069, 0x10018510,
    0x01561106, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0d600d60, 0xe0370961, 0x001b0004,
    0x80041461, 0x10014110, 0x00000000, 0x0c400c40,
    0x00040069, 0x10018510, 0x01561106, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0c400c40,
    0xe0790961, 0x001b0004, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x80041361, 0x10014110,
    0x00000000, 0x06a006a0, 0x00040069, 0x10018510,
    0x01561106, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x06a006a0, 0xe03b0961, 0x001b0004,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80041261, 0x10014110, 0x00000000, 0x0c800c80,
    0x00040069, 0x10018510, 0x01561106, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0c800c80,
    0xe0730961, 0x001b0004, 0x80003601, 0x00000000,
    0x00000000, 0x00000000, 0x80041161, 0x10014110,
    0x00000000, 0x0cc00cc0, 0x00040069, 0x10018510,
    0x01561106, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0cc00cc0, 0xe0750961, 0x001b0004,
    0x256d0c62, 0x39003300, 0x25711d62, 0x37006b00,
    0x80003601, 0x00000000, 0x00000000, 0x00000000,
    0x27771c62, 0x79006200, 0x256f0b62, 0x3b003500,
    0x27791a62, 0x73006400, 0x277b1962, 0x75006600,
    0x80041661, 0x10014110, 0x00000000, 0x0da00da0,
    0x00040069, 0x10018510, 0x01561306, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0da00da0,
    0xe04f0961, 0x001b0004, 0x80041561, 0x10014110,
    0x00000000, 0x0e200e20, 0x00040069, 0x10018510,
    0x01561306, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0e200e20, 0xe0380961, 0x001b0004,
    0x80041461, 0x10014110, 0x00000000, 0x0ee00ee0,
    0x00040069, 0x10018510, 0x01561306, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0ee00ee0,
    0xe01f0961, 0x001b0004, 0x80041361, 0x10014110,
    0x00000000, 0x0de00de0, 0x00040069, 0x10018510,
    0x01561306, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0de00de0, 0xe0510961, 0x001b0004,
    0x80041261, 0x10014110, 0x00000000, 0x0f200f20,
    0x00040069, 0x10018510, 0x01561306, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0f200f20,
    0xe07d0961, 0x001b0004, 0x80041161, 0x10014110,
    0x00000000, 0x0f600f60, 0x00040069, 0x10018510,
    0x01561306, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0f600f60, 0xe01d0961, 0x001b0004,
    0x25551e62, 0x4f006d00, 0x25591d62, 0x38007100,
    0x274f1c62, 0x1f007700, 0x25571b62, 0x51006f00,
    0x27531962, 0x1d007b00, 0x27511a62, 0x7d007900,
    0x01040022, 0x0001c060, 0x00000dd0, 0x00000dd0,
    0x255b1462, 0x55aa4f00, 0x255d1262, 0x57aa5100,
    0x255f1462, 0x59aa5300, 0xa0371740, 0x01004b03,
    0x00040061, 0x68064540, 0x00000000, 0xffffffff,
    0x25611162, 0x5f005d00, 0x00040061, 0x3b070200,
    0x00463d05, 0x00000000, 0x00031b61, 0x33060220,
    0x00343705, 0x00000000, 0x00131c61, 0x35060220,
    0x00343805, 0x00000000, 0x255d1162, 0x61005b00,
    0x00040b61, 0x39050020, 0x00663b07, 0x00000000,
    0xe05f1141, 0x34005d00, 0x20091140, 0x5f214f00,
    0x200b0040, 0x5f215100, 0x200d0040, 0x5f215300,
    0x20610040, 0x5f015500, 0x20630040, 0x5f015700,
    0x20650040, 0x5f015900, 0x206b1340, 0x09206100,
    0x206d1340, 0x0b206300, 0x206f0b40, 0x0d206500,
    0x00041341, 0x71058aa0, 0x0a466b05, 0x3f800001,
    0x00040941, 0x3b058aa0, 0x0a466d05, 0x3f800001,
    0x00040b41, 0x7b058aa0, 0x0a466f05, 0x3f800001,
    0x00041165, 0x6f058220, 0x02467105, 0x80000000,
    0x00041565, 0x62058220, 0x02467105, 0x007fffff,
    0x00040065, 0x75058220, 0x02467105, 0x7f800000,
    0x00041265, 0x64058220, 0x02463b05, 0x007fffff,
    0x00040065, 0x77058220, 0x02463b05, 0x7f800000,
    0x00041165, 0x66058220, 0x02467b05, 0x007fffff,
    0x00040065, 0x73058220, 0x02467b05, 0x80000000,
    0x00040065, 0x79058220, 0x02467b05, 0x7f800000,
    0x00040065, 0x71058220, 0x02463b05, 0x80000000,
    0x00040b40, 0x6b058660, 0x06466205, 0x3f000000,
    0x00040a40, 0x6d058660, 0x06466405, 0x3f000000,
    0x00041f6c, 0x7b058660, 0x02467505, 0x00000017,
    0x00041f6c, 0x7d058660, 0x02467705, 0x00000017,
    0x00041f40, 0x1d058660, 0x06466605, 0x3f000000,
    0x00041f6c, 0x1f058660, 0x02467905, 0x00000017,
    0xac001e70, 0x3f806b01, 0x11040062, 0x3c058220,
    0x02466b05, 0x3f000000, 0x00041965, 0x4f058220,
    0x02463c05, 0x7fffffff, 0xac001f70, 0x3f806d01,
    0x20511966, 0x6f004f03, 0x11040062, 0x75058220,
    0x02466d05, 0x3f000000, 0x00041965, 0x77058220,
    0x02467505, 0x7fffffff, 0xac001f70, 0x3f801d01,
    0x20531966, 0x71007703, 0x11040062, 0x76058220,
    0x02461d05, 0x3f000000, 0x00041965, 0x78058220,
    0x02467605, 0x7fffffff, 0x00041e70, 0x3b058aa0,
    0x3a465105, 0x3f7f0000, 0x00041b70, 0x3d058aa0,
    0x3a465305, 0x3f7f0000, 0x201d0966, 0x73007803,
    0x00041152, 0x54044560, 0x0e2eff82, 0x3b057b05,
    0x00041152, 0x56044560, 0x0e2eff82, 0x3d057d05,
    0x00041b70, 0x4f058aa0, 0x3a461d05, 0x3f7f0000,
    0x27511370, 0x4b003703, 0x00041152, 0x58044560,
    0x0e2eff82, 0x4f051f05, 0xa05a1a40, 0x4d025102,
    0x00031961, 0x33260220, 0x00345a05, 0x00000000,
    0x00131a61, 0x35260220, 0x00345b05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044731, 0x00000000, 0xf3083324, 0x00023914,
    0x00044431, 0x00000000, 0xfb0c1524, 0x003c0944,
    0xa05b0040, 0x01204b03, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0x00041f61, 0x34070200,
    0x00465405, 0x00000000, 0x275d1a70, 0x4b005b03,
    0x00033761, 0x36060220, 0x00345b05, 0x00000000,
    0x00133761, 0x38060220, 0x00345c05, 0x00000000,
    0x00040061, 0x3c070200, 0x00465605, 0x00000000,
    0x00041f61, 0x4f070200, 0x00465805, 0x00000000,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x00041e61, 0x3a070000, 0x00663407, 0x00000000,
    0xa05f1e40, 0x4d025d02, 0x00041a61, 0x3a0f0000,
    0x00663c07, 0x00000000, 0x00031a61, 0x36260220,
    0x00345f05, 0x00000000, 0x00131b61, 0x38260220,
    0x00346005, 0x00000000, 0x00041b61, 0x3a170000,
    0x00664f07, 0x00000000, 0x00041961, 0x3a1f0000,
    0x00566806, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044831, 0x00000000,
    0xfb083624, 0x00003a14, 0x00040070, 0x00010220,
    0x52464305, 0x00464505, 0x01040022, 0x0001c060,
    0x00000400, 0x000003a0, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0x25331f62, 0x29432100,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0x25351f62, 0x2b432300, 0x25373862, 0x2d432500,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0x00041f69, 0x3b05a660, 0x02465405, 0x00000017,
    0x00040069, 0x5105a660, 0x02465605, 0x00000017,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00040069, 0x7e05a660, 0x02465805, 0x00000017,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0x25391162, 0x37003500, 0x00041b40, 0x3d058660,
    0x06463b05, 0x43800000, 0x00041b40, 0x4f058660,
    0x06465105, 0x43800000, 0x00041b40, 0x51058660,
    0x06467e05, 0x43800000, 0x25601162, 0x39003300,
    0xe01d1141, 0x34006000, 0x201f1140, 0x1d202100,
    0x20330040, 0x1d202300, 0x20350040, 0x1d202500,
    0x20370040, 0x1d002900, 0x20390040, 0x1d002b00,
    0x203b1b40, 0x1d002d00, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x201d1640, 0x09201f00,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x201f1640, 0x0b203300, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x20331640, 0x0d203500,
    0x20351640, 0x09203700, 0x20371640, 0x0b203900,
    0x20391640, 0x0d203b00, 0x00041641, 0x3b058aa0,
    0x0a461d05, 0x3f7ffffe, 0x00041641, 0x1d058aa0,
    0x0a461f05, 0x3f7ffffe, 0x00041641, 0x1f058aa0,
    0x0a463305, 0x3f7ffffe, 0x00041641, 0x33058aa0,
    0x0a463505, 0x3f800001, 0x00041641, 0x35058aa0,
    0x0a463705, 0x3f800001, 0x00041641, 0x37058aa0,
    0x0a463905, 0x3f800001, 0x20390b41, 0x3d003b00,
    0x203b0a41, 0x4f001d00, 0x201d0941, 0x51001f00,
    0x201f1641, 0x3d003300, 0x20331641, 0x4f003500,
    0x605f1545, 0x00103900, 0x20351741, 0x51003700,
    0x60611645, 0x00103b00, 0x60631645, 0x00101d00,
    0x60651645, 0x00121f00, 0x60670e45, 0x00123300,
    0xe56d1662, 0x00005f00, 0x606b1645, 0x00123500,
    0xe56f1662, 0x00006100, 0xe5711662, 0x00006300,
    0xe5731662, 0x00026500, 0xe5751662, 0x00026700,
    0x00041662, 0x61058aa0, 0x5a466d05, 0x437f0000,
    0xe5771662, 0x00026b00, 0x00041662, 0x63058aa0,
    0x5a466f05, 0x437f0000, 0x00041662, 0x65058aa0,
    0x5a467105, 0x437f0000, 0x00041662, 0x67058aa0,
    0x5a467305, 0x437f0000, 0x00041662, 0x6b058aa0,
    0x5a467505, 0x437f0000, 0x00041661, 0x53070a00,
    0x00466105, 0x00000000, 0x00041562, 0x6d058aa0,
    0x5a467705, 0x437f0000, 0x00041561, 0x55070a00,
    0x00466305, 0x00000000, 0x00041461, 0x57070a00,
    0x00466505, 0x00000000, 0x00041361, 0x59070a00,
    0x00466705, 0x00000000, 0x00041261, 0x5b070a00,
    0x00466b05, 0x00000000, 0x00041d61, 0x51050010,
    0x00665307, 0x00000000, 0x00041161, 0x5d070a00,
    0x00466d05, 0x00000000, 0x00041e61, 0x50050010,
    0x00665507, 0x00000000, 0x00041e61, 0x4f050010,
    0x00665707, 0x00000000, 0x00041e61, 0x54050010,
    0x00665907, 0x00000000, 0x00041e61, 0x53050010,
    0x00665b07, 0x00000000, 0x00041d61, 0x52050010,
    0x00665d07, 0x00000000, 0x00040024, 0x0001c060,
    0x00000070, 0x00000070, 0x00041c61, 0x4f054110,
    0x00000000, 0x00800080, 0x00041d61, 0x50054110,
    0x00000000, 0x00800080, 0x00041f61, 0x51054110,
    0x00000000, 0x00800080, 0x00041d61, 0x52054110,
    0x00000000, 0x00000000, 0x00040a61, 0x53054110,
    0x00000000, 0x00000000, 0x00041f61, 0x54054110,
    0x00000000, 0x00000000, 0x00040025, 0x00004600,
    0x00000000, 0x00000568, 0xa0781140, 0x43004b02,
    0x00041f61, 0x55060100, 0x00584f05, 0x00000000,
    0x00041f61, 0x56060100, 0x00585005, 0x00000000,
    0x00041f61, 0x57060100, 0x00585105, 0x00000000,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0x00041f61, 0x39060100, 0x00585205, 0x00000000,
    0x00041f61, 0x3c060100, 0x00585305, 0x00000000,
    0x00041f61, 0x58060100, 0x00585405, 0x00000000,
    0x276e0d70, 0x4b007803, 0xa0701440, 0x01c07803,
    0x00041e61, 0x3d050020, 0x00565706, 0x00000000,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0xa03a1b40, 0x4d026e02, 0x27720b70, 0x78007003,
    0x00030061, 0x7b060220, 0x00347005, 0x00000000,
    0x00130061, 0x7d060220, 0x00347105, 0x00000000,
    0xa0740a40, 0x3a027202, 0x00031961, 0x7b260220,
    0x00347405, 0x00000000, 0x00131a61, 0x7d260220,
    0x00347505, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044931, 0x00000000,
    0xf3087b24, 0x00023d14, 0xa0751240, 0x02807803,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00040061, 0x4f050020, 0x00565606, 0x00000000,
    0x80003901, 0x00000000, 0x00000000, 0x00000000,
    0x277a1a70, 0x78007503, 0x00033961, 0x7c060220,
    0x00347505, 0x00000000, 0x80103001, 0x00000000,
    0x00000000, 0x00000000, 0x00133961, 0x7e060220,
    0x00347605, 0x00000000, 0xa01d0940, 0x3a027a02,
    0x00031961, 0x7c260220, 0x00341d05, 0x00000000,
    0x00131a61, 0x7e260220, 0x00341e05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x00000000, 0xf3087c24, 0x00024f14,
    0xa0333740, 0x03407803, 0x00043061, 0x50050020,
    0x00565506, 0x00000000, 0x80003801, 0x00000000,
    0x00000000, 0x00000000, 0x27351a70, 0x78003303,
    0x00030061, 0x1d060220, 0x00343305, 0x00000000,
    0x00130061, 0x1f060220, 0x00343405, 0x00000000,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0xa0371b40, 0x3a023502, 0x00031961, 0x1d260220,
    0x00343705, 0x00000000, 0x00131a61, 0x1f260220,
    0x00343805, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044431, 0x00000000,
    0xf3081d24, 0x00025014, 0xa03d3940, 0x02207803,
    0x00040061, 0x53050020, 0x00565806, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x274f1a70, 0x78003d03, 0x00030061, 0x33060220,
    0x00343d05, 0x00000000, 0x00130061, 0x35060220,
    0x00343e05, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0xa0511b40, 0x3a024f02,
    0x00031961, 0x33260220, 0x00345105, 0x00000000,
    0x00131a61, 0x35260220, 0x00345205, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044a31, 0x00000000, 0xf3083324, 0x00025314,
    0xa0523a40, 0x02e07803, 0x00040061, 0x58050020,
    0x00563c06, 0x00000000, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x27541a70, 0x78005203,
    0x00033a61, 0x34060220, 0x00345205, 0x00000000,
    0x00133a61, 0x36060220, 0x00345305, 0x00000000,
    0xa0561b40, 0x3a025402, 0x00031961, 0x34260220,
    0x00345605, 0x00000000, 0x00131a61, 0x36260220,
    0x00345705, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044b31, 0x00000000,
    0xf3083424, 0x00025814, 0xa0573b40, 0x03a07803,
    0x00040061, 0x5d050020, 0x00563906, 0x00000000,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0x27591a70, 0x78005703, 0x00033b61, 0x35060220,
    0x00345705, 0x00000000, 0x00133b61, 0x37060220,
    0x00345805, 0x00000000, 0xa05b1b40, 0x3a025902,
    0x00031961, 0x35260220, 0x00345b05, 0x00000000,
    0x00131a61, 0x37260220, 0x00345c05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044c31, 0x00000000, 0xf3083524, 0x00025d14,
    0x00040070, 0x00018660, 0x26461b05, 0x00000000,
    0x80003a61, 0x33054660, 0x00000000, 0x00000002,
    0xa05e3c40, 0x01607803, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0xef5c3c62, 0x00113303,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x00031a61, 0x36060220, 0x00345e05, 0x00000000,
    0x80103c01, 0x00000000, 0x00000000, 0x00000000,
    0x00131b61, 0x38060220, 0x00345f05, 0x00000000,
    0x00041b61, 0x59070200, 0x00465c05, 0x00000000,
    0x27600070, 0x78005e03, 0x00041a61, 0x5e050020,
    0x00665907, 0x00000000, 0xa0621a40, 0x3a026002,
    0x00031961, 0x36260220, 0x00346205, 0x00000000,
    0x00131a61, 0x38260220, 0x00346305, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044d31, 0x00000000, 0xf3083624, 0x00025e14,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x00040024, 0x0001c060, 0x00001818, 0x00001818,
    0x80002401, 0x00000000, 0x00000000, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x00040b61, 0x63050020, 0x0066271f, 0x00000000,
    0x00040070, 0x00010660, 0x56466905, 0x00464505,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0xef4f0a62, 0x00006303, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x00040961, 0x64050020,
    0x00664f07, 0x00000000, 0x00040070, 0x00010220,
    0x52464305, 0x00464505, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x80040a61, 0x10014110,
    0x00000000, 0x0c800c80, 0x00040069, 0x10018510,
    0x01561906, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0c800c80, 0xe0660961, 0x001b0004,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x206d0966, 0x66004f03, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x00040961, 0x6f050020,
    0x00666d07, 0x00000000, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x80040961, 0x10014110,
    0x00000000, 0x0de00de0, 0x00040069, 0x10018510,
    0x01561106, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0de00de0, 0xe0710961, 0x001b0004,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x20730966, 0x71006f03, 0x80003601, 0x00000000,
    0x00000000, 0x00000000, 0x00040961, 0x75050020,
    0x00667307, 0x00000000, 0x80003601, 0x00000000,
    0x00000000, 0x00000000, 0x80040961, 0x10014110,
    0x00000000, 0x0ea00ea0, 0x00040069, 0x10018510,
    0x01561306, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0ea00ea0, 0xe0770961, 0x001b0004,
    0x20790966, 0x77007303, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x00040961, 0x6b070200,
    0x00007904, 0x00000000, 0x01040022, 0x0001c060,
    0x00000248, 0x00000188, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x00041261, 0x7b050020,
    0x00007904, 0x00000000, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0xac7d0970, 0x00007b03,
    0x00040070, 0x00018660, 0x16464f05, 0x00000000,
    0x01041a62, 0x1b058220, 0x02467d05, 0xffffffff,
    0x00041970, 0x00018660, 0x26461b05, 0x00000000,
    0x80002401, 0x00000000, 0x00000000, 0x00000000,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x01041562, 0x39058220, 0x02462905, 0xff800000,
    0x80002401, 0x00000000, 0x00000000, 0x00000000,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x01041762, 0x3b058220, 0x02462b05, 0xff800000,
    0x01042462, 0x3d058220, 0x02462d05, 0xff800000,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0x01042462, 0x33058220, 0x02462105, 0x7f800000,
    0x80002401, 0x00000000, 0x00000000, 0x00000000,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x01041762, 0x35058220, 0x02462305, 0x7f800000,
    0x80002401, 0x00000000, 0x00000000, 0x00000000,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x01041562, 0x37058220, 0x02462505, 0x7f800000,
    0x00040024, 0x0001c060, 0x000000d0, 0x000000d0,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x00040a61, 0x37054220, 0x00000000, 0x7f800000,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x00040c61, 0x35054220, 0x00000000, 0x7f800000,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0x00041e61, 0x33054220, 0x00000000, 0x7f800000,
    0x00041f61, 0x3d054220, 0x00000000, 0xff800000,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x00040f61, 0x3b054220, 0x00000000, 0xff800000,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x00041561, 0x39054220, 0x00000000, 0xff800000,
    0x00040025, 0x00004600, 0x00000000, 0x000013c8,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80040961, 0x10014110, 0x00000000, 0x06600660,
    0x00040069, 0x10018510, 0x01561906, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x06600660,
    0xe01c0961, 0x001b0004, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80040961, 0x10014110,
    0x00000000, 0x06a006a0, 0x00040069, 0x10018510,
    0x01561906, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x06a006a0, 0xe01e0961, 0x001b0004,
    0x80040961, 0x10014110, 0x00000000, 0x06e006e0,
    0x00040069, 0x10018510, 0x01561906, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x06e006e0,
    0xe0510961, 0x001b0004, 0x80040a61, 0x10014110,
    0x00000000, 0x07200720, 0x00040069, 0x10018510,
    0x01561906, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x07200720, 0xe0530961, 0x001b0004,
    0x80040d61, 0x10014110, 0x00000000, 0x07600760,
    0x00040069, 0x10018510, 0x01561906, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x07600760,
    0xe06d0961, 0x001b0004, 0x80040b61, 0x10014110,
    0x00000000, 0x07a007a0, 0x00040069, 0x10018510,
    0x01561906, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x07a007a0, 0xe06f0961, 0x001b0004,
    0x00040070, 0x00018220, 0x52464305, 0x00000006,
    0x27191a62, 0x1c003300, 0x25550c62, 0x53003900,
    0x271b1e62, 0x1e003500, 0x25570b62, 0x6d003b00,
    0x25590a62, 0x6f003d00, 0x271d1d62, 0x51003700,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80041461, 0x10014110, 0x00000000, 0x03200320,
    0x00040069, 0x10018510, 0x01561106, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x03200320,
    0xe01f0961, 0x001b0004, 0x80041561, 0x10014110,
    0x00000000, 0x0aa00aa0, 0x00040069, 0x10018510,
    0x01561106, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0aa00aa0, 0xe05b0961, 0x001b0004,
    0x80041161, 0x10014110, 0x00000000, 0x03600360,
    0x00040069, 0x10018510, 0x01561106, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x03600360,
    0xe0510961, 0x001b0004, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x80041361, 0x10014110,
    0x00000000, 0x0ae00ae0, 0x00040069, 0x10018510,
    0x01561106, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0ae00ae0, 0xe05d0961, 0x001b0004,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x80041261, 0x10014110, 0x00000000, 0x0b200b20,
    0x00040069, 0x10018510, 0x01561106, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0b200b20,
    0xe05f0961, 0x001b0004, 0x80041161, 0x10014110,
    0x00000000, 0x03a003a0, 0x00040069, 0x10018510,
    0x01561106, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x03a003a0, 0xe0530961, 0x001b0004,
    0x27111962, 0x1f001900, 0x27190962, 0x51001b00,
    0x251f1b62, 0x5d005700, 0x271b0962, 0x53001d00,
    0x251d0962, 0x5b005500, 0x80041261, 0x10014110,
    0x00000000, 0x02200220, 0x00040069, 0x10018510,
    0x01561306, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x02200220, 0xe0530961, 0x001b0004,
    0x80040961, 0x10014110, 0x00000000, 0x03200320,
    0x00040069, 0x10018510, 0x01561306, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x03200320,
    0xe0550961, 0x001b0004, 0x255b1c62, 0x5f005900,
    0x80041461, 0x10014110, 0x00000000, 0x03e003e0,
    0x00040069, 0x10018510, 0x01561306, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x03e003e0,
    0xe05d0961, 0x001b0004, 0x80040b61, 0x10014110,
    0x00000000, 0x03600360, 0x00040069, 0x10018510,
    0x01561306, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x03600360, 0xe0570961, 0x001b0004,
    0x80040961, 0x10014110, 0x00000000, 0x03a003a0,
    0x00040069, 0x10018510, 0x01561306, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x03a003a0,
    0xe0590961, 0x001b0004, 0x27511d62, 0x53001100,
    0x80041261, 0x10014110, 0x00000000, 0x0b600b60,
    0x00040069, 0x10018510, 0x01561306, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0b600b60,
    0xe05f0961, 0x001b0004, 0x27531d62, 0x55001900,
    0x27551b62, 0x57001b00, 0x25571a62, 0x59001d00,
    0x25591c62, 0x5d001f00, 0x255d1962, 0x5f005b00,
    0x01040022, 0x0001c060, 0x00000ec8, 0x00000ec8,
    0x251b0b62, 0x57aa5100, 0x25190b62, 0x59aa5300,
    0x251f0b62, 0x5daa5500, 0xa0650040, 0x01004b03,
    0x251d0962, 0x1f001900, 0x00040b61, 0x5b064540,
    0x00000000, 0x00010001, 0x00030a61, 0x11060220,
    0x00346505, 0x00000000, 0x00131b61, 0x13060220,
    0x00346605, 0x00000000, 0x25191162, 0x1d001b00,
    0x00041b61, 0x67050020, 0x00565b06, 0x00000000,
    0xe05b0941, 0x34001900, 0x20191140, 0x5b215100,
    0x201b0040, 0x5b215300, 0x201d0040, 0x5b215500,
    0x20510040, 0x5b015700, 0x20530040, 0x5b015900,
    0x20550040, 0x5b015d00, 0x20571340, 0x19205100,
    0x20511340, 0x1b205300, 0x20531340, 0x1d205500,
    0x00041341, 0x55058aa0, 0x0a465705, 0x3f800001,
    0x00041341, 0x57058aa0, 0x0a465105, 0x3f800001,
    0x00041341, 0x51058aa0, 0x0a465305, 0x3f800001,
    0x00041165, 0x53058220, 0x02465505, 0x007fffff,
    0x00041765, 0x59058220, 0x02465505, 0x80000000,
    0x00040065, 0x5f058220, 0x02465505, 0x7f800000,
    0x00041265, 0x5b058220, 0x02465705, 0x80000000,
    0x00040065, 0x61058220, 0x02465705, 0x7f800000,
    0x00041165, 0x5d058220, 0x02465105, 0x80000000,
    0x00040065, 0x63058220, 0x02465105, 0x7f800000,
    0x00040065, 0x55058220, 0x02465705, 0x007fffff,
    0x00040065, 0x57058220, 0x02465105, 0x007fffff,
    0x00041f40, 0x51058660, 0x06465305, 0x3f000000,
    0x00041f6c, 0x53058660, 0x02465f05, 0x00000017,
    0x00041c40, 0x5f058660, 0x06465505, 0x3f000000,
    0x00041f6c, 0x55058660, 0x02466105, 0x00000017,
    0x00041d40, 0x61058660, 0x06465705, 0x3f000000,
    0xac001d70, 0x3f805101, 0x00041f6c, 0x57058660,
    0x02466305, 0x00000017, 0x11040062, 0x63058220,
    0x02465105, 0x3f000000, 0x00040965, 0x51058220,
    0x02466305, 0x7fffffff, 0xac001e70, 0x3f805f01,
    0x20631966, 0x59005103, 0x11040062, 0x51058220,
    0x02465f05, 0x3f000000, 0x00041965, 0x59058220,
    0x02465105, 0x7fffffff, 0xac001f70, 0x3f806101,
    0x20511966, 0x5b005903, 0x11040062, 0x59058220,
    0x02466105, 0x3f000000, 0x00041965, 0x5b058220,
    0x02465905, 0x7fffffff, 0x00041970, 0x59058aa0,
    0x3a466305, 0x3f7f0000, 0x205f0966, 0x5d005b03,
    0x00041970, 0x5b058aa0, 0x3a465105, 0x3f7f0000,
    0x00041152, 0x51044560, 0x0e2eff82, 0x59055305,
    0x00041970, 0x59058aa0, 0x3a465f05, 0x3f7f0000,
    0x00041252, 0x53044560, 0x0e2eff82, 0x5b055505,
    0x275b0070, 0x4b006503, 0x00041152, 0x55044560,
    0x0e2eff82, 0x59055705, 0xa0571a40, 0x4d025b02,
    0x00031961, 0x11260220, 0x00345705, 0x00000000,
    0x00131a61, 0x13260220, 0x00345805, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044e31, 0x00000000, 0xf3081124, 0x00026714,
    0x00043461, 0x1f050660, 0x00460f05, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044f31, 0x00000000, 0xfb0c1524, 0x003c1944,
    0xa0580040, 0x01204b03, 0x00041f61, 0x5c070200,
    0x00465105, 0x00000000, 0x275a1a70, 0x4b005803,
    0x00033461, 0x09060220, 0x00345805, 0x00000000,
    0x00133461, 0x0b060220, 0x00345905, 0x00000000,
    0x00041461, 0x61070200, 0x00465305, 0x00000000,
    0x00041361, 0x63070200, 0x00465505, 0x00000000,
    0x00040961, 0x5f070000, 0x00665c07, 0x00000000,
    0xa05c1e40, 0x4d025a02, 0x00041a61, 0x5f0f0000,
    0x00666107, 0x00000000, 0x00031a61, 0x09260220,
    0x00345c05, 0x00000000, 0x00131b61, 0x0b260220,
    0x00345d05, 0x00000000, 0x00041b61, 0x5f170000,
    0x00666307, 0x00000000, 0x00041961, 0x5f1f0000,
    0x00666b07, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044d31, 0x00000000,
    0xfb080924, 0x00005f14, 0x00040070, 0x00010220,
    0x52464305, 0x00464505, 0x01040022, 0x0001c060,
    0x000004f0, 0x00000490, 0x25700062, 0x39433300,
    0x25720062, 0x3b433500, 0x25740062, 0x3d433700,
    0x00040069, 0x7605a660, 0x02465105, 0x00000017,
    0x00040069, 0x7805a660, 0x02465305, 0x00000017,
    0x00040069, 0x7a05a660, 0x02465505, 0x00000017,
    0x00040070, 0x00018660, 0x16464f05, 0x00000000,
    0x255d0962, 0x74007200, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x00041c40, 0x5f058660,
    0x06467605, 0x43800000, 0x00041c40, 0x61058660,
    0x06467805, 0x43800000, 0x00041c40, 0x63058660,
    0x06467a05, 0x43800000, 0x25651162, 0x5d007000,
    0x80003e01, 0x00000000, 0x00000000, 0x00000000,
    0xe0671141, 0x34006500, 0x206b0940, 0x67203300,
    0x206d0040, 0x67203500, 0x206f0040, 0x67203700,
    0x20710040, 0x67003900, 0x20730040, 0x67003b00,
    0x20751b40, 0x67003d00, 0x80003f01, 0x00000000,
    0x00000000, 0x00000000, 0x20770a40, 0x19206b00,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x20790940, 0x1b206d00, 0x80003f01, 0x00000000,
    0x00000000, 0x00000000, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x207b0940, 0x1d206f00,
    0x20361640, 0x19207100, 0x20381640, 0x1b207300,
    0x203a1640, 0x1d207500, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x00041641, 0x7d058aa0,
    0x0a467705, 0x3f7ffffe, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x00041641, 0x09058aa0,
    0x0a467905, 0x3f7ffffe, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x00041641, 0x0b058aa0,
    0x0a467b05, 0x3f7ffffe, 0x00041641, 0x3c058aa0,
    0x0a463605, 0x3f800001, 0x00040c41, 0x4f058aa0,
    0x0a463805, 0x3f800001, 0x00041641, 0x5d058aa0,
    0x0a463a05, 0x3f800001, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x200d0b41, 0x5f007d00,
    0x200f0a41, 0x61000900, 0x80003e01, 0x00000000,
    0x00000000, 0x00000000, 0x20110941, 0x63000b00,
    0x20651641, 0x5f003c00, 0x20671641, 0x61004f00,
    0x206b1641, 0x63005d00, 0x80003e01, 0x00000000,
    0x00000000, 0x00000000, 0x60131645, 0x00100d00,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x60151645, 0x00100f00, 0x80003f01, 0x00000000,
    0x00000000, 0x00000000, 0x60171645, 0x00101100,
    0x606d1645, 0x00126500, 0x606f1645, 0x00126700,
    0x60711645, 0x00126b00, 0xe5191662, 0x00001300,
    0xe51b1662, 0x00001500, 0xe51d1662, 0x00001700,
    0xe5731662, 0x00026d00, 0xe5751662, 0x00026f00,
    0xe5771662, 0x00027100, 0x80003f01, 0x00000000,
    0x00000000, 0x00000000, 0x00041662, 0x1f058aa0,
    0x5a461905, 0x437f0000, 0x00041662, 0x33058aa0,
    0x5a461b05, 0x437f0000, 0x00041662, 0x35058aa0,
    0x5a461d05, 0x437f0000, 0x00041662, 0x79058aa0,
    0x5a467305, 0x437f0000, 0x00041662, 0x7b058aa0,
    0x5a467505, 0x437f0000, 0x00041662, 0x7d058aa0,
    0x5a467705, 0x437f0000, 0x00041661, 0x51070a00,
    0x00461f05, 0x00000000, 0x00041561, 0x53070a00,
    0x00463305, 0x00000000, 0x00041461, 0x55070a00,
    0x00463505, 0x00000000, 0x00041361, 0x57070a00,
    0x00467905, 0x00000000, 0x00041261, 0x59070a00,
    0x00467b05, 0x00000000, 0x00041161, 0x5b070a00,
    0x00467d05, 0x00000000, 0x00041e61, 0x0a050010,
    0x00665107, 0x00000000, 0x00041e61, 0x0b050010,
    0x00665307, 0x00000000, 0x00041e61, 0x0c050010,
    0x00665507, 0x00000000, 0x00041e61, 0x7e050010,
    0x00665707, 0x00000000, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x00041e61, 0x7f050010,
    0x00665907, 0x00000000, 0x00041e61, 0x09050010,
    0x00665b07, 0x00000000, 0x11041e62, 0x57058110,
    0x01580a05, 0x00000000, 0x11041e62, 0x56058110,
    0x01580b05, 0x00000000, 0x11041e62, 0x55058110,
    0x01580c05, 0x00000000, 0x11041e62, 0x5a058110,
    0x01587e05, 0x00000000, 0x11041e62, 0x59058110,
    0x01587f05, 0x00000000, 0x11041e62, 0x58058110,
    0x01580905, 0x00000000, 0x00040024, 0x0001c060,
    0x00000070, 0x00000070, 0x00041d61, 0x55054110,
    0x00000000, 0x00800080, 0x00041f61, 0x56054110,
    0x00000000, 0x00800080, 0x00041f61, 0x57054110,
    0x00000000, 0x00800080, 0x00041d61, 0x58054110,
    0x00000000, 0x00000000, 0x00041f61, 0x59054110,
    0x00000000, 0x00000000, 0x00041f61, 0x5a054110,
    0x00000000, 0x00000000, 0x00040025, 0x00004600,
    0x00000000, 0x00000570, 0xa00d3440, 0x43004b02,
    0x80003e01, 0x00000000, 0x00000000, 0x00000000,
    0x00041f61, 0x67060100, 0x00585505, 0x00000000,
    0x80003e01, 0x00000000, 0x00000000, 0x00000000,
    0x00041f61, 0x68060100, 0x00585605, 0x00000000,
    0x00041f61, 0x6b060100, 0x00585705, 0x00000000,
    0x00041f61, 0x3d060100, 0x00585805, 0x00000000,
    0x00041f61, 0x7b060100, 0x00585905, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00041f61, 0x7c060100, 0x00585a05, 0x00000000,
    0x270f1f70, 0x4b000d03, 0xa0133e40, 0x01c00d03,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x00041e61, 0x60050020, 0x00566b06, 0x00000000,
    0x80003e01, 0x00000000, 0x00000000, 0x00000000,
    0xa0111b40, 0x4d020f02, 0x80003f01, 0x00000000,
    0x00000000, 0x00000000, 0x27151b70, 0x0d001303,
    0x00031161, 0x76060220, 0x00341305, 0x00000000,
    0x00131161, 0x78060220, 0x00341405, 0x00000000,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0xa0170b40, 0x11021502, 0x00031961, 0x76260220,
    0x00341705, 0x00000000, 0x00131a61, 0x78260220,
    0x00341805, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044031, 0x00000000,
    0xf3087624, 0x00026014, 0x80003f01, 0x00000000,
    0x00000000, 0x00000000, 0xa0181640, 0x02800d03,
    0x00040061, 0x65050020, 0x00566806, 0x00000000,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x271a0a70, 0x0d001803, 0x00033061, 0x61060220,
    0x00341805, 0x00000000, 0x00130061, 0x63060220,
    0x00341905, 0x00000000, 0x80003f01, 0x00000000,
    0x00000000, 0x00000000, 0xa01c0b40, 0x11021a02,
    0x00031961, 0x61260220, 0x00341c05, 0x00000000,
    0x00131a61, 0x63260220, 0x00341d05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x00000000, 0xf3086124, 0x00026514,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0xa01d1440, 0x03400d03, 0x00041761, 0x6f050020,
    0x00566706, 0x00000000, 0x80003f01, 0x00000000,
    0x00000000, 0x00000000, 0x271f1a70, 0x0d001d03,
    0x00030061, 0x6b060220, 0x00341d05, 0x00000000,
    0x00131761, 0x6d060220, 0x00341e05, 0x00000000,
    0xa0331b40, 0x11021f02, 0x00031961, 0x6b260220,
    0x00343305, 0x00000000, 0x00131a61, 0x6d260220,
    0x00343405, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xf3086b24, 0x00026f14, 0xa0340040, 0x02200d03,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x00041261, 0x74050020, 0x00567c06, 0x00000000,
    0x27361a70, 0x0d003403, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x00031761, 0x70060220,
    0x00343405, 0x00000000, 0x00131361, 0x72060220,
    0x00343505, 0x00000000, 0xa0381b40, 0x11023602,
    0x00031961, 0x70260220, 0x00343805, 0x00000000,
    0x00131a61, 0x72260220, 0x00343905, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x00000000, 0xf3087024, 0x00027414,
    0xa0390040, 0x02e00d03, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x00043061, 0x79050020,
    0x00567b06, 0x00000000, 0x273b1a70, 0x0d003903,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00033361, 0x75060220, 0x00343905, 0x00000000,
    0x00133061, 0x77060220, 0x00343a05, 0x00000000,
    0xa0431b40, 0x11023b02, 0x00031961, 0x75260220,
    0x00344305, 0x00000000, 0x00131a61, 0x77260220,
    0x00344405, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044431, 0x00000000,
    0xf3087524, 0x00027914, 0xa04b0040, 0x03a00d03,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00040061, 0x7e050020, 0x00563d06, 0x00000000,
    0x274d1a70, 0x0d004b03, 0x00033461, 0x7a060220,
    0x00344b05, 0x00000000, 0x00133061, 0x7c060220,
    0x00344c05, 0x00000000, 0xa04f1b40, 0x11024d02,
    0x00031961, 0x7a260220, 0x00344f05, 0x00000000,
    0x00131a61, 0x7c260220, 0x00345005, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x00000000, 0xf3087a24, 0x00027e14,
    0xa0500040, 0x01600d03, 0x00043061, 0x7d064540,
    0x00000000, 0x00020002, 0x27521a70, 0x0d005003,
    0x00033d61, 0x09060220, 0x00345005, 0x00000000,
    0x00133d61, 0x0b060220, 0x00345105, 0x00000000,
    0x00041c61, 0x0d050020, 0x00567d06, 0x00000000,
    0xa0541c40, 0x11025202, 0x00031961, 0x09260220,
    0x00345405, 0x00000000, 0x00131a61, 0x0b260220,
    0x00345505, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044431, 0x00000000,
    0xf3080924, 0x00020d14, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x00040070, 0x00010660,
    0x56466905, 0x00464505, 0x01040022, 0x0001c060,
    0x000006d8, 0x000006d8, 0x00040c6c, 0x55058660,
    0x02464505, 0x0000001f, 0x00041369, 0x57058660,
    0x02466905, 0x00000005, 0xe0591268, 0x01b06903,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80002101, 0x00000000, 0x00000000, 0x00000000,
    0xa05b0940, 0x57000502, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x275d0970, 0x05005b03,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x00031761, 0x0e060220, 0x00345b05, 0x00000000,
    0x80103401, 0x00000000, 0x00000000, 0x00000000,
    0x00131661, 0x10060220, 0x00345c05, 0x00000000,
    0x80002101, 0x00000000, 0x00000000, 0x00000000,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00040952, 0x5f040e68, 0x0e2e0705, 0x5d055905,
    0x00031961, 0x0e260220, 0x00345f05, 0x00000000,
    0x00131a61, 0x10260220, 0x00346005, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x00000000, 0xfb0c0e24, 0x003c2144,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0xa0611740, 0x01005b03, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x27630970, 0x5b006103,
    0x00033461, 0x0f060220, 0x00346105, 0x00000000,
    0x80103401, 0x00000000, 0x00000000, 0x00000000,
    0x00131661, 0x11060220, 0x00346205, 0x00000000,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0xa0650b40, 0x5f026302, 0x00031961, 0x0f260220,
    0x00346505, 0x00000000, 0x00131a61, 0x11260220,
    0x00346605, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044431, 0x00000000,
    0xfb0c0f24, 0x003c2944, 0x00041769, 0x66058660,
    0x02466905, 0x00000002, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0xe06b1668, 0x01e06903,
    0x80002101, 0x00000000, 0x00000000, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0xa06d0a40, 0x66000102, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x276f0970, 0x01006d03,
    0x00033461, 0x10060220, 0x00346d05, 0x00000000,
    0x00133461, 0x12060220, 0x00346e05, 0x00000000,
    0x80002101, 0x00000000, 0x00000000, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x00040b52, 0x71040e68, 0x0e2e0305, 0x6f056b05,
    0x00031961, 0x10260220, 0x00347105, 0x00000000,
    0x00131a61, 0x12260220, 0x00347205, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x00000000, 0xfb0c1024, 0x00046914,
    0xe0153468, 0x00613103, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0xa0721340, 0x69004502,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x27740970, 0x45007203, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x00041169, 0x78058660,
    0x02467205, 0x00000002, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0xe07b1268, 0x01e07203,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0xa0760940, 0x55027402, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0xa07e0940, 0x78000102,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00040969, 0x79058660, 0x02467605, 0x00000002,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x27091a70, 0x01007e03, 0x00033461, 0x11060220,
    0x00347e05, 0x00000000, 0x00133461, 0x13060220,
    0x00347f05, 0x00000000, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x207d0966, 0x7b007903,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x00041952, 0x0b040e68, 0x0e2e0305, 0x09057d05,
    0x00031961, 0x11260220, 0x00340b05, 0x00000000,
    0x00131a61, 0x13260220, 0x00340c05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x00000000, 0xfb0c1124, 0x00041514,
    0x00040065, 0x00010220, 0x22464705, 0x00464905,
    0x01040022, 0x0001c060, 0x00000220, 0x00000220,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x00040b69, 0x0c058660, 0x02464505, 0x00000003,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0xa00e1740, 0x02803f03, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x00041a66, 0x21058220,
    0x02460c05, 0xffffffc0, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x27100a70, 0x3f000e03,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x00031761, 0x16060220, 0x00340e05, 0x00000000,
    0x80103401, 0x00000000, 0x00000000, 0x00000000,
    0x00131561, 0x18060220, 0x00340f05, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0xa0120b40, 0x41021002, 0x00031961, 0x16260220,
    0x00341205, 0x00000000, 0x00131a61, 0x18260220,
    0x00341305, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044531, 0x13140000,
    0xfb041624, 0x00040000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x00042569, 0x15058660,
    0x02461305, 0x00000006, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0xa0171940, 0x15003f02,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x27190970, 0x3f001703, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x00031161, 0x1d060220,
    0x00341705, 0x00000000, 0x80103401, 0x00000000,
    0x00000000, 0x00000000, 0x00131261, 0x1f060220,
    0x00341805, 0x00000000, 0xa01b0b40, 0x41021902,
    0x00031961, 0x1d260220, 0x00341b05, 0x00000000,
    0x00131a61, 0x1f260220, 0x00341c05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x00000000, 0xfb0c1d24, 0x00042114,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80030961, 0x7e050220, 0x00460005, 0x00000000,
    0x80040931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_build_DFS_DFS_trivial_batchable = {
   .prog_data = {
      .base.nr_params = 10,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 15056,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_build_DFS_DFS_trivial_batchable_relocs,
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
      .push.cross_thread.dwords = 10,
      .push.cross_thread.regs = 2,
      .push.cross_thread.size = 64,
   },
   .args_size = 8,
   .arg_count = 1,
   .args = gfx125_bvh_build_DFS_DFS_trivial_batchable_args,
   .code = gfx125_bvh_build_DFS_DFS_trivial_batchable_code,
};
const char *gfx125_bvh_build_DFS_DFS_trivial_batchable_sha1 = "ecef4591a583a3de9acd4dfcb0d09eedabeb626c";
