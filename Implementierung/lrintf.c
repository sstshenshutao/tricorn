#include <stdio.h>
#include <math.h>
int get_number(float x);
int main(){
	float a= 0.99999999999999999999999999999999999999990000;
	printf ("a%ld\n",lrintf(3.49999999999999999999999999999999999999-a));	
	printf ("b%f\n",3.49999999999999999999999999999999999999-a);	
	printf ("c%d\n",get_number(3.499999999999999999999999999999999999999999-a));
	printf ("d%d\n",get_number(3.5000000000000000000000000000000));
	printf ("e%d\n",get_number(3.999999999999999999999999999999999999999999));
	printf ("f%d\n",get_number(3.899999999999999999999999999999999999999999));
	printf ("g%d\n",get_number(4.00000000000000000000000000000000000000001));
	printf ("h%d\n",get_number(4.099999999999999999999999999999999999999999));
	return 0;
//a2
//b2.500000
//c3
//d4
//e5
//f4
//g5
//h5`
}
int get_number(float x)
{
    // the value int(x) is only not exact at xxx.999999
    // so we use the following mechanism to avoid it:
    // round_x - x <0.1  Yes? means the value is about x.99999
    int ret = 0;
    int round_x = lrintf(x);
    float diff = round_x - x;
    if (diff > 0 && diff < 0.1)
    {
        //use the value round_x - 1
        ret = round_x;
    }
    else
    {
        //directly cast
        ret = (int)x + 1;
    }
    return ret;
}
