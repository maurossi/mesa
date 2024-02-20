#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_misc_copy_geo_meta_data_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_misc_copy_geo_meta_data_args[] = {
   { 0, 8 },
   { 8, 8 },
   { 16, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g123<1>UD       g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
mov(16)         g103<1>UD       g0.1<0,1,0>UD                   { align1 1H };
add(1)          g124<1>UD       g123<0,1,0>UD   0x00000000UD    { align1 WE_all 1N I@2 compacted };
mov(8)          g20<1>UW        0x76543210V                     { align1 WE_all 1Q };
mov(16)         g18<1>UD        g103<8,8,1>UD                   { align1 1H I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(1)         g1UD            g124UD          nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
add(8)          g20.8<1>UW      g20<1,1,0>UW    0x0008UW        { align1 WE_all 1Q I@2 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
mov(16)         g31<1>D         g20<8,8,1>UW                    { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g41.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g23.1<2>F       g2.1<0,1,0>F                    { align1 2Q compacted };
mov(8)          g21.1<2>F       g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g25.1<2>F       g2.3<0,1,0>F                    { align1 2Q };
mov(8)          g114.1<2>F      g2.5<0,1,0>F                    { align1 1Q };
mov(8)          g27.1<2>F       g2.5<0,1,0>F                    { align1 2Q };
mov(8)          g41<2>F         g2<0,1,0>F                      { align1 1Q F@6 compacted };
mov(8)          g23<2>F         g2<0,1,0>F                      { align1 2Q F@6 compacted };
mov(8)          g21<2>F         g2.2<0,1,0>F                    { align1 1Q F@6 compacted };
mov(8)          g25<2>F         g2.2<0,1,0>F                    { align1 2Q F@6 compacted };
mov(8)          g114<2>F        g2.4<0,1,0>F                    { align1 1Q F@6 compacted };
mov(8)          g27<2>F         g2.4<0,1,0>F                    { align1 2Q F@6 compacted };
shr(8)          g29<1>UD        g114<8,4,2>UD   0x00000006UD    { align1 1Q F@2 compacted };
and(8)          g33<1>UD        g114<8,4,2>UD   0xffffffc0UD    { align1 1Q };
mov(8)          g37<1>UD        g114.1<8,4,2>UD                 { align1 1Q };
shr(8)          g30<1>UD        g27<8,4,2>UD    0x00000006UD    { align1 2Q F@1 compacted };
and(8)          g34<1>UD        g27<8,4,2>UD    0xffffffc0UD    { align1 2Q };
mov(8)          g38<1>UD        g27.1<8,4,2>UD                  { align1 2Q };
add(8)          g39<1>D         g114<8,4,2>D    -g33<1,1,0>D    { align1 1Q I@5 compacted };
add(16)         g53<1>D         g103<1,1,0>D    -g29<1,1,0>D    { align1 1H I@4 compacted };
cmp.g.f0.0(16)  g35<1>UD        g33<1,1,0>UD    0x00000000UD    { align1 1H I@4 compacted };
add(8)          g40<1>D         g27<8,4,2>D     -g34<1,1,0>D    { align1 2Q compacted };
shr(16)         g51<1>UD        g33<1,1,0>UD    0x00000006UD    { align1 1H compacted };
cmp.l.f0.0(8)   g16<1>UD        g39<8,8,1>UD    g114<8,4,2>UD   { align1 1Q I@5 };
cmp.l.f0.0(8)   g17<1>UD        g40<8,8,1>UD    g27<8,4,2>UD    { align1 2Q I@3 };
shr(16)         g45<1>UD        g39<1,1,0>UD    0x00000003UD    { align1 1H compacted };
add3(16)        g43<1>D         g37<8,8,1>D     g35<8,8,1>D     -g16<1,1,1>D { align1 1H I@2 };
shl(16)         g47<1>D         g43<8,8,1>D     0x0000001dUD    { align1 1H I@1 };
or(16)          g49<1>UD        g45<1,1,0>UD    g47<1,1,0>UD    { align1 1H I@1 compacted };
cmp.l.f0.0(16)  g55<1>UD        g31<1,1,0>UD    g49<1,1,0>UD    { align1 1H I@1 compacted };
cmp.z.f0.0(16)  g57<1>D         -g53<1,1,0>D    g51<1,1,0>D     { align1 1H I@7 compacted };
and.nz.f0.0(16) null<1>UD       g55<8,8,1>UD    g57<8,8,1>UD    { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL0          UIP:  LABEL0              { align1 1H };
shr(16)         g58<1>UD        g33<1,1,0>UD    0x00000003UD    { align1 1H compacted };
mov(8)          g66<1>UD        g21.1<8,4,2>UD                  { align1 1Q F@4 };
mov(8)          g67<1>UD        g25.1<8,4,2>UD                  { align1 2Q F@3 };
add(16)         g60<1>D         g31<1,1,0>D     g58<1,1,0>D     { align1 1H I@3 compacted };
mul(8)          acc0<1>UD       g60<8,8,1>UD    0x0038UW        { align1 1Q I@1 };
mul(16)         g64<1>D         g60<1,1,0>D     56W             { align1 1H I@2 compacted };
mach(8)         g62<1>UD        g60<1,1,0>UD    0x00000038UD    { align1 1Q compacted AccWrEnable };
add(8)          g68<1>D         g21<8,4,2>D     g64<1,1,0>D     { align1 1Q I@2 compacted };
add(8)          g69<1>D         g25<8,4,2>D     g65<1,1,0>D     { align1 2Q I@3 compacted };
mul(8)          acc0<1>UD       g61<8,8,1>UD    0x0038UW        { align1 2Q };
cmp.l.f0.0(8)   g70<1>UD        g68<8,8,1>UD    g21<8,4,2>UD    { align1 1Q I@3 };
cmp.l.f0.0(8)   g71<1>UD        g69<8,8,1>UD    g25<8,4,2>UD    { align1 2Q I@3 };
add(16)         g73<1>D         g68<1,1,0>D     48D             { align1 1H compacted };
mach(8)         g63<1>UD        g61<8,8,1>UD    0x00000038UD    { align1 2Q AccWrEnable };
cmp.l.f0.0(16)  g75<1>UD        g73<1,1,0>UD    g68<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g104<2>UD       g73<4,4,1>UD                    { align1 1Q };
mov(8)          g106<2>UD       g74<4,4,1>UD                    { align1 2Q };
add3(16)        g72<1>D         g66<8,8,1>D     g62<8,8,1>D     -g70<1,1,1>D { align1 1H I@4 };
add(16)         g77<1>D         -g75<1,1,0>D    g72<1,1,0>D     { align1 1H I@1 compacted };
mov(8)          g104.1<2>UD     g77<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g106.1<2>UD     g78<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g78UD           g104UD          nullUD          0x08200980                0x00000000
                            ugm MsgDesc: ( load, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
mov(16)         g80<1>UD        g78<32,8,4>UB                   { align1 1H $1.dst };
cmp.z.f0.0(16)  null<1>D        g80<8,8,1>D     1D              { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL2          UIP:  LABEL1              { align1 1H };
add(16)         g82<1>D         g68<1,1,0>D     16D             { align1 1H compacted };
cmp.l.f0.0(16)  g84<1>UD        g82<1,1,0>UD    g68<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g105<2>UD       g82<4,4,1>UD                    { align1 1Q $1.src };
mov(8)          g107<2>UD       g83<4,4,1>UD                    { align1 2Q $1.src };
add(16)         g86<1>D         -g84<1,1,0>D    g72<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g105.1<2>UD     g86<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g107.1<2>UD     g87<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g9UD            g105UD          nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
else(16)        JIP:  LABEL1          UIP:  LABEL1              { align1 1H };

LABEL2:
add(16)         g87<1>D         g68<1,1,0>D     40D             { align1 1H compacted };
cmp.l.f0.0(16)  g89<1>UD        g87<1,1,0>UD    g68<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g106<2>UD       g87<4,4,1>UD                    { align1 1Q $1.src };
mov(8)          g108<2>UD       g88<4,4,1>UD                    { align1 2Q $1.src };
add(16)         g91<1>D         -g89<1,1,0>D    g72<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g106.1<2>UD     g91<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g108.1<2>UD     g92<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g92UD           g106UD          nullUD          0x08200980                0x00000000
                            ugm MsgDesc: ( load, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
mov.nz.f0.0(16) null<1>UD       g92<32,8,4>UB                   { align1 1H $1.dst };
(+f0.0) if(16)  JIP:  LABEL4          UIP:  LABEL3              { align1 1H };
add(16)         g93<1>D         g68<1,1,0>D     32D             { align1 1H compacted };
cmp.l.f0.0(16)  g95<1>UD        g93<1,1,0>UD    g68<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g107<2>UD       g93<4,4,1>UD                    { align1 1Q $1.src };
mov(8)          g109<2>UD       g94<4,4,1>UD                    { align1 2Q $1.src };
add(16)         g97<1>D         -g95<1,1,0>D    g72<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g107.1<2>UD     g97<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g109.1<2>UD     g98<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g98UD           g107UD          nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
mul(8)          acc0<1>UD       g98<8,8,1>UD    0xaaabUW        { align1 1Q $1.dst };
mach(8)         g100<1>UD       g98<8,8,1>UD    0xaaaaaaabUD    { align1 1Q AccWrEnable };
mul(8)          acc0<1>UD       g99<8,8,1>UD    0xaaabUW        { align1 2Q $1.dst };
mach(8)         g101<1>UD       g99<8,8,1>UD    0xaaaaaaabUD    { align1 2Q AccWrEnable };
shr(16)         g9<1>UD         g100<1,1,0>UD   0x00000001UD    { align1 1H @1 $1.dst compacted };
else(16)        JIP:  LABEL3          UIP:  LABEL3              { align1 1H };

LABEL4:
add(16)         g101<1>D        g68<1,1,0>D     36D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
cmp.l.f0.0(16)  g103<1>UD       g101<1,1,0>UD   g68<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g108<2>UD       g101<4,4,1>UD                   { align1 1Q $1.src };
mov(8)          g110<2>UD       g102<4,4,1>UD                   { align1 2Q $1.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
add(16)         g105<1>D        -g103<1,1,0>D   g72<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g108.1<2>UD     g105<4,4,1>UD                   { align1 1Q I@1 };
mov(8)          g110.1<2>UD     g106<4,4,1>UD                   { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g106UD          g108UD          nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $1 };
mul(8)          acc0<1>UD       g106<8,8,1>UD   0xaaabUW        { align1 1Q $1.dst };
mach(8)         g108<1>UD       g106<8,8,1>UD   0xaaaaaaabUD    { align1 1Q $1.src AccWrEnable };
mul(8)          acc0<1>UD       g107<8,8,1>UD   0xaaabUW        { align1 2Q $1.dst };
mach(8)         g109<1>UD       g107<8,8,1>UD   0xaaaaaaabUD    { align1 2Q $1.src AccWrEnable };
shr(16)         g9<1>UD         g108<1,1,0>UD   0x00000001UD    { align1 1H @1 $1.dst compacted };

LABEL3:
endif(16)       JIP:  LABEL1                                    { align1 1H };

LABEL1:
endif(16)       JIP:  LABEL0                                    { align1 1H };
add(16)         g109<1>D        g68<1,1,0>D     49D             { align1 1H $1.src compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
shl(16)         g119<1>D        g60<8,8,1>D     0x00000003UD    { align1 1H };
shr(16)         g121<1>UD       g60<1,1,0>UD    0x0000001dUD    { align1 1H compacted };
mov(8)          g123<1>UD       g41.1<8,4,2>UD                  { align1 1Q F@6 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $0.src };
mov(8)          g124<1>UD       g23.1<8,4,2>UD                  { align1 2Q F@5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.src };
cmp.l.f0.0(16)  g111<1>UD       g109<1,1,0>UD   g68<1,1,0>UD    { align1 1H I@5 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g1<2>UD         g109<4,4,1>UD                   { align1 1Q F@1 };
mov(8)          g3<2>UD         g110<4,4,1>UD                   { align1 2Q };
add(8)          g115<1>D        g41<8,4,2>D     g119<1,1,0>D    { align1 1Q I@7 compacted };
add(8)          g125<1>D        g23<8,4,2>D     g120<1,1,0>D    { align1 2Q I@7 compacted };
add(16)         g113<1>D        -g111<1,1,0>D   g72<1,1,0>D     { align1 1H I@5 compacted };
cmp.l.f0.0(8)   g126<1>UD       g115<8,8,1>UD   g41<8,4,2>UD    { align1 1Q I@3 };
mov(8)          g5<2>UD         g115<4,4,1>UD                   { align1 1Q };
cmp.l.f0.0(8)   g127<1>UD       g125<8,8,1>UD   g23<8,4,2>UD    { align1 2Q I@4 };
mov(8)          g7<2>UD         g125<4,4,1>UD                   { align1 2Q };
mov(8)          g1.1<2>UD       g113<4,4,1>UD                   { align1 1Q I@5 };
mov(8)          g3.1<2>UD       g114<4,4,1>UD                   { align1 2Q I@6 };
add3(16)        g13<1>D         g123<8,8,1>D    g121<8,8,1>D    -g126<1,1,1>D { align1 1H I@4 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g114UD          g1UD            nullUD          0x08200980                0x00000000
                            ugm MsgDesc: ( load, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $0 };
mov(8)          g5.1<2>UD       g13<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g7.1<2>UD       g14<4,4,1>UD                    { align1 2Q I@2 };
mov(16)         g116<1>UD       g114<32,8,4>UB                  { align1 1H $0.dst };
shl(16)         g118<1>D        g116<8,8,1>D    0x00000010UD    { align1 1H I@1 };
or(16)          g11<1>UD        g80<1,1,0>UD    g118<1,1,0>UD   { align1 1H I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $1.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        nullUD          g5UD            g9UD            0x08003586                0x00000100
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 4, src1_len = 4 flat )  base_offset 0  { align1 1H $2 };

LABEL0:
endif(16)       JIP:  LABEL5                                    { align1 1H };

LABEL5:
shr(16)         g27<1>UD        g31<1,1,0>UD    0x00000001UD    { align1 1H compacted };
mov(8)          g33<1>UD        g21.1<8,4,2>UD                  { align1 1Q F@4 };
mov(8)          g34<1>UD        g25.1<8,4,2>UD                  { align1 2Q F@3 };

LABEL14:
cmp.ge.f0.0(16) null<1>UD       g18<8,8,1>UD    g51<8,8,1>UD    { align1 1H I@2 };
(+f0.0) break(16) JIP:  LABEL6        UIP:  LABEL6              { align1 1H };
shl(16)         g35<1>D         g18<8,8,1>D     0x00000006UD    { align1 1H };
shr(16)         g37<1>UD        g35<1,1,0>UD    0x00000003UD    { align1 1H I@1 compacted };
add(16)         g39<1>D         g37<1,1,0>D     g27<1,1,0>D     { align1 1H I@1 compacted };
mul(8)          acc0<1>UD       g39<8,8,1>UD    0x0038UW        { align1 1Q I@1 };
mul(16)         g43<1>D         g39<1,1,0>D     56W             { align1 1H I@2 compacted };
mach(8)         g16<1>UD        g39<1,1,0>UD    0x00000038UD    { align1 1Q compacted AccWrEnable };
add(8)          g45<1>D         g21<8,4,2>D     g43<1,1,0>D     { align1 1Q I@2 compacted };
add(8)          g46<1>D         g25<8,4,2>D     g44<1,1,0>D     { align1 2Q I@3 compacted };
mul(8)          acc0<1>UD       g40<8,8,1>UD    0x0038UW        { align1 2Q };
cmp.l.f0.0(8)   g47<1>UD        g45<8,8,1>UD    g21<8,4,2>UD    { align1 1Q I@3 };
cmp.l.f0.0(8)   g48<1>UD        g46<8,8,1>UD    g25<8,4,2>UD    { align1 2Q I@3 };
add(16)         g53<1>D         g45<1,1,0>D     48D             { align1 1H compacted };
mach(8)         g17<1>UD        g40<8,8,1>UD    0x00000038UD    { align1 2Q AccWrEnable };
cmp.l.f0.0(16)  g55<1>UD        g53<1,1,0>UD    g45<1,1,0>UD    { align1 1H I@2 compacted };
mov(8)          g10<2>UD        g53<4,4,1>UD                    { align1 1Q $2.src };
mov(8)          g12<2>UD        g54<4,4,1>UD                    { align1 2Q $2.src };
and.z.f0.0(16)  null<1>UD       g31<8,8,1>UD    0x00000001UD    { align1 1H };
add3(16)        g49<1>D         g33<8,8,1>D     g16<8,8,1>D     -g47<1,1,1>D { align1 1H I@5 };
add(16)         g57<1>D         -g55<1,1,0>D    g49<1,1,0>D     { align1 1H I@1 compacted };
mov(8)          g10.1<2>UD      g57<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g12.1<2>UD      g58<4,4,1>UD                    { align1 2Q I@2 };
(+f0.0) if(16)  JIP:  LABEL8          UIP:  LABEL7              { align1 1H };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(16)        g58UD           g10UD           nullUD          0x08200980                0x00000000
                            ugm MsgDesc: ( load, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
mov(16)         g60<1>UD        g58<32,8,4>UB                   { align1 1H $2.dst };
cmp.z.f0.0(16)  null<1>D        g60<8,8,1>D     1D              { align1 1H I@1 };
(+f0.0) if(16)  JIP:  LABEL10         UIP:  LABEL9              { align1 1H };
add(16)         g61<1>D         g45<1,1,0>D     16D             { align1 1H compacted };
cmp.l.f0.0(16)  g63<1>UD        g61<1,1,0>UD    g45<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g109<2>UD       g61<4,4,1>UD                    { align1 1Q $3.src };
mov(8)          g111<2>UD       g62<4,4,1>UD                    { align1 2Q $3.src };
add(16)         g65<1>D         -g63<1,1,0>D    g49<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g109.1<2>UD     g65<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g111.1<2>UD     g66<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g14UD           g109UD          nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
else(16)        JIP:  LABEL9          UIP:  LABEL9              { align1 1H };

LABEL10:
add(16)         g66<1>D         g45<1,1,0>D     40D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
cmp.l.f0.0(16)  g68<1>UD        g66<1,1,0>UD    g45<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g110<2>UD       g66<4,4,1>UD                    { align1 1Q $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $3.src };
mov(8)          g112<2>UD       g67<4,4,1>UD                    { align1 2Q };
add(16)         g70<1>D         -g68<1,1,0>D    g49<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g110.1<2>UD     g70<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g112.1<2>UD     g71<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g71UD           g110UD          nullUD          0x08200980                0x00000000
                            ugm MsgDesc: ( load, a64, d8u32, V1, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
mov.nz.f0.0(16) null<1>UD       g71<32,8,4>UB                   { align1 1H $3.dst };
(+f0.0) if(16)  JIP:  LABEL12         UIP:  LABEL11             { align1 1H };
add(16)         g72<1>D         g45<1,1,0>D     32D             { align1 1H compacted };
cmp.l.f0.0(16)  g74<1>UD        g72<1,1,0>UD    g45<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g111<2>UD       g72<4,4,1>UD                    { align1 1Q $3.src };
mov(8)          g113<2>UD       g73<4,4,1>UD                    { align1 2Q $3.src };
add(16)         g76<1>D         -g74<1,1,0>D    g49<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g111.1<2>UD     g76<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g113.1<2>UD     g77<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g77UD           g111UD          nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
mul(8)          acc0<1>UD       g77<8,8,1>UD    0xaaabUW        { align1 1Q $3.dst };
mach(8)         g79<1>UD        g77<8,8,1>UD    0xaaaaaaabUD    { align1 1Q AccWrEnable };
mul(8)          acc0<1>UD       g78<8,8,1>UD    0xaaabUW        { align1 2Q $3.dst };
mach(8)         g80<1>UD        g78<8,8,1>UD    0xaaaaaaabUD    { align1 2Q AccWrEnable };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.dst };
shr(16)         g14<1>UD        g79<1,1,0>UD    0x00000001UD    { align1 1H I@1 compacted };
else(16)        JIP:  LABEL11         UIP:  LABEL11             { align1 1H };

LABEL12:
add(16)         g80<1>D         g45<1,1,0>D     36D             { align1 1H compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
cmp.l.f0.0(16)  g82<1>UD        g80<1,1,0>UD    g45<1,1,0>UD    { align1 1H I@1 compacted };
mov(8)          g112<2>UD       g80<4,4,1>UD                    { align1 1Q $3.src };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 3N $3.src };
mov(8)          g114<2>UD       g81<4,4,1>UD                    { align1 2Q };
add(16)         g84<1>D         -g82<1,1,0>D    g49<1,1,0>D     { align1 1H I@3 compacted };
mov(8)          g112.1<2>UD     g84<4,4,1>UD                    { align1 1Q I@1 };
mov(8)          g114.1<2>UD     g85<4,4,1>UD                    { align1 2Q I@2 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(16)        g85UD           g112UD          nullUD          0x08201582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $3 };
mul(8)          acc0<1>UD       g85<8,8,1>UD    0xaaabUW        { align1 1Q $3.dst };
mach(8)         g87<1>UD        g85<8,8,1>UD    0xaaaaaaabUD    { align1 1Q AccWrEnable };
mul(8)          acc0<1>UD       g86<8,8,1>UD    0xaaabUW        { align1 2Q $3.dst };
mach(8)         g88<1>UD        g86<8,8,1>UD    0xaaaaaaabUD    { align1 2Q AccWrEnable };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
shr(16)         g14<1>UD        g87<1,1,0>UD    0x00000001UD    { align1 1H compacted };

LABEL11:
endif(16)       JIP:  LABEL9                                    { align1 1H };

LABEL9:
endif(16)       JIP:  LABEL13                                   { align1 1H };

LABEL13:
else(16)        JIP:  LABEL7          UIP:  LABEL7              { align1 1H };

LABEL8:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(16)        g89UD           g10UD           nullUD          0x08200b80                0x00000000
                            ugm MsgDesc: ( load, a64, d16u32, V1, L1STATE_L3MOCS dst_len = 2, src0_len = 4, src1_len = 0 flat )  base_offset 0  { align1 1H $2 };
mov(16)         g121<2>UB       g89<16,8,2>UW                   { align1 1H $2.dst };
mov(16)         g119<2>HF       g89<16,8,2>HF                   { align1 1H };
mov(16)         g90<1>UD        g121<16,8,2>UB                  { align1 1H A@1 };
mov(16)         g88<1>UW        g119<16,8,2>UW                  { align1 1H F@1 };
mov(16)         g92<1>UD        g88.1<16,8,2>UB                 { align1 1H I@1 };
shl(16)         g94<1>D         g92<8,8,1>D     0x00000010UD    { align1 1H I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.dst };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
or(16)          g14<1>UD        g90<1,1,0>UD    g94<1,1,0>UD    { align1 1H compacted };

LABEL7:
endif(16)       JIP:  LABEL6                                    { align1 1H };
add(8)          g116<1>D        g41<8,4,2>D     g35<1,1,0>D     { align1 1Q F@6 compacted };
add(8)          g95<1>D         g23<8,4,2>D     g36<1,1,0>D     { align1 2Q F@5 compacted };
cmp.l.f0.0(8)   g117<1>UD       g116<8,8,1>UD   g41<8,4,2>UD    { align1 1Q I@2 };
cmp.l.f0.0(8)   g96<1>UD        g95<8,8,1>UD    g23<8,4,2>UD    { align1 2Q I@2 };
mov(8)          g98<2>UD        g116<4,4,1>UD                   { align1 1Q };
mov(8)          g100<2>UD       g95<4,4,1>UD                    { align1 2Q };
add(8)          g118<1>D        -g117<8,8,1>D   g41.1<8,4,2>D   { align1 1Q I@4 };
add(8)          g97<1>D         -g96<8,8,1>D    g23.1<8,4,2>D   { align1 2Q I@4 };
mov(8)          g98.1<2>UD      g118<4,4,1>UD                   { align1 1Q I@2 };
mov(8)          g100.1<2>UD     g97<4,4,1>UD                    { align1 2Q I@2 };
fbl(1)          g102<1>UD       mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.src };
shl(1)          a0<1>UD         g102<0,1,0>UD   0x00000003UD    { align1 WE_all 1N I@2 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000c00UD    { align1 WE_all 1N A@1 };
mov(1)          g122<2>D        g[a0 64]<0,1,0>D                { align1 WE_all 1N A@1 };
mov(1)          g122.1<2>D      g[a0 68]<0,1,0>D                { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.src };
mov(8)          g113.1<2>UD     g122.1<0,1,0>UD                 { align1 WE_all 1Q I@1 };
mov(8)          g113<2>UD       g122<0,1,0>UD                   { align1 WE_all 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any16h) send(1) nullUD   g113UD          g14UD           0x0200d584                0x00000080
                            ugm MsgDesc: ( store, a64, d32, V16, transpose, L1STATE_L3MOCS dst_len = 0, src0_len = 1, src1_len = 2 flat )  base_offset 0  { align1 WE_all 1N $3 };
add3(16)        g18<1>D         0x0001UW        g29<8,8,1>D     g18<1,1,1>D { align1 1H };

LABEL6:
while(16)       JIP:  LABEL14                                   { align1 1H };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(16)        nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1H A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_misc_copy_geo_meta_data_code[] = {
    0x80000065, 0x7b058220, 0x02000004, 0xffffffc0,
    0x00040061, 0x67050220, 0x00000024, 0x00000000,
    0xe27c1a40, 0x00017b03, 0x80030061, 0x14054410,
    0x00000000, 0x76543210, 0x00041b61, 0x12050220,
    0x00466705, 0x00000000, 0x80001b01, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa007c0c, 0x00340000, 0x64141a40, 0x00801495,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00040061, 0x1f050160, 0x00461405, 0x00000000,
    0x80002001, 0x00000000, 0x00000000, 0x00000000,
    0x21290061, 0x001102cc, 0x2a170061, 0x001102cc,
    0x00030061, 0x15260aa0, 0x00000264, 0x00000000,
    0x00130061, 0x19260aa0, 0x00000264, 0x00000000,
    0x00030061, 0x72260aa0, 0x000002a4, 0x00000000,
    0x00130061, 0x1b260aa0, 0x000002a4, 0x00000000,
    0x21291661, 0x00110204, 0x2a171661, 0x00110204,
    0x21151661, 0x00110244, 0x2a191661, 0x00110244,
    0x21721661, 0x0011025c, 0x2a1b1661, 0x0011025c,
    0xe11d1268, 0x006e7203, 0x00030065, 0x21058220,
    0x02447206, 0xffffffc0, 0x00030061, 0x25050220,
    0x00447226, 0x00000000, 0xea1e1168, 0x006e1b03,
    0x00130065, 0x22058220, 0x02441b06, 0xffffffc0,
    0x00130061, 0x26050220, 0x00441b26, 0x00000000,
    0xa1271d40, 0x212e7202, 0xa0351c40, 0x1d206702,
    0xeb231c70, 0x00002103, 0xaa280040, 0x222e1b02,
    0xe0330068, 0x00602103, 0x00031d70, 0x10050220,
    0x52462705, 0x00447206, 0x00131b70, 0x11050220,
    0x52462805, 0x00441b06, 0xe02d0068, 0x00302703,
    0x00041a52, 0x2b040e68, 0x0e2e2505, 0x10052305,
    0x00041969, 0x2f058660, 0x02462b05, 0x0000001d,
    0x20311966, 0x2f002d03, 0x27371970, 0x31001f03,
    0xac391f70, 0x33023502, 0x00041965, 0x00010220,
    0x22463705, 0x00463905, 0x01040022, 0x0001c060,
    0x00000620, 0x00000620, 0xe03a0068, 0x00302103,
    0x00031461, 0x42050220, 0x00441526, 0x00000000,
    0x00131361, 0x43050220, 0x00441926, 0x00000000,
    0xa03c1b40, 0x3a001f02, 0x00031941, 0x20018220,
    0x01463c05, 0x00380038, 0x60401a41, 0x03803c02,
    0xfe3e0049, 0x03803c03, 0xa1441a40, 0x400e1502,
    0xaa451b40, 0x410e1902, 0x00130041, 0x20018220,
    0x01463d05, 0x00380038, 0x00031b70, 0x46050220,
    0x52464405, 0x00441506, 0x00131b70, 0x47050220,
    0x52464505, 0x00441906, 0xa0490040, 0x03004403,
    0x00130049, 0x3f058222, 0x02463d05, 0x00000038,
    0x274b1a70, 0x44004903, 0x00030061, 0x68060220,
    0x00344905, 0x00000000, 0x00130061, 0x6a060220,
    0x00344a05, 0x00000000, 0x00041c52, 0x48040e68,
    0x0e2e4205, 0x46053e05, 0xa04d1940, 0x48024b02,
    0x00031961, 0x68260220, 0x00344d05, 0x00000000,
    0x00131a61, 0x6a260220, 0x00344e05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x4e140000, 0xf3006824, 0x00020000,
    0x00042161, 0x50050020, 0x00664e07, 0x00000000,
    0x00041970, 0x00018660, 0x16465005, 0x00000001,
    0x01040022, 0x0001c060, 0x000002f0, 0x00000098,
    0xa0520040, 0x01004403, 0x27541970, 0x44005203,
    0x00033161, 0x69060220, 0x00345205, 0x00000000,
    0x00133161, 0x6b060220, 0x00345305, 0x00000000,
    0xa0561b40, 0x48025402, 0x00031961, 0x69260220,
    0x00345605, 0x00000000, 0x00131a61, 0x6b260220,
    0x00345705, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044131, 0x09140000,
    0xfb046924, 0x00040000, 0x00040024, 0x0001c060,
    0x00000268, 0x00000268, 0xa0570040, 0x02804403,
    0x27591970, 0x44005703, 0x00033161, 0x6a060220,
    0x00345705, 0x00000000, 0x00133161, 0x6c060220,
    0x00345805, 0x00000000, 0xa05b1b40, 0x48025902,
    0x00031961, 0x6a260220, 0x00345b05, 0x00000000,
    0x00131a61, 0x6c260220, 0x00345c05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044131, 0x5c140000, 0xf3006a24, 0x00020000,
    0x00042161, 0x00010020, 0x20665c07, 0x00000000,
    0x01040022, 0x0001c060, 0x000001c0, 0x000000e0,
    0xa05d0040, 0x02004403, 0x275f1970, 0x44005d03,
    0x00033161, 0x6b060220, 0x00345d05, 0x00000000,
    0x00133161, 0x6d060220, 0x00345e05, 0x00000000,
    0xa0611b40, 0x48025f02, 0x00031961, 0x6b260220,
    0x00346105, 0x00000000, 0x00131a61, 0x6d260220,
    0x00346205, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044131, 0x62140000,
    0xfb046b24, 0x00040000, 0x00032141, 0x20018220,
    0x01466205, 0xaaabaaab, 0x00030049, 0x64058222,
    0x02466205, 0xaaaaaaab, 0x00132141, 0x20018220,
    0x01466305, 0xaaabaaab, 0x00130049, 0x65058222,
    0x02466305, 0xaaaaaaab, 0xe0099168, 0x00106403,
    0x00040024, 0x0001c060, 0x000000f0, 0x000000f0,
    0xa0650040, 0x02404403, 0x80003101, 0x00000000,
    0x00000000, 0x00000000, 0x27671970, 0x44006503,
    0x00033161, 0x6c060220, 0x00346505, 0x00000000,
    0x00133161, 0x6e060220, 0x00346605, 0x00000000,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0xa0691b40, 0x48026702, 0x00031961, 0x6c260220,
    0x00346905, 0x00000000, 0x00131a61, 0x6e260220,
    0x00346a05, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044131, 0x6a140000,
    0xfb046c24, 0x00040000, 0x00032141, 0x20018220,
    0x01466a05, 0xaaabaaab, 0x00033149, 0x6c058222,
    0x02466a05, 0xaaaaaaab, 0x00132141, 0x20018220,
    0x01466b05, 0xaaabaaab, 0x00133149, 0x6d058222,
    0x02466b05, 0xaaaaaaab, 0xe0099168, 0x00106c03,
    0x00040025, 0x00004600, 0x00000000, 0x00000010,
    0x00040025, 0x00004600, 0x00000000, 0x000001d8,
    0xa06d3140, 0x03104403, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x00040069, 0x77058660,
    0x02463c05, 0x00000003, 0xe0790068, 0x01d03c03,
    0x00031661, 0x7b050220, 0x00442926, 0x00000000,
    0x80103001, 0x00000000, 0x00000000, 0x00000000,
    0x00131561, 0x7c050220, 0x00441726, 0x00000000,
    0x80003101, 0x00000000, 0x00000000, 0x00000000,
    0x276f1d70, 0x44006d03, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x00031161, 0x01060220,
    0x00346d05, 0x00000000, 0x00130061, 0x03060220,
    0x00346e05, 0x00000000, 0xa1731f40, 0x770e2902,
    0xaa7d1f40, 0x780e1702, 0xa0711d40, 0x48026f02,
    0x00031b70, 0x7e050220, 0x52467305, 0x00442906,
    0x00030061, 0x05060220, 0x00347305, 0x00000000,
    0x00131c70, 0x7f050220, 0x52467d05, 0x00441706,
    0x00130061, 0x07060220, 0x00347d05, 0x00000000,
    0x00031d61, 0x01260220, 0x00347105, 0x00000000,
    0x00131e61, 0x03260220, 0x00347205, 0x00000000,
    0x00041c52, 0x0d040e68, 0x0e2e7b05, 0x7e057905,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00044031, 0x72140000, 0xf3000124, 0x00020000,
    0x00031961, 0x05260220, 0x00340d05, 0x00000000,
    0x00131a61, 0x07260220, 0x00340e05, 0x00000000,
    0x00042061, 0x74050020, 0x00667207, 0x00000000,
    0x00041969, 0x76058660, 0x02467405, 0x00000010,
    0x200b1966, 0x76005003, 0x80002101, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x00000000,
    0xfb0c0524, 0x000c0924, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0xe01b0068, 0x00101f03,
    0x00031461, 0x21050220, 0x00441526, 0x00000000,
    0x00131361, 0x22050220, 0x00441926, 0x00000000,
    0x00041a70, 0x00010220, 0x42461205, 0x00463305,
    0x01040028, 0x0001c660, 0x000006e8, 0x000006e8,
    0x00040069, 0x23058660, 0x02461205, 0x00000006,
    0xe0251968, 0x00302303, 0xa0271940, 0x1b002502,
    0x00031941, 0x20018220, 0x01462705, 0x00380038,
    0x602b1a41, 0x03802702, 0xfe100049, 0x03802703,
    0xa12d1a40, 0x2b0e1502, 0xaa2e1b40, 0x2c0e1902,
    0x00130041, 0x20018220, 0x01462805, 0x00380038,
    0x00031b70, 0x2f050220, 0x52462d05, 0x00441506,
    0x00131b70, 0x30050220, 0x52462e05, 0x00441906,
    0xa0350040, 0x03002d03, 0x00130049, 0x11058222,
    0x02462805, 0x00000038, 0x27371a70, 0x2d003503,
    0x00033261, 0x0a060220, 0x00343505, 0x00000000,
    0x00133261, 0x0c060220, 0x00343605, 0x00000000,
    0x00040065, 0x00018220, 0x12461f05, 0x00000001,
    0x00041d52, 0x31040e68, 0x0e2e2105, 0x2f051005,
    0xa0391940, 0x31023702, 0x00031961, 0x0a260220,
    0x00343905, 0x00000000, 0x00131a61, 0x0c260220,
    0x00343a05, 0x00000000, 0x01040022, 0x0001c060,
    0x00000468, 0x000003c0, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x3a140000,
    0xf3000a24, 0x00020000, 0x00042261, 0x3c050020,
    0x00663a07, 0x00000000, 0x00041970, 0x00018660,
    0x16463c05, 0x00000001, 0x01040022, 0x0001c060,
    0x00000350, 0x000000a8, 0xa03d0040, 0x01002d03,
    0x273f1970, 0x2d003d03, 0x00033361, 0x6d060220,
    0x00343d05, 0x00000000, 0x00133361, 0x6f060220,
    0x00343e05, 0x00000000, 0xa0411b40, 0x31023f02,
    0x00031961, 0x6d260220, 0x00344105, 0x00000000,
    0x00131a61, 0x6f260220, 0x00344205, 0x00000000,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x0e140000, 0xfb046d24, 0x00040000,
    0x00040024, 0x0001c060, 0x000002b8, 0x000002b8,
    0xa0420040, 0x02802d03, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x27441970, 0x2d004203,
    0x00033361, 0x6e060220, 0x00344205, 0x00000000,
    0x80103301, 0x00000000, 0x00000000, 0x00000000,
    0x00130061, 0x70060220, 0x00344305, 0x00000000,
    0xa0461b40, 0x31024402, 0x00031961, 0x6e260220,
    0x00344605, 0x00000000, 0x00131a61, 0x70260220,
    0x00344705, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x00044331, 0x47140000,
    0xf3006e24, 0x00020000, 0x00042361, 0x00010020,
    0x20664707, 0x00000000, 0x01040022, 0x0001c060,
    0x000001f0, 0x000000f0, 0xa0480040, 0x02002d03,
    0x274a1970, 0x2d004803, 0x00033361, 0x6f060220,
    0x00344805, 0x00000000, 0x00133361, 0x71060220,
    0x00344905, 0x00000000, 0xa04c1b40, 0x31024a02,
    0x00031961, 0x6f260220, 0x00344c05, 0x00000000,
    0x00131a61, 0x71260220, 0x00344d05, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x4d140000, 0xfb046f24, 0x00040000,
    0x00032341, 0x20018220, 0x01464d05, 0xaaabaaab,
    0x00030049, 0x4f058222, 0x02464d05, 0xaaaaaaab,
    0x00132341, 0x20018220, 0x01464e05, 0xaaabaaab,
    0x00130049, 0x50058222, 0x02464e05, 0xaaaaaaab,
    0x80002301, 0x00000000, 0x00000000, 0x00000000,
    0xe00e1968, 0x00104f03, 0x00040024, 0x0001c060,
    0x00000110, 0x00000110, 0xa0500040, 0x02402d03,
    0x80003301, 0x00000000, 0x00000000, 0x00000000,
    0x27521970, 0x2d005003, 0x00033361, 0x70060220,
    0x00345005, 0x00000000, 0x80103301, 0x00000000,
    0x00000000, 0x00000000, 0x00130061, 0x72060220,
    0x00345105, 0x00000000, 0xa0541b40, 0x31025202,
    0x00031961, 0x70260220, 0x00345405, 0x00000000,
    0x00131a61, 0x72260220, 0x00345505, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00044331, 0x55140000, 0xfb047024, 0x00040000,
    0x00032341, 0x20018220, 0x01465505, 0xaaabaaab,
    0x00030049, 0x57058222, 0x02465505, 0xaaaaaaab,
    0x00132341, 0x20018220, 0x01465605, 0xaaabaaab,
    0x00130049, 0x58058222, 0x02465605, 0xaaaaaaab,
    0x80002301, 0x00000000, 0x00000000, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0xe00e0068, 0x00105703, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x00040025, 0x00004600,
    0x00000000, 0x00000010, 0x00040024, 0x0001c060,
    0x000000b8, 0x000000b8, 0x80000901, 0x00000000,
    0x00000000, 0x00000000, 0x00044231, 0x59140000,
    0xf7000a24, 0x00020000, 0x00042261, 0x79060100,
    0x00565906, 0x00000000, 0x00040061, 0x77060990,
    0x00565906, 0x00000000, 0x00040961, 0x5a050020,
    0x00567906, 0x00000000, 0x00041161, 0x58050110,
    0x00567706, 0x00000000, 0x00041961, 0x5c050020,
    0x0056580e, 0x00000000, 0x00041969, 0x5e058660,
    0x02465c05, 0x00000010, 0x80002301, 0x00000000,
    0x00000000, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x200e0066, 0x5e005a03,
    0x00040025, 0x00004600, 0x00000000, 0x00000168,
    0xa1741640, 0x230e2902, 0xaa5f1540, 0x240e1702,
    0x00031a70, 0x75050220, 0x52467405, 0x00442906,
    0x00131a70, 0x60050220, 0x52465f05, 0x00441706,
    0x00030061, 0x62060220, 0x00347405, 0x00000000,
    0x00130061, 0x64060220, 0x00345f05, 0x00000000,
    0x00031c40, 0x76052660, 0x06467505, 0x00442926,
    0x00131c40, 0x61052660, 0x06466005, 0x00441726,
    0x00031a61, 0x62260220, 0x00347605, 0x00000000,
    0x00131a61, 0x64260220, 0x00346105, 0x00000000,
    0xe266004c, 0x00114004, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80003001, 0x00000000,
    0x00000000, 0x00000000, 0x80001a69, 0x10018220,
    0x02006604, 0x00000003, 0x80000940, 0x10018220,
    0x02001000, 0x00000c00, 0x80000961, 0x7a060660,
    0x00010100, 0x00000000, 0x80000061, 0x7a260660,
    0x00010110, 0x00000000, 0x80003301, 0x00000000,
    0x00000000, 0x00000000, 0x80031961, 0x71260220,
    0x00007a24, 0x00000000, 0x80031961, 0x71060220,
    0x00007a04, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x8a004331, 0x00000000,
    0xfb08710c, 0x00340e14, 0x00040052, 0x12044160,
    0x0e0e0001, 0x12051d05, 0x00040027, 0x00014060,
    0x00000000, 0xfffff908, 0x80030061, 0x7e050220,
    0x00460005, 0x00000000, 0x80040931, 0x00000004,
    0x30007e0c, 0x00000000, 0x20000060, 0x00000000,
};
static const struct brw_kernel gfx125_misc_copy_geo_meta_data = {
   .prog_data = {
      .base.nr_params = 14,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 3984,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_misc_copy_geo_meta_data_relocs,
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
   .args = gfx125_misc_copy_geo_meta_data_args,
   .code = gfx125_misc_copy_geo_meta_data_code,
};
const char *gfx125_misc_copy_geo_meta_data_sha1 = "02fbe18e0f4da9c919d2cedae6f1958af2435d43";
