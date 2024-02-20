#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_build_BFS_BFS_pass1_initial_batchable_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_build_BFS_BFS_pass1_initial_batchable_args[] = {
   { 0, 8 },
   { 8, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g106<1>UD       g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g42<1>UD        g0.1<0,1,0>UD                   { align1 1H };
mov(16)         g46<1>UD        0x00000000UD                    { align1 1H };
add(1)          g107<1>UD       g106<0,1,0>UD   0x00000000UD    { align1 WE_all 1N I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(1)         g1UD            g107UD          nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.dst };
mov(8)          g60.1<2>F       g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g31.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g62.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g60<2>F         g2<0,1,0>F                      { align1 2Q F@3 compacted };
mov(8)          g31<2>F         g2<0,1,0>F                      { align1 1Q F@3 compacted };
mov(8)          g62<2>F         g2.2<0,1,0>F                    { align1 2Q F@3 compacted };
add(8)          g65<1>D         g60<8,4,2>D     1064D           { align1 2Q F@3 compacted };
add(8)          g118<1>D        g31<8,4,2>D     1064D           { align1 1Q F@2 compacted };
mov(8)          g64<1>UD        g31.1<8,4,2>UD                  { align1 1Q };
cmp.l.f0.0(8)   g66<1>UD        g65<8,8,1>UD    g60<8,4,2>UD    { align1 2Q I@3 };
mov(8)          g36<2>UD        g65<4,4,1>UD                    { align1 2Q };
cmp.l.f0.0(8)   g119<1>UD       g118<8,8,1>UD   g31<8,4,2>UD    { align1 1Q I@4 };
mov(8)          g34<2>UD        g118<4,4,1>UD                   { align1 1Q };
mov(8)          g65<1>UD        g60.1<8,4,2>UD                  { align1 2Q };
add(8)          g67<1>D         -g66<8,8,1>D    g60.1<8,4,2>D   { align1 2Q I@5 };
add(8)          g120<1>D        -g119<8,8,1>D   g31.1<8,4,2>D   { align1 1Q I@4 };
mov(8)          g36.1<2>UD      g67<4,4,1>UD                    { align1 2Q I@2 };
mov(8)          g34.1<2>UD      g120<4,4,1>UD                   { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g68UD           g34UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
add(8)          g36<1>D         g31<8,4,2>D     1068D           { align1 1Q $1.src compacted };
add(8)          g37<1>D         g60<8,4,2>D     1068D           { align1 2Q $1.src compacted };
mov(8)          g33.1<2>F       g2.3<0,1,0>F                    { align1 1Q $1.src };
mov(8)          g35<1>UW        0x76543210V                     { align1 WE_all 1Q $1.src };
cmp.l.f0.0(8)   g70<1>UD        g36<8,8,1>UD    g31<8,4,2>UD    { align1 1Q I@3 };
mov(16)         g40<1>UD        g36<8,8,1>UD                    { align1 1H I@3 };
cmp.l.f0.0(8)   g71<1>UD        g37<8,8,1>UD    g60<8,4,2>UD    { align1 2Q };
mov(8)          g33<2>F         g2.2<0,1,0>F                    { align1 1Q F@1 compacted };
add(8)          g35.8<1>UW      g35<1,1,0>UW    0x0008UW        { align1 WE_all 1Q I@4 compacted };

LABEL3:
cmp.l.f0.0(16)  g72<1>UD        g40<1,1,0>UD    g36<1,1,0>UD    { align1 1H I@2 compacted };
add3(16)        g74<1>D         -g70<8,8,1>D    g64<8,8,1>D     -g72<1,1,1>D { align1 1H I@1 };
mov(8)          g76<2>UD        g40<4,4,1>UD                    { align1 1Q };
mov(8)          g78<2>UD        g41<4,4,1>UD                    { align1 2Q };
mov(8)          g76.1<2>UD      g74<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g78.1<2>UD      g75<4,4,1>UD                    { align1 2Q I@2 };
fbl(1)          g80<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
add(16)         g86<1>W         g35<16,16,1>UW  -1W             { align1 WE_all 1H I@7 };
shl(1)          a0<1>UD         g80<0,1,0>UD    0x00000003UD    { align1 WE_all 1N I@3 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000800UD    { align1 WE_all 1N A@1 };
mov(1)          g123<2>D        g[a0 384]<0,1,0>D               { align1 WE_all 1N A@1 };
mov(1)          g123.1<2>D      g[a0 388]<0,1,0>D               { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
mov(8)          g48.1<2>UD      g123.1<0,1,0>UD                 { align1 WE_all 1Q I@1 };
mov(8)          g48<2>UD        g123<0,1,0>UD                   { align1 WE_all 1Q I@1 };
mov(1)          g124<2>UW       0x00000000UD                    { align1 WE_all 1N I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
(+f0.0.any16h) send(1) g77UD    g48UD           nullUD          0x0220d580                0x00000000
                            ugm MsgDesc: ( load, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $2 };
mov(16)         g82<1>D         0D                              { align1 WE_all 1H $2.dst };
mov(16)         g82<1>D         g77<8,8,1>D                     { align1 1H };
mov(16)         a0<1>UW         0x0a40UW                        { align1 WE_all 1H I@1 };
shl(16)         a0<1>UW         g86<8,8,1>W     0x0002UW        { align1 WE_all 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0a40UW        { align1 WE_all 1H A@1 };
mov(16)         g84<1>UD        g[a0]<VxH,1,0>UD                { align1 WE_all 1H A@1 compacted };
mov(1)          g84<1>D         0D                              { align1 WE_all 1N I@1 };
add(8)          g84.1<2>D       g84<8,4,2>D     g84.1<8,4,2>D   { align1 WE_all 1Q I@1 };
add(4)          g84.2<4>D       g84.1<8,2,4>D   g84.2<8,2,4>D   { align1 WE_all 1N I@1 };
add(4)          g84.3<4>D       g84.1<8,2,4>D   g84.3<8,2,4>D   { align1 WE_all 1N I@1 };
add(4)          g84.4<1>D       g84.3<0,1,0>D   g84.4<4,4,1>D   { align1 WE_all 1N I@1 };
add(4)          g85.4<1>D       g85.3<0,1,0>D   g85.4<4,4,1>D   { align1 WE_all 1N I@2 };
add(8)          g85<1>D         g84.7<0,1,0>D   g85<1,1,0>D     { align1 WE_all 1Q I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
add(16)         g87<1>D         g42<1,1,0>D     -g84<1,1,0>D    { align1 1H compacted };
mov(16)         g81<1>D         g84<8,8,1>D                     { align1 1H };
cmp.l.f0.0(16)  g89<1>UD        g87<1,1,0>UD    g77<1,1,0>UD    { align1 1H I@2 compacted };
mov(1)          f0<1>UW         g124<0,1,0>UW                   { align1 WE_all 1N };
cmp.nz.f0.0(16) null<1>UD       g89<8,8,1>UD    0x00000000UD    { align1 1H I@2 };
mov.nz.f0.0(16) g91<1>UD        f0<0,1,0>UW                     { align1 1H };
(+f0.0) if(16)  JIP:  LABEL0          UIP:  LABEL0              { align1 1H };
mov(16)         g44<1>UD        0xffffffffUD                    { align1 1H };
break(16)       JIP:  LABEL0          UIP:  LABEL1              { align1 1H };

LABEL0:
endif(16)       JIP:  LABEL1                                    { align1 1H };
add(16)         g93<1>D         g81<1,1,0>D     g77<1,1,0>D     { align1 1H I@7 compacted };
add(16)         g38<1>D         g46<1,1,0>D     16D             { align1 1H compacted };
add(16)         g42<1>D         g42<1,1,0>D     -g94.7<0,1,0>D  { align1 1H I@2 compacted };
cmp.ge.f0.0(16) null<1>UD       g38<8,8,1>UD    g68<8,8,1>UD    { align1 1H @2 $1.dst };
(+f0.0) if(16)  JIP:  LABEL2          UIP:  LABEL2              { align1 1H };
mov(16)         g46<1>UD        g38<8,8,1>UD                    { align1 1H };
mov(16)         g44<1>UD        0x00000000UD                    { align1 1H I@7 };
break(16)       JIP:  LABEL2          UIP:  LABEL1              { align1 1H };

LABEL2:
endif(16)       JIP:  LABEL1                                    { align1 1H };
shl(16)         g94<1>D         g46<8,8,1>D     0x00000002UD    { align1 1H I@4 };
mov(16)         g46<1>UD        g38<8,8,1>UD                    { align1 1H };
add3(16)        g40<1>D         0x0040UW        g94<8,8,1>D     g36<1,1,1>D { align1 1H I@2 };

LABEL1:
while(16)       JIP:  LABEL3                                    { align1 1H };
mov.nz.f0.0(16) null<1>D        g44<8,8,1>D                     { align1 1H I@2 };
(+f0.0) if(16)  JIP:  LABEL5          UIP:  LABEL4              { align1 1H };
fbl(16)         g95<1>UD        g91<8,8,1>UD                    { align1 1H I@6 };
cmp.z.f0.0(16)  null<1>D        g91<8,8,1>D     0D              { align1 1H };
(-f0.0) sel(16) g97<1>UD        g95<8,8,1>UD    0x00000020UD    { align1 1H I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
add(16)         g48<1>D         g46<1,1,0>D     g97<1,1,0>D     { align1 1H compacted };
mov(16)         a0<1>UW         0x0ae0UW                        { align1 WE_all 1H };
shl(16)         a0<1>UW         g97<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0ae0UW        { align1 1H A@1 };
mov(16)         g50<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
else(16)        JIP:  LABEL4          UIP:  LABEL4              { align1 1H };

LABEL5:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
mov(16)         g48<1>UD        0x00000000UD                    { align1 1H };
mov(16)         g50<1>UD        0x00000000UD                    { align1 1H I@3 };

LABEL4:
endif(16)       JIP:  LABEL6                                    { align1 1H };

LABEL6:
add(8)          g98<1>D         g31<8,4,2>D     2092D           { align1 1Q };
add(8)          g99<1>D         g60<8,4,2>D     2092D           { align1 2Q };
shl(16)         g101<1>D        g48<8,8,1>D     0x00000002UD    { align1 1H I@5 };
add(8)          g120<1>D        g31<8,4,2>D     11308D          { align1 1Q };
add(8)          g121<1>D        g60<8,4,2>D     11308D          { align1 2Q };
add(8)          g44<1>D         g33<8,4,2>D     8D              { align1 1Q F@1 compacted };
add(8)          g45<1>D         g62<8,4,2>D     8D              { align1 2Q F@3 compacted };
and(16)         g82<1>UD        g0.2<0,1,0>UD   0x000000ffUD    { align1 1H compacted };
mov(16)         g86<1>D         g35<8,8,1>UW                    { align1 1H };
add(16)         g103<1>D        g98<1,1,0>D     g101<1,1,0>D    { align1 1H I@7 compacted };
cmp.l.f0.0(16)  g100<1>UD       g98<1,1,0>UD    0x0000082cUD    { align1 1H compacted };
shl(16)         g84<1>D         g82<8,8,1>D     0x00000004UD    { align1 1H I@4 };
cmp.l.f0.0(16)  g105<1>UD       g103<1,1,0>UD   g98<1,1,0>UD    { align1 1H I@3 compacted };
mov(8)          g52<2>UD        g103<4,4,1>UD                   { align1 1Q };
mov(8)          g54<2>UD        g104<4,4,1>UD                   { align1 2Q };
cmp.l.f0.0(8)   g122<1>UD       g120<8,8,1>UD   g31<8,4,2>UD    { align1 1Q };
add(16)         g88<1>D         g86<1,1,0>D     g84<1,1,0>D     { align1 1H I@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add3(16)        g107<1>D        -g100<8,8,1>D   g64<8,8,1>D     -g105<1,1,1>D { align1 1H I@5 };
mov(8)          g54.1<2>UD      g108<4,4,1>UD                   { align1 2Q I@1 };
mov(8)          g52.1<2>UD      g107<4,4,1>UD                   { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g108UD          g52UD           nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
mov(16)         g110<1>UD       g108<16,8,2>UW                  { align1 1H $3.dst };
mov(16)         g123<1>UD       g108.1<16,8,2>UW                { align1 1H };
mul(8)          acc0<1>UD       g110<8,8,1>UD   0x0058UW        { align1 1Q I@2 };
asr(16)         g112<1>D        g110<8,8,1>D    0x0000001fUD    { align1 1H I@3 };
mul(16)         g116<1>D        g110<1,1,0>D    88W             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
mul(16)         g125<1>D        g123<1,1,0>D    1484W           { align1 1H compacted };
cmp.l.f0.0(8)   g123<1>UD       g121<8,8,1>UD   g60<8,4,2>UD    { align1 2Q };
and(16)         g59<1>UD        g88<1,1,0>UD    0x000001ffUD    { align1 1H compacted };
mach(8)         g114<1>UD       g110<1,1,0>UD   0x00000058UD    { align1 1Q compacted AccWrEnable };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
add(16)         g52<1>D         g44<1,1,0>D     g116<1,1,0>D    { align1 1H I@5 compacted };
mul(16)         g117<1>D        g112<1,1,0>D    88W             { align1 1H I@7 compacted };
add(16)         g38<1>D         g120<1,1,0>D    g125<1,1,0>D    { align1 1H I@6 compacted };
mul(8)          acc0<1>UD       g111<8,8,1>UD   0x0058UW        { align1 2Q };
mov(8)          g69<2>UD        g52<4,4,1>UD                    { align1 1Q @4 $1.dst };
mov(8)          g71<2>UD        g53<4,4,1>UD                    { align1 2Q I@5 };
cmp.l.f0.0(16)  g40<1>UD        g38<1,1,0>UD    g120<1,1,0>UD   { align1 1H I@4 compacted };
mov(8)          g75<2>UD        g38<4,4,1>UD                    { align1 1Q };
mov(8)          g77<2>UD        g39<4,4,1>UD                    { align1 2Q };
mach(8)         g115<1>UD       g111<8,8,1>UD   0x00000058UD    { align1 2Q AccWrEnable };
cmp.l.f0.0(8)   g46<1>UD        g45<8,8,1>UD    g62<8,4,2>UD    { align1 2Q };
cmp.l.f0.0(8)   g121<1>UD       g44<8,8,1>UD    g33<8,4,2>UD    { align1 1Q };
cmp.l.f0.0(16)  g54<1>UD        g52<1,1,0>UD    g44<1,1,0>UD    { align1 1H $3.src compacted };
add3(16)        g42<1>D         -g122<8,8,1>D   g64<8,8,1>D     -g40<1,1,1>D { align1 1H I@7 };
add(16)         g119<1>D        g114<1,1,0>D    g117<1,1,0>D    { align1 1H I@5 compacted };
add(8)          g48<1>D         -g46<8,8,1>D    g62.1<8,4,2>D   { align1 2Q I@5 };
add(8)          g47<1>D         -g121<8,8,1>D   g33.1<8,4,2>D   { align1 1Q I@5 };
mov(8)          g77.1<2>UD      g43<4,4,1>UD                    { align1 2Q I@4 };
mov(8)          g75.1<2>UD      g42<4,4,1>UD                    { align1 1Q I@5 };
add3(16)        g56<1>D         g47<8,8,1>D     g119<8,8,1>D    -g54<1,1,1>D { align1 1H I@3 };
mov(8)          g71.1<2>UD      g57<4,4,1>UD                    { align1 2Q I@1 };
mov(8)          g69.1<2>UD      g56<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g61UD           g69UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@4 };
send(16)        g69UD           g75UD           nullUD          0x08607582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyz, L1STATE_L3MOCS dst_len = 6, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $5 };
add(16)         g90<1>D         g65<1,1,0>D     32D             { align1 1H $4.dst compacted };
shl(16)         g95<1>D         g73<8,8,1>D     0x00000005UD    { align1 1H $5.dst };
cmp.l.f0.0(16)  g92<1>UD        g90<1,1,0>UD    g65<1,1,0>UD    { align1 1H I@2 compacted };
add(16)         g98<1>D         g90<1,1,0>D     g95<1,1,0>D     { align1 1H I@2 compacted };
shr(16)         g96<1>UD        g73<1,1,0>UD    0x0000001bUD    { align1 1H compacted };
add(16)         g94<1>D         -g92<1,1,0>D    g67<1,1,0>D     { align1 1H @3 $4.dst compacted };
cmp.l.f0.0(16)  g100<1>UD       g98<1,1,0>UD    g90<1,1,0>UD    { align1 1H I@3 compacted };
mov(8)          g76<2>UD        g98<4,4,1>UD                    { align1 1Q $5.src };
mov(8)          g78<2>UD        g99<4,4,1>UD                    { align1 2Q $5.src };
add(16)         g104<1>D        g98<1,1,0>D     16D             { align1 1H compacted };
add3(16)        g102<1>D        g94<8,8,1>D     g96<8,8,1>D     -g100<1,1,1>D { align1 1H I@4 };
cmp.l.f0.0(16)  g106<1>UD       g104<1,1,0>UD   0x00000010UD    { align1 1H I@2 compacted };
mov(8)          g78.1<2>UD      g103<4,4,1>UD                   { align1 2Q I@2 };
mov(8)          g76.1<2>UD      g102<4,4,1>UD                   { align1 1Q I@3 };
add(16)         g108<1>D        -g106<1,1,0>D   g102<1,1,0>D    { align1 1H I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        g1UD            g76UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.src };
shl(16)         g75<1>D         g50<8,8,1>D     0x00000009UD    { align1 1H $5.src };
mov(8)          g77<2>UD        g104<4,4,1>UD                   { align1 1Q $6.src };
mov(8)          g79<2>UD        g105<4,4,1>UD                   { align1 2Q $6.src };
mov(8)          g77.1<2>UD      g108<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g79.1<2>UD      g109<4,4,1>UD                   { align1 2Q I@2 };
add(16)         g110<1>F        g7<1,1,0>F      -g1<1,1,0>F     { align1 1H $6.dst compacted };
sel.ge(16)      g116<1>F        g110<8,8,1>F    0x704ec3dF  /* 1e-34F */ { align1 1H F@1 };
cmp.l.f0.0(16)  g14<1>F         g116<8,8,1>F    0x77f684dfF  /* 1e+34F */ { align1 1H F@1 };
math inv(16)    g119<1>F        g116<8,8,1>F    null<8,8,1>F    { align1 1H $7 };
cmp.g.f0.0(16)  g16<1>F         g110<8,8,1>F    0x704ec3dF  /* 1e-34F */ { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        g109UD          g77UD           nullUD          0x08403582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 4, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $8 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
add(16)         g77<1>D         g69<1,1,0>D     g75<1,1,0>D     { align1 1H @5 $5.dst compacted };
mul(16)         g121<1>F        g119<8,8,1>F    0x417d70a4F  /* 15.84F */ { align1 1H $7.dst };
and.nz.f0.0(16) null<1>UD       g14<8,8,1>UD    g16<8,8,1>UD    { align1 1H F@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $8.src };
add(16)         g79<1>D         g77<1,1,0>D     512D            { align1 1H I@2 compacted };
add(16)         g89<1>D         g77<1,1,0>D     g59<1,1,0>D     { align1 1H compacted };
(+f0.0) sel(16) g9<1>UD         g121<1,1,0>UD   0x00000000UD    { align1 1H F@1 compacted };
sel.l(16)       g81<1>UD        g79<1,1,0>UD    g71<1,1,0>UD    { align1 1H @3 $5.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.dst };
add(16)         g113<1>F        g109<1,1,0>F    -g3<1,1,0>F     { align1 1H $8.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
add(16)         g115<1>F        g111<1,1,0>F    -g5<1,1,0>F     { align1 1H $8.dst compacted };
cmp.g.f0.0(16)  g17<1>F         g113<8,8,1>F    0x704ec3dF  /* 1e-34F */ { align1 1H A@2 };
sel.ge(16)      g117<1>F        g113<8,8,1>F    0x704ec3dF  /* 1e-34F */ { align1 1H $7.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
math inv(16)    g122<1>F        g117<8,8,1>F    null<8,8,1>F    { align1 1H $9 };
cmp.l.f0.0(16)  g19<1>F         g117<8,8,1>F    0x77f684dfF  /* 1e+34F */ { align1 1H $9.src };
sel.ge(16)      g118<1>F        g115<8,8,1>F    0x704ec3dF  /* 1e-34F */ { align1 1H F@4 };
mul(16)         g124<1>F        g122<8,8,1>F    0x417d70a4F  /* 15.84F */ { align1 1H $9.dst };
and.nz.f0.0(16) null<1>UD       g19<8,8,1>UD    g17<8,8,1>UD    { align1 1H F@3 };
(+f0.0) sel(16) g11<1>UD        g124<1,1,0>UD   0x00000000UD    { align1 1H F@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
math inv(16)    g125<1>F        g118<8,8,1>F    null<8,8,1>F    { align1 1H $10 };
cmp.g.f0.0(16)  g20<1>F         g115<8,8,1>F    0x704ec3dF  /* 1e-34F */ { align1 1H I@2 };
cmp.l.f0.0(16)  g22<1>F         g118<8,8,1>F    0x77f684dfF  /* 1e+34F */ { align1 1H $10.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $10.dst };
mul(16)         g13<1>F         g125<8,8,1>F    0x417d70a4F  /* 15.84F */ { align1 1H I@7 };
and.nz.f0.0(16) null<1>UD       g22<8,8,1>UD    g20<8,8,1>UD    { align1 1H F@2 };
(+f0.0) sel(16) g40<1>UD        g13<1,1,0>UD    0x00000000UD    { align1 1H F@1 compacted };
cmp.l.f0.0(16)  g44<1>UD        g89<1,1,0>UD    g81<1,1,0>UD    { align1 1H I@5 compacted };
(+f0.0) if(16)  JIP:  LABEL7          UIP:  LABEL7              { align1 1H };
shl(16)         g46<1>D         g89<8,8,1>D     0x00000005UD    { align1 1H };
add(16)         g49<1>D         g61<1,1,0>D     g46<1,1,0>D     { align1 1H @1 $4.dst compacted };
shr(16)         g47<1>UD        g89<1,1,0>UD    0x0000001bUD    { align1 1H compacted };
cmp.l.f0.0(16)  g51<1>UD        g49<1,1,0>UD    g61<1,1,0>UD    { align1 1H I@2 compacted };
add(16)         g55<1>D         g49<1,1,0>D     16D             { align1 1H compacted };
mov(8)          g78<2>UD        g49<4,4,1>UD                    { align1 1Q };
mov(8)          g80<2>UD        g50<4,4,1>UD                    { align1 2Q };
add3(16)        g53<1>D         g63<8,8,1>D     g47<8,8,1>D     -g51<1,1,1>D { align1 1H @4 $4.dst };
cmp.l.f0.0(16)  g57<1>UD        g55<1,1,0>UD    g49<1,1,0>UD    { align1 1H I@4 compacted };
mov(8)          g82<2>UD        g55<4,4,1>UD                    { align1 1Q };
mov(8)          g84<2>UD        g56<4,4,1>UD                    { align1 2Q };
mov(8)          g80.1<2>UD      g54<4,4,1>UD                    { align1 2Q I@4 };
mov(8)          g78.1<2>UD      g53<4,4,1>UD                    { align1 1Q I@5 };
add(16)         g61<1>D         -g57<1,1,0>D    g53<1,1,0>D     { align1 1H I@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g13UD           g78UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $11 };
mov(8)          g82.1<2>UD      g61<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g84.1<2>UD      g62<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g21UD           g82UD           nullUD          0x0880f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 8, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $12 };

LABEL7:
endif(16)       JIP:  LABEL8                                    { align1 1H };

LABEL8:
shl(16)         g53<1>D         g59<8,8,1>D     0x00000002UD    { align1 1H };
mov(16)         g51<1>UD        g53<8,8,1>UD                    { align1 1H I@1 };

LABEL10:
cmp.ge.f0.0(16) null<1>UD       g51<8,8,1>UD    0x00000120UD    { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL9        UIP:  LABEL9              { align1 1H };
shl(16)         g83<1>D         g51<8,8,1>D     0x00000002UD    { align1 1H $12.src };
mov(16)         g29<1>UD        0x7f800000UD                    { align1 1H $12.src };
mov(16)         g31<1>UD        0x7f800000UD                    { align1 1H $12.src };
mov(16)         g33<1>UD        0x7f800000UD                    { align1 1H $12.src };
mov(16)         g35<1>UD        0x7f800000UD                    { align1 1H $12.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g83UD           g29UD           0x0400f506                0x00000200
                            slm MsgDesc: ( store_cmask, a32, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 8 flat )  base_offset 0  { align1 1H $12 };
add(16)         g51<1>D         g51<8,8,1>D     2048D           { align1 1H };

LABEL9:
while(16)       JIP:  LABEL10                                   { align1 1H };

LABEL12:
cmp.ge.f0.0(16) null<1>UD       g53<8,8,1>UD    0x00000030UD    { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL11       UIP:  LABEL11             { align1 1H };
shl(16)         g62<1>D         g53<8,8,1>D     0x00000002UD    { align1 1H $4.dst };
mov(16)         g30<1>UD        0x00000000UD                    { align1 1H $12.src };
mov(16)         g32<1>UD        0x00000000UD                    { align1 1H $12.src };
mov(16)         g34<1>UD        0x00000000UD                    { align1 1H $12.src };
mov(16)         g36<1>UD        0x00000000UD                    { align1 1H $12.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
add(16)         g84<1>D         g62<1,1,0>D     1152D           { align1 1H I@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g84UD           g30UD           0x0400f506                0x00000200
                            slm MsgDesc: ( store_cmask, a32, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 8 flat )  base_offset 0  { align1 1H $12 };
add(16)         g53<1>D         g53<8,8,1>D     2048D           { align1 1H };

LABEL11:
while(16)       JIP:  LABEL12                                   { align1 1H };
sync allwr(1)                   null<0,1,0>UB                   { align1 WE_all 1N };
sync allwr(1)                   null<0,1,0>UB                   { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@5 };
send(1)         g63UD           g0UD            nullUD          0x0210011f                0x00000000
                            slm MsgDesc: ( fence, a32, threadgroup, none, normal_routing dst_len = 1, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $13 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.dst };
mov(8)          g64<1>UD        0x00000000UD                    { align1 WE_all 1Q $4.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $13.src };
mov(2)          g64.10<1>UB     g0.11<0,1,0>UB                  { align1 WE_all 1N I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g64UD           nullUD          0x02000004                0x00000000
                            gateway MsgDesc: (barrier msg)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H $14 };
sync bar(16)                    null<0,1,0>UB                   { align1 WE_all 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.dst };
add(16)         g65<1>F         g13<1,1,0>F     g21<1,1,0>F     { align1 1H $11.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.dst };
add(16)         g67<1>F         g15<1,1,0>F     g23<1,1,0>F     { align1 1H $11.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.dst };
add(16)         g69<1>F         g17<1,1,0>F     g25<1,1,0>F     { align1 1H $11.dst compacted };
add(16)         g73<1>F         g69<1,1,0>F     -g5<1,1,0>F     { align1 1H F@1 compacted };
mov(1)          g125<2>UW       0x00000000UD                    { align1 WE_all 1N F@5 };
add(16)         g70<1>F         g65<1,1,0>F     -g1<1,1,0>F     { align1 1H F@4 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mul(16)         g78<1>F         g73<1,1,0>F     g40<1,1,0>F     { align1 1H A@2 compacted };
add(16)         g72<1>F         g67<1,1,0>F     -g3<1,1,0>F     { align1 1H F@5 compacted };
mul(16)         g74<1>F         g70<1,1,0>F     g9<1,1,0>F      { align1 1H F@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mov(16)         g84<1>UD        g78<8,8,1>F                     { align1 1H F@3 };
mul(16)         g76<1>F         g72<1,1,0>F     g11<1,1,0>F     { align1 1H F@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.src };
mov(16)         g80<1>UD        g74<8,8,1>F                     { align1 1H F@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mov(16)         g82<1>UD        g76<8,8,1>F                     { align1 1H F@1 };
mov(1)          f0<1>UW         g125<0,1,0>UW                   { align1 WE_all 1N I@4 };
cmp.nz.f0.0(16) null<1>UD       g44<8,8,1>UD    0x00000000UD    { align1 1H };
mov(16)         g54<1>UD        f0<0,1,0>UW                     { align1 1H };
mov(1)          g122<1>D        1D                              { align1 WE_all 1N };
and(16)         g88<1>UD        g86<1,1,0>UD    0x00000003UD    { align1 1H compacted };
shr(16)         g90<1>UD        g86<1,1,0>UD    0x00000002UD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
shl(16)         g92<1>D         g122<0,1,0>D    g86<8,8,1>UD    { align1 1H };

LABEL15:
cmp.z.f0.0(16)  null<1>D        g54<8,8,1>D     0D              { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL13       UIP:  LABEL13             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
fbl(16)         g94<1>UD        g54<8,8,1>UD                    { align1 1H F@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
mov(16)         a0<1>UW         0x0a00UW                        { align1 WE_all 1H I@1 };
shl(16)         a0<1>UW         g94<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0a00UW        { align1 1H A@1 };
mov(16)         g96<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
cmp.z.f0.0(16)  g98<1>D         g96<1,1,0>D     g80<1,1,0>D     { align1 1H I@1 compacted };
and.nz.f0.0(16) null<1>UD       g98<8,8,1>UD    g44<8,8,1>UD    { align1 1H I@1 };
(+f0.0) sel(16) g100<1>UD       g21<8,8,1>UD    0xff800000UD    { align1 1H };
(+f0.0) sel(16) g102<1>UD       g23<8,8,1>UD    0xff800000UD    { align1 1H };
(+f0.0) sel(16) g104<1>UD       g13<8,8,1>UD    0x7f800000UD    { align1 1H };
(+f0.0) sel(16) g106<1>UD       g15<8,8,1>UD    0x7f800000UD    { align1 1H };
mov(16)         g115<1>F        0xff800000F      /* -infF */    { align1 WE_all 1H I@4 compacted };
mov(16)         g115<1>F        g100<1,1,0>F                    { align1 1H compacted };
mov(16)         g117<1>F        0xff800000F      /* -infF */    { align1 WE_all 1H I@3 compacted };
mov(16)         g117<1>F        g102<1,1,0>F                    { align1 1H compacted };
mov(16)         g109<1>F        0x7f800000F      /* infF */     { align1 WE_all 1H I@2 compacted };
mov(16)         g109<1>F        g104<1,1,0>F                    { align1 1H compacted };
mov(16)         g111<1>F        0x7f800000F      /* infF */     { align1 WE_all 1H I@1 compacted };
mov(16)         g111<1>F        g106<1,1,0>F                    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@2 };
(+f0.0) sel(16) g103<1>UD       g25<8,8,1>UD    0xff800000UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
(+f0.0) sel(16) g107<1>UD       g17<8,8,1>UD    0x7f800000UD    { align1 1H };
mov(8)          g69<2>UD        g115.1<8,4,2>UD                 { align1 WE_all 1Q F@4 };
mov(8)          g100<2>UD       g117.1<8,4,2>UD                 { align1 WE_all 1Q F@3 };
mov(8)          g1<2>UD         g109.1<8,4,2>UD                 { align1 WE_all 1Q F@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mov(8)          g31<2>UD        g111.1<8,4,2>UD                 { align1 WE_all 1Q F@1 };
mov(16)         g119<1>F        0xff800000F      /* -infF */    { align1 WE_all 1H A@6 compacted };
mov(16)         g119<1>F        g103<1,1,0>F                    { align1 1H compacted };
mov(16)         g113<1>F        0x7f800000F      /* infF */     { align1 WE_all 1H I@5 compacted };
mov(16)         g113<1>F        g107<1,1,0>F                    { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g88<8,8,1>D     1D              { align1 1H };
sel.ge(8)       g67<2>F         g115<8,4,2>F    g69<8,4,2>F     { align1 WE_all 1Q A@5 };
sel.ge(8)       g94<2>F         g117<8,4,2>F    g100<8,4,2>F    { align1 WE_all 1Q I@4 };
sel.l(8)        g126<2>F        g109<8,4,2>F    g1<8,4,2>F      { align1 WE_all 1Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
sel.l(8)        g29<2>F         g111<8,4,2>F    g31<8,4,2>F     { align1 WE_all 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mov(8)          g51<2>UD        g113.1<8,4,2>UD                 { align1 WE_all 1Q F@5 };
mov(8)          g115.1<2>UD     g67<8,4,2>UD                    { align1 WE_all 1Q A@4 };
mov(8)          g117.1<2>UD     g94<8,4,2>UD                    { align1 WE_all 1Q A@3 };
mov(8)          g109.1<2>UD     g126<8,4,2>UD                   { align1 WE_all 1Q A@2 };
mov(8)          g111.1<2>UD     g29<8,4,2>UD                    { align1 WE_all 1Q A@1 };
sel.l(8)        g49<2>F         g113<8,4,2>F    g51<8,4,2>F     { align1 WE_all 1Q I@5 };
mov(4)          g74<4>UD        g115.2<8,2,4>UD                 { align1 WE_all 1N I@4 };
mov(4)          g72<4>UD        g115.1<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g105<4>UD       g117.2<8,2,4>UD                 { align1 WE_all 1N A@5 };
mov(4)          g103<4>UD       g117.1<8,2,4>UD                 { align1 WE_all 1N F@7 };
mov(4)          g6<4>UD         g109.2<8,2,4>UD                 { align1 WE_all 1N I@6 };
mov(4)          g4<4>UD         g109.1<8,2,4>UD                 { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mov(4)          g36<4>UD        g111.2<8,2,4>UD                 { align1 WE_all 1N I@7 };
mov(4)          g34<4>UD        g111.1<8,2,4>UD                 { align1 WE_all 1N $12.src };
mov(8)          g126<2>UD       g119.1<8,4,2>UD                 { align1 WE_all 1Q F@7 };
mov(8)          g113.1<2>UD     g49<8,4,2>UD                    { align1 WE_all 1Q F@1 };
sel.ge(4)       g70<4>F         g72<8,2,4>F     g74<8,2,4>F     { align1 WE_all 1N I@7 };
sel.ge(4)       g101<4>F        g103<8,2,4>F    g105<8,2,4>F    { align1 WE_all 1N I@7 };
sel.l(4)        g2<4>F          g4<8,2,4>F      g6<8,2,4>F      { align1 WE_all 1N I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
sel.l(4)        g32<4>F         g34<8,2,4>F     g36<8,2,4>F     { align1 WE_all 1N I@3 };
sel.ge(8)       g124<2>F        g119<8,4,2>F    g126<8,4,2>F    { align1 WE_all 1Q I@2 };
mov(4)          g61<4>UD        g113.2<8,2,4>UD                 { align1 WE_all 1N @1 $4.dst };
mov(4)          g56<4>UD        g113.1<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g115.2<4>UD     g70<8,2,4>UD                    { align1 WE_all 1N F@5 };
mov(4)          g117.2<4>UD     g101<8,2,4>UD                   { align1 WE_all 1N F@4 };
mov(4)          g109.2<4>UD     g2<8,2,4>UD                     { align1 WE_all 1N F@3 };
mov(4)          g111.2<4>UD     g32<8,2,4>UD                    { align1 WE_all 1N F@2 };
mov(8)          g119.1<2>UD     g124<8,4,2>UD                   { align1 WE_all 1Q A@1 };
sel.l(4)        g52<4>F         g56<8,2,4>F     g61<8,2,4>F     { align1 WE_all 1N I@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
mov(4)          g86<4>UD        g115.3<8,2,4>UD                 { align1 WE_all 1N I@5 };
mov(4)          g77<4>UD        g115.1<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g121<4>UD       g117.1<8,2,4>UD                 { align1 WE_all 1N I@6 };
mov(4)          g11<4>UD        g109.3<8,2,4>UD                 { align1 WE_all 1N I@6 };
mov(4)          g9<4>UD         g109.1<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g48<4>UD        g111.3<8,2,4>UD                 { align1 WE_all 1N I@7 };
mov(4)          g46<4>UD        g111.1<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g5<4>UD         g119.2<8,2,4>UD                 { align1 WE_all 1N A@4 };
mov(4)          g3<4>UD         g119.1<8,2,4>UD                 { align1 WE_all 1N F@4 };
mov(4)          g123<4>UD       g117.3<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g113.2<4>UD     g52<8,2,4>UD                    { align1 WE_all 1N F@1 };
sel.ge(4)       g75<4>F         g77<8,2,4>F     g86<8,2,4>F     { align1 WE_all 1N I@7 };
sel.l(4)        g7<4>F          g9<8,2,4>F      g11<8,2,4>F     { align1 WE_all 1N I@7 };
sel.l(4)        g40<4>F         g46<8,2,4>F     g48<8,2,4>F     { align1 WE_all 1N I@5 };
sel.ge(4)       g1<4>F          g3<8,2,4>F      g5<8,2,4>F      { align1 WE_all 1N I@3 };
sel.ge(4)       g106<4>F        g121<8,2,4>F    g123<8,2,4>F    { align1 WE_all 1N I@2 };
mov(4)          g66<4>UD        g113.3<8,2,4>UD                 { align1 WE_all 1N I@1 };
mov(4)          g64<4>UD        g113.1<8,2,4>UD                 { align1 WE_all 1N $14.src };
mov(4)          g115.3<4>UD     g75<8,2,4>UD                    { align1 WE_all 1N F@5 };
mov(4)          g109.3<4>UD     g7<8,2,4>UD                     { align1 WE_all 1N F@4 };
mov(4)          g111.3<4>UD     g40<8,2,4>UD                    { align1 WE_all 1N F@3 };
mov(4)          g119.2<4>UD     g1<8,2,4>UD                     { align1 WE_all 1N F@2 };
mov(4)          g117.3<4>UD     g106<8,2,4>UD                   { align1 WE_all 1N F@1 };
sel.l(4)        g62<4>F         g64<8,2,4>F     g66<8,2,4>F     { align1 WE_all 1N I@6 };
sel.ge(4)       g115.4<1>F      g115.3<0,1,0>F  g115.4<4,4,1>F  { align1 WE_all 1N I@5 };
sel.ge(4)       g116.4<1>F      g116.3<0,1,0>F  g116.4<4,4,1>F  { align1 WE_all 1N I@5 };
sel.l(4)        g109.4<1>F      g109.3<0,1,0>F  g109.4<4,4,1>F  { align1 WE_all 1N I@4 };
sel.l(4)        g110.4<1>F      g110.3<0,1,0>F  g110.4<4,4,1>F  { align1 WE_all 1N I@4 };
sel.l(4)        g111.4<1>F      g111.3<0,1,0>F  g111.4<4,4,1>F  { align1 WE_all 1N I@3 };
sel.l(4)        g112.4<1>F      g112.3<0,1,0>F  g112.4<4,4,1>F  { align1 WE_all 1N I@3 };
mov(4)          g10<4>UD        g119.3<8,2,4>UD                 { align1 WE_all 1N I@2 };
mov(4)          g8<4>UD         g119.1<8,2,4>UD                 { align1 WE_all 1N };
sel.ge(4)       g117.4<1>F      g117.3<0,1,0>F  g117.4<4,4,1>F  { align1 WE_all 1N I@3 };
sel.ge(4)       g118.4<1>F      g118.3<0,1,0>F  g118.4<4,4,1>F  { align1 WE_all 1N I@3 };
mov(4)          g113.3<4>UD     g62<8,2,4>UD                    { align1 WE_all 1N F@7 };
sel.ge(8)       g116<1>F        g115.7<0,1,0>F  g116<8,8,1>F    { align1 WE_all 1Q F@7 };
sel.l(8)        g110<1>F        g109.7<0,1,0>F  g110<8,8,1>F    { align1 WE_all 1Q F@6 };
sel.l(8)        g112<1>F        g111.7<0,1,0>F  g112<8,8,1>F    { align1 WE_all 1Q F@5 };
sel.ge(4)       g6<4>F          g8<8,2,4>F      g10<8,2,4>F     { align1 WE_all 1N I@2 };
sel.ge(8)       g118<1>F        g117.7<0,1,0>F  g118<8,8,1>F    { align1 WE_all 1Q F@5 };
sel.l(4)        g113.4<1>F      g113.3<0,1,0>F  g113.4<4,4,1>F  { align1 WE_all 1N I@1 };
sel.l(4)        g114.4<1>F      g114.3<0,1,0>F  g114.4<4,4,1>F  { align1 WE_all 1N I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@5 };
(+f0.0) sel(16) g121<1>UD       g112.7<0,1,0>UD g110.7<0,1,0>UD { align1 1H };
mov(4)          g119.3<4>UD     g6<8,2,4>UD                     { align1 WE_all 1N F@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@3 };
(+f0.0) sel(16) g123<1>UD       g118.7<0,1,0>UD g116.7<0,1,0>UD { align1 1H };
sel.l(8)        g114<1>F        g113.7<0,1,0>F  g114<8,8,1>F    { align1 WE_all 1Q F@1 };
sel.ge(4)       g119.4<1>F      g119.3<0,1,0>F  g119.4<4,4,1>F  { align1 WE_all 1N I@2 };
sel.ge(4)       g120.4<1>F      g120.3<0,1,0>F  g120.4<4,4,1>F  { align1 WE_all 1N I@2 };
cmp.z.f0.0(16)  null<1>D        g88<8,8,1>D     2D              { align1 1H };
sel.ge(8)       g120<1>F        g119.7<0,1,0>F  g120<8,8,1>F    { align1 WE_all 1Q F@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@4 };
(+f0.0) sel(16) g125<1>UD       g114.7<0,1,0>UD g121<1,1,0>UD   { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
(+f0.0) sel(16) g1<1>UD         g120.7<0,1,0>UD g123<1,1,0>UD   { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g90<8,8,1>D     0D              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
(+f0.0) sel(16) g94<1>F         g125<1,1,0>F    -g1<1,1,0>F     { align1 1H compacted };
and.nz.f0.0(16) null<1>UD       g92<8,8,1>UD    0x00000077UD    { align1 1H };
(+f0.0) if(16)  JIP:  LABEL14         UIP:  LABEL14             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
mul(16)         g2<1>D          g96<1,1,0>D     6W              { align1 1H compacted };
mul(16)         g4<1>D          g90<1,1,0>D     3W              { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
add3(16)        g6<1>D          g2<8,8,1>D      g88<8,8,1>D     g4<1,1,1>D { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
shl(16)         g86<1>D         g6<8,8,1>D      0x00000002UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        nullUD          g86UD           g94UD           0x04040515                0x00000080
                            slm MsgDesc: ( atomic_fmin, a32, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1H $15 };

LABEL14:
endif(16)       JIP:  LABEL13                                   { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@2 };
or(16)          g7<1>D          ~g98<1,1,0>D    ~g44<1,1,0>D    { align1 1H compacted };
mov(1)          g11<2>UW        0x00000000UD                    { align1 WE_all 1N F@7 };
mov(1)          f0<1>UW         g11<0,1,0>UW                    { align1 WE_all 1N I@1 };
cmp.nz.f0.0(16) null<1>UD       g7<8,8,1>UD     0x00000000UD    { align1 1H I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@7 };
mov(16)         g8<1>UD         f0<0,1,0>UW                     { align1 1H };
and(16)         g54<1>UD        g54<1,1,0>UD    g8<1,1,0>UD     { align1 1H I@1 compacted };

LABEL13:
while(16)       JIP:  LABEL15                                   { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@2 };
add(16)         g9<1>D          g82<1,1,0>D     16D             { align1 1H compacted };
mov(1)          g12<2>UW        0x00000000UD                    { align1 WE_all 1N F@1 };
mov(1)          f0<1>UW         g12<0,1,0>UW                    { align1 WE_all 1N I@1 };
cmp.nz.f0.0(16) null<1>UD       g44<8,8,1>UD    0x00000000UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@6 };
mov(16)         g55<1>UD        f0<0,1,0>UW                     { align1 1H };

LABEL18:
cmp.z.f0.0(16)  null<1>D        g55<8,8,1>D     0D              { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL16       UIP:  LABEL16             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
fbl(16)         g11<1>UD        g55<8,8,1>UD                    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mov(16)         a0<1>UW         0x0120UW                        { align1 WE_all 1H I@1 };
shl(16)         a0<1>UW         g11<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0120UW        { align1 1H A@1 };
mov(16)         g29<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
cmp.z.f0.0(16)  g31<1>D         g29<1,1,0>D     g9<1,1,0>D      { align1 1H compacted };
and.nz.f0.0(16) null<1>UD       g31<8,8,1>UD    g44<8,8,1>UD    { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
(+f0.0) sel(16) g33<1>UD        g21<8,8,1>UD    0xff800000UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
(+f0.0) sel(16) g35<1>UD        g23<8,8,1>UD    0xff800000UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@4 };
(+f0.0) sel(16) g40<1>UD        g13<8,8,1>UD    0x7f800000UD    { align1 1H };
(+f0.0) sel(16) g46<1>UD        g15<8,8,1>UD    0x7f800000UD    { align1 1H };
mov(16)         g57<1>F         0xff800000F      /* -infF */    { align1 WE_all 1H I@4 compacted };
mov(16)         g57<1>F         g33<1,1,0>F                     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.dst };
mov(16)         g61<1>F         0xff800000F      /* -infF */    { align1 WE_all 1H I@3 compacted };
mov(16)         g61<1>F         g35<1,1,0>F                     { align1 1H compacted };
mov(16)         g49<1>F         0x7f800000F      /* infF */     { align1 WE_all 1H I@2 compacted };
mov(16)         g49<1>F         g40<1,1,0>F                     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@2 };
(+f0.0) sel(16) g36<1>UD        g25<8,8,1>UD    0xff800000UD    { align1 1H };
mov(16)         g51<1>F         0x7f800000F      /* infF */     { align1 WE_all 1H I@2 compacted };
mov(16)         g51<1>F         g46<1,1,0>F                     { align1 1H compacted };
mov(8)          g114<2>UD       g57.1<8,4,2>UD                  { align1 WE_all 1Q F@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
(+f0.0) sel(16) g47<1>UD        g17<8,8,1>UD    0x7f800000UD    { align1 1H };
mov(8)          g1<2>UD         g61.1<8,4,2>UD                  { align1 WE_all 1Q F@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
mov(16)         g63<1>F         0xff800000F      /* -infF */    { align1 WE_all 1H A@4 compacted };
mov(16)         g63<1>F         g36<1,1,0>F                     { align1 1H compacted };
mov(8)          g75<2>UD        g51.1<8,4,2>UD                  { align1 WE_all 1Q F@2 };
mov(8)          g40<2>UD        g49.1<8,4,2>UD                  { align1 WE_all 1Q F@3 };
sel.ge(8)       g112<2>F        g57<8,4,2>F     g114<8,4,2>F    { align1 WE_all 1Q A@5 };
mov(16)         g53<1>F         0x7f800000F      /* infF */     { align1 WE_all 1H I@4 compacted };
mov(16)         g53<1>F         g47<1,1,0>F                     { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g88<8,8,1>D     1D              { align1 1H };
sel.ge(8)       g125<2>F        g61<8,4,2>F     g1<8,4,2>F      { align1 WE_all 1Q A@4 };
mov(8)          g36<2>UD        g63.1<8,4,2>UD                  { align1 WE_all 1Q F@4 };
sel.l(8)        g73<2>F         g51<8,4,2>F     g75<8,4,2>F     { align1 WE_all 1Q A@4 };
sel.l(8)        g33<2>F         g49<8,4,2>F     g40<8,4,2>F     { align1 WE_all 1Q I@3 };
mov(8)          g57.1<2>UD      g112<8,4,2>UD                   { align1 WE_all 1Q A@5 };
mov(8)          g101<2>UD       g53.1<8,4,2>UD                  { align1 WE_all 1Q F@4 };
mov(8)          g61.1<2>UD      g125<8,4,2>UD                   { align1 WE_all 1Q A@3 };
mov(8)          g51.1<2>UD      g73<8,4,2>UD                    { align1 WE_all 1Q A@2 };
mov(8)          g49.1<2>UD      g33<8,4,2>UD                    { align1 WE_all 1Q A@1 };
mov(4)          g119<4>UD       g57.2<8,2,4>UD                  { align1 WE_all 1N I@5 };
mov(4)          g117<4>UD       g57.1<8,2,4>UD                  { align1 WE_all 1N };
sel.l(8)        g99<2>F         g53<8,4,2>F     g101<8,4,2>F    { align1 WE_all 1Q I@6 };
mov(4)          g6<4>UD         g61.2<8,2,4>UD                  { align1 WE_all 1N I@5 };
mov(4)          g4<4>UD         g61.1<8,2,4>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
mov(4)          g86<4>UD        g51.2<8,2,4>UD                  { align1 WE_all 1N I@6 };
mov(4)          g78<4>UD        g51.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g67<4>UD        g49.2<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g65<4>UD        g49.1<8,2,4>UD                  { align1 WE_all 1N };
sel.ge(4)       g115<4>F        g117<8,2,4>F    g119<8,2,4>F    { align1 WE_all 1N I@7 };
sel.ge(8)       g34<2>F         g63<8,4,2>F     g36<8,4,2>F     { align1 WE_all 1Q I@7 };
mov(8)          g53.1<2>UD      g99<8,4,2>UD                    { align1 WE_all 1Q F@3 };
sel.ge(4)       g2<4>F          g4<8,2,4>F      g6<8,2,4>F      { align1 WE_all 1N I@6 };
sel.l(4)        g76<4>F         g78<8,2,4>F     g86<8,2,4>F     { align1 WE_all 1N I@4 };
sel.l(4)        g46<4>F         g65<8,2,4>F     g67<8,2,4>F     { align1 WE_all 1N I@2 };
mov(4)          g57.2<4>UD      g115<8,2,4>UD                   { align1 WE_all 1N F@5 };
mov(8)          g63.1<2>UD      g34<8,4,2>UD                    { align1 WE_all 1Q F@4 };
mov(4)          g106<4>UD       g53.2<8,2,4>UD                  { align1 WE_all 1N I@3 };
mov(4)          g104<4>UD       g53.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g61.2<4>UD      g2<8,2,4>UD                     { align1 WE_all 1N F@3 };
mov(4)          g51.2<4>UD      g76<8,2,4>UD                    { align1 WE_all 1N F@2 };
mov(4)          g49.2<4>UD      g46<8,2,4>UD                    { align1 WE_all 1N F@1 };
mov(4)          g124<4>UD       g57.3<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g122<4>UD       g57.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g65<4>UD        g63.2<8,2,4>UD                  { align1 WE_all 1N A@1 };
sel.l(4)        g102<4>F        g104<8,2,4>F    g106<8,2,4>F    { align1 WE_all 1N I@7 };
mov(4)          g11<4>UD        g61.1<8,2,4>UD                  { align1 WE_all 1N I@6 };
mov(4)          g33<4>UD        g61.3<8,2,4>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
mov(4)          g98<4>UD        g51.3<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g96<4>UD        g51.1<8,2,4>UD                  { align1 WE_all 1N $15.src };
mov(4)          g72<4>UD        g49.3<8,2,4>UD                  { align1 WE_all 1N I@7 };
mov(4)          g70<4>UD        g49.1<8,2,4>UD                  { align1 WE_all 1N };
sel.ge(4)       g120<4>F        g122<8,2,4>F    g124<8,2,4>F    { align1 WE_all 1N I@7 };
mov(4)          g46<4>UD        g63.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g53.2<4>UD      g102<8,2,4>UD                   { align1 WE_all 1N F@2 };
sel.ge(4)       g7<4>F          g11<8,2,4>F     g33<8,2,4>F     { align1 WE_all 1N I@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
sel.l(4)        g94<4>F         g96<8,2,4>F     g98<8,2,4>F     { align1 WE_all 1N I@5 };
sel.l(4)        g68<4>F         g70<8,2,4>F     g72<8,2,4>F     { align1 WE_all 1N I@3 };
mov(4)          g57.3<4>UD      g120<8,2,4>UD                   { align1 WE_all 1N F@4 };
sel.ge(4)       g40<4>F         g46<8,2,4>F     g65<8,2,4>F     { align1 WE_all 1N I@3 };
mov(4)          g111<4>UD       g53.3<8,2,4>UD                  { align1 WE_all 1N I@2 };
mov(4)          g109<4>UD       g53.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g61.3<4>UD      g7<8,2,4>UD                     { align1 WE_all 1N F@4 };
mov(4)          g51.3<4>UD      g94<8,2,4>UD                    { align1 WE_all 1N F@3 };
mov(4)          g49.3<4>UD      g68<8,2,4>UD                    { align1 WE_all 1N F@2 };
sel.ge(4)       g57.4<1>F       g57.3<0,1,0>F   g57.4<4,4,1>F   { align1 WE_all 1N I@6 };
sel.ge(4)       g58.4<1>F       g58.3<0,1,0>F   g58.4<4,4,1>F   { align1 WE_all 1N I@6 };
mov(4)          g63.2<4>UD      g40<8,2,4>UD                    { align1 WE_all 1N F@3 };
sel.l(4)        g107<4>F        g109<8,2,4>F    g111<8,2,4>F    { align1 WE_all 1N I@5 };
sel.ge(4)       g61.4<1>F       g61.3<0,1,0>F   g61.4<4,4,1>F   { align1 WE_all 1N I@4 };
sel.ge(4)       g62.4<1>F       g62.3<0,1,0>F   g62.4<4,4,1>F   { align1 WE_all 1N I@4 };
sel.l(4)        g51.4<1>F       g51.3<0,1,0>F   g51.4<4,4,1>F   { align1 WE_all 1N I@3 };
sel.l(4)        g52.4<1>F       g52.3<0,1,0>F   g52.4<4,4,1>F   { align1 WE_all 1N I@3 };
sel.l(4)        g49.4<1>F       g49.3<0,1,0>F   g49.4<4,4,1>F   { align1 WE_all 1N I@2 };
sel.l(4)        g50.4<1>F       g50.3<0,1,0>F   g50.4<4,4,1>F   { align1 WE_all 1N I@2 };
sel.ge(8)       g58<1>F         g57.7<0,1,0>F   g58<8,8,1>F     { align1 WE_all 1Q F@7 };
mov(4)          g70<4>UD        g63.3<8,2,4>UD                  { align1 WE_all 1N I@1 };
mov(4)          g68<4>UD        g63.1<8,2,4>UD                  { align1 WE_all 1N };
mov(4)          g53.3<4>UD      g107<8,2,4>UD                   { align1 WE_all 1N F@7 };
sel.ge(8)       g62<1>F         g61.7<0,1,0>F   g62<8,8,1>F     { align1 WE_all 1Q F@6 };
sel.l(8)        g52<1>F         g51.7<0,1,0>F   g52<8,8,1>F     { align1 WE_all 1Q F@5 };
sel.l(8)        g50<1>F         g49.7<0,1,0>F   g50<8,8,1>F     { align1 WE_all 1Q F@4 };
sel.ge(4)       g66<4>F         g68<8,2,4>F     g70<8,2,4>F     { align1 WE_all 1N I@2 };
sel.l(4)        g53.4<1>F       g53.3<0,1,0>F   g53.4<4,4,1>F   { align1 WE_all 1N I@1 };
sel.l(4)        g54.4<1>F       g54.3<0,1,0>F   g54.4<4,4,1>F   { align1 WE_all 1N I@1 };
mov(4)          g63.3<4>UD      g66<8,2,4>UD                    { align1 WE_all 1N F@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@4 };
(+f0.0) sel(16) g65<1>UD        g52.7<0,1,0>UD  g50.7<0,1,0>UD  { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@3 };
(+f0.0) sel(16) g67<1>UD        g62.7<0,1,0>UD  g58.7<0,1,0>UD  { align1 1H };
sel.l(8)        g54<1>F         g53.7<0,1,0>F   g54<8,8,1>F     { align1 WE_all 1Q F@1 };
sel.ge(4)       g63.4<1>F       g63.3<0,1,0>F   g63.4<4,4,1>F   { align1 WE_all 1N I@3 };
sel.ge(4)       g64.4<1>F       g64.3<0,1,0>F   g64.4<4,4,1>F   { align1 WE_all 1N I@3 };
cmp.z.f0.0(16)  null<1>D        g88<8,8,1>D     2D              { align1 1H };
sel.ge(8)       g64<1>F         g63.7<0,1,0>F   g64<8,8,1>F     { align1 WE_all 1Q F@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@3 };
(+f0.0) sel(16) g69<1>UD        g54.7<0,1,0>UD  g65<1,1,0>UD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
(+f0.0) sel(16) g71<1>UD        g64.7<0,1,0>UD  g67<1,1,0>UD    { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g90<8,8,1>D     0D              { align1 1H };
(+f0.0) sel(16) g97<1>F         g69<1,1,0>F     -g71<1,1,0>F    { align1 1H I@2 compacted };
and.nz.f0.0(16) null<1>UD       g92<8,8,1>UD    0x00000077UD    { align1 1H };
(+f0.0) if(16)  JIP:  LABEL17         UIP:  LABEL17             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
mul(16)         g72<1>D         g29<1,1,0>D     6W              { align1 1H compacted };
mul(16)         g74<1>D         g90<1,1,0>D     3W              { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
add3(16)        g76<1>D         g72<8,8,1>D     g88<8,8,1>D     g74<1,1,1>D { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
shl(16)         g95<1>D         g76<8,8,1>D     0x00000002UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        nullUD          g95UD           g97UD           0x04040515                0x00000080
                            slm MsgDesc: ( atomic_fmin, a32, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1H $15 };

LABEL17:
endif(16)       JIP:  LABEL16                                   { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
or(16)          g77<1>D         ~g31<1,1,0>D    ~g44<1,1,0>D    { align1 1H compacted };
mov(1)          g71<2>UW        0x00000000UD                    { align1 WE_all 1N F@1 };
mov(1)          f0<1>UW         g71<0,1,0>UW                    { align1 WE_all 1N I@1 };
cmp.nz.f0.0(16) null<1>UD       g77<8,8,1>UD    0x00000000UD    { align1 1H I@3 };
mov(16)         g78<1>UD        f0<0,1,0>UW                     { align1 1H };
and(16)         g55<1>UD        g55<1,1,0>UD    g78<1,1,0>UD    { align1 1H I@1 compacted };

LABEL16:
while(16)       JIP:  LABEL18                                   { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@6 };
add(16)         g86<1>D         g84<1,1,0>D     32D             { align1 1H compacted };
mov(1)          g72<2>UW        0x00000000UD                    { align1 WE_all 1N F@1 };
mov(1)          f0<1>UW         g72<0,1,0>UW                    { align1 WE_all 1N I@1 };
cmp.nz.f0.0(16) null<1>UD       g44<8,8,1>UD    0x00000000UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@5 };
mov(16)         g56<1>UD        f0<0,1,0>UW                     { align1 1H };

LABEL21:
cmp.z.f0.0(16)  null<1>D        g56<8,8,1>D     0D              { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL19       UIP:  LABEL19             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
fbl(16)         g94<1>UD        g56<8,8,1>UD                    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $14.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $15.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
mov(16)         a0<1>UW         0x0ac0UW                        { align1 WE_all 1H };
shl(16)         a0<1>UW         g94<16,8,2>W    0x0002UW        { align1 1H };
add(16)         a0<1>UW         a0<8,8,1>UW     0x0ac0UW        { align1 1H A@1 };
mov(16)         g96<1>UD        g[a0]<VxH,1,0>UD                { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
cmp.z.f0.0(16)  g98<1>D         g96<1,1,0>D     g86<1,1,0>D     { align1 1H $15.src compacted };
and.nz.f0.0(16) null<1>UD       g98<8,8,1>UD    g44<8,8,1>UD    { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
(+f0.0) sel(16) g100<1>UD       g21<8,8,1>UD    0xff800000UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
(+f0.0) sel(16) g102<1>UD       g23<8,8,1>UD    0xff800000UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@7 };
(+f0.0) sel(16) g104<1>UD       g13<8,8,1>UD    0x7f800000UD    { align1 1H };
(+f0.0) sel(16) g106<1>UD       g15<8,8,1>UD    0x7f800000UD    { align1 1H };
mov(16)         g115<1>F        0xff800000F      /* -infF */    { align1 WE_all 1H I@4 compacted };
mov(16)         g115<1>F        g100<1,1,0>F                    { align1 1H compacted };
mov(16)         g117<1>F        0xff800000F      /* -infF */    { align1 WE_all 1H I@3 compacted };
mov(16)         g117<1>F        g102<1,1,0>F                    { align1 1H compacted };
mov(16)         g109<1>F        0x7f800000F      /* infF */     { align1 WE_all 1H I@2 compacted };
mov(16)         g109<1>F        g104<1,1,0>F                    { align1 1H compacted };
mov(16)         g111<1>F        0x7f800000F      /* infF */     { align1 WE_all 1H I@1 compacted };
mov(16)         g111<1>F        g106<1,1,0>F                    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@2 };
(+f0.0) sel(16) g103<1>UD       g25<8,8,1>UD    0xff800000UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
(+f0.0) sel(16) g107<1>UD       g17<8,8,1>UD    0x7f800000UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mov(8)          g36<2>UD        g115.1<8,4,2>UD                 { align1 WE_all 1Q F@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $4.dst };
mov(8)          g61<2>UD        g117.1<8,4,2>UD                 { align1 WE_all 1Q F@3 };
mov(8)          g75<2>UD        g109.1<8,4,2>UD                 { align1 WE_all 1Q F@2 };
mov(8)          g113<2>UD       g111.1<8,4,2>UD                 { align1 WE_all 1Q F@1 };
mov(16)         g119<1>F        0xff800000F      /* -infF */    { align1 WE_all 1H A@6 compacted };
mov(16)         g119<1>F        g103<1,1,0>F                    { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g88<8,8,1>D     1D              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
sel.ge(8)       g34<2>F         g115<8,4,2>F    g36<8,4,2>F     { align1 WE_all 1Q I@5 };
sel.ge(8)       g54<2>F         g117<8,4,2>F    g61<8,4,2>F     { align1 WE_all 1Q I@4 };
sel.l(8)        g73<2>F         g109<8,4,2>F    g75<8,4,2>F     { align1 WE_all 1Q A@3 };
sel.l(8)        g105<2>F        g111<8,4,2>F    g113<8,4,2>F    { align1 WE_all 1Q I@2 };
mov(16)         g113<1>F        0x7f800000F      /* infF */     { align1 WE_all 1H I@6 compacted };
mov(16)         g113<1>F        g107<1,1,0>F                    { align1 1H compacted };
mov(8)          g115.1<2>UD     g34<8,4,2>UD                    { align1 WE_all 1Q A@5 };
mov(8)          g117.1<2>UD     g54<8,4,2>UD                    { align1 WE_all 1Q A@4 };
mov(8)          g109.1<2>UD     g73<8,4,2>UD                    { align1 WE_all 1Q A@3 };
mov(8)          g111.1<2>UD     g105<8,4,2>UD                   { align1 WE_all 1Q A@2 };
mov(8)          g6<2>UD         g113.1<8,4,2>UD                 { align1 WE_all 1Q F@1 };
mov(4)          g48<4>UD        g115.2<8,2,4>UD                 { align1 WE_all 1N I@5 };
mov(4)          g46<4>UD        g115.1<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g66<4>UD        g117.2<8,2,4>UD                 { align1 WE_all 1N I@6 };
mov(4)          g64<4>UD        g117.1<8,2,4>UD                 { align1 WE_all 1N $14.src };
mov(4)          g94<4>UD        g109.2<8,2,4>UD                 { align1 WE_all 1N I@7 };
mov(4)          g78<4>UD        g109.1<8,2,4>UD                 { align1 WE_all 1N };
mov(8)          g74<2>UD        g119.1<8,4,2>UD                 { align1 WE_all 1Q F@3 };
mov(4)          g125<4>UD       g111.2<8,2,4>UD                 { align1 WE_all 1N I@7 };
mov(4)          g123<4>UD       g111.1<8,2,4>UD                 { align1 WE_all 1N };
sel.l(8)        g4<2>F          g113<8,4,2>F    g6<8,4,2>F      { align1 WE_all 1Q I@7 };
sel.ge(4)       g40<4>F         g46<8,2,4>F     g48<8,2,4>F     { align1 WE_all 1N I@7 };
sel.ge(4)       g62<4>F         g64<8,2,4>F     g66<8,2,4>F     { align1 WE_all 1N I@6 };
sel.l(4)        g76<4>F         g78<8,2,4>F     g94<8,2,4>F     { align1 WE_all 1N I@4 };
sel.ge(8)       g72<2>F         g119<8,4,2>F    g74<8,4,2>F     { align1 WE_all 1Q I@3 };
sel.l(4)        g121<4>F        g123<8,2,4>F    g125<8,2,4>F    { align1 WE_all 1N I@1 };
mov(8)          g113.1<2>UD     g4<8,4,2>UD                     { align1 WE_all 1Q A@6 };
mov(4)          g115.2<4>UD     g40<8,2,4>UD                    { align1 WE_all 1N F@5 };
mov(4)          g117.2<4>UD     g62<8,2,4>UD                    { align1 WE_all 1N F@4 };
mov(4)          g109.2<4>UD     g76<8,2,4>UD                    { align1 WE_all 1N F@3 };
mov(8)          g119.1<2>UD     g72<8,4,2>UD                    { align1 WE_all 1Q A@2 };
mov(4)          g111.2<4>UD     g121<8,2,4>UD                   { align1 WE_all 1N F@1 };
mov(4)          g11<4>UD        g113.2<8,2,4>UD                 { align1 WE_all 1N I@6 };
mov(4)          g9<4>UD         g113.1<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g53<4>UD        g115.3<8,2,4>UD                 { align1 WE_all 1N I@7 };
mov(4)          g51<4>UD        g115.1<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g69<4>UD        g117.1<8,2,4>UD                 { align1 WE_all 1N I@7 };
mov(4)          g104<4>UD       g109.3<8,2,4>UD                 { align1 WE_all 1N I@7 };
mov(4)          g102<4>UD       g109.1<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g94<4>UD        g119.2<8,2,4>UD                 { align1 WE_all 1N A@3 };
mov(4)          g77<4>UD        g119.1<8,2,4>UD                 { align1 WE_all 1N F@3 };
mov(4)          g71<4>UD        g117.3<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g3<4>UD         g111.3<8,2,4>UD                 { align1 WE_all 1N };
mov(4)          g1<4>UD         g111.1<8,2,4>UD                 { align1 WE_all 1N };
sel.l(4)        g7<4>F          g9<8,2,4>F      g11<8,2,4>F     { align1 WE_all 1N };
sel.ge(4)       g49<4>F         g51<8,2,4>F     g53<8,2,4>F     { align1 WE_all 1N I@7 };
sel.l(4)        g100<4>F        g102<8,2,4>F    g104<8,2,4>F    { align1 WE_all 1N I@6 };
sel.ge(4)       g75<4>F         g77<8,2,4>F     g94<8,2,4>F     { align1 WE_all 1N I@4 };
sel.ge(4)       g67<4>F         g69<8,2,4>F     g71<8,2,4>F     { align1 WE_all 1N I@3 };
sel.l(4)        g126<4>F        g1<8,2,4>F      g3<8,2,4>F      { align1 WE_all 1N I@1 };
mov(4)          g113.2<4>UD     g7<8,2,4>UD                     { align1 WE_all 1N F@6 };
mov(4)          g115.3<4>UD     g49<8,2,4>UD                    { align1 WE_all 1N F@5 };
mov(4)          g109.3<4>UD     g100<8,2,4>UD                   { align1 WE_all 1N F@4 };
mov(4)          g119.2<4>UD     g75<8,2,4>UD                    { align1 WE_all 1N F@3 };
mov(4)          g117.3<4>UD     g67<8,2,4>UD                    { align1 WE_all 1N F@2 };
mov(4)          g111.3<4>UD     g126<8,2,4>UD                   { align1 WE_all 1N F@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
mov(4)          g33<4>UD        g113.3<8,2,4>UD                 { align1 WE_all 1N I@6 };
mov(4)          g31<4>UD        g113.1<8,2,4>UD                 { align1 WE_all 1N $12.src };
sel.ge(4)       g115.4<1>F      g115.3<0,1,0>F  g115.4<4,4,1>F  { align1 WE_all 1N I@7 };
sel.ge(4)       g116.4<1>F      g116.3<0,1,0>F  g116.4<4,4,1>F  { align1 WE_all 1N I@7 };
sel.l(4)        g109.4<1>F      g109.3<0,1,0>F  g109.4<4,4,1>F  { align1 WE_all 1N I@6 };
sel.l(4)        g110.4<1>F      g110.3<0,1,0>F  g110.4<4,4,1>F  { align1 WE_all 1N I@6 };
mov(4)          g104<4>UD       g119.3<8,2,4>UD                 { align1 WE_all 1N A@5 };
mov(4)          g102<4>UD       g119.1<8,2,4>UD                 { align1 WE_all 1N F@7 };
sel.ge(4)       g117.4<1>F      g117.3<0,1,0>F  g117.4<4,4,1>F  { align1 WE_all 1N I@6 };
sel.ge(4)       g118.4<1>F      g118.3<0,1,0>F  g118.4<4,4,1>F  { align1 WE_all 1N I@6 };
sel.l(4)        g111.4<1>F      g111.3<0,1,0>F  g111.4<4,4,1>F  { align1 WE_all 1N I@5 };
sel.l(4)        g112.4<1>F      g112.3<0,1,0>F  g112.4<4,4,1>F  { align1 WE_all 1N I@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
sel.l(4)        g29<4>F         g31<8,2,4>F     g33<8,2,4>F     { align1 WE_all 1N I@3 };
sel.ge(8)       g116<1>F        g115.7<0,1,0>F  g116<8,8,1>F    { align1 WE_all 1Q F@7 };
sel.l(8)        g110<1>F        g109.7<0,1,0>F  g110<8,8,1>F    { align1 WE_all 1Q F@7 };
sel.ge(4)       g100<4>F        g102<8,2,4>F    g104<8,2,4>F    { align1 WE_all 1N I@1 };
sel.ge(8)       g118<1>F        g117.7<0,1,0>F  g118<8,8,1>F    { align1 WE_all 1Q F@7 };
sel.l(8)        g112<1>F        g111.7<0,1,0>F  g112<8,8,1>F    { align1 WE_all 1Q F@6 };
mov(4)          g113.3<4>UD     g29<8,2,4>UD                    { align1 WE_all 1N F@6 };
mov(4)          g119.3<4>UD     g100<8,2,4>UD                   { align1 WE_all 1N F@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@2 };
(+f0.0) sel(16) g123<1>UD       g118.7<0,1,0>UD g116.7<0,1,0>UD { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
(+f0.0) sel(16) g121<1>UD       g112.7<0,1,0>UD g110.7<0,1,0>UD { align1 1H };
sel.l(4)        g113.4<1>F      g113.3<0,1,0>F  g113.4<4,4,1>F  { align1 WE_all 1N I@4 };
sel.l(4)        g114.4<1>F      g114.3<0,1,0>F  g114.4<4,4,1>F  { align1 WE_all 1N I@4 };
sel.ge(4)       g119.4<1>F      g119.3<0,1,0>F  g119.4<4,4,1>F  { align1 WE_all 1N I@3 };
sel.ge(4)       g120.4<1>F      g120.3<0,1,0>F  g120.4<4,4,1>F  { align1 WE_all 1N I@3 };
cmp.z.f0.0(16)  null<1>D        g88<8,8,1>D     2D              { align1 1H };
sel.l(8)        g114<1>F        g113.7<0,1,0>F  g114<8,8,1>F    { align1 WE_all 1Q F@3 };
sel.ge(8)       g120<1>F        g119.7<0,1,0>F  g120<8,8,1>F    { align1 WE_all 1Q F@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@2 };
(+f0.0) sel(16) g125<1>UD       g114.7<0,1,0>UD g121<1,1,0>UD   { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
(+f0.0) sel(16) g1<1>UD         g120.7<0,1,0>UD g123<1,1,0>UD   { align1 1H compacted };
cmp.z.f0.0(16)  null<1>D        g90<8,8,1>D     0D              { align1 1H };
(+f0.0) sel(16) g102<1>F        g125<1,1,0>F    -g1<1,1,0>F     { align1 1H I@2 compacted };
and.nz.f0.0(16) null<1>UD       g92<8,8,1>UD    0x00000077UD    { align1 1H };
(+f0.0) if(16)  JIP:  LABEL20         UIP:  LABEL20             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
mul(16)         g2<1>D          g96<1,1,0>D     6W              { align1 1H compacted };
mul(16)         g4<1>D          g90<1,1,0>D     3W              { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
add3(16)        g6<1>D          g2<8,8,1>D      g88<8,8,1>D     g4<1,1,1>D { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
shl(16)         g100<1>D        g6<8,8,1>D      0x00000002UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        nullUD          g100UD          g102UD          0x04040515                0x00000080
                            slm MsgDesc: ( atomic_fmin, a32, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1H $0 };

LABEL20:
endif(16)       JIP:  LABEL19                                   { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
or(16)          g7<1>D          ~g98<1,1,0>D    ~g44<1,1,0>D    { align1 1H compacted };
mov(1)          g105<2>UW       0x00000000UD                    { align1 WE_all 1N F@7 };
mov(1)          f0<1>UW         g105<0,1,0>UW                   { align1 WE_all 1N I@1 };
cmp.nz.f0.0(16) null<1>UD       g7<8,8,1>UD     0x00000000UD    { align1 1H I@3 };
mov(16)         g8<1>UD         f0<0,1,0>UW                     { align1 1H };
and(16)         g56<1>UD        g56<1,1,0>UD    g8<1,1,0>UD     { align1 1H I@1 compacted };

LABEL19:
while(16)       JIP:  LABEL21                                   { align1 1H };
mov.nz.f0.0(16) null<1>D        g44<8,8,1>D                     { align1 1H };
(+f0.0) if(16)  JIP:  LABEL22         UIP:  LABEL22             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@6 };
shl(16)         g9<1>D          g80<8,8,1>D     0x00000002UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
add(16)         g103<1>D        g9<1,1,0>D      1152D           { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g103UD          nullUD          0x04040508                0x00000000
                            slm MsgDesc: ( atomic_inc, a32, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1H $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
shl(16)         g10<1>D         g82<8,8,1>D     0x00000002UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
add(16)         g104<1>D        g10<1,1,0>D     1216D           { align1 1H $0.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g104UD          nullUD          0x04040508                0x00000000
                            slm MsgDesc: ( atomic_inc, a32, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@1 };
shl(16)         g11<1>D         g84<8,8,1>D     0x00000002UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
add(16)         g105<1>D        g11<1,1,0>D     1280D           { align1 1H $1.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g105UD          nullUD          0x04040508                0x00000000
                            slm MsgDesc: ( atomic_inc, a32, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };

LABEL22:
endif(16)       JIP:  LABEL23                                   { align1 1H };

LABEL23:
sync allwr(1)                   null<0,1,0>UB                   { align1 WE_all 1N };
sync allwr(1)                   null<0,1,0>UB                   { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(1)         g12UD           g0UD            nullUD          0x0210011f                0x00000000
                            slm MsgDesc: ( fence, a32, threadgroup, none, normal_routing dst_len = 1, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.dst };
mov(8)          g13<1>UD        0x00000000UD                    { align1 WE_all 1Q F@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(2)          g13.10<1>UB     g0.11<0,1,0>UB                  { align1 WE_all 1N I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g13UD           nullUD          0x02000004                0x00000000
                            gateway MsgDesc: (barrier msg)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H $4 };
sync bar(16)                    null<0,1,0>UB                   { align1 WE_all 1H };
mov(16)         g57<1>UD        g59<8,8,1>UD                    { align1 1H I@6 };

LABEL26:
cmp.ge.f0.0(16) null<1>UD       g57<8,8,1>UD    0x00000120UD    { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL24       UIP:  LABEL24             { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
shl(16)         g106<1>D        g57<8,8,1>D     0x00000002UD    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        g111UD          g106UD          nullUD          0x04201502                0x00000000
                            slm MsgDesc: ( load_cmask, a32, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
cmp.nz.f0.0(16) null<1>F        g111<1,1,0>F    0x7f800000F  /* infF */ { align1 1H $2.dst compacted };
(+f0.0) if(16)  JIP:  LABEL25         UIP:  LABEL25             { align1 1H };
add(16)         g14<1>D         g38<1,1,0>D     44D             { align1 1H F@7 compacted };
cmp.l.f0.0(16)  g16<1>UD        g14<1,1,0>UD    g38<1,1,0>UD    { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.dst };
add(16)         g18<1>D         g14<1,1,0>D     g106<1,1,0>D    { align1 1H F@7 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $11.dst };
cmp.l.f0.0(16)  g20<1>UD        g18<1,1,0>UD    g14<1,1,0>UD    { align1 1H A@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
mov(8)          g107<2>UD       g18<4,4,1>UD                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $2.src };
mov(8)          g109<2>UD       g19<4,4,1>UD                    { align1 2Q };
add3(16)        g22<1>D         -g16<8,8,1>D    g42<8,8,1>D     -g20<1,1,1>D { align1 1H A@3 };
mov(8)          g109.1<2>UD     g23<4,4,1>UD                    { align1 2Q I@1 };
mov(8)          g107.1<2>UD     g22<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g107UD          g111UD          0x08040595                0x00000080
                            ugm MsgDesc: ( atomic_fmin, a64, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $2 };

LABEL25:
endif(16)       JIP:  LABEL24                                   { align1 1H };
add(16)         g57<1>D         g57<1,1,0>D     512D            { align1 1H compacted };

LABEL24:
while(16)       JIP:  LABEL26                                   { align1 1H };

LABEL29:
cmp.ge.f0.0(16) null<1>UD       g59<8,8,1>UD    0x00000030UD    { align1 1H I@1 };
(+f0.0) break(16) JIP:  LABEL27       UIP:  LABEL27             { align1 1H };
shl(16)         g23<1>D         g59<8,8,1>D     0x00000002UD    { align1 1H A@7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
add(16)         g112<1>D        g23<1,1,0>D     1152D           { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        g117UD          g112UD          nullUD          0x04201502                0x00000000
                            slm MsgDesc: ( load_cmask, a32, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
cmp.nz.f0.0(16) null<1>D        g117<8,8,1>D    0D              { align1 1H $2.dst };
(+f0.0) if(16)  JIP:  LABEL28         UIP:  LABEL28             { align1 1H };
add(16)         g25<1>D         g38<1,1,0>D     1196D           { align1 1H F@7 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@3 };
cmp.l.f0.0(16)  g27<1>UD        g25<1,1,0>UD    g38<1,1,0>UD    { align1 1H @1 $12.dst compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
add(16)         g29<1>D         g25<1,1,0>D     g23<1,1,0>D     { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
cmp.l.f0.0(16)  g31<1>UD        g29<1,1,0>UD    g25<1,1,0>UD    { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $2.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N F@3 };
mov(8)          g113<2>UD       g29<4,4,1>UD                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $2.src };
mov(8)          g115<2>UD       g30<4,4,1>UD                    { align1 2Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $12.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
add3(16)        g33<1>D         -g27<8,8,1>D    g42<8,8,1>D     -g31<1,1,1>D { align1 1H };
mov(8)          g115.1<2>UD     g34<4,4,1>UD                    { align1 2Q I@1 };
mov(8)          g113.1<2>UD     g33<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g113UD          g117UD          0x0804058c                0x00000080
                            ugm MsgDesc: ( atomic_add, a64, d32, V1, L1UC_L3WB dst_len = 0, src0_len = 4, src1_len = 2 flat )  base_offset 0  { align1 1H $2 };

LABEL28:
endif(16)       JIP:  LABEL27                                   { align1 1H };
add(16)         g59<1>D         g59<1,1,0>D     512D            { align1 1H compacted };

LABEL27:
while(16)       JIP:  LABEL29                                   { align1 1H };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q F@1 };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_build_BFS_BFS_pass1_initial_batchable_code[] = {
    0x80000065, 0x6a058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x2a050220, 0x00000024, 0x00000000,
    0x00040061, 0x2e054220, 0x00000000, 0x00000000,
    0xe26b1b40, 0x00016a03, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa006b0c, 0x00340000, 0x80102001, 0x00000000,
    0x00000000, 0x00000000, 0x2a3c0061, 0x001102cc,
    0x211f0061, 0x001102cc, 0x00130061, 0x3e260aa0,
    0x00000264, 0x00000000, 0x2a3c1361, 0x00110204,
    0x211f1361, 0x00110204, 0x2a3e1361, 0x00110244,
    0xaa411340, 0x428e3c03, 0xa1761240, 0x428e1f03,
    0x00030061, 0x40050220, 0x00441f26, 0x00000000,
    0x00131b70, 0x42050220, 0x52464105, 0x00443c06,
    0x00130061, 0x24060220, 0x00344105, 0x00000000,
    0x00031c70, 0x77050220, 0x52467605, 0x00441f06,
    0x00030061, 0x22060220, 0x00347605, 0x00000000,
    0x00130061, 0x41050220, 0x00443c26, 0x00000000,
    0x00131d40, 0x43052660, 0x06464205, 0x00443c26,
    0x00031c40, 0x78052660, 0x06467705, 0x00441f26,
    0x00131a61, 0x24260220, 0x00344305, 0x00000000,
    0x00031a61, 0x22260220, 0x00347805, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x44140000, 0xfb042224, 0x00040000,
    0xa1243140, 0x42ce1f03, 0xaa253140, 0x42ce3c03,
    0x00033161, 0x21260aa0, 0x00000264, 0x00000000,
    0x80033161, 0x23054410, 0x00000000, 0x76543210,
    0x00031b70, 0x46050220, 0x52462405, 0x00441f06,
    0x00041b61, 0x28050220, 0x00462405, 0x00000000,
    0x00130070, 0x47050220, 0x52462505, 0x00443c06,
    0x21211161, 0x00110244, 0x64231c40, 0x00802395,
    0x27481a70, 0x24002803, 0x00041952, 0x4a042e68,
    0x0e2e4605, 0x48054005, 0x00030061, 0x4c060220,
    0x00342805, 0x00000000, 0x00130061, 0x4e060220,
    0x00342905, 0x00000000, 0x00031a61, 0x4c260220,
    0x00344a05, 0x00000000, 0x00131a61, 0x4e260220,
    0x00344b05, 0x00000000, 0xe250004c, 0x00114004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80041f40, 0x56058150, 0x05582305, 0xffffffff,
    0x80001b69, 0x10018220, 0x02005004, 0x00000003,
    0x80000940, 0x10018220, 0x02001000, 0x00000800,
    0x80000961, 0x7b060660, 0x00010600, 0x00000000,
    0x80000061, 0x7b260660, 0x00010610, 0x00000000,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80031961, 0x30260220, 0x00007b24, 0x00000000,
    0x80031961, 0x30060220, 0x00007b04, 0x00000000,
    0x80001b61, 0x7c064210, 0x00000000, 0x00000000,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x8a004231, 0x4d140000, 0xfb00300c, 0x00340000,
    0x80042261, 0x52054660, 0x00000000, 0x00000000,
    0x00040061, 0x52050660, 0x00464d05, 0x00000000,
    0x80041961, 0x10014110, 0x00000000, 0x0a400a40,
    0x80040069, 0x10018510, 0x01465605, 0x00020002,
    0x80040940, 0x10018110, 0x01461001, 0x0a400a40,
    0xe3540961, 0x001b0004, 0x80001961, 0x54054660,
    0x00000000, 0x00000000, 0x80031940, 0x54260660,
    0x06445406, 0x00445426, 0x80021940, 0x54470660,
    0x06425427, 0x00425447, 0x80021940, 0x54670660,
    0x06425427, 0x00425467, 0x80021940, 0x54850660,
    0x06005464, 0x00345485, 0x80021a40, 0x55850660,
    0x06005564, 0x00345585, 0xa4551940, 0x55015482,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0xa0570040, 0x54202a02, 0x00040061, 0x51050660,
    0x00465405, 0x00000000, 0x27591a70, 0x4d005703,
    0x80000061, 0x30010110, 0x00007c04, 0x00000000,
    0x00041a70, 0x00018220, 0x22465905, 0x00000000,
    0x00040061, 0x5b050120, 0x20003000, 0x00000000,
    0x01040022, 0x0001c060, 0x00000030, 0x00000030,
    0x00040061, 0x2c054220, 0x00000000, 0xffffffff,
    0x00040028, 0x0001c660, 0x000000c8, 0x00000010,
    0x00040025, 0x00004600, 0x00000000, 0x000000b8,
    0xa05d1f40, 0x4d005102, 0xa0260040, 0x01002e03,
    0xa02a1a40, 0x5e302a52, 0x0004a170, 0x00010220,
    0x42462605, 0x00464405, 0x01040022, 0x0001c060,
    0x00000040, 0x00000040, 0x00040061, 0x2e050220,
    0x00462605, 0x00000000, 0x00041f61, 0x2c054220,
    0x00000000, 0x00000000, 0x00040028, 0x0001c660,
    0x00000050, 0x00000010, 0x00040025, 0x00004600,
    0x00000000, 0x00000040, 0x00041c69, 0x5e058660,
    0x02462e05, 0x00000002, 0x00040061, 0x2e050220,
    0x00462605, 0x00000000, 0x00041a52, 0x28044160,
    0x0e0e0040, 0x24055e05, 0x00040027, 0x00014060,
    0x00000000, 0xfffffcd8, 0x00041a61, 0x00010660,
    0x20462c05, 0x00000000, 0x01040022, 0x0001c060,
    0x000000f0, 0x000000b0, 0x00041e4c, 0x5f050220,
    0x00465b05, 0x00000000, 0x00040070, 0x00018660,
    0x16465b05, 0x00000000, 0x11041a62, 0x61058220,
    0x02465f05, 0x00000020, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0xa0300040, 0x61002e02,
    0x80040061, 0x10014110, 0x00000000, 0x0ae00ae0,
    0x00040069, 0x10018510, 0x01566106, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0ae00ae0,
    0xe0320961, 0x001b0004, 0x00040024, 0x0001c060,
    0x00000050, 0x00000050, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x00040061, 0x30054220,
    0x00000000, 0x00000000, 0x00041b61, 0x32054220,
    0x00000000, 0x00000000, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x00030040, 0x62058660,
    0x06441f06, 0x0000082c, 0x00130040, 0x63058660,
    0x06443c06, 0x0000082c, 0x00041d69, 0x65058660,
    0x02463005, 0x00000002, 0x00030040, 0x78058660,
    0x06441f06, 0x00002c2c, 0x00130040, 0x79058660,
    0x06443c06, 0x00002c2c, 0xa12c1140, 0x008e2103,
    0xaa2d1340, 0x008e3e03, 0xe0520065, 0x0ff10043,
    0x00040061, 0x56050160, 0x00462305, 0x00000000,
    0xa0671f40, 0x65006202, 0xe7640070, 0x82c06203,
    0x00041c69, 0x54058660, 0x02465205, 0x00000004,
    0x27691b70, 0x62006703, 0x00030061, 0x34060220,
    0x00346705, 0x00000000, 0x00130061, 0x36060220,
    0x00346805, 0x00000000, 0x00030070, 0x7a050220,
    0x52467805, 0x00441f06, 0xa0581d40, 0x54005602,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x00041d52, 0x6b042e68, 0x0e2e6405, 0x69054005,
    0x00131961, 0x36260220, 0x00346c05, 0x00000000,
    0x00031a61, 0x34260220, 0x00346b05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x6c140000, 0xfb043424, 0x00040000,
    0x00042361, 0x6e050120, 0x00566c06, 0x00000000,
    0x00040061, 0x7b050120, 0x00566c16, 0x00000000,
    0x00031a41, 0x20018220, 0x01466e05, 0x00580058,
    0x00041b6c, 0x70058660, 0x02466e05, 0x0000001f,
    0x60740041, 0x05806e02, 0x80001c01, 0x00000000,
    0x00000000, 0x00000000, 0x607d0041, 0x5cc07b02,
    0x00130070, 0x7b050220, 0x52467905, 0x00443c06,
    0xe03b0065, 0x1ff05803, 0xfe720049, 0x05806e03,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0xa0341d40, 0x74002c02, 0x60751f41, 0x05807002,
    0xa0261e40, 0x7d007802, 0x00130041, 0x20018220,
    0x01466f05, 0x00580058, 0x0003c161, 0x45060220,
    0x00343405, 0x00000000, 0x00131d61, 0x47060220,
    0x00343505, 0x00000000, 0x27281c70, 0x78002603,
    0x00030061, 0x4b060220, 0x00342605, 0x00000000,
    0x00130061, 0x4d060220, 0x00342705, 0x00000000,
    0x00130049, 0x73058222, 0x02466f05, 0x00000058,
    0x00130070, 0x2e050220, 0x52462d05, 0x00443e06,
    0x00030070, 0x79050220, 0x52462c05, 0x00442106,
    0x27363370, 0x2c003403, 0x00041f52, 0x2a042e68,
    0x0e2e7a05, 0x28054005, 0xa0771d40, 0x75007202,
    0x00131d40, 0x30052660, 0x06462e05, 0x00443e26,
    0x00031d40, 0x2f052660, 0x06467905, 0x00442126,
    0x00131c61, 0x4d260220, 0x00342b05, 0x00000000,
    0x00031d61, 0x4b260220, 0x00342a05, 0x00000000,
    0x00041b52, 0x38040e68, 0x0e2e2f05, 0x36057705,
    0x00131961, 0x47260220, 0x00343905, 0x00000000,
    0x00031a61, 0x45260220, 0x00343805, 0x00000000,
    0x80002101, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044431, 0x3d440000, 0xfb044524, 0x003c0000,
    0x80003401, 0x00000000, 0x00000000, 0x00000000,
    0x80001c01, 0x00000000, 0x00000000, 0x00000000,
    0x00044531, 0x45340000, 0xfb044b24, 0x001c0000,
    0xa05a2440, 0x02004103, 0x00042569, 0x5f058660,
    0x02464905, 0x00000005, 0x275c1a70, 0x41005a03,
    0xa0621a40, 0x5f005a02, 0xe0600068, 0x01b04903,
    0xa05eb440, 0x43025c02, 0x27641b70, 0x5a006203,
    0x00033561, 0x4c060220, 0x00346205, 0x00000000,
    0x00133561, 0x4e060220, 0x00346305, 0x00000000,
    0xa0680040, 0x01006203, 0x00041c52, 0x66040e68,
    0x0e2e5e05, 0x64056005, 0xe76a1a70, 0x01006803,
    0x00131a61, 0x4e260220, 0x00346705, 0x00000000,
    0x00031b61, 0x4c260220, 0x00346605, 0x00000000,
    0xa06c1b40, 0x66026a02, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0x00044631, 0x01440000,
    0xfb044c24, 0x003c0000, 0x80003601, 0x00000000,
    0x00000000, 0x00000000, 0x00043569, 0x4b058660,
    0x02463205, 0x00000009, 0x00033661, 0x4d060220,
    0x00346805, 0x00000000, 0x00133661, 0x4f060220,
    0x00346905, 0x00000000, 0x00031a61, 0x4d260220,
    0x00346c05, 0x00000000, 0x00131a61, 0x4f260220,
    0x00346d05, 0x00000000, 0x206e2640, 0x01200700,
    0x00041162, 0x74058aa0, 0x4a466e05, 0x0704ec3d,
    0x00041170, 0x0e058aa0, 0x5a467405, 0x77f684df,
    0x00044738, 0x77050aa0, 0x1a467405, 0x00460001,
    0x00040070, 0x10058aa0, 0x3a466e05, 0x0704ec3d,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00044831, 0x6d240000, 0xfb044d24, 0x000c0000,
    0x80003801, 0x00000000, 0x00000000, 0x00000000,
    0xa04dd540, 0x4b004502, 0x00042741, 0x79058aa0,
    0x0a467705, 0x417d70a4, 0x00041265, 0x00010220,
    0x22460e05, 0x00461005, 0x80003801, 0x00000000,
    0x00000000, 0x00000000, 0xa04f1a40, 0x20004d03,
    0xa0590040, 0x3b004d02, 0xef091162, 0x00007903,
    0x2751b562, 0x47004f03, 0x80002601, 0x00000000,
    0x00000000, 0x00000000, 0x20712840, 0x03206d00,
    0x80002601, 0x00000000, 0x00000000, 0x00000000,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x20732840, 0x05206f00, 0x00040a70, 0x11058aa0,
    0x3a467105, 0x0704ec3d, 0x00043762, 0x75058aa0,
    0x4a467105, 0x0704ec3d, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0x00044938, 0x7a050aa0,
    0x1a467505, 0x00460001, 0x00043970, 0x13058aa0,
    0x5a467505, 0x77f684df, 0x00041462, 0x76058aa0,
    0x4a467305, 0x0704ec3d, 0x00042941, 0x7c058aa0,
    0x0a467a05, 0x417d70a4, 0x00041365, 0x00010220,
    0x22461305, 0x00461105, 0xef0b1162, 0x00007c03,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00044a38, 0x7d050aa0, 0x1a467605, 0x00460001,
    0x00041a70, 0x14058aa0, 0x3a467305, 0x0704ec3d,
    0x00043a70, 0x16058aa0, 0x5a467605, 0x77f684df,
    0x80002a01, 0x00000000, 0x00000000, 0x00000000,
    0x00041f41, 0x0d058aa0, 0x0a467d05, 0x417d70a4,
    0x00041265, 0x00010220, 0x22461605, 0x00461405,
    0xef281162, 0x00000d03, 0x272c1d70, 0x51005903,
    0x01040022, 0x0001c060, 0x00000120, 0x00000120,
    0x00040069, 0x2e058660, 0x02465905, 0x00000005,
    0xa0319440, 0x2e003d02, 0xe02f0068, 0x01b05903,
    0x27331a70, 0x3d003103, 0xa0370040, 0x01003103,
    0x00030061, 0x4e060220, 0x00343105, 0x00000000,
    0x00130061, 0x50060220, 0x00343205, 0x00000000,
    0x0004c452, 0x35040e68, 0x0e2e3f05, 0x33052f05,
    0x27391c70, 0x31003703, 0x00030061, 0x52060220,
    0x00343705, 0x00000000, 0x00130061, 0x54060220,
    0x00343805, 0x00000000, 0x00131c61, 0x50260220,
    0x00343605, 0x00000000, 0x00031d61, 0x4e260220,
    0x00343505, 0x00000000, 0xa03d1d40, 0x35023902,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00044b31, 0x0d440000, 0xfb044e24, 0x003c0000,
    0x00031961, 0x52260220, 0x00343d05, 0x00000000,
    0x00131a61, 0x54260220, 0x00343e05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044c31, 0x15440000, 0xfb045224, 0x003c0000,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x00040069, 0x35058660, 0x02463b05, 0x00000002,
    0x00041961, 0x33050220, 0x00463505, 0x00000000,
    0x00041970, 0x00018220, 0x42463305, 0x00000120,
    0x01040028, 0x0001c660, 0x00000090, 0x00000090,
    0x00043c69, 0x53058660, 0x02463305, 0x00000002,
    0x00043c61, 0x1d054220, 0x00000000, 0x7f800000,
    0x00043c61, 0x1f054220, 0x00000000, 0x7f800000,
    0x00043c61, 0x21054220, 0x00000000, 0x7f800000,
    0x00043c61, 0x23054220, 0x00000000, 0x7f800000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044c31, 0x00000000, 0xea0c5314, 0x003c1d44,
    0x00040040, 0x33058660, 0x06463305, 0x00000800,
    0x00040027, 0x00014060, 0x00000000, 0xffffff60,
    0x00041970, 0x00018220, 0x42463505, 0x00000030,
    0x01040028, 0x0001c660, 0x000000a8, 0x000000a8,
    0x00042469, 0x3e058660, 0x02463505, 0x00000002,
    0x00043c61, 0x1e054220, 0x00000000, 0x00000000,
    0x00043c61, 0x20054220, 0x00000000, 0x00000000,
    0x00043c61, 0x22054220, 0x00000000, 0x00000000,
    0x00043c61, 0x24054220, 0x00000000, 0x00000000,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0xa0541d40, 0x48003e03, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044c31, 0x00000000,
    0xea0c5414, 0x003c1e44, 0x00040040, 0x35058660,
    0x06463505, 0x00000800, 0x00040027, 0x00014060,
    0x00000000, 0xffffff48, 0x80000001, 0x00000000,
    0x30000000, 0x00000000, 0x80000001, 0x00000000,
    0x30000000, 0x00000000, 0x80002401, 0x00000000,
    0x00000000, 0x00000000, 0x80001d01, 0x00000000,
    0x00000000, 0x00000000, 0x80004d31, 0x3f0c0000,
    0xe23e000c, 0x00000000, 0x80002d01, 0x00000000,
    0x00000000, 0x00000000, 0x80032461, 0x40054220,
    0x00000000, 0x00000000, 0x80003d01, 0x00000000,
    0x00000000, 0x00000000, 0x80011961, 0x40550000,
    0x0000005c, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x80044e31, 0x00000000,
    0x3008400c, 0x00000000, 0x80040001, 0x00000000,
    0xe0000000, 0x00000000, 0x80002c01, 0x00000000,
    0x00000000, 0x00000000, 0x20412b40, 0x15000d00,
    0x80002c01, 0x00000000, 0x00000000, 0x00000000,
    0x20432b40, 0x17000f00, 0x80002c01, 0x00000000,
    0x00000000, 0x00000000, 0x20452b40, 0x19001100,
    0x20491140, 0x05204500, 0x80001561, 0x7d064210,
    0x00000000, 0x00000000, 0x20461440, 0x01204100,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0x204e0a41, 0x28004900, 0x20481540, 0x03204300,
    0x204a1341, 0x09004600, 0x80003c01, 0x00000000,
    0x00000000, 0x00000000, 0x00041361, 0x54050a20,
    0x00464e05, 0x00000000, 0x204c1241, 0x0b004800,
    0x80003b01, 0x00000000, 0x00000000, 0x00000000,
    0x00041261, 0x50050a20, 0x00464a05, 0x00000000,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x00041161, 0x52050a20, 0x00464c05, 0x00000000,
    0x80001c61, 0x30010110, 0x00007d04, 0x00000000,
    0x00040070, 0x00018220, 0x22462c05, 0x00000000,
    0x00040061, 0x36050120, 0x00003000, 0x00000000,
    0x80000061, 0x7a054660, 0x00000000, 0x00000001,
    0xe0580065, 0x00305603, 0xe05a0068, 0x00205603,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00040069, 0x5c050660, 0x02007a04, 0x00465605,
    0x00041a70, 0x00018660, 0x16463605, 0x00000000,
    0x01040028, 0x0001c660, 0x000008f8, 0x000008f8,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x0004114c, 0x5e050220, 0x00463605, 0x00000000,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x80003e01, 0x00000000, 0x00000000, 0x00000000,
    0x80041961, 0x10014110, 0x00000000, 0x0a000a00,
    0x00040069, 0x10018510, 0x01565e06, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0a000a00,
    0xe0600961, 0x001b0004, 0xac621970, 0x50006002,
    0x00041965, 0x00010220, 0x22466205, 0x00462c05,
    0x01040062, 0x64058220, 0x02461505, 0xff800000,
    0x01040062, 0x66058220, 0x02461705, 0xff800000,
    0x01040062, 0x68058220, 0x02460d05, 0x7f800000,
    0x01040062, 0x6a058220, 0x02460f05, 0x7f800000,
    0xa3731c61, 0xff810000, 0x60730061, 0x00106400,
    0xa3751b61, 0xff810000, 0x60750061, 0x00106600,
    0xa36d1a61, 0x7f810000, 0x606d0061, 0x00106800,
    0xa36f1961, 0x7f810000, 0x606f0061, 0x00106a00,
    0x80001201, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x67058220, 0x02461905, 0xff800000,
    0x80001101, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x6b058220, 0x02461105, 0x7f800000,
    0x80031461, 0x45060220, 0x00447326, 0x00000000,
    0x80031361, 0x64060220, 0x00447526, 0x00000000,
    0x80031261, 0x01060220, 0x00446d26, 0x00000000,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x80031161, 0x1f060220, 0x00446f26, 0x00000000,
    0xa3770e61, 0xff810000, 0x60770061, 0x00106700,
    0xa3711d61, 0x7f810000, 0x60710061, 0x00106b00,
    0x00040070, 0x00018660, 0x16465805, 0x00000001,
    0x80030d62, 0x43060aa0, 0x4a447306, 0x00444506,
    0x80031c62, 0x5e060aa0, 0x4a447506, 0x00446406,
    0x80031b62, 0x7e060aa0, 0x5a446d06, 0x00440106,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x80031a62, 0x1d060aa0, 0x5a446f06, 0x00441f06,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x80031561, 0x33060220, 0x00447126, 0x00000000,
    0x80030c61, 0x73260220, 0x00444306, 0x00000000,
    0x80030b61, 0x75260220, 0x00445e06, 0x00000000,
    0x80030a61, 0x6d260220, 0x00447e06, 0x00000000,
    0x80030961, 0x6f260220, 0x00441d06, 0x00000000,
    0x80031d62, 0x31060aa0, 0x5a447106, 0x00443306,
    0x80021c61, 0x4a070220, 0x00427347, 0x00000000,
    0x80020061, 0x48070220, 0x00427327, 0x00000000,
    0x80020d61, 0x69070220, 0x00427547, 0x00000000,
    0x80021761, 0x67070220, 0x00427527, 0x00000000,
    0x80021e61, 0x06070220, 0x00426d47, 0x00000000,
    0x80020061, 0x04070220, 0x00426d27, 0x00000000,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x80021f61, 0x24070220, 0x00426f47, 0x00000000,
    0x80023c61, 0x22070220, 0x00426f27, 0x00000000,
    0x80031761, 0x7e060220, 0x00447726, 0x00000000,
    0x80031161, 0x71260220, 0x00443106, 0x00000000,
    0x80021f62, 0x46070aa0, 0x4a424807, 0x00424a07,
    0x80021f62, 0x65070aa0, 0x4a426707, 0x00426907,
    0x80021d62, 0x02070aa0, 0x5a420407, 0x00420607,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x80021b62, 0x20070aa0, 0x5a422207, 0x00422407,
    0x80031a62, 0x7c060aa0, 0x4a447706, 0x00447e06,
    0x80029461, 0x3d070220, 0x00427147, 0x00000000,
    0x80020061, 0x38070220, 0x00427127, 0x00000000,
    0x80021561, 0x73470220, 0x00424607, 0x00000000,
    0x80021461, 0x75470220, 0x00426507, 0x00000000,
    0x80021361, 0x6d470220, 0x00420207, 0x00000000,
    0x80021261, 0x6f470220, 0x00422007, 0x00000000,
    0x80030961, 0x77260220, 0x00447c06, 0x00000000,
    0x80021e62, 0x34070aa0, 0x5a423807, 0x00423d07,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x80021d61, 0x56070220, 0x00427367, 0x00000000,
    0x80020061, 0x4d070220, 0x00427327, 0x00000000,
    0x80021e61, 0x79070220, 0x00427527, 0x00000000,
    0x80021e61, 0x0b070220, 0x00426d67, 0x00000000,
    0x80020061, 0x09070220, 0x00426d27, 0x00000000,
    0x80021f61, 0x30070220, 0x00426f67, 0x00000000,
    0x80020061, 0x2e070220, 0x00426f27, 0x00000000,
    0x80020c61, 0x05070220, 0x00427747, 0x00000000,
    0x80021461, 0x03070220, 0x00427727, 0x00000000,
    0x80020061, 0x7b070220, 0x00427567, 0x00000000,
    0x80021161, 0x71470220, 0x00423407, 0x00000000,
    0x80021f62, 0x4b070aa0, 0x4a424d07, 0x00425607,
    0x80021f62, 0x07070aa0, 0x5a420907, 0x00420b07,
    0x80021d62, 0x28070aa0, 0x5a422e07, 0x00423007,
    0x80021b62, 0x01070aa0, 0x4a420307, 0x00420507,
    0x80021a62, 0x6a070aa0, 0x4a427907, 0x00427b07,
    0x80021961, 0x42070220, 0x00427167, 0x00000000,
    0x80023e61, 0x40070220, 0x00427127, 0x00000000,
    0x80021561, 0x73670220, 0x00424b07, 0x00000000,
    0x80021461, 0x6d670220, 0x00420707, 0x00000000,
    0x80021361, 0x6f670220, 0x00422807, 0x00000000,
    0x80021261, 0x77470220, 0x00420107, 0x00000000,
    0x80021161, 0x75670220, 0x00426a07, 0x00000000,
    0x80021e62, 0x3e070aa0, 0x5a424007, 0x00424207,
    0x80021d62, 0x73850aa0, 0x4a007364, 0x00347385,
    0x80021d62, 0x74850aa0, 0x4a007464, 0x00347485,
    0x80021c62, 0x6d850aa0, 0x5a006d64, 0x00346d85,
    0x80021c62, 0x6e850aa0, 0x5a006e64, 0x00346e85,
    0x80021b62, 0x6f850aa0, 0x5a006f64, 0x00346f85,
    0x80021b62, 0x70850aa0, 0x5a007064, 0x00347085,
    0x80021a61, 0x0a070220, 0x00427767, 0x00000000,
    0x80020061, 0x08070220, 0x00427727, 0x00000000,
    0x80021b62, 0x75850aa0, 0x4a007564, 0x00347585,
    0x80021b62, 0x76850aa0, 0x4a007664, 0x00347685,
    0x80021761, 0x71670220, 0x00423e07, 0x00000000,
    0x80031762, 0x74050aa0, 0x4a0073e4, 0x00467405,
    0x80031662, 0x6e050aa0, 0x5a006de4, 0x00466e05,
    0x80031562, 0x70050aa0, 0x5a006fe4, 0x00467005,
    0x80021a62, 0x06070aa0, 0x4a420807, 0x00420a07,
    0x80031562, 0x76050aa0, 0x4a0075e4, 0x00467605,
    0x80021962, 0x71850aa0, 0x5a007164, 0x00347185,
    0x80021962, 0x72850aa0, 0x5a007264, 0x00347285,
    0x80001501, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x79050220, 0x020070e4, 0x00006ee4,
    0x80021461, 0x77670220, 0x00420607, 0x00000000,
    0x80001301, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x7b050220, 0x020076e4, 0x000074e4,
    0x80031162, 0x72050aa0, 0x5a0071e4, 0x00467205,
    0x80021a62, 0x77850aa0, 0x4a007764, 0x00347785,
    0x80021a62, 0x78850aa0, 0x4a007864, 0x00347885,
    0x00040070, 0x00018660, 0x16465805, 0x00000002,
    0x80031162, 0x78050aa0, 0x4a0077e4, 0x00467805,
    0x80000c01, 0x00000000, 0x00000000, 0x00000000,
    0x2f7d0062, 0x79017283, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0x2f010062, 0x7b017883,
    0x00040070, 0x00018660, 0x16465a05, 0x00000000,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x2f5e0062, 0x01207d00, 0x00040065, 0x00018220,
    0x22465c05, 0x00000077, 0x01040022, 0x0001c060,
    0x00000090, 0x00000090, 0x80001101, 0x00000000,
    0x00000000, 0x00000000, 0x60020041, 0x00606002,
    0x60040041, 0x00305a02, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00040052, 0x06040e68,
    0x0e0e0205, 0x04055805, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00040069, 0x56058660,
    0x02460605, 0x00000002, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0x00044f31, 0x00000000,
    0xea2a5614, 0x01005e14, 0x00040025, 0x00004600,
    0x00000000, 0x00000080, 0x80000a01, 0x00000000,
    0x00000000, 0x00000000, 0xa0070066, 0x2c226202,
    0x80001761, 0x0b064210, 0x00000000, 0x00000000,
    0x80001961, 0x30010110, 0x00000b04, 0x00000000,
    0x00041b70, 0x00018220, 0x22460705, 0x00000000,
    0x80001701, 0x00000000, 0x00000000, 0x00000000,
    0x00040061, 0x08050120, 0x00003000, 0x00000000,
    0x20361965, 0x08003603, 0x00040027, 0x00014060,
    0x00000000, 0xfffff6f8, 0x80000a01, 0x00000000,
    0x00000000, 0x00000000, 0xa0090040, 0x01005203,
    0x80001161, 0x0c064210, 0x00000000, 0x00000000,
    0x80001961, 0x30010110, 0x00000c04, 0x00000000,
    0x00040070, 0x00018220, 0x22462c05, 0x00000000,
    0x80001e01, 0x00000000, 0x00000000, 0x00000000,
    0x00040061, 0x37050120, 0x00003000, 0x00000000,
    0x00041970, 0x00018660, 0x16463705, 0x00000000,
    0x01040028, 0x0001c660, 0x00000948, 0x00000948,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x0004004c, 0x0b050220, 0x00463705, 0x00000000,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x80041961, 0x10014110, 0x00000000, 0x01200120,
    0x00040069, 0x10018510, 0x01560b06, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x01200120,
    0xe01d0961, 0x001b0004, 0x80003c01, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0xac1f0070, 0x09001d02,
    0x00041965, 0x00010220, 0x22461f05, 0x00462c05,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x21058220, 0x02461505, 0xff800000,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x23058220, 0x02461705, 0xff800000,
    0x80001401, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x28058220, 0x02460d05, 0x7f800000,
    0x01040062, 0x2e058220, 0x02460f05, 0x7f800000,
    0xa3391c61, 0xff810000, 0x60390061, 0x00102100,
    0x80002401, 0x00000000, 0x00000000, 0x00000000,
    0xa33d1b61, 0xff810000, 0x603d0061, 0x00102300,
    0xa3311a61, 0x7f810000, 0x60310061, 0x00102800,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x80001201, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x24058220, 0x02461905, 0xff800000,
    0xa3331a61, 0x7f810000, 0x60330061, 0x00102e00,
    0x80031461, 0x72060220, 0x00443926, 0x00000000,
    0x80001101, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x2f058220, 0x02461105, 0x7f800000,
    0x80031361, 0x01060220, 0x00443d26, 0x00000000,
    0x80003e01, 0x00000000, 0x00000000, 0x00000000,
    0xa33f0c61, 0xff810000, 0x603f0061, 0x00102400,
    0x80031261, 0x4b060220, 0x00443326, 0x00000000,
    0x80031361, 0x28060220, 0x00443126, 0x00000000,
    0x80030d62, 0x70060aa0, 0x4a443906, 0x00447206,
    0xa3351c61, 0x7f810000, 0x60350061, 0x00102f00,
    0x00040070, 0x00018660, 0x16465805, 0x00000001,
    0x80030c62, 0x7d060aa0, 0x4a443d06, 0x00440106,
    0x80031461, 0x24060220, 0x00443f26, 0x00000000,
    0x80030c62, 0x49060aa0, 0x5a443306, 0x00444b06,
    0x80031b62, 0x21060aa0, 0x5a443106, 0x00442806,
    0x80030d61, 0x39260220, 0x00447006, 0x00000000,
    0x80031461, 0x65060220, 0x00443526, 0x00000000,
    0x80030b61, 0x3d260220, 0x00447d06, 0x00000000,
    0x80030a61, 0x33260220, 0x00444906, 0x00000000,
    0x80030961, 0x31260220, 0x00442106, 0x00000000,
    0x80021d61, 0x77070220, 0x00423947, 0x00000000,
    0x80020061, 0x75070220, 0x00423927, 0x00000000,
    0x80031e62, 0x63060aa0, 0x5a443506, 0x00446506,
    0x80021d61, 0x06070220, 0x00423d47, 0x00000000,
    0x80020061, 0x04070220, 0x00423d27, 0x00000000,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x80021e61, 0x56070220, 0x00423347, 0x00000000,
    0x80020061, 0x4e070220, 0x00423327, 0x00000000,
    0x80021f61, 0x43070220, 0x00423147, 0x00000000,
    0x80020061, 0x41070220, 0x00423127, 0x00000000,
    0x80021f62, 0x73070aa0, 0x4a427507, 0x00427707,
    0x80031f62, 0x22060aa0, 0x4a443f06, 0x00442406,
    0x80031361, 0x35260220, 0x00446306, 0x00000000,
    0x80021e62, 0x02070aa0, 0x4a420407, 0x00420607,
    0x80021c62, 0x4c070aa0, 0x5a424e07, 0x00425607,
    0x80021a62, 0x2e070aa0, 0x5a424107, 0x00424307,
    0x80021561, 0x39470220, 0x00427307, 0x00000000,
    0x80031461, 0x3f260220, 0x00442206, 0x00000000,
    0x80021b61, 0x6a070220, 0x00423547, 0x00000000,
    0x80020061, 0x68070220, 0x00423527, 0x00000000,
    0x80021361, 0x3d470220, 0x00420207, 0x00000000,
    0x80021261, 0x33470220, 0x00424c07, 0x00000000,
    0x80021161, 0x31470220, 0x00422e07, 0x00000000,
    0x80021f61, 0x7c070220, 0x00423967, 0x00000000,
    0x80020061, 0x7a070220, 0x00423927, 0x00000000,
    0x80020961, 0x41070220, 0x00423f47, 0x00000000,
    0x80021f62, 0x66070aa0, 0x5a426807, 0x00426a07,
    0x80021e61, 0x0b070220, 0x00423d27, 0x00000000,
    0x80020061, 0x21070220, 0x00423d67, 0x00000000,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x80021f61, 0x62070220, 0x00423367, 0x00000000,
    0x80023f61, 0x60070220, 0x00423327, 0x00000000,
    0x80021f61, 0x48070220, 0x00423167, 0x00000000,
    0x80020061, 0x46070220, 0x00423127, 0x00000000,
    0x80021f62, 0x78070aa0, 0x4a427a07, 0x00427c07,
    0x80020061, 0x2e070220, 0x00423f27, 0x00000000,
    0x80021261, 0x35470220, 0x00426607, 0x00000000,
    0x80021f62, 0x07070aa0, 0x4a420b07, 0x00422107,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x80021d62, 0x5e070aa0, 0x5a426007, 0x00426207,
    0x80021b62, 0x44070aa0, 0x5a424607, 0x00424807,
    0x80021461, 0x39670220, 0x00427807, 0x00000000,
    0x80021b62, 0x28070aa0, 0x4a422e07, 0x00424107,
    0x80021a61, 0x6f070220, 0x00423567, 0x00000000,
    0x80020061, 0x6d070220, 0x00423527, 0x00000000,
    0x80021461, 0x3d670220, 0x00420707, 0x00000000,
    0x80021361, 0x33670220, 0x00425e07, 0x00000000,
    0x80021261, 0x31670220, 0x00424407, 0x00000000,
    0x80021e62, 0x39850aa0, 0x4a003964, 0x00343985,
    0x80021e62, 0x3a850aa0, 0x4a003a64, 0x00343a85,
    0x80021361, 0x3f470220, 0x00422807, 0x00000000,
    0x80021d62, 0x6b070aa0, 0x5a426d07, 0x00426f07,
    0x80021c62, 0x3d850aa0, 0x4a003d64, 0x00343d85,
    0x80021c62, 0x3e850aa0, 0x4a003e64, 0x00343e85,
    0x80021b62, 0x33850aa0, 0x5a003364, 0x00343385,
    0x80021b62, 0x34850aa0, 0x5a003464, 0x00343485,
    0x80021a62, 0x31850aa0, 0x5a003164, 0x00343185,
    0x80021a62, 0x32850aa0, 0x5a003264, 0x00343285,
    0x80031762, 0x3a050aa0, 0x4a0039e4, 0x00463a05,
    0x80021961, 0x46070220, 0x00423f67, 0x00000000,
    0x80020061, 0x44070220, 0x00423f27, 0x00000000,
    0x80021761, 0x35670220, 0x00426b07, 0x00000000,
    0x80031662, 0x3e050aa0, 0x4a003de4, 0x00463e05,
    0x80031562, 0x34050aa0, 0x5a0033e4, 0x00463405,
    0x80031462, 0x32050aa0, 0x5a0031e4, 0x00463205,
    0x80021a62, 0x42070aa0, 0x4a424407, 0x00424607,
    0x80021962, 0x35850aa0, 0x5a003564, 0x00343585,
    0x80021962, 0x36850aa0, 0x5a003664, 0x00343685,
    0x80021361, 0x3f670220, 0x00424207, 0x00000000,
    0x80001401, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x41050220, 0x020034e4, 0x000032e4,
    0x80001301, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x43050220, 0x02003ee4, 0x00003ae4,
    0x80031162, 0x36050aa0, 0x5a0035e4, 0x00463605,
    0x80021b62, 0x3f850aa0, 0x4a003f64, 0x00343f85,
    0x80021b62, 0x40850aa0, 0x4a004064, 0x00344085,
    0x00040070, 0x00018660, 0x16465805, 0x00000002,
    0x80031162, 0x40050aa0, 0x4a003fe4, 0x00464005,
    0x80000b01, 0x00000000, 0x00000000, 0x00000000,
    0x2f450062, 0x41013683, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0x2f470062, 0x43014083,
    0x00040070, 0x00018660, 0x16465a05, 0x00000000,
    0x2f611a62, 0x47204500, 0x00040065, 0x00018220,
    0x22465c05, 0x00000077, 0x01040022, 0x0001c060,
    0x00000090, 0x00000090, 0x80001101, 0x00000000,
    0x00000000, 0x00000000, 0x60480041, 0x00601d02,
    0x604a0041, 0x00305a02, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00040052, 0x4c040e68,
    0x0e0e4805, 0x4a055805, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00040069, 0x5f058660,
    0x02464c05, 0x00000002, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0x00044f31, 0x00000000,
    0xea2a5f14, 0x01006114, 0x00040025, 0x00004600,
    0x00000000, 0x00000070, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0xa04d0066, 0x2c221f02,
    0x80001161, 0x47064210, 0x00000000, 0x00000000,
    0x80001961, 0x30010110, 0x00004704, 0x00000000,
    0x00041b70, 0x00018220, 0x22464d05, 0x00000000,
    0x00040061, 0x4e050120, 0x00003000, 0x00000000,
    0x20371965, 0x4e003703, 0x00040027, 0x00014060,
    0x00000000, 0xfffff6a8, 0x80003f01, 0x00000000,
    0x00000000, 0x00000000, 0x80001e01, 0x00000000,
    0x00000000, 0x00000000, 0xa0560040, 0x02005403,
    0x80001161, 0x48064210, 0x00000000, 0x00000000,
    0x80001961, 0x30010110, 0x00004804, 0x00000000,
    0x00040070, 0x00018220, 0x22462c05, 0x00000000,
    0x80001d01, 0x00000000, 0x00000000, 0x00000000,
    0x00040061, 0x38050120, 0x00003000, 0x00000000,
    0x00041970, 0x00018660, 0x16463805, 0x00000000,
    0x01040028, 0x0001c660, 0x00000938, 0x00000938,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x80001101, 0x00000000, 0x00000000, 0x00000000,
    0x0004004c, 0x5e050220, 0x00463805, 0x00000000,
    0x80003e01, 0x00000000, 0x00000000, 0x00000000,
    0x80003f01, 0x00000000, 0x00000000, 0x00000000,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x80040061, 0x10014110, 0x00000000, 0x0ac00ac0,
    0x00040069, 0x10018510, 0x01565e06, 0x00020002,
    0x00040940, 0x10018110, 0x01461001, 0x0ac00ac0,
    0xe0600961, 0x001b0004, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0xac623f70, 0x56006002,
    0x00041965, 0x00010220, 0x22466205, 0x00462c05,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x64058220, 0x02461505, 0xff800000,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80001101, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x66058220, 0x02461705, 0xff800000,
    0x80001701, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x68058220, 0x02460d05, 0x7f800000,
    0x01040062, 0x6a058220, 0x02460f05, 0x7f800000,
    0xa3731c61, 0xff810000, 0x60730061, 0x00106400,
    0xa3751b61, 0xff810000, 0x60750061, 0x00106600,
    0xa36d1a61, 0x7f810000, 0x606d0061, 0x00106800,
    0xa36f1961, 0x7f810000, 0x606f0061, 0x00106a00,
    0x80001201, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x67058220, 0x02461905, 0xff800000,
    0x80001101, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x6b058220, 0x02461105, 0x7f800000,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x80031461, 0x24060220, 0x00447326, 0x00000000,
    0x80002401, 0x00000000, 0x00000000, 0x00000000,
    0x80031361, 0x3d060220, 0x00447526, 0x00000000,
    0x80031261, 0x4b060220, 0x00446d26, 0x00000000,
    0x80031161, 0x71060220, 0x00446f26, 0x00000000,
    0xa3770e61, 0xff810000, 0x60770061, 0x00106700,
    0x00040070, 0x00018660, 0x16465805, 0x00000001,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x80031d62, 0x22060aa0, 0x4a447306, 0x00442406,
    0x80031c62, 0x36060aa0, 0x4a447506, 0x00443d06,
    0x80030b62, 0x49060aa0, 0x5a446d06, 0x00444b06,
    0x80031a62, 0x69060aa0, 0x5a446f06, 0x00447106,
    0xa3711e61, 0x7f810000, 0x60710061, 0x00106b00,
    0x80030d61, 0x73260220, 0x00442206, 0x00000000,
    0x80030c61, 0x75260220, 0x00443606, 0x00000000,
    0x80030b61, 0x6d260220, 0x00444906, 0x00000000,
    0x80030a61, 0x6f260220, 0x00446906, 0x00000000,
    0x80031161, 0x06060220, 0x00447126, 0x00000000,
    0x80021d61, 0x30070220, 0x00427347, 0x00000000,
    0x80020061, 0x2e070220, 0x00427327, 0x00000000,
    0x80021e61, 0x42070220, 0x00427547, 0x00000000,
    0x80023e61, 0x40070220, 0x00427527, 0x00000000,
    0x80021f61, 0x5e070220, 0x00426d47, 0x00000000,
    0x80020061, 0x4e070220, 0x00426d27, 0x00000000,
    0x80031361, 0x4a060220, 0x00447726, 0x00000000,
    0x80021f61, 0x7d070220, 0x00426f47, 0x00000000,
    0x80020061, 0x7b070220, 0x00426f27, 0x00000000,
    0x80031f62, 0x04060aa0, 0x5a447106, 0x00440606,
    0x80021f62, 0x28070aa0, 0x4a422e07, 0x00423007,
    0x80021e62, 0x3e070aa0, 0x4a424007, 0x00424207,
    0x80021c62, 0x4c070aa0, 0x5a424e07, 0x00425e07,
    0x80031b62, 0x48060aa0, 0x4a447706, 0x00444a06,
    0x80021962, 0x79070aa0, 0x5a427b07, 0x00427d07,
    0x80030e61, 0x71260220, 0x00440406, 0x00000000,
    0x80021561, 0x73470220, 0x00422807, 0x00000000,
    0x80021461, 0x75470220, 0x00423e07, 0x00000000,
    0x80021361, 0x6d470220, 0x00424c07, 0x00000000,
    0x80030a61, 0x77260220, 0x00444806, 0x00000000,
    0x80021161, 0x6f470220, 0x00427907, 0x00000000,
    0x80021e61, 0x0b070220, 0x00427147, 0x00000000,
    0x80020061, 0x09070220, 0x00427127, 0x00000000,
    0x80021f61, 0x35070220, 0x00427367, 0x00000000,
    0x80020061, 0x33070220, 0x00427327, 0x00000000,
    0x80021f61, 0x45070220, 0x00427527, 0x00000000,
    0x80021f61, 0x68070220, 0x00426d67, 0x00000000,
    0x80020061, 0x66070220, 0x00426d27, 0x00000000,
    0x80020b61, 0x5e070220, 0x00427747, 0x00000000,
    0x80021361, 0x4d070220, 0x00427727, 0x00000000,
    0x80020061, 0x47070220, 0x00427567, 0x00000000,
    0x80020061, 0x03070220, 0x00426f67, 0x00000000,
    0x80020061, 0x01070220, 0x00426f27, 0x00000000,
    0x80020062, 0x07070aa0, 0x5a420907, 0x00420b07,
    0x80021f62, 0x31070aa0, 0x4a423307, 0x00423507,
    0x80021e62, 0x64070aa0, 0x5a426607, 0x00426807,
    0x80021c62, 0x4b070aa0, 0x4a424d07, 0x00425e07,
    0x80021b62, 0x43070aa0, 0x4a424507, 0x00424707,
    0x80021962, 0x7e070aa0, 0x5a420107, 0x00420307,
    0x80021661, 0x71470220, 0x00420707, 0x00000000,
    0x80021561, 0x73670220, 0x00423107, 0x00000000,
    0x80021461, 0x6d670220, 0x00426407, 0x00000000,
    0x80021361, 0x77470220, 0x00424b07, 0x00000000,
    0x80021261, 0x75670220, 0x00424307, 0x00000000,
    0x80021161, 0x6f670220, 0x00427e07, 0x00000000,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x80021e61, 0x21070220, 0x00427167, 0x00000000,
    0x80023c61, 0x1f070220, 0x00427127, 0x00000000,
    0x80021f62, 0x73850aa0, 0x4a007364, 0x00347385,
    0x80021f62, 0x74850aa0, 0x4a007464, 0x00347485,
    0x80021e62, 0x6d850aa0, 0x5a006d64, 0x00346d85,
    0x80021e62, 0x6e850aa0, 0x5a006e64, 0x00346e85,
    0x80020d61, 0x68070220, 0x00427767, 0x00000000,
    0x80021761, 0x66070220, 0x00427727, 0x00000000,
    0x80021e62, 0x75850aa0, 0x4a007564, 0x00347585,
    0x80021e62, 0x76850aa0, 0x4a007664, 0x00347685,
    0x80021d62, 0x6f850aa0, 0x5a006f64, 0x00346f85,
    0x80021d62, 0x70850aa0, 0x5a007064, 0x00347085,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x80021b62, 0x1d070aa0, 0x5a421f07, 0x00422107,
    0x80031762, 0x74050aa0, 0x4a0073e4, 0x00467405,
    0x80031762, 0x6e050aa0, 0x5a006de4, 0x00466e05,
    0x80021962, 0x64070aa0, 0x4a426607, 0x00426807,
    0x80031762, 0x76050aa0, 0x4a0075e4, 0x00467605,
    0x80031662, 0x70050aa0, 0x5a006fe4, 0x00467005,
    0x80021661, 0x71670220, 0x00421d07, 0x00000000,
    0x80021361, 0x77670220, 0x00426407, 0x00000000,
    0x80001201, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x7b050220, 0x020076e4, 0x000074e4,
    0x80001101, 0x00000000, 0x00000000, 0x00000000,
    0x01040062, 0x79050220, 0x020070e4, 0x00006ee4,
    0x80021c62, 0x71850aa0, 0x5a007164, 0x00347185,
    0x80021c62, 0x72850aa0, 0x5a007264, 0x00347285,
    0x80021b62, 0x77850aa0, 0x4a007764, 0x00347785,
    0x80021b62, 0x78850aa0, 0x4a007864, 0x00347885,
    0x00040070, 0x00018660, 0x16465805, 0x00000002,
    0x80031362, 0x72050aa0, 0x5a0071e4, 0x00467205,
    0x80031262, 0x78050aa0, 0x4a0077e4, 0x00467805,
    0x80000a01, 0x00000000, 0x00000000, 0x00000000,
    0x2f7d0062, 0x79017283, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0x2f010062, 0x7b017883,
    0x00040070, 0x00018660, 0x16465a05, 0x00000000,
    0x2f661a62, 0x01207d00, 0x00040065, 0x00018220,
    0x22465c05, 0x00000077, 0x01040022, 0x0001c060,
    0x00000090, 0x00000090, 0x80001101, 0x00000000,
    0x00000000, 0x00000000, 0x60020041, 0x00606002,
    0x60040041, 0x00305a02, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00040052, 0x06040e68,
    0x0e0e0205, 0x04055805, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00040069, 0x64058660,
    0x02460605, 0x00000002, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0x00044031, 0x00000000,
    0xea2a6414, 0x01006614, 0x00040025, 0x00004600,
    0x00000000, 0x00000070, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0xa0070066, 0x2c226202,
    0x80001761, 0x69064210, 0x00000000, 0x00000000,
    0x80001961, 0x30010110, 0x00006904, 0x00000000,
    0x00041b70, 0x00018220, 0x22460705, 0x00000000,
    0x00040061, 0x08050120, 0x00003000, 0x00000000,
    0x20381965, 0x08003803, 0x00040027, 0x00014060,
    0x00000000, 0xfffff6b8, 0x00040061, 0x00010660,
    0x20462c05, 0x00000000, 0x01040022, 0x0001c060,
    0x00000128, 0x00000128, 0x80001e01, 0x00000000,
    0x00000000, 0x00000000, 0x00040069, 0x09058660,
    0x02465005, 0x00000002, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0xa0670040, 0x48000903,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x00000000, 0xea106714, 0x01000000,
    0x80001101, 0x00000000, 0x00000000, 0x00000000,
    0x00040069, 0x0a058660, 0x02465205, 0x00000002,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0xa0683040, 0x4c000a03, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044131, 0x00000000,
    0xea106814, 0x01000000, 0x80001101, 0x00000000,
    0x00000000, 0x00000000, 0x00040069, 0x0b058660,
    0x02465405, 0x00000002, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0xa0693140, 0x50000b03,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x00000000, 0xea106914, 0x01000000,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x80000001, 0x00000000, 0x30000000, 0x00000000,
    0x80000001, 0x00000000, 0x30000000, 0x00000000,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x80004331, 0x0c0c0000, 0xe23e000c, 0x00000000,
    0x80002301, 0x00000000, 0x00000000, 0x00000000,
    0x80031761, 0x0d054220, 0x00000000, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x80011961, 0x0d550000, 0x0000005c, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x80044431, 0x00000000, 0x30080d0c, 0x00000000,
    0x80040001, 0x00000000, 0xe0000000, 0x00000000,
    0x00041e61, 0x39050220, 0x00463b05, 0x00000000,
    0x00041970, 0x00018220, 0x42463905, 0x00000120,
    0x01040028, 0x0001c660, 0x000001a0, 0x000001a0,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x00040069, 0x6a058660, 0x02463905, 0x00000002,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00044231, 0x6f140000, 0xea046a14, 0x00040000,
    0xae002270, 0x7f806f01, 0x01040022, 0x0001c060,
    0x00000120, 0x00000120, 0xa00e1740, 0x02c02603,
    0x27100970, 0x26000e03, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80002b01, 0x00000000,
    0x00000000, 0x00000000, 0xa0121740, 0x6a000e02,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x80002b01, 0x00000000, 0x00000000, 0x00000000,
    0x27140970, 0x0e001203, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x00030061, 0x6b060220,
    0x00341205, 0x00000000, 0x80103201, 0x00000000,
    0x00000000, 0x00000000, 0x00130061, 0x6d060220,
    0x00341305, 0x00000000, 0x00040b52, 0x16042e68,
    0x0e2e1005, 0x14052a05, 0x00131961, 0x6d260220,
    0x00341705, 0x00000000, 0x00031a61, 0x6b260220,
    0x00341605, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb2a6b24, 0x01006f14, 0x00040025, 0x00004600,
    0x00000000, 0x00000018, 0xa0390040, 0x20003903,
    0x00040027, 0x00014060, 0x00000000, 0xfffffe50,
    0x00041970, 0x00018220, 0x42463b05, 0x00000030,
    0x01040028, 0x0001c660, 0x000001e0, 0x000001e0,
    0x00040f69, 0x17058660, 0x02463b05, 0x00000002,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0xa0700040, 0x48001703, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x75140000,
    0xea047014, 0x00040000, 0x00042270, 0x00018660,
    0x26467505, 0x00000000, 0x01040022, 0x0001c060,
    0x00000140, 0x00000140, 0xa0191740, 0x4ac02603,
    0x80003201, 0x00000000, 0x00000000, 0x00000000,
    0x80001301, 0x00000000, 0x00000000, 0x00000000,
    0x271b9c70, 0x26001903, 0x80003c01, 0x00000000,
    0x00000000, 0x00000000, 0xa01d0040, 0x17001902,
    0x80003c01, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x271f0070, 0x19001d03, 0x80003201, 0x00000000,
    0x00000000, 0x00000000, 0x80001301, 0x00000000,
    0x00000000, 0x00000000, 0x00030061, 0x71060220,
    0x00341d05, 0x00000000, 0x80103201, 0x00000000,
    0x00000000, 0x00000000, 0x00130061, 0x73060220,
    0x00341e05, 0x00000000, 0x80003c01, 0x00000000,
    0x00000000, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x00040052, 0x21042e68,
    0x0e2e1b05, 0x1f052a05, 0x00131961, 0x73260220,
    0x00342205, 0x00000000, 0x00031a61, 0x71260220,
    0x00342105, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb187124, 0x01007514, 0x00040025, 0x00004600,
    0x00000000, 0x00000018, 0xa03b0040, 0x20003b03,
    0x00040027, 0x00014060, 0x00000000, 0xfffffe10,
    0x80031161, 0x7e050220, 0x00460005, 0x00000000,
    0x80040931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_build_BFS_BFS_pass1_initial_batchable = {
   .prog_data = {
      .base.nr_params = 12,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 1344,
      .base.program_size = 13408,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_build_BFS_BFS_pass1_initial_batchable_relocs,
      .base.uses_atomic_load_store = false,
      .local_size = { 512, 1, 1 },
      .prog_offset = { 0, 0, 0 },
      .prog_mask = 2,
      .prog_spilled = 0,
      .uses_barrier = true,
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
   .args = gfx125_bvh_build_BFS_BFS_pass1_initial_batchable_args,
   .code = gfx125_bvh_build_BFS_BFS_pass1_initial_batchable_code,
};
const char *gfx125_bvh_build_BFS_BFS_pass1_initial_batchable_sha1 = "bf780b8dd391c04cacdebd70a43d49705699b311";
