#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#define A_START -2.0
#define A_END 1.0
#define B_START -1.0
#define B_END 1.0
#define ITERATION_NUMBER 500
#define N 5
#define PRECISION 0.999
void multicorn_iteration_step(float r_start, float r_end, float i_start, float i_end, float res, unsigned char *img, float *last_a, float *last_b);
void multicorn_iteration_number(float r_start, float r_end, float i_start, float i_end, float res, unsigned char *img, int iteration_number);
float calculate_accuracy(unsigned char *a, unsigned char *b, int len, int *is_perfect_match);
int calculate_block_len(float res);
int get_number(float x);
// we define a "parameters" struct
struct parameters
{
    float r_start;
    float r_end;
    float i_start;
    float i_end;
    float res;
};
float do_test(double res, int array_num, struct parameters *parameters_array);
// this function generate series of parameter by changing the res step by step
void generate_serial_parameters(double res, struct parameters parameters_array[], float r_start, float r_end, float i_start, float i_end)
{
    // res: 0 to 1, step: res
    // to deal with the float round-off error, we use 0.001 * (0..1000), here:res=0.001, array_num=1000
    int k = 0;
    int array_num = get_number(1 / res) - 1;
    for (size_t i = 1; i <= array_num; i++)
    {
        struct parameters tmp = {
            r_start, r_end, i_start, i_end,
            (float)(res * i)};
        parameters_array[k] = tmp;
        k++;
        
    }
}
// this is the entry of the program.
// the program will run multicorn_iteration_number() with different iteration_number step by step.
// if the result is the same as the iteration_500 and
//   stable after consequent n times increase of the iteration_number, then we regard it as the minimal iteration_number(at some 'res' point).
int main(int argc, char **argv)
{
    printf("start measuring iteration_number: \n");
    double res = 0.001;
    int array_num = get_number(1 / res) - 1;
    int iteration_number = 1;
    struct parameters *parameters_array = malloc(array_num * sizeof(struct parameters));
    generate_serial_parameters(res, parameters_array, -2, 2, -2, 2);
    float total_average_accuracy = do_test(res, array_num, parameters_array);
    free(parameters_array);
    return 0;
}
// do one round test, return the average_accuracy
float do_test(double res, int array_num, struct parameters *parameters_array)
{

    int mem_len = 0;
    unsigned char *array_test = NULL;
    unsigned char *array_500 = NULL;
    int is_perfect_match = 1;
    float accuracy = 0;
    int perfect_match_num = 0;
    int flag = 1;
    printf("Arguments:\n");
    printf("r_start:%f, r_end:%f, i_start:%f, i_end:%f\n", parameters_array[0].r_start, parameters_array[0].r_end, parameters_array[0].i_start, parameters_array[0].i_end);
    printf("res: from %f to 1, step:%f, total:%d\n", res, res, array_num);
    float average_accuracy = 0;
    for (size_t i = 0; i < array_num; i++)
    {

        // calculate the img with 500 iteration_number:
        mem_len = calculate_block_len(parameters_array[i].res) * 3 * sizeof(unsigned char);
        array_500 = realloc(array_500, mem_len);
        if (array_500 == NULL)
        {
            printf("Memory not allocated.\n");
            exit(1);
        }
        multicorn_iteration_number(parameters_array[i].r_start, parameters_array[i].r_end, parameters_array[i].i_start, parameters_array[i].i_end, parameters_array[i].res, array_500, ITERATION_NUMBER);

        array_test = realloc(array_test, mem_len);
        if (array_test == NULL)
        {
            printf("Memory not allocated.\n");
            exit(1);
        }
        int a_number = get_number((A_END - A_START) / res);
        int b_number = get_number((B_END - B_START) / res);
        int len = a_number * b_number;
        float *init_a = malloc(len * sizeof(float));
        float *init_b = malloc(len * sizeof(float));
        for (size_t m = 0; m < len; m++)
        {
            init_a[m] = 0;
            init_b[m] = 0;
        }

        for (size_t j = 0; j < ITERATION_NUMBER; j++)
        {
            multicorn_iteration_step(parameters_array[i].r_start, parameters_array[i].r_end, parameters_array[i].i_start, parameters_array[i].i_end, parameters_array[i].res, array_test, init_a, init_b);
            // calculate_accuracy
            is_perfect_match = 1;
            accuracy = calculate_accuracy(array_test, array_500, mem_len, &is_perfect_match);
            if (accuracy > PRECISION)
            {
                // printf("!!almost_perfect_match:%ld\n", j);
                perfect_match_num++;
                if (perfect_match_num >= N)
                {
                    flag = 0;
                    printf("\tres:%f => min_Iteration_num:%ld\n", parameters_array[i].res, j - N + 1);
                    break;
                }
            }
            else
            {
                perfect_match_num = 0;
            }
        }
        if (flag)
        {
            printf("\tres:%f => not found\n", parameters_array[i].res);
        }
        free(init_a);
        free(init_b);
    }
    return 0;
}
// use 1-(different number of pixels)/(the total number of pixels)
float calculate_accuracy(unsigned char *a, unsigned char *b, int len, int *is_perfect_match)
{
    int diff = 0;
    // only check one byte is ok,
    // since the consequent 3 bytes are the same and represent one pixel
    for (size_t i = 0; i < len; i += 3)
    {
        if (a[i] != b[i])
        {
            diff++;
            // red: #ff0000 rgb
            a[i] = 0x00;     //b
            a[i + 1] = 0x00; //g
            a[i + 2] = 0xff; //r
        }
    }
    if (diff == 0)
    {
        *is_perfect_match = 0;
    }
    else
    {
        *is_perfect_match = diff;
    }

    return 1 - ((float)diff) / (len / 3);
}
int is_same(unsigned char *a, unsigned char *b, int len)
{
    return memcmp(a, b, len);
}

// this function is the same with the function in the main program, see @@@.c
int calculate_block_len(float res)
{
    float a_len = A_END - A_START;
    float b_len = B_END - B_START;
    int width = get_number(a_len / res);
    int height = get_number(b_len / res);
    // printf("debug:calculate_block_len: %d \n", width * height);
    return width * height;
}
// this function is the same with the function in the main program, see @@@.c
int get_number(float x)
{
    int ret = 0;
    int round_x = lrintf(x);
    float diff = round_x - x;
    if (diff > 0 && diff < 0.01)
    {
        ret = round_x + 1;
    }
    else
    {
        ret = (int)x + 1;
    }
    return ret;
}
