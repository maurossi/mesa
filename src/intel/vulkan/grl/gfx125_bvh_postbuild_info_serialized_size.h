#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_postbuild_info_serialized_size_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_postbuild_info_serialized_size_args[] = {
   { 0, 8 },
   { 8, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g24<1>UD        g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
add(1)          g115<1>UD       g24<0,1,0>UD    0x00000000UD    { align1 WE_all 1N I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(1)         g1UD            g115UD          nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g11.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g3.1<2>F        g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g11<2>F         g2<0,1,0>F                      { align1 1Q F@2 compacted };
mov(8)          g3<2>F          g2.2<0,1,0>F                    { align1 1Q F@2 compacted };
add(8)          g13<1>D         g11<8,4,2>D     156D            { align1 1Q F@2 compacted };
add(8)          g18<1>D         g11<8,4,2>D     32D             { align1 1Q compacted };
add(8)          g29<1>D         g11<8,4,2>D     16D             { align1 1Q compacted };
cmp.l.f0.0(8)   g14<1>UD        g13<1,1,0>UD    0x0000009cUD    { align1 1Q I@3 compacted };
mov(8)          g124<2>UD       g13<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g19<1>UD        g18<1,1,0>UD    0x00000020UD    { align1 1Q I@4 compacted };
mov(8)          g126<2>UD       g18<4,4,1>UD                    { align1 1Q };
mov(8)          g9<2>UD         g29<4,4,1>UD                    { align1 1Q I@5 };
cmp.l.f0.0(8)   g30<1>UD        g29<1,1,0>UD    0x00000010UD    { align1 1Q compacted };
add(8)          g38<1>D         -g14<8,8,1>D    g11.1<8,4,2>D   { align1 1Q I@6 };
add(8)          g20<1>D         -g19<8,8,1>D    g11.1<8,4,2>D   { align1 1Q I@5 };
add(8)          g31<1>D         -g30<8,8,1>D    g11.1<8,4,2>D   { align1 1Q I@3 };
mov(8)          g124.1<2>UD     g38<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g126.1<2>UD     g20<4,4,1>UD                    { align1 1Q I@3 };
mov(8)          g9.1<2>UD       g31<4,4,1>UD                    { align1 1Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(8)         g7UD            g124UD          nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(8)         g21UD           g126UD          nullUD          0x0440f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 4, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g32UD           g9UD            nullUD          0x0440f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 4, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $3 };
shl(8)          g97<1>D         g7<8,8,1>D      0x00000003UD    { align1 1Q $1.dst };
shr(8)          g44<1>UD        g7<1,1,0>UD     0x0000001dUD    { align1 1Q compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(8)          g115<1>D        g22<1,1,0>D     -g21<1,1,0>D    { align1 1Q $2.dst compacted };
add(8)          g36<1>D         g35<1,1,0>D     -g34<1,1,0>D    { align1 1Q $3.dst compacted };
add(8)          g42<1>D         g33<1,1,0>D     -g32<1,1,0>D    { align1 1Q $3.dst compacted };
shr(8)          g26<1>UD        g115<1,1,0>UD   0x00000001UD    { align1 1Q I@3 compacted };
shr(8)          g28<1>UD        g115<1,1,0>UD   0x0000001aUD    { align1 1Q compacted };
shl(8)          g37<1>D         g36<8,8,1>D     0x00000006UD    { align1 1Q I@4 };
shr(8)          g15<1>UD        g36<1,1,0>UD    0x0000001aUD    { align1 1Q compacted };
shl(8)          g43<1>D         g42<8,8,1>D     0x00000006UD    { align1 1Q I@5 };
shr(8)          g17<1>UD        g42<1,1,0>UD    0x0000001aUD    { align1 1Q compacted };
shl(8)          g27<1>D         g26<8,8,1>D     0x00000007UD    { align1 1Q I@6 };
add(8)          g39<1>D         g27<1,1,0>D     g37<1,1,0>D     { align1 1Q I@1 compacted };
cmp.l.f0.0(8)   g40<1>UD        g39<1,1,0>UD    g27<1,1,0>UD    { align1 1Q I@1 compacted };
add(8)          g45<1>D         g39<1,1,0>D     g43<1,1,0>D     { align1 1Q I@5 compacted };
add3(8)         g41<1>D         g28<8,8,1>D     g15<8,8,1>D     -g40<1,1,1>D { align1 1Q I@2 };
cmp.l.f0.0(8)   g46<1>UD        g45<1,1,0>UD    g39<1,1,0>UD    { align1 1Q I@2 compacted };
cmp.l.f0.0(8)   null<1>UD       g23<8,8,1>UD    g24<8,8,1>UD    { align1 1Q $2.dst };
add3(8)         g47<1>D         g41<8,8,1>D     g17<8,8,1>D     -g46<1,1,1>D { align1 1Q I@2 };
(+f0.0) if(8)   JIP:  LABEL1          UIP:  LABEL0              { align1 1Q };
add(8)          g48<1>D         g11<8,4,2>D     64D             { align1 1Q compacted };
add(8)          g59<1>D         g11<8,4,2>D     88D             { align1 1Q compacted };
cmp.l.f0.0(8)   g49<1>UD        g48<8,8,1>UD    g11<8,4,2>UD    { align1 1Q I@2 };
mov(8)          g13<2>UD        g48<4,4,1>UD                    { align1 1Q };
cmp.l.f0.0(8)   g60<1>UD        g59<8,8,1>UD    g11<8,4,2>UD    { align1 1Q I@3 };
mov(8)          g18<2>UD        g59<4,4,1>UD                    { align1 1Q };
add(8)          g50<1>D         -g49<8,8,1>D    g11.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g61<1>D         -g60<8,8,1>D    g11.1<8,4,2>D   { align1 1Q I@3 };
mov(8)          g13.1<2>UD      g50<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g18.1<2>UD      g61<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(8)         g51UD           g13UD           nullUD          0x04203582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 2, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g62UD           g18UD           nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $5 };
mul(8)          g53<1>D         g51<1,1,0>D     12W             { align1 1Q $4.dst compacted };
shl(8)          g56<1>D         g52<8,8,1>D     0x00000003UD    { align1 1Q $4.dst };
cmp.l.f0.0(8)   null<1>UD       g23<8,8,1>UD    g62<8,8,1>UD    { align1 1Q $5.dst };
add(8)          g54<1>D         g53<1,1,0>D     63D             { align1 1Q I@3 compacted };
add(8)          g57<1>D         g56<1,1,0>D     63D             { align1 1Q I@3 compacted };
and(8)          g55<1>UD        g54<8,8,1>UD    0xffffffc0UD    { align1 1Q I@2 };
and(8)          g58<1>UD        g57<8,8,1>UD    0xffffffc0UD    { align1 1Q I@2 };
(+f0.0) if(8)   JIP:  LABEL3          UIP:  LABEL2              { align1 1Q };
add(8)          g63<1>D         g42<1,1,0>D     255D            { align1 1Q compacted };
add(8)          g65<1>D         g11<8,4,2>D     80D             { align1 1Q compacted };
shl(8)          g75<1>D         g42<8,8,1>D     0x00000005UD    { align1 1Q };
and(8)          g64<1>UD        g63<8,8,1>UD    0xffffff00UD    { align1 1Q I@3 };
cmp.l.f0.0(8)   g66<1>UD        g65<8,8,1>UD    g11<8,4,2>UD    { align1 1Q I@3 };
mov(8)          g19<2>UD        g65<4,4,1>UD                    { align1 1Q $5.src };
add(8)          g76<1>D         g75<1,1,0>D     63D             { align1 1Q I@4 compacted };
add(8)          g67<1>D         -g66<8,8,1>D    g11.1<8,4,2>D   { align1 1Q I@3 };
and(8)          g77<1>UD        g76<8,8,1>UD    0xffffffc0UD    { align1 1Q I@2 };
mov(8)          g19.1<2>UD      g67<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g68UD           g19UD           nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $5 };
add(8)          g69<1>D         g68<1,1,0>D     255D            { align1 1Q $5.dst compacted };
and(8)          g70<1>UD        g69<8,8,1>UD    0xffffff00UD    { align1 1Q I@1 };
add(8)          g71<1>D         g64<1,1,0>D     g70<1,1,0>D     { align1 1Q I@1 compacted };
mul(8)          g72<1>D         g71<1,1,0>D     12W             { align1 1Q I@1 compacted };
add(8)          g73<1>D         g72<1,1,0>D     63D             { align1 1Q I@1 compacted };
and(8)          g74<1>UD        g73<8,8,1>UD    0xffffffc0UD    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
add(8)          g9<1>D          g74<1,1,0>D     g77<1,1,0>D     { align1 1Q I@1 compacted };
else(8)         JIP:  LABEL2          UIP:  LABEL2              { align1 1Q };

LABEL3:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(8)          g9<1>UD         0x00000000UD                    { align1 1Q I@2 };

LABEL2:
endif(8)        JIP:  LABEL4                                    { align1 1Q };
add(8)          g78<1>D         g11<8,4,2>D     12D             { align1 1Q compacted };
cmp.l.f0.0(8)   g79<1>UD        g78<8,8,1>UD    g11<8,4,2>UD    { align1 1Q I@1 };
mov(8)          g20<2>UD        g78<4,4,1>UD                    { align1 1Q $5.src };
add(8)          g80<1>D         -g79<8,8,1>D    g11.1<8,4,2>D   { align1 1Q I@2 };
mov(8)          g20.1<2>UD      g80<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g81UD           g20UD           nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $6 };
shl(8)          g82<1>D         g81<8,8,1>D     0x00000002UD    { align1 1Q $6.dst };
add(8)          g83<1>D         g82<1,1,0>D     51D             { align1 1Q I@1 compacted };
and(8)          g84<1>UD        g83<8,8,1>UD    0xffffffc0UD    { align1 1Q I@1 };
add(8)          g85<1>D         g84<1,1,0>D     g55<1,1,0>D     { align1 1Q I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
add3(8)         g10<1>D         g85<8,8,1>D     g58<8,8,1>D     g9<1,1,1>D { align1 1Q I@1 };

LABEL4:
else(8)         JIP:  LABEL0          UIP:  LABEL0              { align1 1Q };

LABEL1:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(8)          g10<1>UD        0x00000000UD                    { align1 1Q I@2 };

LABEL0:
endif(8)        JIP:  LABEL5                                    { align1 1Q };

LABEL5:
add(8)          g86<1>D         g45<1,1,0>D     g10<1,1,0>D     { align1 1Q I@2 compacted };
add(8)          g88<1>D         g11<8,4,2>D     12D             { align1 1Q compacted };
shl(8)          g98<1>D         g7<8,8,1>D      0x00000006UD    { align1 1Q };
shr(8)          g99<1>UD        g7<1,1,0>UD     0x0000001aUD    { align1 1Q compacted };
add(8)          g103<1>D        g11<8,4,2>D     164D            { align1 1Q compacted };
add(8)          g120<1>D        g97<1,1,0>D     56D             { align1 1Q compacted };
mov(8)          g8<1>D          0D                              { align1 1Q };
cmp.l.f0.0(8)   g87<1>UD        g86<1,1,0>UD    g45<1,1,0>UD    { align1 1Q I@7 compacted };
cmp.l.f0.0(8)   g89<1>UD        g88<8,8,1>UD    g11<8,4,2>UD    { align1 1Q I@7 };
mov(8)          g21<2>UD        g88<4,4,1>UD                    { align1 1Q $6.src };
mov(8)          g23<2>UD        g103<4,4,1>UD                   { align1 1Q I@6 };
add(8)          g90<1>D         -g89<8,8,1>D    g11.1<8,4,2>D   { align1 1Q I@3 };
mov(8)          g21.1<2>UD      g90<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g91UD           g21UD           nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $7 };
add(8)          g92<1>D         g91<1,1,0>D     -3D             { align1 1Q $7.dst compacted };
shl(8)          g93<1>D         g91<8,8,1>D     0x00000006UD    { align1 1Q };
shr(8)          g94<1>UD        g92<1,1,0>UD    0x0000001aUD    { align1 1Q I@2 compacted };
add3(8)         g95<1>D         65344W          g93<8,8,1>D     g86<1,1,1>D { align1 1Q I@2 };
add3(8)         g16<1>D         -g87<8,8,1>D    g47<8,8,1>D     g94<1,1,1>D { align1 1Q I@2 };
cmp.l.f0.0(8)   g96<1>UD        g95<1,1,0>UD    g86<1,1,0>UD    { align1 1Q I@2 compacted };
add(8)          g100<1>D        g95<1,1,0>D     g98<1,1,0>D     { align1 1Q compacted };
add3(8)         g102<1>D        -g96<8,8,1>D    g16<8,8,1>D     g99<1,1,1>D { align1 1Q I@2 };
cmp.l.f0.0(8)   g101<1>UD       g100<1,1,0>UD   g95<1,1,0>UD    { align1 1Q I@2 compacted };
cmp.l.f0.0(8)   g104<1>UD       g103<1,1,0>UD   0x000000a4UD    { align1 1Q compacted };
add(8)          g105<1>D        -g104<8,8,1>D   g11.1<8,4,2>D   { align1 1Q I@1 };
mov(8)          g23.1<2>UD      g105<4,4,1>UD                   { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g106UD          g23UD           nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $8 };
shl(8)          g107<1>D        g106<8,8,1>D    0x00000003UD    { align1 1Q $8.dst };
shr(8)          g108<1>UD       g106<1,1,0>UD   0x0000001dUD    { align1 1Q compacted };
add(8)          g109<1>D        g107<1,1,0>D    63D             { align1 1Q I@2 compacted };
cmp.l.f0.0(8)   g110<1>UD       g109<1,1,0>UD   g107<1,1,0>UD   { align1 1Q I@1 compacted };
and(8)          g112<1>UD       g109<8,8,1>UD   0xffffffc0UD    { align1 1Q };
add(8)          g111<1>D        -g110<1,1,0>D   g108<1,1,0>D    { align1 1Q I@2 compacted };
add(8)          g113<1>D        g100<1,1,0>D    g112<1,1,0>D    { align1 1Q I@2 compacted };
add3(8)         g25<1>D         -g101<8,8,1>D   g102<8,8,1>D    g111<1,1,1>D { align1 1Q I@2 };
cmp.l.f0.0(8)   g114<1>UD       g113<1,1,0>UD   g100<1,1,0>UD   { align1 1Q I@2 compacted };
add(8)          g116<1>D        g113<1,1,0>D    255D            { align1 1Q compacted };
cmp.l.f0.0(8)   g117<1>UD       g116<1,1,0>UD   0x000000ffUD    { align1 1Q I@1 compacted };
and(8)          g119<1>UD       g116<8,8,1>UD   0xffffffc0UD    { align1 1Q };
cmp.l.f0.0(8)   g121<1>UD       g120<1,1,0>UD   g97<1,1,0>UD    { align1 1Q compacted };
add3(8)         g118<1>D        -g114<8,8,1>D   g25<8,8,1>D     -g117<1,1,1>D { align1 1Q I@3 };
add(8)          g5<1>D          g120<1,1,0>D    g119<1,1,0>D    { align1 1Q I@3 compacted };
add(8)          g122<1>D        -g121<1,1,0>D   g44<1,1,0>D     { align1 1Q I@3 compacted };
cmp.l.f0.0(8)   g123<1>UD       g5<1,1,0>UD     g120<1,1,0>UD   { align1 1Q I@2 compacted };
add3(8)         g6<1>D          g122<8,8,1>D    g118<8,8,1>D    -g123<1,1,1>D { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(8)         nullUD          g3UD            g5UD            0x0400f586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 4 flat )  base_offset 0  { align1 1Q $9 };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q $2.src };
send(8)         nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1Q A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_postbuild_info_serialized_size_code[] = {
    0x80000065, 0x18058220, 0x02000004, 0xffffffc0,
    0xe2731940, 0x00011803, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa00730c, 0x00340000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x210b0061, 0x001102cc,
    0x00030061, 0x03260aa0, 0x00000264, 0x00000000,
    0x210b1261, 0x00110204, 0x21031261, 0x00110244,
    0xa10d1240, 0x09ce0b03, 0xa1120040, 0x020e0b03,
    0xa11d0040, 0x010e0b03, 0xf00e1b70, 0x09c00d03,
    0x00030061, 0x7c060220, 0x00340d05, 0x00000000,
    0xf0131c70, 0x02001203, 0x00030061, 0x7e060220,
    0x00341205, 0x00000000, 0x00031d61, 0x09060220,
    0x00341d05, 0x00000000, 0xf01e0070, 0x01001d03,
    0x00031e40, 0x26052660, 0x06460e05, 0x00440b26,
    0x00031d40, 0x14052660, 0x06461305, 0x00440b26,
    0x00031b40, 0x1f052660, 0x06461e05, 0x00440b26,
    0x00031b61, 0x7c260220, 0x00342605, 0x00000000,
    0x00031b61, 0x7e260220, 0x00341405, 0x00000000,
    0x00031b61, 0x09260220, 0x00341f05, 0x00000000,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00034131, 0x070c0000, 0xfb047c14, 0x00040000,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00034231, 0x15240000, 0xfb047e14, 0x003c0000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034331, 0x20240000, 0xfb040914, 0x003c0000,
    0x00032169, 0x61058660, 0x02460705, 0x00000003,
    0xe12c0068, 0x01d00703, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0xa1732240, 0x15201602,
    0xa1242340, 0x22202302, 0xa12a2340, 0x20202102,
    0xe11a1b68, 0x00107303, 0xe11c0068, 0x01a07303,
    0x00031c69, 0x25058660, 0x02462405, 0x00000006,
    0xe10f0068, 0x01a02403, 0x00031d69, 0x2b058660,
    0x02462a05, 0x00000006, 0xe1110068, 0x01a02a03,
    0x00031e69, 0x1b058660, 0x02461a05, 0x00000007,
    0xa1271940, 0x25001b02, 0x30281970, 0x1b002703,
    0xa12d1d40, 0x2b002702, 0x00031a52, 0x29040e68,
    0x0e2e1c05, 0x28050f05, 0x302e1a70, 0x27002d03,
    0x00032270, 0x00010220, 0x52461705, 0x00461805,
    0x00031a52, 0x2f040e68, 0x0e2e2905, 0x2e051105,
    0x01030022, 0x0001c060, 0x00000370, 0x00000350,
    0xa1300040, 0x040e0b03, 0xa13b0040, 0x058e0b03,
    0x00031a70, 0x31050220, 0x52463005, 0x00440b06,
    0x00030061, 0x0d060220, 0x00343005, 0x00000000,
    0x00031b70, 0x3c050220, 0x52463b05, 0x00440b06,
    0x00030061, 0x12060220, 0x00343b05, 0x00000000,
    0x00031c40, 0x32052660, 0x06463105, 0x00440b26,
    0x00031b40, 0x3d052660, 0x06463c05, 0x00440b26,
    0x00031a61, 0x0d260220, 0x00343205, 0x00000000,
    0x00031a61, 0x12260220, 0x00343d05, 0x00000000,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00034431, 0x33140000, 0xfb040d14, 0x000c0000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034531, 0x3e0c0000, 0xfb041214, 0x00040000,
    0x61352441, 0x00c03302, 0x00032469, 0x38058660,
    0x02463405, 0x00000003, 0x00032570, 0x00010220,
    0x52461705, 0x00463e05, 0xa1361b40, 0x03f03503,
    0xa1391b40, 0x03f03803, 0x00031a65, 0x37058220,
    0x02463605, 0xffffffc0, 0x00031a65, 0x3a058220,
    0x02463905, 0xffffffc0, 0x01030022, 0x0001c060,
    0x00000140, 0x00000120, 0xa13f0040, 0x0ff02a03,
    0xa1410040, 0x050e0b03, 0x00030069, 0x4b058660,
    0x02462a05, 0x00000005, 0x00031b65, 0x40058220,
    0x02463f05, 0xffffff00, 0x00031b70, 0x42050220,
    0x52464105, 0x00440b06, 0x00033561, 0x13060220,
    0x00344105, 0x00000000, 0xa14c1c40, 0x03f04b03,
    0x00031b40, 0x43052660, 0x06464205, 0x00440b26,
    0x00031a65, 0x4d058220, 0x02464c05, 0xffffffc0,
    0x00031a61, 0x13260220, 0x00344305, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034531, 0x440c0000, 0xfb041314, 0x00040000,
    0xa1452540, 0x0ff04403, 0x00031965, 0x46058220,
    0x02464505, 0xffffff00, 0xa1471940, 0x46004002,
    0x61481941, 0x00c04702, 0xa1491940, 0x03f04803,
    0x00031965, 0x4a058220, 0x02464905, 0xffffffc0,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0xa1091940, 0x4d004a02, 0x00030024, 0x0001c060,
    0x00000030, 0x00000030, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x00031a61, 0x09054220,
    0x00000000, 0x00000000, 0x00030025, 0x00004600,
    0x00000000, 0x000000c8, 0xa14e0040, 0x00ce0b03,
    0x00031970, 0x4f050220, 0x52464e05, 0x00440b06,
    0x00033561, 0x14060220, 0x00344e05, 0x00000000,
    0x00031a40, 0x50052660, 0x06464f05, 0x00440b26,
    0x00031961, 0x14260220, 0x00345005, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034631, 0x510c0000, 0xfb041414, 0x00040000,
    0x00032669, 0x52058660, 0x02465105, 0x00000002,
    0xa1531940, 0x03305203, 0x00031965, 0x54058220,
    0x02465305, 0xffffffc0, 0xa1551940, 0x37005402,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x00031952, 0x0a040e68, 0x0e0e5505, 0x09053a05,
    0x00030024, 0x0001c060, 0x00000030, 0x00000030,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x00031a61, 0x0a054220, 0x00000000, 0x00000000,
    0x00030025, 0x00004600, 0x00000000, 0x00000010,
    0xa1561a40, 0x0a002d02, 0xa1580040, 0x00ce0b03,
    0x00030069, 0x62058660, 0x02460705, 0x00000006,
    0xe1630068, 0x01a00703, 0xa1670040, 0x0a4e0b03,
    0xa1780040, 0x03806103, 0x00030061, 0x08054660,
    0x00000000, 0x00000000, 0x30571f70, 0x2d005603,
    0x00031f70, 0x59050220, 0x52465805, 0x00440b06,
    0x00033661, 0x15060220, 0x00345805, 0x00000000,
    0x00031e61, 0x17060220, 0x00346705, 0x00000000,
    0x00031b40, 0x5a052660, 0x06465905, 0x00440b26,
    0x00031961, 0x15260220, 0x00345a05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034731, 0x5b0c0000, 0xfb041514, 0x00040000,
    0xa15c2740, 0xffd05b03, 0x00030069, 0x5d058660,
    0x02465b05, 0x00000006, 0xe15e1a68, 0x01a05c03,
    0x00031a52, 0x5f044560, 0x0e0eff40, 0x56055d05,
    0x00031a52, 0x10042e68, 0x0e0e5705, 0x5e052f05,
    0x30601a70, 0x56005f03, 0xa1640040, 0x62005f02,
    0x00031a52, 0x66042e68, 0x0e0e6005, 0x63051005,
    0x30651a70, 0x5f006403, 0xf0680070, 0x0a406703,
    0x00031940, 0x69052660, 0x06466805, 0x00440b26,
    0x00031961, 0x17260220, 0x00346905, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034831, 0x6a0c0000, 0xfb041714, 0x00040000,
    0x00032869, 0x6b058660, 0x02466a05, 0x00000003,
    0xe16c0068, 0x01d06a03, 0xa16d1a40, 0x03f06b03,
    0x306e1970, 0x6b006d03, 0x00030065, 0x70058220,
    0x02466d05, 0xffffffc0, 0xa16f1a40, 0x6c026e02,
    0xa1711a40, 0x70006402, 0x00031a52, 0x19042e68,
    0x0e0e6505, 0x6f056605, 0x30721a70, 0x64007103,
    0xa1740040, 0x0ff07103, 0xf0751970, 0x0ff07403,
    0x00030065, 0x77058220, 0x02467405, 0xffffffc0,
    0x30790070, 0x61007803, 0x00031b52, 0x76042e68,
    0x0e2e7205, 0x75051905, 0xa1051b40, 0x77007802,
    0xa17a1b40, 0x2c027902, 0x307b1a70, 0x78000503,
    0x00031952, 0x06040e68, 0x0e2e7a05, 0x7b057605,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00034931, 0x00000000, 0xfb0c0314, 0x003c0524,
    0x80033261, 0x7e050220, 0x00460005, 0x00000000,
    0x80030931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_postbuild_info_serialized_size = {
   .prog_data = {
      .base.nr_params = 12,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 2144,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_postbuild_info_serialized_size_relocs,
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
      .push.cross_thread.dwords = 12,
      .push.cross_thread.regs = 2,
      .push.cross_thread.size = 64,
   },
   .args_size = 16,
   .arg_count = 2,
   .args = gfx125_bvh_postbuild_info_serialized_size_args,
   .code = gfx125_bvh_postbuild_info_serialized_size_code,
};
const char *gfx125_bvh_postbuild_info_serialized_size_sha1 = "6ca36a8b3a463fff378b93fd63ed8fcbbf394021";
