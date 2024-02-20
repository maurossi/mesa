#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_build_primref_procedurals_to_primrefs_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_build_primref_procedurals_to_primrefs_args[] = {
   { 0, 8 },
   { 8, 8 },
   { 16, 8 },
   { 24, 8 },
   { 32, 4 },
   { 36, 4 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g89<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g27<1>UD        g0.1<0,1,0>UD                   { align1 1H };
add(1)          g90<1>UD        g89<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@2 compacted };
add(1)          g91<1>UD        g89<0,1,0>UD    0x00000040UD    { align1 WE_all 1N compacted };
mov(8)          g13<1>UW        0x76543210V                     { align1 WE_all 1Q };
shl(16)         g38<1>D         g27<8,8,1>D     0x00000004UD    { align1 1H I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
send(1)         g1UD            g90UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(1)         g3UD            g91UD           nullUD          0x0210c500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V8, transpose, L1STATE_L3MOCS dst_len = 1, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $1 };
add(8)          g13.8<1>UW      g13<1,1,0>UW    0x0008UW        { align1 WE_all 1Q I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         g40<1>D         g13<8,8,1>UW                    { align1 1H };
add(16)         g42<1>D         g38<1,1,0>D     g40<1,1,0>D     { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g52.1<2>F       g2.7<0,1,0>F                    { align1 1Q };
mov(8)          g54.1<2>F       g2.7<0,1,0>F                    { align1 2Q };
mov(8)          g31.1<2>F       g2.5<0,1,0>F                    { align1 1Q };
mov(8)          g33.1<2>F       g2.5<0,1,0>F                    { align1 2Q };
mov(8)          g27.1<2>F       g2.3<0,1,0>F                    { align1 1Q I@4 };
mov(8)          g29.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g23.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g25.1<2>F       g2.1<0,1,0>F                    { align1 2Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.dst };
cmp.l.f0.0(16)  null<1>UD       g42<8,8,1>UD    g3.1<0,1,0>UD   { align1 1H I@1 };
mov(16)         g35<1>F         g3<0,1,0>F                      { align1 1H compacted };
and(16)         g10<1>UD        g3<0,1,0>UD     0x00ffffffUD    { align1 1H };
mov(8)          g52<2>F         g2.6<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g54<2>F         g2.6<0,1,0>F                    { align1 2Q F@7 compacted };
mov(8)          g31<2>F         g2.4<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g33<2>F         g2.4<0,1,0>F                    { align1 2Q F@7 compacted };
mov(8)          g27<2>F         g2.2<0,1,0>F                    { align1 1Q F@7 compacted };
mov(8)          g29<2>F         g2.2<0,1,0>F                    { align1 2Q F@7 compacted };
mov(8)          g23<2>F         g2<0,1,0>F                      { align1 1Q F@7 compacted };
mov(8)          g25<2>F         g2<0,1,0>F                      { align1 2Q F@7 compacted };
mov(16)         g37<1>UD        g35.3<32,8,4>UB                 { align1 1H F@7 };
(+f0.0) if(16)  JIP:  LABEL1          UIP:  LABEL0              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@7 };
send(16)        g44UD           g52UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
mul(8)          acc0<1>UD       g42<8,8,1>UD    g48<16,8,2>UW   { align1 1Q $2.dst };
mul(16)         g54<1>D         g42<8,8,1>D     g48<16,8,2>UW   { align1 1H $2.dst };
mul(16)         g107<1>D        g42<8,8,1>D     g48.1<16,8,2>UW { align1 1H };
mul(16)         g56<1>D         g42<8,8,1>D     g50<16,8,2>UW   { align1 1H $2.dst };
mach(8)         g52<1>UD        g42<1,1,0>UD    g48<1,1,0>UD    { align1 1Q $2.src compacted AccWrEnable };
add(16)         g54.1<2>UW      g54.1<16,8,2>UW g107<16,8,2>UW  { align1 1H I@3 };
mul(16)         g108<1>D        g42<8,8,1>D     g50.1<16,8,2>UW { align1 1H };
mul(8)          acc0<1>UD       g43<8,8,1>UD    g49<16,8,2>UW   { align1 2Q };
add(16)         g60<1>D         g44<1,1,0>D     g54<1,1,0>D     { align1 1H @3 $2.dst compacted };
add(16)         g56.1<2>UW      g56.1<16,8,2>UW g108<16,8,2>UW  { align1 1H I@3 };
mach(8)         g53<1>UD        g43<8,8,1>UD    g49<8,8,1>UD    { align1 2Q $2.src AccWrEnable };
cmp.l.f0.0(16)  g62<1>UD        g60<1,1,0>UD    g44<1,1,0>UD    { align1 1H I@3 compacted };
add(16)         g73<1>D         g60<1,1,0>D     16D             { align1 1H compacted };
add(16)         g58<1>D         g52<1,1,0>D     g56<1,1,0>D     { align1 1H I@3 compacted };
cmp.l.f0.0(16)  g75<1>UD        g73<1,1,0>UD    g60<1,1,0>UD    { align1 1H I@2 compacted };
add3(16)        g64<1>D         g46<8,8,1>D     g58<8,8,1>D     -g62<1,1,1>D { align1 1H @2 $2.dst };
mov(8)          g53<2>UD        g60<4,4,1>UD                    { align1 1Q };
mov(8)          g55<2>UD        g61<4,4,1>UD                    { align1 2Q };
add(16)         g77<1>D         -g75<1,1,0>D    g64<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g53.1<2>UD      g64<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g55.1<2>UD      g65<4,4,1>UD                    { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g65UD           g53UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
mov(8)          g54<2>UD        g73<4,4,1>UD                    { align1 1Q $3.src };
mov(8)          g56<2>UD        g74<4,4,1>UD                    { align1 2Q $3.src };
mov(8)          g54.1<2>UD      g77<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g56.1<2>UD      g78<4,4,1>UD                    { align1 2Q I@2 };
cmp.l.f0.0(16)  g79<1>F         (abs)g67<1,1,0>F 0x7f800000F  /* infF */ { align1 1H $3.dst compacted };
cmp.l.f0.0(16)  g81<1>F         (abs)g65<1,1,0>F 0x7f800000F  /* infF */ { align1 1H $3.dst compacted };
cmp.l.f0.0(16)  g85<1>F         (abs)g69<1,1,0>F 0x7f800000F  /* infF */ { align1 1H $3.dst compacted };
and(16)         g83<1>UD        g79<1,1,0>UD    g81<1,1,0>UD    { align1 1H F@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g78UD           g54UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
and(16)         g87<1>UD        g83<1,1,0>UD    g85<1,1,0>UD    { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
cmp.l.f0.0(16)  g89<1>F         (abs)g78<1,1,0>F 0x7f800000F  /* infF */ { align1 1H $4.dst compacted };
sel.l(16)       g101<1>F        g67<1,1,0>F     g78<1,1,0>F     { align1 1H compacted };
sel.ge(16)      g105<1>F        g67<1,1,0>F     g78<1,1,0>F     { align1 1H compacted };
sel.ge(16)      g107<1>F        g69<1,1,0>F     g80<1,1,0>F     { align1 1H $4.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
cmp.l.f0.0(16)  g91<1>F         (abs)g71<1,1,0>F 0x7f800000F  /* infF */ { align1 1H $3.dst compacted };
cmp.l.f0.0(16)  g95<1>F         (abs)g80<1,1,0>F 0x7f800000F  /* infF */ { align1 1H compacted };
and(16)         g93<1>UD        g89<1,1,0>UD    g91<1,1,0>UD    { align1 1H F@2 compacted };
and(16)         g97<1>UD        g93<1,1,0>UD    g95<1,1,0>UD    { align1 1H A@1 compacted };
and.nz.f0.0(16) g99<1>UD        g87<1,1,0>UD    g97<1,1,0>UD    { align1 1H I@1 compacted };
mov(16)         g109<2>W        -g99<8,8,1>D                    { align1 1H I@1 };
(+f0.0) sel(16) g6<1>UD         g101<8,8,1>UD   0x7f800000UD    { align1 1H F@5 };
(+f0.0) sel(16) g16<1>UD        g107<8,8,1>UD   0xff800000UD    { align1 1H F@3 };
(+f0.0) sel(16) g14<1>UD        g105<8,8,1>UD   0xff800000UD    { align1 1H F@4 };
sel.l(16)       g100<1>F        g65<1,1,0>F     g71<1,1,0>F     { align1 1H I@3 compacted };
sel.l(16)       g102<1>F        g69<1,1,0>F     g80<1,1,0>F     { align1 1H I@3 compacted };
mov(16)         g20<1>UW        g109<16,8,2>UW                  { align1 1H I@4 };
(+f0.0) sel(16) g4<1>UD         g100<8,8,1>UD   0x7f800000UD    { align1 1H F@2 };
(+f0.0) sel(16) g8<1>UD         g102<8,8,1>UD   0x7f800000UD    { align1 1H F@1 };
sel.ge(16)      g103<1>F        g65<1,1,0>F     g71<1,1,0>F     { align1 1H I@1 compacted };
(+f0.0) sel(16) g12<1>UD        g103<8,8,1>UD   0xff800000UD    { align1 1H F@1 };
else(16)        JIP:  LABEL0          UIP:  LABEL0              { align1 1H };

LABEL1:
mov(16)         g20<1>UW        0x0000UW                        { align1 1H I@5 };
mov(16)         g8<1>UD         0x7f800000UD                    { align1 1H I@4 };
mov(16)         g6<1>UD         0x7f800000UD                    { align1 1H I@7 };
mov(16)         g4<1>UD         0x7f800000UD                    { align1 1H I@7 };
mov(16)         g16<1>UD        0xff800000UD                    { align1 1H };
mov(16)         g14<1>UD        0xff800000UD                    { align1 1H };
mov(16)         g12<1>UD        0xff800000UD                    { align1 1H I@7 };

LABEL0:
endif(16)       JIP:  LABEL2                                    { align1 1H };

LABEL2:
and(16)         g108<1>UW       g20<1,1,0>UW    0x0001UW        { align1 1H I@6 compacted };
mov(16)         g109<1>W        g108<32,16,2>B                  { align1 1H I@1 };
mov(1)          g113<2>UW       0x00000000UD                    { align1 WE_all 1N };
cmp.nz.f0.0(16) g112<1>W        g109<16,16,1>W  0W              { align1 1H I@2 };
mov(16)         g110<1>D        g112<8,8,1>W                    { align1 1H I@1 };
mov(1)          f0<1>UW         g113<0,1,0>UW                   { align1 WE_all 1N I@3 };
cmp.nz.f0.0(16) null<1>UD       g110<8,8,1>UD   0x00000000UD    { align1 1H I@2 };
mov(16)         g113<1>UD       f0<0,1,0>UW                     { align1 1H };
bfi1(16)        g115<1>UD       g40<8,8,1>D     0D              { align1 1H };
cbit(16)        g59<1>UD        g113<8,8,1>UD                   { align1 1H I@2 };
cmp.z.f0.0(16)  null<1>D        g40<8,8,1>D     0D              { align1 1H };
and(16)         g117<1>UD       g113<1,1,0>UD   g115<1,1,0>UD   { align1 1H I@3 compacted };
cbit(16)        g119<1>UD       g117<8,8,1>UD                   { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL3          UIP:  LABEL3              { align1 1H };
add(8)          g121<1>D        g23<8,4,2>D     36D             { align1 1Q F@2 compacted };
add(8)          g122<1>D        g25<8,4,2>D     36D             { align1 2Q F@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
cmp.l.f0.0(8)   g123<1>UD       g121<8,8,1>UD   g23<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g124<1>UD       g122<8,8,1>UD   g25<8,4,2>UD    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g55<2>UD        g121<4,4,1>UD                   { align1 1Q F@7 };
mov(8)          g57<2>UD        g122<4,4,1>UD                   { align1 2Q $4.src };
add(8)          g125<1>D        -g123<8,8,1>D   g23.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g126<1>D        -g124<8,8,1>D   g25.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g55.1<2>UD      g125<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g57.1<2>UD      g126<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g21UD           g55UD           g59UD           0x0824058c                0x00000080
                            ugm MsgDesc: ( atomic_add, a64, d32, V1, L1UC_L3WB dst_len = 2, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $4 };

LABEL3:
endif(16)       JIP:  LABEL4                                    { align1 1H };

LABEL4:
add(16)         g126<1>D        g119<1,1,0>D    g21<0,1,0>D     { align1 1H @3 $4.dst compacted };
mov.nz.f0.0(16) null<1>D        g110<8,8,1>D                    { align1 1H };
(+f0.0) if(16)  JIP:  LABEL5          UIP:  LABEL5              { align1 1H };
shl(16)         g2<1>D          g126<8,8,1>D    0x00000005UD    { align1 1H A@1 };
shr(16)         g20<1>UD        g126<1,1,0>UD   0x0000001bUD    { align1 1H compacted };
mov(8)          g35<1>UD        g31.1<8,4,2>UD                  { align1 1Q F@6 };
mov(8)          g36<1>UD        g33.1<8,4,2>UD                  { align1 2Q F@5 };
add(8)          g44<1>D         g31<8,4,2>D     g2<1,1,0>D      { align1 1Q I@4 compacted };
add(8)          g45<1>D         g33<8,4,2>D     g3<1,1,0>D      { align1 2Q I@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
shl(16)         g1<1>D          g37<8,8,1>D     0x0000001dUD    { align1 1H };
cmp.l.f0.0(8)   g46<1>UD        g44<8,8,1>UD    g31<8,4,2>UD    { align1 1Q I@3 };
mov(8)          g60<2>UD        g44<4,4,1>UD                    { align1 1Q $4.src };
cmp.l.f0.0(8)   g47<1>UD        g45<8,8,1>UD    g33<8,4,2>UD    { align1 2Q I@4 };
mov(8)          g62<2>UD        g45<4,4,1>UD                    { align1 2Q };
or(16)          g18<1>UD        g42<1,1,0>UD    g1<1,1,0>UD     { align1 1H I@5 compacted };
add3(16)        g48<1>D         g35<8,8,1>D     g20<8,8,1>D     -g46<1,1,1>D { align1 1H I@3 };
mov(8)          g62.1<2>UD      g49<4,4,1>UD                    { align1 2Q I@1 };
mov(8)          g60.1<2>UD      g48<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g60UD           g4UD            0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $4 };
add(16)         g50<1>D         g44<1,1,0>D     16D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
cmp.l.f0.0(16)  g52<1>UD        g50<1,1,0>UD    g44<1,1,0>UD    { align1 1H A@1 compacted };
mov(8)          g61<2>UD        g50<4,4,1>UD                    { align1 1Q $4.src };
mov(8)          g63<2>UD        g51<4,4,1>UD                    { align1 2Q $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add(16)         g54<1>D         -g52<1,1,0>D    g48<1,1,0>D     { align1 1H A@3 compacted };
mov(8)          g63.1<2>UD      g55<4,4,1>UD                    { align1 2Q I@1 };
mov(8)          g61.1<2>UD      g54<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g61UD           g12UD           0x0800f586                0x00000200
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 8 flat )  base_offset 0  { align1 1H $5 };

LABEL5:
endif(16)       JIP:  LABEL6                                    { align1 1H };

LABEL6:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
add(16)         g55<1>F         g4<1,1,0>F      g12<1,1,0>F     { align1 1H A@3 compacted };
cmp.ge.f0.0(16) g61<1>F         g12<1,1,0>F     g4<1,1,0>F      { align1 1H $5.src compacted };
mov(16)         g79<1>F         0x7f800000F      /* infF */     { align1 WE_all 1H compacted };
mov(16)         g79<1>F         g4<1,1,0>F                      { align1 1H compacted };
mov(16)         g81<1>F         0x7f800000F      /* infF */     { align1 WE_all 1H $4.src compacted };
mov(16)         g81<1>F         g6<1,1,0>F                      { align1 1H compacted };
mov(16)         g83<1>F         0x7f800000F      /* infF */     { align1 WE_all 1H $4.src compacted };
mov(16)         g83<1>F         g8<1,1,0>F                      { align1 1H compacted };
mov(16)         g85<1>F         0xff800000F      /* -infF */    { align1 WE_all 1H compacted };
mov(16)         g85<1>F         g12<1,1,0>F                     { align1 1H compacted };
mov(16)         g87<1>F         0xff800000F      /* -infF */    { align1 WE_all 1H $5.src compacted };
mov(16)         g87<1>F         g14<1,1,0>F                     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(16)         g89<1>F         0xff800000F      /* -infF */    { align1 WE_all 1H $5.src compacted };
mov(16)         g89<1>F         g16<1,1,0>F                     { align1 1H compacted };
add(16)         g57<1>F         g6<1,1,0>F      g14<1,1,0>F     { align1 1H $4.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
(+f0.0) sel(16) g63<1>UD        g55<8,8,1>UD    0x7f800000UD    { align1 1H F@7 };
mov(8)          g116<2>UD       g79.1<8,4,2>UD                  { align1 WE_all 1Q F@7 };
mov(8)          g20<2>UD        g81.1<8,4,2>UD                  { align1 WE_all 1Q F@6 };
mov(8)          g44<2>UD        g83.1<8,4,2>UD                  { align1 WE_all 1Q F@5 };
mov(8)          g67<2>UD        g85.1<8,4,2>UD                  { align1 WE_all 1Q F@4 };
mov(8)          g98<2>UD        g87.1<8,4,2>UD                  { align1 WE_all 1Q F@3 };
mov(8)          g111<2>UD       g89.1<8,4,2>UD                  { align1 WE_all 1Q F@2 };
cmp.ge.f0.0(16) g65<1>F         g14<1,1,0>F     g6<1,1,0>F      { align1 1H compacted };
sel.l(8)        g114<2>F        g79<8,4,2>F     g116<8,4,2>F    { align1 WE_all 1Q I@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
sel.l(8)        g1<2>F          g81<8,4,2>F     g20<8,4,2>F     { align1 WE_all 1Q I@5 };
sel.l(8)        g42<2>F         g83<8,4,2>F     g44<8,4,2>F     { align1 WE_all 1Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sel.ge(8)       g59<2>F         g85<8,4,2>F     g67<8,4,2>F     { align1 WE_all 1Q I@3 };
sel.ge(8)       g96<2>F         g87<8,4,2>F     g98<8,4,2>F     { align1 WE_all 1Q I@2 };
sel.ge(8)       g109<2>F        g89<8,4,2>F     g111<8,4,2>F    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@3 };
(+f0.0) sel(16) g67<1>UD        g57<8,8,1>UD    0x7f800000UD    { align1 1H };
mov(8)          g79.1<2>UD      g114<8,4,2>UD                   { align1 WE_all 1Q A@6 };
mov(8)          g81.1<2>UD      g1<8,4,2>UD                     { align1 WE_all 1Q A@5 };
mov(8)          g83.1<2>UD      g42<8,4,2>UD                    { align1 WE_all 1Q A@4 };
mov(8)          g85.1<2>UD      g59<8,4,2>UD                    { align1 WE_all 1Q A@3 };
mov(8)          g87.1<2>UD      g96<8,4,2>UD                    { align1 WE_all 1Q A@2 };
mov(8)          g89.1<2>UD      g109<8,4,2>UD                   { align1 WE_all 1Q A@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
add(16)         g59<1>F         g8<1,1,0>F      g16<1,1,0>F     { align1 1H compacted };
cmp.ge.f0.0(16) g69<1>F         g16<1,1,0>F     g8<1,1,0>F      { align1 1H compacted };
mov(4)          g121<4>UD       g79.2<8,2,4>UD                  { align1 WE_all 1N I@6 };
mov(4)          g119<4>UD       g79.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g33<4>UD        g81.2<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g31<4>UD        g81.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g49<4>UD        g83.2<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g47<4>UD        g83.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g77<4>UD        g85.2<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g75<4>UD        g85.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g103<4>UD       g87.2<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g101<4>UD       g87.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g123<4>UD       g89.2<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g114<4>UD       g89.1<8,2,4>UD                  { align1 WE_all 1N };
(+f0.0) sel(16) g71<1>UD        g59<8,8,1>UD    0x7f800000UD    { align1 1H F@2 };
sel.l(4)        g117<4>F        g119<8,2,4>F    g121<8,2,4>F    { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.dst };
sel.l(4)        g21<4>F         g31<8,2,4>F     g33<8,2,4>F     { align1 WE_all 1N I@7 };
sel.l(4)        g45<4>F         g47<8,2,4>F     g49<8,2,4>F     { align1 WE_all 1N I@7 };
sel.ge(4)       g73<4>F         g75<8,2,4>F     g77<8,2,4>F     { align1 WE_all 1N I@6 };
sel.ge(4)       g99<4>F         g101<8,2,4>F    g103<8,2,4>F    { align1 WE_all 1N I@4 };
sel.ge(4)       g112<4>F        g114<8,2,4>F    g123<8,2,4>F    { align1 WE_all 1N I@2 };
cmp.nz.f0.0(16) null<1>D        g61<8,8,1>D     0D              { align1 1H };
mov(4)          g79.2<4>UD      g117<8,2,4>UD                   { align1 WE_all 1N F@6 };
mov(4)          g81.2<4>UD      g21<8,2,4>UD                    { align1 WE_all 1N F@5 };
mov(4)          g83.2<4>UD      g45<8,2,4>UD                    { align1 WE_all 1N F@4 };
mov(4)          g85.2<4>UD      g73<8,2,4>UD                    { align1 WE_all 1N F@3 };
mov(4)          g87.2<4>UD      g99<8,2,4>UD                    { align1 WE_all 1N F@2 };
mov(4)          g89.2<4>UD      g112<8,2,4>UD                   { align1 WE_all 1N F@1 };
(+f0.0) sel(16) g73<1>UD        g55<8,8,1>UD    0xff800000UD    { align1 1H };
mov(4)          g126<4>UD       g79.3<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g124<4>UD       g79.1<8,2,4>UD                  { align1 WE_all 1N F@1 };
mov(4)          g38<4>UD        g81.3<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g36<4>UD        g81.1<8,2,4>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(4)          g52<4>UD        g83.1<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g95<4>UD        g85.3<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g93<4>UD        g85.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g108<4>UD       g87.3<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g106<4>UD       g87.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g3<4>UD         g89.3<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g1<4>UD         g89.1<8,2,4>UD                  { align1 WE_all 1N };
cmp.nz.f0.0(16) null<1>D        g65<8,8,1>D     0D              { align1 1H };
mov(4)          g54<4>UD        g83.3<8,2,4>UD                  { align1 WE_all 1N $4.src };
sel.l(4)        g122<4>F        g124<8,2,4>F    g126<8,2,4>F    { align1 WE_all 1N };
sel.l(4)        g34<4>F         g36<8,2,4>F     g38<8,2,4>F     { align1 WE_all 1N I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
sel.ge(4)       g91<4>F         g93<8,2,4>F     g95<8,2,4>F     { align1 WE_all 1N I@7 };
sel.ge(4)       g104<4>F        g106<8,2,4>F    g108<8,2,4>F    { align1 WE_all 1N I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@7 };
(+f0.0) sel(16) g75<1>UD        g57<8,8,1>UD    0xff800000UD    { align1 1H };
sel.l(4)        g50<4>F         g52<8,2,4>F     g54<8,2,4>F     { align1 WE_all 1N I@2 };
mov(4)          g79.3<4>UD      g122<8,2,4>UD                   { align1 WE_all 1N F@5 };
mov(4)          g81.3<4>UD      g34<8,2,4>UD                    { align1 WE_all 1N F@4 };
mov(4)          g85.3<4>UD      g91<8,2,4>UD                    { align1 WE_all 1N F@3 };
sel.ge(4)       g124<4>F        g1<8,2,4>F      g3<8,2,4>F      { align1 WE_all 1N I@7 };
mov(4)          g87.3<4>UD      g104<8,2,4>UD                   { align1 WE_all 1N F@3 };
cmp.nz.f0.0(16) null<1>D        g69<8,8,1>D     0D              { align1 1H };
mov(4)          g83.3<4>UD      g50<8,2,4>UD                    { align1 WE_all 1N F@2 };
sel.l(4)        g79.4<1>F       g79.3<0,1,0>F   g79.4<4,4,1>F   { align1 WE_all 1N I@6 };
sel.l(4)        g80.4<1>F       g80.3<0,1,0>F   g80.4<4,4,1>F   { align1 WE_all 1N I@6 };
sel.l(4)        g81.4<1>F       g81.3<0,1,0>F   g81.4<4,4,1>F   { align1 WE_all 1N I@5 };
sel.l(4)        g82.4<1>F       g82.3<0,1,0>F   g82.4<4,4,1>F   { align1 WE_all 1N I@5 };
sel.ge(4)       g85.4<1>F       g85.3<0,1,0>F   g85.4<4,4,1>F   { align1 WE_all 1N I@4 };
sel.ge(4)       g86.4<1>F       g86.3<0,1,0>F   g86.4<4,4,1>F   { align1 WE_all 1N I@4 };
mov(4)          g89.3<4>UD      g124<8,2,4>UD                   { align1 WE_all 1N F@7 };
sel.ge(4)       g87.4<1>F       g87.3<0,1,0>F   g87.4<4,4,1>F   { align1 WE_all 1N I@4 };
sel.ge(4)       g88.4<1>F       g88.3<0,1,0>F   g88.4<4,4,1>F   { align1 WE_all 1N I@4 };
(+f0.0) sel(16) g77<1>UD        g59<8,8,1>UD    0xff800000UD    { align1 1H };
sel.l(4)        g83.4<1>F       g83.3<0,1,0>F   g83.4<4,4,1>F   { align1 WE_all 1N I@3 };
sel.l(4)        g84.4<1>F       g84.3<0,1,0>F   g84.4<4,4,1>F   { align1 WE_all 1N I@3 };
sel.l(8)        g80<1>F         g79.7<0,1,0>F   g80<8,8,1>F     { align1 WE_all 1Q F@7 };
sel.l(8)        g82<1>F         g81.7<0,1,0>F   g82<8,8,1>F     { align1 WE_all 1Q F@7 };
sel.ge(8)       g86<1>F         g85.7<0,1,0>F   g86<8,8,1>F     { align1 WE_all 1Q F@7 };
sel.ge(4)       g89.4<1>F       g89.3<0,1,0>F   g89.4<4,4,1>F   { align1 WE_all 1N I@2 };
sel.ge(4)       g90.4<1>F       g90.3<0,1,0>F   g90.4<4,4,1>F   { align1 WE_all 1N I@2 };
sel.ge(8)       g88<1>F         g87.7<0,1,0>F   g88<8,8,1>F     { align1 WE_all 1Q F@7 };
cmp.l.f0.0(16)  null<1>UD       g40<8,8,1>UD    0x00000003UD    { align1 1H };
sel.l(8)        g84<1>F         g83.7<0,1,0>F   g84<8,8,1>F     { align1 WE_all 1Q F@7 };
sel.ge(8)       g90<1>F         g89.7<0,1,0>F   g90<8,8,1>F     { align1 WE_all 1Q F@3 };
(+f0.0) if(16)  JIP:  LABEL7          UIP:  LABEL7              { align1 1H };
add(8)          g91<1>D         g27<8,4,2>D     128D            { align1 1Q compacted };
add(8)          g92<1>D         g29<8,4,2>D     128D            { align1 2Q compacted };
shl(16)         g96<1>D         g40<8,8,1>D     0x00000002UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
cmp.l.f0.0(8)   g93<1>UD        g91<8,8,1>UD    g27<8,4,2>UD    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@3 };
cmp.l.f0.0(8)   g94<1>UD        g92<8,8,1>UD    g29<8,4,2>UD    { align1 2Q };
add(16)         g99<1>D         g91<1,1,0>D     g96<1,1,0>D     { align1 1H I@3 compacted };
shr(16)         g97<1>UD        g40<1,1,0>UD    0x0000001eUD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
add(8)          g95<1>D         -g93<8,8,1>D    g27.1<8,4,2>D   { align1 1Q };
add(8)          g96<1>D         -g94<8,8,1>D    g29.1<8,4,2>D   { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
cmp.l.f0.0(16)  g101<1>UD       g99<1,1,0>UD    g91<1,1,0>UD    { align1 1H compacted };
cmp.l.f0.0(16)  g105<1>UD       g40<1,1,0>UD    0x00000002UD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
add3(16)        g103<1>D        g95<8,8,1>D     g97<8,8,1>D     -g101<1,1,1>D { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@2 };
(+f0.0) sel(16) g107<1>UD       g82.7<0,1,0>UD  g84.7<0,1,0>UD  { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@2 };
cmp.l.f0.0(16)  g109<1>UD       g40<1,1,0>UD    0x00000001UD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@7 };
mov(8)          g81<2>UD        g99<4,4,1>UD                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N F@2 };
mov(8)          g83<2>UD        g100<4,4,1>UD                   { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@4 };
(+f0.0) sel(16) g91<1>UD        g80.7<0,1,0>UD  g107<1,1,0>UD   { align1 1H compacted };
mov(8)          g81.1<2>UD      g103<4,4,1>UD                   { align1 1Q I@3 };
mov(8)          g83.1<2>UD      g104<4,4,1>UD                   { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g81UD           g91UD           0x08040595                0x00000080
                            ugm MsgDesc: ( atomic_fmin, a64, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $6 };
add(16)         g111<1>D        g99<1,1,0>D     12D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
cmp.l.f0.0(16)  g113<1>UD       g111<1,1,0>UD   g99<1,1,0>UD    { align1 1H compacted };
cmp.nz.f0.0(16) null<1>D        g105<8,8,1>D    0D              { align1 1H };
mov(8)          g92<2>UD        g111<4,4,1>UD                   { align1 1Q $6.src };
mov(8)          g94<2>UD        g112<4,4,1>UD                   { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
add(16)         g115<1>D        -g113<1,1,0>D   g103<1,1,0>D    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
(+f0.0) sel(16) g117<1>UD       g88.7<0,1,0>UD  g90.7<0,1,0>UD  { align1 1H };
mov(8)          g94.1<2>UD      g116<4,4,1>UD                   { align1 2Q I@2 };
mov(8)          g92.1<2>UD      g115<4,4,1>UD                   { align1 1Q I@3 };
cmp.nz.f0.0(16) null<1>D        g109<8,8,1>D    0D              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@4 };
(+f0.0) sel(16) g96<1>UD        g86.7<0,1,0>UD  g117<1,1,0>UD   { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g92UD           g96UD           0x08040596                0x00000080
                            ugm MsgDesc: ( atomic_fmax, a64, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $7 };

LABEL7:
endif(16)       JIP:  LABEL8                                    { align1 1H };

LABEL8:
mov(16)         g118<1>F        0x7f800000F      /* infF */     { align1 WE_all 1H I@2 compacted };
mov(16)         g118<1>F        g63<1,1,0>F                     { align1 1H compacted };
mov(16)         g120<1>F        0x7f800000F      /* infF */     { align1 WE_all 1H compacted };
mov(16)         g120<1>F        g67<1,1,0>F                     { align1 1H compacted };
mov(16)         g122<1>F        0x7f800000F      /* infF */     { align1 WE_all 1H compacted };
mov(16)         g122<1>F        g71<1,1,0>F                     { align1 1H compacted };
mov(16)         g124<1>F        0xff800000F      /* -infF */    { align1 WE_all 1H I@5 compacted };
mov(16)         g124<1>F        g73<1,1,0>F                     { align1 1H compacted };
mov(16)         g126<1>F        0xff800000F      /* -infF */    { align1 WE_all 1H compacted };
mov(16)         g126<1>F        g75<1,1,0>F                     { align1 1H compacted };
mov(16)         g1<1>F          0xff800000F      /* -infF */    { align1 WE_all 1H I@4 compacted };
mov(16)         g1<1>F          g77<1,1,0>F                     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
cmp.l.f0.0(16)  null<1>UD       g40<8,8,1>UD    0x00000003UD    { align1 1H };
mov(8)          g6<2>UD         g118.1<8,4,2>UD                 { align1 WE_all 1Q F@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(8)          g19<2>UD        g120.1<8,4,2>UD                 { align1 WE_all 1Q F@5 };
mov(8)          g37<2>UD        g122.1<8,4,2>UD                 { align1 WE_all 1Q F@4 };
mov(8)          g52<2>UD        g124.1<8,4,2>UD                 { align1 WE_all 1Q F@3 };
mov(8)          g65<2>UD        g126.1<8,4,2>UD                 { align1 WE_all 1Q F@2 };
sel.l(8)        g4<2>F          g118<8,4,2>F    g6<8,4,2>F      { align1 WE_all 1Q I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
sel.l(8)        g17<2>F         g120<8,4,2>F    g19<8,4,2>F     { align1 WE_all 1Q I@4 };
sel.l(8)        g35<2>F         g122<8,4,2>F    g37<8,4,2>F     { align1 WE_all 1Q I@3 };
mov(8)          g78<2>UD        g1.1<8,4,2>UD                   { align1 WE_all 1Q F@4 };
sel.ge(8)       g50<2>F         g124<8,4,2>F    g52<8,4,2>F     { align1 WE_all 1Q I@3 };
sel.ge(8)       g63<2>F         g126<8,4,2>F    g65<8,4,2>F     { align1 WE_all 1Q I@2 };
mov(8)          g118.1<2>UD     g4<8,4,2>UD                     { align1 WE_all 1Q A@5 };
mov(8)          g120.1<2>UD     g17<8,4,2>UD                    { align1 WE_all 1Q A@4 };
mov(8)          g122.1<2>UD     g35<8,4,2>UD                    { align1 WE_all 1Q A@3 };
sel.ge(8)       g76<2>F         g1<8,4,2>F      g78<8,4,2>F     { align1 WE_all 1Q I@4 };
mov(8)          g124.1<2>UD     g50<8,4,2>UD                    { align1 WE_all 1Q A@3 };
mov(8)          g126.1<2>UD     g63<8,4,2>UD                    { align1 WE_all 1Q A@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(4)          g11<4>UD        g118.2<8,2,4>UD                 { align1 WE_all 1N I@5 };
mov(4)          g9<4>UD         g118.1<8,2,4>UD                 { align1 WE_all 1N $4.src };
mov(4)          g29<4>UD        g120.2<8,2,4>UD                 { align1 WE_all 1N I@6 };
mov(4)          g27<4>UD        g120.1<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g44<4>UD        g122.2<8,2,4>UD                 { align1 WE_all 1N I@7 };
mov(4)          g42<4>UD        g122.1<8,2,4>UD                 { align1 WE_all 1N };
mov(8)          g1.1<2>UD       g76<8,4,2>UD                    { align1 WE_all 1Q F@1 };
mov(4)          g57<4>UD        g124.2<8,2,4>UD                 { align1 WE_all 1N I@7 };
mov(4)          g55<4>UD        g124.1<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g70<4>UD        g126.2<8,2,4>UD                 { align1 WE_all 1N A@7 };
mov(4)          g68<4>UD        g126.1<8,2,4>UD                 { align1 WE_all 1N };
sel.l(4)        g7<4>F          g9<8,2,4>F      g11<8,2,4>F     { align1 WE_all 1N I@7 };
sel.l(4)        g20<4>F         g27<8,2,4>F     g29<8,2,4>F     { align1 WE_all 1N I@7 };
sel.l(4)        g38<4>F         g42<8,2,4>F     g44<8,2,4>F     { align1 WE_all 1N I@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
mov(4)          g83<4>UD        g1.2<8,2,4>UD                   { align1 WE_all 1N I@5 };
mov(4)          g81<4>UD        g1.1<8,2,4>UD                   { align1 WE_all 1N $6.src };
sel.ge(4)       g53<4>F         g55<8,2,4>F     g57<8,2,4>F     { align1 WE_all 1N I@5 };
sel.ge(4)       g66<4>F         g68<8,2,4>F     g70<8,2,4>F     { align1 WE_all 1N I@3 };
mov(4)          g118.2<4>UD     g7<8,2,4>UD                     { align1 WE_all 1N F@5 };
mov(4)          g120.2<4>UD     g20<8,2,4>UD                    { align1 WE_all 1N F@4 };
mov(4)          g122.2<4>UD     g38<8,2,4>UD                    { align1 WE_all 1N F@3 };
sel.ge(4)       g79<4>F         g81<8,2,4>F     g83<8,2,4>F     { align1 WE_all 1N I@4 };
mov(4)          g124.2<4>UD     g53<8,2,4>UD                    { align1 WE_all 1N F@3 };
mov(4)          g126.2<4>UD     g66<8,2,4>UD                    { align1 WE_all 1N F@2 };
mov(4)          g16<4>UD        g118.3<8,2,4>UD                 { align1 WE_all 1N I@5 };
mov(4)          g14<4>UD        g118.1<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g34<4>UD        g120.3<8,2,4>UD                 { align1 WE_all 1N I@6 };
mov(4)          g32<4>UD        g120.1<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g49<4>UD        g122.3<8,2,4>UD                 { align1 WE_all 1N I@7 };
mov(4)          g47<4>UD        g122.1<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g1.2<4>UD       g79<8,2,4>UD                    { align1 WE_all 1N F@1 };
mov(4)          g62<4>UD        g124.3<8,2,4>UD                 { align1 WE_all 1N I@7 };
mov(4)          g60<4>UD        g124.1<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g75<4>UD        g126.3<8,2,4>UD                 { align1 WE_all 1N I@7 };
mov(4)          g73<4>UD        g126.1<8,2,4>UD                 { align1 WE_all 1N };
sel.l(4)        g12<4>F         g14<8,2,4>F     g16<8,2,4>F     { align1 WE_all 1N I@7 };
sel.l(4)        g30<4>F         g32<8,2,4>F     g34<8,2,4>F     { align1 WE_all 1N I@7 };
sel.l(4)        g45<4>F         g47<8,2,4>F     g49<8,2,4>F     { align1 WE_all 1N I@6 };
mov(4)          g88<4>UD        g1.3<8,2,4>UD                   { align1 WE_all 1N I@5 };
mov(4)          g86<4>UD        g1.1<8,2,4>UD                   { align1 WE_all 1N };
sel.ge(4)       g58<4>F         g60<8,2,4>F     g62<8,2,4>F     { align1 WE_all 1N I@5 };
sel.ge(4)       g71<4>F         g73<8,2,4>F     g75<8,2,4>F     { align1 WE_all 1N I@3 };
mov(4)          g118.3<4>UD     g12<8,2,4>UD                    { align1 WE_all 1N F@5 };
mov(4)          g120.3<4>UD     g30<8,2,4>UD                    { align1 WE_all 1N F@4 };
mov(4)          g122.3<4>UD     g45<8,2,4>UD                    { align1 WE_all 1N F@3 };
sel.ge(4)       g84<4>F         g86<8,2,4>F     g88<8,2,4>F     { align1 WE_all 1N I@4 };
mov(4)          g124.3<4>UD     g58<8,2,4>UD                    { align1 WE_all 1N F@3 };
mov(4)          g126.3<4>UD     g71<8,2,4>UD                    { align1 WE_all 1N F@2 };
sel.l(4)        g118.4<1>F      g118.3<0,1,0>F  g118.4<4,4,1>F  { align1 WE_all 1N I@5 };
sel.l(4)        g119.4<1>F      g119.3<0,1,0>F  g119.4<4,4,1>F  { align1 WE_all 1N I@5 };
sel.l(4)        g120.4<1>F      g120.3<0,1,0>F  g120.4<4,4,1>F  { align1 WE_all 1N I@4 };
sel.l(4)        g121.4<1>F      g121.3<0,1,0>F  g121.4<4,4,1>F  { align1 WE_all 1N I@4 };
sel.l(4)        g122.4<1>F      g122.3<0,1,0>F  g122.4<4,4,1>F  { align1 WE_all 1N I@3 };
sel.l(4)        g123.4<1>F      g123.3<0,1,0>F  g123.4<4,4,1>F  { align1 WE_all 1N I@3 };
mov(4)          g1.3<4>UD       g84<8,2,4>UD                    { align1 WE_all 1N F@7 };
sel.ge(4)       g124.4<1>F      g124.3<0,1,0>F  g124.4<4,4,1>F  { align1 WE_all 1N I@3 };
sel.ge(4)       g125.4<1>F      g125.3<0,1,0>F  g125.4<4,4,1>F  { align1 WE_all 1N I@3 };
sel.ge(4)       g126.4<1>F      g126.3<0,1,0>F  g126.4<4,4,1>F  { align1 WE_all 1N I@2 };
sel.ge(4)       g127.4<1>F      g127.3<0,1,0>F  g127.4<4,4,1>F  { align1 WE_all 1N I@2 };
sel.l(8)        g119<1>F        g118.7<0,1,0>F  g119<8,8,1>F    { align1 WE_all 1Q F@7 };
sel.l(8)        g121<1>F        g120.7<0,1,0>F  g121<8,8,1>F    { align1 WE_all 1Q F@7 };
sel.l(8)        g123<1>F        g122.7<0,1,0>F  g123<8,8,1>F    { align1 WE_all 1Q F@7 };
sel.ge(4)       g1.4<1>F        g1.3<0,1,0>F    g1.4<4,4,1>F    { align1 WE_all 1N I@1 };
sel.ge(4)       g2.4<1>F        g2.3<0,1,0>F    g2.4<4,4,1>F    { align1 WE_all 1N I@1 };
sel.ge(8)       g125<1>F        g124.7<0,1,0>F  g125<8,8,1>F    { align1 WE_all 1Q F@7 };
sel.ge(8)       g127<1>F        g126.7<0,1,0>F  g127<8,8,1>F    { align1 WE_all 1Q F@7 };
sel.ge(8)       g2<1>F          g1.7<0,1,0>F    g2<8,8,1>F      { align1 WE_all 1Q F@3 };
(+f0.0) if(16)  JIP:  LABEL9          UIP:  LABEL9              { align1 1H };
shl(16)         g3<1>D          g40<8,8,1>D     0x00000002UD    { align1 1H };
mov(8)          g6<1>UD         g23.1<8,4,2>UD                  { align1 1Q };
mov(8)          g7<1>UD         g25.1<8,4,2>UD                  { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
add(8)          g8<1>D          g23<8,4,2>D     g3<1,1,0>D      { align1 1Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@4 };
add(8)          g9<1>D          g25<8,4,2>D     g4<1,1,0>D      { align1 2Q compacted };
shr(16)         g4<1>UD         g40<1,1,0>UD    0x0000001eUD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
cmp.l.f0.0(8)   g10<1>UD        g8<8,8,1>UD     g23<8,4,2>UD    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
mov(8)          g97<2>UD        g8<4,4,1>UD                     { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@4 };
cmp.l.f0.0(8)   g11<1>UD        g9<8,8,1>UD     g25<8,4,2>UD    { align1 2Q };
mov(8)          g99<2>UD        g9<4,4,1>UD                     { align1 2Q };
cmp.l.f0.0(16)  g14<1>UD        g40<1,1,0>UD    0x00000002UD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
add3(16)        g12<1>D         g6<8,8,1>D      g4<8,8,1>D      -g10<1,1,1>D { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@6 };
(+f0.0) sel(16) g16<1>UD        g121.7<0,1,0>UD g123.7<0,1,0>UD { align1 1H };
mov(8)          g99.1<2>UD      g13<4,4,1>UD                    { align1 2Q I@2 };
mov(8)          g97.1<2>UD      g12<4,4,1>UD                    { align1 1Q I@3 };
cmp.l.f0.0(16)  g18<1>UD        g40<1,1,0>UD    0x00000001UD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@4 };
(+f0.0) sel(16) g101<1>UD       g119.7<0,1,0>UD g16<1,1,0>UD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g97UD           g101UD          0x08040595                0x00000080
                            ugm MsgDesc: ( atomic_fmin, a64, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $7 };
add(16)         g20<1>D         g8<1,1,0>D      16D             { align1 1H compacted };
cmp.l.f0.0(16)  g22<1>UD        g20<1,1,0>UD    g8<1,1,0>UD     { align1 1H I@1 compacted };
cmp.nz.f0.0(16) null<1>D        g14<8,8,1>D     0D              { align1 1H I@7 };
mov(8)          g102<2>UD       g20<4,4,1>UD                    { align1 1Q $7.src };
mov(8)          g104<2>UD       g21<4,4,1>UD                    { align1 2Q };
add(16)         g24<1>D         -g22<1,1,0>D    g12<1,1,0>D     { align1 1H I@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
(+f0.0) sel(16) g26<1>UD        g127.7<0,1,0>UD g2.7<0,1,0>UD   { align1 1H };
mov(8)          g104.1<2>UD     g25<4,4,1>UD                    { align1 2Q I@2 };
mov(8)          g102.1<2>UD     g24<4,4,1>UD                    { align1 1Q I@3 };
cmp.nz.f0.0(16) null<1>D        g18<8,8,1>D     0D              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@3 };
(+f0.0) sel(16) g106<1>UD       g125.7<0,1,0>UD g26<1,1,0>UD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g102UD          g106UD          0x08040596                0x00000080
                            ugm MsgDesc: ( atomic_fmax, a64, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $8 };

LABEL9:
endif(16)       JIP:  LABEL10                                   { align1 1H };

LABEL10:
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q F@2 };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_build_primref_procedurals_to_primrefs_code[] = {
    0x80000065, 0x59058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x1b050220, 0x00000024, 0x00000000,
    0xe25a1a40, 0x00015903, 0xe25b0040, 0x04015903,
    0x80030061, 0x0d054410, 0x00000000, 0x76543210,
    0x00041c69, 0x26058660, 0x02461b05, 0x00000004,
    0x80001c01, 0x00000000, 0x00000000, 0x00000000,
    0x80004031, 0x01140000, 0xfa005a0c, 0x00340000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x80004131, 0x030c0000, 0xfa005b0c, 0x00300000,
    0x640d1a40, 0x00800d95, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00040061, 0x28050160,
    0x00460d05, 0x00000000, 0xa02a1940, 0x28002602,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x34260aa0, 0x000002e4, 0x00000000,
    0x00130061, 0x36260aa0, 0x000002e4, 0x00000000,
    0x00030061, 0x1f260aa0, 0x000002a4, 0x00000000,
    0x00130061, 0x21260aa0, 0x000002a4, 0x00000000,
    0x00031c61, 0x1b260aa0, 0x00000264, 0x00000000,
    0x00130061, 0x1d260aa0, 0x00000264, 0x00000000,
    0x21170061, 0x001102cc, 0x2a190061, 0x001102cc,
    0x80002101, 0x00000000, 0x00000000, 0x00000000,
    0x00041970, 0x00010220, 0x52462a05, 0x00000324,
    0x60230061, 0x00110300, 0x00040065, 0x0a058220,
    0x02000304, 0x00ffffff, 0x21341761, 0x0011026c,
    0x2a361761, 0x0011026c, 0x211f1761, 0x0011025c,
    0x2a211761, 0x0011025c, 0x211b1761, 0x00110244,
    0x2a1d1761, 0x00110244, 0x21171761, 0x00110204,
    0x2a191761, 0x00110204, 0x00041761, 0x25050020,
    0x0066231f, 0x00000000, 0x01040022, 0x0001c060,
    0x00000370, 0x00000300, 0x80001701, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x2c440000,
    0xfb043424, 0x003c0000, 0x00032241, 0x20010220,
    0x01462a05, 0x00563006, 0x00042241, 0x36050660,
    0x01462a05, 0x00563006, 0x00040041, 0x6b050660,
    0x01462a05, 0x00563016, 0x00042241, 0x38050660,
    0x01462a05, 0x00563206, 0x3e343249, 0x30002a03,
    0x00041b40, 0x36160110, 0x01563616, 0x00566b06,
    0x00040041, 0x6c050660, 0x01462a05, 0x00563216,
    0x00130041, 0x20010220, 0x01462b05, 0x00563106,
    0xa03cb240, 0x36002c02, 0x00041b40, 0x38160110,
    0x01563816, 0x00566c06, 0x00133249, 0x35050222,
    0x02462b05, 0x00463105, 0x273e1b70, 0x2c003c03,
    0xa0490040, 0x01003c03, 0xa03a1b40, 0x38003402,
    0x274b1a70, 0x3c004903, 0x0004a252, 0x40040e68,
    0x0e2e2e05, 0x3e053a05, 0x00030061, 0x35060220,
    0x00343c05, 0x00000000, 0x00130061, 0x37060220,
    0x00343d05, 0x00000000, 0xa04d1b40, 0x40024b02,
    0x00031b61, 0x35260220, 0x00344005, 0x00000000,
    0x00131b61, 0x37260220, 0x00344105, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x41440000, 0xfb043524, 0x003c0000,
    0x00033361, 0x36060220, 0x00344905, 0x00000000,
    0x00133361, 0x38060220, 0x00344a05, 0x00000000,
    0x00031a61, 0x36260220, 0x00344d05, 0x00000000,
    0x00131a61, 0x38260220, 0x00344e05, 0x00000000,
    0xe74f2370, 0x7f834300, 0xe7512370, 0x7f834100,
    0xe7552370, 0x7f834500, 0x20531265, 0x51004f03,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x4e240000, 0xfb043624, 0x000c0000,
    0x20570965, 0x55005303, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0xe7592470, 0x7f834e00,
    0x27650062, 0x4e004300, 0x25690062, 0x4e004300,
    0x256b2462, 0x50004500, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0xe75b2370, 0x7f834700,
    0xe75f0070, 0x7f835000, 0x205d1265, 0x5b005903,
    0x20610965, 0x5f005d03, 0x2e631965, 0x61005703,
    0x00041961, 0x6d062650, 0x00466305, 0x00000000,
    0x01041562, 0x06058220, 0x02466505, 0x7f800000,
    0x01041362, 0x10058220, 0x02466b05, 0xff800000,
    0x01041462, 0x0e058220, 0x02466905, 0xff800000,
    0x27641b62, 0x47004100, 0x27661b62, 0x50004500,
    0x00041c61, 0x14050110, 0x00566d06, 0x00000000,
    0x01041262, 0x04058220, 0x02466405, 0x7f800000,
    0x01041162, 0x08058220, 0x02466605, 0x7f800000,
    0x25671962, 0x47004100, 0x01041162, 0x0c058220,
    0x02466705, 0xff800000, 0x00040024, 0x0001c060,
    0x00000080, 0x00000080, 0x00041d61, 0x14054110,
    0x00000000, 0x00000000, 0x00041c61, 0x08054220,
    0x00000000, 0x7f800000, 0x00041f61, 0x06054220,
    0x00000000, 0x7f800000, 0x00041f61, 0x04054220,
    0x00000000, 0x7f800000, 0x00040061, 0x10054220,
    0x00000000, 0xff800000, 0x00040061, 0x0e054220,
    0x00000000, 0xff800000, 0x00041f61, 0x0c054220,
    0x00000000, 0xff800000, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x606c1e65, 0x00101405,
    0x00041961, 0x6d050450, 0x00686c06, 0x00000000,
    0x80000061, 0x71064210, 0x00000000, 0x00000000,
    0x00041a70, 0x70058550, 0x25586d05, 0x00000000,
    0x00041961, 0x6e050560, 0x00467005, 0x00000000,
    0x80001b61, 0x30010110, 0x00007104, 0x00000000,
    0x00041a70, 0x00018220, 0x22466e05, 0x00000000,
    0x00040061, 0x71050120, 0x00003000, 0x00000000,
    0x00040079, 0x73058620, 0x06462805, 0x00000000,
    0x00041a4d, 0x3b050220, 0x00467105, 0x00000000,
    0x00040070, 0x00018660, 0x16462805, 0x00000000,
    0x20751b65, 0x73007103, 0x0004194d, 0x77050220,
    0x00467505, 0x00000000, 0x01040022, 0x0001c060,
    0x000000e0, 0x000000e0, 0xa1791240, 0x024e1703,
    0xaa7a1140, 0x024e1903, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x00031a70, 0x7b050220,
    0x52467905, 0x00441706, 0x00131a70, 0x7c050220,
    0x52467a05, 0x00441906, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x00031761, 0x37060220,
    0x00347905, 0x00000000, 0x00133461, 0x39060220,
    0x00347a05, 0x00000000, 0x00031c40, 0x7d052660,
    0x06467b05, 0x00441726, 0x00131c40, 0x7e052660,
    0x06467c05, 0x00441926, 0x00031a61, 0x37260220,
    0x00347d05, 0x00000000, 0x00131a61, 0x39260220,
    0x00347e05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044431, 0x15140000,
    0xfb183724, 0x01003b14, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0xa07eb440, 0x15107702,
    0x00040061, 0x00010660, 0x20466e05, 0x00000000,
    0x01040022, 0x0001c060, 0x000001a8, 0x000001a8,
    0x00040969, 0x02058660, 0x02467e05, 0x00000005,
    0xe0140068, 0x01b07e03, 0x00031661, 0x23050220,
    0x00441f26, 0x00000000, 0x00131561, 0x24050220,
    0x00442126, 0x00000000, 0xa12c1c40, 0x020e1f02,
    0xaa2d1d40, 0x030e2102, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x00040069, 0x01058660,
    0x02462505, 0x0000001d, 0x00031b70, 0x2e050220,
    0x52462c05, 0x00441f06, 0x00033461, 0x3c060220,
    0x00342c05, 0x00000000, 0x00131c70, 0x2f050220,
    0x52462d05, 0x00442106, 0x00130061, 0x3e060220,
    0x00342d05, 0x00000000, 0x20121d66, 0x01002a03,
    0x00041b52, 0x30040e68, 0x0e2e2305, 0x2e051405,
    0x00131961, 0x3e260220, 0x00343105, 0x00000000,
    0x00031a61, 0x3c260220, 0x00343005, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x00000000, 0xfb0c3c24, 0x003c0444,
    0xa0320040, 0x01002c03, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x27340970, 0x2c003203,
    0x00033461, 0x3d060220, 0x00343205, 0x00000000,
    0x00133461, 0x3f060220, 0x00343305, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0xa0360b40, 0x30023402, 0x00131961, 0x3f260220,
    0x00343705, 0x00000000, 0x00031a61, 0x3d260220,
    0x00343605, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044531, 0x00000000,
    0xfb0c3d24, 0x003c0c44, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x20370b40, 0x0c000400,
    0x253d3570, 0x04000c00, 0xa34f0061, 0x7f810000,
    0x604f0061, 0x00100400, 0xa3513461, 0x7f810000,
    0x60510061, 0x00100600, 0xa3533461, 0x7f810000,
    0x60530061, 0x00100800, 0xa3550061, 0xff810000,
    0x60550061, 0x00100c00, 0xa3573561, 0xff810000,
    0x60570061, 0x00100e00, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0xa3593561, 0xff810000,
    0x60590061, 0x00101000, 0x20393440, 0x0e000600,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x01041762, 0x3f058220, 0x02463705, 0x7f800000,
    0x80031761, 0x74060220, 0x00444f26, 0x00000000,
    0x80031661, 0x14060220, 0x00445126, 0x00000000,
    0x80031561, 0x2c060220, 0x00445326, 0x00000000,
    0x80031461, 0x43060220, 0x00445526, 0x00000000,
    0x80031361, 0x62060220, 0x00445726, 0x00000000,
    0x80031261, 0x6f060220, 0x00445926, 0x00000000,
    0x25410070, 0x06000e00, 0x80031e62, 0x72060aa0,
    0x5a444f06, 0x00447406, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x80031d62, 0x01060aa0,
    0x5a445106, 0x00441406, 0x80031c62, 0x2a060aa0,
    0x5a445306, 0x00442c06, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80031b62, 0x3b060aa0,
    0x4a445506, 0x00444306, 0x80031a62, 0x60060aa0,
    0x4a445706, 0x00446206, 0x80031962, 0x6d060aa0,
    0x4a445906, 0x00446f06, 0x80001301, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x43058220,
    0x02463905, 0x7f800000, 0x80030e61, 0x4f260220,
    0x00447206, 0x00000000, 0x80030d61, 0x51260220,
    0x00440106, 0x00000000, 0x80030c61, 0x53260220,
    0x00442a06, 0x00000000, 0x80030b61, 0x55260220,
    0x00443b06, 0x00000000, 0x80030a61, 0x57260220,
    0x00446006, 0x00000000, 0x80030961, 0x59260220,
    0x00446d06, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x203b0040, 0x10000800,
    0x25450070, 0x08001000, 0x80021e61, 0x79070220,
    0x00424f47, 0x00000000, 0x80020061, 0x77070220,
    0x00424f27, 0x00000000, 0x80021f61, 0x21070220,
    0x00425147, 0x00000000, 0x80020061, 0x1f070220,
    0x00425127, 0x00000000, 0x80021f61, 0x31070220,
    0x00425347, 0x00000000, 0x80020061, 0x2f070220,
    0x00425327, 0x00000000, 0x80021f61, 0x4d070220,
    0x00425547, 0x00000000, 0x80020061, 0x4b070220,
    0x00425527, 0x00000000, 0x80021f61, 0x67070220,
    0x00425747, 0x00000000, 0x80020061, 0x65070220,
    0x00425727, 0x00000000, 0x80020061, 0x7b070220,
    0x00425947, 0x00000000, 0x80020061, 0x72070220,
    0x00425927, 0x00000000, 0x01041262, 0x47058220,
    0x02463b05, 0x7f800000, 0x80020062, 0x75070aa0,
    0x5a427707, 0x00427907, 0x80002401, 0x00000000,
    0x00000000, 0x00000000, 0x80021f62, 0x15070aa0,
    0x5a421f07, 0x00422107, 0x80021f62, 0x2d070aa0,
    0x5a422f07, 0x00423107, 0x80021e62, 0x49070aa0,
    0x4a424b07, 0x00424d07, 0x80021c62, 0x63070aa0,
    0x4a426507, 0x00426707, 0x80021a62, 0x70070aa0,
    0x4a427207, 0x00427b07, 0x00040070, 0x00018660,
    0x26463d05, 0x00000000, 0x80021661, 0x4f470220,
    0x00427507, 0x00000000, 0x80021561, 0x51470220,
    0x00421507, 0x00000000, 0x80021461, 0x53470220,
    0x00422d07, 0x00000000, 0x80021361, 0x55470220,
    0x00424907, 0x00000000, 0x80021261, 0x57470220,
    0x00426307, 0x00000000, 0x80021161, 0x59470220,
    0x00427007, 0x00000000, 0x01040062, 0x49058220,
    0x02463705, 0xff800000, 0x80021f61, 0x7e070220,
    0x00424f67, 0x00000000, 0x80021161, 0x7c070220,
    0x00424f27, 0x00000000, 0x80021f61, 0x26070220,
    0x00425167, 0x00000000, 0x80020061, 0x24070220,
    0x00425127, 0x00000000, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x80021f61, 0x34070220,
    0x00425327, 0x00000000, 0x80021f61, 0x5f070220,
    0x00425567, 0x00000000, 0x80020061, 0x5d070220,
    0x00425527, 0x00000000, 0x80021f61, 0x6c070220,
    0x00425767, 0x00000000, 0x80020061, 0x6a070220,
    0x00425727, 0x00000000, 0x80020061, 0x03070220,
    0x00425967, 0x00000000, 0x80020061, 0x01070220,
    0x00425927, 0x00000000, 0x00040070, 0x00018660,
    0x26464105, 0x00000000, 0x80023461, 0x36070220,
    0x00425367, 0x00000000, 0x80020062, 0x7a070aa0,
    0x5a427c07, 0x00427e07, 0x80021f62, 0x22070aa0,
    0x5a422407, 0x00422607, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x80021f62, 0x5b070aa0,
    0x4a425d07, 0x00425f07, 0x80021d62, 0x68070aa0,
    0x4a426a07, 0x00426c07, 0x80001701, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x4b058220,
    0x02463905, 0xff800000, 0x80021a62, 0x32070aa0,
    0x5a423407, 0x00423607, 0x80021561, 0x4f670220,
    0x00427a07, 0x00000000, 0x80021461, 0x51670220,
    0x00422207, 0x00000000, 0x80021361, 0x55670220,
    0x00425b07, 0x00000000, 0x80021f62, 0x7c070aa0,
    0x4a420107, 0x00420307, 0x80021361, 0x57670220,
    0x00426807, 0x00000000, 0x00040070, 0x00018660,
    0x26464505, 0x00000000, 0x80021261, 0x53670220,
    0x00423207, 0x00000000, 0x80021e62, 0x4f850aa0,
    0x5a004f64, 0x00344f85, 0x80021e62, 0x50850aa0,
    0x5a005064, 0x00345085, 0x80021d62, 0x51850aa0,
    0x5a005164, 0x00345185, 0x80021d62, 0x52850aa0,
    0x5a005264, 0x00345285, 0x80021c62, 0x55850aa0,
    0x4a005564, 0x00345585, 0x80021c62, 0x56850aa0,
    0x4a005664, 0x00345685, 0x80021761, 0x59670220,
    0x00427c07, 0x00000000, 0x80021c62, 0x57850aa0,
    0x4a005764, 0x00345785, 0x80021c62, 0x58850aa0,
    0x4a005864, 0x00345885, 0x01040062, 0x4d058220,
    0x02463b05, 0xff800000, 0x80021b62, 0x53850aa0,
    0x5a005364, 0x00345385, 0x80021b62, 0x54850aa0,
    0x5a005464, 0x00345485, 0x80031762, 0x50050aa0,
    0x5a004fe4, 0x00465005, 0x80031762, 0x52050aa0,
    0x5a0051e4, 0x00465205, 0x80031762, 0x56050aa0,
    0x4a0055e4, 0x00465605, 0x80021a62, 0x59850aa0,
    0x4a005964, 0x00345985, 0x80021a62, 0x5a850aa0,
    0x4a005a64, 0x00345a85, 0x80031762, 0x58050aa0,
    0x4a0057e4, 0x00465805, 0x00040070, 0x00018220,
    0x52462805, 0x00000003, 0x80031762, 0x54050aa0,
    0x5a0053e4, 0x00465405, 0x80031362, 0x5a050aa0,
    0x4a0059e4, 0x00465a05, 0x01040022, 0x0001c060,
    0x000002b0, 0x000002b0, 0xa15b0040, 0x080e1b03,
    0xaa5c0040, 0x080e1d03, 0x00040069, 0x60058660,
    0x02462805, 0x00000002, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x00030070, 0x5d050220,
    0x52465b05, 0x00441b06, 0x80101b01, 0x00000000,
    0x00000000, 0x00000000, 0x00130070, 0x5e050220,
    0x52465c05, 0x00441d06, 0xa0631b40, 0x60005b02,
    0xe0610068, 0x01e02803, 0x80001c01, 0x00000000,
    0x00000000, 0x00000000, 0x00030040, 0x5f052660,
    0x06465d05, 0x00441b26, 0x00131c40, 0x60052660,
    0x06465e05, 0x00441d26, 0x80001c01, 0x00000000,
    0x00000000, 0x00000000, 0x27650070, 0x5b006303,
    0xe7690070, 0x00202803, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x00040052, 0x67040e68,
    0x0e2e5f05, 0x65056105, 0x80001201, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x6b050220,
    0x020052e4, 0x000054e4, 0x80001201, 0x00000000,
    0x00000000, 0x00000000, 0xe76d0070, 0x00102803,
    0x80001701, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x51060220, 0x00346305, 0x00000000,
    0x80101201, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x53060220, 0x00346405, 0x00000000,
    0x80000c01, 0x00000000, 0x00000000, 0x00000000,
    0x2f5b0062, 0x6b015083, 0x00031b61, 0x51260220,
    0x00346705, 0x00000000, 0x00131b61, 0x53260220,
    0x00346805, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044631, 0x00000000,
    0xfb2a5124, 0x01005b14, 0xa06f0040, 0x00c06303,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x27710070, 0x63006f03, 0x00040070, 0x00018660,
    0x26466905, 0x00000000, 0x00033661, 0x5c060220,
    0x00346f05, 0x00000000, 0x00130061, 0x5e060220,
    0x00347005, 0x00000000, 0x80001c01, 0x00000000,
    0x00000000, 0x00000000, 0xa0730040, 0x67027102,
    0x80001101, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x75050220, 0x020058e4, 0x00005ae4,
    0x00131a61, 0x5e260220, 0x00347405, 0x00000000,
    0x00031b61, 0x5c260220, 0x00347305, 0x00000000,
    0x00040070, 0x00018660, 0x26466d05, 0x00000000,
    0x80000c01, 0x00000000, 0x00000000, 0x00000000,
    0x2f600062, 0x75015683, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044731, 0x00000000,
    0xfb2c5c24, 0x01006014, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0xa3761a61, 0x7f810000,
    0x60760061, 0x00103f00, 0xa3780061, 0x7f810000,
    0x60780061, 0x00104300, 0xa37a0061, 0x7f810000,
    0x607a0061, 0x00104700, 0xa37c1d61, 0xff810000,
    0x607c0061, 0x00104900, 0xa37e0061, 0xff810000,
    0x607e0061, 0x00104b00, 0xa3011c61, 0xff810000,
    0x60010061, 0x00104d00, 0x80003601, 0x00000000,
    0x00000000, 0x00000000, 0x00040070, 0x00018220,
    0x52462805, 0x00000003, 0x80031661, 0x06060220,
    0x00447626, 0x00000000, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x80031561, 0x13060220,
    0x00447826, 0x00000000, 0x80031461, 0x25060220,
    0x00447a26, 0x00000000, 0x80031361, 0x34060220,
    0x00447c26, 0x00000000, 0x80031261, 0x41060220,
    0x00447e26, 0x00000000, 0x80031d62, 0x04060aa0,
    0x5a447606, 0x00440606, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x80031c62, 0x11060aa0,
    0x5a447806, 0x00441306, 0x80031b62, 0x23060aa0,
    0x5a447a06, 0x00442506, 0x80031461, 0x4e060220,
    0x00440126, 0x00000000, 0x80031b62, 0x32060aa0,
    0x4a447c06, 0x00443406, 0x80031a62, 0x3f060aa0,
    0x4a447e06, 0x00444106, 0x80030d61, 0x76260220,
    0x00440406, 0x00000000, 0x80030c61, 0x78260220,
    0x00441106, 0x00000000, 0x80030b61, 0x7a260220,
    0x00442306, 0x00000000, 0x80031c62, 0x4c060aa0,
    0x4a440106, 0x00444e06, 0x80030b61, 0x7c260220,
    0x00443206, 0x00000000, 0x80030a61, 0x7e260220,
    0x00443f06, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80021d61, 0x0b070220,
    0x00427647, 0x00000000, 0x80023461, 0x09070220,
    0x00427627, 0x00000000, 0x80021e61, 0x1d070220,
    0x00427847, 0x00000000, 0x80020061, 0x1b070220,
    0x00427827, 0x00000000, 0x80021f61, 0x2c070220,
    0x00427a47, 0x00000000, 0x80020061, 0x2a070220,
    0x00427a27, 0x00000000, 0x80031161, 0x01260220,
    0x00444c06, 0x00000000, 0x80021f61, 0x39070220,
    0x00427c47, 0x00000000, 0x80020061, 0x37070220,
    0x00427c27, 0x00000000, 0x80020f61, 0x46070220,
    0x00427e47, 0x00000000, 0x80020061, 0x44070220,
    0x00427e27, 0x00000000, 0x80021f62, 0x07070aa0,
    0x5a420907, 0x00420b07, 0x80021f62, 0x14070aa0,
    0x5a421b07, 0x00421d07, 0x80021e62, 0x26070aa0,
    0x5a422a07, 0x00422c07, 0x80003601, 0x00000000,
    0x00000000, 0x00000000, 0x80021d61, 0x53070220,
    0x00420147, 0x00000000, 0x80023661, 0x51070220,
    0x00420127, 0x00000000, 0x80021d62, 0x35070aa0,
    0x4a423707, 0x00423907, 0x80021b62, 0x42070aa0,
    0x4a424407, 0x00424607, 0x80021561, 0x76470220,
    0x00420707, 0x00000000, 0x80021461, 0x78470220,
    0x00421407, 0x00000000, 0x80021361, 0x7a470220,
    0x00422607, 0x00000000, 0x80021c62, 0x4f070aa0,
    0x4a425107, 0x00425307, 0x80021361, 0x7c470220,
    0x00423507, 0x00000000, 0x80021261, 0x7e470220,
    0x00424207, 0x00000000, 0x80021d61, 0x10070220,
    0x00427667, 0x00000000, 0x80020061, 0x0e070220,
    0x00427627, 0x00000000, 0x80021e61, 0x22070220,
    0x00427867, 0x00000000, 0x80020061, 0x20070220,
    0x00427827, 0x00000000, 0x80021f61, 0x31070220,
    0x00427a67, 0x00000000, 0x80020061, 0x2f070220,
    0x00427a27, 0x00000000, 0x80021161, 0x01470220,
    0x00424f07, 0x00000000, 0x80021f61, 0x3e070220,
    0x00427c67, 0x00000000, 0x80020061, 0x3c070220,
    0x00427c27, 0x00000000, 0x80021f61, 0x4b070220,
    0x00427e67, 0x00000000, 0x80020061, 0x49070220,
    0x00427e27, 0x00000000, 0x80021f62, 0x0c070aa0,
    0x5a420e07, 0x00421007, 0x80021f62, 0x1e070aa0,
    0x5a422007, 0x00422207, 0x80021e62, 0x2d070aa0,
    0x5a422f07, 0x00423107, 0x80021d61, 0x58070220,
    0x00420167, 0x00000000, 0x80020061, 0x56070220,
    0x00420127, 0x00000000, 0x80021d62, 0x3a070aa0,
    0x4a423c07, 0x00423e07, 0x80021b62, 0x47070aa0,
    0x4a424907, 0x00424b07, 0x80021561, 0x76670220,
    0x00420c07, 0x00000000, 0x80021461, 0x78670220,
    0x00421e07, 0x00000000, 0x80021361, 0x7a670220,
    0x00422d07, 0x00000000, 0x80021c62, 0x54070aa0,
    0x4a425607, 0x00425807, 0x80021361, 0x7c670220,
    0x00423a07, 0x00000000, 0x80021261, 0x7e670220,
    0x00424707, 0x00000000, 0x80021d62, 0x76850aa0,
    0x5a007664, 0x00347685, 0x80021d62, 0x77850aa0,
    0x5a007764, 0x00347785, 0x80021c62, 0x78850aa0,
    0x5a007864, 0x00347885, 0x80021c62, 0x79850aa0,
    0x5a007964, 0x00347985, 0x80021b62, 0x7a850aa0,
    0x5a007a64, 0x00347a85, 0x80021b62, 0x7b850aa0,
    0x5a007b64, 0x00347b85, 0x80021761, 0x01670220,
    0x00425407, 0x00000000, 0x80021b62, 0x7c850aa0,
    0x4a007c64, 0x00347c85, 0x80021b62, 0x7d850aa0,
    0x4a007d64, 0x00347d85, 0x80021a62, 0x7e850aa0,
    0x4a007e64, 0x00347e85, 0x80021a62, 0x7f850aa0,
    0x4a007f64, 0x00347f85, 0x80031762, 0x77050aa0,
    0x5a0076e4, 0x00467705, 0x80031762, 0x79050aa0,
    0x5a0078e4, 0x00467905, 0x80031762, 0x7b050aa0,
    0x5a007ae4, 0x00467b05, 0x80021962, 0x01850aa0,
    0x4a000164, 0x00340185, 0x80021962, 0x02850aa0,
    0x4a000264, 0x00340285, 0x80031762, 0x7d050aa0,
    0x4a007ce4, 0x00467d05, 0x80031762, 0x7f050aa0,
    0x4a007ee4, 0x00467f05, 0x80031362, 0x02050aa0,
    0x4a0001e4, 0x00460205, 0x01040022, 0x0001c060,
    0x00000270, 0x00000270, 0x00040069, 0x03058660,
    0x02462805, 0x00000002, 0x00030061, 0x06050220,
    0x00441726, 0x00000000, 0x00130061, 0x07050220,
    0x00441926, 0x00000000, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0xa1080040, 0x030e1702,
    0x80101c01, 0x00000000, 0x00000000, 0x00000000,
    0xaa090040, 0x040e1902, 0xe0040068, 0x01e02803,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00030070, 0x0a050220, 0x52460805, 0x00441706,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x61060220, 0x00340805, 0x00000000,
    0x80101c01, 0x00000000, 0x00000000, 0x00000000,
    0x00130070, 0x0b050220, 0x52460905, 0x00441906,
    0x00130061, 0x63060220, 0x00340905, 0x00000000,
    0xe70e0070, 0x00202803, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x00040052, 0x0c040e68,
    0x0e2e0605, 0x0a050405, 0x80001601, 0x00000000,
    0x00000000, 0x00000000, 0x01040062, 0x10050220,
    0x020079e4, 0x00007be4, 0x00131a61, 0x63260220,
    0x00340d05, 0x00000000, 0x00031b61, 0x61260220,
    0x00340c05, 0x00000000, 0xe7120070, 0x00102803,
    0x80000c01, 0x00000000, 0x00000000, 0x00000000,
    0x2f650062, 0x10017783, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044731, 0x00000000,
    0xfb2a6124, 0x01006514, 0xa0140040, 0x01000803,
    0x27161970, 0x08001403, 0x00041f70, 0x00018660,
    0x26460e05, 0x00000000, 0x00033761, 0x66060220,
    0x00341405, 0x00000000, 0x00130061, 0x68060220,
    0x00341505, 0x00000000, 0xa0181c40, 0x0c021602,
    0x80001101, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x1a050220, 0x02007fe4, 0x000002e4,
    0x00131a61, 0x68260220, 0x00341905, 0x00000000,
    0x00031b61, 0x66260220, 0x00341805, 0x00000000,
    0x00040070, 0x00018660, 0x26461205, 0x00000000,
    0x80000b01, 0x00000000, 0x00000000, 0x00000000,
    0x2f6a0062, 0x1a017d83, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044831, 0x00000000,
    0xfb2c6624, 0x01006a14, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x80031261, 0x7e050220,
    0x00460005, 0x00000000, 0x80040931, 0x00000004,
    0x30007e0c, 0x00000000, 0x20000060, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_build_primref_procedurals_to_primrefs = {
   .prog_data = {
      .base.nr_params = 18,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 3,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 7040,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_build_primref_procedurals_to_primrefs_relocs,
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
      .push.cross_thread.dwords = 18,
      .push.cross_thread.regs = 3,
      .push.cross_thread.size = 96,
   },
   .args_size = 40,
   .arg_count = 6,
   .args = gfx125_bvh_build_primref_procedurals_to_primrefs_args,
   .code = gfx125_bvh_build_primref_procedurals_to_primrefs_code,
};
const char *gfx125_bvh_build_primref_procedurals_to_primrefs_sha1 = "4e3ea1b155dc886ec515a285040197a14c5eb2b7";
