#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_copy_clone_indirect_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_copy_clone_indirect_args[] = {
   { 0, 8 },
   { 8, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g65<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g33<1>UD        g0.1<0,1,0>UD                   { align1 1H };
add(1)          g66<1>UD        g65<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@2 compacted };
mov(8)          g64<1>UW        0x76543210V                     { align1 WE_all 1Q };
shl(16)         g26<1>D         g33<8,8,1>D     0x00000004UD    { align1 1H I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(1)         g1UD            g66UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
add(8)          g64.8<1>UW      g64<1,1,0>UW    0x0008UW        { align1 WE_all 1Q I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         g28<1>D         g64<8,8,1>UW                    { align1 1H };
and(16)         g30<1>UD        g28<1,1,0>UD    0x0000000fUD    { align1 1H I@1 compacted };
add(16)         g6<1>D          g26<1,1,0>D     g30<1,1,0>D     { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g12.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g14.1<2>F       g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g8.1<2>F        g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g10.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g12<2>F         g2<0,1,0>F                      { align1 1Q F@4 compacted };
mov(8)          g14<2>F         g2<0,1,0>F                      { align1 2Q F@4 compacted };
mov(8)          g8<2>F          g2.2<0,1,0>F                    { align1 1Q F@4 compacted };
mov(8)          g10<2>F         g2.2<0,1,0>F                    { align1 2Q F@4 compacted };
and(8)          g32<1>UD        g12<8,4,2>UD    0x0000003fUD    { align1 1Q F@4 compacted };
and(8)          g33<1>UD        g14<8,4,2>UD    0x0000003fUD    { align1 2Q F@3 compacted };
add(8)          g79<1>D         g8<8,4,2>D      168D            { align1 1Q F@2 compacted };
add(8)          g80<1>D         g10<8,4,2>D     168D            { align1 2Q F@1 compacted };
add(16)         g34<1>D         -g32<1,1,0>D    64D             { align1 1H I@3 compacted };
mov(8)          g38<2>UD        g79<4,4,1>UD                    { align1 1Q I@3 };
cmp.l.f0.0(16)  g18<1>UD        g79<1,1,0>UD    0x000000a8UD    { align1 1H I@3 compacted };
mov(8)          g40<2>UD        g80<4,4,1>UD                    { align1 2Q };
and(16)         g36<1>UD        g34<1,1,0>UD    0x0000003fUD    { align1 1H I@4 compacted };
add(8)          g20<1>D         -g18<8,8,1>D    g8.1<8,4,2>D    { align1 1Q I@3 };
add(8)          g21<1>D         -g19<8,8,1>D    g10.1<8,4,2>D   { align1 2Q I@4 };
add(8)          g42<1>D         g12<8,4,2>D     g36<1,1,0>D     { align1 1Q I@3 compacted };
add(8)          g43<1>D         g14<8,4,2>D     g37<1,1,0>D     { align1 2Q I@4 compacted };
add(8)          g48<1>D         g8<8,4,2>D      g36<1,1,0>D     { align1 1Q compacted };
add(8)          g49<1>D         g10<8,4,2>D     g37<1,1,0>D     { align1 2Q compacted };
mov(8)          g38.1<2>UD      g20<4,4,1>UD                    { align1 1Q I@6 };
mov(8)          g40.1<2>UD      g21<4,4,1>UD                    { align1 2Q I@6 };
cmp.l.f0.0(8)   g44<1>UD        g42<8,8,1>UD    g12<8,4,2>UD    { align1 1Q I@6 };
cmp.l.f0.0(8)   g45<1>UD        g43<8,8,1>UD    g14<8,4,2>UD    { align1 2Q I@6 };
cmp.l.f0.0(8)   g50<1>UD        g48<8,8,1>UD    g8<8,4,2>UD     { align1 1Q I@6 };
cmp.l.f0.0(8)   g51<1>UD        g49<8,8,1>UD    g10<8,4,2>UD    { align1 2Q I@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@5 };
send(16)        g21UD           g38UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
add(8)          g46<1>D         -g44<8,8,1>D    g12.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g47<1>D         -g45<8,8,1>D    g14.1<8,4,2>D   { align1 2Q I@4 };
add(8)          g52<1>D         -g50<8,8,1>D    g8.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g53<1>D         -g51<8,8,1>D    g10.1<8,4,2>D   { align1 2Q I@4 };
shr(16)         g25<1>UD        g21<1,1,0>UD    0x00000008UD    { align1 1H $1.dst compacted };
add(16)         g38<1>D         g21<1,1,0>D     -g36<1,1,0>D    { align1 1H $1.src compacted };
cmp.l.f0.0(16)  null<1>UD       g36<8,8,1>UD    g21<8,8,1>UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
shr(16)         g40<1>UD        g38<1,1,0>UD    0x00000002UD    { align1 1H I@2 compacted };
(+f0.0) if(16)  JIP:  LABEL0          UIP:  LABEL0              { align1 1H };
shl(16)         g54<1>D         g25<8,8,1>D     0x00000004UD    { align1 1H I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
mov(16)         g65<1>UD        g6<8,8,1>UD                     { align1 1H };

LABEL2:
cmp.ge.f0.0(16) null<1>UD       g65<8,8,1>UD    g40<8,8,1>UD    { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL1        UIP:  LABEL1              { align1 1H };
shl(16)         g56<1>D         g65<8,8,1>D     0x00000002UD    { align1 1H };
shr(16)         g58<1>UD        g65<1,1,0>UD    0x0000001eUD    { align1 1H compacted };
add(16)         g60<1>D         g48<1,1,0>D     g56<1,1,0>D     { align1 1H I@2 compacted };
add(16)         g4<1>D          g42<1,1,0>D     g56<1,1,0>D     { align1 1H compacted };
cmp.l.f0.0(16)  g81<1>UD        g60<1,1,0>UD    g48<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g71<2>UD        g60<4,4,1>UD                    { align1 1Q $2.src };
mov(8)          g73<2>UD        g61<4,4,1>UD                    { align1 2Q $2.src };
cmp.l.f0.0(16)  g67<1>UD        g4<1,1,0>UD     g42<1,1,0>UD    { align1 1H I@4 compacted };
mov(8)          g75<2>UD        g4<4,4,1>UD                     { align1 1Q $3.src };
mov(8)          g77<2>UD        g5<4,4,1>UD                     { align1 2Q $3.src };
add3(16)        g3<1>D          g52<8,8,1>D     g58<8,8,1>D     -g81<1,1,1>D { align1 1H I@6 };
add3(16)        g69<1>D         g46<8,8,1>D     g58<8,8,1>D     -g67<1,1,1>D { align1 1H I@4 };
mov(8)          g71.1<2>UD      g3<4,4,1>UD                     { align1 1Q I@2 };
mov(8)          g73.1<2>UD      g4<4,4,1>UD                     { align1 2Q I@3 };
mov(8)          g75.1<2>UD      g69<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g77.1<2>UD      g70<4,4,1>UD                    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g16UD           g71UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g75UD           g16UD           0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $3 };
add3(16)        g65<1>D         0x0040UW        g54<8,8,1>D     g65<1,1,1>D { align1 1H };

LABEL1:
while(16)       JIP:  LABEL2                                    { align1 1H };
and(16)         g70<1>UD        g38<8,8,1>UD    0xfffffffcUD    { align1 1H $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
add(16)         g72<1>D         g38<1,1,0>D     -g70<1,1,0>D    { align1 1H I@1 compacted };
cmp.l.f0.0(16)  null<1>UD       g6<8,8,1>UD     g72<8,8,1>UD    { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL3          UIP:  LABEL3              { align1 1H };
add(16)         g73<1>D         g42<1,1,0>D     g70<1,1,0>D     { align1 1H $2.src compacted };
add(16)         g77<1>D         g48<1,1,0>D     g70<1,1,0>D     { align1 1H $3.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
cmp.l.f0.0(16)  g75<1>UD        g73<1,1,0>UD    g42<1,1,0>UD    { align1 1H I@2 compacted };
add(16)         g87<1>D         g73<1,1,0>D     g6<1,1,0>D      { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
cmp.l.f0.0(16)  g16<1>UD        g77<1,1,0>UD    g48<1,1,0>UD    { align1 1H I@3 compacted };
add(16)         g62<1>D         g77<1,1,0>D     g6<1,1,0>D      { align1 1H compacted };
mov(8)          g97<2>UD        g87<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g99<2>UD        g88<4,4,1>UD                    { align1 2Q I@4 };
mov(8)          g93<2>UD        g62<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g95<2>UD        g63<4,4,1>UD                    { align1 2Q I@4 };
cmp.l.f0.0(16)  g83<1>UD        g62<1,1,0>UD    g77<1,1,0>UD    { align1 1H compacted };
cmp.l.f0.0(16)  g89<1>UD        g87<1,1,0>UD    g73<1,1,0>UD    { align1 1H compacted };
add3(16)        g85<1>D         -g16<8,8,1>D    g52<8,8,1>D     -g83<1,1,1>D { align1 1H I@2 };
add3(16)        g91<1>D         -g75<8,8,1>D    g46<8,8,1>D     -g89<1,1,1>D { align1 1H I@2 };
mov(8)          g93.1<2>UD      g85<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g95.1<2>UD      g86<4,4,1>UD                    { align1 2Q I@3 };
mov(8)          g97.1<2>UD      g91<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g99.1<2>UD      g92<4,4,1>UD                    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g86UD           g93UD           nullUD          0x08200980                0x00000000
                            ugm MsgDesc: ( load, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
mov(16)         g101<1>UD       g86<32,8,4>UB                   { align1 1H $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g97UD           g101UD          0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $5 };

LABEL3:
endif(16)       JIP:  LABEL0                                    { align1 1H };

LABEL0:
endif(16)       JIP:  LABEL4                                    { align1 1H };

LABEL4:
and(8)          g92<1>UD        g12<8,4,2>UD    0x00000003UD    { align1 1Q compacted };
and(8)          g93<1>UD        g14<8,4,2>UD    0x00000003UD    { align1 2Q $4.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
add(16)         g94<1>D         -g92<1,1,0>D    4D              { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
and.nz.f0.0(16) g96<1>UD        g94<1,1,0>UD    0x00000003UD    { align1 1H I@1 compacted };
(+f0.0) if(16)  JIP:  LABEL5          UIP:  LABEL5              { align1 1H };
cmp.l.f0.0(16)  null<1>UD       g6<8,8,1>UD     g96<8,8,1>UD    { align1 1H I@2 };
(+f0.0) if(16)  JIP:  LABEL6          UIP:  LABEL6              { align1 1H };
add(8)          g98<1>D         g8<8,4,2>D      g6<1,1,0>D      { align1 1Q $5.src compacted };
add(8)          g99<1>D         g10<8,4,2>D     g7<1,1,0>D      { align1 2Q $5.src compacted };
add(8)          g104<1>D        g12<8,4,2>D     g6<1,1,0>D      { align1 1Q compacted };
add(8)          g105<1>D        g14<8,4,2>D     g7<1,1,0>D      { align1 2Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
cmp.l.f0.0(8)   g100<1>UD       g98<8,8,1>UD    g8<8,4,2>UD     { align1 1Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $5.src };
cmp.l.f0.0(8)   g101<1>UD       g99<8,8,1>UD    g10<8,4,2>UD    { align1 2Q I@4 };
mov(8)          g110<2>UD       g98<4,4,1>UD                    { align1 1Q };
mov(8)          g112<2>UD       g99<4,4,1>UD                    { align1 2Q };
cmp.l.f0.0(8)   g106<1>UD       g104<8,8,1>UD   g12<8,4,2>UD    { align1 1Q I@6 };
cmp.l.f0.0(8)   g107<1>UD       g105<8,8,1>UD   g14<8,4,2>UD    { align1 2Q I@6 };
mov(8)          g114<2>UD       g104<4,4,1>UD                   { align1 1Q };
mov(8)          g116<2>UD       g105<4,4,1>UD                   { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
add(8)          g102<1>D        -g100<8,8,1>D   g8.1<8,4,2>D    { align1 1Q I@7 };
add(8)          g103<1>D        -g101<8,8,1>D   g10.1<8,4,2>D   { align1 2Q I@7 };
add(8)          g108<1>D        -g106<8,8,1>D   g12.1<8,4,2>D   { align1 1Q I@6 };
add(8)          g109<1>D        -g107<8,8,1>D   g14.1<8,4,2>D   { align1 2Q I@6 };
mov(8)          g110.1<2>UD     g102<4,4,1>UD                   { align1 1Q I@4 };
mov(8)          g112.1<2>UD     g103<4,4,1>UD                   { align1 2Q I@4 };
mov(8)          g114.1<2>UD     g108<4,4,1>UD                   { align1 1Q I@4 };
mov(8)          g116.1<2>UD     g109<4,4,1>UD                   { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(16)        g103UD          g110UD          nullUD          0x08200980                0x00000000
                            ugm MsgDesc: ( load, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $6 };
mov(16)         g118<1>UD       g103<32,8,4>UB                  { align1 1H $6.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g114UD          g118UD          0x08000984                0x00000080
                            ugm MsgDesc: ( store, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $7 };

LABEL6:
endif(16)       JIP:  LABEL5                                    { align1 1H };
add(8)          g109<1>D        g12<8,4,2>D     g96<1,1,0>D     { align1 1Q compacted };
add(8)          g110<1>D        g14<8,4,2>D     g97<1,1,0>D     { align1 2Q $6.src compacted };
add(8)          g115<1>D        g8<8,4,2>D      g96<1,1,0>D     { align1 1Q $7.src compacted };
add(8)          g116<1>D        g10<8,4,2>D     g97<1,1,0>D     { align1 2Q $7.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
cmp.l.f0.0(8)   g111<1>UD       g109<8,8,1>UD   g12<8,4,2>UD    { align1 1Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $6.src };
cmp.l.f0.0(8)   g112<1>UD       g110<8,8,1>UD   g14<8,4,2>UD    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
cmp.l.f0.0(8)   g117<1>UD       g115<8,8,1>UD   g8<8,4,2>UD     { align1 1Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $7.src };
cmp.l.f0.0(8)   g118<1>UD       g116<8,8,1>UD   g10<8,4,2>UD    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
add(8)          g113<1>D        -g111<8,8,1>D   g12.1<8,4,2>D   { align1 1Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $7.src };
add(8)          g114<1>D        -g112<8,8,1>D   g14.1<8,4,2>D   { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
add(8)          g119<1>D        -g117<8,8,1>D   g8.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g120<1>D        -g118<8,8,1>D   g10.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g12<2>UD        g109<4,4,1>UD                   { align1 1Q };
mov(8)          g14<2>UD        g110<4,4,1>UD                   { align1 2Q };
mov(8)          g8<2>UD         g115<4,4,1>UD                   { align1 1Q };
mov(8)          g10<2>UD        g116<4,4,1>UD                   { align1 2Q };
mov(8)          g12.1<2>UD      g113<4,4,1>UD                   { align1 1Q I@4 };
mov(8)          g14.1<2>UD      g114<4,4,1>UD                   { align1 2Q I@4 };
mov(8)          g8.1<2>UD       g119<4,4,1>UD                   { align1 1Q I@4 };
mov(8)          g10.1<2>UD      g120<4,4,1>UD                   { align1 2Q I@4 };

LABEL5:
endif(16)       JIP:  LABEL7                                    { align1 1H };

LABEL7:
and(8)          g120<1>UD       g12<8,4,2>UD    0x0000003fUD    { align1 1Q I@5 compacted };
and(8)          g121<1>UD       g14<8,4,2>UD    0x0000003fUD    { align1 2Q I@5 compacted };
add(16)         g122<1>D        -g120<1,1,0>D   64D             { align1 1H I@1 compacted };
and(16)         g124<1>UD       g122<1,1,0>UD   0x0000003fUD    { align1 1H I@1 compacted };
shr(16)         g126<1>UD       g124<1,1,0>UD   0x00000002UD    { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
cmp.l.f0.0(16)  g1<1>UD         g6<1,1,0>UD     g126<1,1,0>UD   { align1 1H A@1 compacted };
cmp.nz.f0.0(16) g64<1>D         g124<1,1,0>D    0D              { align1 1H compacted };
and.nz.f0.0(16) null<1>UD       g1<8,8,1>UD     g64<8,8,1>UD    { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL8          UIP:  LABEL8              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
shl(16)         g65<1>D         g6<8,8,1>D      0x00000002UD    { align1 1H };
shr(16)         g79<1>UD        g6<1,1,0>UD     0x0000001eUD    { align1 1H compacted };
mov(8)          g18<1>UD        g8.1<8,4,2>UD                   { align1 1Q };
mov(8)          g19<1>UD        g10.1<8,4,2>UD                  { align1 2Q };
mov(8)          g26<1>UD        g12.1<8,4,2>UD                  { align1 1Q };
mov(8)          g27<1>UD        g14.1<8,4,2>UD                  { align1 2Q };
add(8)          g20<1>D         g8<8,4,2>D      g65<1,1,0>D     { align1 1Q I@6 compacted };
add(8)          g21<1>D         g10<8,4,2>D     g66<1,1,0>D     { align1 2Q I@7 compacted };
add(8)          g28<1>D         g12<8,4,2>D     g65<1,1,0>D     { align1 1Q compacted };
add(8)          g29<1>D         g14<8,4,2>D     g66<1,1,0>D     { align1 2Q compacted };
cmp.l.f0.0(8)   g22<1>UD        g20<8,8,1>UD    g8<8,4,2>UD     { align1 1Q I@4 };
mov(8)          g119<2>UD       g20<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g23<1>UD        g21<8,8,1>UD    g10<8,4,2>UD    { align1 2Q @5 $1.dst };
mov(8)          g121<2>UD       g21<4,4,1>UD                    { align1 2Q };
cmp.l.f0.0(8)   g30<1>UD        g28<8,8,1>UD    g12<8,4,2>UD    { align1 1Q I@6 };
mov(8)          g123<2>UD       g28<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g31<1>UD        g29<8,8,1>UD    g14<8,4,2>UD    { align1 2Q I@7 };
mov(8)          g125<2>UD       g29<4,4,1>UD                    { align1 2Q };
add3(16)        g24<1>D         g18<8,8,1>D     g79<8,8,1>D     -g22<1,1,1>D { align1 1H @6 $1.dst };
add3(16)        g32<1>D         g26<8,8,1>D     g79<8,8,1>D     -g30<1,1,1>D { align1 1H I@3 };
mov(8)          g119.1<2>UD     g24<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g121.1<2>UD     g25<4,4,1>UD                    { align1 2Q I@3 };
mov(8)          g123.1<2>UD     g32<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g125.1<2>UD     g33<4,4,1>UD                    { align1 2Q I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        g1UD            g119UD          nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $8 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g123UD          g1UD            0x08001586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $9 };

LABEL8:
endif(16)       JIP:  LABEL9                                    { align1 1H };

LABEL9:
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q $9.src };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_copy_clone_indirect_code[] = {
    0x80000065, 0x41058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x21050220, 0x00000024, 0x00000000,
    0xe2421a40, 0x00014103, 0x80030061, 0x40054410,
    0x00000000, 0x76543210, 0x00041b69, 0x1a058660,
    0x02462105, 0x00000004, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa00420c, 0x00340000, 0x64401a40, 0x00804095,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00040061, 0x1c050160, 0x00464005, 0x00000000,
    0xe01e1965, 0x00f01c03, 0xa0061940, 0x1e001a02,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x210c0061, 0x001102cc, 0x2a0e0061, 0x001102cc,
    0x00030061, 0x08260aa0, 0x00000264, 0x00000000,
    0x00130061, 0x0a260aa0, 0x00000264, 0x00000000,
    0x210c1461, 0x00110204, 0x2a0e1461, 0x00110204,
    0x21081461, 0x00110244, 0x2a0a1461, 0x00110244,
    0xe1201465, 0x03fe0c03, 0xea211365, 0x03fe0e03,
    0xa14f1240, 0x0a8e0803, 0xaa501140, 0x0a8e0a03,
    0xa0221b40, 0x04022003, 0x00031b61, 0x26060220,
    0x00344f05, 0x00000000, 0xe7121b70, 0x0a804f03,
    0x00130061, 0x28060220, 0x00345005, 0x00000000,
    0xe0241c65, 0x03f02203, 0x00031b40, 0x14052660,
    0x06461205, 0x00440826, 0x00131c40, 0x15052660,
    0x06461305, 0x00440a26, 0xa12a1b40, 0x240e0c02,
    0xaa2b1c40, 0x250e0e02, 0xa1300040, 0x240e0802,
    0xaa310040, 0x250e0a02, 0x00031e61, 0x26260220,
    0x00341405, 0x00000000, 0x00131e61, 0x28260220,
    0x00341505, 0x00000000, 0x00031e70, 0x2c050220,
    0x52462a05, 0x00440c06, 0x00131e70, 0x2d050220,
    0x52462b05, 0x00440e06, 0x00031e70, 0x32050220,
    0x52463005, 0x00440806, 0x00131e70, 0x33050220,
    0x52463105, 0x00440a06, 0x80001d01, 0x00000000,
    0x00000000, 0x00000000, 0x00044131, 0x15240000,
    0xfb042624, 0x000c0000, 0x00031c40, 0x2e052660,
    0x06462c05, 0x00440c26, 0x00131c40, 0x2f052660,
    0x06462d05, 0x00440e26, 0x00031c40, 0x34052660,
    0x06463205, 0x00440826, 0x00131c40, 0x35052660,
    0x06463305, 0x00440a26, 0xe0192168, 0x00801503,
    0xa0263140, 0x24201502, 0x00040070, 0x00010220,
    0x52462405, 0x00461505, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0xe0281a68, 0x00202603,
    0x01040022, 0x0001c060, 0x00000360, 0x00000360,
    0x00041d69, 0x36058660, 0x02461905, 0x00000004,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00040061, 0x41050220, 0x00460605, 0x00000000,
    0x00041970, 0x00010220, 0x42464105, 0x00462805,
    0x01040028, 0x0001c660, 0x00000158, 0x00000158,
    0x00040069, 0x38058660, 0x02464105, 0x00000002,
    0xe03a0068, 0x01e04103, 0xa03c1a40, 0x38003002,
    0xa0040040, 0x38002a02, 0x27511a70, 0x30003c03,
    0x00033261, 0x47060220, 0x00343c05, 0x00000000,
    0x00133261, 0x49060220, 0x00343d05, 0x00000000,
    0x27431c70, 0x2a000403, 0x00033361, 0x4b060220,
    0x00340405, 0x00000000, 0x00133361, 0x4d060220,
    0x00340505, 0x00000000, 0x00041e52, 0x03040e68,
    0x0e2e3405, 0x51053a05, 0x00041c52, 0x45040e68,
    0x0e2e2e05, 0x43053a05, 0x00031a61, 0x47260220,
    0x00340305, 0x00000000, 0x00131b61, 0x49260220,
    0x00340405, 0x00000000, 0x00031b61, 0x4b260220,
    0x00344505, 0x00000000, 0x00131c61, 0x4d260220,
    0x00344605, 0x00000000, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x10140000,
    0xfb044724, 0x00040000, 0x80002201, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044331, 0x00000000,
    0xfb0c4b24, 0x00041014, 0x00040052, 0x41044160,
    0x0e0e0040, 0x41053605, 0x00040027, 0x00014060,
    0x00000000, 0xfffffe98, 0x00043265, 0x46058220,
    0x02462605, 0xfffffffc, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0xa0481940, 0x46202602,
    0x00041970, 0x00010220, 0x52460605, 0x00464805,
    0x01040022, 0x0001c060, 0x00000160, 0x00000160,
    0xa0493240, 0x46002a02, 0xa04d3340, 0x46003002,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x274b1a70, 0x2a004903, 0xa0570040, 0x06004902,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x27101b70, 0x30004d03, 0xa03e0040, 0x06004d02,
    0x00031b61, 0x61060220, 0x00345705, 0x00000000,
    0x00131c61, 0x63060220, 0x00345805, 0x00000000,
    0x00031b61, 0x5d060220, 0x00343e05, 0x00000000,
    0x00131c61, 0x5f060220, 0x00343f05, 0x00000000,
    0x27530070, 0x4d003e03, 0x27590070, 0x49005703,
    0x00041a52, 0x55042e68, 0x0e2e1005, 0x53053405,
    0x00041a52, 0x5b042e68, 0x0e2e4b05, 0x59052e05,
    0x00031a61, 0x5d260220, 0x00345505, 0x00000000,
    0x00131b61, 0x5f260220, 0x00345605, 0x00000000,
    0x00031b61, 0x61260220, 0x00345b05, 0x00000000,
    0x00131c61, 0x63260220, 0x00345c05, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x56140000, 0xf3005d24, 0x00020000,
    0x00042461, 0x65050020, 0x00665607, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044531, 0x00000000, 0xf3086124, 0x00026514,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0xe15c0065, 0x003e0c03, 0xea5d3465, 0x003e0e03,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0xa05e1940, 0x00425c03, 0x80003401, 0x00000000,
    0x00000000, 0x00000000, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0xee601965, 0x00305e03,
    0x01040022, 0x0001c060, 0x00000370, 0x00000370,
    0x00041a70, 0x00010220, 0x52460605, 0x00466005,
    0x01040022, 0x0001c060, 0x000001b0, 0x000001b0,
    0xa1623540, 0x060e0802, 0xaa633540, 0x070e0a02,
    0xa1680040, 0x060e0c02, 0xaa690040, 0x070e0e02,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x00031c70, 0x64050220, 0x52466205, 0x00440806,
    0x80103501, 0x00000000, 0x00000000, 0x00000000,
    0x00131c70, 0x65050220, 0x52466305, 0x00440a06,
    0x00030061, 0x6e060220, 0x00346205, 0x00000000,
    0x00130061, 0x70060220, 0x00346305, 0x00000000,
    0x00031e70, 0x6a050220, 0x52466805, 0x00440c06,
    0x00131e70, 0x6b050220, 0x52466905, 0x00440e06,
    0x00030061, 0x72060220, 0x00346805, 0x00000000,
    0x00130061, 0x74060220, 0x00346905, 0x00000000,
    0x80003501, 0x00000000, 0x00000000, 0x00000000,
    0x00031f40, 0x66052660, 0x06466405, 0x00440826,
    0x00131f40, 0x67052660, 0x06466505, 0x00440a26,
    0x00031e40, 0x6c052660, 0x06466a05, 0x00440c26,
    0x00131e40, 0x6d052660, 0x06466b05, 0x00440e26,
    0x00031c61, 0x6e260220, 0x00346605, 0x00000000,
    0x00131c61, 0x70260220, 0x00346705, 0x00000000,
    0x00031c61, 0x72260220, 0x00346c05, 0x00000000,
    0x00131c61, 0x74260220, 0x00346d05, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00044631, 0x67140000, 0xf3006e24, 0x00020000,
    0x00042661, 0x76050020, 0x00666707, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044731, 0x00000000, 0xf3087224, 0x00027614,
    0x00040025, 0x00004600, 0x00000000, 0x000001a0,
    0xa16d0040, 0x600e0c02, 0xaa6e3640, 0x610e0e02,
    0xa1733740, 0x600e0802, 0xaa743740, 0x610e0a02,
    0x80003601, 0x00000000, 0x00000000, 0x00000000,
    0x00031c70, 0x6f050220, 0x52466d05, 0x00440c06,
    0x80103601, 0x00000000, 0x00000000, 0x00000000,
    0x00131c70, 0x70050220, 0x52466e05, 0x00440e06,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x00031c70, 0x75050220, 0x52467305, 0x00440806,
    0x80103701, 0x00000000, 0x00000000, 0x00000000,
    0x00131c70, 0x76050220, 0x52467405, 0x00440a06,
    0x80003601, 0x00000000, 0x00000000, 0x00000000,
    0x00031c40, 0x71052660, 0x06466f05, 0x00440c26,
    0x80103701, 0x00000000, 0x00000000, 0x00000000,
    0x00131c40, 0x72052660, 0x06467005, 0x00440e26,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x00031c40, 0x77052660, 0x06467505, 0x00440826,
    0x00131c40, 0x78052660, 0x06467605, 0x00440a26,
    0x00030061, 0x0c060220, 0x00346d05, 0x00000000,
    0x00130061, 0x0e060220, 0x00346e05, 0x00000000,
    0x00030061, 0x08060220, 0x00347305, 0x00000000,
    0x00130061, 0x0a060220, 0x00347405, 0x00000000,
    0x00031c61, 0x0c260220, 0x00347105, 0x00000000,
    0x00131c61, 0x0e260220, 0x00347205, 0x00000000,
    0x00031c61, 0x08260220, 0x00347705, 0x00000000,
    0x00131c61, 0x0a260220, 0x00347805, 0x00000000,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0xe1781d65, 0x03fe0c03, 0xea791d65, 0x03fe0e03,
    0xa07a1940, 0x04027803, 0xe07c1965, 0x03f07a03,
    0xe07e1968, 0x00207c03, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x27010970, 0x7e000603,
    0xae400070, 0x00007c03, 0x00041965, 0x00010220,
    0x22460105, 0x00464005, 0x01040022, 0x0001c060,
    0x000001c8, 0x000001c8, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x00040069, 0x41058660,
    0x02460605, 0x00000002, 0xe04f0068, 0x01e00603,
    0x00030061, 0x12050220, 0x00440826, 0x00000000,
    0x00130061, 0x13050220, 0x00440a26, 0x00000000,
    0x00030061, 0x1a050220, 0x00440c26, 0x00000000,
    0x00130061, 0x1b050220, 0x00440e26, 0x00000000,
    0xa1141e40, 0x410e0802, 0xaa151f40, 0x420e0a02,
    0xa11c0040, 0x410e0c02, 0xaa1d0040, 0x420e0e02,
    0x00031c70, 0x16050220, 0x52461405, 0x00440806,
    0x00030061, 0x77060220, 0x00341405, 0x00000000,
    0x0013d170, 0x17050220, 0x52461505, 0x00440a06,
    0x00130061, 0x79060220, 0x00341505, 0x00000000,
    0x00031e70, 0x1e050220, 0x52461c05, 0x00440c06,
    0x00030061, 0x7b060220, 0x00341c05, 0x00000000,
    0x00131f70, 0x1f050220, 0x52461d05, 0x00440e06,
    0x00130061, 0x7d060220, 0x00341d05, 0x00000000,
    0x0004e152, 0x18040e68, 0x0e2e1205, 0x16054f05,
    0x00041b52, 0x20040e68, 0x0e2e1a05, 0x1e054f05,
    0x00031a61, 0x77260220, 0x00341805, 0x00000000,
    0x00131b61, 0x79260220, 0x00341905, 0x00000000,
    0x00031b61, 0x7b260220, 0x00342005, 0x00000000,
    0x00131c61, 0x7d260220, 0x00342105, 0x00000000,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00044831, 0x01140000, 0xfb047724, 0x00040000,
    0x80002801, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044931, 0x00000000, 0xfb0c7b24, 0x00040114,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x80033961, 0x7e050220, 0x00460005, 0x00000000,
    0x80040931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_copy_clone_indirect = {
   .prog_data = {
      .base.nr_params = 12,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 3104,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_copy_clone_indirect_relocs,
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
      .push.cross_thread.dwords = 12,
      .push.cross_thread.regs = 2,
      .push.cross_thread.size = 64,
   },
   .args_size = 16,
   .arg_count = 2,
   .args = gfx125_bvh_copy_clone_indirect_args,
   .code = gfx125_bvh_copy_clone_indirect_code,
};
const char *gfx125_bvh_copy_clone_indirect_sha1 = "d5ec55f99f7bbf8e739500902ffb0757c28c3c00";
