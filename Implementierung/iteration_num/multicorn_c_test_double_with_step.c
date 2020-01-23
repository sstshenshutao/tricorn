#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define A_START -2.0
#define A_END 1.0
#define B_START -1.0
#define B_END 1.0
int is_in_boundary(float a, float b, float r_start, float r_end, float i_start, float i_end, float res)
{
    return (a > r_start) && (a < r_end) && (b > i_start) && (b < i_end);
}
int get_number_x(float x)
{
    // the value int(x) is only not exact at xxx.999999
    // so we use the following mechanism to avoid it:
    // round_x - x <0.1  Yes? means the value is about x.99999
    int ret = 0;
    int round_x = lrintf(x);
    float diff = round_x - x;
    if (diff > 0 && diff < 0.01)
    {
        //use the value round_x - 1
        ret = round_x + 1;
    }
    else
    {
        //directly cast
        ret = (int)x + 1;
    }
    return ret;
}
// this function is the same with the function in the test_correction program, see @@@.c
void multicorn_iteration_number(float r_start, float r_end, float i_start, float i_end, float res, unsigned char *img, int iteration_number)
{
    int a_number = get_number_x((A_END - A_START) / res);
    int b_number = get_number_x((B_END - B_START) / res);
    float a = A_START;
    float b = B_START;
    size_t img_index = 0;
    float new_a = 0;
    float new_b = 0;
    float tmp = 0;
    int flag = 1;

    for (size_t i = 0; i < b_number; i++)
    {
        for (size_t j = 0; j < a_number; j++)
        {

            new_a = 0;
            new_b = 0;

            //iteration process
            for (size_t m = 0; m < iteration_number; m++)
            {

                tmp = new_a;
                new_a = new_a * new_a - new_b * new_b + a;
                new_b = -2 * tmp * new_b + b;
            }
            if (isnanf(new_a) || isinff(new_a) || isnanf(new_b) || isinff(new_b) || !is_in_boundary(new_a, new_b, r_start, r_end, i_start, i_end, res))
            {
                img[img_index] = 0xff;
                img[img_index + 1] = 0xff;
                img[img_index + 2] = 0xff;
            }
            else
            {

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
void multicorn_iteration_step(float r_start, float r_end, float i_start, float i_end, float res, unsigned char *img, float *last_a, float *last_b)
{
    int a_number = get_number_x((A_END - A_START) / res);
    int b_number = get_number_x((B_END - B_START) / res);
    float a = A_START;
    float b = B_START;
    size_t img_index = 0;
    float new_a = 0;
    float new_b = 0;
    float tmp = 0;
    int flag = 1;
    int k = 0;
    for (size_t i = 0; i < b_number; i++)
    {
        for (size_t j = 0; j < a_number; j++)
        {
            new_a = last_a[k];
            new_b = last_b[k];

            for (size_t m = 0; m < 1; m++)
            {
                tmp = new_a;
                new_a = new_a * new_a - new_b * new_b + a;
                new_b = -2 * tmp * new_b + b;
            }
            last_a[k] = new_a;
            last_b[k] = new_b;
            if (isnanf(new_a) || isinff(new_a) || isnanf(new_b) || isinff(new_b) || !is_in_boundary(new_a, new_b, r_start, r_end, i_start, i_end, res))
            {
                img[img_index] = 0xff;
                img[img_index + 1] = 0xff;
                img[img_index + 2] = 0xff;
            }
            else
            {
                img[img_index] = 0x00;
                img[img_index + 1] = 0x00;
                img[img_index + 2] = 0x00;
            }
            img_index += 3;
            a += res;
            k++;
        }
        b += res;
        a = A_START;
    }
}
