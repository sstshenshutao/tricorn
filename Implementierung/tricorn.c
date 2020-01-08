//todos: change calculate_block_len, finish all the todos, check free, check under_score_case
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
extern void multicorn(float r_start, float r_end, float i_start, float i_end, float res, unsigned char *img);
void parse_arg(int argc, char **argv);
void print_manual();
int check_error(int *flags, struct option opts[]);
int safe_strtof(float *parameter, char *optarg);
void write_out_file(int block_len,unsigned char *img,char* output_path);
int calculate_block_len(float res);
#define A_START -2
#define A_END 1
#define B_START -1
#define B_END 1

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
            break;
        default:
            // error: invalid parameters:
            // fprintf(stderr, "\e[1;31m"
            printf("defualt");
            print_manual();
            exit(1);
            break;
        }
    }
    if (check_error(flags, opts) != 0)
    {
        print_manual();
        exit(1);
    }
    //the call-part
    printf("r_start:%.3f\n", r_start);
    printf("r_end:%.3f\n", r_end);
    printf("i_start:%.3f\n", i_start);
    printf("i_end:%.3f\n", i_end);
    printf("res:%.3f\n", res);
    printf("output:%s\n", output);
    // malloc the space for the img buffer.
    int block_len= calculate_block_len(res);
    unsigned char *img= malloc(block_len*3*sizeof(unsigned char));
    if (img == NULL)
    {
        printf("Memory not allocated.\n");
        exit(1);
    }
    multicorn(r_start,r_end,i_start,i_end,res,img);
    write_out_file(block_len,img,output);
    free(img);
    free(flags);
}
int calculate_block_len(float res){
    // float a_len = A_END-A_START;
    // float b_len = B_END-B_START;
    // int width=(int)(a_len/res)+1;
    // int height= (int)(b_len/res)+1;
    // return width*height;
    return 500000;
}
void write_out_file(int block_len,unsigned char *img,char* output_path){
    unsigned char header[54]={0x42,0x4d,0x96,0xe3,0x16,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0xe8,0x03,0x00,0x00,0xf4,0x01,0x00,0x00,0x01,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0x0b,0x00,0x00,0x12,0x0b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    int block_size = block_len*3+54;
    if (block_size>0xffffffff){
        printf("file is too big to save in .bmp\n");
        exit(1);
    }
    int a = block_size;
    int b;
    for (size_t i = 0; i < 4; i++)
    {
        b= (a>>8);
        header[2+i]=a-(b<<8);
        a=b;
    }
    FILE *f = fopen(output_path, "w");
    if (f == NULL)
    {
        printf("error in opening the output file: %s\n",output_path);
        exit(1);
    }
    unsigned char* buffer=  malloc(block_size * sizeof(unsigned char));
    memcpy(buffer,header,54);
    memcpy(buffer+54,img,(block_size-54)* sizeof(unsigned char));
    fwrite (buffer , sizeof(unsigned char), block_size, f);
    fclose(f);
    free(buffer);
}

int safe_strtof(float *parameter, char *optarg)
{
    if (optarg[0]=='-'){
        //error no parameter
        return 1;
    }
    *parameter = strtof(optarg, NULL);
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

int check_error(int *flags, struct option opts[])
{
    int non_empty_parameters = 1;
    for (size_t i = 0; i < 6; i++)
    {
        if (!flags[i])
        {
            char *tipp = " - wrong or not given";
            char *buff = malloc((strlen(opts[i].name) + strlen(tipp) + 1) * sizeof(char));
            strcat(buff, opts[i].name);
            strcat(buff, tipp);
            fprintf(stderr, "%s\n", buff);
            free(buff);
            return 1;
        }
        non_empty_parameters &= flags[i];
    }
    // -h duplicated
    if (flags[6] & non_empty_parameters)
    {
        fprintf(stderr, "the 'help' option is duplicated here\n");
        return 1;
    }
    return 0;
}

void print_manual()
{
    fprintf(stderr, "doc:\n");
}