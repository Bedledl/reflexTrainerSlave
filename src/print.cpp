#ifndef DEBUG
#include <stdint.h>
#include "print.h"

void printuint64(uint64_t number)
{
    uint64divided combined;
    combined.the_long_long = number;
    print("%8lu%8lu \n", combined.the_longs[1], combined.the_longs[0]);
}
#endif