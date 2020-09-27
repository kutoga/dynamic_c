#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <dlfcn.h>
#include <errno.h>
#include <sys/wait.h>
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
        fprintf(stderr, "Cannot create a temp. C-code file!");
        exit(1);
    }
    else {
        return dc;
    }
}

void *dc_finish(dynamic_c_t *dc, const char **cc_args)
{
    fflush(dc->f);
    fclose(dc->f);

    char so_name[128];
    snprintf(so_name, sizeof(so_name), "/tmp/dc_XXXXXX.so");
    if (mkstemps(so_name, 3) < 1) {
        unlink(dc->_filename);
        return NULL;
    }

    int pid = fork();
    if (pid == 0) {
        printf("%s\n", dc->_filename);
        char *args[] = {"cc", dc->_filename, "-o", so_name, "-Ofast", "-Wall", "-Wextra", "-fPIC", "-shared", NULL};
        execvp("cc", args);
        exit(1);
    }
    else {
        int status;
        waitpid(pid, &status, 0);
    }

    unlink(dc->_filename);

    void *so_handle = dlopen(so_name, RTLD_LAZY);
    unlink(so_name);
    return so_handle;
}
