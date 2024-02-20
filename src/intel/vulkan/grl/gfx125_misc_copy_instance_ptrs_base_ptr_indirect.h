#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_misc_copy_instance_ptrs_base_ptr_indirect_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_misc_copy_instance_ptrs_base_ptr_indirect_args[] = {
   { 0, 8 },
   { 8, 8 },
   { 16, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g64<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g126<1>UD       g0.1<0,1,0>UD                   { align1 1H };
add(1)          g65<1>UD        g64<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@2 compacted };
mov(16)         g75<1>UD        g126<8,8,1>UD                   { align1 1H I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(1)         g1UD            g65UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g72.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g42.1<2>F       g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g38.1<2>F       g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g44.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g10.1<2>F       g2.5<0,1,0>F                    { align1 1Q };
mov(8)          g12.1<2>F       g2.5<0,1,0>F                    { align1 2Q };
mov(8)          g72<2>F         g2<0,1,0>F                      { align1 1Q F@6 compacted };
mov(8)          g42<2>F         g2<0,1,0>F                      { align1 2Q F@6 compacted };
mov(8)          g38<2>F         g2.2<0,1,0>F                    { align1 1Q F@6 compacted };
mov(8)          g44<2>F         g2.2<0,1,0>F                    { align1 2Q F@6 compacted };
mov(8)          g10<2>F         g2.4<0,1,0>F                    { align1 1Q F@6 compacted };
mov(8)          g12<2>F         g2.4<0,1,0>F                    { align1 2Q F@6 compacted };
add(8)          g46<1>D         g72<8,4,2>D     152D            { align1 1Q F@6 compacted };
add(8)          g47<1>D         g42<8,4,2>D     152D            { align1 2Q F@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
send(16)        g1UD            g10UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
mov(8)          g6<2>UD         g46<4,4,1>UD                    { align1 1Q I@2 };
cmp.l.f0.0(16)  g48<1>UD        g46<1,1,0>UD    0x00000098UD    { align1 1H I@2 compacted };
mov(8)          g8<2>UD         g47<4,4,1>UD                    { align1 2Q };
add(8)          g74<1>D         -g48<8,8,1>D    g72.1<8,4,2>D   { align1 1Q I@2 };
add(8)          g49<1>D         -g49<8,8,1>D    g42.1<8,4,2>D   { align1 2Q I@3 };
mov(8)          g6.1<2>UD       g74<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g8.1<2>UD       g49<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g50UD           g6UD            nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
shl(16)         g56<1>D         g1<8,8,1>D      0x00000006UD    { align1 1H $1.dst };
shr(16)         g58<1>UD        g56<1,1,0>UD    0x00000008UD    { align1 1H I@1 compacted };
and(16)         g105<1>UD       g56<8,8,1>UD    0xffffff00UD    { align1 1H };
add3(16)        g68<1>D         65534W          -g58<8,8,1>D    g126<1,1,1>D { align1 1H I@2 };
add(16)         g107<1>D        g56<1,1,0>D     -g105<1,1,0>D   { align1 1H I@2 compacted };
mov(16)         g70<1>D         -g68<8,8,1>D                    { align1 1H I@2 };
and(16)         g109<1>UD       g107<8,8,1>UD   0xffffffc0UD    { align1 1H I@2 };
add(8)          g52<1>D         g72<8,4,2>D     g50<1,1,0>D     { align1 1Q $2.dst compacted };
add(8)          g53<1>D         g42<8,4,2>D     g51<1,1,0>D     { align1 2Q $2.dst compacted };
shr(16)         g111<1>UD       g109<1,1,0>UD   0x00000006UD    { align1 1H I@3 compacted };
cmp.l.f0.0(8)   g40<1>UD        g52<8,8,1>UD    g72<8,4,2>UD    { align1 1Q I@3 };
cmp.l.f0.0(8)   g54<1>UD        g53<8,8,1>UD    g42<8,4,2>UD    { align1 2Q I@3 };
cmp.l.f0.0(16)  null<1>UD       g70<8,8,1>UD    g111<8,8,1>UD   { align1 1H I@3 };
add(8)          g55<1>D         -g40<8,8,1>D    g72.1<8,4,2>D   { align1 1Q I@3 };
add(8)          g56<1>D         -g54<8,8,1>D    g42.1<8,4,2>D   { align1 2Q I@3 };
(+f0.0) if(16)  JIP:  LABEL0          UIP:  LABEL0              { align1 1H };
shl(16)         g71<1>D         -g68<8,8,1>D    0x00000006UD    { align1 1H };
mov(8)          g89<1>UD        g38.1<8,4,2>UD                  { align1 1Q F@4 };
mov(8)          g90<1>UD        g44.1<8,4,2>UD                  { align1 2Q F@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
add(16)         g73<1>D         g105<1,1,0>D    g71<1,1,0>D     { align1 1H I@3 compacted };
shr(16)         g40<1>UD        g73<1,1,0>UD    0x00000006UD    { align1 1H I@1 compacted };
add(16)         g96<1>D         g52<1,1,0>D     g73<1,1,0>D     { align1 1H compacted };
add(16)         g77<1>D         g3<1,1,0>D      g40<1,1,0>D     { align1 1H @2 $1.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(8)          g9<2>UD         g96<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $1.src };
mov(8)          g11<2>UD        g97<4,4,1>UD                    { align1 2Q I@3 };
cmp.l.f0.0(16)  g79<1>UD        g77<1,1,0>UD    g3<1,1,0>UD     { align1 1H I@3 compacted };
shl(16)         g81<1>D         g77<8,8,1>D     0x00000003UD    { align1 1H };
shr(16)         g85<1>UD        g77<1,1,0>UD    0x0000001dUD    { align1 1H compacted };
shl(16)         g83<1>D         -g79<8,8,1>D    0x00000003UD    { align1 1H I@3 };
add(8)          g41<1>D         g38<8,4,2>D     g81<1,1,0>D     { align1 1Q I@3 compacted };
add(8)          g91<1>D         g44<8,4,2>D     g82<1,1,0>D     { align1 2Q I@4 compacted };
or(16)          g87<1>UD        g83<1,1,0>UD    g85<1,1,0>UD    { align1 1H I@3 compacted };
cmp.l.f0.0(8)   g92<1>UD        g41<8,8,1>UD    g38<8,4,2>UD    { align1 1Q I@3 };
mov(8)          g30<2>UD        g41<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g93<1>UD        g91<8,8,1>UD    g44<8,4,2>UD    { align1 2Q I@4 };
mov(8)          g32<2>UD        g91<4,4,1>UD                    { align1 2Q };
cmp.l.f0.0(16)  g98<1>UD        g96<1,1,0>UD    g52<1,1,0>UD    { align1 1H compacted };
add3(16)        g94<1>D         g89<8,8,1>D     g87<8,8,1>D     -g92<1,1,1>D { align1 1H I@3 };
add(16)         g100<1>D        -g98<1,1,0>D    g55<1,1,0>D     { align1 1H I@2 compacted };
mov(8)          g30.1<2>UD      g94<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g32.1<2>UD      g95<4,4,1>UD                    { align1 2Q I@3 };
mov(8)          g9.1<2>UD       g100<4,4,1>UD                   { align1 1Q I@3 };
mov(8)          g11.1<2>UD      g101<4,4,1>UD                   { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g95UD           g30UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g5<2>UD         g95<4,4,1>UD                    { align1 1Q $3.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $2.src };
mov(8)          g7<2>UD         g96<4,4,1>UD                    { align1 2Q $3.dst };
mov(8)          g5.1<2>UD       g97<4,4,1>UD                    { align1 1Q @2 $3.dst };
mov(8)          g7.1<2>UD       g98<4,4,1>UD                    { align1 2Q @2 $3.dst };
fbl(1)          g13<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N $1.src compacted };
shl(1)          a0<1>UD         g13<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g90<2>D         g[a0 160]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g90.1<2>D       g[a0 164]<0,1,0>D               { align1 WE_all 1N };
mov(8)          g15.1<2>UD      g90.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g15<2>UD        g90<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g71<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g20UD    g15UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $4 };
mov(1)          f0<1>UD         g71<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g13<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
mov(1)          g91<2>D         g[a0 288]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g91.1<2>D       g[a0 292]<0,1,0>D               { align1 WE_all 1N };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
mov(8)          g16.1<2>UD      g91.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g16<2>UD        g91<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g16UD           g20UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $5 };

LABEL0:
endif(16)       JIP:  LABEL1                                    { align1 1H };

LABEL1:
shr(16)         g101<1>UD       g105<1,1,0>UD   0x00000008UD    { align1 1H compacted };

LABEL3:
cmp.ge.f0.0(16) null<1>UD       g75<8,8,1>UD    g101<8,8,1>UD   { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL2        UIP:  LABEL2              { align1 1H };
shl(16)         g103<1>D        g75<8,8,1>D     0x00000008UD    { align1 1H };
mov(8)          g119<1>UD       g38.1<8,4,2>UD                  { align1 1Q F@4 };
mov(8)          g120<1>UD       g44.1<8,4,2>UD                  { align1 2Q F@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
shr(16)         g60<1>UD        g103<1,1,0>UD   0x00000006UD    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g6<1>D          g52<1,1,0>D     g103<1,1,0>D    { align1 1H compacted };
add(16)         g62<1>D         g3<1,1,0>D      g60<1,1,0>D     { align1 1H @2 $1.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g14<2>UD        g6<4,4,1>UD                     { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $5.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N I@3 };
mov(8)          g16<2>UD        g7<4,4,1>UD                     { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
cmp.l.f0.0(16)  g64<1>UD        g62<1,1,0>UD    g3<1,1,0>UD     { align1 1H compacted };
shl(16)         g66<1>D         g62<8,8,1>D     0x00000003UD    { align1 1H };
shr(16)         g115<1>UD       g62<1,1,0>UD    0x0000001dUD    { align1 1H compacted };
shl(16)         g113<1>D        -g64<8,8,1>D    0x00000003UD    { align1 1H I@3 };
add(8)          g77<1>D         g38<8,4,2>D     g66<1,1,0>D     { align1 1Q I@3 compacted };
add(8)          g121<1>D        g44<8,4,2>D     g67<1,1,0>D     { align1 2Q I@4 compacted };
or(16)          g117<1>UD       g113<1,1,0>UD   g115<1,1,0>UD   { align1 1H I@3 compacted };
cmp.l.f0.0(8)   g122<1>UD       g77<8,8,1>UD    g38<8,4,2>UD    { align1 1Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(8)          g31<2>UD        g77<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g123<1>UD       g121<8,8,1>UD   g44<8,4,2>UD    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $6.src };
mov(8)          g33<2>UD        g121<4,4,1>UD                   { align1 2Q $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
cmp.l.f0.0(16)  g8<1>UD         g6<1,1,0>UD     g52<1,1,0>UD    { align1 1H compacted };
add3(16)        g124<1>D        g119<8,8,1>D    g117<8,8,1>D    -g122<1,1,1>D { align1 1H I@3 };
add(16)         g18<1>D         -g8<1,1,0>D     g55<1,1,0>D     { align1 1H I@2 compacted };
mov(8)          g31.1<2>UD      g124<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g33.1<2>UD      g125<4,4,1>UD                   { align1 2Q I@3 };
mov(8)          g14.1<2>UD      g18<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g16.1<2>UD      g19<4,4,1>UD                    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g5UD            g31UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
mov(8)          g10<2>UD        g5<4,4,1>UD                     { align1 1Q $7.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $1.src };
mov(8)          g12<2>UD        g6<4,4,1>UD                     { align1 2Q $7.dst };
mov(8)          g10.1<2>UD      g7<4,4,1>UD                     { align1 1Q @2 $7.dst };
mov(8)          g12.1<2>UD      g8<4,4,1>UD                     { align1 2Q @2 $7.dst };
fbl(1)          g20<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N $5.src compacted };
shl(1)          a0<1>UD         g20<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g92<2>D         g[a0 320]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g92.1<2>D       g[a0 324]<0,1,0>D               { align1 WE_all 1N };
mov(8)          g22.1<2>UD      g92.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g22<2>UD        g92<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g70<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g27UD    g22UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $8 };
mov(1)          f0<1>UD         g70<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g20<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
mov(1)          g93<2>D         g[a0 448]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g93.1<2>D       g[a0 452]<0,1,0>D               { align1 WE_all 1N };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
mov(8)          g23.1<2>UD      g93.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g23<2>UD        g93<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g23UD           g27UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $9 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
add(16)         g23<1>D         g103<1,1,0>D    64D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
shr(16)         g25<1>UD        g23<1,1,0>UD    0x00000006UD    { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
add(16)         g109<1>D        g52<1,1,0>D     g23<1,1,0>D     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
add(16)         g27<1>D         g3<1,1,0>D      g25<1,1,0>D     { align1 1H I@2 compacted };
mov(8)          g19<2>UD        g109<4,4,1>UD                   { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $5.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $8.src };
mov(8)          g21<2>UD        g110<4,4,1>UD                   { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
shl(16)         g36<1>D         g27<8,8,1>D     0x00000003UD    { align1 1H I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
shr(16)         g46<1>UD        g27<1,1,0>UD    0x0000001dUD    { align1 1H compacted };
add(8)          g78<1>D         g38<8,4,2>D     g36<1,1,0>D     { align1 1Q I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $11.src };
add(8)          g50<1>D         g44<8,4,2>D     g37<1,1,0>D     { align1 2Q I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
mov(8)          g32<2>UD        g78<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $6.src };
mov(8)          g34<2>UD        g50<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
cmp.l.f0.0(16)  g29<1>UD        g27<1,1,0>UD    g3<1,1,0>UD     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
cmp.l.f0.0(8)   g105<1>UD       g78<8,8,1>UD    g38<8,4,2>UD    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $10.src };
cmp.l.f0.0(8)   g106<1>UD       g50<8,8,1>UD    g44<8,4,2>UD    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.src };
cmp.l.f0.0(16)  g111<1>UD       g109<1,1,0>UD   g52<1,1,0>UD    { align1 1H compacted };
shl(16)         g42<1>D         -g29<8,8,1>D    0x00000003UD    { align1 1H I@4 };
add(16)         g68<1>D         -g111<1,1,0>D   g55<1,1,0>D     { align1 1H I@2 compacted };
or(16)          g48<1>UD        g42<1,1,0>UD    g46<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g19.1<2>UD      g68<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g21.1<2>UD      g69<4,4,1>UD                    { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
add3(16)        g107<1>D        g119<8,8,1>D    g48<8,8,1>D     -g105<1,1,1>D { align1 1H };
mov(8)          g32.1<2>UD      g107<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g34.1<2>UD      g108<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g108UD          g32UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $13 };
mov(8)          g15<2>UD        g108<4,4,1>UD                   { align1 1Q $13.dst };
mov(8)          g17<2>UD        g109<4,4,1>UD                   { align1 2Q $13.dst };
mov(8)          g15.1<2>UD      g110<4,4,1>UD                   { align1 1Q @2 $13.dst };
mov(8)          g17.1<2>UD      g111<4,4,1>UD                   { align1 2Q @2 $13.dst };
fbl(1)          g70<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g70<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g94<2>D         g[a0 480]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g94.1<2>D       g[a0 484]<0,1,0>D               { align1 WE_all 1N };
mov(8)          g28.1<2>UD      g94.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g28<2>UD        g94<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g69<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g33UD    g28UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $14 };
mov(1)          f0<1>UD         g69<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g70<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000200UD    { align1 WE_all 1N A@1 };
mov(1)          g95<2>D         g[a0 96]<0,1,0>D                { align1 WE_all 1N A@1 };
mov(1)          g95.1<2>D       g[a0 100]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
mov(8)          g29.1<2>UD      g95.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g29<2>UD        g95<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g29UD           g33UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $15 };
add(16)         g69<1>D         g103<1,1,0>D    128D            { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
shr(16)         g71<1>UD        g69<1,1,0>UD    0x00000006UD    { align1 1H I@1 compacted };
add(16)         g90<1>D         g52<1,1,0>D     g69<1,1,0>D     { align1 1H compacted };
add(16)         g73<1>D         g3<1,1,0>D      g71<1,1,0>D     { align1 1H I@2 compacted };
mov(8)          g24<2>UD        g90<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g26<2>UD        g91<4,4,1>UD                    { align1 2Q I@3 };
shl(16)         g77<1>D         g73<8,8,1>D     0x00000003UD    { align1 1H I@3 };
shr(16)         g81<1>UD        g73<1,1,0>UD    0x0000001dUD    { align1 1H compacted };
add(8)          g88<1>D         g38<8,4,2>D     g77<1,1,0>D     { align1 1Q I@2 compacted };
add(8)          g85<1>D         g44<8,4,2>D     g78<1,1,0>D     { align1 2Q I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
mov(8)          g33<2>UD        g88<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $13.src };
mov(8)          g35<2>UD        g85<4,4,1>UD                    { align1 2Q I@2 };
cmp.l.f0.0(16)  g40<1>UD        g73<1,1,0>UD    g3<1,1,0>UD     { align1 1H compacted };
cmp.l.f0.0(8)   g86<1>UD        g88<8,8,1>UD    g38<8,4,2>UD    { align1 1Q };
cmp.l.f0.0(8)   g87<1>UD        g85<8,8,1>UD    g44<8,4,2>UD    { align1 2Q };
cmp.l.f0.0(16)  g92<1>UD        g90<1,1,0>UD    g52<1,1,0>UD    { align1 1H compacted };
shl(16)         g79<1>D         -g40<8,8,1>D    0x00000003UD    { align1 1H I@4 };
add(16)         g94<1>D         -g92<1,1,0>D    g55<1,1,0>D     { align1 1H I@2 compacted };
or(16)          g83<1>UD        g79<1,1,0>UD    g81<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g24.1<2>UD      g94<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g26.1<2>UD      g95<4,4,1>UD                    { align1 2Q I@3 };
add3(16)        g88<1>D         g119<8,8,1>D    g83<8,8,1>D     -g86<1,1,1>D { align1 1H I@3 };
mov(8)          g33.1<2>UD      g88<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g35.1<2>UD      g89<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g89UD           g33UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
mov(8)          g20<2>UD        g89<4,4,1>UD                    { align1 1Q $3.dst };
mov(8)          g22<2>UD        g90<4,4,1>UD                    { align1 2Q $3.dst };
mov(8)          g20.1<2>UD      g91<4,4,1>UD                    { align1 1Q @2 $3.dst };
mov(8)          g22.1<2>UD      g92<4,4,1>UD                    { align1 2Q @2 $3.dst };
fbl(1)          g96<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g96<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000200UD    { align1 WE_all 1N A@1 };
mov(1)          g60<2>D         g[a0 128]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g60.1<2>D       g[a0 132]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(8)          g34.1<2>UD      g60.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g34<2>UD        g60<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g68<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g50UD    g34UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
mov(1)          f0<1>UD         g68<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g96<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000200UD    { align1 WE_all 1N A@1 };
mov(1)          g61<2>D         g[a0 256]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g61.1<2>D       g[a0 260]<0,1,0>D               { align1 WE_all 1N };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
mov(8)          g46.1<2>UD      g61.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g46<2>UD        g61<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g46UD           g50UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $11 };
add(16)         g95<1>D         g103<1,1,0>D    192D            { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
shr(16)         g97<1>UD        g95<1,1,0>UD    0x00000006UD    { align1 1H compacted };
add(16)         g121<1>D        g52<1,1,0>D     g95<1,1,0>D     { align1 1H compacted };
add(16)         g99<1>D         g3<1,1,0>D      g97<1,1,0>D     { align1 1H I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
mov(8)          g29<2>UD        g121<4,4,1>UD                   { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $13.src };
mov(8)          g31<2>UD        g122<4,4,1>UD                   { align1 2Q I@3 };
shl(16)         g60<1>D         g99<8,8,1>D     0x00000003UD    { align1 1H I@3 };
shr(16)         g64<1>UD        g99<1,1,0>UD    0x0000001dUD    { align1 1H compacted };
add(8)          g89<1>D         g38<8,4,2>D     g60<1,1,0>D     { align1 1Q I@2 compacted };
add(8)          g113<1>D        g44<8,4,2>D     g61<1,1,0>D     { align1 2Q I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
mov(8)          g34<2>UD        g89<4,4,1>UD                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $3.src };
mov(8)          g36<2>UD        g113<4,4,1>UD                   { align1 2Q I@2 };
cmp.l.f0.0(16)  g103<1>UD       g99<1,1,0>UD    g3<1,1,0>UD     { align1 1H compacted };
cmp.l.f0.0(8)   g114<1>UD       g89<8,8,1>UD    g38<8,4,2>UD    { align1 1Q };
cmp.l.f0.0(8)   g115<1>UD       g113<8,8,1>UD   g44<8,4,2>UD    { align1 2Q };
cmp.l.f0.0(16)  g123<1>UD       g121<1,1,0>UD   g52<1,1,0>UD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
shl(16)         g62<1>D         -g103<8,8,1>D   0x00000003UD    { align1 1H };
add(16)         g125<1>D        -g123<1,1,0>D   g55<1,1,0>D     { align1 1H I@2 compacted };
or(16)          g66<1>UD        g62<1,1,0>UD    g64<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g29.1<2>UD      g125<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g31.1<2>UD      g126<4,4,1>UD                   { align1 2Q I@3 };
add3(16)        g116<1>D        g119<8,8,1>D    g66<8,8,1>D     -g114<1,1,1>D { align1 1H I@3 };
mov(8)          g34.1<2>UD      g116<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g36.1<2>UD      g117<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g117UD          g34UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $6 };
mov(8)          g25<2>UD        g117<4,4,1>UD                   { align1 1Q $6.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $14.src };
mov(8)          g27<2>UD        g118<4,4,1>UD                   { align1 2Q $6.dst };
mov(8)          g25.1<2>UD      g119<4,4,1>UD                   { align1 1Q @2 $6.dst };
mov(8)          g27.1<2>UD      g120<4,4,1>UD                   { align1 2Q @2 $6.dst };
fbl(1)          g5<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g5<0,1,0>UD     0x00000003UD    { align1 WE_all 1N I@1 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000200UD    { align1 WE_all 1N A@1 };
mov(1)          g62<2>D         g[a0 288]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g62.1<2>D       g[a0 292]<0,1,0>D               { align1 WE_all 1N };
mov(8)          g105.1<2>UD     g62.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g105<2>UD       g62<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g112<1>UD       f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g110UD   g105UD          nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $12 };
mov(1)          f0<1>UD         g112<0,1,0>UD                   { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g5<0,1,0>UD     0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000200UD    { align1 WE_all 1N A@1 };
mov(1)          g63<2>D         g[a0 416]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g63.1<2>D       g[a0 420]<0,1,0>D               { align1 WE_all 1N };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
mov(8)          g106.1<2>UD     g63.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g106<2>UD       g63<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g106UD          g110UD          0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $10 };
add3(16)        g75<1>D         0x0003UW        g58<8,8,1>D     g75<1,1,1>D { align1 1H };

LABEL2:
while(16)       JIP:  LABEL3                                    { align1 1H };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_misc_copy_instance_ptrs_base_ptr_indirect_code[] = {
    0x80000065, 0x40058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x7e050220, 0x00000024, 0x00000000,
    0xe2411a40, 0x00014003, 0x00041a61, 0x4b050220,
    0x00467e05, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa00410c, 0x00340000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x21480061, 0x001102cc,
    0x2a2a0061, 0x001102cc, 0x00030061, 0x26260aa0,
    0x00000264, 0x00000000, 0x00130061, 0x2c260aa0,
    0x00000264, 0x00000000, 0x00030061, 0x0a260aa0,
    0x000002a4, 0x00000000, 0x00130061, 0x0c260aa0,
    0x000002a4, 0x00000000, 0x21481661, 0x00110204,
    0x2a2a1661, 0x00110204, 0x21261661, 0x00110244,
    0x2a2c1661, 0x00110244, 0x210a1661, 0x0011025c,
    0x2a0c1661, 0x0011025c, 0xa12e1640, 0x098e4803,
    0xaa2f1540, 0x098e2a03, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x80001101, 0x00000000,
    0x00000000, 0x00000000, 0x00044131, 0x01240000,
    0xfb040a24, 0x000c0000, 0x00031a61, 0x06060220,
    0x00342e05, 0x00000000, 0xe7301a70, 0x09802e03,
    0x00130061, 0x08060220, 0x00342f05, 0x00000000,
    0x00031a40, 0x4a052660, 0x06463005, 0x00444826,
    0x00131b40, 0x31052660, 0x06463105, 0x00442a26,
    0x00031a61, 0x06260220, 0x00344a05, 0x00000000,
    0x00131a61, 0x08260220, 0x00343105, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x32140000, 0xfb040624, 0x00040000,
    0x00042169, 0x38058660, 0x02460105, 0x00000006,
    0xe03a1968, 0x00803803, 0x00040065, 0x69058220,
    0x02463805, 0xffffff00, 0x00041a52, 0x44044560,
    0x0e8efffe, 0x7e053a05, 0xa06b1a40, 0x69203802,
    0x00041a61, 0x46052660, 0x00464405, 0x00000000,
    0x00041a65, 0x6d058220, 0x02466b05, 0xffffffc0,
    0xa1342240, 0x320e4802, 0xaa352240, 0x330e2a02,
    0xe06f1b68, 0x00606d03, 0x00031b70, 0x28050220,
    0x52463405, 0x00444806, 0x00131b70, 0x36050220,
    0x52463505, 0x00442a06, 0x00041b70, 0x00010220,
    0x52464605, 0x00466f05, 0x00031b40, 0x37052660,
    0x06462805, 0x00444826, 0x00131b40, 0x38052660,
    0x06463605, 0x00442a26, 0x01040022, 0x0001c060,
    0x00000358, 0x00000358, 0x00040069, 0x4705a660,
    0x02464405, 0x00000006, 0x00031461, 0x59050220,
    0x00442626, 0x00000000, 0x00131361, 0x5a050220,
    0x00442c26, 0x00000000, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0xa0491b40, 0x47006902,
    0xe0281968, 0x00604903, 0xa0600040, 0x49003402,
    0xa04da140, 0x28000302, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x00031a61, 0x09060220,
    0x00346005, 0x00000000, 0x80103101, 0x00000000,
    0x00000000, 0x00000000, 0x00131b61, 0x0b060220,
    0x00346105, 0x00000000, 0x274f1b70, 0x03004d03,
    0x00040069, 0x51058660, 0x02464d05, 0x00000003,
    0xe0550068, 0x01d04d03, 0x00041b69, 0x5305a660,
    0x02464f05, 0x00000003, 0xa1291b40, 0x510e2602,
    0xaa5b1c40, 0x520e2c02, 0x20571b66, 0x55005303,
    0x00031b70, 0x5c050220, 0x52462905, 0x00442606,
    0x00030061, 0x1e060220, 0x00342905, 0x00000000,
    0x00131c70, 0x5d050220, 0x52465b05, 0x00442c06,
    0x00130061, 0x20060220, 0x00345b05, 0x00000000,
    0x27620070, 0x34006003, 0x00041b52, 0x5e040e68,
    0x0e2e5905, 0x5c055705, 0xa0641a40, 0x37026202,
    0x00031a61, 0x1e260220, 0x00345e05, 0x00000000,
    0x00131b61, 0x20260220, 0x00345f05, 0x00000000,
    0x00031b61, 0x09260220, 0x00346405, 0x00000000,
    0x00131c61, 0x0b260220, 0x00346505, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x5f240000, 0xfb041e24, 0x000c0000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x00032361, 0x05060220, 0x00345f05, 0x00000000,
    0x80103201, 0x00000000, 0x00000000, 0x00000000,
    0x00132361, 0x07060220, 0x00346005, 0x00000000,
    0x0003a361, 0x05260220, 0x00346105, 0x00000000,
    0x0013a361, 0x07260220, 0x00346205, 0x00000000,
    0xe20d314c, 0x00114004, 0x80001969, 0x10018220,
    0x02000d04, 0x00000003, 0x80000961, 0x5a060660,
    0x00010280, 0x00000000, 0x80000061, 0x5a260660,
    0x00010290, 0x00000000, 0x80031961, 0x0f260220,
    0x00005a24, 0x00000000, 0x80031961, 0x0f060220,
    0x00005a04, 0x00000000, 0xe2470061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004431, 0x14140000, 0xfb000f0c, 0x00340000,
    0x80001a61, 0x30010220, 0x00004704, 0x00000000,
    0x80001e69, 0x10018220, 0x02000d04, 0x00000003,
    0x80000961, 0x5b060660, 0x00010480, 0x00000000,
    0x80000061, 0x5b260660, 0x00010490, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80031a61, 0x10260220, 0x00005b24, 0x00000000,
    0x80031961, 0x10060220, 0x00005b04, 0x00000000,
    0x80002401, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004531, 0x00000000, 0xfb08100c, 0x00341414,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0xe0650068, 0x00806903, 0x00041970, 0x00010220,
    0x42464b05, 0x00466505, 0x01040028, 0x0001c660,
    0x00000f28, 0x00000f28, 0x00040069, 0x67058660,
    0x02464b05, 0x00000008, 0x00031461, 0x77050220,
    0x00442626, 0x00000000, 0x00131361, 0x78050220,
    0x00442c26, 0x00000000, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x80001f01, 0x00000000,
    0x00000000, 0x00000000, 0xe03c1b68, 0x00606703,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0xa0060040, 0x67003402, 0xa03ea140, 0x3c000302,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x00031a61, 0x0e060220, 0x00340605, 0x00000000,
    0x80103501, 0x00000000, 0x00000000, 0x00000000,
    0x80101b01, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x10060220, 0x00340705, 0x00000000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x27400070, 0x03003e03, 0x00040069, 0x42058660,
    0x02463e05, 0x00000003, 0xe0730068, 0x01d03e03,
    0x00041b69, 0x7105a660, 0x02464005, 0x00000003,
    0xa14d1b40, 0x420e2602, 0xaa791c40, 0x430e2c02,
    0x20751b66, 0x73007103, 0x00031b70, 0x7a050220,
    0x52464d05, 0x00442606, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x00030061, 0x1f060220,
    0x00344d05, 0x00000000, 0x00131c70, 0x7b050220,
    0x52467905, 0x00442c06, 0x80103601, 0x00000000,
    0x00000000, 0x00000000, 0x00133361, 0x21060220,
    0x00347905, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x27080070, 0x34000603,
    0x00041b52, 0x7c040e68, 0x0e2e7705, 0x7a057505,
    0xa0121a40, 0x37020802, 0x00031a61, 0x1f260220,
    0x00347c05, 0x00000000, 0x00131b61, 0x21260220,
    0x00347d05, 0x00000000, 0x00031b61, 0x0e260220,
    0x00341205, 0x00000000, 0x00131c61, 0x10260220,
    0x00341305, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x00044731, 0x05240000,
    0xfb041f24, 0x000c0000, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x00032761, 0x0a060220,
    0x00340505, 0x00000000, 0x80103101, 0x00000000,
    0x00000000, 0x00000000, 0x00132761, 0x0c060220,
    0x00340605, 0x00000000, 0x0003a761, 0x0a260220,
    0x00340705, 0x00000000, 0x0013a761, 0x0c260220,
    0x00340805, 0x00000000, 0xe214354c, 0x00114004,
    0x80001969, 0x10018220, 0x02001404, 0x00000003,
    0x80000961, 0x5c060660, 0x00010500, 0x00000000,
    0x80000061, 0x5c260660, 0x00010510, 0x00000000,
    0x80031961, 0x16260220, 0x00005c24, 0x00000000,
    0x80031961, 0x16060220, 0x00005c04, 0x00000000,
    0xe2460061, 0x00113004, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x8a004831, 0x1b140000,
    0xfb00160c, 0x00340000, 0x80001a61, 0x30010220,
    0x00004604, 0x00000000, 0x80001e69, 0x10018220,
    0x02001404, 0x00000003, 0x80000961, 0x5d060660,
    0x00010700, 0x00000000, 0x80000061, 0x5d260660,
    0x00010710, 0x00000000, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80031a61, 0x17260220,
    0x00005d24, 0x00000000, 0x80031961, 0x17060220,
    0x00005d04, 0x00000000, 0x80002801, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x8a004931, 0x00000000,
    0xfb08170c, 0x00341b14, 0x80003901, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0xa0170040, 0x04006703,
    0x80003601, 0x00000000, 0x00000000, 0x00000000,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0xe0191968, 0x00601703, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0xa06d0040, 0x17003402,
    0x80003901, 0x00000000, 0x00000000, 0x00000000,
    0xa01b1a40, 0x19000302, 0x00031a61, 0x13060220,
    0x00346d05, 0x00000000, 0x80103501, 0x00000000,
    0x00000000, 0x00000000, 0x80103801, 0x00000000,
    0x00000000, 0x00000000, 0x00131b61, 0x15060220,
    0x00346e05, 0x00000000, 0x80003601, 0x00000000,
    0x00000000, 0x00000000, 0x00041b69, 0x24058660,
    0x02461b05, 0x00000003, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0xe02e0068, 0x01d01b03,
    0xa14e1a40, 0x240e2602, 0x80103b01, 0x00000000,
    0x00000000, 0x00000000, 0xaa321b40, 0x250e2c02,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x00031a61, 0x20060220, 0x00344e05, 0x00000000,
    0x80103701, 0x00000000, 0x00000000, 0x00000000,
    0x80103601, 0x00000000, 0x00000000, 0x00000000,
    0x00131a61, 0x22060220, 0x00343205, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x271d0070, 0x03001b03, 0x80003c01, 0x00000000,
    0x00000000, 0x00000000, 0x00030070, 0x69050220,
    0x52464e05, 0x00442606, 0x80103a01, 0x00000000,
    0x00000000, 0x00000000, 0x00130070, 0x6a050220,
    0x52463205, 0x00442c06, 0x80003a01, 0x00000000,
    0x00000000, 0x00000000, 0x276f0070, 0x34006d03,
    0x00041c69, 0x2a05a660, 0x02461d05, 0x00000003,
    0xa0441a40, 0x37026f02, 0x20301a66, 0x2e002a03,
    0x00031a61, 0x13260220, 0x00344405, 0x00000000,
    0x00131b61, 0x15260220, 0x00344505, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00040052, 0x6b040e68, 0x0e2e7705, 0x69053005,
    0x00031961, 0x20260220, 0x00346b05, 0x00000000,
    0x00131a61, 0x22260220, 0x00346c05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044d31, 0x6c240000, 0xfb042024, 0x000c0000,
    0x00032d61, 0x0f060220, 0x00346c05, 0x00000000,
    0x00132d61, 0x11060220, 0x00346d05, 0x00000000,
    0x0003ad61, 0x0f260220, 0x00346e05, 0x00000000,
    0x0013ad61, 0x11260220, 0x00346f05, 0x00000000,
    0xe246004c, 0x00114004, 0x80001969, 0x10018220,
    0x02004604, 0x00000003, 0x80000961, 0x5e060660,
    0x00010780, 0x00000000, 0x80000061, 0x5e260660,
    0x00010790, 0x00000000, 0x80031961, 0x1c260220,
    0x00005e24, 0x00000000, 0x80031961, 0x1c060220,
    0x00005e04, 0x00000000, 0xe2450061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004e31, 0x21140000, 0xfb001c0c, 0x00340000,
    0x80001a61, 0x30010220, 0x00004504, 0x00000000,
    0x80001e69, 0x10018220, 0x02004604, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000200,
    0x80000961, 0x5f060660, 0x00010180, 0x00000000,
    0x80000061, 0x5f260660, 0x00010190, 0x00000000,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80031a61, 0x1d260220, 0x00005f24, 0x00000000,
    0x80031961, 0x1d060220, 0x00005f04, 0x00000000,
    0x80002e01, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004f31, 0x00000000, 0xfb081d0c, 0x00342114,
    0xa0450040, 0x08006703, 0x80003f01, 0x00000000,
    0x00000000, 0x00000000, 0xe0471968, 0x00604503,
    0xa05a0040, 0x45003402, 0xa0491a40, 0x47000302,
    0x00031a61, 0x18060220, 0x00345a05, 0x00000000,
    0x00131b61, 0x1a060220, 0x00345b05, 0x00000000,
    0x00041b69, 0x4d058660, 0x02464905, 0x00000003,
    0xe0510068, 0x01d04903, 0xa1581a40, 0x4d0e2602,
    0xaa551b40, 0x4e0e2c02, 0x80003f01, 0x00000000,
    0x00000000, 0x00000000, 0x00031a61, 0x21060220,
    0x00345805, 0x00000000, 0x80103d01, 0x00000000,
    0x00000000, 0x00000000, 0x00131a61, 0x23060220,
    0x00345505, 0x00000000, 0x27280070, 0x03004903,
    0x00030070, 0x56050220, 0x52465805, 0x00442606,
    0x00130070, 0x57050220, 0x52465505, 0x00442c06,
    0x275c0070, 0x34005a03, 0x00041c69, 0x4f05a660,
    0x02462805, 0x00000003, 0xa05e1a40, 0x37025c02,
    0x20531a66, 0x51004f03, 0x00031a61, 0x18260220,
    0x00345e05, 0x00000000, 0x00131b61, 0x1a260220,
    0x00345f05, 0x00000000, 0x00041b52, 0x58040e68,
    0x0e2e7705, 0x56055305, 0x00031961, 0x21260220,
    0x00345805, 0x00000000, 0x00131a61, 0x23260220,
    0x00345905, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044331, 0x59240000,
    0xfb042124, 0x000c0000, 0x00032361, 0x14060220,
    0x00345905, 0x00000000, 0x00132361, 0x16060220,
    0x00345a05, 0x00000000, 0x0003a361, 0x14260220,
    0x00345b05, 0x00000000, 0x0013a361, 0x16260220,
    0x00345c05, 0x00000000, 0xe260004c, 0x00114004,
    0x80001969, 0x10018220, 0x02006004, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000200,
    0x80000961, 0x3c060660, 0x00010200, 0x00000000,
    0x80000061, 0x3c260660, 0x00010210, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x80031961, 0x22260220, 0x00003c24, 0x00000000,
    0x80031961, 0x22060220, 0x00003c04, 0x00000000,
    0xe2440061, 0x00113004, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x8a004031, 0x32140000,
    0xfb00220c, 0x00340000, 0x80001a61, 0x30010220,
    0x00004404, 0x00000000, 0x80001e69, 0x10018220,
    0x02006004, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000200, 0x80000961, 0x3d060660,
    0x00010400, 0x00000000, 0x80000061, 0x3d260660,
    0x00010410, 0x00000000, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80031a61, 0x2e260220,
    0x00003d24, 0x00000000, 0x80031961, 0x2e060220,
    0x00003d04, 0x00000000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x8a004b31, 0x00000000,
    0xfb082e0c, 0x00343214, 0xa05f0040, 0x0c006703,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80001f01, 0x00000000, 0x00000000, 0x00000000,
    0x80003e01, 0x00000000, 0x00000000, 0x00000000,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x80003d01, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0xe0610068, 0x00605f03, 0xa0790040, 0x5f003402,
    0xa0631a40, 0x61000302, 0x80003f01, 0x00000000,
    0x00000000, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x00030061, 0x1d060220,
    0x00347905, 0x00000000, 0x80103701, 0x00000000,
    0x00000000, 0x00000000, 0x80103d01, 0x00000000,
    0x00000000, 0x00000000, 0x00131b61, 0x1f060220,
    0x00347a05, 0x00000000, 0x00041b69, 0x3c058660,
    0x02466305, 0x00000003, 0xe0400068, 0x01d06303,
    0xa1591a40, 0x3c0e2602, 0xaa711b40, 0x3d0e2c02,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x22060220, 0x00345905, 0x00000000,
    0x80103301, 0x00000000, 0x00000000, 0x00000000,
    0x00131a61, 0x24060220, 0x00347105, 0x00000000,
    0x27670070, 0x03006303, 0x00030070, 0x72050220,
    0x52465905, 0x00442606, 0x00130070, 0x73050220,
    0x52467105, 0x00442c06, 0x277b0070, 0x34007903,
    0x80001c01, 0x00000000, 0x00000000, 0x00000000,
    0x00040069, 0x3e05a660, 0x02466705, 0x00000003,
    0xa07d1a40, 0x37027b02, 0x20421a66, 0x40003e03,
    0x00031a61, 0x1d260220, 0x00347d05, 0x00000000,
    0x00131b61, 0x1f260220, 0x00347e05, 0x00000000,
    0x00041b52, 0x74040e68, 0x0e2e7705, 0x72054205,
    0x00031961, 0x22260220, 0x00347405, 0x00000000,
    0x00131a61, 0x24260220, 0x00347505, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044631, 0x75240000, 0xfb042224, 0x000c0000,
    0x00032661, 0x19060220, 0x00347505, 0x00000000,
    0x80103e01, 0x00000000, 0x00000000, 0x00000000,
    0x00132661, 0x1b060220, 0x00347605, 0x00000000,
    0x0003a661, 0x19260220, 0x00347705, 0x00000000,
    0x0013a661, 0x1b260220, 0x00347805, 0x00000000,
    0xe205004c, 0x00114004, 0x80001969, 0x10018220,
    0x02000504, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000200, 0x80000961, 0x3e060660,
    0x00010480, 0x00000000, 0x80000061, 0x3e260660,
    0x00010490, 0x00000000, 0x80031961, 0x69260220,
    0x00003e24, 0x00000000, 0x80031961, 0x69060220,
    0x00003e04, 0x00000000, 0xe2700061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004c31, 0x6e140000, 0xfb00690c, 0x00340000,
    0x80001a61, 0x30010220, 0x00007004, 0x00000000,
    0x80001e69, 0x10018220, 0x02000504, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000200,
    0x80000961, 0x3f060660, 0x00010680, 0x00000000,
    0x80000061, 0x3f260660, 0x00010690, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80031a61, 0x6a260220, 0x00003f24, 0x00000000,
    0x80031961, 0x6a060220, 0x00003f04, 0x00000000,
    0x80002c01, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004a31, 0x00000000, 0xfb086a0c, 0x00346e14,
    0x00040052, 0x4b044160, 0x0e0e0003, 0x4b053a05,
    0x00040027, 0x00014060, 0x00000000, 0xfffff0c8,
    0x80030061, 0x7e050220, 0x00460005, 0x00000000,
    0x80040931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_misc_copy_instance_ptrs_base_ptr_indirect = {
   .prog_data = {
      .base.nr_params = 14,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 5456,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_misc_copy_instance_ptrs_base_ptr_indirect_relocs,
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
   .args = gfx125_misc_copy_instance_ptrs_base_ptr_indirect_args,
   .code = gfx125_misc_copy_instance_ptrs_base_ptr_indirect_code,
};
const char *gfx125_misc_copy_instance_ptrs_base_ptr_indirect_sha1 = "37640b4b1a14751f2a3d07b8ded479c95b64788f";
