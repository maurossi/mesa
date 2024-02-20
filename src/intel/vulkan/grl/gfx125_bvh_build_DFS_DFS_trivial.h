#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_build_DFS_DFS_trivial_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_build_DFS_DFS_trivial_args[] = {
   { 0, 8 },
   { 8, 8 },
   { 16, 8 },
   { 24, 8 },
   { 32, 4 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g13<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
add(1)          g14<1>UD        g13<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@1 compacted };
add(1)          g15<1>UD        g13<0,1,0>UD    0x00000040UD    { align1 WE_all 1N compacted };
mov(8)          g41<1>UW        0x76543210V                     { align1 WE_all 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(1)         g1UD            g14UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(1)         g3UD            g15UD           nullUD          0x0210c500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V8, transpose, L1STATE_L3MOCS dst_len = 1, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
add(8)          g41.8<1>UW      g41<1,1,0>UW    0x0008UW        { align1 WE_all 1Q I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         g67<1>D         g41<8,8,1>UW                    { align1 1H };
and(16)         g117<1>UD       g67<1,1,0>UD    0x0000000fUD    { align1 1H I@1 compacted };
mov(16)         g55<2>UW        g117<8,8,1>UD                   { align1 1H I@1 };
mov(16)         g105<1>UD       g117<16,8,2>UW                  { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g77.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g109.1<2>F      g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g91.1<2>F       g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g61.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g43.1<2>F       g2.5<0,1,0>F                    { align1 1Q };
mov(8)          g63.1<2>F       g2.5<0,1,0>F                    { align1 2Q };
mov(8)          g45.1<2>F       g2.7<0,1,0>F                    { align1 1Q };
mov(8)          g65.1<2>F       g2.7<0,1,0>F                    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
and(16)         g115<1>UD       g3<0,1,0>UD     0x00000001UD    { align1 1H compacted };
mov(8)          g77<2>F         g2<0,1,0>F                      { align1 1Q F@7 compacted };
mov(8)          g109<2>F        g2<0,1,0>F                      { align1 2Q F@7 compacted };
mov(8)          g91<2>F         g2.2<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g61<2>F         g2.2<0,1,0>F                    { align1 2Q F@7 compacted };
mov(8)          g43<2>F         g2.4<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g63<2>F         g2.4<0,1,0>F                    { align1 2Q F@7 compacted };
mov(8)          g45<2>F         g2.6<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g65<2>F         g2.6<0,1,0>F                    { align1 2Q F@7 compacted };
add(8)          g111<1>D        g77<8,4,2>D     36D             { align1 1Q F@7 compacted };
add(8)          g112<1>D        g109<8,4,2>D    36D             { align1 2Q F@7 compacted };
mov(8)          g73<2>UD        g111<4,4,1>UD                   { align1 1Q I@2 };
cmp.l.f0.0(16)  g113<1>UD       g111<1,1,0>UD   0x00000024UD    { align1 1H I@2 compacted };
mov(8)          g75<2>UD        g112<4,4,1>UD                   { align1 2Q };
cmp.nz.f0.0(16) g71<1>D         g115<1,1,0>D    0D              { align1 1H I@6 compacted };
add(8)          g78<1>D         -g113<8,8,1>D   g77.1<8,4,2>D   { align1 1Q I@3 };
add(8)          g114<1>D        -g114<8,8,1>D   g109.1<8,4,2>D  { align1 2Q I@4 };
mov(8)          g73.1<2>UD      g78<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g75.1<2>UD      g114<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g17UD           g73UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
mov(16)         g53<2>UW        g19<8,8,1>UD                    { align1 1H $1.dst };
mov(16)         g69<1>UD        g17<16,8,2>UW                   { align1 1H $1.dst };
cmp.l.f0.0(16)  null<1>D        g105<8,8,1>D    g69<8,8,1>D     { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL1          UIP:  LABEL0              { align1 1H };
shl(16)         g118<1>D        g105<8,8,1>D    0x00000005UD    { align1 1H };
shr(16)         g120<1>UD       g105<1,1,0>UD   0x0000001bUD    { align1 1H compacted };
mov(8)          g122<1>UD       g43.1<8,4,2>UD                  { align1 1Q F@4 };
mov(8)          g123<1>UD       g63.1<8,4,2>UD                  { align1 2Q F@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(8)          g124<1>D        g43<8,4,2>D     g118<1,1,0>D    { align1 1Q I@4 compacted };
add(8)          g125<1>D        g63<8,4,2>D     g119<1,1,0>D    { align1 2Q I@5 compacted };
cmp.l.f0.0(8)   g126<1>UD       g124<8,8,1>UD   g43<8,4,2>UD    { align1 1Q I@2 };
mov(8)          g12<2>UD        g124<4,4,1>UD                   { align1 1Q };
cmp.l.f0.0(8)   g127<1>UD       g125<8,8,1>UD   g63<8,4,2>UD    { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
mov(8)          g14<2>UD        g125<4,4,1>UD                   { align1 2Q };
add(16)         g3<1>D          g124<1,1,0>D    16D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
add3(16)        g1<1>D          g122<8,8,1>D    g120<8,8,1>D    -g126<1,1,1>D { align1 1H A@1 };
cmp.l.f0.0(16)  g5<1>UD         g3<1,1,0>UD     g124<1,1,0>UD   { align1 1H I@2 compacted };
mov(8)          g21<2>UD        g3<4,4,1>UD                     { align1 1Q };
mov(8)          g23<2>UD        g4<4,4,1>UD                     { align1 2Q };
mov(8)          g12.1<2>UD      g1<4,4,1>UD                     { align1 1Q I@4 };
mov(8)          g14.1<2>UD      g2<4,4,1>UD                     { align1 2Q I@5 };
add(16)         g7<1>D          -g5<1,1,0>D     g1<1,1,0>D      { align1 1H I@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g25UD           g12UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $0 };
mov(8)          g21.1<2>UD      g7<4,4,1>UD                     { align1 1Q I@1 };
mov(8)          g23.1<2>UD      g8<4,4,1>UD                     { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g33UD           g21UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
else(16)        JIP:  LABEL0          UIP:  LABEL0              { align1 1H };

LABEL1:
mov(16)         g31<1>UD        0x00000000UD                    { align1 1H $0.dst };
mov(16)         g29<1>UD        0x7f800000UD                    { align1 1H $0.dst };
mov(16)         g27<1>UD        0x7f800000UD                    { align1 1H $0.dst };
mov(16)         g25<1>UD        0x7f800000UD                    { align1 1H $0.dst };
mov(16)         g39<1>UD        0x80000000UD                    { align1 1H $2.dst };
mov(16)         g37<1>UD        0xff800000UD                    { align1 1H $2.dst };
mov(16)         g35<1>UD        0xff800000UD                    { align1 1H $2.dst };
mov(16)         g33<1>UD        0xff800000UD                    { align1 1H $2.dst };

LABEL0:
endif(16)       JIP:  LABEL2                                    { align1 1H };

LABEL2:
cmp.z.f0.0(16)  g8<1>W          g55<16,8,2>W    0W              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov.nz.f0.0(16) g73<1>D         g8<8,8,1>W                      { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL3          UIP:  LABEL3              { align1 1H };
add(8)          g79<1>D         g91<8,4,2>D     12D             { align1 1Q F@6 compacted };
add(8)          g9<1>D          g61<8,4,2>D     12D             { align1 2Q F@5 compacted };
cmp.l.f0.0(8)   g80<1>UD        g79<8,8,1>UD    g91<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g10<1>UD        g9<8,8,1>UD     g61<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g75<2>UD        g79<4,4,1>UD                    { align1 1Q $1.src };
mov(8)          g77<2>UD        g9<4,4,1>UD                     { align1 2Q };
add(8)          g81<1>D         -g80<8,8,1>D    g91.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g11<1>D         -g10<8,8,1>D    g61.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g75.1<2>UD      g81<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g77.1<2>UD      g11<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g12UD           g75UD           nullUD          0x08240588                0x00000000
                            ugm MsgDesc: ( atomic_inc, a64, d32, V1, L1UC_L3WB dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
shl(16)         g47<1>D         g12<8,8,1>D     0x00000006UD    { align1 1H $1.dst };

LABEL3:
endif(16)       JIP:  LABEL4                                    { align1 1H };

LABEL4:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g21<1>UD        g91<8,4,2>UD                    { align1 1Q A@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $2.src };
mov(8)          g22<1>UD        g61<8,4,2>UD                    { align1 2Q A@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
add(8)          g75<1>D         g91<8,4,2>D     g47<0,1,0>D     { align1 1Q I@4 compacted };
add(8)          g76<1>D         g61<8,4,2>D     g47<0,1,0>D     { align1 2Q $1.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(8)          g95<1>D         g91<8,4,2>D     16D             { align1 1Q compacted };
add(8)          g24<1>D         g61<8,4,2>D     16D             { align1 2Q $2.src compacted };
add(16)         g83<1>D         g67<1,1,0>D     4D              { align1 1H compacted };
add(16)         g85<1>D         g67<1,1,0>D     -12D            { align1 1H compacted };
add(16)         g87<1>D         g67<1,1,0>D     2D              { align1 1H compacted };
add(16)         g89<1>D         g67<1,1,0>D     -14D            { align1 1H compacted };
add(16)         g41<1>D         g67<1,1,0>D     1D              { align1 1H compacted };
add(16)         g93<1>D         g67<1,1,0>D     -15D            { align1 1H compacted };
cmp.l.f0.0(8)   g82<1>UD        g75<8,8,1>UD    g91<8,4,2>UD    { align1 1Q I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $2.src };
cmp.l.f0.0(8)   g23<1>UD        g76<8,8,1>UD    g61<8,4,2>UD    { align1 2Q I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(8)          g13<2>UD        g75<4,4,1>UD                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
mov(8)          g15<2>UD        g76<4,4,1>UD                    { align1 2Q };
mov(8)          g49<2>UD        g95<4,4,1>UD                    { align1 1Q };
mov(8)          g51<2>UD        g24<4,4,1>UD                    { align1 2Q };
cmp.l.f0.0(8)   g96<1>UD        g95<8,8,1>UD    g91<8,4,2>UD    { align1 1Q };
cmp.l.f0.0(8)   g55<1>UD        g24<8,8,1>UD    g61<8,4,2>UD    { align1 2Q };
cmp.l.f0.0(16)  null<1>D        g83<8,8,1>D     16D             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
add(8)          g77<1>D         -g82<8,8,1>D    g91.1<8,4,2>D   { align1 1Q I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $1.src };
add(8)          g78<1>D         -g23<8,8,1>D    g61.1<8,4,2>D   { align1 2Q I@7 };
add(8)          g97<1>D         -g96<8,8,1>D    g91.1<8,4,2>D   { align1 1Q I@5 };
add(8)          g56<1>D         -g55<8,8,1>D    g61.1<8,4,2>D   { align1 2Q I@5 };
(+f0.0) sel(16) g81<1>UD        g83<1,1,0>UD    g85<1,1,0>UD    { align1 1H compacted };
mov(8)          g13.1<2>UD      g77<4,4,1>UD                    { align1 1Q I@5 };
mov(8)          g15.1<2>UD      g78<4,4,1>UD                    { align1 2Q I@5 };
mov(8)          g49.1<2>UD      g97<4,4,1>UD                    { align1 1Q I@5 };
mov(8)          g51.1<2>UD      g56<4,4,1>UD                    { align1 2Q I@5 };
cmp.l.f0.0(16)  null<1>D        g87<8,8,1>D     16D             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g57UD           g49UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
(+f0.0) sel(16) g9<1>UD         g87<1,1,0>UD    g89<1,1,0>UD    { align1 1H compacted };
cmp.l.f0.0(16)  null<1>D        g41<8,8,1>D     16D             { align1 1H };
(+f0.0) sel(16) g11<1>UD        g41<1,1,0>UD    g93<1,1,0>UD    { align1 1H $0.src compacted };
cmp.z.f0.0(16)  g94<1>W         g53<16,8,2>W    1W              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov.nz.f0.0(16) g49<1>D         g94<8,8,1>W                     { align1 1H I@1 };
shl(16)         g59<1>D         g57<8,8,1>D     0x00000006UD    { align1 1H $3.dst };
add3(16)        g79<1>D         g21<8,8,1>D     g59<8,8,1>D     -g75<1,1,1>D { align1 1H I@1 };
asr(16)         g7<1>D          g79<8,8,1>D     0x00000006UD    { align1 1H I@1 };
(-f0.0) if(16)  JIP:  LABEL6          UIP:  LABEL5              { align1 1H };
cmp.l.f0.0(16)  null<1>UD       g67<8,8,1>UD    g69<8,8,1>UD    { align1 1H };
(+f0.0) sel(16) g83<1>UD        g33<8,8,1>UD    0xff800000UD    { align1 1H $2.dst };
(+f0.0) sel(16) g85<1>UD        g35<8,8,1>UD    0xff800000UD    { align1 1H $2.dst };
(+f0.0) sel(16) g87<1>UD        g37<8,8,1>UD    0xff800000UD    { align1 1H $2.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
(+f0.0) sel(16) g51<1>UD        g25<8,8,1>UD    0x7f800000UD    { align1 1H $0.dst };
(+f0.0) sel(16) g55<1>UD        g27<8,8,1>UD    0x7f800000UD    { align1 1H $0.dst };
(+f0.0) sel(16) g57<1>UD        g29<8,8,1>UD    0x7f800000UD    { align1 1H $0.dst };
mov(16)         a0<1>UW         0x0a60UW                        { align1 WE_all 1H I@6 };
shl(16)         a0<1>UW         g81<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0a60UW        { align1 1H A@1 };
mov(16)         g41<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0aa0UW                        { align1 WE_all 1H I@6 };
shl(16)         a0<1>UW         g81<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0aa0UW        { align1 1H A@1 };
mov(16)         g89<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0ae0UW                        { align1 WE_all 1H I@6 };
shl(16)         a0<1>UW         g81<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0ae0UW        { align1 1H A@1 };
mov(16)         g23<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0660UW                        { align1 WE_all 1H I@6 };
shl(16)         a0<1>UW         g81<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0660UW        { align1 1H A@1 };
mov(16)         g53<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x06e0UW                        { align1 WE_all 1H I@6 };
shl(16)         a0<1>UW         g81<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x06e0UW        { align1 1H A@1 };
mov(16)         g59<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0720UW                        { align1 WE_all 1H I@6 };
shl(16)         a0<1>UW         g81<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0720UW        { align1 1H A@1 };
mov(16)         g79<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
cmp.l.f0.0(16)  null<1>UD       g67<8,8,1>UD    0x00000006UD    { align1 1H };
sel.ge(16)      g21<1>F         g83<1,1,0>F     g41<1,1,0>F     { align1 1H I@7 compacted };
sel.ge(16)      g83<1>F         g85<1,1,0>F     g89<1,1,0>F     { align1 1H A@1 compacted };
sel.ge(16)      g89<1>F         g87<1,1,0>F     g23<1,1,0>F     { align1 1H I@5 compacted };
sel.l(16)       g23<1>F         g51<1,1,0>F     g53<1,1,0>F     { align1 1H I@4 compacted };
sel.l(16)       g53<1>F         g55<1,1,0>F     g59<1,1,0>F     { align1 1H I@3 compacted };
sel.l(16)       g55<1>F         g57<1,1,0>F     g79<1,1,0>F     { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0a60UW                        { align1 WE_all 1H A@4 };
shl(16)         a0<1>UW         g9<16,8,2>W     0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0a60UW        { align1 1H A@1 };
mov(16)         g87<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x02a0UW                        { align1 WE_all 1H F@1 };
shl(16)         a0<1>UW         g9<16,8,2>W     0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x02a0UW        { align1 1H A@1 };
mov(16)         g79<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0b20UW                        { align1 WE_all 1H F@4 };
shl(16)         a0<1>UW         g9<16,8,2>W     0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0b20UW        { align1 1H A@1 };
mov(16)         g41<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x02e0UW                        { align1 WE_all 1H A@1 };
shl(16)         a0<1>UW         g9<16,8,2>W     0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x02e0UW        { align1 1H A@1 };
mov(16)         g57<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x06a0UW                        { align1 WE_all 1H F@2 };
shl(16)         a0<1>UW         g9<16,8,2>W     0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x06a0UW        { align1 1H A@1 };
mov(16)         g59<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x06e0UW                        { align1 WE_all 1H F@1 };
shl(16)         a0<1>UW         g9<16,8,2>W     0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x06e0UW        { align1 1H A@1 };
mov(16)         g85<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sel.ge(16)      g51<1>F         g21<1,1,0>F     g79<1,1,0>F     { align1 1H A@3 compacted };
sel.ge(16)      g79<1>F         g83<1,1,0>F     g87<1,1,0>F     { align1 1H I@6 compacted };
sel.l(16)       g21<1>F         g23<1,1,0>F     g57<1,1,0>F     { align1 1H A@2 compacted };
sel.ge(16)      g87<1>F         g89<1,1,0>F     g41<1,1,0>F     { align1 1H I@4 compacted };
sel.l(16)       g23<1>F         g53<1,1,0>F     g59<1,1,0>F     { align1 1H A@2 compacted };
sel.l(16)       g83<1>F         g55<1,1,0>F     g85<1,1,0>F     { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0660UW                        { align1 WE_all 1H F@2 };
shl(16)         a0<1>UW         g11<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0660UW        { align1 1H A@1 };
mov(16)         g59<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x09e0UW                        { align1 WE_all 1H F@3 };
shl(16)         a0<1>UW         g11<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x09e0UW        { align1 1H A@1 };
mov(16)         g41<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x02a0UW                        { align1 WE_all 1H A@2 };
shl(16)         a0<1>UW         g11<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x02a0UW        { align1 1H A@1 };
mov(16)         g53<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0ae0UW                        { align1 WE_all 1H A@3 };
shl(16)         a0<1>UW         g11<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0ae0UW        { align1 1H A@1 };
mov(16)         g89<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x02e0UW                        { align1 WE_all 1H A@1 };
shl(16)         a0<1>UW         g11<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x02e0UW        { align1 1H A@1 };
mov(16)         g55<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0a60UW                        { align1 WE_all 1H F@1 };
shl(16)         a0<1>UW         g11<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0a60UW        { align1 1H A@1 };
mov(16)         g85<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sel.ge(16)      g57<1>F         g51<1,1,0>F     g59<1,1,0>F     { align1 1H I@6 compacted };
sel.l(16)       g51<1>F         g21<1,1,0>F     g53<1,1,0>F     { align1 1H A@1 compacted };
sel.ge(16)      g59<1>F         g79<1,1,0>F     g41<1,1,0>F     { align1 1H I@5 compacted };
sel.l(16)       g53<1>F         g23<1,1,0>F     g55<1,1,0>F     { align1 1H I@2 compacted };
sel.ge(16)      g79<1>F         g87<1,1,0>F     g89<1,1,0>F     { align1 1H A@2 compacted };
sel.l(16)       g55<1>F         g83<1,1,0>F     g85<1,1,0>F     { align1 1H I@1 compacted };
(+f0.0) if(16)  JIP:  LABEL7          UIP:  LABEL7              { align1 1H };
sel.ge(16)      g83<1>F         (abs)g51<0,1,0>F (abs)g57<0,1,0>F { align1 1H A@1 compacted };
sel.ge(16)      g85<1>F         (abs)g53<0,1,0>F (abs)g59<0,1,0>F { align1 1H F@2 compacted };
sel.ge(16)      g87<1>F         (abs)g55<0,1,0>F (abs)g79<0,1,0>F { align1 1H A@3 compacted };
add(16)         g41<1>D         g75<1,1,0>D     16D             { align1 1H F@7 compacted };
mov(16)         g95<2>B         -1W                             { align1 1H };
sel.ge(16)      g89<1>F         g85<1,1,0>F     g87<1,1,0>F     { align1 1H F@1 compacted };
mov(16)         g93<4>UB        g19<8,8,1>UD                    { align1 1H };
mov(8)          g21<2>UD        g41<4,4,1>UD                    { align1 1Q A@3 };
mov(8)          g23<2>UD        g42<4,4,1>UD                    { align1 2Q A@4 };
sel.ge(16)      g19<1>F         g83<1,1,0>F     g89<1,1,0>F     { align1 1H A@1 compacted };
mov(16)         g17<1>UD        g93<32,8,4>UB                   { align1 1H I@3 };
mul(16)         g96<1>F         g19<1,1,0>F     0x34000000F  /* 1.19209e-07F */ { align1 1H F@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
add(16)         g1<1>F          g51<0,1,0>F     -g96<1,1,0>F    { align1 1H compacted };
add(16)         g3<1>F          g53<0,1,0>F     -g96<1,1,0>F    { align1 1H compacted };
add(16)         g5<1>F          g55<0,1,0>F     -g96<1,1,0>F    { align1 1H compacted };
add(16)         g98<1>F         g57<0,1,0>F     g96<1,1,0>F     { align1 1H compacted };
add(16)         g100<1>F        g59<0,1,0>F     g96<1,1,0>F     { align1 1H compacted };
add(16)         g102<1>F        g79<0,1,0>F     g96<1,1,0>F     { align1 1H compacted };
add(16)         g107<1>F        g98<1,1,0>F     -g1<1,1,0>F     { align1 1H F@3 compacted };
add(16)         g109<1>F        g100<1,1,0>F    -g3<1,1,0>F     { align1 1H F@3 compacted };
add(16)         g111<1>F        g102<1,1,0>F    -g5<1,1,0>F     { align1 1H F@3 compacted };
mul(16)         g113<1>F        g107<8,8,1>F    0x3f800001F  /* 1F */ { align1 1H F@3 };
mul(16)         g55<1>F         g109<8,8,1>F    0x3f800001F  /* 1F */ { align1 1H F@3 };
mul(16)         g51<1>F         g111<8,8,1>F    0x3f800001F  /* 1F */ { align1 1H F@3 };
and(16)         g19<1>UD        g113<8,8,1>UD   0x007fffffUD    { align1 1H F@3 };
and(16)         g79<1>UD        g113<8,8,1>UD   0x80000000UD    { align1 1H F@7 };
and(16)         g57<1>UD        g113<8,8,1>UD   0x7f800000UD    { align1 1H F@7 };
and(16)         g53<1>UD        g55<8,8,1>UD    0x007fffffUD    { align1 1H F@2 };
and(16)         g83<1>UD        g55<8,8,1>UD    0x80000000UD    { align1 1H };
and(16)         g59<1>UD        g55<8,8,1>UD    0x7f800000UD    { align1 1H F@7 };
and(16)         g85<1>UD        g51<8,8,1>UD    0x80000000UD    { align1 1H F@1 };
and(16)         g87<1>UD        g51<8,8,1>UD    0x7f800000UD    { align1 1H };
and(16)         g55<1>UD        g51<8,8,1>UD    0x007fffffUD    { align1 1H };
add(16)         g51<1>D         g19<8,8,1>D     1056964608D     { align1 1H I@7 };
asr(16)         g19<1>D         g57<8,8,1>D     0x00000017UD    { align1 1H I@7 };
add(16)         g57<1>D         g53<8,8,1>D     1056964608D     { align1 1H I@7 };
asr(16)         g53<1>D         g59<8,8,1>D     0x00000017UD    { align1 1H I@7 };
add(16)         g59<1>D         g55<8,8,1>D     1056964608D     { align1 1H I@5 };
cmp.z.f0.0(16)  null<1>F        g51<1,1,0>F     0x3f800000F  /* 1F */ { align1 1H I@5 compacted };
asr(16)         g55<1>D         g87<8,8,1>D     0x00000017UD    { align1 1H I@7 };
(-f0.0) sel(16) g87<1>UD        g51<8,8,1>UD    0x3f000000UD    { align1 1H };
and(16)         g51<1>UD        g87<8,8,1>UD    0x7fffffffUD    { align1 1H A@1 };
cmp.z.f0.0(16)  null<1>F        g57<1,1,0>F     0x3f800000F  /* 1F */ { align1 1H I@6 compacted };
or(16)          g87<1>UD        g51<1,1,0>UD    g79<1,1,0>UD    { align1 1H I@1 compacted };
(-f0.0) sel(16) g79<1>UD        g57<8,8,1>UD    0x3f000000UD    { align1 1H };
and(16)         g89<1>UD        g79<8,8,1>UD    0x7fffffffUD    { align1 1H I@1 };
cmp.z.f0.0(16)  null<1>F        g59<1,1,0>F     0x3f800000F  /* 1F */ { align1 1H I@7 compacted };
or(16)          g93<1>UD        g89<1,1,0>UD    g83<1,1,0>UD    { align1 1H I@1 compacted };
(-f0.0) sel(16) g114<1>UD       g59<8,8,1>UD    0x3f000000UD    { align1 1H };
and(16)         g116<1>UD       g114<8,8,1>UD   0x7fffffffUD    { align1 1H I@1 };
cmp.g.f0.0(16)  g96<1>F         g87<8,8,1>F     0x3f7f0000F  /* 0.996094F */ { align1 1H I@6 };
cmp.g.f0.0(16)  g120<1>F        g93<8,8,1>F     0x3f7f0000F  /* 0.996094F */ { align1 1H I@3 };
or(16)          g118<1>UD       g116<1,1,0>UD   g85<1,1,0>UD    { align1 1H I@1 compacted };
add3(16)        g51<1>D         65410W          g19<8,8,1>D     -g96<1,1,1>D { align1 1H F@2 };
add3(16)        g57<1>D         65410W          g53<8,8,1>D     -g120<1,1,1>D { align1 1H A@1 };
cmp.g.f0.0(16)  g122<1>F        g118<8,8,1>F    0x3f7f0000F  /* 0.996094F */ { align1 1H I@3 };
cmp.l.f0.0(16)  g124<1>UD       g41<1,1,0>UD    g75<1,1,0>UD    { align1 1H compacted };
add3(16)        g59<1>D         65410W          g55<8,8,1>D     -g122<1,1,1>D { align1 1H A@1 };
add(16)         g126<1>D        -g124<1,1,0>D   g77<1,1,0>D     { align1 1H I@2 compacted };
mov(8)          g21.1<2>UD      g126<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g23.1<2>UD      g127<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g21UD           g17UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $4 };
send(16)        nullUD          g13UD           g1UD            0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $0 };
add(16)         g21<1>D         g75<1,1,0>D     18D             { align1 1H $4.src compacted };
mov(16)         g108<4>UB       g51<8,8,1>UD                    { align1 1H A@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
cmp.l.f0.0(16)  g23<1>UD        g21<1,1,0>UD    g75<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g17<2>UD        g21<4,4,1>UD                    { align1 1Q $4.src };
mov(8)          g19<2>UD        g22<4,4,1>UD                    { align1 2Q };
mov(16)         g110<4>UB       g57<8,8,1>UD                    { align1 1H F@7 };
mov(16)         g112<4>UB       g59<8,8,1>UD                    { align1 1H A@7 };
mov(16)         g79<4>UB        g108<32,8,4>UB                  { align1 1H I@6 };
add(16)         g53<1>D         -g23<1,1,0>D    g77<1,1,0>D     { align1 1H I@6 compacted };
mov(16)         g79.1<4>UB      g110<32,8,4>UB                  { align1 1H I@2 };
mov(8)          g17.1<2>UD      g53<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g19.1<2>UD      g54<4,4,1>UD                    { align1 2Q I@3 };
mov(16)         g79.2<4>UB      g112<32,8,4>UB                  { align1 1H I@3 };
mov(16)         g79.3<4>UB      g95<16,8,2>UB                   { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g17UD           g79UD           0x08000584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $5 };
cmp.l.f0.0(16)  null<1>UD       g67<8,8,1>UD    g69<8,8,1>UD    { align1 1H };
(+f0.0) if(16)  JIP:  LABEL9          UIP:  LABEL8              { align1 1H };
sel.ge(16)      g19<1>F         (abs)g25<1,1,0>F (abs)g33<1,1,0>F { align1 1H $5.src compacted };
sel.ge(16)      g53<1>F         (abs)g27<1,1,0>F (abs)g35<1,1,0>F { align1 1H I@5 compacted };
sel.ge(16)      g55<1>F         (abs)g29<1,1,0>F (abs)g37<1,1,0>F { align1 1H compacted };
shl(16)         g23<1>D         -g51<8,8,1>D    0x00000017UD    { align1 1H I@7 };
shl(16)         g17<1>D         -g59<8,8,1>D    0x00000017UD    { align1 1H $5.src };
shl(16)         g51<1>D         -g57<8,8,1>D    0x00000017UD    { align1 1H };
sel.ge(16)      g21<1>F         g53<1,1,0>F     g55<1,1,0>F     { align1 1H F@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
add(16)         g79<1>D         g17<8,8,1>D     1132462080D     { align1 1H I@2 };
add(16)         g57<1>D         g23<8,8,1>D     1132462080D     { align1 1H I@4 };
add(16)         g59<1>D         g51<8,8,1>D     1132462080D     { align1 1H I@3 };
sel.ge(16)      g54<1>F         g19<1,1,0>F     g21<1,1,0>F     { align1 1H F@1 compacted };
mul(16)         g17<1>F         g54<1,1,0>F     0x34000000F  /* 1.19209e-07F */ { align1 1H A@1 compacted };
add(16)         g19<1>F         g25<1,1,0>F     -g17<1,1,0>F    { align1 1H F@1 compacted };
add(16)         g21<1>F         g27<1,1,0>F     -g17<1,1,0>F    { align1 1H compacted };
add(16)         g23<1>F         g29<1,1,0>F     -g17<1,1,0>F    { align1 1H I@2 compacted };
add(16)         g51<1>F         g33<1,1,0>F     g17<1,1,0>F     { align1 1H I@1 compacted };
add(16)         g53<1>F         g35<1,1,0>F     g17<1,1,0>F     { align1 1H compacted };
add(16)         g55<1>F         g37<1,1,0>F     g17<1,1,0>F     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(16)         g17<1>F         g19<1,1,0>F     -g1<1,1,0>F     { align1 1H F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(16)         g19<1>F         g21<1,1,0>F     -g3<1,1,0>F     { align1 1H F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(16)         g21<1>F         g23<1,1,0>F     -g5<1,1,0>F     { align1 1H F@6 compacted };
add(16)         g23<1>F         g51<1,1,0>F     -g1<1,1,0>F     { align1 1H F@6 compacted };
add(16)         g51<1>F         g53<1,1,0>F     -g3<1,1,0>F     { align1 1H F@6 compacted };
add(16)         g53<1>F         g55<1,1,0>F     -g5<1,1,0>F     { align1 1H F@6 compacted };
mul(16)         g55<1>F         g17<8,8,1>F     0x3f7ffffeF  /* 1F */ { align1 1H F@6 };
mul(16)         g17<1>F         g19<8,8,1>F     0x3f7ffffeF  /* 1F */ { align1 1H F@6 };
mul(16)         g19<1>F         g21<8,8,1>F     0x3f7ffffeF  /* 1F */ { align1 1H F@6 };
mul(16)         g21<1>F         g23<8,8,1>F     0x3f800001F  /* 1F */ { align1 1H F@6 };
mul(16)         g23<1>F         g51<8,8,1>F     0x3f800001F  /* 1F */ { align1 1H F@6 };
mul(16)         g51<1>F         g53<8,8,1>F     0x3f800001F  /* 1F */ { align1 1H F@6 };
mul(16)         g53<1>F         g55<1,1,0>F     g57<1,1,0>F     { align1 1H A@2 compacted };
mul(16)         g55<1>F         g17<1,1,0>F     g59<1,1,0>F     { align1 1H A@1 compacted };
mul(16)         g17<1>F         g19<1,1,0>F     g79<1,1,0>F     { align1 1H A@3 compacted };
mul(16)         g19<1>F         g21<1,1,0>F     g57<1,1,0>F     { align1 1H F@6 compacted };
mul(16)         g21<1>F         g23<1,1,0>F     g59<1,1,0>F     { align1 1H F@6 compacted };
rndd(16)        g95<1>F         g53<1,1,0>F                     { align1 1H A@5 compacted };
mul(16)         g23<1>F         g51<1,1,0>F     g79<1,1,0>F     { align1 1H F@7 compacted };
rndd(16)        g97<1>F         g55<1,1,0>F                     { align1 1H F@6 compacted };
rndd(16)        g99<1>F         g17<1,1,0>F                     { align1 1H F@6 compacted };
rndd(16)        g101<1>F        -g19<1,1,0>F                    { align1 1H F@6 compacted };
rndd(16)        g103<1>F        -g21<1,1,0>F                    { align1 1H F@6 compacted };
sel.ge(16)      g109<1>F        g95<1,1,0>F     0x0F  /* 0F */  { align1 1H F@6 compacted };
rndd(16)        g107<1>F        -g23<1,1,0>F                    { align1 1H F@6 compacted };
sel.ge(16)      g111<1>F        g97<1,1,0>F     0x0F  /* 0F */  { align1 1H A@6 compacted };
sel.ge(16)      g113<1>F        g99<1,1,0>F     0x0F  /* 0F */  { align1 1H A@6 compacted };
sel.ge(16)      g115<1>F        -g101<1,1,0>F   0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.ge(16)      g117<1>F        -g103<1,1,0>F   0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.l(16)       g55<1>F         g109<8,8,1>F    0x437f0000F  /* 255F */ { align1 1H F@6 };
sel.ge(16)      g119<1>F        -g107<1,1,0>F   0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.l(16)       g57<1>F         g111<8,8,1>F    0x437f0000F  /* 255F */ { align1 1H F@6 };
sel.l(16)       g59<1>F         g113<8,8,1>F    0x437f0000F  /* 255F */ { align1 1H F@6 };
sel.l(16)       g79<1>F         g115<8,8,1>F    0x437f0000F  /* 255F */ { align1 1H F@6 };
sel.l(16)       g95<1>F         g117<8,8,1>F    0x437f0000F  /* 255F */ { align1 1H F@6 };
mov(16)         g83<4>UB        g55<8,8,1>F                     { align1 1H F@6 };
sel.l(16)       g97<1>F         g119<8,8,1>F    0x437f0000F  /* 255F */ { align1 1H F@5 };
mov(16)         g85<4>UB        g57<8,8,1>F                     { align1 1H F@5 };
mov(16)         g87<4>UB        g59<8,8,1>F                     { align1 1H F@4 };
mov(16)         g89<4>UB        g79<8,8,1>F                     { align1 1H F@3 };
mov(16)         g41<4>UB        g95<8,8,1>F                     { align1 1H F@2 };
mov(16)         g93<4>UB        g97<8,8,1>F                     { align1 1H F@1 };
mov(16)         g96<1>UW        g85<32,8,4>UB                   { align1 1H I@5 };
mov(16)         g97<1>UW        g83<32,8,4>UB                   { align1 1H I@7 };
mov(16)         g95<1>UW        g87<32,8,4>UB                   { align1 1H I@6 };
mov(16)         g100<1>UW       g89<32,8,4>UB                   { align1 1H A@6 };
mov(16)         g99<1>UW        g41<32,8,4>UB                   { align1 1H A@6 };
mov(16)         g98<1>UW        g93<32,8,4>UB                   { align1 1H I@6 };
else(16)        JIP:  LABEL8          UIP:  LABEL8              { align1 1H };

LABEL9:
mov(16)         g95<1>UW        0x0080UW                        { align1 1H I@3 };
mov(16)         g96<1>UW        0x0080UW                        { align1 1H I@7 };
mov(16)         g97<1>UW        0x0080UW                        { align1 1H I@7 };
mov(16)         g98<1>UW        0x0000UW                        { align1 1H I@5 };
mov(16)         g99<1>UW        0x0000UW                        { align1 1H I@7 };
mov(16)         g100<1>UW       0x0000UW                        { align1 1H I@7 };

LABEL8:
endif(16)       JIP:  LABEL7                                    { align1 1H };
add(16)         g120<1>D        g75<1,1,0>D     g67<1,1,0>D     { align1 1H F@1 compacted };
mov(16)         g115<2>UB       g95<16,16,1>UW                  { align1 1H A@3 };
mov(16)         g116<2>UB       g96<16,16,1>UW                  { align1 1H A@3 };
mov(16)         g117<2>UB       g97<16,16,1>UW                  { align1 1H A@2 };
mov(16)         g56<2>UB        g98<16,16,1>UW                  { align1 1H I@7 };
mov(16)         g57<2>UB        g99<16,16,1>UW                  { align1 1H I@7 };
mov(16)         g124<2>UB       g100<16,16,1>UW                 { align1 1H I@7 };
cmp.l.f0.0(16)  g98<1>UD        g120<1,1,0>UD   g75<1,1,0>UD    { align1 1H I@7 compacted };
add(16)         g100<1>D        g120<1,1,0>D    28D             { align1 1H F@7 compacted };
mov(16)         g83<1>UD        g117<16,8,2>UB                  { align1 1H I@6 };
add(16)         g122<1>D        -g98<1,1,0>D    g77<1,1,0>D     { align1 1H I@3 compacted };
cmp.l.f0.0(16)  g102<1>UD       g100<1,1,0>UD   g120<1,1,0>UD   { align1 1H A@3 compacted };
mov(8)          g17<2>UD        g100<4,4,1>UD                   { align1 1Q $5.src };
mov(8)          g19<2>UD        g101<4,4,1>UD                   { align1 2Q $5.src };
add(16)         g107<1>D        -g102<1,1,0>D   g122<1,1,0>D    { align1 1H A@3 compacted };
mov(8)          g17.1<2>UD      g107<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g19.1<2>UD      g108<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g17UD           g83UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $6 };
add(16)         g108<1>D        g120<1,1,0>D    40D             { align1 1H F@7 compacted };
mov(16)         g84<1>UD        g116<16,8,2>UB                  { align1 1H $6.src };
cmp.l.f0.0(16)  g110<1>UD       g108<1,1,0>UD   g120<1,1,0>UD   { align1 1H A@2 compacted };
mov(8)          g17<2>UD        g108<4,4,1>UD                   { align1 1Q $6.src };
mov(8)          g19<2>UD        g109<4,4,1>UD                   { align1 2Q $6.src };
add(16)         g112<1>D        -g110<1,1,0>D   g122<1,1,0>D    { align1 1H A@3 compacted };
mov(8)          g17.1<2>UD      g112<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g19.1<2>UD      g113<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g17UD           g84UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $7 };
add(16)         g113<1>D        g120<1,1,0>D    52D             { align1 1H F@4 compacted };
mov(16)         g85<1>UD        g115<16,8,2>UB                  { align1 1H $7.src };
cmp.l.f0.0(16)  g115<1>UD       g113<1,1,0>UD   g120<1,1,0>UD   { align1 1H I@2 compacted };
mov(8)          g17<2>UD        g113<4,4,1>UD                   { align1 1Q $7.src };
mov(8)          g19<2>UD        g114<4,4,1>UD                   { align1 2Q $7.src };
add(16)         g117<1>D        -g115<1,1,0>D   g122<1,1,0>D    { align1 1H A@1 compacted };
mov(8)          g17.1<2>UD      g117<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g19.1<2>UD      g118<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g17UD           g85UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $8 };
add(16)         g118<1>D        g120<1,1,0>D    34D             { align1 1H F@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
mov(16)         g86<1>UD        g124<16,8,2>UB                  { align1 1H F@3 };
cmp.l.f0.0(16)  g124<1>UD       g118<1,1,0>UD   g120<1,1,0>UD   { align1 1H I@2 compacted };
mov(8)          g17<2>UD        g118<4,4,1>UD                   { align1 1Q $8.src };
mov(8)          g19<2>UD        g119<4,4,1>UD                   { align1 2Q $8.src };
add(16)         g126<1>D        -g124<1,1,0>D   g122<1,1,0>D    { align1 1H I@3 compacted };
mov(8)          g17.1<2>UD      g126<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g19.1<2>UD      g127<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g17UD           g86UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $9 };
add(16)         g21<1>D         g120<1,1,0>D    46D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
mov(16)         g87<1>UD        g57<16,8,2>UB                   { align1 1H F@3 };
cmp.l.f0.0(16)  g23<1>UD        g21<1,1,0>UD    g120<1,1,0>UD   { align1 1H I@2 compacted };
mov(8)          g17<2>UD        g21<4,4,1>UD                    { align1 1Q $9.src };
mov(8)          g19<2>UD        g22<4,4,1>UD                    { align1 2Q $9.src };
add(16)         g51<1>D         -g23<1,1,0>D    g122<1,1,0>D    { align1 1H I@3 compacted };
mov(8)          g17.1<2>UD      g51<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g19.1<2>UD      g52<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g17UD           g87UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $10 };
add(16)         g52<1>D         g120<1,1,0>D    58D             { align1 1H compacted };
mov(16)         g88<1>UD        g56<16,8,2>UB                   { align1 1H $10.src };
cmp.l.f0.0(16)  g54<1>UD        g52<1,1,0>UD    g120<1,1,0>UD   { align1 1H I@2 compacted };
mov(8)          g17<2>UD        g52<4,4,1>UD                    { align1 1Q $10.src };
mov(8)          g19<2>UD        g53<4,4,1>UD                    { align1 2Q $10.src };
add(16)         g56<1>D         -g54<1,1,0>D    g122<1,1,0>D    { align1 1H I@3 compacted };
mov(8)          g17.1<2>UD      g56<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g19.1<2>UD      g57<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g17UD           g88UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $11 };
cmp.nz.f0.0(16) null<1>D        g49<8,8,1>D     0D              { align1 1H };
mov(1)          g107<1>D        2D                              { align1 WE_all 1N };
add(16)         g59<1>D         g120<1,1,0>D    22D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
(+f0.0) sel(16) g57<1>UD        g107<0,1,0>UD   0x00000001UD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mov(8)          g17<2>UD        g59<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $11.src };
mov(8)          g19<2>UD        g60<4,4,1>UD                    { align1 2Q I@3 };
mov(16)         g125<4>UB       g57<8,8,1>UD                    { align1 1H I@3 };
cmp.l.f0.0(16)  g79<1>UD        g59<1,1,0>UD    g120<1,1,0>UD   { align1 1H $5.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mov(16)         g89<1>UD        g125<32,8,4>UB                  { align1 1H I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
add(16)         g83<1>D         -g79<1,1,0>D    g122<1,1,0>D    { align1 1H I@2 compacted };
mov(8)          g17.1<2>UD      g83<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g19.1<2>UD      g84<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g17UD           g89UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };

LABEL7:
endif(16)       JIP:  LABEL10                                   { align1 1H };

LABEL10:
else(16)        JIP:  LABEL5          UIP:  LABEL5              { align1 1H };

LABEL6:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
mov(16)         g84<1>UD        g31.3<32,8,4>UB                 { align1 1H A@1 };
cmp.l.f0.0(16)  null<1>D        g105<8,8,1>D    g69<8,8,1>D     { align1 1H };
(+f0.0) sel(16) g79<1>UD        g84<1,1,0>UD    0x00000000UD    { align1 1H A@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
mov(16)         g85<1>UD        g79<32,8,4>UB                   { align1 1H A@1 };
cmp.l.f0.0(16)  null<1>UD       g67<8,8,1>UD    g69<8,8,1>UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mov(16)         a0<1>UW         0x0aa0UW                        { align1 WE_all 1H A@2 };
shl(16)         a0<1>UW         g81<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0aa0UW        { align1 1H A@1 };
mov(16)         g87<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
or(16)          g89<1>UD        g79<1,1,0>UD    g87<1,1,0>UD    { align1 1H A@1 compacted };
mov(16)         g41<1>UD        g89<32,8,4>UB                   { align1 1H A@1 };
mov(16)         a0<1>UW         0x0520UW                        { align1 WE_all 1H A@1 };
shl(16)         a0<1>UW         g9<16,8,2>W     0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0520UW        { align1 1H A@1 };
mov(16)         g93<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
or(16)          g95<1>UD        g41<1,1,0>UD    g93<1,1,0>UD    { align1 1H I@1 compacted };
mov(16)         g97<1>UD        g95<32,8,4>UB                   { align1 1H I@1 };
mov(16)         a0<1>UW         0x0c20UW                        { align1 WE_all 1H I@1 };
shl(16)         a0<1>UW         g11<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0c20UW        { align1 1H A@1 };
mov(16)         g99<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
or(16)          g101<1>UD       g95<1,1,0>UD    g99<1,1,0>UD    { align1 1H A@1 compacted };
mov(16)         g107<4>UB       g101<0,1,0>UD                   { align1 1H A@1 };
(+f0.0) if(16)  JIP:  LABEL12         UIP:  LABEL11             { align1 1H };
mov(16)         g103<1>UD       g101<0,1,0>UB                   { align1 1H F@7 };
cmp.z.f0.0(16)  g109<1>D        g103<1,1,0>D    0D              { align1 1H A@1 compacted };
cmp.z.f0.0(16)  null<1>D        g79<8,8,1>D     0D              { align1 1H };
(+f0.0) sel(16) g111<1>UD       g109<8,8,1>UD   0xffffffffUD    { align1 1H A@2 };
cmp.nz.f0.0(16) null<1>D        g111<8,8,1>D    0D              { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.dst };
(+f0.0) sel(16) g55<1>UD        g33<8,8,1>UD    0xff800000UD    { align1 1H F@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.dst };
(+f0.0) sel(16) g57<1>UD        g35<8,8,1>UD    0xff800000UD    { align1 1H F@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.dst };
(+f0.0) sel(16) g59<1>UD        g37<8,8,1>UD    0xff800000UD    { align1 1H F@4 };
(+f0.0) sel(16) g49<1>UD        g25<8,8,1>UD    0x7f800000UD    { align1 1H $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
(+f0.0) sel(16) g51<1>UD        g27<8,8,1>UD    0x7f800000UD    { align1 1H F@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
(+f0.0) sel(16) g53<1>UD        g29<8,8,1>UD    0x7f800000UD    { align1 1H F@3 };
else(16)        JIP:  LABEL11         UIP:  LABEL11             { align1 1H };

LABEL12:
mov(16)         g53<1>UD        0x7f800000UD                    { align1 1H A@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(16)         g51<1>UD        0x7f800000UD                    { align1 1H A@4 };
mov(16)         g49<1>UD        0x7f800000UD                    { align1 1H I@6 };
mov(16)         g59<1>UD        0xff800000UD                    { align1 1H A@4 };
mov(16)         g57<1>UD        0xff800000UD                    { align1 1H A@6 };
mov(16)         g55<1>UD        0xff800000UD                    { align1 1H F@1 };

LABEL11:
endif(16)       JIP:  LABEL5                                    { align1 1H };
mov(16)         a0<1>UW         0x0620UW                        { align1 WE_all 1H A@2 };
shl(16)         a0<1>UW         g81<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0620UW        { align1 1H A@1 };
mov(16)         g89<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0660UW                        { align1 WE_all 1H A@5 };
shl(16)         a0<1>UW         g81<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0660UW        { align1 1H A@1 };
mov(16)         g21<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x06a0UW                        { align1 WE_all 1H A@1 };
shl(16)         a0<1>UW         g81<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x06a0UW        { align1 1H A@1 };
mov(16)         g85<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x06e0UW                        { align1 WE_all 1H A@2 };
shl(16)         a0<1>UW         g81<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x06e0UW        { align1 1H A@1 };
mov(16)         g87<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0720UW                        { align1 WE_all 1H A@1 };
shl(16)         a0<1>UW         g81<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0720UW        { align1 1H A@1 };
mov(16)         g83<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0760UW                        { align1 WE_all 1H A@3 };
shl(16)         a0<1>UW         g81<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0760UW        { align1 1H A@1 };
mov(16)         g23<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
cmp.l.f0.0(16)  null<1>UD       g67<8,8,1>UD    0x00000006UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sel.l(16)       g17<1>F         g49<1,1,0>F     g89<1,1,0>F     { align1 1H I@7 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sel.l(16)       g19<1>F         g51<1,1,0>F     g21<1,1,0>F     { align1 1H I@6 compacted };
sel.l(16)       g21<1>F         g53<1,1,0>F     g85<1,1,0>F     { align1 1H I@5 compacted };
sel.ge(16)      g89<1>F         g59<1,1,0>F     g23<1,1,0>F     { align1 1H I@2 compacted };
sel.ge(16)      g85<1>F         g55<1,1,0>F     g87<1,1,0>F     { align1 1H I@4 compacted };
mov(16)         a0<1>UW         0x0220UW                        { align1 WE_all 1H F@2 };
shl(16)         a0<1>UW         g9<16,8,2>W     0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0220UW        { align1 1H A@1 };
mov(16)         g23<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sel.ge(16)      g87<1>F         g57<1,1,0>F     g83<1,1,0>F     { align1 1H I@4 compacted };
mov(16)         a0<1>UW         0x0260UW                        { align1 WE_all 1H F@5 };
shl(16)         a0<1>UW         g9<16,8,2>W     0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0260UW        { align1 1H A@1 };
mov(16)         g81<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x02a0UW                        { align1 WE_all 1H F@1 };
shl(16)         a0<1>UW         g9<16,8,2>W     0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x02a0UW        { align1 1H A@1 };
mov(16)         g83<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0b20UW                        { align1 WE_all 1H F@3 };
shl(16)         a0<1>UW         g9<16,8,2>W     0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0b20UW        { align1 1H A@1 };
mov(16)         g95<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0aa0UW                        { align1 WE_all 1H F@2 };
shl(16)         a0<1>UW         g9<16,8,2>W     0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0aa0UW        { align1 1H A@1 };
mov(16)         g41<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0ae0UW                        { align1 WE_all 1H F@1 };
shl(16)         a0<1>UW         g9<16,8,2>W     0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0ae0UW        { align1 1H A@1 };
mov(16)         g93<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sel.l(16)       g9<1>F          g17<1,1,0>F     g23<1,1,0>F     { align1 1H I@1 compacted };
sel.l(16)       g17<1>F         g19<1,1,0>F     g81<1,1,0>F     { align1 1H A@1 compacted };
sel.l(16)       g19<1>F         g21<1,1,0>F     g83<1,1,0>F     { align1 1H A@1 compacted };
sel.ge(16)      g21<1>F         g85<1,1,0>F     g41<1,1,0>F     { align1 1H A@1 compacted };
sel.ge(16)      g23<1>F         g87<1,1,0>F     g93<1,1,0>F     { align1 1H I@1 compacted };
sel.ge(16)      g41<1>F         g89<1,1,0>F     g95<1,1,0>F     { align1 1H I@3 compacted };
mov(16)         a0<1>UW         0x0120UW                        { align1 WE_all 1H F@4 };
shl(16)         a0<1>UW         g11<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0120UW        { align1 1H A@1 };
mov(16)         g83<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0220UW                        { align1 WE_all 1H A@3 };
shl(16)         a0<1>UW         g11<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0220UW        { align1 1H A@1 };
mov(16)         g85<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0260UW                        { align1 WE_all 1H A@2 };
shl(16)         a0<1>UW         g11<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0260UW        { align1 1H A@1 };
mov(16)         g87<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x02a0UW                        { align1 WE_all 1H A@1 };
shl(16)         a0<1>UW         g11<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x02a0UW        { align1 1H A@1 };
mov(16)         g89<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x02e0UW                        { align1 WE_all 1H F@2 };
shl(16)         a0<1>UW         g11<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x02e0UW        { align1 1H A@1 };
mov(16)         g93<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
mov(16)         a0<1>UW         0x0520UW                        { align1 WE_all 1H F@1 };
shl(16)         a0<1>UW         g11<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0520UW        { align1 1H A@1 };
mov(16)         g95<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sel.l(16)       g81<1>F         g9<1,1,0>F      g83<1,1,0>F     { align1 1H I@6 compacted };
sel.l(16)       g83<1>F         g17<1,1,0>F     g85<1,1,0>F     { align1 1H I@5 compacted };
sel.l(16)       g85<1>F         g19<1,1,0>F     g87<1,1,0>F     { align1 1H I@4 compacted };
sel.ge(16)      g87<1>F         g21<1,1,0>F     g89<1,1,0>F     { align1 1H I@3 compacted };
sel.ge(16)      g89<1>F         g23<1,1,0>F     g93<1,1,0>F     { align1 1H I@2 compacted };
sel.ge(16)      g93<1>F         g41<1,1,0>F     g95<1,1,0>F     { align1 1H I@1 compacted };
(+f0.0) if(16)  JIP:  LABEL13         UIP:  LABEL13             { align1 1H };
sel.ge(16)      g19<1>F         (abs)g81<0,1,0>F (abs)g87<0,1,0>F { align1 1H A@3 compacted };
sel.ge(16)      g17<1>F         (abs)g83<0,1,0>F (abs)g89<0,1,0>F { align1 1H A@3 compacted };
sel.ge(16)      g23<1>F         (abs)g85<0,1,0>F (abs)g93<0,1,0>F { align1 1H A@3 compacted };
add(16)         g101<1>D        g75<1,1,0>D     16D             { align1 1H compacted };
sel.ge(16)      g21<1>F         g17<1,1,0>F     g23<1,1,0>F     { align1 1H A@1 compacted };
mov(16)         g126<2>B        1W                              { align1 1H };
mov(8)          g9<2>UD         g101<4,4,1>UD                   { align1 1Q A@2 };
mov(8)          g11<2>UD        g102<4,4,1>UD                   { align1 2Q I@3 };
sel.ge(16)      g17<1>F         g19<1,1,0>F     g21<1,1,0>F     { align1 1H F@1 compacted };
mov(16)         g103<1>UD       g126<16,8,2>UB                  { align1 1H I@3 };
mul(16)         g41<1>F         g17<1,1,0>F     0x34000000F  /* 1.19209e-07F */ { align1 1H A@1 compacted };
add(16)         g17<1>F         g81<0,1,0>F     -g41<1,1,0>F    { align1 1H F@1 compacted };
add(16)         g19<1>F         g83<0,1,0>F     -g41<1,1,0>F    { align1 1H compacted };
add(16)         g21<1>F         g85<0,1,0>F     -g41<1,1,0>F    { align1 1H compacted };
add(16)         g81<1>F         g87<0,1,0>F     g41<1,1,0>F     { align1 1H compacted };
add(16)         g83<1>F         g89<0,1,0>F     g41<1,1,0>F     { align1 1H compacted };
add(16)         g85<1>F         g93<0,1,0>F     g41<1,1,0>F     { align1 1H compacted };
add(16)         g87<1>F         g81<1,1,0>F     -g17<1,1,0>F    { align1 1H F@3 compacted };
add(16)         g81<1>F         g83<1,1,0>F     -g19<1,1,0>F    { align1 1H F@3 compacted };
add(16)         g83<1>F         g85<1,1,0>F     -g21<1,1,0>F    { align1 1H F@3 compacted };
mul(16)         g85<1>F         g87<8,8,1>F     0x3f800001F  /* 1F */ { align1 1H F@3 };
mul(16)         g87<1>F         g81<8,8,1>F     0x3f800001F  /* 1F */ { align1 1H F@3 };
mul(16)         g81<1>F         g83<8,8,1>F     0x3f800001F  /* 1F */ { align1 1H F@3 };
and(16)         g83<1>UD        g85<8,8,1>UD    0x007fffffUD    { align1 1H F@1 };
and(16)         g89<1>UD        g85<8,8,1>UD    0x80000000UD    { align1 1H F@7 };
and(16)         g95<1>UD        g85<8,8,1>UD    0x7f800000UD    { align1 1H };
and(16)         g41<1>UD        g87<8,8,1>UD    0x80000000UD    { align1 1H F@2 };
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
or(16)          g81<1>UD        g89<1,1,0>UD    g41<1,1,0>UD    { align1 1H I@1 compacted };
(-f0.0) sel(16) g89<1>UD        g97<8,8,1>UD    0x3f000000UD    { align1 1H };
and(16)         g41<1>UD        g89<8,8,1>UD    0x7fffffffUD    { align1 1H I@1 };
cmp.g.f0.0(16)  g89<1>F         g99<8,8,1>F     0x3f7f0000F  /* 0.996094F */ { align1 1H I@1 };
or(16)          g95<1>UD        g41<1,1,0>UD    g93<1,1,0>UD    { align1 1H A@1 compacted };
cmp.g.f0.0(16)  g41<1>F         g81<8,8,1>F     0x3f7f0000F  /* 0.996094F */ { align1 1H I@1 };
add3(16)        g81<1>D         65410W          g83<8,8,1>D     -g89<1,1,1>D { align1 1H F@1 };
cmp.g.f0.0(16)  g89<1>F         g95<8,8,1>F     0x3f7f0000F  /* 0.996094F */ { align1 1H I@1 };
add3(16)        g83<1>D         65410W          g85<8,8,1>D     -g41<1,1,1>D { align1 1H F@2 };
cmp.l.f0.0(16)  g41<1>UD        g101<1,1,0>UD   g75<1,1,0>UD    { align1 1H compacted };
add3(16)        g85<1>D         65410W          g87<8,8,1>D     -g89<1,1,1>D { align1 1H F@1 };
add(16)         g87<1>D         -g41<1,1,0>D    g77<1,1,0>D     { align1 1H I@2 compacted };
mov(8)          g9.1<2>UD       g87<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g11.1<2>UD      g88<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g9UD            g103UD          0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $12 };
mov(16)         g23<1>D         g7<8,8,1>D                      { align1 1H $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g13UD           g17UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $0 };
add(16)         g112<1>D        g75<1,1,0>D     18D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(16)         g5<4>UB         g81<8,8,1>UD                    { align1 1H I@7 };
cmp.l.f0.0(16)  g114<1>UD       g112<1,1,0>UD   g75<1,1,0>UD    { align1 1H I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g1<2>UD         g112<4,4,1>UD                   { align1 1Q };
mov(8)          g3<2>UD         g113<4,4,1>UD                   { align1 2Q $0.src };
mov(16)         g87<4>UB        g83<8,8,1>UD                    { align1 1H };
mov(16)         g89<4>UB        g85<8,8,1>UD                    { align1 1H };
mov(16)         g41<4>UB        g5<32,8,4>UB                    { align1 1H I@6 };
add(16)         g116<1>D        -g114<1,1,0>D   g77<1,1,0>D     { align1 1H I@6 compacted };
mov(16)         g41.1<4>UB      g87<32,8,4>UB                   { align1 1H I@2 };
mov(8)          g1.1<2>UD       g116<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g3.1<2>UD       g117<4,4,1>UD                   { align1 2Q I@3 };
mov(16)         g41.2<4>UB      g89<32,8,4>UB                   { align1 1H I@3 };
mov(16)         g41.3<4>UB      g107<32,8,4>UB                  { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g1UD            g41UD           0x08000584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $13 };
cmp.l.f0.0(16)  null<1>UD       g67<8,8,1>UD    g69<8,8,1>UD    { align1 1H };
(+f0.0) if(16)  JIP:  LABEL15         UIP:  LABEL14             { align1 1H };
sel.ge(16)      g108<1>F        (abs)g49<1,1,0>F (abs)g55<1,1,0>F { align1 1H I@3 compacted };
sel.ge(16)      g110<1>F        (abs)g51<1,1,0>F (abs)g57<1,1,0>F { align1 1H compacted };
sel.ge(16)      g112<1>F        (abs)g53<1,1,0>F (abs)g59<1,1,0>F { align1 1H compacted };
shl(16)         g114<1>D        -g81<8,8,1>D    0x00000017UD    { align1 1H I@7 };
shl(16)         g116<1>D        -g83<8,8,1>D    0x00000017UD    { align1 1H I@6 };
shl(16)         g118<1>D        -g85<8,8,1>D    0x00000017UD    { align1 1H };
cmp.z.f0.0(16)  null<1>D        g79<8,8,1>D     0D              { align1 1H };
sel.ge(16)      g5<1>F          g110<1,1,0>F    g112<1,1,0>F    { align1 1H F@1 compacted };
add(16)         g7<1>D          g114<8,8,1>D    1132462080D     { align1 1H I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
add(16)         g9<1>D          g116<8,8,1>D    1132462080D     { align1 1H I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
add(16)         g11<1>D         g118<8,8,1>D    1132462080D     { align1 1H I@4 };
sel.ge(16)      g117<1>F        g108<1,1,0>F    g5<1,1,0>F      { align1 1H A@1 compacted };
mul(16)         g5<1>F          g117<1,1,0>F    0x34000000F  /* 1.19209e-07F */ { align1 1H F@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(16)         g13<1>F         g49<1,1,0>F     -g5<1,1,0>F     { align1 1H F@1 compacted };
add(16)         g15<1>F         g51<1,1,0>F     -g5<1,1,0>F     { align1 1H $0.src compacted };
add(16)         g49<1>F         g53<1,1,0>F     -g5<1,1,0>F     { align1 1H F@2 compacted };
add(16)         g51<1>F         g55<1,1,0>F     g5<1,1,0>F      { align1 1H F@2 compacted };
add(16)         g53<1>F         g57<1,1,0>F     g5<1,1,0>F      { align1 1H F@2 compacted };
add(16)         g55<1>F         g59<1,1,0>F     g5<1,1,0>F      { align1 1H F@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(16)         g57<1>F         g13<1,1,0>F     -g17<1,1,0>F    { align1 1H F@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(16)         g59<1>F         g15<1,1,0>F     -g19<1,1,0>F    { align1 1H F@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(16)         g79<1>F         g49<1,1,0>F     -g21<1,1,0>F    { align1 1H A@4 compacted };
add(16)         g112<1>F        g51<1,1,0>F     -g17<1,1,0>F    { align1 1H F@6 compacted };
add(16)         g114<1>F        g53<1,1,0>F     -g19<1,1,0>F    { align1 1H A@3 compacted };
add(16)         g116<1>F        g55<1,1,0>F     -g21<1,1,0>F    { align1 1H A@2 compacted };
mul(16)         g81<1>F         g57<8,8,1>F     0x3f7ffffeF  /* 1F */ { align1 1H A@6 };
mul(16)         g83<1>F         g59<8,8,1>F     0x3f7ffffeF  /* 1F */ { align1 1H A@6 };
mul(16)         g85<1>F         g79<8,8,1>F     0x3f7ffffeF  /* 1F */ { align1 1H A@5 };
mul(16)         g118<1>F        g112<8,8,1>F    0x3f800001F  /* 1F */ { align1 1H A@1 };
mul(16)         g5<1>F          g114<8,8,1>F    0x3f800001F  /* 1F */ { align1 1H F@6 };
mul(16)         g13<1>F         g116<8,8,1>F    0x3f800001F  /* 1F */ { align1 1H F@6 };
mul(16)         g87<1>F         g81<1,1,0>F     g7<1,1,0>F      { align1 1H A@3 compacted };
mul(16)         g89<1>F         g83<1,1,0>F     g9<1,1,0>F      { align1 1H A@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
mul(16)         g41<1>F         g85<1,1,0>F     g11<1,1,0>F     { align1 1H A@1 compacted };
mul(16)         g15<1>F         g118<1,1,0>F    g7<1,1,0>F      { align1 1H F@6 compacted };
mul(16)         g17<1>F         g5<1,1,0>F      g9<1,1,0>F      { align1 1H F@6 compacted };
mul(16)         g19<1>F         g13<1,1,0>F     g11<1,1,0>F     { align1 1H F@6 compacted };
rndd(16)        g93<1>F         g87<1,1,0>F                     { align1 1H F@6 compacted };
rndd(16)        g95<1>F         g89<1,1,0>F                     { align1 1H F@6 compacted };
rndd(16)        g97<1>F         g41<1,1,0>F                     { align1 1H F@6 compacted };
rndd(16)        g21<1>F         -g15<1,1,0>F                    { align1 1H F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
rndd(16)        g23<1>F         -g17<1,1,0>F                    { align1 1H F@6 compacted };
rndd(16)        g49<1>F         -g19<1,1,0>F                    { align1 1H F@6 compacted };
sel.ge(16)      g99<1>F         g93<1,1,0>F     0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.ge(16)      g101<1>F        g95<1,1,0>F     0x0F  /* 0F */  { align1 1H F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
sel.ge(16)      g103<1>F        g97<1,1,0>F     0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.ge(16)      g51<1>F         -g21<1,1,0>F    0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.ge(16)      g53<1>F         -g23<1,1,0>F    0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.ge(16)      g55<1>F         -g49<1,1,0>F    0x0F  /* 0F */  { align1 1H F@6 compacted };
sel.l(16)       g107<1>F        g99<8,8,1>F     0x437f0000F  /* 255F */ { align1 1H A@6 };
sel.l(16)       g109<1>F        g101<8,8,1>F    0x437f0000F  /* 255F */ { align1 1H F@6 };
sel.l(16)       g111<1>F        g103<8,8,1>F    0x437f0000F  /* 255F */ { align1 1H F@6 };
sel.l(16)       g57<1>F         g51<8,8,1>F     0x437f0000F  /* 255F */ { align1 1H F@6 };
sel.l(16)       g59<1>F         g53<8,8,1>F     0x437f0000F  /* 255F */ { align1 1H F@6 };
sel.l(16)       g79<1>F         g55<8,8,1>F     0x437f0000F  /* 255F */ { align1 1H F@6 };
mov(16)         g120<4>UB       g107<8,8,1>F                    { align1 1H F@6 };
mov(16)         g122<4>UB       g109<8,8,1>F                    { align1 1H F@5 };
mov(16)         g124<4>UB       g111<8,8,1>F                    { align1 1H F@4 };
mov(16)         g126<4>UB       g57<8,8,1>F                     { align1 1H F@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
mov(16)         g1<4>UB         g59<8,8,1>F                     { align1 1H F@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
mov(16)         g3<4>UB         g79<8,8,1>F                     { align1 1H F@1 };
mov(16)         g83<1>UW        g120<32,8,4>UB                  { align1 1H I@6 };
mov(16)         g84<1>UW        g122<32,8,4>UB                  { align1 1H I@6 };
mov(16)         g85<1>UW        g124<32,8,4>UB                  { align1 1H I@6 };
mov(16)         g80<1>UW        g126<32,8,4>UB                  { align1 1H I@6 };
mov(16)         g81<1>UW        g1<32,8,4>UB                    { align1 1H I@6 };
mov(16)         g82<1>UW        g3<32,8,4>UB                    { align1 1H I@6 };
(-f0.0) sel(16) g103<1>UW       g83<16,16,1>UW  0x0000UW        { align1 1H A@4 };
(-f0.0) sel(16) g102<1>UW       g84<16,16,1>UW  0x0000UW        { align1 1H A@5 };
(-f0.0) sel(16) g101<1>UW       g85<16,16,1>UW  0x0000UW        { align1 1H A@5 };
(-f0.0) sel(16) g108<1>UW       g80<16,16,1>UW  0x0000UW        { align1 1H I@6 };
(-f0.0) sel(16) g107<1>UW       g81<16,16,1>UW  0x0000UW        { align1 1H I@6 };
(-f0.0) sel(16) g104<1>UW       g82<16,16,1>UW  0x0000UW        { align1 1H A@4 };
else(16)        JIP:  LABEL14         UIP:  LABEL14             { align1 1H };

LABEL15:
mov(16)         g101<1>UW       0x0080UW                        { align1 1H I@5 };
mov(16)         g102<1>UW       0x0080UW                        { align1 1H I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mov(16)         g103<1>UW       0x0080UW                        { align1 1H I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mov(16)         g104<1>UW       0x0000UW                        { align1 1H I@5 };
mov(16)         g107<1>UW       0x0000UW                        { align1 1H I@7 };
mov(16)         g108<1>UW       0x0000UW                        { align1 1H I@7 };

LABEL14:
endif(16)       JIP:  LABEL13                                   { align1 1H };
add(16)         g86<1>D         g75<1,1,0>D     g67<1,1,0>D     { align1 1H compacted };
mov(16)         g6<2>UB         g101<16,16,1>UW                 { align1 1H I@7 };
mov(16)         g7<2>UB         g102<16,16,1>UW                 { align1 1H I@7 };
mov(16)         g8<2>UB         g103<16,16,1>UW                 { align1 1H I@7 };
mov(16)         g90<2>UB        g104<16,16,1>UW                 { align1 1H I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mov(16)         g9<2>UB         g107<16,16,1>UW                 { align1 1H I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mov(16)         g10<2>UB        g108<16,16,1>UW                 { align1 1H I@7 };
cmp.l.f0.0(16)  g88<1>UD        g86<1,1,0>UD    g75<1,1,0>UD    { align1 1H I@7 compacted };
add(16)         g93<1>D         g86<1,1,0>D     28D             { align1 1H compacted };
mov(16)         g99<1>UD        g8<16,8,2>UB                    { align1 1H A@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
add(16)         g41<1>D         -g88<1,1,0>D    g77<1,1,0>D     { align1 1H I@3 compacted };
cmp.l.f0.0(16)  g95<1>UD        g93<1,1,0>UD    g86<1,1,0>UD    { align1 1H A@1 compacted };
mov(8)          g50<2>UD        g93<4,4,1>UD                    { align1 1Q F@3 };
mov(8)          g52<2>UD        g94<4,4,1>UD                    { align1 2Q F@2 };
add(16)         g97<1>D         -g95<1,1,0>D    g41<1,1,0>D     { align1 1H A@3 compacted };
mov(8)          g50.1<2>UD      g97<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g52.1<2>UD      g98<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g50UD           g99UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $14 };
add(16)         g98<1>D         g86<1,1,0>D     40D             { align1 1H $14.src compacted };
mov(16)         g111<1>UD       g7<16,8,2>UB                    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
cmp.l.f0.0(16)  g100<1>UD       g98<1,1,0>UD    g86<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g107<2>UD       g98<4,4,1>UD                    { align1 1Q };
mov(8)          g109<2>UD       g99<4,4,1>UD                    { align1 2Q };
add(16)         g102<1>D        -g100<1,1,0>D   g41<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g107.1<2>UD     g102<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g109.1<2>UD     g103<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g107UD          g111UD          0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $15 };
add(16)         g103<1>D        g86<1,1,0>D     52D             { align1 1H compacted };
mov(16)         g116<1>UD       g6<16,8,2>UB                    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
cmp.l.f0.0(16)  g107<1>UD       g103<1,1,0>UD   g86<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g112<2>UD       g103<4,4,1>UD                   { align1 1Q $15.src };
mov(8)          g114<2>UD       g104<4,4,1>UD                   { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
add(16)         g109<1>D        -g107<1,1,0>D   g41<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g112.1<2>UD     g109<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g114.1<2>UD     g110<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g112UD          g116UD          0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };
add(16)         g110<1>D        g86<1,1,0>D     34D             { align1 1H $15.src compacted };
mov(16)         g121<1>UD       g10<16,8,2>UB                   { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
cmp.l.f0.0(16)  g112<1>UD       g110<1,1,0>UD   g86<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g117<2>UD       g110<4,4,1>UD                   { align1 1Q $0.src };
mov(8)          g119<2>UD       g111<4,4,1>UD                   { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(16)         g114<1>D        -g112<1,1,0>D   g41<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g117.1<2>UD     g114<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g119.1<2>UD     g115<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g117UD          g121UD          0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $1 };
add(16)         g115<1>D        g86<1,1,0>D     46D             { align1 1H $0.src compacted };
mov(16)         g126<1>UD       g9<16,8,2>UB                    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
cmp.l.f0.0(16)  g117<1>UD       g115<1,1,0>UD   g86<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g122<2>UD       g115<4,4,1>UD                   { align1 1Q $1.src };
mov(8)          g124<2>UD       g116<4,4,1>UD                   { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
add(16)         g119<1>D        -g117<1,1,0>D   g41<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g122.1<2>UD     g119<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g124.1<2>UD     g120<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g122UD          g126UD          0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $2 };
add(16)         g120<1>D        g86<1,1,0>D     58D             { align1 1H $1.src compacted };
mov(16)         g5<1>UD         g90<16,8,2>UB                   { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
cmp.l.f0.0(16)  g122<1>UD       g120<1,1,0>UD   g86<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g1<2>UD         g120<4,4,1>UD                   { align1 1Q $13.src };
mov(8)          g3<2>UD         g121<4,4,1>UD                   { align1 2Q $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g124<1>D        -g122<1,1,0>D   g41<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g1.1<2>UD       g124<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g3.1<2>UD       g125<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g1UD            g5UD            0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };
add(16)         g125<1>D        g86<1,1,0>D     22D             { align1 1H $2.src compacted };
mov(16)         g12<2>B         2W                              { align1 1H $12.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
cmp.l.f0.0(16)  g1<1>UD         g125<1,1,0>UD   g86<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g6<2>UD         g125<4,4,1>UD                   { align1 1Q $0.src };
mov(8)          g8<2>UD         g126<4,4,1>UD                   { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mov(16)         g10<1>UD        g12<16,8,2>UB                   { align1 1H I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(16)         g3<1>D          -g1<1,1,0>D     g41<1,1,0>D     { align1 1H I@4 compacted };
mov(8)          g6.1<2>UD       g3<4,4,1>UD                     { align1 1Q I@1 };
mov(8)          g8.1<2>UD       g4<4,4,1>UD                     { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g6UD            g10UD           0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };

LABEL13:
endif(16)       JIP:  LABEL5                                    { align1 1H };

LABEL5:
endif(16)       JIP:  LABEL16                                   { align1 1H };

LABEL16:
cmp.l.f0.0(16)  null<1>D        g105<8,8,1>D    g69<8,8,1>D     { align1 1H };
(+f0.0) if(16)  JIP:  LABEL17         UIP:  LABEL17             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
asr(16)         g4<1>D          g69<8,8,1>D     0x0000001fUD    { align1 1H A@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
shl(16)         g6<1>D          g105<8,8,1>D    0x00000005UD    { align1 1H F@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
shr(16)         g8<1>UD         g105<1,1,0>UD   0x0000001bUD    { align1 1H F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(8)          g10<1>UD        g43.1<8,4,2>UD                  { align1 1Q F@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
mov(8)          g11<1>UD        g63.1<8,4,2>UD                  { align1 2Q I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(8)          g12<1>D         g43<8,4,2>D     g6<1,1,0>D      { align1 1Q I@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
add(8)          g13<1>D         g63<8,4,2>D     g7<1,1,0>D      { align1 2Q I@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
cmp.l.f0.0(8)   g14<1>UD        g12<8,8,1>UD    g43<8,4,2>UD    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(8)          g18<2>UD        g12<4,4,1>UD                    { align1 1Q F@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
cmp.l.f0.0(8)   g15<1>UD        g13<8,8,1>UD    g63<8,4,2>UD    { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
mov(8)          g20<2>UD        g13<4,4,1>UD                    { align1 2Q F@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add3(16)        g16<1>D         g10<8,8,1>D     g8<8,8,1>D      -g14<1,1,1>D { align1 1H A@2 };
mov(8)          g18.1<2>UD      g16<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g20.1<2>UD      g17<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g18UD           g25UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $3 };
add(16)         g18<1>D         g12<1,1,0>D     16D             { align1 1H $3.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
cmp.l.f0.0(16)  g20<1>UD        g18<1,1,0>UD    g12<1,1,0>UD    { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(8)          g24<2>UD        g18<4,4,1>UD                    { align1 1Q F@2 };
mov(8)          g26<2>UD        g19<4,4,1>UD                    { align1 2Q $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(16)         g22<1>D         -g20<1,1,0>D    g16<1,1,0>D     { align1 1H A@2 compacted };
mov(8)          g24.1<2>UD      g22<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g26.1<2>UD      g23<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g24UD           g33UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $4 };
shl(16)         g23<1>D         g105<8,8,1>D    0x00000002UD    { align1 1H $4.src };
shr(16)         g25<1>UD        g105<1,1,0>UD   0x0000001eUD    { align1 1H $4.src compacted };
mov(8)          g27<1>UD        g45.1<8,4,2>UD                  { align1 1Q $4.src };
mov(8)          g28<1>UD        g65.1<8,4,2>UD                  { align1 2Q $3.src };
add(8)          g98<1>D         g45<8,4,2>D     g23<1,1,0>D     { align1 1Q A@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $3.src };
add(8)          g29<1>D         g65<8,4,2>D     g24<1,1,0>D     { align1 2Q I@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
cmp.l.f0.0(8)   g30<1>UD        g98<8,8,1>UD    g45<8,4,2>UD    { align1 1Q I@2 };
mov(8)          g34<2>UD        g98<4,4,1>UD                    { align1 1Q $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $3.src };
cmp.l.f0.0(8)   g31<1>UD        g29<8,8,1>UD    g65<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g36<2>UD        g29<4,4,1>UD                    { align1 2Q $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add3(16)        g32<1>D         g27<8,8,1>D     g25<8,8,1>D     -g30<1,1,1>D { align1 1H I@2 };
mov(8)          g34.1<2>UD      g32<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g36.1<2>UD      g33<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g34UD           g105UD          0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $5 };
shr(16)         g59<1>UD        g47<0,1,0>UD    0x00000006UD    { align1 1H F@4 compacted };
add(16)         g33<1>D         g69<1,1,0>D     g105<1,1,0>D    { align1 1H $5.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
cmp.l.f0.0(16)  g35<1>UD        g33<1,1,0>UD    g69<1,1,0>UD    { align1 1H I@1 compacted };
shl(16)         g39<1>D         g33<8,8,1>D     0x00000002UD    { align1 1H $4.src };
shr(16)         g47<1>UD        g33<1,1,0>UD    0x0000001eUD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add(16)         g37<1>D         -g35<1,1,0>D    g4<1,1,0>D      { align1 1H I@3 compacted };
add(8)          g99<1>D         g45<8,4,2>D     g39<1,1,0>D     { align1 1Q A@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $14.src };
add(8)          g50<1>D         g65<8,4,2>D     g40<1,1,0>D     { align1 2Q A@4 compacted };
shl(16)         g43<1>D         g37<8,8,1>D     0x00000002UD    { align1 1H I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
cmp.l.f0.0(8)   g51<1>UD        g99<8,8,1>UD    g45<8,4,2>UD    { align1 1Q A@3 };
mov(8)          g55<2>UD        g99<4,4,1>UD                    { align1 1Q F@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $14.src };
cmp.l.f0.0(8)   g52<1>UD        g50<8,8,1>UD    g65<8,4,2>UD    { align1 2Q A@3 };
mov(8)          g57<2>UD        g50<4,4,1>UD                    { align1 2Q F@6 };
or(16)          g49<1>UD        g43<1,1,0>UD    g47<1,1,0>UD    { align1 1H A@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
add3(16)        g53<1>D         g27<8,8,1>D     g49<8,8,1>D     -g51<1,1,1>D { align1 1H A@1 };
mov(8)          g55.1<2>UD      g53<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g57.1<2>UD      g54<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g55UD           g59UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $6 };
and.nz.f0.0(16) null<1>UD       g71<8,8,1>UD    g73<8,8,1>UD    { align1 1H };
(+f0.0) if(16)  JIP:  LABEL18         UIP:  LABEL18             { align1 1H };
shl(16)         g54<1>D         g69<8,8,1>D     0x00000003UD    { align1 1H $6.src };
add(8)          g100<1>D        g91<8,4,2>D     40D             { align1 1Q F@3 compacted };
add(8)          g56<1>D         g61<8,4,2>D     40D             { align1 2Q $6.src compacted };
or(16)          g71<1>UD        g54<8,8,1>UD    0xffffffc0UD    { align1 1H I@3 };
cmp.l.f0.0(8)   g101<1>UD       g100<8,8,1>UD   g91<8,4,2>UD    { align1 1Q A@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $6.src };
cmp.l.f0.0(8)   g57<1>UD        g56<8,8,1>UD    g61<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g63<2>UD        g100<4,4,1>UD                   { align1 1Q };
mov(8)          g65<2>UD        g56<4,4,1>UD                    { align1 2Q };
add(8)          g102<1>D        -g101<8,8,1>D   g91.1<8,4,2>D   { align1 1Q A@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $6.src };
add(8)          g58<1>D         -g57<8,8,1>D    g61.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g63.1<2>UD      g102<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g65.1<2>UD      g58<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g59UD           g63UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $7 };
shl(16)         g63<1>D         g59<8,8,1>D     0x00000006UD    { align1 1H $7.dst };
add(8)          g103<1>D        g91<8,4,2>D     g63<1,1,0>D     { align1 1Q A@1 compacted };
add(8)          g64<1>D         g61<8,4,2>D     g64<1,1,0>D     { align1 2Q I@2 compacted };
cmp.l.f0.0(8)   g104<1>UD       g103<8,8,1>UD   g91<8,4,2>UD    { align1 1Q A@2 };
mov(8)          g67<2>UD        g103<4,4,1>UD                   { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $7.src };
cmp.l.f0.0(8)   g65<1>UD        g64<8,8,1>UD    g61<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g69<2>UD        g64<4,4,1>UD                    { align1 2Q };
add(8)          g105<1>D        -g104<8,8,1>D   g91.1<8,4,2>D   { align1 1Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $7.src };
add(8)          g66<1>D         -g65<8,8,1>D    g61.1<8,4,2>D   { align1 2Q I@3 };
mov(8)          g67.1<2>UD      g105<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g69.1<2>UD      g66<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g67UD           g71UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $8 };

LABEL18:
endif(16)       JIP:  LABEL17                                   { align1 1H };

LABEL17:
endif(16)       JIP:  LABEL19                                   { align1 1H };

LABEL19:
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_build_DFS_DFS_trivial_code[] = {
    0x80000065, 0x0d058220, 0x02000004, 0xffffffc0,
    0xe20e1940, 0x00010d03, 0xe20f0040, 0x04010d03,
    0x80030061, 0x29054410, 0x00000000, 0x76543210,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x80004031, 0x01140000, 0xfa000e0c, 0x00340000,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x80004031, 0x030c0000, 0xfa000f0c, 0x00300000,
    0x64291940, 0x00802995, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00040061, 0x43050160,
    0x00462905, 0x00000000, 0xe0751965, 0x00f04303,
    0x00041961, 0x37060210, 0x00467505, 0x00000000,
    0x00040061, 0x69050120, 0x00567506, 0x00000000,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x214d0061, 0x001102cc, 0x2a6d0061, 0x001102cc,
    0x00030061, 0x5b260aa0, 0x00000264, 0x00000000,
    0x00130061, 0x3d260aa0, 0x00000264, 0x00000000,
    0x00030061, 0x2b260aa0, 0x000002a4, 0x00000000,
    0x00130061, 0x3f260aa0, 0x000002a4, 0x00000000,
    0x00030061, 0x2d260aa0, 0x000002e4, 0x00000000,
    0x00130061, 0x41260aa0, 0x000002e4, 0x00000000,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0xe0730065, 0x00110303, 0x214d1761, 0x00110204,
    0x2a6d1761, 0x00110204, 0x215b1761, 0x00110244,
    0x2a3d1761, 0x00110244, 0x212b1761, 0x0011025c,
    0x2a3f1761, 0x0011025c, 0x212d1761, 0x0011026c,
    0x2a411761, 0x0011026c, 0xa16f1740, 0x024e4d03,
    0xaa701740, 0x024e6d03, 0x00031a61, 0x49060220,
    0x00346f05, 0x00000000, 0xe7711a70, 0x02406f03,
    0x00130061, 0x4b060220, 0x00347005, 0x00000000,
    0xae471e70, 0x00007303, 0x00031b40, 0x4e052660,
    0x06467105, 0x00444d26, 0x00131c40, 0x72052660,
    0x06467205, 0x00446d26, 0x00031a61, 0x49260220,
    0x00344e05, 0x00000000, 0x00131a61, 0x4b260220,
    0x00347205, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044131, 0x11240000,
    0xfb044924, 0x000c0000, 0x00042161, 0x35060210,
    0x00461305, 0x00000000, 0x00042161, 0x45050120,
    0x00561106, 0x00000000, 0x00041970, 0x00010660,
    0x56466905, 0x00464505, 0x01040022, 0x0001c060,
    0x00000220, 0x000001a0, 0x00040069, 0x76058660,
    0x02466905, 0x00000005, 0xe0780068, 0x01b06903,
    0x00031461, 0x7a050220, 0x00442b26, 0x00000000,
    0x00131361, 0x7b050220, 0x00443f26, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0xa17c1c40, 0x760e2b02, 0xaa7d1d40, 0x770e3f02,
    0x00031a70, 0x7e050220, 0x52467c05, 0x00442b06,
    0x00030061, 0x0c060220, 0x00347c05, 0x00000000,
    0x00131b70, 0x7f050220, 0x52467d05, 0x00443f06,
    0x80103001, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x0e060220, 0x00347d05, 0x00000000,
    0xa0030040, 0x01007c03, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x00040952, 0x01040e68,
    0x0e2e7a05, 0x7e057805, 0x27051a70, 0x7c000303,
    0x00030061, 0x15060220, 0x00340305, 0x00000000,
    0x00130061, 0x17060220, 0x00340405, 0x00000000,
    0x00031c61, 0x0c260220, 0x00340105, 0x00000000,
    0x00131d61, 0x0e260220, 0x00340205, 0x00000000,
    0xa0071d40, 0x01020502, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x00044031, 0x19440000,
    0xfb040c24, 0x003c0000, 0x00031961, 0x15260220,
    0x00340705, 0x00000000, 0x00131a61, 0x17260220,
    0x00340805, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x21440000,
    0xfb041524, 0x003c0000, 0x00040024, 0x0001c060,
    0x00000090, 0x00000090, 0x00042061, 0x1f054220,
    0x00000000, 0x00000000, 0x00042061, 0x1d054220,
    0x00000000, 0x7f800000, 0x00042061, 0x1b054220,
    0x00000000, 0x7f800000, 0x00042061, 0x19054220,
    0x00000000, 0x7f800000, 0x00042261, 0x27054220,
    0x00000000, 0x80000000, 0x00042261, 0x25054220,
    0x00000000, 0xff800000, 0x00042261, 0x23054220,
    0x00000000, 0xff800000, 0x00042261, 0x21054220,
    0x00000000, 0xff800000, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x00040070, 0x08058550,
    0x15563706, 0x00000000, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x00041961, 0x49050560,
    0x20460805, 0x00000000, 0x01040022, 0x0001c060,
    0x000000e0, 0x000000e0, 0xa14f1640, 0x00ce5b03,
    0xaa091540, 0x00ce3d03, 0x00031a70, 0x50050220,
    0x52464f05, 0x00445b06, 0x00131a70, 0x0a050220,
    0x52460905, 0x00443d06, 0x00033161, 0x4b060220,
    0x00344f05, 0x00000000, 0x00130061, 0x4d060220,
    0x00340905, 0x00000000, 0x00031c40, 0x51052660,
    0x06465005, 0x00445b26, 0x00131c40, 0x0b052660,
    0x06460a05, 0x00443d26, 0x00031a61, 0x4b260220,
    0x00345105, 0x00000000, 0x00131a61, 0x4d260220,
    0x00340b05, 0x00000000, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044131, 0x0c140000,
    0xfb104b24, 0x01000000, 0x00042169, 0x2f058660,
    0x02460c05, 0x00000006, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x00030e61, 0x15050220,
    0x00445b06, 0x00000000, 0x80103201, 0x00000000,
    0x00000000, 0x00000000, 0x00130d61, 0x16050220,
    0x00443d06, 0x00000000, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0xa14b1c40, 0x2f1e5b02,
    0xaa4c3140, 0x2f1e3d02, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0xa15f0040, 0x010e5b03,
    0xaa183240, 0x010e3d03, 0xa0530040, 0x00404303,
    0xa0550040, 0xff404303, 0xa0570040, 0x00204303,
    0xa0590040, 0xff204303, 0xa0290040, 0x00104303,
    0xa05d0040, 0xff104303, 0x00031f70, 0x52050220,
    0x52464b05, 0x00445b06, 0x80103201, 0x00000000,
    0x00000000, 0x00000000, 0x00131f70, 0x17050220,
    0x52464c05, 0x00443d06, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x00030061, 0x0d060220,
    0x00344b05, 0x00000000, 0x80103001, 0x00000000,
    0x00000000, 0x00000000, 0x00130061, 0x0f060220,
    0x00344c05, 0x00000000, 0x00030061, 0x31060220,
    0x00345f05, 0x00000000, 0x00130061, 0x33060220,
    0x00341805, 0x00000000, 0x00030070, 0x60050220,
    0x52465f05, 0x00445b06, 0x00130070, 0x37050220,
    0x52461805, 0x00443d06, 0x00040070, 0x00018660,
    0x56465305, 0x00000010, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x00031f40, 0x4d052660,
    0x06465205, 0x00445b26, 0x80103101, 0x00000000,
    0x00000000, 0x00000000, 0x00131f40, 0x4e052660,
    0x06461705, 0x00443d26, 0x00031d40, 0x61052660,
    0x06466005, 0x00445b26, 0x00131d40, 0x38052660,
    0x06463705, 0x00443d26, 0x2f510062, 0x55005303,
    0x00031d61, 0x0d260220, 0x00344d05, 0x00000000,
    0x00131d61, 0x0f260220, 0x00344e05, 0x00000000,
    0x00031d61, 0x31260220, 0x00346105, 0x00000000,
    0x00131d61, 0x33260220, 0x00343805, 0x00000000,
    0x00040070, 0x00018660, 0x56465705, 0x00000010,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x39140000, 0xfb043124, 0x00040000,
    0x2f090062, 0x59005703, 0x00040070, 0x00018660,
    0x56462905, 0x00000010, 0x2f0b3062, 0x5d002903,
    0x00040070, 0x5e058550, 0x15563506, 0x00010001,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x00041961, 0x31050560, 0x20465e05, 0x00000000,
    0x00042369, 0x3b058660, 0x02463905, 0x00000006,
    0x00041952, 0x4f040e68, 0x0e2e1505, 0x4b053b05,
    0x0004196c, 0x07058660, 0x02464f05, 0x00000006,
    0x11040022, 0x0001c060, 0x000028f8, 0x00001270,
    0x00040070, 0x00010220, 0x52464305, 0x00464505,
    0x01042262, 0x53058220, 0x02462105, 0xff800000,
    0x01042262, 0x55058220, 0x02462305, 0xff800000,
    0x01042262, 0x57058220, 0x02462505, 0xff800000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x01042062, 0x33058220, 0x02461905, 0x7f800000,
    0x01042062, 0x37058220, 0x02461b05, 0x7f800000,
    0x01042062, 0x39058220, 0x02461d05, 0x7f800000,
    0x80041e61, 0x10014110, 0x00000000, 0x0a600a60,
    0x00040069, 0x10018510, 0x01565106, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0a600a60,
    0xe0290961, 0x001b0004, 0x80041e61, 0x10014110,
    0x00000000, 0x0aa00aa0, 0x00040069, 0x10018510,
    0x01565106, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0aa00aa0, 0xe0590961, 0x001b0004,
    0x80041e61, 0x10014110, 0x00000000, 0x0ae00ae0,
    0x00040069, 0x10018510, 0x01565106, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0ae00ae0,
    0xe0170961, 0x001b0004, 0x80041e61, 0x10014110,
    0x00000000, 0x06600660, 0x00040069, 0x10018510,
    0x01565106, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x06600660, 0xe0350961, 0x001b0004,
    0x80041e61, 0x10014110, 0x00000000, 0x06e006e0,
    0x00040069, 0x10018510, 0x01565106, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x06e006e0,
    0xe03b0961, 0x001b0004, 0x80041e61, 0x10014110,
    0x00000000, 0x07200720, 0x00040069, 0x10018510,
    0x01565106, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x07200720, 0xe04f0961, 0x001b0004,
    0x00040070, 0x00018220, 0x52464305, 0x00000006,
    0x25151f62, 0x29005300, 0x25530962, 0x59005500,
    0x25591d62, 0x17005700, 0x27171c62, 0x35003300,
    0x27351b62, 0x3b003700, 0x27370962, 0x4f003900,
    0x80040c61, 0x10014110, 0x00000000, 0x0a600a60,
    0x00040069, 0x10018510, 0x01560906, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0a600a60,
    0xe0570961, 0x001b0004, 0x80041161, 0x10014110,
    0x00000000, 0x02a002a0, 0x00040069, 0x10018510,
    0x01560906, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x02a002a0, 0xe04f0961, 0x001b0004,
    0x80041461, 0x10014110, 0x00000000, 0x0b200b20,
    0x00040069, 0x10018510, 0x01560906, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0b200b20,
    0xe0290961, 0x001b0004, 0x80040961, 0x10014110,
    0x00000000, 0x02e002e0, 0x00040069, 0x10018510,
    0x01560906, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x02e002e0, 0xe0390961, 0x001b0004,
    0x80041261, 0x10014110, 0x00000000, 0x06a006a0,
    0x00040069, 0x10018510, 0x01560906, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x06a006a0,
    0xe03b0961, 0x001b0004, 0x80041161, 0x10014110,
    0x00000000, 0x06e006e0, 0x00040069, 0x10018510,
    0x01560906, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x06e006e0, 0xe0550961, 0x001b0004,
    0x25330b62, 0x4f001500, 0x254f1e62, 0x57005300,
    0x27150a62, 0x39001700, 0x25571c62, 0x29005900,
    0x27170a62, 0x3b003500, 0x27530962, 0x55003700,
    0x80041261, 0x10014110, 0x00000000, 0x06600660,
    0x00040069, 0x10018510, 0x01560b06, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x06600660,
    0xe03b0961, 0x001b0004, 0x80041361, 0x10014110,
    0x00000000, 0x09e009e0, 0x00040069, 0x10018510,
    0x01560b06, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x09e009e0, 0xe0290961, 0x001b0004,
    0x80040a61, 0x10014110, 0x00000000, 0x02a002a0,
    0x00040069, 0x10018510, 0x01560b06, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x02a002a0,
    0xe0350961, 0x001b0004, 0x80040b61, 0x10014110,
    0x00000000, 0x0ae00ae0, 0x00040069, 0x10018510,
    0x01560b06, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0ae00ae0, 0xe0590961, 0x001b0004,
    0x80040961, 0x10014110, 0x00000000, 0x02e002e0,
    0x00040069, 0x10018510, 0x01560b06, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x02e002e0,
    0xe0370961, 0x001b0004, 0x80041161, 0x10014110,
    0x00000000, 0x0a600a60, 0x00040069, 0x10018510,
    0x01560b06, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0a600a60, 0xe0550961, 0x001b0004,
    0x25391e62, 0x3b003300, 0x27330962, 0x35001500,
    0x253b1d62, 0x29004f00, 0x27351a62, 0x37001700,
    0x254f0a62, 0x59005700, 0x27371962, 0x55005300,
    0x01040022, 0x0001c060, 0x00000d30, 0x00000d30,
    0x25530962, 0x39aa3300, 0x25551262, 0x3baa3500,
    0x25570b62, 0x4faa3700, 0xa0291740, 0x01004b03,
    0x00040061, 0x5f064540, 0x00000000, 0xffffffff,
    0x25591162, 0x57005500, 0x00040061, 0x5d070200,
    0x00461305, 0x00000000, 0x00030b61, 0x15060220,
    0x00342905, 0x00000000, 0x00130c61, 0x17060220,
    0x00342a05, 0x00000000, 0x25130962, 0x59005300,
    0x00041b61, 0x11050020, 0x00665d07, 0x00000000,
    0xe0601141, 0x34001300, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x80001101, 0x00000000,
    0x00000000, 0x00000000, 0x20010040, 0x60213300,
    0x20030040, 0x60213500, 0x20050040, 0x60213700,
    0x20620040, 0x60013900, 0x20640040, 0x60013b00,
    0x20660040, 0x60014f00, 0x206b1340, 0x01206200,
    0x206d1340, 0x03206400, 0x206f1340, 0x05206600,
    0x00041341, 0x71058aa0, 0x0a466b05, 0x3f800001,
    0x00041341, 0x37058aa0, 0x0a466d05, 0x3f800001,
    0x00041341, 0x33058aa0, 0x0a466f05, 0x3f800001,
    0x00041365, 0x13058220, 0x02467105, 0x007fffff,
    0x00041765, 0x4f058220, 0x02467105, 0x80000000,
    0x00041765, 0x39058220, 0x02467105, 0x7f800000,
    0x00041265, 0x35058220, 0x02463705, 0x007fffff,
    0x00040065, 0x53058220, 0x02463705, 0x80000000,
    0x00041765, 0x3b058220, 0x02463705, 0x7f800000,
    0x00041165, 0x55058220, 0x02463305, 0x80000000,
    0x00040065, 0x57058220, 0x02463305, 0x7f800000,
    0x00040065, 0x37058220, 0x02463305, 0x007fffff,
    0x00041f40, 0x33058660, 0x06461305, 0x3f000000,
    0x00041f6c, 0x13058660, 0x02463905, 0x00000017,
    0x00041f40, 0x39058660, 0x06463505, 0x3f000000,
    0x00041f6c, 0x35058660, 0x02463b05, 0x00000017,
    0x00041d40, 0x3b058660, 0x06463705, 0x3f000000,
    0xac001d70, 0x3f803301, 0x00041f6c, 0x37058660,
    0x02465705, 0x00000017, 0x11040062, 0x57058220,
    0x02463305, 0x3f000000, 0x00040965, 0x33058220,
    0x02465705, 0x7fffffff, 0xac001e70, 0x3f803901,
    0x20571966, 0x4f003303, 0x11040062, 0x4f058220,
    0x02463905, 0x3f000000, 0x00041965, 0x59058220,
    0x02464f05, 0x7fffffff, 0xac001f70, 0x3f803b01,
    0x205d1966, 0x53005903, 0x11040062, 0x72058220,
    0x02463b05, 0x3f000000, 0x00041965, 0x74058220,
    0x02467205, 0x7fffffff, 0x00041e70, 0x60058aa0,
    0x3a465705, 0x3f7f0000, 0x00041b70, 0x78058aa0,
    0x3a465d05, 0x3f7f0000, 0x20761966, 0x55007403,
    0x00041252, 0x33044560, 0x0e2eff82, 0x60051305,
    0x00040952, 0x39044560, 0x0e2eff82, 0x78053505,
    0x00041b70, 0x7a058aa0, 0x3a467605, 0x3f7f0000,
    0x277c0070, 0x4b002903, 0x00040952, 0x3b044560,
    0x0e2eff82, 0x7a053705, 0xa07e1a40, 0x4d027c02,
    0x00031961, 0x15260220, 0x00347e05, 0x00000000,
    0x00131a61, 0x17260220, 0x00347f05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x00000000, 0xf3081524, 0x00021114,
    0x00044031, 0x00000000, 0xfb0c0d24, 0x003c0144,
    0xa0153440, 0x01204b03, 0x00040f61, 0x6c070200,
    0x00463305, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x27171a70, 0x4b001503,
    0x00033461, 0x11060220, 0x00341505, 0x00000000,
    0x00130061, 0x13060220, 0x00341605, 0x00000000,
    0x00041761, 0x6e070200, 0x00463905, 0x00000000,
    0x00040f61, 0x70070200, 0x00463b05, 0x00000000,
    0x00041e61, 0x4f070000, 0x00666c07, 0x00000000,
    0xa0351e40, 0x4d021702, 0x00041a61, 0x4f0f0000,
    0x00666e07, 0x00000000, 0x00031a61, 0x11260220,
    0x00343505, 0x00000000, 0x00131b61, 0x13260220,
    0x00343605, 0x00000000, 0x00041b61, 0x4f170000,
    0x00667007, 0x00000000, 0x00041961, 0x4f1f0000,
    0x00565f06, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044531, 0x00000000,
    0xfb081124, 0x00004f14, 0x00040070, 0x00010220,
    0x52464305, 0x00464505, 0x01040022, 0x0001c060,
    0x000003c0, 0x00000360, 0x25133562, 0x21431900,
    0x25351d62, 0x23431b00, 0x25370062, 0x25431d00,
    0x00041f69, 0x1705a660, 0x02463305, 0x00000017,
    0x00043569, 0x1105a660, 0x02463b05, 0x00000017,
    0x00040069, 0x3305a660, 0x02463905, 0x00000017,
    0x25151162, 0x37003500, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x00041a40, 0x4f058660,
    0x06461105, 0x43800000, 0x00041c40, 0x39058660,
    0x06461705, 0x43800000, 0x00041b40, 0x3b058660,
    0x06463305, 0x43800000, 0x25361162, 0x15001300,
    0xe0110941, 0x34003600, 0x20131140, 0x11201900,
    0x20150040, 0x11201b00, 0x20171a40, 0x11201d00,
    0x20331940, 0x11002100, 0x20350040, 0x11002300,
    0x20370040, 0x11002500, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x20111640, 0x01201300,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x20131640, 0x03201500, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x20151640, 0x05201700,
    0x20171640, 0x01203300, 0x20331640, 0x03203500,
    0x20351640, 0x05203700, 0x00041641, 0x37058aa0,
    0x0a461105, 0x3f7ffffe, 0x00041641, 0x11058aa0,
    0x0a461305, 0x3f7ffffe, 0x00041641, 0x13058aa0,
    0x0a461505, 0x3f7ffffe, 0x00041641, 0x15058aa0,
    0x0a461705, 0x3f800001, 0x00041641, 0x17058aa0,
    0x0a463305, 0x3f800001, 0x00041641, 0x33058aa0,
    0x0a463505, 0x3f800001, 0x20350a41, 0x39003700,
    0x20370941, 0x3b001100, 0x20110b41, 0x4f001300,
    0x20131641, 0x39001500, 0x20151641, 0x3b001700,
    0x605f0d45, 0x00103500, 0x20171741, 0x4f003300,
    0x60611645, 0x00103700, 0x60631645, 0x00101100,
    0x60651645, 0x00121300, 0x60671645, 0x00121500,
    0xe56d1662, 0x00005f00, 0x606b1645, 0x00121700,
    0xe56f0e62, 0x00006100, 0xe5710e62, 0x00006300,
    0xe5731662, 0x00026500, 0xe5751662, 0x00026700,
    0x00041662, 0x37058aa0, 0x5a466d05, 0x437f0000,
    0xe5771662, 0x00026b00, 0x00041662, 0x39058aa0,
    0x5a466f05, 0x437f0000, 0x00041662, 0x3b058aa0,
    0x5a467105, 0x437f0000, 0x00041662, 0x4f058aa0,
    0x5a467305, 0x437f0000, 0x00041662, 0x5f058aa0,
    0x5a467505, 0x437f0000, 0x00041661, 0x53070a00,
    0x00463705, 0x00000000, 0x00041562, 0x61058aa0,
    0x5a467705, 0x437f0000, 0x00041561, 0x55070a00,
    0x00463905, 0x00000000, 0x00041461, 0x57070a00,
    0x00463b05, 0x00000000, 0x00041361, 0x59070a00,
    0x00464f05, 0x00000000, 0x00041261, 0x29070a00,
    0x00465f05, 0x00000000, 0x00041161, 0x5d070a00,
    0x00466105, 0x00000000, 0x00041d61, 0x60050010,
    0x00665507, 0x00000000, 0x00041f61, 0x61050010,
    0x00665307, 0x00000000, 0x00041e61, 0x5f050010,
    0x00665707, 0x00000000, 0x00040e61, 0x64050010,
    0x00665907, 0x00000000, 0x00040e61, 0x63050010,
    0x00662907, 0x00000000, 0x00041e61, 0x62050010,
    0x00665d07, 0x00000000, 0x00040024, 0x0001c060,
    0x00000070, 0x00000070, 0x00041b61, 0x5f054110,
    0x00000000, 0x00800080, 0x00041f61, 0x60054110,
    0x00000000, 0x00800080, 0x00041f61, 0x61054110,
    0x00000000, 0x00800080, 0x00041d61, 0x62054110,
    0x00000000, 0x00000000, 0x00041f61, 0x63054110,
    0x00000000, 0x00000000, 0x00041f61, 0x64054110,
    0x00000000, 0x00000000, 0x00040025, 0x00004600,
    0x00000000, 0x000004f8, 0xa0781140, 0x43004b02,
    0x00040b61, 0x73060100, 0x00585f05, 0x00000000,
    0x00040b61, 0x74060100, 0x00586005, 0x00000000,
    0x00040a61, 0x75060100, 0x00586105, 0x00000000,
    0x00041f61, 0x38060100, 0x00586205, 0x00000000,
    0x00041f61, 0x39060100, 0x00586305, 0x00000000,
    0x00041f61, 0x7c060100, 0x00586405, 0x00000000,
    0x27621f70, 0x4b007803, 0xa0641740, 0x01c07803,
    0x00041e61, 0x53050020, 0x00567506, 0x00000000,
    0xa07a1b40, 0x4d026202, 0x27660b70, 0x78006403,
    0x00033561, 0x11060220, 0x00346405, 0x00000000,
    0x00133561, 0x13060220, 0x00346505, 0x00000000,
    0xa06b0b40, 0x7a026602, 0x00031961, 0x11260220,
    0x00346b05, 0x00000000, 0x00131a61, 0x13260220,
    0x00346c05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044631, 0x00000000,
    0xf3081124, 0x00025314, 0xa06c1740, 0x02807803,
    0x00043661, 0x54050020, 0x00567406, 0x00000000,
    0x276e0a70, 0x78006c03, 0x00033661, 0x11060220,
    0x00346c05, 0x00000000, 0x00133661, 0x13060220,
    0x00346d05, 0x00000000, 0xa0700b40, 0x7a026e02,
    0x00031961, 0x11260220, 0x00347005, 0x00000000,
    0x00131a61, 0x13260220, 0x00347105, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044731, 0x00000000, 0xf3081124, 0x00025414,
    0xa0711440, 0x03407803, 0x00043761, 0x55050020,
    0x00567306, 0x00000000, 0x27731a70, 0x78007103,
    0x00033761, 0x11060220, 0x00347105, 0x00000000,
    0x00133761, 0x13060220, 0x00347205, 0x00000000,
    0xa0750940, 0x7a027302, 0x00031961, 0x11260220,
    0x00347505, 0x00000000, 0x00131a61, 0x13260220,
    0x00347605, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044831, 0x00000000,
    0xf3081124, 0x00025514, 0xa0761140, 0x02207803,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0x00041361, 0x56050020, 0x00567c06, 0x00000000,
    0x277c1a70, 0x78007603, 0x00033861, 0x11060220,
    0x00347605, 0x00000000, 0x00133861, 0x13060220,
    0x00347705, 0x00000000, 0xa07e1b40, 0x7a027c02,
    0x00031961, 0x11260220, 0x00347e05, 0x00000000,
    0x00131a61, 0x13260220, 0x00347f05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044931, 0x00000000, 0xf3081124, 0x00025614,
    0xa0150040, 0x02e07803, 0x80003901, 0x00000000,
    0x00000000, 0x00000000, 0x00041361, 0x57050020,
    0x00563906, 0x00000000, 0x27171a70, 0x78001503,
    0x00033961, 0x11060220, 0x00341505, 0x00000000,
    0x00133961, 0x13060220, 0x00341605, 0x00000000,
    0xa0331b40, 0x7a021702, 0x00031961, 0x11260220,
    0x00343305, 0x00000000, 0x00131a61, 0x13260220,
    0x00343405, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044a31, 0x00000000,
    0xf3081124, 0x00025714, 0xa0340040, 0x03a07803,
    0x00043a61, 0x58050020, 0x00563806, 0x00000000,
    0x27361a70, 0x78003403, 0x00033a61, 0x11060220,
    0x00343405, 0x00000000, 0x00133a61, 0x13060220,
    0x00343505, 0x00000000, 0xa0381b40, 0x7a023602,
    0x00031961, 0x11260220, 0x00343805, 0x00000000,
    0x00131a61, 0x13260220, 0x00343905, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044b31, 0x00000000, 0xf3081124, 0x00025814,
    0x00040070, 0x00018660, 0x26463105, 0x00000000,
    0x80000061, 0x6b054660, 0x00000000, 0x00000002,
    0xa03b0040, 0x01607803, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0xef390062, 0x00116b03,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0x00031a61, 0x11060220, 0x00343b05, 0x00000000,
    0x80103b01, 0x00000000, 0x00000000, 0x00000000,
    0x00131b61, 0x13060220, 0x00343c05, 0x00000000,
    0x00041b61, 0x7d070200, 0x00463905, 0x00000000,
    0x274f3570, 0x78003b03, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x00041a61, 0x59050020,
    0x00667d07, 0x00000000, 0x80003601, 0x00000000,
    0x00000000, 0x00000000, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0xa0531a40, 0x7a024f02,
    0x00031961, 0x11260220, 0x00345305, 0x00000000,
    0x00131a61, 0x13260220, 0x00345405, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x00000000, 0xf3081124, 0x00025914,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x00040024, 0x0001c060, 0x00001698, 0x00001698,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0x00040961, 0x54050020, 0x00661f1f, 0x00000000,
    0x00040070, 0x00010660, 0x56466905, 0x00464505,
    0xef4f0a62, 0x00005403, 0x80003901, 0x00000000,
    0x00000000, 0x00000000, 0x00040961, 0x55050020,
    0x00664f07, 0x00000000, 0x00040070, 0x00010220,
    0x52464305, 0x00464505, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x80040a61, 0x10014110,
    0x00000000, 0x0aa00aa0, 0x00040069, 0x10018510,
    0x01565106, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0aa00aa0, 0xe0570961, 0x001b0004,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x20590966, 0x57004f03, 0x00040961, 0x29050020,
    0x00665907, 0x00000000, 0x80040961, 0x10014110,
    0x00000000, 0x05200520, 0x00040069, 0x10018510,
    0x01560906, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x05200520, 0xe05d0961, 0x001b0004,
    0x205f1966, 0x5d002903, 0x00041961, 0x61050020,
    0x00665f07, 0x00000000, 0x80041961, 0x10014110,
    0x00000000, 0x0c200c20, 0x00040069, 0x10018510,
    0x01560b06, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0c200c20, 0xe0630961, 0x001b0004,
    0x20650966, 0x63005f03, 0x00040961, 0x6b070200,
    0x00006504, 0x00000000, 0x01040022, 0x0001c060,
    0x00000198, 0x00000128, 0x00041761, 0x67050020,
    0x00006504, 0x00000000, 0xac6d0970, 0x00006703,
    0x00040070, 0x00018660, 0x16464f05, 0x00000000,
    0x01040a62, 0x6f058220, 0x02466d05, 0xffffffff,
    0x00041970, 0x00018660, 0x26466f05, 0x00000000,
    0x80002201, 0x00000000, 0x00000000, 0x00000000,
    0x01041162, 0x37058220, 0x02462105, 0xff800000,
    0x80002201, 0x00000000, 0x00000000, 0x00000000,
    0x01041662, 0x39058220, 0x02462305, 0xff800000,
    0x80002201, 0x00000000, 0x00000000, 0x00000000,
    0x01041462, 0x3b058220, 0x02462505, 0xff800000,
    0x01042062, 0x31058220, 0x02461905, 0x7f800000,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x01041562, 0x33058220, 0x02461b05, 0x7f800000,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x01041362, 0x35058220, 0x02461d05, 0x7f800000,
    0x00040024, 0x0001c060, 0x00000080, 0x00000080,
    0x00040a61, 0x35054220, 0x00000000, 0x7f800000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x00040c61, 0x33054220, 0x00000000, 0x7f800000,
    0x00041e61, 0x31054220, 0x00000000, 0x7f800000,
    0x00040c61, 0x3b054220, 0x00000000, 0xff800000,
    0x00040e61, 0x39054220, 0x00000000, 0xff800000,
    0x00041161, 0x37054220, 0x00000000, 0xff800000,
    0x00040025, 0x00004600, 0x00000000, 0x00001358,
    0x80040a61, 0x10014110, 0x00000000, 0x06200620,
    0x00040069, 0x10018510, 0x01565106, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x06200620,
    0xe0590961, 0x001b0004, 0x80040d61, 0x10014110,
    0x00000000, 0x06600660, 0x00040069, 0x10018510,
    0x01565106, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x06600660, 0xe0150961, 0x001b0004,
    0x80040961, 0x10014110, 0x00000000, 0x06a006a0,
    0x00040069, 0x10018510, 0x01565106, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x06a006a0,
    0xe0550961, 0x001b0004, 0x80040a61, 0x10014110,
    0x00000000, 0x06e006e0, 0x00040069, 0x10018510,
    0x01565106, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x06e006e0, 0xe0570961, 0x001b0004,
    0x80040961, 0x10014110, 0x00000000, 0x07200720,
    0x00040069, 0x10018510, 0x01565106, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x07200720,
    0xe0530961, 0x001b0004, 0x80040b61, 0x10014110,
    0x00000000, 0x07600760, 0x00040069, 0x10018510,
    0x01565106, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x07600760, 0xe0170961, 0x001b0004,
    0x00040070, 0x00018220, 0x52464305, 0x00000006,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x27111f62, 0x59003100, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x27131e62, 0x15003300,
    0x27151d62, 0x55003500, 0x25591a62, 0x17003b00,
    0x25551c62, 0x57003700, 0x80041261, 0x10014110,
    0x00000000, 0x02200220, 0x00040069, 0x10018510,
    0x01560906, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x02200220, 0xe0170961, 0x001b0004,
    0x25571c62, 0x53003900, 0x80041561, 0x10014110,
    0x00000000, 0x02600260, 0x00040069, 0x10018510,
    0x01560906, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x02600260, 0xe0510961, 0x001b0004,
    0x80041161, 0x10014110, 0x00000000, 0x02a002a0,
    0x00040069, 0x10018510, 0x01560906, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x02a002a0,
    0xe0530961, 0x001b0004, 0x80041361, 0x10014110,
    0x00000000, 0x0b200b20, 0x00040069, 0x10018510,
    0x01560906, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0b200b20, 0xe05f0961, 0x001b0004,
    0x80041261, 0x10014110, 0x00000000, 0x0aa00aa0,
    0x00040069, 0x10018510, 0x01560906, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0aa00aa0,
    0xe0290961, 0x001b0004, 0x80041161, 0x10014110,
    0x00000000, 0x0ae00ae0, 0x00040069, 0x10018510,
    0x01560906, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x0ae00ae0, 0xe05d0961, 0x001b0004,
    0x27091962, 0x17001100, 0x27110962, 0x51001300,
    0x27130962, 0x53001500, 0x25150962, 0x29005500,
    0x25171962, 0x5d005700, 0x25291b62, 0x5f005900,
    0x80041461, 0x10014110, 0x00000000, 0x01200120,
    0x00040069, 0x10018510, 0x01560b06, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x01200120,
    0xe0530961, 0x001b0004, 0x80040b61, 0x10014110,
    0x00000000, 0x02200220, 0x00040069, 0x10018510,
    0x01560b06, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x02200220, 0xe0550961, 0x001b0004,
    0x80040a61, 0x10014110, 0x00000000, 0x02600260,
    0x00040069, 0x10018510, 0x01560b06, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x02600260,
    0xe0570961, 0x001b0004, 0x80040961, 0x10014110,
    0x00000000, 0x02a002a0, 0x00040069, 0x10018510,
    0x01560b06, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x02a002a0, 0xe0590961, 0x001b0004,
    0x80041261, 0x10014110, 0x00000000, 0x02e002e0,
    0x00040069, 0x10018510, 0x01560b06, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x02e002e0,
    0xe05d0961, 0x001b0004, 0x80041161, 0x10014110,
    0x00000000, 0x05200520, 0x00040069, 0x10018510,
    0x01560b06, 0x00020002, 0x00040940, 0x10018110,
    0x01461001, 0x05200520, 0xe05f0961, 0x001b0004,
    0x27511e62, 0x53000900, 0x27531d62, 0x55001100,
    0x27551c62, 0x57001300, 0x25571b62, 0x59001500,
    0x25591a62, 0x5d001700, 0x255d1962, 0x5f002900,
    0x01040022, 0x0001c060, 0x00000e88, 0x00000e88,
    0x25130b62, 0x57aa5100, 0x25110b62, 0x59aa5300,
    0x25170b62, 0x5daa5500, 0xa0650040, 0x01004b03,
    0x25150962, 0x17001100, 0x00040061, 0x7e064540,
    0x00000000, 0x00010001, 0x00030a61, 0x09060220,
    0x00346505, 0x00000000, 0x00131b61, 0x0b060220,
    0x00346605, 0x00000000, 0x25111162, 0x15001300,
    0x00041b61, 0x67050020, 0x00567e06, 0x00000000,
    0xe0290941, 0x34001100, 0x20111140, 0x29215100,
    0x20130040, 0x29215300, 0x20150040, 0x29215500,
    0x20510040, 0x29015700, 0x20530040, 0x29015900,
    0x20550040, 0x29015d00, 0x20571340, 0x11205100,
    0x20511340, 0x13205300, 0x20531340, 0x15205500,
    0x00041341, 0x55058aa0, 0x0a465705, 0x3f800001,
    0x00041341, 0x57058aa0, 0x0a465105, 0x3f800001,
    0x00041341, 0x51058aa0, 0x0a465305, 0x3f800001,
    0x00041165, 0x53058220, 0x02465505, 0x007fffff,
    0x00041765, 0x59058220, 0x02465505, 0x80000000,
    0x00040065, 0x5f058220, 0x02465505, 0x7f800000,
    0x00041265, 0x29058220, 0x02465705, 0x80000000,
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
    0x20511966, 0x29005903, 0x11040062, 0x59058220,
    0x02466105, 0x3f000000, 0x00041965, 0x29058220,
    0x02465905, 0x7fffffff, 0x00041970, 0x59058aa0,
    0x3a466305, 0x3f7f0000, 0x205f0966, 0x5d002903,
    0x00041970, 0x29058aa0, 0x3a465105, 0x3f7f0000,
    0x00041152, 0x51044560, 0x0e2eff82, 0x59055305,
    0x00041970, 0x59058aa0, 0x3a465f05, 0x3f7f0000,
    0x00041252, 0x53044560, 0x0e2eff82, 0x29055505,
    0x27290070, 0x4b006503, 0x00041152, 0x55044560,
    0x0e2eff82, 0x59055705, 0xa0571a40, 0x4d022902,
    0x00031961, 0x09260220, 0x00345705, 0x00000000,
    0x00131a61, 0x0b260220, 0x00345805, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044c31, 0x00000000, 0xf3080924, 0x00026714,
    0x00043061, 0x17050660, 0x00460705, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x00000000, 0xfb0c0d24, 0x003c1144,
    0xa0700040, 0x01204b03, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x00041f61, 0x05070200,
    0x00465105, 0x00000000, 0x27721a70, 0x4b007003,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x01060220, 0x00347005, 0x00000000,
    0x00133061, 0x03060220, 0x00347105, 0x00000000,
    0x00040061, 0x57070200, 0x00465305, 0x00000000,
    0x00040061, 0x59070200, 0x00465505, 0x00000000,
    0x00041e61, 0x29070000, 0x00660507, 0x00000000,
    0xa0741e40, 0x4d027202, 0x00041a61, 0x290f0000,
    0x00665707, 0x00000000, 0x00031a61, 0x01260220,
    0x00347405, 0x00000000, 0x00131b61, 0x03260220,
    0x00347505, 0x00000000, 0x00041b61, 0x29170000,
    0x00665907, 0x00000000, 0x00041961, 0x291f0000,
    0x00666b07, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044d31, 0x00000000,
    0xfb080124, 0x00002914, 0x00040070, 0x00010220,
    0x52464305, 0x00464505, 0x01040022, 0x0001c060,
    0x000004c0, 0x00000440, 0x256c1b62, 0x37433100,
    0x256e0062, 0x39433300, 0x25700062, 0x3b433500,
    0x00041f69, 0x7205a660, 0x02465105, 0x00000017,
    0x00041e69, 0x7405a660, 0x02465305, 0x00000017,
    0x00040069, 0x7605a660, 0x02465505, 0x00000017,
    0x00040070, 0x00018660, 0x16464f05, 0x00000000,
    0x25051162, 0x70006e00, 0x00041c40, 0x07058660,
    0x06467205, 0x43800000, 0x80003c01, 0x00000000,
    0x00000000, 0x00000000, 0x00041c40, 0x09058660,
    0x06467405, 0x43800000, 0x80003c01, 0x00000000,
    0x00000000, 0x00000000, 0x00041c40, 0x0b058660,
    0x06467605, 0x43800000, 0x25750962, 0x05006c00,
    0xe0051141, 0x34007500, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x200d1140, 0x05203100,
    0x200f3040, 0x05203300, 0x20311240, 0x05203500,
    0x20331240, 0x05003700, 0x20351240, 0x05003900,
    0x20371240, 0x05003b00, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x20391240, 0x11200d00,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x203b1240, 0x13200f00, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x204f0c40, 0x15203100,
    0x20701640, 0x11203300, 0x20720b40, 0x13203500,
    0x20740a40, 0x15203700, 0x00040e41, 0x51058aa0,
    0x0a463905, 0x3f7ffffe, 0x00040e41, 0x53058aa0,
    0x0a463b05, 0x3f7ffffe, 0x00040d41, 0x55058aa0,
    0x0a464f05, 0x3f7ffffe, 0x00040941, 0x76058aa0,
    0x0a467005, 0x3f800001, 0x00041641, 0x05058aa0,
    0x0a467205, 0x3f800001, 0x00041641, 0x0d058aa0,
    0x0a467405, 0x3f800001, 0x20570b41, 0x07005100,
    0x20590a41, 0x09005300, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x20290941, 0x0b005500,
    0x200f1641, 0x07007600, 0x20111641, 0x09000500,
    0x20131641, 0x0b000d00, 0x605d1645, 0x00105700,
    0x605f1645, 0x00105900, 0x60611645, 0x00102900,
    0x60151645, 0x00120f00, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x60171645, 0x00121100,
    0x60311645, 0x00121300, 0xe5631662, 0x00005d00,
    0xe5651662, 0x00005f00, 0x80003c01, 0x00000000,
    0x00000000, 0x00000000, 0xe5671662, 0x00006100,
    0xe5331662, 0x00021500, 0xe5351662, 0x00021700,
    0xe5371662, 0x00023100, 0x00040e62, 0x6b058aa0,
    0x5a466305, 0x437f0000, 0x00041662, 0x6d058aa0,
    0x5a466505, 0x437f0000, 0x00041662, 0x6f058aa0,
    0x5a466705, 0x437f0000, 0x00041662, 0x39058aa0,
    0x5a463305, 0x437f0000, 0x00041662, 0x3b058aa0,
    0x5a463505, 0x437f0000, 0x00041662, 0x4f058aa0,
    0x5a463705, 0x437f0000, 0x00041661, 0x78070a00,
    0x00466b05, 0x00000000, 0x00041561, 0x7a070a00,
    0x00466d05, 0x00000000, 0x00041461, 0x7c070a00,
    0x00466f05, 0x00000000, 0x00041361, 0x7e070a00,
    0x00463905, 0x00000000, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x00041261, 0x01070a00,
    0x00463b05, 0x00000000, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x00041161, 0x03070a00,
    0x00464f05, 0x00000000, 0x00041e61, 0x53050010,
    0x00667807, 0x00000000, 0x00041e61, 0x54050010,
    0x00667a07, 0x00000000, 0x00041e61, 0x55050010,
    0x00667c07, 0x00000000, 0x00041e61, 0x50050010,
    0x00667e07, 0x00000000, 0x00041e61, 0x51050010,
    0x00660107, 0x00000000, 0x00041e61, 0x52050010,
    0x00660307, 0x00000000, 0x11040c62, 0x67058110,
    0x01585305, 0x00000000, 0x11040d62, 0x66058110,
    0x01585405, 0x00000000, 0x11040d62, 0x65058110,
    0x01585505, 0x00000000, 0x11041e62, 0x6c058110,
    0x01585005, 0x00000000, 0x11041e62, 0x6b058110,
    0x01585105, 0x00000000, 0x11040c62, 0x68058110,
    0x01585205, 0x00000000, 0x00040024, 0x0001c060,
    0x00000090, 0x00000090, 0x00041d61, 0x65054110,
    0x00000000, 0x00800080, 0x00041f61, 0x66054110,
    0x00000000, 0x00800080, 0x80003c01, 0x00000000,
    0x00000000, 0x00000000, 0x00041f61, 0x67054110,
    0x00000000, 0x00800080, 0x80003c01, 0x00000000,
    0x00000000, 0x00000000, 0x00041d61, 0x68054110,
    0x00000000, 0x00000000, 0x00041f61, 0x6b054110,
    0x00000000, 0x00000000, 0x00041f61, 0x6c054110,
    0x00000000, 0x00000000, 0x00040025, 0x00004600,
    0x00000000, 0x00000540, 0xa0560040, 0x43004b02,
    0x00041f61, 0x06060100, 0x00586505, 0x00000000,
    0x00041f61, 0x07060100, 0x00586605, 0x00000000,
    0x00041f61, 0x08060100, 0x00586705, 0x00000000,
    0x00041f61, 0x5a060100, 0x00586805, 0x00000000,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x00041f61, 0x09060100, 0x00586b05, 0x00000000,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x00041f61, 0x0a060100, 0x00586c05, 0x00000000,
    0x27581f70, 0x4b005603, 0xa05d0040, 0x01c05603,
    0x00040b61, 0x63050020, 0x00560806, 0x00000000,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0xa0291b40, 0x4d025802, 0x275f0970, 0x56005d03,
    0x00031361, 0x32060220, 0x00345d05, 0x00000000,
    0x00131261, 0x34060220, 0x00345e05, 0x00000000,
    0xa0610b40, 0x29025f02, 0x00031961, 0x32260220,
    0x00346105, 0x00000000, 0x00131a61, 0x34260220,
    0x00346205, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044e31, 0x00000000,
    0xf3083224, 0x00026314, 0xa0623e40, 0x02805603,
    0x00040061, 0x6f050020, 0x00560706, 0x00000000,
    0x80003e01, 0x00000000, 0x00000000, 0x00000000,
    0x27641a70, 0x56006203, 0x00030061, 0x6b060220,
    0x00346205, 0x00000000, 0x00130061, 0x6d060220,
    0x00346305, 0x00000000, 0xa0661b40, 0x29026402,
    0x00031961, 0x6b260220, 0x00346605, 0x00000000,
    0x00131a61, 0x6d260220, 0x00346705, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044f31, 0x00000000, 0xf3086b24, 0x00026f14,
    0xa0670040, 0x03405603, 0x00040061, 0x74050020,
    0x00560606, 0x00000000, 0x80003f01, 0x00000000,
    0x00000000, 0x00000000, 0x276b1a70, 0x56006703,
    0x00033f61, 0x70060220, 0x00346705, 0x00000000,
    0x00130061, 0x72060220, 0x00346805, 0x00000000,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0xa06d1b40, 0x29026b02, 0x00031961, 0x70260220,
    0x00346d05, 0x00000000, 0x00131a61, 0x72260220,
    0x00346e05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044031, 0x00000000,
    0xf3087024, 0x00027414, 0xa06e3f40, 0x02205603,
    0x00040061, 0x79050020, 0x00560a06, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x27701a70, 0x56006e03, 0x00033061, 0x75060220,
    0x00346e05, 0x00000000, 0x00130061, 0x77060220,
    0x00346f05, 0x00000000, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0xa0721b40, 0x29027002,
    0x00031961, 0x75260220, 0x00347205, 0x00000000,
    0x00131a61, 0x77260220, 0x00347305, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x00000000, 0xf3087524, 0x00027914,
    0xa0733040, 0x02e05603, 0x00040061, 0x7e050020,
    0x00560906, 0x00000000, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x27751a70, 0x56007303,
    0x00033161, 0x7a060220, 0x00347305, 0x00000000,
    0x00130061, 0x7c060220, 0x00347405, 0x00000000,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0xa0771b40, 0x29027502, 0x00031961, 0x7a260220,
    0x00347705, 0x00000000, 0x00131a61, 0x7c260220,
    0x00347805, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xf3087a24, 0x00027e14, 0xa0783140, 0x03a05603,
    0x00040061, 0x05050020, 0x00565a06, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x277a1a70, 0x56007803, 0x00033d61, 0x01060220,
    0x00347805, 0x00000000, 0x00133d61, 0x03060220,
    0x00347905, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0xa07c1b40, 0x29027a02,
    0x00031961, 0x01260220, 0x00347c05, 0x00000000,
    0x00131a61, 0x03260220, 0x00347d05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x00000000, 0xf3080124, 0x00020514,
    0xa07d3240, 0x01605603, 0x00043c61, 0x0c064540,
    0x00000000, 0x00020002, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x27011a70, 0x56007d03,
    0x00033061, 0x06060220, 0x00347d05, 0x00000000,
    0x00130061, 0x08060220, 0x00347e05, 0x00000000,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x00041c61, 0x0a050020, 0x00560c06, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0xa0031c40, 0x29020102, 0x00031961, 0x06260220,
    0x00340305, 0x00000000, 0x00131a61, 0x08260220,
    0x00340405, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044031, 0x00000000,
    0xf3080624, 0x00020a14, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x00040070, 0x00010660,
    0x56466905, 0x00464505, 0x01040022, 0x0001c060,
    0x00000748, 0x00000748, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x00040d6c, 0x04058660,
    0x02464505, 0x0000001f, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x00041769, 0x06058660,
    0x02466905, 0x00000005, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0xe0081668, 0x01b06903,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00031661, 0x0a050220, 0x00442b26, 0x00000000,
    0x80103001, 0x00000000, 0x00000000, 0x00000000,
    0x00131f61, 0x0b050220, 0x00443f26, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0xa10c1c40, 0x060e2b02, 0x80103001, 0x00000000,
    0x00000000, 0x00000000, 0xaa0d1d40, 0x070e3f02,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00031a70, 0x0e050220, 0x52460c05, 0x00442b06,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00031461, 0x12060220, 0x00340c05, 0x00000000,
    0x80103001, 0x00000000, 0x00000000, 0x00000000,
    0x00131b70, 0x0f050220, 0x52460d05, 0x00443f06,
    0x80103001, 0x00000000, 0x00000000, 0x00000000,
    0x00131361, 0x14060220, 0x00340d05, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00040a52, 0x10040e68, 0x0e2e0a05, 0x0e050805,
    0x00031961, 0x12260220, 0x00341005, 0x00000000,
    0x00131a61, 0x14260220, 0x00341105, 0x00000000,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x00000000, 0xfb0c1224, 0x003c1944,
    0xa0123340, 0x01000c03, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x27141970, 0x0c001203,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x00031261, 0x18060220, 0x00341205, 0x00000000,
    0x00133361, 0x1a060220, 0x00341305, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0xa0160a40, 0x10021402, 0x00031961, 0x18260220,
    0x00341605, 0x00000000, 0x00131a61, 0x1a260220,
    0x00341705, 0x00000000, 0x80002201, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044431, 0x00000000,
    0xfb0c1824, 0x003c2144, 0x00043469, 0x17058660,
    0x02466905, 0x00000002, 0xe0193468, 0x01e06903,
    0x00033461, 0x1b050220, 0x00442d26, 0x00000000,
    0x00133361, 0x1c050220, 0x00444126, 0x00000000,
    0xa1620c40, 0x170e2d02, 0x80103301, 0x00000000,
    0x00000000, 0x00000000, 0xaa1d1d40, 0x180e4102,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x00031a70, 0x1e050220, 0x52466205, 0x00442d06,
    0x00033461, 0x22060220, 0x00346205, 0x00000000,
    0x80103301, 0x00000000, 0x00000000, 0x00000000,
    0x00131b70, 0x1f050220, 0x52461d05, 0x00444106,
    0x00133461, 0x24060220, 0x00341d05, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x00041a52, 0x20040e68, 0x0e2e1b05, 0x1e051905,
    0x00031961, 0x22260220, 0x00342005, 0x00000000,
    0x00131a61, 0x24260220, 0x00342105, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044531, 0x00000000, 0xfb0c2224, 0x00046914,
    0xe03b1468, 0x00612f03, 0xa0213540, 0x69004502,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x27231970, 0x45002103, 0x00043469, 0x27058660,
    0x02462105, 0x00000002, 0xe02f0068, 0x01e02103,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0xa0251b40, 0x04022302, 0xa1630b40, 0x270e2d02,
    0x80103e01, 0x00000000, 0x00000000, 0x00000000,
    0xaa320c40, 0x280e4102, 0x00041b69, 0x2b058660,
    0x02462505, 0x00000002, 0x80003e01, 0x00000000,
    0x00000000, 0x00000000, 0x00030b70, 0x33050220,
    0x52466305, 0x00442d06, 0x00031161, 0x37060220,
    0x00346305, 0x00000000, 0x80103e01, 0x00000000,
    0x00000000, 0x00000000, 0x00130b70, 0x34050220,
    0x52463205, 0x00444106, 0x00131661, 0x39060220,
    0x00343205, 0x00000000, 0x20310d66, 0x2f002b03,
    0x80003e01, 0x00000000, 0x00000000, 0x00000000,
    0x00040952, 0x35040e68, 0x0e2e1b05, 0x33053105,
    0x00031961, 0x37260220, 0x00343505, 0x00000000,
    0x00131a61, 0x39260220, 0x00343605, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044631, 0x00000000, 0xfb0c3724, 0x00043b14,
    0x00040065, 0x00010220, 0x22464705, 0x00464905,
    0x01040022, 0x0001c060, 0x000001f0, 0x000001f0,
    0x00043669, 0x36058660, 0x02464505, 0x00000003,
    0xa1641340, 0x028e5b03, 0xaa383640, 0x028e3d03,
    0x00041b66, 0x47058220, 0x02463605, 0xffffffc0,
    0x00030b70, 0x65050220, 0x52466405, 0x00445b06,
    0x80103601, 0x00000000, 0x00000000, 0x00000000,
    0x00131b70, 0x39050220, 0x52463805, 0x00443d06,
    0x00030061, 0x3f060220, 0x00346405, 0x00000000,
    0x00130061, 0x41060220, 0x00343805, 0x00000000,
    0x00030c40, 0x66052660, 0x06466505, 0x00445b26,
    0x80103601, 0x00000000, 0x00000000, 0x00000000,
    0x00131c40, 0x3a052660, 0x06463905, 0x00443d26,
    0x00031a61, 0x3f260220, 0x00346605, 0x00000000,
    0x00131a61, 0x41260220, 0x00343a05, 0x00000000,
    0x80003601, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044731, 0x3b140000, 0xfb043f24, 0x00040000,
    0x00042769, 0x3f058660, 0x02463b05, 0x00000006,
    0xa1670940, 0x3f0e5b02, 0xaa401a40, 0x400e3d02,
    0x00030a70, 0x68050220, 0x52466705, 0x00445b06,
    0x00030061, 0x43060220, 0x00346705, 0x00000000,
    0x80103701, 0x00000000, 0x00000000, 0x00000000,
    0x00131b70, 0x41050220, 0x52464005, 0x00443d06,
    0x00130061, 0x45060220, 0x00344005, 0x00000000,
    0x00031c40, 0x69052660, 0x06466805, 0x00445b26,
    0x80103701, 0x00000000, 0x00000000, 0x00000000,
    0x00131b40, 0x42052660, 0x06464105, 0x00443d26,
    0x00031a61, 0x43260220, 0x00346905, 0x00000000,
    0x00131a61, 0x45260220, 0x00344205, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044831, 0x00000000, 0xfb0c4324, 0x00044714,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x80030061, 0x7e050220, 0x00460005, 0x00000000,
    0x80040931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_build_DFS_DFS_trivial = {
   .prog_data = {
      .base.nr_params = 17,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 3,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 14608,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_build_DFS_DFS_trivial_relocs,
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
      .push.cross_thread.dwords = 17,
      .push.cross_thread.regs = 3,
      .push.cross_thread.size = 96,
   },
   .args_size = 36,
   .arg_count = 5,
   .args = gfx125_bvh_build_DFS_DFS_trivial_args,
   .code = gfx125_bvh_build_DFS_DFS_trivial_code,
};
const char *gfx125_bvh_build_DFS_DFS_trivial_sha1 = "9f0f2036ebeefc179922dff7f2ddba6fd48c5dbd";
