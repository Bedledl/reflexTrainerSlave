#ifndef PRINT_H
#define PRINT_H

#ifndef DEBUG
#include <stdio.h>
#include <stdint.h>

#define print printf
union uint64divided
{
    uint64_t the_long_long;
    uint32_t the_longs[2];
};

void printuint64(uint64_t number);
#else
#define print (void)
#define printuint64 (void)
#endif

#endif // PRINT_H