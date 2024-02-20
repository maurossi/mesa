#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_misc_copy_instance_ptrs_indirect_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_misc_copy_instance_ptrs_indirect_args[] = {
   { 0, 8 },
   { 8, 8 },
   { 16, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g95<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g105<1>UD       g0.1<0,1,0>UD                   { align1 1H };
add(1)          g96<1>UD        g95<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@2 compacted };
mov(8)          g42<1>UW        0x76543210V                     { align1 WE_all 1Q };
shl(16)         g52<1>D         g105<8,8,1>D    0x00000004UD    { align1 1H I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(1)         g1UD            g96UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
add(8)          g42.8<1>UW      g42<1,1,0>UW    0x0008UW        { align1 WE_all 1Q I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         g50<1>D         g42<8,8,1>UW                    { align1 1H };
add.z.f0.0(16)  null<1>D        g50<8,8,1>D     g52<8,8,1>D     { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g43.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g45.1<2>F       g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g40.1<2>F       g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g47.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g54.1<2>F       g2.5<0,1,0>F                    { align1 1Q };
mov(8)          g56.1<2>F       g2.5<0,1,0>F                    { align1 2Q };
mov(8)          g43<2>F         g2<0,1,0>F                      { align1 1Q F@6 compacted };
mov(8)          g45<2>F         g2<0,1,0>F                      { align1 2Q F@6 compacted };
mov(8)          g40<2>F         g2.2<0,1,0>F                    { align1 1Q F@6 compacted };
mov(8)          g47<2>F         g2.2<0,1,0>F                    { align1 2Q F@6 compacted };
mov(8)          g54<2>F         g2.4<0,1,0>F                    { align1 1Q F@6 compacted };
mov(8)          g56<2>F         g2.4<0,1,0>F                    { align1 2Q F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
send(16)        g1UD            g54UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
shl(16)         g49<1>D         g1<8,8,1>D      0x00000006UD    { align1 1H $1.dst };
(+f0.0) if(16)  JIP:  LABEL0          UIP:  LABEL0              { align1 1H };
add(8)          g53<1>D         g43<8,4,2>D     156D            { align1 1Q F@6 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $1.src };
add(8)          g54<1>D         g45<8,4,2>D     156D            { align1 2Q F@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
cmp.l.f0.0(16)  g55<1>UD        g53<1,1,0>UD    0x0000009cUD    { align1 1H I@1 compacted };
mov(8)          g57<2>UD        g53<4,4,1>UD                    { align1 1Q $1.src };
mov(8)          g59<2>UD        g54<4,4,1>UD                    { align1 2Q };
add(8)          g97<1>D         -g55<8,8,1>D    g43.1<8,4,2>D   { align1 1Q I@3 };
add(8)          g56<1>D         -g56<8,8,1>D    g45.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g57.1<2>UD      g97<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g59.1<2>UD      g56<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g57UD           g1UD            0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $1 };

LABEL0:
endif(16)       JIP:  LABEL1                                    { align1 1H };

LABEL1:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
shr(16)         g57<1>UD        g49<1,1,0>UD    0x00000008UD    { align1 1H I@3 compacted };
mov(16)         g38<1>UD        g105<8,8,1>UD                   { align1 1H };
and(16)         g59<1>UD        g49<8,8,1>UD    0xffffff00UD    { align1 1H $1.src };
add3(16)        g67<1>D         65534W          -g57<8,8,1>D    g105<1,1,1>D { align1 1H I@3 };
add(16)         g61<1>D         g49<1,1,0>D     -g59<1,1,0>D    { align1 1H I@2 compacted };
mov(16)         g69<1>D         -g67<8,8,1>D                    { align1 1H I@2 };
and(16)         g63<1>UD        g61<8,8,1>UD    0xffffffc0UD    { align1 1H I@2 };
shr(16)         g65<1>UD        g63<1,1,0>UD    0x00000006UD    { align1 1H I@1 compacted };
cmp.l.f0.0(16)  null<1>UD       g69<8,8,1>UD    g65<8,8,1>UD    { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL2          UIP:  LABEL2              { align1 1H };
shl(16)         g70<1>D         -g67<8,8,1>D    0x00000006UD    { align1 1H };
mov(8)          g88<1>UD        g40.1<8,4,2>UD                  { align1 1Q F@4 };
mov(8)          g89<1>UD        g47.1<8,4,2>UD                  { align1 2Q F@3 };
add(16)         g72<1>D         g59<1,1,0>D     g70<1,1,0>D     { align1 1H I@3 compacted };
shr(16)         g74<1>UD        g72<1,1,0>UD    0x00000006UD    { align1 1H I@1 compacted };
add(8)          g99<1>D         g43<8,4,2>D     g72<1,1,0>D     { align1 1Q F@6 compacted };
add(8)          g95<1>D         g45<8,4,2>D     g73<1,1,0>D     { align1 2Q F@5 compacted };
add(16)         g76<1>D         g3<1,1,0>D      g74<1,1,0>D     { align1 1H @3 $1.dst compacted };
mov(8)          g9<2>UD         g99<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g11<2>UD        g95<4,4,1>UD                    { align1 2Q I@3 };
cmp.l.f0.0(16)  g78<1>UD        g76<1,1,0>UD    g3<1,1,0>UD     { align1 1H I@3 compacted };
shl(16)         g80<1>D         g76<8,8,1>D     0x00000003UD    { align1 1H };
shr(16)         g84<1>UD        g76<1,1,0>UD    0x0000001dUD    { align1 1H compacted };
shl(16)         g82<1>D         -g78<8,8,1>D    0x00000003UD    { align1 1H I@3 };
add(8)          g98<1>D         g40<8,4,2>D     g80<1,1,0>D     { align1 1Q I@3 compacted };
add(8)          g90<1>D         g47<8,4,2>D     g81<1,1,0>D     { align1 2Q I@4 compacted };
or(16)          g86<1>UD        g82<1,1,0>UD    g84<1,1,0>UD    { align1 1H I@3 compacted };
cmp.l.f0.0(8)   g91<1>UD        g98<8,8,1>UD    g40<8,4,2>UD    { align1 1Q I@3 };
mov(8)          g30<2>UD        g98<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g92<1>UD        g90<8,8,1>UD    g47<8,4,2>UD    { align1 2Q I@4 };
mov(8)          g32<2>UD        g90<4,4,1>UD                    { align1 2Q };
cmp.l.f0.0(8)   g100<1>UD       g99<8,8,1>UD    g43<8,4,2>UD    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
cmp.l.f0.0(8)   g96<1>UD        g95<8,8,1>UD    g45<8,4,2>UD    { align1 2Q };
add3(16)        g93<1>D         g88<8,8,1>D     g86<8,8,1>D     -g91<1,1,1>D { align1 1H I@4 };
add(8)          g101<1>D        -g100<8,8,1>D   g43.1<8,4,2>D   { align1 1Q I@3 };
add(8)          g97<1>D         -g96<8,8,1>D    g45.1<8,4,2>D   { align1 2Q I@3 };
mov(8)          g30.1<2>UD      g93<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g32.1<2>UD      g94<4,4,1>UD                    { align1 2Q I@4 };
mov(8)          g9.1<2>UD       g101<4,4,1>UD                   { align1 1Q I@4 };
mov(8)          g11.1<2>UD      g97<4,4,1>UD                    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g94UD           g30UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
mov(8)          g5<2>UD         g94<4,4,1>UD                    { align1 1Q $2.dst };
mov(8)          g7<2>UD         g95<4,4,1>UD                    { align1 2Q $2.dst };
mov(8)          g5.1<2>UD       g96<4,4,1>UD                    { align1 1Q @2 $2.dst };
mov(8)          g7.1<2>UD       g97<4,4,1>UD                    { align1 2Q @2 $2.dst };
fbl(1)          g106<1>UD       mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g106<0,1,0>UD   0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g65<2>D         g[a0 160]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g65.1<2>D       g[a0 164]<0,1,0>D               { align1 WE_all 1N };
mov(8)          g61.1<2>UD      g65.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g61<2>UD        g65<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g96<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g66UD    g61UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $3 };
mov(1)          f0<1>UD         g96<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g106<0,1,0>UD   0x00000003UD    { align1 WE_all 1N I@6 };
mov(1)          g68<2>D         g[a0 288]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g68.1<2>D       g[a0 292]<0,1,0>D               { align1 WE_all 1N };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
mov(8)          g62.1<2>UD      g68.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g62<2>UD        g68<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g62UD           g66UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $4 };

LABEL2:
endif(16)       JIP:  LABEL3                                    { align1 1H };

LABEL3:
shr(16)         g98<1>UD        g59<1,1,0>UD    0x00000008UD    { align1 1H compacted };

LABEL5:
cmp.ge.f0.0(16) null<1>UD       g38<8,8,1>UD    g98<8,8,1>UD    { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL4        UIP:  LABEL4              { align1 1H };
shl(16)         g100<1>D        g38<8,8,1>D     0x00000008UD    { align1 1H };
mov(8)          g116<1>UD       g40.1<8,4,2>UD                  { align1 1Q F@4 };
mov(8)          g117<1>UD       g47.1<8,4,2>UD                  { align1 2Q F@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
shr(16)         g102<1>UD       g100<1,1,0>UD   0x00000006UD    { align1 1H I@3 compacted };
add(8)          g121<1>D        g43<8,4,2>D     g100<1,1,0>D    { align1 1Q F@6 compacted };
add(8)          g123<1>D        g45<8,4,2>D     g101<1,1,0>D    { align1 2Q F@5 compacted };
add(16)         g104<1>D        g3<1,1,0>D      g102<1,1,0>D    { align1 1H @3 $1.dst compacted };
mov(8)          g14<2>UD        g121<4,4,1>UD                   { align1 1Q I@3 };
mov(8)          g16<2>UD        g123<4,4,1>UD                   { align1 2Q I@3 };
cmp.l.f0.0(16)  g106<1>UD       g104<1,1,0>UD   g3<1,1,0>UD     { align1 1H I@3 compacted };
shl(16)         g108<1>D        g104<8,8,1>D    0x00000003UD    { align1 1H };
shr(16)         g112<1>UD       g104<1,1,0>UD   0x0000001dUD    { align1 1H compacted };
shl(16)         g110<1>D        -g106<8,8,1>D   0x00000003UD    { align1 1H I@3 };
add(8)          g102<1>D        g40<8,4,2>D     g108<1,1,0>D    { align1 1Q I@3 compacted };
add(8)          g118<1>D        g47<8,4,2>D     g109<1,1,0>D    { align1 2Q I@4 compacted };
or(16)          g114<1>UD       g110<1,1,0>UD   g112<1,1,0>UD   { align1 1H I@3 compacted };
cmp.l.f0.0(8)   g119<1>UD       g102<8,8,1>UD   g40<8,4,2>UD    { align1 1Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g31<2>UD        g102<4,4,1>UD                   { align1 1Q };
cmp.l.f0.0(8)   g120<1>UD       g118<8,8,1>UD   g47<8,4,2>UD    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $5.src };
mov(8)          g33<2>UD        g118<4,4,1>UD                   { align1 2Q $2.src };
cmp.l.f0.0(8)   g125<1>UD       g121<8,8,1>UD   g43<8,4,2>UD    { align1 1Q };
cmp.l.f0.0(8)   g124<1>UD       g123<8,8,1>UD   g45<8,4,2>UD    { align1 2Q };
add3(16)        g121<1>D        g116<8,8,1>D    g114<8,8,1>D    -g119<1,1,1>D { align1 1H I@4 };
add(8)          g5<1>D          -g125<8,8,1>D   g43.1<8,4,2>D   { align1 1Q I@3 };
add(8)          g125<1>D        -g124<8,8,1>D   g45.1<8,4,2>D   { align1 2Q I@3 };
mov(8)          g31.1<2>UD      g121<4,4,1>UD                   { align1 1Q I@3 };
mov(8)          g33.1<2>UD      g122<4,4,1>UD                   { align1 2Q I@4 };
mov(8)          g14.1<2>UD      g5<4,4,1>UD                     { align1 1Q I@4 };
mov(8)          g16.1<2>UD      g125<4,4,1>UD                   { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g122UD          g31UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $6 };
mov(8)          g10<2>UD        g122<4,4,1>UD                   { align1 1Q $6.dst };
mov(8)          g12<2>UD        g123<4,4,1>UD                   { align1 2Q $6.dst };
mov(8)          g10.1<2>UD      g124<4,4,1>UD                   { align1 1Q @2 $6.dst };
mov(8)          g12.1<2>UD      g125<4,4,1>UD                   { align1 2Q @2 $6.dst };
fbl(1)          g126<1>UD       mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g126<0,1,0>UD   0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g71<2>D         g[a0 320]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g71.1<2>D       g[a0 324]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
mov(8)          g67.1<2>UD      g71.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g67<2>UD        g71<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g95<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g72UD    g67UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $7 };
mov(1)          f0<1>UD         g95<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g126<0,1,0>UD   0x00000003UD    { align1 WE_all 1N I@6 };
mov(1)          g74<2>D         g[a0 448]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g74.1<2>D       g[a0 452]<0,1,0>D               { align1 WE_all 1N };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
mov(8)          g68.1<2>UD      g74.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g68<2>UD        g74<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g68UD           g72UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $8 };
add(16)         g126<1>D        g100<1,1,0>D    64D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
shr(16)         g5<1>UD         g126<1,1,0>UD   0x00000006UD    { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(8)          g30<1>D         g43<8,4,2>D     g126<1,1,0>D    { align1 1Q compacted };
add(8)          g50<1>D         g45<8,4,2>D     g127<1,1,0>D    { align1 2Q compacted };
add(16)         g7<1>D          g3<1,1,0>D      g5<1,1,0>D      { align1 1H I@3 compacted };
mov(8)          g19<2>UD        g30<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g21<2>UD        g50<4,4,1>UD                    { align1 2Q I@3 };
shl(16)         g11<1>D         g7<8,8,1>D      0x00000003UD    { align1 1H I@3 };
shr(16)         g23<1>UD        g7<1,1,0>UD     0x0000001dUD    { align1 1H compacted };
add(8)          g6<1>D          g40<8,4,2>D     g11<1,1,0>D     { align1 1Q I@2 compacted };
add(8)          g27<1>D         g47<8,4,2>D     g12<1,1,0>D     { align1 2Q I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
mov(8)          g32<2>UD        g6<4,4,1>UD                     { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $6.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $5.src };
mov(8)          g34<2>UD        g27<4,4,1>UD                    { align1 2Q I@2 };
cmp.l.f0.0(16)  g9<1>UD         g7<1,1,0>UD     g3<1,1,0>UD     { align1 1H compacted };
cmp.l.f0.0(8)   g28<1>UD        g6<8,8,1>UD     g40<8,4,2>UD    { align1 1Q };
cmp.l.f0.0(8)   g29<1>UD        g27<8,8,1>UD    g47<8,4,2>UD    { align1 2Q };
cmp.l.f0.0(8)   g42<1>UD        g30<8,8,1>UD    g43<8,4,2>UD    { align1 1Q };
cmp.l.f0.0(8)   g51<1>UD        g50<8,8,1>UD    g45<8,4,2>UD    { align1 2Q };
shl(16)         g13<1>D         -g9<8,8,1>D     0x00000003UD    { align1 1H I@5 };
add(8)          g49<1>D         -g42<8,8,1>D    g43.1<8,4,2>D   { align1 1Q I@3 };
add(8)          g52<1>D         -g51<8,8,1>D    g45.1<8,4,2>D   { align1 2Q I@3 };
or(16)          g25<1>UD        g13<1,1,0>UD    g23<1,1,0>UD    { align1 1H I@3 compacted };
mov(8)          g19.1<2>UD      g49<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g21.1<2>UD      g52<4,4,1>UD                    { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
add3(16)        g30<1>D         g116<8,8,1>D    g25<8,8,1>D     -g28<1,1,1>D { align1 1H I@3 };
mov(8)          g32.1<2>UD      g30<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g34.1<2>UD      g31<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g49UD           g32UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $9 };
mov(8)          g15<2>UD        g49<4,4,1>UD                    { align1 1Q $9.dst };
mov(8)          g17<2>UD        g50<4,4,1>UD                    { align1 2Q $9.dst };
mov(8)          g15.1<2>UD      g51<4,4,1>UD                    { align1 1Q @2 $9.dst };
mov(8)          g17.1<2>UD      g52<4,4,1>UD                    { align1 2Q @2 $9.dst };
fbl(1)          g36<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N $5.src compacted };
shl(1)          a0<1>UD         g36<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
mov(1)          g77<2>D         g[a0 480]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g77.1<2>D       g[a0 484]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
mov(8)          g73.1<2>UD      g77.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g73<2>UD        g77<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g94<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g78UD    g73UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $10 };
mov(1)          f0<1>UD         g94<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g36<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000200UD    { align1 WE_all 1N A@1 };
mov(1)          g80<2>D         g[a0 96]<0,1,0>D                { align1 WE_all 1N A@1 };
mov(1)          g80.1<2>D       g[a0 100]<0,1,0>D               { align1 WE_all 1N };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
mov(8)          g74.1<2>UD      g80.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g74<2>UD        g80<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g74UD           g78UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $11 };
add(16)         g53<1>D         g100<1,1,0>D    128D            { align1 1H $1.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
shr(16)         g55<1>UD        g53<1,1,0>UD    0x00000006UD    { align1 1H I@1 compacted };
add(8)          g52<1>D         g43<8,4,2>D     g53<1,1,0>D     { align1 1Q compacted };
add(8)          g76<1>D         g45<8,4,2>D     g54<1,1,0>D     { align1 2Q compacted };
add(16)         g59<1>D         g3<1,1,0>D      g55<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g24<2>UD        g52<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g26<2>UD        g76<4,4,1>UD                    { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
shl(16)         g63<1>D         g59<8,8,1>D     0x00000003UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
shr(16)         g67<1>UD        g59<1,1,0>UD    0x0000001dUD    { align1 1H compacted };
add(8)          g51<1>D         g40<8,4,2>D     g63<1,1,0>D     { align1 1Q I@2 compacted };
add(8)          g71<1>D         g47<8,4,2>D     g64<1,1,0>D     { align1 2Q I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $9.src };
mov(8)          g33<2>UD        g51<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $9.src };
mov(8)          g35<2>UD        g71<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
cmp.l.f0.0(16)  g61<1>UD        g59<1,1,0>UD    g3<1,1,0>UD     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
cmp.l.f0.0(8)   g72<1>UD        g51<8,8,1>UD    g40<8,4,2>UD    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $10.src };
cmp.l.f0.0(8)   g73<1>UD        g71<8,8,1>UD    g47<8,4,2>UD    { align1 2Q };
cmp.l.f0.0(8)   g53<1>UD        g52<8,8,1>UD    g43<8,4,2>UD    { align1 1Q };
cmp.l.f0.0(8)   g77<1>UD        g76<8,8,1>UD    g45<8,4,2>UD    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
shl(16)         g65<1>D         -g61<8,8,1>D    0x00000003UD    { align1 1H I@5 };
add(8)          g54<1>D         -g53<8,8,1>D    g43.1<8,4,2>D   { align1 1Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $11.src };
add(8)          g78<1>D         -g77<8,8,1>D    g45.1<8,4,2>D   { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
or(16)          g69<1>UD        g65<1,1,0>UD    g67<1,1,0>UD    { align1 1H compacted };
mov(8)          g24.1<2>UD      g54<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g26.1<2>UD      g78<4,4,1>UD                    { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
add3(16)        g74<1>D         g116<8,8,1>D    g69<8,8,1>D     -g72<1,1,1>D { align1 1H };
mov(8)          g33.1<2>UD      g74<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g35.1<2>UD      g75<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g75UD           g33UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
mov(8)          g20<2>UD        g75<4,4,1>UD                    { align1 1Q $2.dst };
mov(8)          g22<2>UD        g76<4,4,1>UD                    { align1 2Q $2.dst };
mov(8)          g20.1<2>UD      g77<4,4,1>UD                    { align1 1Q @2 $2.dst };
mov(8)          g22.1<2>UD      g78<4,4,1>UD                    { align1 2Q @2 $2.dst };
fbl(1)          g49<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
shl(1)          a0<1>UD         g49<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000200UD    { align1 WE_all 1N A@1 };
mov(1)          g83<2>D         g[a0 128]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g83.1<2>D       g[a0 132]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mov(8)          g79.1<2>UD      g83.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g79<2>UD        g83<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g93<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g84UD    g79UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $14 };
mov(1)          f0<1>UD         g93<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g49<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000200UD    { align1 WE_all 1N A@1 };
mov(1)          g86<2>D         g[a0 256]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g86.1<2>D       g[a0 260]<0,1,0>D               { align1 WE_all 1N };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
mov(8)          g80.1<2>UD      g86.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g80<2>UD        g86<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g80UD           g84UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $15 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
add(16)         g79<1>D         g100<1,1,0>D    192D            { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
shr(16)         g81<1>UD        g79<1,1,0>UD    0x00000006UD    { align1 1H compacted };
add(8)          g56<1>D         g43<8,4,2>D     g79<1,1,0>D     { align1 1Q compacted };
add(8)          g102<1>D        g45<8,4,2>D     g80<1,1,0>D     { align1 2Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
add(16)         g83<1>D         g3<1,1,0>D      g81<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g29<2>UD        g56<4,4,1>UD                    { align1 1Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $9.src };
mov(8)          g31<2>UD        g102<4,4,1>UD                   { align1 2Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
shl(16)         g87<1>D         g83<8,8,1>D     0x00000003UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
shr(16)         g91<1>UD        g83<1,1,0>UD    0x0000001dUD    { align1 1H compacted };
add(8)          g55<1>D         g40<8,4,2>D     g87<1,1,0>D     { align1 1Q I@2 compacted };
add(8)          g95<1>D         g47<8,4,2>D     g88<1,1,0>D     { align1 2Q I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g34<2>UD        g55<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $5.src };
mov(8)          g36<2>UD        g95<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
cmp.l.f0.0(16)  g85<1>UD        g83<1,1,0>UD    g3<1,1,0>UD     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
cmp.l.f0.0(8)   g96<1>UD        g55<8,8,1>UD    g40<8,4,2>UD    { align1 1Q };
cmp.l.f0.0(8)   g97<1>UD        g95<8,8,1>UD    g47<8,4,2>UD    { align1 2Q };
cmp.l.f0.0(8)   g59<1>UD        g56<8,8,1>UD    g43<8,4,2>UD    { align1 1Q };
cmp.l.f0.0(8)   g103<1>UD       g102<8,8,1>UD   g45<8,4,2>UD    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@5 };
shl(16)         g89<1>D         -g85<8,8,1>D    0x00000003UD    { align1 1H };
add(8)          g60<1>D         -g59<8,8,1>D    g43.1<8,4,2>D   { align1 1Q I@3 };
add(8)          g104<1>D        -g103<8,8,1>D   g45.1<8,4,2>D   { align1 2Q I@3 };
or(16)          g93<1>UD        g89<1,1,0>UD    g91<1,1,0>UD    { align1 1H I@3 compacted };
mov(8)          g29.1<2>UD      g60<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g31.1<2>UD      g104<4,4,1>UD                   { align1 2Q I@3 };
add3(16)        g100<1>D        g116<8,8,1>D    g93<8,8,1>D     -g96<1,1,1>D { align1 1H I@3 };
mov(8)          g34.1<2>UD      g100<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g36.1<2>UD      g101<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g101UD          g34UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $5 };
mov(8)          g25<2>UD        g101<4,4,1>UD                   { align1 1Q $5.dst };
mov(8)          g27<2>UD        g102<4,4,1>UD                   { align1 2Q $5.dst };
mov(8)          g25.1<2>UD      g103<4,4,1>UD                   { align1 1Q @2 $5.dst };
mov(8)          g27.1<2>UD      g104<4,4,1>UD                   { align1 2Q @2 $5.dst };
fbl(1)          g50<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
shl(1)          a0<1>UD         g50<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@1 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000200UD    { align1 WE_all 1N A@1 };
mov(1)          g93<2>D         g[a0 288]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g93.1<2>D       g[a0 292]<0,1,0>D               { align1 WE_all 1N };
mov(8)          g85.1<2>UD      g93.1<0,1,0>UD                  { align1 WE_all 1Q I@1 };
mov(8)          g85<2>UD        g93<0,1,0>UD                    { align1 WE_all 1Q I@1 };
mov(1)          g92<1>UD        f0<0,1,0>UD                     { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
(+f0.0.any16h) send(1) g90UD    g85UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $12 };
mov(1)          f0<1>UD         g92<0,1,0>UD                    { align1 WE_all 1N I@2 };
shl(1)          a0<1>UD         g50<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@6 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000200UD    { align1 WE_all 1N A@1 };
mov(1)          g94<2>D         g[a0 416]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g94.1<2>D       g[a0 420]<0,1,0>D               { align1 WE_all 1N };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
mov(8)          g86.1<2>UD      g94.1<0,1,0>UD                  { align1 WE_all 1Q I@2 };
mov(8)          g86<2>UD        g94<0,1,0>UD                    { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g86UD           g90UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $13 };
add3(16)        g38<1>D         0x0003UW        g57<8,8,1>D     g38<1,1,1>D { align1 1H };

LABEL4:
while(16)       JIP:  LABEL5                                    { align1 1H };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_misc_copy_instance_ptrs_indirect_code[] = {
    0x80000065, 0x5f058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x69050220, 0x00000024, 0x00000000,
    0xe2601a40, 0x00015f03, 0x80030061, 0x2a054410,
    0x00000000, 0x76543210, 0x00041b69, 0x34058660,
    0x02466905, 0x00000004, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa00600c, 0x00340000, 0x642a1a40, 0x00802a95,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00040061, 0x32050160, 0x00462a05, 0x00000000,
    0x00041940, 0x00010660, 0x16463205, 0x00463405,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x212b0061, 0x001102cc, 0x2a2d0061, 0x001102cc,
    0x00030061, 0x28260aa0, 0x00000264, 0x00000000,
    0x00130061, 0x2f260aa0, 0x00000264, 0x00000000,
    0x00030061, 0x36260aa0, 0x000002a4, 0x00000000,
    0x00130061, 0x38260aa0, 0x000002a4, 0x00000000,
    0x212b1661, 0x00110204, 0x2a2d1661, 0x00110204,
    0x21281661, 0x00110244, 0x2a2f1661, 0x00110244,
    0x21361661, 0x0011025c, 0x2a381661, 0x0011025c,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x80001101, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x01240000, 0xfb043624, 0x000c0000,
    0x00042169, 0x31058660, 0x02460105, 0x00000006,
    0x01040022, 0x0001c060, 0x000000c8, 0x000000c8,
    0xa1351640, 0x09ce2b03, 0x80103101, 0x00000000,
    0x00000000, 0x00000000, 0xaa361540, 0x09ce2d03,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0xe7371970, 0x09c03503, 0x00033161, 0x39060220,
    0x00343505, 0x00000000, 0x00130061, 0x3b060220,
    0x00343605, 0x00000000, 0x00031b40, 0x61052660,
    0x06463705, 0x00442b26, 0x00131c40, 0x38052660,
    0x06463805, 0x00442d26, 0x00031a61, 0x39260220,
    0x00346105, 0x00000000, 0x00131a61, 0x3b260220,
    0x00343805, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044131, 0x00000000,
    0xfb0c3924, 0x00040114, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0xe0391b68, 0x00803103,
    0x00040061, 0x26050220, 0x00466905, 0x00000000,
    0x00043165, 0x3b058220, 0x02463105, 0xffffff00,
    0x00041b52, 0x43044560, 0x0e8efffe, 0x69053905,
    0xa03d1a40, 0x3b203102, 0x00041a61, 0x45052660,
    0x00464305, 0x00000000, 0x00041a65, 0x3f058220,
    0x02463d05, 0xffffffc0, 0xe0411968, 0x00603f03,
    0x00041970, 0x00010220, 0x52464505, 0x00464105,
    0x01040022, 0x0001c060, 0x00000340, 0x00000340,
    0x00040069, 0x4605a660, 0x02464305, 0x00000006,
    0x00031461, 0x58050220, 0x00442826, 0x00000000,
    0x00131361, 0x59050220, 0x00442f26, 0x00000000,
    0xa0481b40, 0x46003b02, 0xe04a1968, 0x00604803,
    0xa1631640, 0x480e2b02, 0xaa5f1540, 0x490e2d02,
    0xa04cb140, 0x4a000302, 0x00031b61, 0x09060220,
    0x00346305, 0x00000000, 0x00131b61, 0x0b060220,
    0x00345f05, 0x00000000, 0x274e1b70, 0x03004c03,
    0x00040069, 0x50058660, 0x02464c05, 0x00000003,
    0xe0540068, 0x01d04c03, 0x00041b69, 0x5205a660,
    0x02464e05, 0x00000003, 0xa1621b40, 0x500e2802,
    0xaa5a1c40, 0x510e2f02, 0x20561b66, 0x54005203,
    0x00031b70, 0x5b050220, 0x52466205, 0x00442806,
    0x00030061, 0x1e060220, 0x00346205, 0x00000000,
    0x00131c70, 0x5c050220, 0x52465a05, 0x00442f06,
    0x00130061, 0x20060220, 0x00345a05, 0x00000000,
    0x00030070, 0x64050220, 0x52466305, 0x00442b06,
    0x80103001, 0x00000000, 0x00000000, 0x00000000,
    0x00130070, 0x60050220, 0x52465f05, 0x00442d06,
    0x00041c52, 0x5d040e68, 0x0e2e5805, 0x5b055605,
    0x00031b40, 0x65052660, 0x06466405, 0x00442b26,
    0x00131b40, 0x61052660, 0x06466005, 0x00442d26,
    0x00031b61, 0x1e260220, 0x00345d05, 0x00000000,
    0x00131c61, 0x20260220, 0x00345e05, 0x00000000,
    0x00031c61, 0x09260220, 0x00346505, 0x00000000,
    0x00131c61, 0x0b260220, 0x00346105, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x5e240000, 0xfb041e24, 0x000c0000,
    0x00032261, 0x05060220, 0x00345e05, 0x00000000,
    0x00132261, 0x07060220, 0x00345f05, 0x00000000,
    0x0003a261, 0x05260220, 0x00346005, 0x00000000,
    0x0013a261, 0x07260220, 0x00346105, 0x00000000,
    0xe26a004c, 0x00114004, 0x80001969, 0x10018220,
    0x02006a04, 0x00000003, 0x80000961, 0x41060660,
    0x00010280, 0x00000000, 0x80000061, 0x41260660,
    0x00010290, 0x00000000, 0x80031961, 0x3d260220,
    0x00004124, 0x00000000, 0x80031961, 0x3d060220,
    0x00004104, 0x00000000, 0xe2600061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004331, 0x42140000, 0xfb003d0c, 0x00340000,
    0x80001a61, 0x30010220, 0x00006004, 0x00000000,
    0x80001e69, 0x10018220, 0x02006a04, 0x00000003,
    0x80000961, 0x44060660, 0x00010480, 0x00000000,
    0x80000061, 0x44260660, 0x00010490, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80031a61, 0x3e260220, 0x00004424, 0x00000000,
    0x80031961, 0x3e060220, 0x00004404, 0x00000000,
    0x80002301, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004431, 0x00000000, 0xfb083e0c, 0x00344214,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0xe0620068, 0x00803b03, 0x00041970, 0x00010220,
    0x42462605, 0x00466205, 0x01040028, 0x0001c660,
    0x00000f18, 0x00000f18, 0x00040069, 0x64058660,
    0x02462605, 0x00000008, 0x00031461, 0x74050220,
    0x00442826, 0x00000000, 0x00131361, 0x75050220,
    0x00442f26, 0x00000000, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0xe0661b68, 0x00606403,
    0xa1791640, 0x640e2b02, 0xaa7b1540, 0x650e2d02,
    0xa068b140, 0x66000302, 0x00031b61, 0x0e060220,
    0x00347905, 0x00000000, 0x00131b61, 0x10060220,
    0x00347b05, 0x00000000, 0x276a1b70, 0x03006803,
    0x00040069, 0x6c058660, 0x02466805, 0x00000003,
    0xe0700068, 0x01d06803, 0x00041b69, 0x6e05a660,
    0x02466a05, 0x00000003, 0xa1661b40, 0x6c0e2802,
    0xaa761c40, 0x6d0e2f02, 0x20721b66, 0x70006e03,
    0x00031b70, 0x77050220, 0x52466605, 0x00442806,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x1f060220, 0x00346605, 0x00000000,
    0x00131c70, 0x78050220, 0x52467605, 0x00442f06,
    0x80103501, 0x00000000, 0x00000000, 0x00000000,
    0x00133261, 0x21060220, 0x00347605, 0x00000000,
    0x00030070, 0x7d050220, 0x52467905, 0x00442b06,
    0x00130070, 0x7c050220, 0x52467b05, 0x00442d06,
    0x00041c52, 0x79040e68, 0x0e2e7405, 0x77057205,
    0x00031b40, 0x05052660, 0x06467d05, 0x00442b26,
    0x00131b40, 0x7d052660, 0x06467c05, 0x00442d26,
    0x00031b61, 0x1f260220, 0x00347905, 0x00000000,
    0x00131c61, 0x21260220, 0x00347a05, 0x00000000,
    0x00031c61, 0x0e260220, 0x00340505, 0x00000000,
    0x00131c61, 0x10260220, 0x00347d05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044631, 0x7a240000, 0xfb041f24, 0x000c0000,
    0x00032661, 0x0a060220, 0x00347a05, 0x00000000,
    0x00132661, 0x0c060220, 0x00347b05, 0x00000000,
    0x0003a661, 0x0a260220, 0x00347c05, 0x00000000,
    0x0013a661, 0x0c260220, 0x00347d05, 0x00000000,
    0xe27e004c, 0x00114004, 0x80001969, 0x10018220,
    0x02007e04, 0x00000003, 0x80000961, 0x47060660,
    0x00010500, 0x00000000, 0x80000061, 0x47260660,
    0x00010510, 0x00000000, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x43260220,
    0x00004724, 0x00000000, 0x80031961, 0x43060220,
    0x00004704, 0x00000000, 0xe25f0061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004731, 0x48140000, 0xfb00430c, 0x00340000,
    0x80001a61, 0x30010220, 0x00005f04, 0x00000000,
    0x80001e69, 0x10018220, 0x02007e04, 0x00000003,
    0x80000961, 0x4a060660, 0x00010700, 0x00000000,
    0x80000061, 0x4a260660, 0x00010710, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80031a61, 0x44260220, 0x00004a24, 0x00000000,
    0x80031961, 0x44060220, 0x00004a04, 0x00000000,
    0x80002701, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004831, 0x00000000, 0xfb08440c, 0x00344814,
    0xa07e0040, 0x04006403, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0xe0051968, 0x00607e03,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0xa11e0040, 0x7e0e2b02, 0xaa320040, 0x7f0e2d02,
    0xa0071b40, 0x05000302, 0x00031b61, 0x13060220,
    0x00341e05, 0x00000000, 0x00131b61, 0x15060220,
    0x00343205, 0x00000000, 0x00041b69, 0x0b058660,
    0x02460705, 0x00000003, 0xe0170068, 0x01d00703,
    0xa1061a40, 0x0b0e2802, 0xaa1b1b40, 0x0c0e2f02,
    0x80003601, 0x00000000, 0x00000000, 0x00000000,
    0x00031a61, 0x20060220, 0x00340605, 0x00000000,
    0x80103601, 0x00000000, 0x00000000, 0x00000000,
    0x80103501, 0x00000000, 0x00000000, 0x00000000,
    0x00131a61, 0x22060220, 0x00341b05, 0x00000000,
    0x27090070, 0x03000703, 0x00030070, 0x1c050220,
    0x52460605, 0x00442806, 0x00130070, 0x1d050220,
    0x52461b05, 0x00442f06, 0x00030070, 0x2a050220,
    0x52461e05, 0x00442b06, 0x00130070, 0x33050220,
    0x52463205, 0x00442d06, 0x00041d69, 0x0d05a660,
    0x02460905, 0x00000003, 0x00031b40, 0x31052660,
    0x06462a05, 0x00442b26, 0x00131b40, 0x34052660,
    0x06463305, 0x00442d26, 0x20191b66, 0x17000d03,
    0x00031b61, 0x13260220, 0x00343105, 0x00000000,
    0x00131b61, 0x15260220, 0x00343405, 0x00000000,
    0x80003601, 0x00000000, 0x00000000, 0x00000000,
    0x00041b52, 0x1e040e68, 0x0e2e7405, 0x1c051905,
    0x00031961, 0x20260220, 0x00341e05, 0x00000000,
    0x00131a61, 0x22260220, 0x00341f05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044931, 0x31240000, 0xfb042024, 0x000c0000,
    0x00032961, 0x0f060220, 0x00343105, 0x00000000,
    0x00132961, 0x11060220, 0x00343205, 0x00000000,
    0x0003a961, 0x0f260220, 0x00343305, 0x00000000,
    0x0013a961, 0x11260220, 0x00343405, 0x00000000,
    0xe224354c, 0x00114004, 0x80001969, 0x10018220,
    0x02002404, 0x00000003, 0x80000961, 0x4d060660,
    0x00010780, 0x00000000, 0x80000061, 0x4d260660,
    0x00010790, 0x00000000, 0x80003801, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x49260220,
    0x00004d24, 0x00000000, 0x80031961, 0x49060220,
    0x00004d04, 0x00000000, 0xe25e0061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004a31, 0x4e140000, 0xfb00490c, 0x00340000,
    0x80001a61, 0x30010220, 0x00005e04, 0x00000000,
    0x80001e69, 0x10018220, 0x02002404, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000200,
    0x80000961, 0x50060660, 0x00010180, 0x00000000,
    0x80000061, 0x50260660, 0x00010190, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80031a61, 0x4a260220, 0x00005024, 0x00000000,
    0x80031961, 0x4a060220, 0x00005004, 0x00000000,
    0x80002a01, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004b31, 0x00000000, 0xfb084a0c, 0x00344e14,
    0xa0353140, 0x08006403, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0xe0371968, 0x00603503,
    0xa1340040, 0x350e2b02, 0xaa4c0040, 0x360e2d02,
    0xa03b1b40, 0x37000302, 0x00031b61, 0x18060220,
    0x00343405, 0x00000000, 0x00131b61, 0x1a060220,
    0x00344c05, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x00040069, 0x3f058660,
    0x02463b05, 0x00000003, 0x80003701, 0x00000000,
    0x00000000, 0x00000000, 0x80003801, 0x00000000,
    0x00000000, 0x00000000, 0xe0430068, 0x01d03b03,
    0xa1331a40, 0x3f0e2802, 0xaa471b40, 0x400e2f02,
    0x80003901, 0x00000000, 0x00000000, 0x00000000,
    0x00031a61, 0x21060220, 0x00343305, 0x00000000,
    0x80103901, 0x00000000, 0x00000000, 0x00000000,
    0x00131a61, 0x23060220, 0x00344705, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x273d0070, 0x03003b03, 0x80003801, 0x00000000,
    0x00000000, 0x00000000, 0x00030070, 0x48050220,
    0x52463305, 0x00442806, 0x80103a01, 0x00000000,
    0x00000000, 0x00000000, 0x00130070, 0x49050220,
    0x52464705, 0x00442f06, 0x00030070, 0x35050220,
    0x52463405, 0x00442b06, 0x00130070, 0x4d050220,
    0x52464c05, 0x00442d06, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x00041d69, 0x4105a660,
    0x02463d05, 0x00000003, 0x00031b40, 0x36052660,
    0x06463505, 0x00442b26, 0x80103b01, 0x00000000,
    0x00000000, 0x00000000, 0x00131b40, 0x4e052660,
    0x06464d05, 0x00442d26, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x20450066, 0x43004103,
    0x00031b61, 0x18260220, 0x00343605, 0x00000000,
    0x00131b61, 0x1a260220, 0x00344e05, 0x00000000,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00040052, 0x4a040e68, 0x0e2e7405, 0x48054505,
    0x00031961, 0x21260220, 0x00344a05, 0x00000000,
    0x00131a61, 0x23260220, 0x00344b05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x4b240000, 0xfb042124, 0x000c0000,
    0x00032261, 0x14060220, 0x00344b05, 0x00000000,
    0x00132261, 0x16060220, 0x00344c05, 0x00000000,
    0x0003a261, 0x14260220, 0x00344d05, 0x00000000,
    0x0013a261, 0x16260220, 0x00344e05, 0x00000000,
    0xe231004c, 0x00114004, 0x80003c01, 0x00000000,
    0x00000000, 0x00000000, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x80001969, 0x10018220,
    0x02003104, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000200, 0x80000961, 0x53060660,
    0x00010200, 0x00000000, 0x80000061, 0x53260660,
    0x00010210, 0x00000000, 0x80003b01, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x4f260220,
    0x00005324, 0x00000000, 0x80031961, 0x4f060220,
    0x00005304, 0x00000000, 0xe25d0061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004e31, 0x54140000, 0xfb004f0c, 0x00340000,
    0x80001a61, 0x30010220, 0x00005d04, 0x00000000,
    0x80001e69, 0x10018220, 0x02003104, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000200,
    0x80000961, 0x56060660, 0x00010400, 0x00000000,
    0x80000061, 0x56260660, 0x00010410, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80031a61, 0x50260220, 0x00005624, 0x00000000,
    0x80031961, 0x50060220, 0x00005604, 0x00000000,
    0x80002e01, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004f31, 0x00000000, 0xfb08500c, 0x00345414,
    0x80003e01, 0x00000000, 0x00000000, 0x00000000,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0xa04f0040, 0x0c006403, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0xe0510068, 0x00604f03,
    0xa1380040, 0x4f0e2b02, 0xaa660040, 0x500e2d02,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0xa0531b40, 0x51000302, 0x00031b61, 0x1d060220,
    0x00343805, 0x00000000, 0x80103901, 0x00000000,
    0x00000000, 0x00000000, 0x00131b61, 0x1f060220,
    0x00346605, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x00040069, 0x57058660,
    0x02465305, 0x00000003, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0xe05b0068, 0x01d05303,
    0xa1371a40, 0x570e2802, 0xaa5f1b40, 0x580e2f02,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x00031a61, 0x22060220, 0x00343705, 0x00000000,
    0x80103201, 0x00000000, 0x00000000, 0x00000000,
    0x80103501, 0x00000000, 0x00000000, 0x00000000,
    0x00131a61, 0x24060220, 0x00345f05, 0x00000000,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x27550070, 0x03005303, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x00030070, 0x60050220,
    0x52463705, 0x00442806, 0x00130070, 0x61050220,
    0x52465f05, 0x00442f06, 0x00030070, 0x3b050220,
    0x52463805, 0x00442b06, 0x00130070, 0x67050220,
    0x52466605, 0x00442d06, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x80001d01, 0x00000000,
    0x00000000, 0x00000000, 0x00040069, 0x5905a660,
    0x02465505, 0x00000003, 0x00031b40, 0x3c052660,
    0x06463b05, 0x00442b26, 0x00131b40, 0x68052660,
    0x06466705, 0x00442d26, 0x205d1b66, 0x5b005903,
    0x00031b61, 0x1d260220, 0x00343c05, 0x00000000,
    0x00131b61, 0x1f260220, 0x00346805, 0x00000000,
    0x00041b52, 0x64040e68, 0x0e2e7405, 0x60055d05,
    0x00031961, 0x22260220, 0x00346405, 0x00000000,
    0x00131a61, 0x24260220, 0x00346505, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044531, 0x65240000, 0xfb042224, 0x000c0000,
    0x00032561, 0x19060220, 0x00346505, 0x00000000,
    0x00132561, 0x1b060220, 0x00346605, 0x00000000,
    0x0003a561, 0x19260220, 0x00346705, 0x00000000,
    0x0013a561, 0x1b260220, 0x00346805, 0x00000000,
    0xe232004c, 0x00114004, 0x80001969, 0x10018220,
    0x02003204, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000200, 0x80000961, 0x5d060660,
    0x00010480, 0x00000000, 0x80000061, 0x5d260660,
    0x00010490, 0x00000000, 0x80031961, 0x55260220,
    0x00005d24, 0x00000000, 0x80031961, 0x55060220,
    0x00005d04, 0x00000000, 0xe25c0061, 0x00113004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004c31, 0x5a140000, 0xfb00550c, 0x00340000,
    0x80001a61, 0x30010220, 0x00005c04, 0x00000000,
    0x80001e69, 0x10018220, 0x02003204, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000200,
    0x80000961, 0x5e060660, 0x00010680, 0x00000000,
    0x80000061, 0x5e260660, 0x00010690, 0x00000000,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80031a61, 0x56260220, 0x00005e24, 0x00000000,
    0x80031961, 0x56060220, 0x00005e04, 0x00000000,
    0x80002c01, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x8a004d31, 0x00000000, 0xfb08560c, 0x00345a14,
    0x00040052, 0x26044160, 0x0e0e0003, 0x26053905,
    0x00040027, 0x00014060, 0x00000000, 0xfffff0d8,
    0x80030061, 0x7e050220, 0x00460005, 0x00000000,
    0x80040931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_misc_copy_instance_ptrs_indirect = {
   .prog_data = {
      .base.nr_params = 14,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 5504,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_misc_copy_instance_ptrs_indirect_relocs,
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
   .args = gfx125_misc_copy_instance_ptrs_indirect_args,
   .code = gfx125_misc_copy_instance_ptrs_indirect_code,
};
const char *gfx125_misc_copy_instance_ptrs_indirect_sha1 = "729d848fc5207de370f916da71b0465eb089d72e";
