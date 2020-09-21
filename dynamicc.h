#ifndef __DYNAMIC_C__
#define __DYNAMIC_C__

#include <stdio.h>

typedef struct {
    FILE *f;
    char _filename[64];
} dynamic_c_t;

dynamic_c_t dc_init();

void *dc_finish(dynamic_c_t *dc);

#endif
