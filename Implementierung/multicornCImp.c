#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define A_START -2.0
#define A_END 1.0
#define B_START -1.0
#define B_END 1.0
#define ITERATION_NUMBER 50
int is_in_boundary(float a, float b, float r_start, float r_end, float i_start, float i_end, float res)
{
    return (a > r_start || fabsf(a - r_start) <= res / 10) && (a < r_end || fabsf(r_end - a) <= res / 10) && (b > i_start || fabsf(b - i_start) <= res / 10) && (b < i_end || fabsf(i_end - b) <= res / 10);
}
void multicorn(float r_start, float r_end, float i_start, float i_end, float res, unsigned char *img)
{
    //todo: ###### this need to change later ######
    int a_number = ((int)roundf((A_END - A_START) / res)) + 1;
    int b_number = ((int)roundf((B_END - B_START) / res)) + 1;
    float a = A_START;
    float b = B_START;
    size_t img_index = 0;
    // firstly raw and the column
    printf("debug: a_number %d \n",a_number);
    printf("debug: b_number %d \n",b_number);
    float new_a = 0;
    float new_b = 0;
    float tmp = 0;
    int flag = 1;
    // #debug
    int black =0;
    int white =0;
    //end debug
    for (size_t i = 0; i < b_number; i++)
    {
        for (size_t j = 0; j < a_number; j++)
        {
            //z0=0;
            new_a = 0;
            new_b = 0;
            flag = 1;
            //iteration process
            for (size_t i = 0; i < ITERATION_NUMBER; i++)
            {
                tmp = new_a;
                new_a = new_a * new_a - new_b * new_b + a;
                new_b = -2 * tmp * new_b + b;
                if (isnanf(new_a) || isinff(new_a) || isnanf(new_b) || isinff(new_b) || !is_in_boundary(new_a, new_b, r_start, r_end, i_start, i_end, res))
                {
                    img[img_index] = 0xff;
                    img[img_index + 1] = 0xff;
                    img[img_index + 2] = 0xff;
                    // printf("debug white new_a: %f \n", new_a);
                    // printf("debug white new_b: %f \n", new_b);
                    white++;
                    flag = 0;
                    break;
                }
            }
            if (flag)
            {
                // printf("debug black new_a: %f \n", new_a);
                // printf("debug black new_b: %f \n", new_b);
                black++;
                img[img_index] = 0x00;
                img[img_index + 1] = 0x00;
                img[img_index + 2] = 0x00;
            }
            // printf("img_index: %d \n", img_index);
            // printf("a: %f \n", a);
            // printf("b: %f \n", b);
            img_index += 3;
            a += res;
        }
        b += res;
        a = A_START;
    }

    // # debug
    printf("black: %d \n", black);
    printf("white: %d \n", white);
    // while (fabsf(B_END - b) >= res / 10)
    // {
    //     while (fabsf(A_END - a) >= res / 10)
    //     {
    //         //z0=0;
    //         float new_a = 0;
    //         float new_b = 0;
    //         int flag = 1;
    //         //iteration process
    //         for (size_t i = 0; i < ITERATION_NUMBER; i++)
    //         {
    //             new_a = new_a * new_a - new_b * new_b + a;
    //             new_b = -2 * new_a * new_b + b;
    //             if (isnanf(new_a) || isinff(new_a) || isnanf(new_b) || isinff(new_b) || !is_in_boundary(new_a, new_b, r_start, r_end, i_start, i_end, res))
    //             {
    //                 img[img_index] = 0xff;
    //                 img[img_index + 1] = 0xff;
    //                 img[img_index + 2] = 0xff;
    //                 flag = 0;
    //                 break;
    //             }
    //         }
    //         if (flag)
    //         {
    //             img[img_index] = 0x00;
    //             img[img_index + 1] = 0x00;
    //             img[img_index + 2] = 0x00;
    //         }
    //         printf("img_index: %d \n", img_index);
    //         printf("a: %f \n", a);
    //         printf("b: %f \n", b);
    //         img_index += 3;
    //         a += res;
    //     }
    //     b += res;
    //     a = A_START;
    // }
}
