#include "aux.h"
#include <julia.h>
#include <iostream>
JULIA_DEFINE_FAST_TLS // only define this once, in an executable (not in a shared library) if you want fast code.

using namespace std;

int main() {
    jl_init();

    jl_function_t *sqrt = jl_get_function(jl_base_module, "sqrt");
    jl_value_t *x = jl_call1(sqrt, jl_box_float64(-1.0));
    handle_julia_exception();
    double xValue = jl_unbox_float64(x);
    cout << xValue << endl;

    jl_atexit_hook(0);
    return 0;
}
