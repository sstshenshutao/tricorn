# arguments
NAME
---
    multicorn
SYNOPSIS
---
    void multicorn(float r_start, float r_end, float i_start, float i_end, float res, unsigned char *img);
DESCRIPTION
---
The function **multicorn** calculates all points like (a,b) iterative, where a is in [-2,1] and b is in [-1,1], every time after a test on the Boundary of point it stores count of a pixel on 24-bit BMP-format from testrusult into address.

--r_start: float    
--r_end: float      
--i_start: float    
--i_end: float      
--res: float -r
--output: location -o
--help:  -h

EXAMPLE
---
--r_start 3.5 --r_end 6.5 --i_start 1.7 --i_end 2.3 --res 0.7 --output './opop'
