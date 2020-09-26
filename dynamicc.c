#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <dlfcn.h>
#include <errno.h>
#include "dynamicc.h"

static bool dc_get_filename(dynamic_c_t *target)
{
    snprintf(target->_filename, sizeof(target->_filename), "/tmp/dc_XXXXXX.c");
    const int fd = mkstemps(target->_filename, 2);
    if (fd < 1) {
        return false;
    }

    target->f = fdopen(fd, "w");
    return true;
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
    fflush(dc->f);
    fclose(dc->f);

    char so_name[128];
    snprintf(so_name, sizeof(so_name), "/tmp/dc_XXXXXX.so");
    if (mkstemps(so_name, 3) < 1) {
        unlink(dc->_filename);
        return NULL;
    }


    char command[256];
    // TODO: Escaping etc.
    snprintf(command, sizeof(command), "gcc %s -o %s -Ofast -Wall -Werror -Wextra -fPIC -shared", dc->_filename, so_name);

    int err = system(command);
    unlink(dc->_filename);
    printf("err %d\n", err);

    printf("created %s\n", so_name);
    void *so_handle = dlopen(so_name, RTLD_LAZY);
    unlink(so_name);
    printf("handle: %p\n", so_handle);
    return so_handle;
}
