.intel_syntax noprefix

//void multicorn(float r_start,float r_end,float i_start,float i_end,float res,unsigned char* img)
//r_start:xmm0 r_end:xmm1 i_start:xmm2 i_end:xmm3 res:xmm4 img:rdi
.global multicorn

.data 
iter_count: .int 0xa
abs_magic_number: .byte 0xff,0xff,0xff,0x7f
max_inf: .byte 0x00,0x00,0x80,0x7f
min_inf: .byte 0x00,0x00,0x80,0xff
max_float: .byte 0xff, 0xff, 0x7f, 0x7f
// max float : 0x7f7fffff 
zero: .byte 0x00,0x00,0x00,0x00
// 0x7f800000 +inf
// 0xff800000 -inf
.text

multicorn:
    movss xmm0, [min_inf]

is_inf:
    // idea:
    // UNORDERED: ZF,PF,CF<=111;
    // GREATER_THAN: ZF,PF,CF<=000;
    // LESS_THAN: ZF,PF,CF<=001;
    // EQUAL: ZF,PF,CF<=100;
    // for a normal value x => 0<|x|<float.max, (ZF(100) or CF(001))
    // for a NAN x =>  UNORDERED (111)
    // but for x= +-inf => |x| = +inf, +inf >float.max (000)
    // the input x in xmm0
    movss xmm1,[abs_magic_number]
    andps xmm1,xmm0
    // the |x| now in xmm1
    ucomiss xmm1,[max_float]
    de1:
    // can't use jg(ZF=0 and SF=OF) here because LESS_THAN also goes
    // use the opposite side jbe(CF=1 or ZF=1) instead.
    jbe is_nan
    jmp handle_inf

is_nan:
    // idea:
    // UNORDERED: ZF,PF,CF<=111;
    // GREATER_THAN: ZF,PF,CF<=000;
    // LESS_THAN: ZF,PF,CF<=001;
    // EQUAL: ZF,PF,CF<=100;
    // according to ucomiss doc: 
    // the result will be UNORDERED(above), if the either SRCREG or DSTREG is NAN
    // the input x in xmm0
    ucomiss xmm0,xmm0
    // use PF(parity) flag to handle the UNORDERED
    jp handle_nan
    jmp normal
handle_inf:
// use for debug 0x88
    mov rax, 0x88
    deinf:
    ret
handle_nan:
// use for debug 0x66
    mov rax, 0x66
    denan:
    ret

normal:
    // the value is normal
    ret