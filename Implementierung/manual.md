**USAGE** 
------
tricorn [options]

**OPTIONS**
------
-a, --r_start float      -put a floating point value as lower bound of real number.It shouldn't greater than *r_end*. 

-b, --r_end float    -put a floating point value as upper bound of real number.It shouldn't lesser than *r_start*.

-c, --i_start float      -put a floating point value as lower bound of imaginary number.It shouldn't greater than *i_end*.

-d, --i_end float    -put a floating point value as upper bound of imaginary number.It shouldn't lesser than *i_start*.

-r, --res float      -put a floating point value as precision for pixel

-o, --output location    -put an adress in,which must have enough places for memory.

-h, --help    -Print this message and exit


**EXAMPLE**
---
./debug.out --r_start 3.5 --r_end 6.5 --i_start 1.7 --i_end 2.3 --res 0.7 --output './opop'
