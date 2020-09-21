#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "dynamicc.h"

static bool dc_get_filename(dynamic_c_t *target)
{
    snprintf(target->_filename, sizeof(target->_filename), "/tmp/dc_XXXXXX");
    target->f = (FILE *)(long)mkstemp(target->_filename);
    return target->f != NULL;
}


dynamic_c_t dc_init()
{
    dynamic_c_t dc = {};
    if (!dc_get_filename(&dc)) {
        fprintf(stderr, "Cannot get a temp. file!");
        exit(1);
    }
    else {
        return dc;
    }
}

void *dc_finish(dynamic_c_t *dc)
{
    fclose(dc->f);
    char so_file[sizeof(dc->_filename) + 10];
    snprintf(so_file, sizeof(so_file), "%s.so", dc->_filename);
    execl("gcc", "-c", dc->_filename, "-o", so_file, "-O3", "-fPIC", "-lshared", NULL);
}
