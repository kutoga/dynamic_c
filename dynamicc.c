#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <dlfcn.h>
#include <errno.h>
#include <sys/wait.h>
#include <assert.h>
#include "dynamicc.h"

#define DC_C_FILE_SUFFIX                        ".c"
#define DC_DL_FILE_SUFFIX                       ".so"
#define DC_C_COMPILER                           "cc"
#define DC_TEMP_DIRECTORY                       "/tmp/"
#define DC_ARR_LENGTH(ARR)                      (sizeof(ARR) / sizeof(ARR[0]))

static inline int count_args(const char **args)
{
    int count = 0;
    while (args[count] != NULL)
    {
        ++count;
    }
    return count;
}

static inline void compile_c_to_dynamic_lib(const char *source_name, const char *dl_file, const char **args)
{
    const char *default_args[] = {"-O3", "-Wall", "-Wextra", "-Werror"};
    int args_count;
    if (args == NULL)
    {
        args = default_args;
        args_count = DC_ARR_LENGTH(default_args);
    }
    else
    {
        args_count = count_args(args);
    }
    const char *cc_base_args[] = {DC_C_COMPILER, source_name, "-o", dl_file, "-fPIC", "-shared"};
    const size_t cc_base_args_count = DC_ARR_LENGTH(cc_base_args);

    char *cc_args[cc_base_args_count + args_count + 1];
    size_t cc_args_index = 0;
    memcpy(&cc_args[cc_args_index], cc_base_args, sizeof(cc_base_args[0]) * cc_base_args_count);
    cc_args_index += cc_base_args_count;
    memcpy(&cc_args[cc_args_index], args, sizeof(args[0]) * args_count);
    cc_args_index += args_count;
    cc_args[cc_args_index] = NULL;
    ++cc_args_index;
    assert(DC_ARR_LENGTH(cc_args) == cc_args_index);

    const int cc_pid = fork();
    if (cc_pid == 0)
    {
        execvp(DC_C_COMPILER, cc_args);
    }
    else
    {
        waitpid(cc_pid, NULL, 0);
    }
}

bool dc_init(dynamic_c_t *dc)
{
    snprintf(dc->_c_filename, sizeof(dc->_c_filename), DC_TEMP_DIRECTORY "dc_XXXXXX" DC_C_FILE_SUFFIX);
    const int fd = mkstemps(dc->_c_filename, strlen(DC_C_FILE_SUFFIX));
    if (fd < 1)
    {
        return false;
    }

    dc->f = fdopen(fd, "w");
    return true;
}

void *dc_finish(dynamic_c_t *dc, const char **cc_args)
{
    fflush(dc->f);
    fclose(dc->f);

    char dl_file[128];
    snprintf(dl_file, sizeof(dl_file), DC_TEMP_DIRECTORY "dc_XXXXXX" DC_DL_FILE_SUFFIX);
    if (mkstemps(dl_file, strlen(DC_DL_FILE_SUFFIX)) < 1)
    {
        unlink(dc->_c_filename);
        return NULL;
    }

    compile_c_to_dynamic_lib(dc->_c_filename, dl_file, cc_args);
    unlink(dc->_c_filename);

    void *dl_handle = dlopen(dl_file, RTLD_LAZY);
    unlink(dl_file);

    return dl_handle;
}
