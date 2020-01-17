#include <stdio.h>
extern float  multicorn(float r_start,float r_end,float i_start,float i_end,float res,unsigned char* img);
int main(int argc, char const *argv[])
{
    float a=multicorn(-10,10,-10,10,0.1,0);
    //printf("c:the sub-program return: %.d\n",a);
    printf("c:the sub-program return: %.4f\n",a);
    return 0;
}
