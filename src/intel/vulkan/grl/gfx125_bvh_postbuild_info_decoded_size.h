#include "intel/compiler/brw_kernel.h"

static const struct brw_shader_reloc gfx125_bvh_postbuild_info_decoded_size_relocs[] = {
};
static const struct brw_kernel_arg_desc gfx125_bvh_postbuild_info_decoded_size_args[] = {
   { 0, 8 },
   { 8, 8 },
};

#if 0  /* BEGIN KERNEL ASSEMBLY */

and(1)          g107<1>UD       g0<0,1,0>UD     0xffffffc0UD    { align1 WE_all 1N };
add(1)          g108<1>UD       g107<0,1,0>UD   0x00000000UD    { align1 WE_all 1N I@1 compacted };
mov(8)          g34<1>UW        0x76543210V                     { align1 WE_all 1Q };
mov(8)          g20.1<2>F       0x0F             /* 0F */       { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(1)         g1UD            g108UD          nullUD          0x0220d500                0x00000000
                            ugm MsgDesc: ( load, a32, d32, V16, transpose, L1STATE_L3MOCS dst_len = 2, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $0 };
mov(8)          g20<2>F         0x0F             /* 0F */       { align1 1Q F@1 };
mov(8)          g12.1<2>UD      g20.1<8,4,2>UD                  { align1 1Q F@1 };
mov(8)          g14.1<2>UD      g20.1<8,4,2>UD                  { align1 1Q };
mov(8)          g16.1<2>UD      g20.1<8,4,2>UD                  { align1 1Q };
mov(8)          g12<2>UD        g20<8,4,2>UD                    { align1 1Q I@3 };
mov(8)          g14<2>UD        g20<8,4,2>UD                    { align1 1Q I@3 };
mov(8)          g16<2>UD        g20<8,4,2>UD                    { align1 1Q I@3 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $0.dst };
mov(8)          g18.1<2>F       g2.1<0,1,0>F                    { align1 1Q compacted };
mov(8)          g3.1<2>F        g2.3<0,1,0>F                    { align1 1Q };
mov(8)          g18<2>F         g2<0,1,0>F                      { align1 1Q F@2 compacted };
mov(8)          g3<2>F          g2.2<0,1,0>F                    { align1 1Q F@2 compacted };
add(8)          g21<1>D         g18<8,4,2>D     160D            { align1 1Q F@2 compacted };
cmp.l.f0.0(8)   g22<1>UD        g21<1,1,0>UD    0x000000a0UD    { align1 1Q I@1 compacted };
mov(8)          g96<2>UD        g21<4,4,1>UD                    { align1 1Q };
add(8)          g23<1>D         -g22<8,8,1>D    g18.1<8,4,2>D   { align1 1Q I@2 };
mov(8)          g96.1<2>UD      g23<4,4,1>UD                    { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
send(8)         g24UD           g96UD           nullUD          0x04203582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 2, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $1 };
add(8)          g26<1>D         g18<8,4,2>D     g24<1,1,0>D     { align1 1Q $1.dst compacted };
cmp.l.f0.0(8)   g27<1>UD        g26<8,8,1>UD    g18<8,4,2>UD    { align1 1Q I@1 };
add(8)          g28<1>D         -g27<8,8,1>D    g18.1<8,4,2>D   { align1 1Q I@1 };

LABEL16:
cmp.z.f0.0(8)   g29<1>D         g12.1<8,4,2>D   0D              { align1 1Q I@2 compacted };
cmp.l.f0.0(8)   g30<1>UD        g12<8,4,2>UD    g25<1,1,0>UD    { align1 1Q $1.dst compacted };
and.nz.f0.0(8)  null<1>UD       g29<8,8,1>UD    g30<8,8,1>UD    { align1 1Q I@1 };
(-f0.0) break(8) JIP:  LABEL0         UIP:  LABEL0              { align1 1Q };
shl(8)          g31<1>D         g12<8,4,2>D     0x00000003UD    { align1 1Q };
shl(8)          g32<1>D         g12.1<8,4,2>D   0x00000003UD    { align1 1Q };
shr(8)          g33<1>UD        g12<8,4,2>UD    0x0000001dUD    { align1 1Q compacted };
add(8)          g35<1>D         g26<1,1,0>D     g31<1,1,0>D     { align1 1Q I@3 compacted };
or(8)           g5<1>UD         g32<1,1,0>UD    g33<1,1,0>UD    { align1 1Q I@2 compacted };
cmp.l.f0.0(8)   g36<1>UD        g35<1,1,0>UD    g26<1,1,0>UD    { align1 1Q I@2 compacted };
mov(8)          g102<2>UD       g35<4,4,1>UD                    { align1 1Q $2.src };
add(8)          g98<1>D         g35<1,1,0>D     4D              { align1 1Q $4.src compacted };
add3(8)         g37<1>D         g28<8,8,1>D     g5<8,8,1>D      -g36<1,1,1>D { align1 1Q I@3 };
cmp.l.f0.0(8)   g38<1>UD        g98<1,1,0>UD    g35<1,1,0>UD    { align1 1Q I@2 compacted };
mov(8)          g99<2>UD        g98<4,4,1>UD                    { align1 1Q $3.src };
mov(8)          g102.1<2>UD     g37<4,4,1>UD                    { align1 1Q I@3 };
add(8)          g39<1>D         -g38<1,1,0>D    g37<1,1,0>D     { align1 1Q I@3 compacted };
mov(8)          g99.1<2>UD      g39<4,4,1>UD                    { align1 1Q I@1 };
shr(8)          g40<1>UD        g39<1,1,0>UD    0x0000001eUD    { align1 1Q compacted };
cmp.z.f0.0(8)   null<1>D        g40<8,8,1>D     2D              { align1 1Q I@1 };
(+f0.0) if(8)   JIP:  LABEL2          UIP:  LABEL1              { align1 1Q };
and(1)          g41<1>UD        g0.5<0,1,0>UD   0xfffffc00UD    { align1 WE_all 1N };
and(8)          g42<1>UD        g98<8,8,1>D     0xfffffffcUD    { align1 1Q };
shl(8)          g43<1>UD        g34<8,8,1>UW    0x00000002UD    { align1 1Q };
and(8)          g97<1>UD        g98<8,8,1>D     0x00000003UD    { align1 1Q $1.src };
shl(8)          g42<1>UD        g42<1,1,0>UD    0x00000003UD    { align1 1Q I@3 compacted };
or(8)           g97<1>UD        g97<1,1,0>UD    g42<1,1,0>UD    { align1 1Q I@1 compacted };
or(8)           g97<1>UD        g97<1,1,0>UD    g43<1,1,0>UD    { align1 1Q I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
or(1)           a0.1<1>UD       g41<0,1,0>UD    0x0000000fUD    { align1 WE_all 1N };
send(8)         g44UD           g97UD           nullUD          0x42100b00                a0.1<0>UD
                            ugm MsgDesc: ( load, a32, d16u32, V1, L1STATE_L3MOCS dst_len = 1, src0_len = 1, ss )  surface_state_index 0  { align1 1Q @1 $1 };
mov(8)          g105<2>UW       g44<8,8,1>UD                    { align1 1Q $1.dst };
mov(8)          g85<1>UW        g105<16,8,2>UW                  { align1 1Q I@1 };
else(8)         JIP:  LABEL1          UIP:  LABEL1              { align1 1Q };

LABEL2:
cmp.z.f0.0(8)   null<1>D        g40<8,8,1>D     1D              { align1 1Q };
(+f0.0) if(8)   JIP:  LABEL4          UIP:  LABEL3              { align1 1Q };
send(8)         g45UD           g98UD           nullUD          0x02100b00                0x00000000
                            slm MsgDesc: ( load, a32, d16u32, V1, L1STATE_L3MOCS dst_len = 1, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 1Q $4 };
mov(8)          g77<1>UW        g45<16,8,2>UW                   { align1 1Q $4.dst };
else(8)         JIP:  LABEL3          UIP:  LABEL3              { align1 1Q };

LABEL4:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(8)         g46UD           g99UD           nullUD          0x04100b80                0x00000000
                            ugm MsgDesc: ( load, a64, d16u32, V1, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $3 };
mov(8)          g106<2>HF       g46<16,8,2>HF                   { align1 1Q $3.dst };
mov(8)          g77<1>UW        g106<16,8,2>UW                  { align1 1Q A@1 };

LABEL3:
endif(8)        JIP:  LABEL1                                    { align1 1Q };
mov(8)          g85<1>UW        g77<8,8,1>UW                    { align1 1Q I@2 };

LABEL1:
endif(8)        JIP:  LABEL0                                    { align1 1Q };
shr(8)          g47<1>UD        g37<1,1,0>UD    0x0000001eUD    { align1 1Q compacted };
cmp.nz.f0.0(8)  g48<1>W         g85<8,8,1>W     0W              { align1 1Q I@3 };
mov.nz.f0.0(8)  null<1>D        g48<8,8,1>W                     { align1 1Q I@1 };
(+f0.0) if(8)   JIP:  LABEL6          UIP:  LABEL5              { align1 1Q };
cmp.z.f0.0(8)   null<1>D        g47<8,8,1>D     2D              { align1 1Q I@4 };
(+f0.0) if(8)   JIP:  LABEL8          UIP:  LABEL7              { align1 1Q };
and(1)          g49<1>UD        g0.5<0,1,0>UD   0xfffffc00UD    { align1 WE_all 1N };
and(8)          g50<1>UD        g35<8,8,1>D     0xfffffffcUD    { align1 1Q };
shl(8)          g51<1>UD        g34<8,8,1>UW    0x00000002UD    { align1 1Q };
and(8)          g100<1>UD       g35<8,8,1>D     0x00000003UD    { align1 1Q $3.src };
shl(8)          g50<1>UD        g50<1,1,0>UD    0x00000003UD    { align1 1Q I@3 compacted };
or(8)           g100<1>UD       g100<1,1,0>UD   g50<1,1,0>UD    { align1 1Q I@1 compacted };
or(8)           g100<1>UD       g100<1,1,0>UD   g51<1,1,0>UD    { align1 1Q I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
or(1)           a0.1<1>UD       g49<0,1,0>UD    0x0000000fUD    { align1 WE_all 1N };
send(8)         g9UD            g100UD          nullUD          0x42101502                a0.1<0>UD
                            ugm MsgDesc: ( load_cmask, a32, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 1, ss )  surface_state_index 0  { align1 1Q @1 $3 };
else(8)         JIP:  LABEL7          UIP:  LABEL7              { align1 1Q };

LABEL8:
cmp.z.f0.0(8)   null<1>D        g47<8,8,1>D     1D              { align1 1Q };
(+f0.0) if(8)   JIP:  LABEL10         UIP:  LABEL9              { align1 1Q };
fbl(1)          g53<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.src };
shl(1)          a0<1>UD         g53<0,1,0>UD    0x00000002UD    { align1 WE_all 1N I@2 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000400UD    { align1 WE_all 1N A@1 };
mov(1)          g54<1>UD        g[a0 96]<0,1,0>UD               { align1 WE_all 1N A@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any8h) send(1) g52UD     g54UD           nullUD          0x0210c500                0x00000000
                            slm MsgDesc: ( load, a32, d32, V8, transpose, L1STATE_L3MOCS dst_len = 1, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $5 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $5.dst };
mov(8)          g8<1>UD         g52<0,1,0>UD                    { align1 1Q };
else(8)         JIP:  LABEL9          UIP:  LABEL9              { align1 1Q };

LABEL10:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(8)         g8UD            g102UD          nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $2 };

LABEL9:
endif(8)        JIP:  LABEL7                                    { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $3.dst };
mov(8)          g9<1>UD         g8<8,8,1>UD                     { align1 1Q @3 $2.dst };

LABEL7:
endif(8)        JIP:  LABEL11                                   { align1 1Q };
add(8)          g55<1>D         g16<8,4,2>D     g9<1,1,0>D      { align1 1Q @2 $3.dst compacted };
cmp.l.f0.0(8)   g56<1>UD        g55<8,8,1>UD    g16<8,4,2>UD    { align1 1Q I@1 };
add(8)          g57<1>D         -g56<8,8,1>D    g16.1<8,4,2>D   { align1 1Q I@1 };
mov(8)          g16<2>UD        g55<4,4,1>UD                    { align1 1Q };
mov(8)          g16.1<2>UD      g57<4,4,1>UD                    { align1 1Q I@1 };

LABEL11:
else(8)         JIP:  LABEL5          UIP:  LABEL5              { align1 1Q };

LABEL6:
cmp.z.f0.0(8)   null<1>D        g47<8,8,1>D     2D              { align1 1Q I@4 };
(+f0.0) if(8)   JIP:  LABEL13         UIP:  LABEL12             { align1 1Q };
and(1)          g58<1>UD        g0.5<0,1,0>UD   0xfffffc00UD    { align1 WE_all 1N };
and(8)          g59<1>UD        g35<8,8,1>D     0xfffffffcUD    { align1 1Q };
shl(8)          g60<1>UD        g34<8,8,1>UW    0x00000002UD    { align1 1Q };
and(8)          g101<1>UD       g35<8,8,1>D     0x00000003UD    { align1 1Q $6.src };
shl(8)          g59<1>UD        g59<1,1,0>UD    0x00000003UD    { align1 1Q I@3 compacted };
or(8)           g101<1>UD       g101<1,1,0>UD   g59<1,1,0>UD    { align1 1Q I@1 compacted };
or(8)           g101<1>UD       g101<1,1,0>UD   g60<1,1,0>UD    { align1 1Q I@1 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
or(1)           a0.1<1>UD       g58<0,1,0>UD    0x0000000fUD    { align1 WE_all 1N };
send(8)         g11UD           g101UD          nullUD          0x42101502                a0.1<0>UD
                            ugm MsgDesc: ( load_cmask, a32, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 1, ss )  surface_state_index 0  { align1 1Q @1 $6 };
else(8)         JIP:  LABEL12         UIP:  LABEL12             { align1 1Q };

LABEL13:
cmp.z.f0.0(8)   null<1>D        g47<8,8,1>D     1D              { align1 1Q };
(+f0.0) if(8)   JIP:  LABEL15         UIP:  LABEL14             { align1 1Q };
fbl(1)          g62<1>UD        mask0<0,1,0>UD                  { align1 WE_all 1N compacted };
mov(1)          f0<1>UD         mask0<0,1,0>UD                  { align1 WE_all 1N };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.src };
shl(1)          a0<1>UD         g62<0,1,0>UD    0x00000002UD    { align1 WE_all 1N I@2 };
add(1)          a0<1>UD         a0<0,1,0>UD     0x00000400UD    { align1 WE_all 1N A@1 };
mov(1)          g63<1>UD        g[a0 96]<0,1,0>UD               { align1 WE_all 1N A@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@1 };
(+f0.0.any8h) send(1) g61UD     g63UD           nullUD          0x0210c500                0x00000000
                            slm MsgDesc: ( load, a32, d32, V8, transpose, L1STATE_L3MOCS dst_len = 1, src0_len = 1, src1_len = 0 flat )  base_offset 0  { align1 WE_all 1N $7 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $7.dst };
mov(8)          g10<1>UD        g61<0,1,0>UD                    { align1 1Q };
else(8)         JIP:  LABEL14         UIP:  LABEL14             { align1 1Q };

LABEL15:
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@2 };
send(8)         g10UD           g102UD          nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $2 };

LABEL14:
endif(8)        JIP:  LABEL12                                   { align1 1Q };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N $6.dst };
mov(8)          g11<1>UD        g10<8,8,1>UD                    { align1 1Q @3 $2.dst };

LABEL12:
endif(8)        JIP:  LABEL5                                    { align1 1Q };
add(8)          g64<1>D         g14<8,4,2>D     g11<1,1,0>D     { align1 1Q @2 $6.dst compacted };
cmp.l.f0.0(8)   g65<1>UD        g64<8,8,1>UD    g14<8,4,2>UD    { align1 1Q I@1 };
add(8)          g66<1>D         -g65<8,8,1>D    g14.1<8,4,2>D   { align1 1Q I@1 };
mov(8)          g14<2>UD        g64<4,4,1>UD                    { align1 1Q };
mov(8)          g14.1<2>UD      g66<4,4,1>UD                    { align1 1Q I@1 };

LABEL5:
endif(8)        JIP:  LABEL0                                    { align1 1Q };
add(8)          g67<1>D         g12<8,4,2>D     1D              { align1 1Q compacted };
cmp.l.f0.0(8)   g68<1>UD        g67<8,8,1>UD    g12<8,4,2>UD    { align1 1Q I@1 };
add(8)          g69<1>D         -g68<8,8,1>D    g12.1<8,4,2>D   { align1 1Q I@1 };
mov(8)          g12<2>UD        g67<4,4,1>UD                    { align1 1Q };
mov(8)          g12.1<2>UD      g69<4,4,1>UD                    { align1 1Q I@1 };

LABEL0:
while(8)        JIP:  LABEL16                                   { align1 1Q };
add(8)          g70<1>D         g18<8,4,2>D     156D            { align1 1Q compacted };
mul(8)          acc0<1>UD       g25<8,8,1>UD    0x0038UW        { align1 1Q $1.dst };
mul(8)          g75<1>D         g25<1,1,0>D     56W             { align1 1Q compacted };
mul(8)          g79<1>D         g14<8,4,2>D     36W             { align1 1Q compacted };
mul(8)          g80<1>D         g14.1<8,4,2>D   36W             { align1 1Q compacted };
mul(8)          g83<1>D         g16<8,4,2>D     24W             { align1 1Q compacted };
mul(8)          g84<1>D         g16.1<8,4,2>D   24W             { align1 1Q compacted };
cmp.l.f0.0(8)   g71<1>UD        g70<1,1,0>UD    0x0000009cUD    { align1 1Q I@7 compacted };
mov(8)          g103<2>UD       g70<4,4,1>UD                    { align1 1Q $2.src };
mach(8)         g74<1>UD        g25<1,1,0>UD    0x00000038UD    { align1 1Q compacted AccWrEnable };
add(8)          g89<1>D         g75<1,1,0>D     8D              { align1 1Q I@7 compacted };
add(8)          g86<1>D         g79<1,1,0>D     g83<1,1,0>D     { align1 1Q I@6 compacted };
add(8)          g72<1>D         -g71<8,8,1>D    g18.1<8,4,2>D   { align1 1Q I@5 };
mul(8)          acc0<1>UD       g14<8,4,2>UD    0x0024UW        { align1 1Q };
cmp.l.f0.0(8)   g87<1>UD        g86<1,1,0>UD    g79<1,1,0>UD    { align1 1Q I@3 compacted };
cmp.l.f0.0(8)   g90<1>UD        g89<1,1,0>UD    g75<1,1,0>UD    { align1 1Q I@5 compacted };
mov(8)          g103.1<2>UD     g72<4,4,1>UD                    { align1 1Q I@4 };
mach(8)         g78<1>UD        g14<8,4,2>UD    0x00000024UD    { align1 1Q compacted AccWrEnable };
add(8)          g91<1>D         -g90<1,1,0>D    g74<1,1,0>D     { align1 1Q I@3 compacted };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N I@3 };
send(8)         g73UD           g103UD          nullUD          0x04101582                0x00000000
                            ugm MsgDesc: ( load_cmask, a64, d32, x, L1STATE_L3MOCS dst_len = 1, src0_len = 2, src1_len = 0 flat )  base_offset 0  { align1 1Q $8 };
add(8)          g81<1>D         g78<1,1,0>D     g80<1,1,0>D     { align1 1Q I@2 compacted };
mul(8)          acc0<1>UD       g16<8,4,2>UD    0x0018UW        { align1 1Q };
mach(8)         g82<1>UD        g16<8,4,2>UD    0x00000018UD    { align1 1Q compacted AccWrEnable };
add(8)          g7<1>D          g82<1,1,0>D     g84<1,1,0>D     { align1 1Q I@1 compacted };
add3(8)         g88<1>D         g81<8,8,1>D     g7<8,8,1>D      -g87<1,1,1>D { align1 1Q I@1 };
shl(8)          g76<1>D         g73<8,8,1>D     0x00000006UD    { align1 1Q $8.dst };
shr(8)          g6<1>UD         g73<1,1,0>UD    0x0000001aUD    { align1 1Q compacted };
add(8)          g92<1>D         g89<1,1,0>D     g76<1,1,0>D     { align1 1Q I@2 compacted };
cmp.l.f0.0(8)   g93<1>UD        g92<1,1,0>UD    g89<1,1,0>UD    { align1 1Q I@1 compacted };
add(8)          g104<1>D        g92<1,1,0>D     g86<1,1,0>D     { align1 1Q $8.src compacted };
add3(8)         g94<1>D         g91<8,8,1>D     g6<8,8,1>D      -g93<1,1,1>D { align1 1Q I@2 };
cmp.l.f0.0(8)   g95<1>UD        g104<1,1,0>UD   g92<1,1,0>UD    { align1 1Q I@2 compacted };
add3(8)         g105<1>D        g94<8,8,1>D     g88<8,8,1>D     -g95<1,1,1>D { align1 1Q I@1 };
sync nop(1)                     null<0,1,0>UB                   { align1 WE_all 1N A@1 };
send(8)         nullUD          g3UD            g104UD          0x04003586                0x00000080
                            ugm MsgDesc: ( store_cmask, a64, d32, xy, L1STATE_L3MOCS dst_len = 0, src0_len = 2, src1_len = 2 flat )  base_offset 0  { align1 1Q $9 };
mov(8)          g126<1>UD       g0<8,8,1>UD                     { align1 WE_all 1Q };
send(8)         nullUD          g126UD          nullUD          0x02000000                0x00000000
                            gateway MsgDesc: (open)  mlen 1 ex_mlen 0 rlen 0 { align1 WE_all 1Q A@1 EOT };

#endif /* END KERNEL ASSEMBLY */
static const uint32_t gfx125_bvh_postbuild_info_decoded_size_code[] = {
    0x80000065, 0x6b058220, 0x02000004, 0xffffffc0,
    0xe26c1940, 0x00016b03, 0x80030061, 0x22054410,
    0x00000000, 0x76543210, 0x00030061, 0x14264aa0,
    0x00000000, 0x00000000, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x80004031, 0x01140000,
    0xfa006c0c, 0x00340000, 0x00031161, 0x14064aa0,
    0x00000000, 0x00000000, 0x00031161, 0x0c260220,
    0x00441426, 0x00000000, 0x00030061, 0x0e260220,
    0x00441426, 0x00000000, 0x00030061, 0x10260220,
    0x00441426, 0x00000000, 0x00031b61, 0x0c060220,
    0x00441406, 0x00000000, 0x00031b61, 0x0e060220,
    0x00441406, 0x00000000, 0x00031b61, 0x10060220,
    0x00441406, 0x00000000, 0x80002001, 0x00000000,
    0x00000000, 0x00000000, 0x21120061, 0x001102cc,
    0x00030061, 0x03260aa0, 0x00000264, 0x00000000,
    0x21121261, 0x00110204, 0x21031261, 0x00110244,
    0xa1151240, 0x0a0e1203, 0xf0161970, 0x0a001503,
    0x00030061, 0x60060220, 0x00341505, 0x00000000,
    0x00031a40, 0x17052660, 0x06461605, 0x00441226,
    0x00031961, 0x60260220, 0x00341705, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x00034131, 0x18140000, 0xfb046014, 0x000c0000,
    0xa11a2140, 0x180e1202, 0x00031970, 0x1b050220,
    0x52461a05, 0x00441206, 0x00031940, 0x1c052660,
    0x06461b05, 0x00441226, 0xbd1d1a70, 0x000e0c23,
    0x301e2170, 0x190e0c03, 0x00031965, 0x00010220,
    0x22461d05, 0x00461e05, 0x11030028, 0x0001c660,
    0x00000748, 0x00000748, 0x00030069, 0x1f058660,
    0x02440c06, 0x00000003, 0x00030069, 0x20058660,
    0x02440c26, 0x00000003, 0xe1210068, 0x01de0c03,
    0xa1231b40, 0x1f001a02, 0x21051a66, 0x21002003,
    0x30241a70, 0x1a002303, 0x00033261, 0x66060220,
    0x00342305, 0x00000000, 0xa1623440, 0x00402303,
    0x00031b52, 0x25040e68, 0x0e2e1c05, 0x24050505,
    0x30261a70, 0x23006203, 0x00033361, 0x63060220,
    0x00346205, 0x00000000, 0x00031b61, 0x66260220,
    0x00342505, 0x00000000, 0xa1271b40, 0x25022602,
    0x00031961, 0x63260220, 0x00342705, 0x00000000,
    0xe1280068, 0x01e02703, 0x00031970, 0x00018660,
    0x16462805, 0x00000002, 0x01030022, 0x0001c060,
    0x00000178, 0x000000c8, 0x80000065, 0x29058220,
    0x020000a4, 0xfffffc00, 0x00030065, 0x2a058620,
    0x02466205, 0xfffffffc, 0x00030069, 0x2b058120,
    0x02462205, 0x00000002, 0x00033165, 0x61058620,
    0x02466205, 0x00000003, 0xe12a1b69, 0x00302a03,
    0x21611966, 0x2a006103, 0x21611966, 0x2b006103,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x80000066, 0x10218220, 0x02002904, 0x0000000f,
    0x00039131, 0x2c0e0100, 0xf600610c, 0x04020000,
    0x00032161, 0x69060210, 0x00462c05, 0x00000000,
    0x00031961, 0x55050110, 0x00566906, 0x00000000,
    0x00030024, 0x0001c060, 0x000000c0, 0x000000c0,
    0x00030070, 0x00018660, 0x16462805, 0x00000001,
    0x01030022, 0x0001c060, 0x00000080, 0x00000040,
    0x00034431, 0x2d0c0000, 0xe600620c, 0x00020000,
    0x00032461, 0x4d050110, 0x00562d06, 0x00000000,
    0x00030024, 0x0001c060, 0x00000050, 0x00000050,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00034331, 0x2e0c0000, 0xf7006314, 0x00020000,
    0x00032361, 0x6a060990, 0x00562e06, 0x00000000,
    0x00030961, 0x4d050110, 0x00566a06, 0x00000000,
    0x00030025, 0x00004600, 0x00000000, 0x00000020,
    0x00031a61, 0x55050110, 0x00464d05, 0x00000000,
    0x00030025, 0x00004600, 0x00000000, 0x00000500,
    0xe12f0068, 0x01e02503, 0x00031b70, 0x30058550,
    0x25465505, 0x00000000, 0x00031961, 0x00010560,
    0x20463005, 0x00000000, 0x01030022, 0x0001c060,
    0x00000470, 0x00000248, 0x00031c70, 0x00018660,
    0x16462f05, 0x00000002, 0x01030022, 0x0001c060,
    0x000001c0, 0x000000a8, 0x80000065, 0x31058220,
    0x020000a4, 0xfffffc00, 0x00030065, 0x32058620,
    0x02462305, 0xfffffffc, 0x00030069, 0x33058120,
    0x02462205, 0x00000002, 0x00033365, 0x64058620,
    0x02462305, 0x00000003, 0xe1321b69, 0x00303203,
    0x21641966, 0x32006403, 0x21641966, 0x33006403,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x80000066, 0x10218220, 0x02003104, 0x0000000f,
    0x00039331, 0x090e0100, 0xfa04640c, 0x04040000,
    0x00030024, 0x0001c060, 0x00000128, 0x00000128,
    0x00030070, 0x00018660, 0x16462f05, 0x00000001,
    0x01030022, 0x0001c060, 0x000000d8, 0x000000b8,
    0xe235004c, 0x00114004, 0x80000061, 0x30010220,
    0x00004000, 0x00000000, 0x80003501, 0x00000000,
    0x00000000, 0x00000000, 0x80001a69, 0x10018220,
    0x02003504, 0x00000002, 0x80000940, 0x10018220,
    0x02001000, 0x00000400, 0x80000961, 0x36050220,
    0x00010180, 0x00000000, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x88004531, 0x340c0000,
    0xea00360c, 0x00300000, 0x80002501, 0x00000000,
    0x00000000, 0x00000000, 0x00030061, 0x08050220,
    0x00003404, 0x00000000, 0x00030024, 0x0001c060,
    0x00000030, 0x00000030, 0x80001a01, 0x00000000,
    0x00000000, 0x00000000, 0x00034231, 0x080c0000,
    0xfb046614, 0x00040000, 0x00030025, 0x00004600,
    0x00000000, 0x00000030, 0x80002301, 0x00000000,
    0x00000000, 0x00000000, 0x0003b261, 0x09050220,
    0x00460805, 0x00000000, 0x00030025, 0x00004600,
    0x00000000, 0x00000058, 0xa137a340, 0x090e1002,
    0x00031970, 0x38050220, 0x52463705, 0x00441006,
    0x00031940, 0x39052660, 0x06463805, 0x00441026,
    0x00030061, 0x10060220, 0x00343705, 0x00000000,
    0x00031961, 0x10260220, 0x00343905, 0x00000000,
    0x00030024, 0x0001c060, 0x00000238, 0x00000238,
    0x00031c70, 0x00018660, 0x16462f05, 0x00000002,
    0x01030022, 0x0001c060, 0x000001c0, 0x000000a8,
    0x80000065, 0x3a058220, 0x020000a4, 0xfffffc00,
    0x00030065, 0x3b058620, 0x02462305, 0xfffffffc,
    0x00030069, 0x3c058120, 0x02462205, 0x00000002,
    0x00033665, 0x65058620, 0x02462305, 0x00000003,
    0xe13b1b69, 0x00303b03, 0x21651966, 0x3b006503,
    0x21651966, 0x3c006503, 0x80001901, 0x00000000,
    0x00000000, 0x00000000, 0x80000066, 0x10218220,
    0x02003a04, 0x0000000f, 0x00039631, 0x0b0e0100,
    0xfa04650c, 0x04040000, 0x00030024, 0x0001c060,
    0x00000128, 0x00000128, 0x00030070, 0x00018660,
    0x16462f05, 0x00000001, 0x01030022, 0x0001c060,
    0x000000d8, 0x000000b8, 0xe23e004c, 0x00114004,
    0x80000061, 0x30010220, 0x00004000, 0x00000000,
    0x80003701, 0x00000000, 0x00000000, 0x00000000,
    0x80001a69, 0x10018220, 0x02003e04, 0x00000002,
    0x80000940, 0x10018220, 0x02001000, 0x00000400,
    0x80000961, 0x3f050220, 0x00010180, 0x00000000,
    0x80001901, 0x00000000, 0x00000000, 0x00000000,
    0x88004731, 0x3d0c0000, 0xea003f0c, 0x00300000,
    0x80002701, 0x00000000, 0x00000000, 0x00000000,
    0x00030061, 0x0a050220, 0x00003d04, 0x00000000,
    0x00030024, 0x0001c060, 0x00000030, 0x00000030,
    0x80001a01, 0x00000000, 0x00000000, 0x00000000,
    0x00034231, 0x0a0c0000, 0xfb046614, 0x00040000,
    0x00030025, 0x00004600, 0x00000000, 0x00000030,
    0x80002601, 0x00000000, 0x00000000, 0x00000000,
    0x0003b261, 0x0b050220, 0x00460a05, 0x00000000,
    0x00030025, 0x00004600, 0x00000000, 0x00000058,
    0xa140a640, 0x0b0e0e02, 0x00031970, 0x41050220,
    0x52464005, 0x00440e06, 0x00031940, 0x42052660,
    0x06464105, 0x00440e26, 0x00030061, 0x0e060220,
    0x00344005, 0x00000000, 0x00031961, 0x0e260220,
    0x00344205, 0x00000000, 0x00030025, 0x00004600,
    0x00000000, 0x00000058, 0xa1430040, 0x001e0c03,
    0x00031970, 0x44050220, 0x52464305, 0x00440c06,
    0x00031940, 0x45052660, 0x06464405, 0x00440c26,
    0x00030061, 0x0c060220, 0x00344305, 0x00000000,
    0x00031961, 0x0c260220, 0x00344505, 0x00000000,
    0x00030027, 0x00014060, 0x00000000, 0xfffff898,
    0xa1460040, 0x09ce1203, 0x00032141, 0x20018220,
    0x01461905, 0x00380038, 0x614b0041, 0x03801902,
    0x614f0041, 0x024e0e02, 0x61500041, 0x024e0e22,
    0x61530041, 0x018e1002, 0x61540041, 0x018e1022,
    0xf0471f70, 0x09c04603, 0x00033261, 0x67060220,
    0x00344605, 0x00000000, 0xfe4a0049, 0x03801903,
    0xa1591f40, 0x00804b03, 0xa1561e40, 0x53004f02,
    0x00031d40, 0x48052660, 0x06464705, 0x00441226,
    0x00030041, 0x20018220, 0x01440e06, 0x00240024,
    0x30571b70, 0x4f005603, 0x305a1d70, 0x4b005903,
    0x00031c61, 0x67260220, 0x00344805, 0x00000000,
    0xfe4e0049, 0x024e0e03, 0xa15b1b40, 0x4a025a02,
    0x80001b01, 0x00000000, 0x00000000, 0x00000000,
    0x00034831, 0x490c0000, 0xfb046714, 0x00040000,
    0xa1511a40, 0x50004e02, 0x00030041, 0x20018220,
    0x01441006, 0x00180018, 0xfe520049, 0x018e1003,
    0xa1071940, 0x54005202, 0x00031952, 0x58040e68,
    0x0e2e5105, 0x57050705, 0x00032869, 0x4c058660,
    0x02464905, 0x00000006, 0xe1060068, 0x01a04903,
    0xa15c1a40, 0x4c005902, 0x305d1970, 0x59005c03,
    0xa1683840, 0x56005c02, 0x00031a52, 0x5e040e68,
    0x0e2e5b05, 0x5d050605, 0x305f1a70, 0x5c006803,
    0x00031952, 0x69040e68, 0x0e2e5e05, 0x5f055805,
    0x80000901, 0x00000000, 0x00000000, 0x00000000,
    0x00034931, 0x00000000, 0xfb0c0314, 0x000c6814,
    0x80030061, 0x7e050220, 0x00460005, 0x00000000,
    0x80030931, 0x00000004, 0x30007e0c, 0x00000000,
};
static const struct brw_kernel gfx125_bvh_postbuild_info_decoded_size = {
   .prog_data = {
      .base.nr_params = 12,
      .base.stage = MESA_SHADER_COMPUTE,
      .base.curb_read_length = 2,
      .base.total_scratch = 0,
      .base.total_shared = 0,
      .base.program_size = 2736,
      .base.const_data_size = 0,
      .base.const_data_offset = 0,
      .base.num_relocs = 0,
      .base.relocs = gfx125_bvh_postbuild_info_decoded_size_relocs,
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
   .args = gfx125_bvh_postbuild_info_decoded_size_args,
   .code = gfx125_bvh_postbuild_info_decoded_size_code,
};
const char *gfx125_bvh_postbuild_info_decoded_size_sha1 = "f8bf9550cc78bc75e6ce2445a076b17317aa7c25";
