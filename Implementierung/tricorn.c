#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
extern void multicorn(float r_start, float r_end, float i_start, float i_end, float res, unsigned char *img);
void parse_arg(int argc, char **argv);
void print_manual();
int check_error(int *flags, struct option opts[]);
int safe_strtof(float *parameter, char *optarg);
void write_out_file(int block_len, unsigned char *img, char *output_path, float res);
int calculate_block_len(float res);
int check_constaint(float r_start, float r_end, float i_start, float i_end, float res);
int get_number(float x);
#define A_START -2.0
#define A_END 1.0
#define B_START -1.0
#define B_END 1.0

// this is the entry of the program
int main(int argc, char **argv)
{
    // firstly we need to parse the user's options
    parse_arg(argc, argv);
    return 0;
}
void parse_arg(int argc, char **argv)
{
    // use the flags to check the input args
    int *flags = calloc(sizeof(int), 7);
    if (flags == NULL)
    {
        printf("Memory not allocated.\n");
        exit(1);
    }
    float r_start;
    float r_end;
    float i_start;
    float i_end;
    float res;
    char *output;
    float help;
    //build a struct to get parameters of function 'multicorn'
    struct option opts[] = {
        {"r_start", required_argument, NULL, 'a'},
        {"r_end", required_argument, NULL, 'b'},
        {"i_start", required_argument, NULL, 'c'},
        {"i_end", required_argument, NULL, 'd'},
        {"res", required_argument, NULL, 'r'},
        {"output", required_argument, NULL, 'o'},
        {"help", no_argument, NULL, 'h'}};
    int opt;
    while ((opt = getopt_long(argc, argv, "a:b:c:d:r:o:h", opts, NULL)) != -1)
    {
        switch (opt)
        {
        case 'a':
            flags[0] = !safe_strtof(&r_start, optarg);
            break;
        case 'b':
            flags[1] = !safe_strtof(&r_end, optarg);
            break;
        case 'c':
            flags[2] = !safe_strtof(&i_start, optarg);
            break;
        case 'd':
            flags[3] = !safe_strtof(&i_end, optarg);
            break;
        case 'r':
            flags[4] = !safe_strtof(&res, optarg);
            break;
        case 'o':
            flags[5] = 1;
            output = optarg;
            break;
        case 'h':
            flags[6] = 1;
            print_manual();
            exit(0);
            break;
        default:
            // error: invalid parameters:
            printf("defualt");
            print_manual();
            exit(1);
            break;
        }
    }
    if (check_error(flags, opts) || check_constaint(r_start, r_end, i_start, i_end, res))
    {
        print_manual();
        exit(1);
    }
    //the call-part
    // malloc the space for the img buffer.
    int block_len = calculate_block_len(res);
    unsigned char *img = malloc(block_len * 3 * sizeof(unsigned char));
    if (img == NULL)
    {
        printf("Memory not allocated.\n");
        exit(1);
    }
    multicorn(r_start, r_end, i_start, i_end, res, img);
    //All datas about pixels are stored
    write_out_file(block_len, img, output, res);
    free(img);
    free(flags);
    printf("the whole process is finished\n");
}

//get sum of pixels
int calculate_block_len(float res)
{
    float a_len = A_END - A_START;
    float b_len = B_END - B_START;
    int width = get_number(a_len / res);
    int height = get_number(b_len / res);
    return width * height;
    // return 500000;
}

void write_out_file(int block_len, unsigned char *img, char *output_path, float res)
{
    //header of a bmp-file
    unsigned char header[54] = {0x42, 0x4d, 0x96, 0xe3, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0xe8, 0x03, 0x00, 0x00, 0xf4, 0x01, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x0b, 0x00, 0x00, 0x12, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    int block_size = block_len * 3 + 54;
    if (block_size > 0xffffffff)
    {
        printf("file is too big to save in .bmp\n");
        exit(1);
    }
    // calculate the pedding 0
    float a_len = A_END - A_START;
    float b_len = B_END - B_START;
    // this is the number of tuple(r,g,b)
    int width = get_number(a_len / res);
    int height = get_number(b_len / res);

    int width_pix = width * 3;
    int zero_number = width_pix & 0x03 ? 4 - (width_pix & 0x03) : 0; 
    //For bmp-file processing we get a correct zero_number,so that new_width_pix%4=0
    int new_width_pix = width_pix + zero_number;
    int new_block_size = new_width_pix * height + 54;
    //update informations in header
    // calculate the header.file.size
    int a = new_block_size;
    int b;
    for (size_t i = 0; i < 4; i++)
    {
        b = (a >> 8);
        header[2 + i] = a - (b << 8);
        a = b;
    }
    // calculate the biWidth
    a = width;
    for (size_t i = 0; i < 4; i++)
    {
        b = (a >> 8);
        header[18 + i] = a - (b << 8);
        a = b;
    }
    // calculate the biHeight
    a = height;
    for (size_t i = 0; i < 4; i++)
    {
        b = (a >> 8);
        header[22 + i] = a - (b << 8);
        a = b;
    }
    //now create file
    unsigned char *buffer = malloc(new_block_size * sizeof(unsigned char));
    if (buffer == NULL)
    {
        printf("Memory not allocated.\n");
        exit(1);
    }
    memcpy(buffer, header, 54);
    // pedding zeros
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
    //write to file
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

int safe_strtof(float *parameter, char *optarg)
{
    if (optarg[0] == '-' && strlen(optarg) == 1)
    {
        //error no parameter
        printf("read - invalid parameter\n");
        return 1;
    }
    *parameter = strtof(optarg, NULL);
    // avoid the input case: --r_start '0', tell the differences of '0' and convert-failing
    if (!(*parameter) && optarg[strlen(optarg) - 1] != '0')
    {
        //error
        return 1;
    }
    else
    {
        // parse ok
        return 0;
    }
}

//check if inputs are in unordered structure
int check_error(int *flags, struct option opts[])
{
    int non_empty_parameters = 1;
    for (size_t i = 0; i < 6; i++)
    {
        //only "a/b/c/d/r/o/h" are allowed
        if (!flags[i])
        {
            char tipp[] = " - wrong or not given";
            char *buff = malloc((strlen(opts[i].name) + strlen(tipp) + 1) * sizeof(char));
            if (buff == NULL)
            {
                printf("Memory not allocated.\n");
                exit(1);
            }
            strcat(buff, opts[i].name);
            strcat(buff, tipp);
            fprintf(stderr, "%s\n", buff);
            free(buff);
            return 1;
        }
        non_empty_parameters &= flags[i];
    }
    return 0;
}

//check if every input value is logical
int check_constaint(float r_start, float r_end, float i_start, float i_end, float res)
{
    if (r_start >= r_end)
    {
        fprintf(stderr, "r_start must be less than r_end\n");
        return 1;
    }
    if (i_start >= i_end)
    {
        fprintf(stderr, "i_start must be less than i_end\n");
        return 1;
    }
    if (res <= 0)
    {
        fprintf(stderr, "res must be greater than 0\n");
        return 1;
    }
    return 0;
}

void print_manual()
{
    fprintf(stdout, "usage: ./tricorn [options]\n"
               "options:\n"
               "-a, --r_start\t[float]\t-the value as lower bound of real number.\n"
               "-b, --r_end\t[float]\t-the value as upper bound of real number.\n"
               "-c, --i_start\t[float]\t-the value as lower bound of imaginary number.\n"
               "-d, --i_end\t[float]\t-the value as upper bound of imaginary number.\n"
               "-r, --res\t[float]\t-the value as resolution of the picture.\n"
               "-o, --output\t[string]-the path of the output image file.\n"
               "-h, --help\t\t-print the help message.\n"
               "example:\n"
               "tricorn --r_start -2 --r_end 2 --i_start -2 --i_end 2 --res 0.01 --output './output.bmp'\n");
}

//cause of character of float pointer value we need get a more accurate number,whenn a float is almost equal xx.5
int get_number(float x)
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
