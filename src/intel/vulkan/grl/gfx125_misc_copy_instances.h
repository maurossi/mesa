#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_misc_copy_instances_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_misc_copy_instances_args[] = {
   { 0, 8 },
   { 8, 8 },
   { 16, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g57<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g107<1>UD       g0.1<0,1,0>UD                   { align1 1H };
add(1)          g58<1>UD        g57<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@2 compacted };
mov(16)         g15<1>UD        g107<8,8,1>UD                   { align1 1H I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(1)         g1UD            g58UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g3.1<2>F        g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g17.1<2>F       g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g5.1<2>F        g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g19.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g55.1<2>F       g2.5<0,1,0>F                    { align1 1Q };
mov(8)          g21.1<2>F       g2.5<0,1,0>F                    { align1 2Q };
mov(8)          g3<2>F          g2<0,1,0>F                      { align1 1Q F@6 compacted };
mov(8)          g17<2>F         g2<0,1,0>F                      { align1 2Q F@6 compacted };
mov(8)          g5<2>F          g2.2<0,1,0>F                    { align1 1Q F@6 compacted };
mov(8)          g19<2>F         g2.2<0,1,0>F                    { align1 2Q F@6 compacted };
mov(8)          g55<2>F         g2.4<0,1,0>F                    { align1 1Q F@6 compacted };
mov(8)          g21<2>F         g2.4<0,1,0>F                    { align1 2Q F@6 compacted };
shr(8)          g23<1>UD        g55<8,4,2>UD    0x00000008UD    { align1 1Q F@2 compacted };
and(8)          g25<1>UD        g55<8,4,2>UD    0xffffff00UD    { align1 1Q };
shr(8)          g24<1>UD        g21<8,4,2>UD    0x00000008UD    { align1 2Q F@1 compacted };
and(8)          g26<1>UD        g21<8,4,2>UD    0xffffff00UD    { align1 2Q };
add(8)          g27<1>D         g55<8,4,2>D     -g25<1,1,0>D    { align1 1Q I@3 compacted };
add3(16)        g33<1>D         65534W          -g23<8,8,1>D    g107<1,1,1>D { align1 1H I@3 };
add(8)          g28<1>D         g21<8,4,2>D     -g26<1,1,0>D    { align1 2Q I@3 compacted };
mov(16)         g35<1>D         -g33<8,8,1>D                    { align1 1H I@2 };
and(16)         g29<1>UD        g27<8,8,1>UD    0xffffffc0UD    { align1 1H I@2 };
shr(16)         g31<1>UD        g29<1,1,0>UD    0x00000006UD    { align1 1H I@1 compacted };
cmp.l.f0.0(16)  null<1>UD       g35<8,8,1>UD    g31<8,8,1>UD    { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL0          UIP:  LABEL0              { align1 1H };
shl(16)         g36<1>D         -g33<8,8,1>D    0x00000006UD    { align1 1H };
or.z.f0.0(8)    null<1>UD       g5<8,4,2>UD     g5.1<8,4,2>UD   { align1 1Q F@4 };
or.z.f0.0(8)    null<1>UD       g19<8,4,2>UD    g19.1<8,4,2>UD  { align1 2Q F@3 };
add(16)         g38<1>D         g25<1,1,0>D     g36<1,1,0>D     { align1 1H I@3 compacted };
(+f0.0) if(16)  JIP:  LABEL2          UIP:  LABEL1              { align1 1H };
shr(16)         g40<1>UD        g38<1,1,0>UD    0x00000006UD    { align1 1H I@2 compacted };
shl(16)         g42<1>D         g40<8,8,1>D     0x00000003UD    { align1 1H I@1 };
mov(8)          g113<2>UD       g42<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g115<2>UD       g43<4,4,1>UD                    { align1 2Q I@2 };
mov(8)          g113.1<2>UD     0x00000000UD                    { align1 1Q I@2 };
mov(8)          g115.1<2>UD     0x00000000UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g43UD           g113UD          nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
mov(8)          g7<2>UD         g43<4,4,1>UD                    { align1 1Q $1.dst };
mov(8)          g9<2>UD         g44<4,4,1>UD                    { align1 2Q $1.dst };
mov(8)          g7.1<2>UD       g45<4,4,1>UD                    { align1 1Q @2 $1.dst };
mov(8)          g9.1<2>UD       g46<4,4,1>UD                    { align1 2Q @2 $1.dst };
else(16)        JIP:  LABEL1          UIP:  LABEL1              { align1 1H };

LABEL2:
add(8)          g56<1>D         g5<8,4,2>D      g38<1,1,0>D     { align1 1Q I@2 compacted };
add(8)          g44<1>D         g19<8,4,2>D     g39<1,1,0>D     { align1 2Q I@3 compacted };
cmp.l.f0.0(8)   g57<1>UD        g56<8,8,1>UD    g5<8,4,2>UD     { align1 1Q I@2 };
cmp.l.f0.0(8)   g45<1>UD        g44<8,8,1>UD    g19<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g7<2>UD         g56<4,4,1>UD                    { align1 1Q I@7 };
mov(8)          g9<2>UD         g44<4,4,1>UD                    { align1 2Q I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(8)          g58<1>D         -g57<8,8,1>D    g5.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g46<1>D         -g45<8,8,1>D    g19.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g7.1<2>UD       g58<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g9.1<2>UD       g46<4,4,1>UD                    { align1 2Q I@2 };

LABEL1:
endif(16)       JIP:  LABEL0                                    { align1 1H };
add(8)          g59<1>D         g3<8,4,2>D      g38<1,1,0>D     { align1 1Q F@6 compacted };
add(8)          g47<1>D         g17<8,4,2>D     g39<1,1,0>D     { align1 2Q F@5 compacted };
cmp.l.f0.0(8)   g60<1>UD        g59<8,8,1>UD    g3<8,4,2>UD     { align1 1Q I@2 };
cmp.l.f0.0(8)   g48<1>UD        g47<8,8,1>UD    g17<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g50<2>UD        g59<4,4,1>UD                    { align1 1Q };
mov(8)          g52<2>UD        g47<4,4,1>UD                    { align1 2Q };
add(8)          g61<1>D         -g60<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g49<1>D         -g48<8,8,1>D    g17.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g50.1<2>UD      g61<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g52.1<2>UD      g49<4,4,1>UD                    { align1 2Q I@2 };
fbl(1)          g108<1>UD       mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g108<0,1,0>UD   0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g118<2>D        g[a0 224]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g118.1<2>D      g[a0 228]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(8)          g114.1<2>UD     g118.1<0,1,0>UD                 { align1 WE_all 1Q I@1 };
mov(8)          g114<2>UD       g118<0,1,0>UD                   { align1 WE_all 1Q I@1 };
mov(1)          g54<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g119UD   g114UD          nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $2 };
mov(1)          f0<1>UD         g54<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g108<0,1,0>UD   0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000600UD    { align1 WE_all 1N A@1 };
mov(1)          g121<2>D        g[a0 64]<0,1,0>D                { align1 WE_all 1N A@1 };
mov(1)          g121.1<2>D      g[a0 68]<0,1,0>D                { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(8)          g115.1<2>UD     g121.1<0,1,0>UD                 { align1 WE_all 1Q I@2 };
mov(8)          g115<2>UD       g121<0,1,0>UD                   { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g115UD          g119UD          0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $0 };

LABEL0:
endif(16)       JIP:  LABEL3                                    { align1 1H };

LABEL3:
shr(16)         g51<1>UD        g25<1,1,0>UD    0x00000008UD    { align1 1H compacted };
or(8)           g53<1>UD        g5<8,4,2>UD     g5.1<8,4,2>UD   { align1 1Q F@4 };
or(8)           g54<1>UD        g19<8,4,2>UD    g19.1<8,4,2>UD  { align1 2Q F@3 };

LABEL13:
cmp.ge.f0.0(16) null<1>UD       g15<8,8,1>UD    g51<8,8,1>UD    { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL4        UIP:  LABEL4              { align1 1H };
shl(16)         g55<1>D         g15<8,8,1>D     0x00000008UD    { align1 1H };
cmp.z.f0.0(16)  null<1>D        g53<8,8,1>D     0D              { align1 1H I@4 };
(+f0.0) if(16)  JIP:  LABEL6          UIP:  LABEL5              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
shr(16)         g57<1>UD        g55<1,1,0>UD    0x00000006UD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
shl(16)         g59<1>D         g57<8,8,1>D     0x00000003UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(8)          g120<2>UD       g59<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@2 };
mov(8)          g122<2>UD       g60<4,4,1>UD                    { align1 2Q };
mov(8)          g120.1<2>UD     0x00000000UD                    { align1 1Q I@2 };
mov(8)          g122.1<2>UD     0x00000000UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g60UD           g120UD          nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $0 };
mov(8)          g8<2>UD         g60<4,4,1>UD                    { align1 1Q $0.dst };
mov(8)          g10<2>UD        g61<4,4,1>UD                    { align1 2Q $0.dst };
mov(8)          g8.1<2>UD       g62<4,4,1>UD                    { align1 1Q @2 $0.dst };
mov(8)          g10.1<2>UD      g63<4,4,1>UD                    { align1 2Q @2 $0.dst };
else(16)        JIP:  LABEL5          UIP:  LABEL5              { align1 1H };

LABEL6:
add(8)          g63<1>D         g5<8,4,2>D      g55<1,1,0>D     { align1 1Q I@3 compacted };
add(8)          g61<1>D         g19<8,4,2>D     g56<1,1,0>D     { align1 2Q I@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
cmp.l.f0.0(8)   g64<1>UD        g63<8,8,1>UD    g5<8,4,2>UD     { align1 1Q I@2 };
cmp.l.f0.0(8)   g62<1>UD        g61<8,8,1>UD    g19<8,4,2>UD    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
mov(8)          g8<2>UD         g63<4,4,1>UD                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@7 };
mov(8)          g10<2>UD        g61<4,4,1>UD                    { align1 2Q };
add(8)          g65<1>D         -g64<8,8,1>D    g5.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g63<1>D         -g62<8,8,1>D    g19.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g8.1<2>UD       g65<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g10.1<2>UD      g63<4,4,1>UD                    { align1 2Q I@2 };

LABEL5:
endif(16)       JIP:  LABEL4                                    { align1 1H };
add(8)          g66<1>D         g3<8,4,2>D      g55<1,1,0>D     { align1 1Q F@6 compacted };
add(8)          g64<1>D         g17<8,4,2>D     g56<1,1,0>D     { align1 2Q F@5 compacted };
cmp.l.f0.0(8)   g71<1>UD        g66<8,8,1>UD    g3<8,4,2>UD     { align1 1Q I@2 };
cmp.l.f0.0(8)   g65<1>UD        g64<8,8,1>UD    g17<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g67<2>UD        g66<4,4,1>UD                    { align1 1Q };
mov(8)          g69<2>UD        g64<4,4,1>UD                    { align1 2Q };
add(8)          g72<1>D         -g71<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g66<1>D         -g65<8,8,1>D    g17.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g67.1<2>UD      g72<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g69.1<2>UD      g66<4,4,1>UD                    { align1 2Q I@2 };
fbl(1)          g109<1>UD       mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
shl(1)          a0<1>UD         g109<0,1,0>UD   0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g25<2>D         g[a0 256]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g25.1<2>D       g[a0 260]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(8)          g121.1<2>UD     g25.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g121<2>UD       g25<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g50<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
(+f0.0.any16h) send(1) g1UD     g121UD          nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
mov(1)          f0<1>UD         g50<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g109<0,1,0>UD   0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000800UD    { align1 WE_all 1N A@1 };
mov(1)          g26<2>D         g[a0 96]<0,1,0>D                { align1 WE_all 1N A@1 };
mov(1)          g26.1<2>D       g[a0 100]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(8)          g122.1<2>UD     g26.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g122<2>UD       g26<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g122UD          g1UD            0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $0 };
add(16)         g68<1>D         g55<1,1,0>D     64D             { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g53<8,8,1>D     0D              { align1 1H };
(+f0.0) if(16)  JIP:  LABEL8          UIP:  LABEL7              { align1 1H };
shr(16)         g70<1>UD        g68<1,1,0>UD    0x00000006UD    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
shl(16)         g72<1>D         g70<8,8,1>D     0x00000003UD    { align1 1H I@1 };
mov(8)          g25<2>UD        g72<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@2 };
mov(8)          g27<2>UD        g73<4,4,1>UD                    { align1 2Q };
mov(8)          g25.1<2>UD      0x00000000UD                    { align1 1Q I@2 };
mov(8)          g27.1<2>UD      0x00000000UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g73UD           g25UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
mov(8)          g9<2>UD         g73<4,4,1>UD                    { align1 1Q $3.dst };
mov(8)          g11<2>UD        g74<4,4,1>UD                    { align1 2Q $3.dst };
mov(8)          g9.1<2>UD       g75<4,4,1>UD                    { align1 1Q @2 $3.dst };
mov(8)          g11.1<2>UD      g76<4,4,1>UD                    { align1 2Q @2 $3.dst };
else(16)        JIP:  LABEL7          UIP:  LABEL7              { align1 1H };

LABEL8:
add(8)          g73<1>D         g5<8,4,2>D      g68<1,1,0>D     { align1 1Q I@3 compacted };
add(8)          g74<1>D         g19<8,4,2>D     g69<1,1,0>D     { align1 2Q I@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
cmp.l.f0.0(8)   g76<1>UD        g73<8,8,1>UD    g5<8,4,2>UD     { align1 1Q I@2 };
cmp.l.f0.0(8)   g75<1>UD        g74<8,8,1>UD    g19<8,4,2>UD    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
mov(8)          g9<2>UD         g73<4,4,1>UD                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@7 };
mov(8)          g11<2>UD        g74<4,4,1>UD                    { align1 2Q };
add(8)          g77<1>D         -g76<8,8,1>D    g5.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g76<1>D         -g75<8,8,1>D    g19.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g9.1<2>UD       g77<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g11.1<2>UD      g76<4,4,1>UD                    { align1 2Q I@2 };

LABEL7:
endif(16)       JIP:  LABEL4                                    { align1 1H };
add(8)          g79<1>D         g3<8,4,2>D      g68<1,1,0>D     { align1 1Q compacted };
add(8)          g77<1>D         g17<8,4,2>D     g69<1,1,0>D     { align1 2Q compacted };
cmp.l.f0.0(8)   g84<1>UD        g79<8,8,1>UD    g3<8,4,2>UD     { align1 1Q I@2 };
cmp.l.f0.0(8)   g78<1>UD        g77<8,8,1>UD    g17<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g80<2>UD        g79<4,4,1>UD                    { align1 1Q };
mov(8)          g82<2>UD        g77<4,4,1>UD                    { align1 2Q };
add(8)          g85<1>D         -g84<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g79<1>D         -g78<8,8,1>D    g17.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g80.1<2>UD      g85<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g82.1<2>UD      g79<4,4,1>UD                    { align1 2Q I@2 };
fbl(1)          g110<1>UD       mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
shl(1)          a0<1>UD         g110<0,1,0>UD   0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g30<2>D         g[a0 288]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g30.1<2>D       g[a0 292]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(8)          g26.1<2>UD      g30.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g26<2>UD        g30<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g49<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g31UD    g26UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $4 };
mov(1)          f0<1>UD         g49<0,1,0>UD                    { align1 WE_all 1N I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
shl(1)          a0<1>UD         g110<0,1,0>UD   0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000a00UD    { align1 WE_all 1N A@1 };
mov(1)          g33<2>D         g[a0]<0,1,0>D                   { align1 WE_all 1N A@1 };
mov(1)          g33.1<2>D       g[a0 4]<0,1,0>D                 { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(8)          g27.1<2>UD      g33.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g27<2>UD        g33<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g27UD           g31UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $5 };
add(16)         g81<1>D         g55<1,1,0>D     128D            { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g53<8,8,1>D     0D              { align1 1H };
(+f0.0) if(16)  JIP:  LABEL10         UIP:  LABEL9              { align1 1H };
shr(16)         g83<1>UD        g81<1,1,0>UD    0x00000006UD    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
shl(16)         g85<1>D         g83<8,8,1>D     0x00000003UD    { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(8)          g32<2>UD        g85<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@2 };
mov(8)          g34<2>UD        g86<4,4,1>UD                    { align1 2Q };
mov(8)          g32.1<2>UD      0x00000000UD                    { align1 1Q I@2 };
mov(8)          g34.1<2>UD      0x00000000UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g86UD           g32UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $5 };
mov(8)          g10<2>UD        g86<4,4,1>UD                    { align1 1Q $5.dst };
mov(8)          g12<2>UD        g87<4,4,1>UD                    { align1 2Q $5.dst };
mov(8)          g10.1<2>UD      g88<4,4,1>UD                    { align1 1Q @2 $5.dst };
mov(8)          g12.1<2>UD      g89<4,4,1>UD                    { align1 2Q @2 $5.dst };
else(16)        JIP:  LABEL9          UIP:  LABEL9              { align1 1H };

LABEL10:
add(8)          g86<1>D         g5<8,4,2>D      g81<1,1,0>D     { align1 1Q I@3 compacted };
add(8)          g87<1>D         g19<8,4,2>D     g82<1,1,0>D     { align1 2Q I@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
cmp.l.f0.0(8)   g89<1>UD        g86<8,8,1>UD    g5<8,4,2>UD     { align1 1Q I@2 };
cmp.l.f0.0(8)   g88<1>UD        g87<8,8,1>UD    g19<8,4,2>UD    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
mov(8)          g10<2>UD        g86<4,4,1>UD                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@7 };
mov(8)          g12<2>UD        g87<4,4,1>UD                    { align1 2Q };
add(8)          g90<1>D         -g89<8,8,1>D    g5.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g89<1>D         -g88<8,8,1>D    g19.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g10.1<2>UD      g90<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g12.1<2>UD      g89<4,4,1>UD                    { align1 2Q I@2 };

LABEL9:
endif(16)       JIP:  LABEL4                                    { align1 1H };
add(8)          g92<1>D         g3<8,4,2>D      g81<1,1,0>D     { align1 1Q compacted };
add(8)          g90<1>D         g17<8,4,2>D     g82<1,1,0>D     { align1 2Q compacted };
cmp.l.f0.0(8)   g97<1>UD        g92<8,8,1>UD    g3<8,4,2>UD     { align1 1Q I@2 };
cmp.l.f0.0(8)   g91<1>UD        g90<8,8,1>UD    g17<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g93<2>UD        g92<4,4,1>UD                    { align1 1Q };
mov(8)          g95<2>UD        g90<4,4,1>UD                    { align1 2Q };
add(8)          g98<1>D         -g97<8,8,1>D    g3.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g92<1>D         -g91<8,8,1>D    g17.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g93.1<2>UD      g98<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g95.1<2>UD      g92<4,4,1>UD                    { align1 2Q I@2 };
fbl(1)          g111<1>UD       mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
shl(1)          a0<1>UD         g111<0,1,0>UD   0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g37<2>D         g[a0 320]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g37.1<2>D       g[a0 324]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(8)          g33.1<2>UD      g37.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g33<2>UD        g37<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g48<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g38UD    g33UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $6 };
mov(1)          f0<1>UD         g48<0,1,0>UD                    { align1 WE_all 1N I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
shl(1)          a0<1>UD         g111<0,1,0>UD   0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000a00UD    { align1 WE_all 1N A@1 };
mov(1)          g40<2>D         g[a0 416]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g40.1<2>D       g[a0 420]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
mov(8)          g34.1<2>UD      g40.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g34<2>UD        g40<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g34UD           g38UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $7 };
add(16)         g94<1>D         g55<1,1,0>D     192D            { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g53<8,8,1>D     0D              { align1 1H };
(+f0.0) if(16)  JIP:  LABEL12         UIP:  LABEL11             { align1 1H };
shr(16)         g96<1>UD        g94<1,1,0>UD    0x00000006UD    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
shl(16)         g98<1>D         g96<8,8,1>D     0x00000003UD    { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
mov(8)          g39<2>UD        g98<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@2 };
mov(8)          g41<2>UD        g99<4,4,1>UD                    { align1 2Q };
mov(8)          g39.1<2>UD      0x00000000UD                    { align1 1Q I@2 };
mov(8)          g41.1<2>UD      0x00000000UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g99UD           g39UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $7 };
mov(8)          g11<2>UD        g99<4,4,1>UD                    { align1 1Q $7.dst };
mov(8)          g13<2>UD        g100<4,4,1>UD                   { align1 2Q $7.dst };
mov(8)          g11.1<2>UD      g101<4,4,1>UD                   { align1 1Q @2 $7.dst };
mov(8)          g13.1<2>UD      g102<4,4,1>UD                   { align1 2Q @2 $7.dst };
else(16)        JIP:  LABEL11         UIP:  LABEL11             { align1 1H };

LABEL12:
add(8)          g99<1>D         g5<8,4,2>D      g94<1,1,0>D     { align1 1Q I@3 compacted };
add(8)          g100<1>D        g19<8,4,2>D     g95<1,1,0>D     { align1 2Q I@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
cmp.l.f0.0(8)   g102<1>UD       g99<8,8,1>UD    g5<8,4,2>UD     { align1 1Q I@2 };
cmp.l.f0.0(8)   g101<1>UD       g100<8,8,1>UD   g19<8,4,2>UD    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
mov(8)          g11<2>UD        g99<4,4,1>UD                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@7 };
mov(8)          g13<2>UD        g100<4,4,1>UD                   { align1 2Q };
add(8)          g103<1>D        -g102<8,8,1>D   g5.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g102<1>D        -g101<8,8,1>D   g19.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g11.1<2>UD      g103<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g13.1<2>UD      g102<4,4,1>UD                   { align1 2Q I@2 };

LABEL11:
endif(16)       JIP:  LABEL4                                    { align1 1H };
add(8)          g105<1>D        g3<8,4,2>D      g94<1,1,0>D     { align1 1Q compacted };
add(8)          g103<1>D        g17<8,4,2>D     g95<1,1,0>D     { align1 2Q compacted };
cmp.l.f0.0(8)   g110<1>UD       g105<8,8,1>UD   g3<8,4,2>UD     { align1 1Q I@2 };
cmp.l.f0.0(8)   g104<1>UD       g103<8,8,1>UD   g17<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g106<2>UD       g105<4,4,1>UD                   { align1 1Q };
mov(8)          g108<2>UD       g103<4,4,1>UD                   { align1 2Q };
add(8)          g111<1>D        -g110<8,8,1>D   g3.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g105<1>D        -g104<8,8,1>D   g17.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g106.1<2>UD     g111<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g108.1<2>UD     g105<4,4,1>UD                   { align1 2Q I@2 };
fbl(1)          g112<1>UD       mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
shl(1)          a0<1>UD         g112<0,1,0>UD   0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g55<2>D         g[a0 352]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g55.1<2>D       g[a0 356]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
mov(8)          g40.1<2>UD      g55.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g40<2>UD        g55<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g47<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g45UD    g40UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $8 };
mov(1)          f0<1>UD         g47<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g112<0,1,0>UD   0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000c00UD    { align1 WE_all 1N A@1 };
mov(1)          g56<2>D         g[a0 320]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g56.1<2>D       g[a0 324]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
mov(8)          g41.1<2>UD      g56.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g41<2>UD        g56<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g41UD           g45UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $9 };
add3(16)        g15<1>D         0x0003UW        g23<8,8,1>D     g15<1,1,1>D { align1 1H };

LABEL4:
while(16)       JIP:  LABEL13                                   { align1 1H };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_misc_copy_instances_code[] = {
    0x80000065, 0x39058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x6b050220, 0x00000024, 0x00000000,
    0xe23a1a40, 0x00013903, 0x00041a61, 0x0f050220,
    0x00466b05, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa003a0c, 0x00340000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x21030061, 0x001102cc,
    0x2a110061, 0x001102cc, 0x00030061, 0x05260aa0,
    0x00000264, 0x00000000, 0x00130061, 0x13260aa0,
    0x00000264, 0x00000000, 0x00030061, 0x37260aa0,
    0x000002a4, 0x00000000, 0x00130061, 0x15260aa0,
    0x000002a4, 0x00000000, 0x21031661, 0x00110204,
    0x2a111661, 0x00110204, 0x21051661, 0x00110244,
    0x2a131661, 0x00110244, 0x21371661, 0x0011025c,
    0x2a151661, 0x0011025c, 0xe1171268, 0x008e3703,
    0x00030065, 0x19058220, 0x02443706, 0xffffff00,
    0xea181168, 0x008e1503, 0x00130065, 0x1a058220,
    0x02441506, 0xffffff00, 0xa11b1b40, 0x192e3702,
    0x00041b52, 0x21044560, 0x0e8efffe, 0x6b051705,
    0xaa1c1b40, 0x1a2e1502, 0x00041a61, 0x23052660,
    0x00462105, 0x00000000, 0x00041a65, 0x1d058220,
    0x02461b05, 0xffffffc0, 0xe01f1968, 0x00601d03,
    0x00041970, 0x00010220, 0x52462305, 0x00461f05,
    0x01040022, 0x0001c060, 0x000003d0, 0x000003d0,
    0x00040069, 0x2405a660, 0x02462105, 0x00000006,
    0x00031466, 0x00010220, 0x12440506, 0x00440526,
    0x00131366, 0x00010220, 0x12441306, 0x00441326,
    0xa0261b40, 0x24001902, 0x01040022, 0x0001c060,
    0x00000178, 0x000000d8, 0xe0281a68, 0x00602603,
    0x00041969, 0x2a058660, 0x02462805, 0x00000003,
    0x00031961, 0x71060220, 0x00342a05, 0x00000000,
    0x00131a61, 0x73060220, 0x00342b05, 0x00000000,
    0x00031a61, 0x71264220, 0x00000000, 0x00000000,
    0x00131a61, 0x73264220, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x2b240000, 0xfb047124, 0x000c0000,
    0x00032161, 0x07060220, 0x00342b05, 0x00000000,
    0x00132161, 0x09060220, 0x00342c05, 0x00000000,
    0x0003a161, 0x07260220, 0x00342d05, 0x00000000,
    0x0013a161, 0x09260220, 0x00342e05, 0x00000000,
    0x00040024, 0x0001c060, 0x000000b0, 0x000000b0,
    0xa1381a40, 0x260e0502, 0xaa2c1b40, 0x270e1302,
    0x00031a70, 0x39050220, 0x52463805, 0x00440506,
    0x00131a70, 0x2d050220, 0x52462c05, 0x00441306,
    0x00031f61, 0x07060220, 0x00343805, 0x00000000,
    0x00131f61, 0x09060220, 0x00342c05, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00031c40, 0x3a052660, 0x06463905, 0x00440526,
    0x00131c40, 0x2e052660, 0x06462d05, 0x00441326,
    0x00031a61, 0x07260220, 0x00343a05, 0x00000000,
    0x00131a61, 0x09260220, 0x00342e05, 0x00000000,
    0x00040025, 0x00004600, 0x00000000, 0x00000210,
    0xa13b1640, 0x260e0302, 0xaa2f1540, 0x270e1102,
    0x00031a70, 0x3c050220, 0x52463b05, 0x00440306,
    0x00131a70, 0x30050220, 0x52462f05, 0x00441106,
    0x00030061, 0x32060220, 0x00343b05, 0x00000000,
    0x00130061, 0x34060220, 0x00342f05, 0x00000000,
    0x00031c40, 0x3d052660, 0x06463c05, 0x00440326,
    0x00131c40, 0x31052660, 0x06463005, 0x00441126,
    0x00031a61, 0x32260220, 0x00343d05, 0x00000000,
    0x00131a61, 0x34260220, 0x00343105, 0x00000000,
    0xe26c004c, 0x00114004, 0x80001969, 0x10018220,
    0x02006c04, 0x00000003, 0x80000961, 0x76060660,
    0x00010380, 0x00000000, 0x80000061, 0x76260660,
    0x00010390, 0x00000000, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x72260220,
    0x00007624, 0x00000000, 0x80031961, 0x72060220,
    0x00007604, 0x00000000, 0xe2360061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004231, 0x77140000, 0xfb00720c, 0x00340000,
    0x80001a61, 0x30010220, 0x00003604, 0x00000000,
    0x80001e69, 0x10018220, 0x02006c04, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000600,
    0x80000961, 0x79060660, 0x00010100, 0x00000000,
    0x80000061, 0x79260660, 0x00010110, 0x00000000,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x80031a61, 0x73260220, 0x00007924, 0x00000000,
    0x80031961, 0x73060220, 0x00007904, 0x00000000,
    0x80002201, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004031, 0x00000000, 0xfb08730c, 0x00347714,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0xe0330068, 0x00801903, 0x00031466, 0x35050220,
    0x02440506, 0x00440526, 0x00131366, 0x36050220,
    0x02441306, 0x00441326, 0x00041a70, 0x00010220,
    0x42460f05, 0x00463305, 0x01040028, 0x0001c660,
    0x000010d8, 0x000010d8, 0x00040069, 0x37058660,
    0x02460f05, 0x00000008, 0x00041c70, 0x00018660,
    0x16463505, 0x00000000, 0x01040022, 0x0001c060,
    0x00000208, 0x00000148, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0xe0390068, 0x00603703,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00040069, 0x3b058660, 0x02463905, 0x00000003,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00031961, 0x78060220, 0x00343b05, 0x00000000,
    0x80103001, 0x00000000, 0x00000000, 0x00000000,
    0x80101a01, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x7a060220, 0x00343c05, 0x00000000,
    0x00031a61, 0x78264220, 0x00000000, 0x00000000,
    0x00131a61, 0x7a264220, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x3c240000, 0xfb047824, 0x000c0000,
    0x00032061, 0x08060220, 0x00343c05, 0x00000000,
    0x00132061, 0x0a060220, 0x00343d05, 0x00000000,
    0x0003a061, 0x08260220, 0x00343e05, 0x00000000,
    0x0013a061, 0x0a260220, 0x00343f05, 0x00000000,
    0x00040024, 0x0001c060, 0x000000d0, 0x000000d0,
    0xa13f1b40, 0x370e0502, 0xaa3d1c40, 0x380e1302,
    0x80001c01, 0x00000000, 0x00000000, 0x00000000,
    0x00031a70, 0x40050220, 0x52463f05, 0x00440506,
    0x00131a70, 0x3e050220, 0x52463d05, 0x00441306,
    0x80001f01, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x08060220, 0x00343f05, 0x00000000,
    0x80101f01, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x0a060220, 0x00343d05, 0x00000000,
    0x00031c40, 0x41052660, 0x06464005, 0x00440526,
    0x00131c40, 0x3f052660, 0x06463e05, 0x00441326,
    0x00031a61, 0x08260220, 0x00344105, 0x00000000,
    0x00131a61, 0x0a260220, 0x00343f05, 0x00000000,
    0x00040025, 0x00004600, 0x00000000, 0x00000ea0,
    0xa1421640, 0x370e0302, 0xaa401540, 0x380e1102,
    0x00031a70, 0x47050220, 0x52464205, 0x00440306,
    0x00131a70, 0x41050220, 0x52464005, 0x00441106,
    0x00030061, 0x43060220, 0x00344205, 0x00000000,
    0x00130061, 0x45060220, 0x00344005, 0x00000000,
    0x00031c40, 0x48052660, 0x06464705, 0x00440326,
    0x00131c40, 0x42052660, 0x06464105, 0x00441126,
    0x00031a61, 0x43260220, 0x00344805, 0x00000000,
    0x00131a61, 0x45260220, 0x00344205, 0x00000000,
    0xe26d004c, 0x00114004, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x80001969, 0x10018220,
    0x02006d04, 0x00000003, 0x80000961, 0x19060660,
    0x00010400, 0x00000000, 0x80000061, 0x19260660,
    0x00010410, 0x00000000, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x79260220,
    0x00001924, 0x00000000, 0x80031961, 0x79060220,
    0x00001904, 0x00000000, 0xe2320061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004031, 0x01140000, 0xfb00790c, 0x00340000,
    0x80001a61, 0x30010220, 0x00003204, 0x00000000,
    0x80001e69, 0x10018220, 0x02006d04, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000800,
    0x80000961, 0x1a060660, 0x00010180, 0x00000000,
    0x80000061, 0x1a260660, 0x00010190, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80031a61, 0x7a260220, 0x00001a24, 0x00000000,
    0x80031961, 0x7a060220, 0x00001a04, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004031, 0x00000000, 0xfb087a0c, 0x00340114,
    0xa0440040, 0x04003703, 0x00040070, 0x00018660,
    0x16463505, 0x00000000, 0x01040022, 0x0001c060,
    0x000001b8, 0x000000f8, 0xe0461b68, 0x00604403,
    0x80001f01, 0x00000000, 0x00000000, 0x00000000,
    0x00041969, 0x48058660, 0x02464605, 0x00000003,
    0x00031961, 0x19060220, 0x00344805, 0x00000000,
    0x80101a01, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x1b060220, 0x00344905, 0x00000000,
    0x00031a61, 0x19264220, 0x00000000, 0x00000000,
    0x00131a61, 0x1b264220, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x49240000, 0xfb041924, 0x000c0000,
    0x00032361, 0x09060220, 0x00344905, 0x00000000,
    0x00132361, 0x0b060220, 0x00344a05, 0x00000000,
    0x0003a361, 0x09260220, 0x00344b05, 0x00000000,
    0x0013a361, 0x0b260220, 0x00344c05, 0x00000000,
    0x00040024, 0x0001c060, 0x000000d0, 0x000000d0,
    0xa1491b40, 0x440e0502, 0xaa4a1c40, 0x450e1302,
    0x80001c01, 0x00000000, 0x00000000, 0x00000000,
    0x00031a70, 0x4c050220, 0x52464905, 0x00440506,
    0x00131a70, 0x4b050220, 0x52464a05, 0x00441306,
    0x80001f01, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x09060220, 0x00344905, 0x00000000,
    0x80101f01, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x0b060220, 0x00344a05, 0x00000000,
    0x00031c40, 0x4d052660, 0x06464c05, 0x00440526,
    0x00131c40, 0x4c052660, 0x06464b05, 0x00441326,
    0x00031a61, 0x09260220, 0x00344d05, 0x00000000,
    0x00131a61, 0x0b260220, 0x00344c05, 0x00000000,
    0x00040025, 0x00004600, 0x00000000, 0x00000aa0,
    0xa14f0040, 0x440e0302, 0xaa4d0040, 0x450e1102,
    0x00031a70, 0x54050220, 0x52464f05, 0x00440306,
    0x00131a70, 0x4e050220, 0x52464d05, 0x00441106,
    0x00030061, 0x50060220, 0x00344f05, 0x00000000,
    0x00130061, 0x52060220, 0x00344d05, 0x00000000,
    0x00031c40, 0x55052660, 0x06465405, 0x00440326,
    0x00131c40, 0x4f052660, 0x06464e05, 0x00441126,
    0x00031a61, 0x50260220, 0x00345505, 0x00000000,
    0x00131a61, 0x52260220, 0x00344f05, 0x00000000,
    0xe26e004c, 0x00114004, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x80003601, 0x00000000,
    0x00000000, 0x00000000, 0x80001969, 0x10018220,
    0x02006e04, 0x00000003, 0x80000961, 0x1e060660,
    0x00010480, 0x00000000, 0x80000061, 0x1e260660,
    0x00010490, 0x00000000, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x1a260220,
    0x00001e24, 0x00000000, 0x80031961, 0x1a060220,
    0x00001e04, 0x00000000, 0xe2310061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004431, 0x1f140000, 0xfb001a0c, 0x00340000,
    0x80001a61, 0x30010220, 0x00003104, 0x00000000,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x80001e69, 0x10018220, 0x02006e04, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000a00,
    0x80000961, 0x21060660, 0x00010000, 0x00000000,
    0x80000061, 0x21260660, 0x00010010, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x80031a61, 0x1b260220, 0x00002124, 0x00000000,
    0x80031961, 0x1b060220, 0x00002104, 0x00000000,
    0x80002401, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004531, 0x00000000, 0xfb081b0c, 0x00341f14,
    0xa0510040, 0x08003703, 0x00040070, 0x00018660,
    0x16463505, 0x00000000, 0x01040022, 0x0001c060,
    0x000001d8, 0x00000118, 0xe0531b68, 0x00605103,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x80001f01, 0x00000000, 0x00000000, 0x00000000,
    0x00041969, 0x55058660, 0x02465305, 0x00000003,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x00031961, 0x20060220, 0x00345505, 0x00000000,
    0x80101a01, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x22060220, 0x00345605, 0x00000000,
    0x00031a61, 0x20264220, 0x00000000, 0x00000000,
    0x00131a61, 0x22264220, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044531, 0x56240000, 0xfb042024, 0x000c0000,
    0x00032561, 0x0a060220, 0x00345605, 0x00000000,
    0x00132561, 0x0c060220, 0x00345705, 0x00000000,
    0x0003a561, 0x0a260220, 0x00345805, 0x00000000,
    0x0013a561, 0x0c260220, 0x00345905, 0x00000000,
    0x00040024, 0x0001c060, 0x000000d0, 0x000000d0,
    0xa1561b40, 0x510e0502, 0xaa571c40, 0x520e1302,
    0x80001c01, 0x00000000, 0x00000000, 0x00000000,
    0x00031a70, 0x59050220, 0x52465605, 0x00440506,
    0x00131a70, 0x58050220, 0x52465705, 0x00441306,
    0x80001f01, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x0a060220, 0x00345605, 0x00000000,
    0x80101f01, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x0c060220, 0x00345705, 0x00000000,
    0x00031c40, 0x5a052660, 0x06465905, 0x00440526,
    0x00131c40, 0x59052660, 0x06465805, 0x00441326,
    0x00031a61, 0x0a260220, 0x00345a05, 0x00000000,
    0x00131a61, 0x0c260220, 0x00345905, 0x00000000,
    0x00040025, 0x00004600, 0x00000000, 0x00000670,
    0xa15c0040, 0x510e0302, 0xaa5a0040, 0x520e1102,
    0x00031a70, 0x61050220, 0x52465c05, 0x00440306,
    0x00131a70, 0x5b050220, 0x52465a05, 0x00441106,
    0x00030061, 0x5d060220, 0x00345c05, 0x00000000,
    0x00130061, 0x5f060220, 0x00345a05, 0x00000000,
    0x00031c40, 0x62052660, 0x06466105, 0x00440326,
    0x00131c40, 0x5c052660, 0x06465b05, 0x00441126,
    0x00031a61, 0x5d260220, 0x00346205, 0x00000000,
    0x00131a61, 0x5f260220, 0x00345c05, 0x00000000,
    0xe26f004c, 0x00114004, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0x80003801, 0x00000000,
    0x00000000, 0x00000000, 0x80001969, 0x10018220,
    0x02006f04, 0x00000003, 0x80000961, 0x25060660,
    0x00010500, 0x00000000, 0x80000061, 0x25260660,
    0x00010510, 0x00000000, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x21260220,
    0x00002524, 0x00000000, 0x80031961, 0x21060220,
    0x00002504, 0x00000000, 0xe2300061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004631, 0x26140000, 0xfb00210c, 0x00340000,
    0x80001a61, 0x30010220, 0x00003004, 0x00000000,
    0x80003901, 0x00000000, 0x00000000, 0x00000000,
    0x80001e69, 0x10018220, 0x02006f04, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000a00,
    0x80000961, 0x28060660, 0x00010680, 0x00000000,
    0x80000061, 0x28260660, 0x00010690, 0x00000000,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x80031a61, 0x22260220, 0x00002824, 0x00000000,
    0x80031961, 0x22060220, 0x00002804, 0x00000000,
    0x80002601, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004731, 0x00000000, 0xfb08220c, 0x00342614,
    0xa05e0040, 0x0c003703, 0x00040070, 0x00018660,
    0x16463505, 0x00000000, 0x01040022, 0x0001c060,
    0x000001d8, 0x00000118, 0xe0601b68, 0x00605e03,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x80001f01, 0x00000000, 0x00000000, 0x00000000,
    0x00041969, 0x62058660, 0x02466005, 0x00000003,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x00031961, 0x27060220, 0x00346205, 0x00000000,
    0x80101a01, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x29060220, 0x00346305, 0x00000000,
    0x00031a61, 0x27264220, 0x00000000, 0x00000000,
    0x00131a61, 0x29264220, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044731, 0x63240000, 0xfb042724, 0x000c0000,
    0x00032761, 0x0b060220, 0x00346305, 0x00000000,
    0x00132761, 0x0d060220, 0x00346405, 0x00000000,
    0x0003a761, 0x0b260220, 0x00346505, 0x00000000,
    0x0013a761, 0x0d260220, 0x00346605, 0x00000000,
    0x00040024, 0x0001c060, 0x000000d0, 0x000000d0,
    0xa1631b40, 0x5e0e0502, 0xaa641c40, 0x5f0e1302,
    0x80001c01, 0x00000000, 0x00000000, 0x00000000,
    0x00031a70, 0x66050220, 0x52466305, 0x00440506,
    0x00131a70, 0x65050220, 0x52466405, 0x00441306,
    0x80001f01, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x0b060220, 0x00346305, 0x00000000,
    0x80101f01, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x0d060220, 0x00346405, 0x00000000,
    0x00031c40, 0x67052660, 0x06466605, 0x00440526,
    0x00131c40, 0x66052660, 0x06466505, 0x00441326,
    0x00031a61, 0x0b260220, 0x00346705, 0x00000000,
    0x00131a61, 0x0d260220, 0x00346605, 0x00000000,
    0x00040025, 0x00004600, 0x00000000, 0x00000240,
    0xa1690040, 0x5e0e0302, 0xaa670040, 0x5f0e1102,
    0x00031a70, 0x6e050220, 0x52466905, 0x00440306,
    0x00131a70, 0x68050220, 0x52466705, 0x00441106,
    0x00030061, 0x6a060220, 0x00346905, 0x00000000,
    0x00130061, 0x6c060220, 0x00346705, 0x00000000,
    0x00031c40, 0x6f052660, 0x06466e05, 0x00440326,
    0x00131c40, 0x69052660, 0x06466805, 0x00441126,
    0x00031a61, 0x6a260220, 0x00346f05, 0x00000000,
    0x00131a61, 0x6c260220, 0x00346905, 0x00000000,
    0xe270004c, 0x00114004, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x80001969, 0x10018220,
    0x02007004, 0x00000003, 0x80000961, 0x37060660,
    0x00010580, 0x00000000, 0x80000061, 0x37260660,
    0x00010590, 0x00000000, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x28260220,
    0x00003724, 0x00000000, 0x80031961, 0x28060220,
    0x00003704, 0x00000000, 0xe22f0061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003901, 0x00000000, 0x00000000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004831, 0x2d140000, 0xfb00280c, 0x00340000,
    0x80001a61, 0x30010220, 0x00002f04, 0x00000000,
    0x80001e69, 0x10018220, 0x02007004, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000c00,
    0x80000961, 0x38060660, 0x00010500, 0x00000000,
    0x80000061, 0x38260660, 0x00010510, 0x00000000,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x80031a61, 0x29260220, 0x00003824, 0x00000000,
    0x80031961, 0x29060220, 0x00003804, 0x00000000,
    0x80002801, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004931, 0x00000000, 0xfb08290c, 0x00342d14,
    0x00040052, 0x0f044160, 0x0e0e0003, 0x0f051705,
    0x00040027, 0x00014060, 0x00000000, 0xffffef18,
    0x80030061, 0x7e050220, 0x00460005, 0x00000000,
    0x80040931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_misc_copy_instances = {
   .prog_data = {
      .base.nr_params = 14,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 5776,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_misc_copy_instances_relocs,
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
      .push.cross_thread.dwords = 14,
      .push.cross_thread.regs = 2,
      .push.cross_thread.size = 64,
   },
   .args_size = 24,
   .arg_count = 3,
   .args = gfx125_misc_copy_instances_args,
   .code = gfx125_misc_copy_instances_code,
};
const char *gfx125_misc_copy_instances_sha1 = "31d3d98287856c30b993c554fd3466a6f2735ad7";
