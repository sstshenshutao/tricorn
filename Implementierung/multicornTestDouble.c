#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
/*
    this is the test double for the output
*/
void readFile(unsigned char *ret)
{
    unsigned char buf[1500000];
    size_t nread;
    FILE *file = fopen("sample.change", "r");
    if (file)
    {
        nread = fread(buf, 1, sizeof buf, file);
        if (nread > 0)
        {
            printf("nread:%zu\n", nread);
        }
        else
        {
            printf("wrong:%zu\n", nread);
        }
        for (size_t i = 2997; i < 2997+50; i++)
        {
            printf("%x,", buf[i]);
        }

        printf("\n");
        fclose(file);
    }
    for (size_t i = 0; i < 1500000; i++)
    {
        ret[i] = buf[i];
    }
}
void multicorn(float r_start, float r_end, float i_start, float i_end, float res, unsigned char *img)
{

    readFile(img);
    // unsigned char buff[1500000] ={}
    // img
}
