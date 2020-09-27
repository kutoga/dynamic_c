# DynamicC

DynamicC is a simple and stupid library with just two API calls (`dc_init` and `dc_finish`). This lib can be used
to compile code at runtime. This might heavily improve the performance of some
code (compared to a generic programmed solution). This should only be done in edge cases
and after a lot of profiling.

How to use it? Just add `dynamicc.c` and `dynamicc.h` to your project and use it.
It is free to use and you do not even have to mention the author of the library.

Example (execute `make && ./main`):

```c
#include "dynamicc.h"

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
    fprintf(dc.f, "void f(int n) {");
    fprintf(dc.f, "    *(int *)%ld += n;", (long)&my_value);
    fprintf(dc.f, "}");
    void *dl_handle = dc_finish(&dc, NULL);
    if (dl_handle == NULL)
    {
        fprintf(stderr, "Cannot create dynamic linked library!\n");
        return 1;
    }

    /* Get the fnction pointer to the compiled functin "f" */
    f_t f = dlsym(dl_handle, "f");

    for (int i = 0; i < 5; ++i)
    {
        f(i);
        printf("my_value = %d\n", my_value);
    }

    /* Finally, close the dynamic linked library */
    dlclose(dl_handle);
}
```