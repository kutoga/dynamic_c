# DynamicC

DynamicC is a simple and stupid library with just two API calls (`dc_init` and `dc_finish`). This lib can be used
to compile and use C code at runtime. This might heavily improve the performance of some
code (compared to a generic programmed solution). **NOTE: This should only be done in edge cases
and after a lot of profiling, because it can make testing harder and adds a runtime-dependency for
a C-compiler.**

How to use it? Just add `dynamic_c.c` and `dynamic_c.h` to your project and use it.
It is free to use and you do not even have to mention the author of the library.

The function `dc_finish` accepts C-compiler arguments as a second parameter. If it
is `NULL`, then `-O3 -march=native -Wall -Wextra -Werror` is used. Otherwise it is expected that
the arguments are followed by `NULL` to indicate the end of the argument list.

Example (execute `make && ./main`):

```c
#include "dynamic_c.h"

static int my_value = 0;

typedef void (*f_t)(int n);

int main()
{
    /* Write C-code. */
    dynamic_c_t dc;
    if (!dc_init(&dc))
    {
        fprintf(stderr, "Cannot initialize dynamic C!\n");
        return 1;
    }

    // Writing code using DC_CODE. NOTE: "#inlcude"s cannot be used inside DC_CODE:
    fprintf(dc.f, "#include <stdio.h>\n");
    fprintf(dc.f, DC_CODE(

        /*
         * This code is compiled at runtime
         */

        static int compute_something(int n)
        {
            int res = 0;
            for (int i = 0; i < n; ++i) {
                res += n;
                res = res | 1;
            }
            return res;
        }

        void f(int n)
        {
            printf("n=%%d\n", n);
            *(int *)%ld += compute_something(n);
        }

    ), (long)&my_value);

    // Alternative:
    // fprintf(dc.f, "#include <stdio.h>                              \n");
    // fprintf(dc.f, "static int compute_something(int n)             \n");
    // fprintf(dc.f, "{                                               \n");
    // fprintf(dc.f, "    int res = 0;                                \n");
    // fprintf(dc.f, "    for (int i = 0; i < n; ++i) {               \n");
    // fprintf(dc.f, "        res += n;                               \n");
    // fprintf(dc.f, "        res = res | 1;                          \n");
    // fprintf(dc.f, "    }                                           \n");
    // fprintf(dc.f, "    return res;                                 \n");
    // fprintf(dc.f, "}                                               \n");
    // fprintf(dc.f, "void f(int n) {                                 \n");
    // fprintf(dc.f, "    printf(\"n=%%d\\n\", n);                    \n");
    // fprintf(dc.f, "    *(int *)%ld += compute_something(n);        \n", (long)&my_value);
    // fprintf(dc.f, "}                                               \n");

    void *dl_handle = dc_finish(&dc, NULL);
    // const char *args[] = {"-O0", "-g3", NULL};
    // void *dl_handle = dc_finish(&dc, args);
    if (dl_handle == NULL)
    {
        fprintf(stderr, "Cannot create dynamic linked library!\n");
        return 1;
    }

    /* Get the function pointer to the compiled functin "f" */
    f_t f = dlsym(dl_handle, "f");

    /* Use the function. */
    for (int i = 0; i < 5; ++i)
    {
        f(i);
        printf("my_value = %d\n", my_value);
    }

    /* Finally, close the dynamic linked library */
    dlclose(dl_handle);
}
```
