.intel_syntax noprefix

//void multicorn(float r_start,float r_end,float i_start,float i_end,float res,unsigned char* img)
//r_start:xmm0 r_end:xmm1 i_start:xmm2 i_end:xmm3 res:xmm4 img:rdi
.global multicorn

.data 
iter_count: .int 0xa
a: .float 0.99999999999999999999999999999999999999990000
b: .float 3.49999999999999999999999999999999999999
halb: .float 0.5
test_value: .float 1.0
test_value1: .float 1.4999999999999999999999999999999
test_value2: .float 1.5000000000000000000000000000001
test_value3: .float 1.9999999999999999999999999999999
test_value4: .float 1.689999999999999999999
precision: .float 0.01
abs_magic_number: .byte 0xff,0xff,0xff,0x7f
cmp_magic_number: .byte 0x00,0x00,0x00,0x41
.text

multicorn:
    movss xmm0, [test_value3]
    jmp get_number

// idea:
// the input value is X, we use Y=X+0.5, 
// we know that Y = xxx.5 may cause problem, so:
// floor(Y)-Y ?.= 0.5  if yes: cvtss2si(Y+0.5),  no: cvtss2si(Y)
// the xmm0 is the input value
get_number:
    // xmm0: Y=X+0.5
    addss xmm0,[halb]
    // use the floor mode, xmm1: floor(Y)
    roundss xmm1,xmm0,0x02
    de1:
    // xmm1=|~Y~|-Y
    subss xmm1,xmm0
    de2:
    // compare with 0.5
    subss xmm1,[halb]
    de3:
    // calculate the abs of xmm1
    movss xmm2,[abs_magic_number]
    andps xmm1,xmm2
    de4:
    // compare with precision True:0xffffffff False:0x00000000
    //cmpss xmm1,[precision],0x02
    de5:
    // mov 0x8 to avoid UNORDERED
    //movss xmm2,[cmp_magic_number]
    //andps xmm1,xmm2

    // UNORDERED: ZF,PF,CF<=111;
    // GREATER_THAN: ZF,PF,CF<=000;
    // LESS_THAN: ZF,PF,CF<=001;
    // EQUAL: ZF,PF,CF<=100;
    ucomiss xmm1,[precision]
    //if floor(Y)-Y .= 0.5 ,then xmm1 will LESS_THAN [precision]
    jbe cmp_yes
    cvtss2si rax,xmm0
    jmp end
    cmp_yes:
        // cmp yes:
        addss xmm0,[halb]
        cvtss2si rax,xmm0
    end:
        // the result is in rax
ret



