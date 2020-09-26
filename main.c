#include "dynamicc.h"

int main()
{
    dynamic_c_t cc = dc_init();
    fprintf(cc.f, "#include <stdio.h>\n");
    fprintf(cc.f, "int f() {");
    fprintf(cc.f, "    printf(\"hello world!\\n\");");
    fprintf(cc.f, "    return 0;");
    fprintf(cc.f, "}");
    void *so_handle = dc_finish(&cc);

    int (*f)() =  dlsym(so_handle, "f");
    printf("%p\n", f);
    f();

    dlclose(so_handle);
}