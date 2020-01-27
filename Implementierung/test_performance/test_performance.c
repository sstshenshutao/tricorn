#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>
#define A_START -2.0
#define A_END 1.0
#define B_START -1.0
#define B_END 1.0
#define ITERATION_NUMBER 70
#define AVERAGE_NUM 10
extern void multicorn(float r_start, float r_end, float i_start, float i_end, float res, unsigned char *img);
extern void multicorn_c_test_double(float r_start, float r_end, float i_start, float i_end, float res, unsigned char *img);
float calculate_accuracy(unsigned char *a, unsigned char *b, int len, int *is_perfect_match);
int calculate_block_len(float res);
static inline double curtime(void);
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
void do_test(double res, int array_num, struct parameters *parameters_array);
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
    printf("%d\n", k);
}
// this is the entry of the program.
// the program will     run multicorn(assembly code)        as array_assembly
//                      run multicorn_c_test_double(c code) as array_c
// and then run the comparison function to test the correction.
int main(int argc, char **argv)
{
    printf("start test performance: \n");
    double res = 0.001;
    int array_num = get_number(1 / res) - 1;
    struct parameters *parameters_array = malloc(array_num * sizeof(struct parameters));
     if (parameters_array == NULL)
    {
        printf("Memory not allocated.\n");
        exit(1);
    }
    generate_serial_parameters(res, parameters_array, -2, 2, -2, 2);
    do_test(res, array_num, parameters_array);
    free(parameters_array);
    printf("test finished.\n");
    return 0;
}
// do one round test, return the average_accuracy
void do_test(double res, int array_num, struct parameters *parameters_array)
{
    int mem_len = 0;
    unsigned char *array_assembly = NULL;
    unsigned char *array_c = NULL;
    printf("Arguments:\n");
    printf("r_start:%f, r_end:%f, i_start:%f, i_end:%f\n", parameters_array[0].r_start, parameters_array[0].r_end, parameters_array[0].i_start, parameters_array[0].i_end);
    printf("res: from %f to 1, step:%f, total:%d\n", res, res, array_num);
    double start = 0;
    double end = 0;
    for (size_t i = 0; i < array_num; i++)
    {

        mem_len = calculate_block_len(parameters_array[i].res) * 3 * sizeof(unsigned char);
        // malloc the space for the img buffer.
        array_assembly = realloc(array_assembly, mem_len);
        array_c = realloc(array_c, mem_len);
        if (array_assembly == NULL || array_c == NULL)
        {
            printf("Memory not allocated.\n");
            exit(1);
        }

        fprintf(stdout, "\tres:%f\n", parameters_array[i].res);
        // calculate array_assembly
        double sum_time = 0;
        for (size_t w = 0; w < AVERAGE_NUM; w++)
        {
            start = curtime();
            multicorn(parameters_array[i].r_start, parameters_array[i].r_end, parameters_array[i].i_start, parameters_array[i].i_end, parameters_array[i].res, array_assembly);
            end = curtime();
            sum_time += end - start;
        }
        fprintf(stdout, "\t\tassembly_code: \trun_time:%lf s\n", sum_time / AVERAGE_NUM);

        // calculate array_c
        sum_time = 0;
        for (size_t w = 0; w < AVERAGE_NUM; w++)
        {
            start = curtime();
            multicorn_c_test_double(parameters_array[i].r_start, parameters_array[i].r_end, parameters_array[i].i_start, parameters_array[i].i_end, parameters_array[i].res, array_c);
            end = curtime();
            sum_time += end - start;
        }
        fprintf(stdout, "\t\tc_code: \trun_time:%lf s\n", sum_time / AVERAGE_NUM);
        // calculate_accuracy
    }
    free(array_assembly);
    free(array_c);
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
// this code is cited from "era_toupper_simd/main.c"
static inline double curtime(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}
