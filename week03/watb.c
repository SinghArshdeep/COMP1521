// COMP1521 19t2 ... lab 03: where are the bits?
// watb.c: determine bit-field order

#include <stdio.h>
#include <stdlib.h>

struct _bit_fields {
	unsigned int a : 4;
	unsigned int b : 8;
	unsigned int c : 20;
};

union bits {
    struct _bit_fields x;
    unsigned int y;
};

int main (void)
{
    union bits b;
    b.x.a = 0x0;
    b.x.b = 0x00;
    b.x.c = 0xfffff;
    
    printf ("%#010x\n", b.y);

	return EXIT_SUCCESS;
}
