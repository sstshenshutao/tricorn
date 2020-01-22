#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define A_START -2.0
#define A_END 1.0
#define B_START -1.0
#define B_END 1.0
#define ITERATION_NUMBER 70

// check if the value after calculate is convergent
int is_in_boundary(float a, float b, float r_start, float r_end, float i_start, float i_end, float res)
{
    return (a > r_start ) && (a < r_end ) && (b > i_start ) && (b < i_end );
}

// cause of character of float we need to set a accurate count for iteration 
// that means
// the value int(x) is only not exact at xxx.999999
// so we use the following mechanism to avoid it:
// round_x - x <0.1  Yes? means the value is about x.99999
// for more detail please see the "ausarbeitung"
int get_number_x(float x)
{
    int ret = 0;
    int round_x = lrintf(x);
    float diff = round_x - x;
    if (diff > 0 && diff < 0.01)
    {
        //use the value round_x - 1
        ret = round_x+1;
    }
    else
    {
        //directly cast
        ret = (int)x + 1;
    }
    return ret;
}
void multicorn(float r_start, float r_end, float i_start, float i_end, float res, unsigned char *img)
{
    //a:real number b:imaginary number
    //a_number:the for_loop number for a
    //b_number:the for_loop number for b
    int a_number = get_number_x((A_END - A_START) / res);
    int b_number = get_number_x((B_END - B_START) / res);
    float a = A_START;
    float b = B_START;
    size_t img_index = 0;
    float new_a = 0;
    float new_b = 0;
    float tmp = 0;
    int flag = 1;
     // firstly raw and then the column
    for (size_t i = 0; i < b_number; i++)
    {
        for (size_t j = 0; j < a_number; j++)
        {
            new_a = 0;
            new_b = 0;
            for (size_t m = 0; m < ITERATION_NUMBER; m++)
            {
                //for zn = a'+b'i,c = a + b i  
                //zn+1 = (a'^2 -b'^2 +a)+(-2a'b' +b)i 
                tmp = new_a;
                new_a = new_a * new_a - new_b * new_b + a;
                new_b = -2 * tmp * new_b + b;
            }
            //the results should be ordered(not NAN, not Inf) and in boundary
            //if true ==> coloring in black, else ==> coloring in white
            if (isnanf(new_a) || isinff(new_a) || isnanf(new_b) || isinff(new_b) || !is_in_boundary(new_a, new_b, r_start, r_end, i_start, i_end, res))
            {
                //white
                img[img_index] = 0xff;
                img[img_index + 1] = 0xff;
                img[img_index + 2] = 0xff;
            }
            else
            {
                //black
                img[img_index] = 0x00;
                img[img_index + 1] = 0x00;
                img[img_index + 2] = 0x00;
            }

            img_index += 3;
            a += res;
        }
        b += res;
        a = A_START;
    }
}
