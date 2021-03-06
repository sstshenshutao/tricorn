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
extern void multicorn(float r_start, float r_end, float i_start, float i_end, float res, unsigned char *img);
extern void multicorn_c_test_double(float r_start, float r_end, float i_start, float i_end, float res, unsigned char *img);
float calculate_accuracy(unsigned char *a, unsigned char *b, int len, int *is_perfect_match);
int calculate_block_len(float res);
int get_number(float x);

void write_out_file(int block_len, unsigned char *img, char *output_path, float res);
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
    printf("%d\n", k);
}
// this is the entry of the program.
// the program will     run multicorn(assembly code)        as array_assembly
//                      run multicorn_c_test_double(c code) as array_c
// and then run the comparison function to test the correction.
int main(int argc, char **argv)
{
    printf("start test: \n");
    double res = 0.001;
    int array_num = get_number(1 / res) - 1;
    // we generate multiple parameters as parameters array to run two functions
    // we generate from square with r_end from 2 to FLT_MAX, with 99 value between them
    // the 100 values that we get are (2, 2^2, 2^3 ,..., 2^99, FLT_MAX)
    float r_end = 1;
    float total_average_accuracy = 0;
    for (size_t i = 0; i < 99; i++)
    {
        struct parameters *parameters_array = malloc(array_num * sizeof(struct parameters));
        if (parameters_array == NULL)
        {
            printf("Memory not allocated.\n");
            exit(1);
        }
        r_end *= 2;
        generate_serial_parameters(res, parameters_array, -1 * r_end, r_end, -1 * r_end, r_end);
        total_average_accuracy += do_test(res, array_num, parameters_array);
        free(parameters_array);
    }
    // avoid round-off error to infinity
    struct parameters *parameters_array = malloc(array_num * sizeof(struct parameters));
    if (parameters_array == NULL)
    {
        printf("Memory not allocated.\n");
        exit(1);
    }
    generate_serial_parameters(res, parameters_array, -1 * FLT_MAX, FLT_MAX, -1 * FLT_MAX, FLT_MAX);
    total_average_accuracy += do_test(res, array_num, parameters_array);
    free(parameters_array);
    printf("total_summary: total_average_accuracy:%.3f%%\n", total_average_accuracy);
    return 0;
}
// do one round test, return the average_accuracy
float do_test(double res, int array_num, struct parameters *parameters_array)
{
    int mem_len = 0;
    unsigned char *array_assembly = NULL;
    unsigned char *array_c = NULL;
    int is_perfect_match = 1;
    float accuracy = 0;
    int non_perfect_match_num = 0;
    printf("Arguments:\n");
    printf("r_start:%f, r_end:%f, i_start:%f, i_end:%f\n", parameters_array[0].r_start, parameters_array[0].r_end, parameters_array[0].i_start, parameters_array[0].i_end);
    printf("res: from %f to 1, step:%f, total:%d\n", res, res, array_num);
    //for visualization
    char visual_correction_dir[] = "./correction/";
    // ‘sprintf’ output between 23 and 641 bytes
    char stri[641];
    char bmp[] = ".bmp";
    char *buff = NULL;
    float average_accuracy = 0;
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
        // calculate array_assembly
        multicorn(parameters_array[i].r_start, parameters_array[i].r_end, parameters_array[i].i_start, parameters_array[i].i_end, parameters_array[i].res, array_assembly);
        // calculate array_c
        multicorn_c_test_double(parameters_array[i].r_start, parameters_array[i].r_end, parameters_array[i].i_start, parameters_array[i].i_end, parameters_array[i].res, array_c);
        // calculate_accuracy
        is_perfect_match = 1;
        accuracy = calculate_accuracy(array_assembly, array_c, mem_len, &is_perfect_match);
        average_accuracy += accuracy;
        if (is_perfect_match)
        {
            non_perfect_match_num++;
            // use red to visualization the wrong pixels.
            // generate the path of wrong visualization bmps.
            sprintf(stri, "r_end_%.3f_res_%.3f", parameters_array[0].r_end, parameters_array[i].res);
            buff = realloc(buff, (strlen(visual_correction_dir) + strlen(stri) + strlen(bmp) + 1) * sizeof(char));
            buff[0] = '\0';
            strcat(buff, visual_correction_dir);
            strcat(buff, stri);
            strcat(buff, bmp);
            write_out_file(calculate_block_len(parameters_array[i].res), array_assembly, buff, parameters_array[i].res);
            printf("\tres:%f => accuracy:%f, diff_pixels:%d\n", parameters_array[i].res, accuracy, is_perfect_match);
        }
    }
    average_accuracy /= array_num;
    printf("summary: %d/%d test, %.3f%% perfect match, average_accuracy:%.3f%%\n", array_num - non_perfect_match_num, array_num, ((float)(array_num - non_perfect_match_num)) / array_num * 100, average_accuracy * 100);
    free(array_assembly);
    free(array_c);
    free(buff);
    return average_accuracy;
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
// this function is the same with the function in the main program, see @@@.c
void write_out_file(int block_len, unsigned char *img, char *output_path, float res)
{
    unsigned char header[54] = {0x42, 0x4d, 0x96, 0xe3, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0xe8, 0x03, 0x00, 0x00, 0xf4, 0x01, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x0b, 0x00, 0x00, 0x12, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    int block_size = block_len * 3 + 54;
    if (block_size > 0xffffffff)
    {
        printf("file is too big to save in .bmp\n");
        exit(1);
    }
    float a_len = A_END - A_START;
    float b_len = B_END - B_START;
    int width = get_number(a_len / res);
    int height = get_number(b_len / res);
    int width_pix = width * 3;
    int zero_number = width_pix & 0x03 ? 4 - (width_pix & 0x03) : 0;
    int new_width_pix = width_pix + zero_number;
    int new_block_size = new_width_pix * height + 54;
    int a = new_block_size;
    int b;
    for (size_t i = 0; i < 4; i++)
    {
        b = (a >> 8);
        header[2 + i] = a - (b << 8);
        a = b;
    }
    a = width;
    for (size_t i = 0; i < 4; i++)
    {
        b = (a >> 8);
        header[18 + i] = a - (b << 8);
        a = b;
    }
    a = height;
    for (size_t i = 0; i < 4; i++)
    {
        b = (a >> 8);
        header[22 + i] = a - (b << 8);
        a = b;
    }
    unsigned char *buffer = malloc(new_block_size * sizeof(unsigned char));
    if (buffer == NULL)
    {
        printf("Memory not allocated.\n");
        exit(1);
    }
    memcpy(buffer, header, 54);
    if (width % 4)
    {
        unsigned char zero_array[zero_number];
        for (size_t i = 0; i < zero_number; i++)
        {
            zero_array[i] = 0x00;
        }

        for (size_t i = 0; i < height; i++)
        {
            memcpy(buffer + 54 + new_width_pix * i, img + width_pix * i, width_pix * sizeof(unsigned char));
            memcpy(buffer + 54 + new_width_pix * i + width_pix, zero_array, zero_number * sizeof(unsigned char));
        }
    }
    else
    {
        memcpy(buffer + 54, img, (block_size - 54) * sizeof(unsigned char));
    }
    FILE *f = fopen(output_path, "w");
    if (f == NULL)
    {
        printf("error in opening the output file: %s\n", output_path);
        exit(1);
    }
    fwrite(buffer, sizeof(unsigned char), new_block_size, f);
    fclose(f);
    free(buffer);
}
