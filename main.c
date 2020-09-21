#include "dynamicc.h"

int main()
{
    dynamic_c_t cc = dc_init();
    fprintf(cc.f, "int f() {");
    fprintf(cc.f, "    return 0;");
    fprintf(cc.f, "}");
    void *so_handle = dc_finish(&cc);
}