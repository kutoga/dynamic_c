#ifndef __DYNAMIC_C__
#define __DYNAMIC_C__

#include <stdio.h>
#include <stdbool.h>
#include <dlfcn.h>

typedef struct {
    FILE *f;
    char _c_filename[64];
} dynamic_c_t;

bool dc_init(dynamic_c_t *dc);

/*
 * Compile the code and return a handle for the created dynamic library.
 * If "optional_args" is NULL, the arguments "-O3 -Wall -Wextra -Werror" are passed to
 * the C-compiler. Otherwise it is expected that "optional_args" contains compiler
 * arguments plus a final NULL value which indicates the end.
 */
void *dc_finish(dynamic_c_t *dc, const char **optional_args);

#endif
