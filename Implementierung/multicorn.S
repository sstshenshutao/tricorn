.intel_syntax noprefix

//void multicorn(float r_start,float r_end,float i_start,float i_end,float res,unsigned char* img)
//r_start:xmm0 r_end:xmm1 i_start:xmm2 i_end:xmm3 res:xmm4 img:rdi
.global multicorn

.data 
iter_count: .int 70
abs_magic_number: .byte 0xff,0xff,0xff,0x7f
max_inf: .byte 0x00,0x00,0x80,0x7f
min_inf: .byte 0x00,0x00,0x80,0xff
max_float: .byte 0xff, 0xff, 0x7f, 0x7f
halb: .float 0.5
test_value: .float 1.0
test_value1: .float 1.4999999999999999999999999999999
test_value2: .float 1.5000000000000000000000000000001
test_value3: .float 1.9999999999999999999999999999999
test_value4: .float 1.689999999999999999999
two: .float 2.0
one: .float 1.0
test_minus_1: .float -1.0
// max float : 0x7f7fffff 
zero: .byte 0x00,0x00,0x00,0x00
// 0x7f800000 +inf
// 0xff800000 -inf
a0: .byte 0x00,0x00,0x00,0xc0
// a0: -2.0: 0xc0000000
b0: .byte 0x00,0x00,0x80,0xbf
// b0: -1.0: 0xbf800000
four: .byte 0x00,0x00,0x80,0x40
// 0x40800000
precision: .float 0.01

.text

// for the reason why we use for-loop instead of while-loop: 
// the round-off error of float point will increase, details @see Ausarbeitung
multicorn:
// calculate the number of for-loop for a
movss xmm6,[one]
subss xmm6,[a0]
divss xmm6,xmm4
push rbp
lea rbp, [rip+label_a]
jmp get_number
label_a:
pop rbp
mov rcx,rax
// result for-loop a =>rcx

// calculate the number of for-loop for b
movss xmm6,[one]
subss xmm6,[b0]
divss xmm6,xmm4
push rbp
lea rbp, [rip+label_b]
jmp get_number
label_b:
pop rbp
// result for-loop b =>rax
//test finished!
jmp iteration_method

// idea:
// the input value is X, we use Y=X+0.5, 
// we know that Y = xxx.5 may cause problem, so:
// floor(Y)-Y ?.= 0.5  if yes: cvtss2si(Y+0.5),  no: cvtss2si(Y)
// the xmm6 is the input value
get_number:
    // xmm6: Y=X+0.5
    addss xmm6,[halb]
    // use the floor mode, xmm7: floor(Y)
    roundss xmm7,xmm6,0x02
    de1:
    // xmm7=|~Y~|-Y
    subss xmm7,xmm6
    // compare with 0.5
    subss xmm7,[halb]
    de3:
    // calculate the abs of xmm7
    movss xmm8,[abs_magic_number]
    andps xmm7,xmm8
    de4:
    // compare with precision True:0xffffffff False:0x00000000
    //cmpss xmm7,[precision],0x02
    de5:
    // mov 0x8 to avoid UNORDERED
    //movss xmm8,[cmp_magic_number]
    //andps xmm7,xmm8
    // UNORDERED: ZF,PF,CF<=111
    // GREATER_THAN: ZF,PF,CF<=000
    // LESS_THAN: ZF,PF,CF<=001
    // EQUAL: ZF,PF,CF<=100
    ucomiss xmm7,[precision]
    jbe cmp_yes
        // cmp no:
        cvtss2si rax,xmm6
        jmp end
    cmp_yes:
        // cmp yes:
        addss xmm6,[halb]
        cvtss2si rax,xmm6
    end:
        jmp rbp
        // the result is in rax

iteration_method:
construct3210AP:
    // Arithmetic progression: common diff= res
    //2 2 0 0
    xorps xmm6,xmm6
    movss xmm6, [two]
    pshufd xmm6,xmm6,0x05
    //1 0 1 0
    xorps xmm7,xmm7
    movss xmm7, [one]
    pshufd xmm7,xmm7,0x11
    // sumto 3 2 1 0 in xmm6
    addps xmm6,xmm7
    // res(xmm4)* 3 2 1 0 => xmm6
    pshufd xmm7, xmm4, 0x00
    mulps xmm6,xmm7
    // result: res|res|res|res(xmm7)* 3| 2| 1| 0 => xmm6
    // init common diff 
    movss xmm9,[four]
    pshufd xmm9, xmm9, 0x00
    // four_four: .float 4.0,4.0,4.0,4.0
    // movups xmm9, four_four
    mulps xmm9,xmm7
    //result: reg(4*res|4*res|4*res|4*res): xmm9
    // init a0[4] 
    movss xmm7, [a0]
    pshufd xmm7, xmm7, 0x00
    addps xmm7, xmm6
    // result: xmm7:(-2+3*res|-2+2*res|-2+1*res|-2+0*res) 
    movaps xmm15, xmm7
    // save common diff xmm7 to xmm15
    // init b0[4] 
    movss xmm8, [b0]
    pshufd xmm8, xmm8, 0x00
    // result: xmm8:(-1|-1|-1|-1)
    pshufd xmm6, xmm4, 0x00
    // result: res|res|res|res(xmm6) for b_interation

    de2:
    // use rsi for the index_offset of img(rdi)
    xor rsi, rsi
    // for-loop number a =>rcx, b =>rax
    // save a_number into r8
    mov r8,rcx
    // save b_number into rcx
    mov rcx,rax
    // input : give the number of checking,( in rcx)
    // input : a0[4](xmm7) and b0[4](xmm8)

iteration_formula:

    // stack: top |b_number|....| down
    push rcx
    //rcx <= a_number
    mov rcx,r8
        iteration_a:
            //buffer the xmm7 and xmm8 before the iteration_core
            sub rsp, 16
            movups xmmword ptr [rsp], xmm7
            sub rsp, 16
            movups xmmword ptr [rsp], xmm8
            // stack: top |a_number|xmm8|xmm7|b_number|....| down
            push rcx 
            // rcx: iter_count
            xor rcx, rcx
            mov ecx, [iter_count]
            // tmp = new_a
            // new_a = new_a(xmm10) * new_a - new_b(xmm11) * new_b + a(xmm7)
            // new_b = -2 * tmp * new_b + b(xmm8)
            // a0[4](xmm7) and b0[4](xmm8)
            movss xmm10,[zero]
            pshufd xmm10,xmm10,0x00
            pshufd xmm11,xmm10,0x00
            //-2:[a0]
            movss xmm5,[a0]
            pshufd xmm5,xmm5,0x00
            iteration_core:
            //10=>13 11=>14 10-14=>10  14+13=>14  10*14=>10 10+7=>10
            //          13*11=>11  11*[-2]=>11 11+8=>11
                //firstline:
            movaps xmm13, xmm10
            movaps xmm14, xmm11
            subps xmm10,xmm14
            addps xmm14,xmm13
            mulps xmm10,xmm14
            addps xmm10,xmm7
            //secondline:
            mulps xmm11,xmm13
            mulps xmm11,xmm5
            addps xmm11,xmm8

            // iter_count
            dec rcx
            jnz iteration_core
            // loop iteration_core
            // new_a => a_n[4](xmm7) and new_b => b_n[4](xmm8)
            // 10=>7 11=>8
            movaps xmm7, xmm10
            movaps xmm8, xmm11
            // rcx <= a_number , top |xmm8|xmm7|b_number|....| down
            pop rcx 
            //handle the result
            //a_number sub 4
            sub rcx,4
            // less EQUAL than 0, means last round (may have valid packs less than 4)
            //stack: top |a_number(already -4)|xmm8|xmm7|b_number|....| down
            push rcx
            jb last_round
            mov rcx, 4
            jmp check_write_out_one_turn
            last_round:
                //may have data_pedding, calculate how many valid packs
                mov rax,rcx
                add rax,4
                mov rcx, rax
            //now use rcx as number of valid packs(shift round)
            check_write_out_one_turn:
                // shift and cmp
                jmp is_inf
            end_save_one_turn:
            //rcx<=a_number(already -4),stack: top |xmm8|xmm7|b_number|....| down
            pop rcx
            //pop the buffered xmm7 and xmm8, stack: top |b_number|....| down
            movups xmm8,xmmword ptr [rsp]
            add rsp, 16
            movups xmm7,xmmword ptr [rsp]
            add rsp, 16
            // result: reg(4*res|4*res|4*res|4*res): xmm9
            // result: xmm7:(-2+3*res|-2+2*res|-2+1*res|-2+0*res) 
            addps xmm7,xmm9
            cmp rcx,0
        jg iteration_a
    // rcx <= b_number , stack: top |....| down
    pop rcx
    // b_number--&  & loop
    // before b_loop renew the a and b value:
    // renew a0[4](xmm7) and b0[4](xmm8), b does not change
    // result: res|res|res|res => xmm6
    // result: reg(4*res|4*res|4*res|4*res): xmm9
    // xmm15:(-2+3*res|-2+2*res|-2+1*res|-2+0*res) : init value
    // a init again
    movaps xmm7, xmm15
    //b++
    addps xmm8, xmm6
    dec rcx
    jnz iteration_formula
//loop iteration_formula
ret


is_inf:
    // idea:
    // UNORDERED: ZF,PF,CF<=111
    // GREATER_THAN: ZF,PF,CF<=000
    // LESS_THAN: ZF,PF,CF<=001
    // EQUAL: ZF,PF,CF<=100
    // for a normal value x => 0<|x|<float.max, (ZF(100) or CF(001))
    // for a NAN x =>  UNORDERED (111)
    // but for x= +-inf => |x| = +inf, +inf >float.max (000)
    // the a_input x in xmm7
    // the b_input x in xmm8
    
    // get the abs of packed xmm7 => xmm10
    movss xmm10,[abs_magic_number]
    pshufd xmm10, xmm10, 0x00
    //result: xmm10: 7ff...|7ff...|7ff...|7ff...
    andps xmm10,xmm7
    // the |a_input| now in xmm10

    // get the abs of packed xmm8 => xmm13
    movss xmm13,[abs_magic_number]
    pshufd xmm13, xmm13, 0x00
    //result: xmm13: 7ff...|7ff...|7ff...|7ff...
    andps xmm13,xmm8
    // the |b_input| now in xmm13

    // copy 7 to 12, use 12 instead 7
    movaps xmm12, xmm7
    // copy 8 to 14, use 14 instead 8
    movaps xmm14, xmm8

    //compare number: float.max
    movss xmm11,[max_float]
// shift the packed data to check every packs
for:
    ucomiss xmm10,xmm11
    // not inf goto is_nan
    // can't use jg(ZF=0 and SF=OF) here because LESS_THAN also goes
    // use the opposite side jbe(CF=1 or ZF=1) instead.
    jbe is_b_inf
    jmp handle_inf
    is_b_inf:
    ucomiss xmm13,xmm11
    jbe is_nan
    jmp handle_inf
is_nan:
    // idea:
    // UNORDERED: ZF,PF,CF<=111
    // GREATER_THAN: ZF,PF,CF<=000
    // LESS_THAN: ZF,PF,CF<=001
    // EQUAL: ZF,PF,CF<=100
    // according to ucomiss doc: 
    // the result will be UNORDERED(above), if the either SRCREG or DSTREG is NAN
    ucomiss xmm10,xmm10
    jp handle_nan
    ucomiss xmm13,xmm13
    jp handle_nan
test_a_boundary:
    // the value is valid number, check in boundary
    // r_end>a>r_start  i_end>b>i_start
    // r_start:xmm0 r_end:xmm1 
    ucomiss xmm12,xmm0
    // a<rstart
    jbe handle_not_in_boundary
    // a<r_end
    ucomiss xmm12,xmm1
    jbe test_b_boundary
    jmp handle_not_in_boundary
test_b_boundary:
    // i_start:xmm2 i_end:xmm3 res:xmm4 img:rdi
    ucomiss xmm14,xmm2
    // a<=rstart
    jbe handle_not_in_boundary
    // a<r_end
    ucomiss xmm14,xmm3
    jbe black
    jmp handle_not_in_boundary
    black:
        mov al, 0x00
        mov [rdi+rsi],al
        inc rsi
        mov [rdi+rsi],al
        inc rsi
        mov [rdi+rsi],al
        inc rsi
    shift_loop:
    // shift 4 bytes per loop 
    psrldq xmm10, 4
    psrldq xmm12, 4
    psrldq xmm13, 4
    psrldq xmm14, 4
    loop for
    jmp end_save_one_turn

handle_nan:
handle_inf:
handle_not_in_boundary:
        white:
        //end white goto 
        mov al, 0xff
        mov [rdi+rsi],al
        inc rsi
        mov [rdi+rsi],al
        inc rsi
        mov [rdi+rsi],al
        inc rsi
    jmp shift_loop


