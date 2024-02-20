#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_postbuild_info_compacted_size_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_postbuild_info_compacted_size_args[] = {
   { 0, 8 },
   { 8, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g127<1>UD       g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
add(1)          g88<1>UD        g127<0,1,0>UD   0x00000000UD    { align1 WE_all 1N I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(1)         g1UD            g88UD           nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g7.1<2>F        g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g3.1<2>F        g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g7<2>F          g2<0,1,0>F                      { align1 1Q F@2 compacted };
mov(8)          g3<2>F          g2.2<0,1,0>F                    { align1 1Q F@2 compacted };
add(8)          g110<1>D        g7<8,4,2>D      32D             { align1 1Q F@2 compacted };
add(8)          g20<1>D         g7<8,4,2>D      16D             { align1 1Q compacted };
cmp.l.f0.0(8)   g35<1>UD        g110<1,1,0>UD   0x00000020UD    { align1 1Q I@2 compacted };
mov(8)          g113<2>UD       g110<4,4,1>UD                   { align1 1Q };
cmp.l.f0.0(8)   g21<1>UD        g20<1,1,0>UD    0x00000010UD    { align1 1Q I@3 compacted };
mov(8)          g115<2>UD       g20<4,4,1>UD                    { align1 1Q };
add(8)          g11<1>D         -g35<8,8,1>D    g7.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g22<1>D         -g21<8,8,1>D    g7.1<8,4,2>D    { align1 1Q I@3 };
mov(8)          g113.1<2>UD     g11<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g115.1<2>UD     g22<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(8)         g12UD           g113UD          nullUD          0x0440f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 4, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g23UD           g115UD          nullUD          0x0440f582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xyzw, L1STATE_L3MOCS dst_len = 4, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $2 };
add(8)          g16<1>D         g13<1,1,0>D     -g12<1,1,0>D    { align1 1Q $1.dst compacted };
add(8)          g27<1>D         g26<1,1,0>D     -g25<1,1,0>D    { align1 1Q $2.dst compacted };
add(8)          g33<1>D         g24<1,1,0>D     -g23<1,1,0>D    { align1 1Q $2.dst compacted };
shr(8)          g17<1>UD        g16<1,1,0>UD    0x00000001UD    { align1 1Q I@3 compacted };
shr(8)          g19<1>UD        g16<1,1,0>UD    0x0000001aUD    { align1 1Q compacted };
shl(8)          g28<1>D         g27<8,8,1>D     0x00000006UD    { align1 1Q I@4 };
shr(8)          g6<1>UD         g27<1,1,0>UD    0x0000001aUD    { align1 1Q compacted };
shl(8)          g34<1>D         g33<8,8,1>D     0x00000006UD    { align1 1Q I@5 };
shr(8)          g10<1>UD        g33<1,1,0>UD    0x0000001aUD    { align1 1Q compacted };
shl(8)          g18<1>D         g17<8,8,1>D     0x00000007UD    { align1 1Q I@6 };
add(8)          g30<1>D         g18<1,1,0>D     g28<1,1,0>D     { align1 1Q I@1 compacted };
cmp.l.f0.0(8)   g31<1>UD        g30<1,1,0>UD    g18<1,1,0>UD    { align1 1Q I@1 compacted };
add(8)          g36<1>D         g30<1,1,0>D     g34<1,1,0>D     { align1 1Q I@5 compacted };
add3(8)         g32<1>D         g19<8,8,1>D     g6<8,8,1>D      -g31<1,1,1>D { align1 1Q I@2 };
cmp.l.f0.0(8)   g37<1>UD        g36<1,1,0>UD    g30<1,1,0>UD    { align1 1Q I@2 compacted };
cmp.l.f0.0(8)   null<1>UD       g14<8,8,1>UD    g15<8,8,1>UD    { align1 1Q $1.dst };
add3(8)         g38<1>D         g32<8,8,1>D     g10<8,8,1>D     -g37<1,1,1>D { align1 1Q I@2 };
(+f0.0) if(8)   JIP:  LABEL1          UIP:  LABEL0              { align1 1Q };
add(8)          g39<1>D         g7<8,4,2>D      64D             { align1 1Q compacted };
add(8)          g50<1>D         g7<8,4,2>D      88D             { align1 1Q compacted };
cmp.l.f0.0(8)   g40<1>UD        g39<8,8,1>UD    g7<8,4,2>UD     { align1 1Q I@2 };
mov(8)          g116<2>UD       g39<4,4,1>UD                    { align1 1Q $2.src };
cmp.l.f0.0(8)   g51<1>UD        g50<8,8,1>UD    g7<8,4,2>UD     { align1 1Q I@3 };
mov(8)          g118<2>UD       g50<4,4,1>UD                    { align1 1Q };
add(8)          g41<1>D         -g40<8,8,1>D    g7.1<8,4,2>D    { align1 1Q I@4 };
add(8)          g52<1>D         -g51<8,8,1>D    g7.1<8,4,2>D    { align1 1Q I@3 };
mov(8)          g116.1<2>UD     g41<4,4,1>UD                    { align1 1Q I@2 };
mov(8)          g118.1<2>UD     g52<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(8)         g42UD           g116UD          nullUD          0x04203582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 2, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g53UD           g118UD          nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $3 };
mul(8)          g44<1>D         g42<1,1,0>D     12W             { align1 1Q $2.dst compacted };
shl(8)          g47<1>D         g43<8,8,1>D     0x00000003UD    { align1 1Q $2.dst };
cmp.l.f0.0(8)   null<1>UD       g14<8,8,1>UD    g53<8,8,1>UD    { align1 1Q $3.dst };
add(8)          g45<1>D         g44<1,1,0>D     63D             { align1 1Q I@3 compacted };
add(8)          g48<1>D         g47<1,1,0>D     63D             { align1 1Q I@3 compacted };
and(8)          g46<1>UD        g45<8,8,1>UD    0xffffffc0UD    { align1 1Q I@2 };
and(8)          g49<1>UD        g48<8,8,1>UD    0xffffffc0UD    { align1 1Q I@2 };
(+f0.0) if(8)   JIP:  LABEL3          UIP:  LABEL2              { align1 1Q };
add(8)          g54<1>D         g33<1,1,0>D     255D            { align1 1Q compacted };
add(8)          g56<1>D         g7<8,4,2>D      80D             { align1 1Q compacted };
shl(8)          g66<1>D         g33<8,8,1>D     0x00000005UD    { align1 1Q };
and(8)          g55<1>UD        g54<8,8,1>UD    0xffffff00UD    { align1 1Q I@3 };
cmp.l.f0.0(8)   g57<1>UD        g56<8,8,1>UD    g7<8,4,2>UD     { align1 1Q I@3 };
mov(8)          g119<2>UD       g56<4,4,1>UD                    { align1 1Q $3.src };
add(8)          g67<1>D         g66<1,1,0>D     63D             { align1 1Q I@4 compacted };
add(8)          g58<1>D         -g57<8,8,1>D    g7.1<8,4,2>D    { align1 1Q I@3 };
and(8)          g68<1>UD        g67<8,8,1>UD    0xffffffc0UD    { align1 1Q I@2 };
mov(8)          g119.1<2>UD     g58<4,4,1>UD                    { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g59UD           g119UD          nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $3 };
add(8)          g60<1>D         g59<1,1,0>D     255D            { align1 1Q $3.dst compacted };
and(8)          g61<1>UD        g60<8,8,1>UD    0xffffff00UD    { align1 1Q I@1 };
add(8)          g62<1>D         g55<1,1,0>D     g61<1,1,0>D     { align1 1Q I@1 compacted };
mul(8)          g63<1>D         g62<1,1,0>D     12W             { align1 1Q I@1 compacted };
add(8)          g64<1>D         g63<1,1,0>D     63D             { align1 1Q I@1 compacted };
and(8)          g65<1>UD        g64<8,8,1>UD    0xffffffc0UD    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
add(8)          g88<1>D         g65<1,1,0>D     g68<1,1,0>D     { align1 1Q I@1 compacted };
else(8)         JIP:  LABEL2          UIP:  LABEL2              { align1 1Q };

LABEL3:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
mov(8)          g88<1>UD        0x00000000UD                    { align1 1Q };

LABEL2:
endif(8)        JIP:  LABEL4                                    { align1 1Q };
add(8)          g69<1>D         g7<8,4,2>D      12D             { align1 1Q compacted };
cmp.l.f0.0(8)   g70<1>UD        g69<8,8,1>UD    g7<8,4,2>UD     { align1 1Q I@1 };
mov(8)          g120<2>UD       g69<4,4,1>UD                    { align1 1Q $3.src };
add(8)          g71<1>D         -g70<8,8,1>D    g7.1<8,4,2>D    { align1 1Q I@2 };
mov(8)          g120.1<2>UD     g71<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g72UD           g120UD          nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $4 };
shl(8)          g73<1>D         g72<8,8,1>D     0x00000002UD    { align1 1Q $4.dst };
add(8)          g74<1>D         g73<1,1,0>D     51D             { align1 1Q I@1 compacted };
and(8)          g75<1>UD        g74<8,8,1>UD    0xffffffc0UD    { align1 1Q I@1 };
add(8)          g76<1>D         g75<1,1,0>D     g46<1,1,0>D     { align1 1Q I@1 compacted };
add3(8)         g29<1>D         g76<8,8,1>D     g49<8,8,1>D     g88<1,1,1>D { align1 1Q I@1 };

LABEL4:
else(8)         JIP:  LABEL0          UIP:  LABEL0              { align1 1Q };

LABEL1:
mov(8)          g29<1>UD        0x00000000UD                    { align1 1Q I@2 };

LABEL0:
endif(8)        JIP:  LABEL5                                    { align1 1Q };

LABEL5:
add(8)          g77<1>D         g36<1,1,0>D     g29<1,1,0>D     { align1 1Q I@2 compacted };
add(8)          g79<1>D         g7<8,4,2>D      12D             { align1 1Q compacted };
add(8)          g89<1>D         g7<8,4,2>D      156D            { align1 1Q compacted };
add(8)          g98<1>D         g7<8,4,2>D      164D            { align1 1Q compacted };
cmp.l.f0.0(8)   g78<1>UD        g77<1,1,0>UD    g36<1,1,0>UD    { align1 1Q I@4 compacted };
cmp.l.f0.0(8)   g80<1>UD        g79<8,8,1>UD    g7<8,4,2>UD     { align1 1Q I@4 };
mov(8)          g121<2>UD       g79<4,4,1>UD                    { align1 1Q $4.src };
mov(8)          g123<2>UD       g89<4,4,1>UD                    { align1 1Q I@5 };
mov(8)          g125<2>UD       g98<4,4,1>UD                    { align1 1Q I@5 };
add(8)          g81<1>D         -g80<8,8,1>D    g7.1<8,4,2>D    { align1 1Q I@4 };
mov(8)          g121.1<2>UD     g81<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g82UD           g121UD          nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $5 };
add(8)          g83<1>D         g82<1,1,0>D     -3D             { align1 1Q $5.dst compacted };
shl(8)          g84<1>D         g82<8,8,1>D     0x00000006UD    { align1 1Q };
shr(8)          g85<1>UD        g83<1,1,0>UD    0x0000001aUD    { align1 1Q I@2 compacted };
add3(8)         g86<1>D         65344W          g84<8,8,1>D     g77<1,1,1>D { align1 1Q I@2 };
add3(8)         g5<1>D          -g78<8,8,1>D    g38<8,8,1>D     g85<1,1,1>D { align1 1Q I@2 };
cmp.l.f0.0(8)   g87<1>UD        g86<1,1,0>UD    g77<1,1,0>UD    { align1 1Q I@2 compacted };
cmp.l.f0.0(8)   g90<1>UD        g89<1,1,0>UD    0x0000009cUD    { align1 1Q compacted };
add(8)          g91<1>D         -g90<8,8,1>D    g7.1<8,4,2>D    { align1 1Q I@1 };
mov(8)          g123.1<2>UD     g91<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g92UD           g123UD          nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $6 };
shl(8)          g93<1>D         g92<8,8,1>D     0x00000006UD    { align1 1Q $6.dst };
shr(8)          g94<1>UD        g92<1,1,0>UD    0x0000001aUD    { align1 1Q compacted };
add(8)          g95<1>D         g86<1,1,0>D     g93<1,1,0>D     { align1 1Q I@2 compacted };
add3(8)         g97<1>D         -g87<8,8,1>D    g5<8,8,1>D      g94<1,1,1>D { align1 1Q I@2 };
cmp.l.f0.0(8)   g96<1>UD        g95<1,1,0>UD    g86<1,1,0>UD    { align1 1Q I@2 compacted };
cmp.l.f0.0(8)   g99<1>UD        g98<1,1,0>UD    0x000000a4UD    { align1 1Q compacted };
add(8)          g100<1>D        -g99<8,8,1>D    g7.1<8,4,2>D    { align1 1Q I@1 };
mov(8)          g125.1<2>UD     g100<4,4,1>UD                   { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g101UD          g125UD          nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $7 };
shl(8)          g102<1>D        g101<8,8,1>D    0x00000003UD    { align1 1Q $7.dst };
shr(8)          g103<1>UD       g101<1,1,0>UD   0x0000001dUD    { align1 1Q compacted };
add(8)          g104<1>D        g102<1,1,0>D    63D             { align1 1Q I@2 compacted };
cmp.l.f0.0(8)   g105<1>UD       g104<1,1,0>UD   g102<1,1,0>UD   { align1 1Q I@1 compacted };
and(8)          g107<1>UD       g104<8,8,1>UD   0xffffffc0UD    { align1 1Q };
add(8)          g106<1>D        -g105<1,1,0>D   g103<1,1,0>D    { align1 1Q I@2 compacted };
add(8)          g108<1>D        g95<1,1,0>D     g107<1,1,0>D    { align1 1Q I@2 compacted };
add3(8)         g9<1>D          -g96<8,8,1>D    g97<8,8,1>D     g106<1,1,1>D { align1 1Q I@2 };
cmp.l.f0.0(8)   g109<1>UD       g108<1,1,0>UD   g95<1,1,0>UD    { align1 1Q I@2 compacted };
add(8)          g111<1>D        g108<1,1,0>D    255D            { align1 1Q compacted };
cmp.l.f0.0(8)   g112<1>UD       g111<1,1,0>UD   0x000000ffUD    { align1 1Q I@1 compacted };
and(8)          g126<1>UD       g111<8,8,1>UD   0xffffffc0UD    { align1 1Q $7.src };
add3(8)         g127<1>D        -g109<8,8,1>D   g9<8,8,1>D      -g112<1,1,1>D { align1 1Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(8)         nullUD          g3UD            g126UD          0x04003586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1Q $8 };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q $8.src };
send(8)         nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1Q A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_postbuild_info_compacted_size_code[] = {
    0x80000065, 0x7f058220, 0x02000004, 0xffffffc0,
    0xe2581940, 0x00017f03, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa00580c, 0x00340000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x21070061, 0x001102cc,
    0x00030061, 0x03260aa0, 0x00000264, 0x00000000,
    0x21071261, 0x00110204, 0x21031261, 0x00110244,
    0xa16e1240, 0x020e0703, 0xa1140040, 0x010e0703,
    0xf0231a70, 0x02006e03, 0x00030061, 0x71060220,
    0x00346e05, 0x00000000, 0xf0151b70, 0x01001403,
    0x00030061, 0x73060220, 0x00341405, 0x00000000,
    0x00031c40, 0x0b052660, 0x06462305, 0x00440726,
    0x00031b40, 0x16052660, 0x06461505, 0x00440726,
    0x00031a61, 0x71260220, 0x00340b05, 0x00000000,
    0x00031a61, 0x73260220, 0x00341605, 0x00000000,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00034131, 0x0c240000, 0xfb047114, 0x003c0000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034231, 0x17240000, 0xfb047314, 0x003c0000,
    0xa1102140, 0x0c200d02, 0xa11b2240, 0x19201a02,
    0xa1212240, 0x17201802, 0xe1111b68, 0x00101003,
    0xe1130068, 0x01a01003, 0x00031c69, 0x1c058660,
    0x02461b05, 0x00000006, 0xe1060068, 0x01a01b03,
    0x00031d69, 0x22058660, 0x02462105, 0x00000006,
    0xe10a0068, 0x01a02103, 0x00031e69, 0x12058660,
    0x02461105, 0x00000007, 0xa11e1940, 0x1c001202,
    0x301f1970, 0x12001e03, 0xa1241d40, 0x22001e02,
    0x00031a52, 0x20040e68, 0x0e2e1305, 0x1f050605,
    0x30251a70, 0x1e002403, 0x00032170, 0x00010220,
    0x52460e05, 0x00460f05, 0x00031a52, 0x26040e68,
    0x0e2e2005, 0x25050a05, 0x01030022, 0x0001c060,
    0x00000360, 0x00000350, 0xa1270040, 0x040e0703,
    0xa1320040, 0x058e0703, 0x00031a70, 0x28050220,
    0x52462705, 0x00440706, 0x00033261, 0x74060220,
    0x00342705, 0x00000000, 0x00031b70, 0x33050220,
    0x52463205, 0x00440706, 0x00030061, 0x76060220,
    0x00343205, 0x00000000, 0x00031c40, 0x29052660,
    0x06462805, 0x00440726, 0x00031b40, 0x34052660,
    0x06463305, 0x00440726, 0x00031a61, 0x74260220,
    0x00342905, 0x00000000, 0x00031a61, 0x76260220,
    0x00343405, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x00034231, 0x2a140000,
    0xfb047414, 0x000c0000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00034331, 0x350c0000,
    0xfb047614, 0x00040000, 0x612c2241, 0x00c02a02,
    0x00032269, 0x2f058660, 0x02462b05, 0x00000003,
    0x00032370, 0x00010220, 0x52460e05, 0x00463505,
    0xa12d1b40, 0x03f02c03, 0xa1301b40, 0x03f02f03,
    0x00031a65, 0x2e058220, 0x02462d05, 0xffffffc0,
    0x00031a65, 0x31058220, 0x02463005, 0xffffffc0,
    0x01030022, 0x0001c060, 0x00000150, 0x00000120,
    0xa1360040, 0x0ff02103, 0xa1380040, 0x050e0703,
    0x00030069, 0x42058660, 0x02462105, 0x00000005,
    0x00031b65, 0x37058220, 0x02463605, 0xffffff00,
    0x00031b70, 0x39050220, 0x52463805, 0x00440706,
    0x00033361, 0x77060220, 0x00343805, 0x00000000,
    0xa1431c40, 0x03f04203, 0x00031b40, 0x3a052660,
    0x06463905, 0x00440726, 0x00031a65, 0x44058220,
    0x02464305, 0xffffffc0, 0x00031a61, 0x77260220,
    0x00343a05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00034331, 0x3b0c0000,
    0xfb047714, 0x00040000, 0xa13c2340, 0x0ff03b03,
    0x00031965, 0x3d058220, 0x02463c05, 0xffffff00,
    0xa13e1940, 0x3d003702, 0x613f1941, 0x00c03e02,
    0xa1401940, 0x03f03f03, 0x00031965, 0x41058220,
    0x02464005, 0xffffffc0, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0xa1581940, 0x44004102,
    0x00030024, 0x0001c060, 0x00000040, 0x00000040,
    0x80003001, 0x00000000, 0x00000000, 0x00000000,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x58054220, 0x00000000, 0x00000000,
    0x00030025, 0x00004600, 0x00000000, 0x000000b8,
    0xa1450040, 0x00ce0703, 0x00031970, 0x46050220,
    0x52464505, 0x00440706, 0x00033361, 0x78060220,
    0x00344505, 0x00000000, 0x00031a40, 0x47052660,
    0x06464605, 0x00440726, 0x00031961, 0x78260220,
    0x00344705, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00034431, 0x480c0000,
    0xfb047814, 0x00040000, 0x00032469, 0x49058660,
    0x02464805, 0x00000002, 0xa14a1940, 0x03304903,
    0x00031965, 0x4b058220, 0x02464a05, 0xffffffc0,
    0xa14c1940, 0x2e004b02, 0x00031952, 0x1d040e68,
    0x0e0e4c05, 0x58053105, 0x00030024, 0x0001c060,
    0x00000020, 0x00000020, 0x00031a61, 0x1d054220,
    0x00000000, 0x00000000, 0x00030025, 0x00004600,
    0x00000000, 0x00000010, 0xa14d1a40, 0x1d002402,
    0xa14f0040, 0x00ce0703, 0xa1590040, 0x09ce0703,
    0xa1620040, 0x0a4e0703, 0x304e1c70, 0x24004d03,
    0x00031c70, 0x50050220, 0x52464f05, 0x00440706,
    0x00033461, 0x79060220, 0x00344f05, 0x00000000,
    0x00031d61, 0x7b060220, 0x00345905, 0x00000000,
    0x00031d61, 0x7d060220, 0x00346205, 0x00000000,
    0x00031c40, 0x51052660, 0x06465005, 0x00440726,
    0x00031961, 0x79260220, 0x00345105, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034531, 0x520c0000, 0xfb047914, 0x00040000,
    0xa1532540, 0xffd05203, 0x00030069, 0x54058660,
    0x02465205, 0x00000006, 0xe1551a68, 0x01a05303,
    0x00031a52, 0x56044560, 0x0e0eff40, 0x4d055405,
    0x00031a52, 0x05042e68, 0x0e0e4e05, 0x55052605,
    0x30571a70, 0x4d005603, 0xf05a0070, 0x09c05903,
    0x00031940, 0x5b052660, 0x06465a05, 0x00440726,
    0x00031961, 0x7b260220, 0x00345b05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034631, 0x5c0c0000, 0xfb047b14, 0x00040000,
    0x00032669, 0x5d058660, 0x02465c05, 0x00000006,
    0xe15e0068, 0x01a05c03, 0xa15f1a40, 0x5d005602,
    0x00031a52, 0x61042e68, 0x0e0e5705, 0x5e050505,
    0x30601a70, 0x56005f03, 0xf0630070, 0x0a406203,
    0x00031940, 0x64052660, 0x06466305, 0x00440726,
    0x00031961, 0x7d260220, 0x00346405, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034731, 0x650c0000, 0xfb047d14, 0x00040000,
    0x00032769, 0x66058660, 0x02466505, 0x00000003,
    0xe1670068, 0x01d06503, 0xa1681a40, 0x03f06603,
    0x30691970, 0x66006803, 0x00030065, 0x6b058220,
    0x02466805, 0xffffffc0, 0xa16a1a40, 0x67026902,
    0xa16c1a40, 0x6b005f02, 0x00031a52, 0x09042e68,
    0x0e0e6005, 0x6a056105, 0x306d1a70, 0x5f006c03,
    0xa16f0040, 0x0ff06c03, 0xf0701970, 0x0ff06f03,
    0x00033765, 0x7e058220, 0x02466f05, 0xffffffc0,
    0x00031a52, 0x7f042e68, 0x0e2e6d05, 0x70050905,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00034831, 0x00000000, 0xfb0c0314, 0x000c7e14,
    0x80033861, 0x7e050220, 0x00460005, 0x00000000,
    0x80030931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_postbuild_info_compacted_size = {
   .prog_data = {
      .base.nr_params = 12,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 2016,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_postbuild_info_compacted_size_relocs,
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
   .args = gfx125_bvh_postbuild_info_compacted_size_args,
   .code = gfx125_bvh_postbuild_info_compacted_size_code,
};
const char *gfx125_bvh_postbuild_info_compacted_size_sha1 = "8aa3e51a3c3b5f734b0fae1cf92fb299485fb2f6";
