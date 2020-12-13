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