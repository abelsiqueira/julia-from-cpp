#include "aux.h"
#include <julia.h>
#include <iostream>
JULIA_DEFINE_FAST_TLS // only define this once, in an executable (not in a shared library) if you want fast code.

using namespace std;

int main() {
    jl_init();

    handle_eval_string("include(\"aux.jl\")");
    jl_function_t *trapezoid = jl_get_function(jl_main_module, "trapezoid");
    handle_julia_exception();
    jl_function_t *foo = handle_eval_string(
        "x -> ccall((:my_c_func, \"libmy_c_func.so\"), Cdouble, (Cdouble,), x)"
    );
    handle_julia_exception();

    jl_value_t *args[4] = { foo, jl_box_float64(0.0), jl_box_float64(1.0), jl_box_int64(100) };
    jl_value_t *I = jl_call(trapezoid, args, 4);
    double IVal = jl_unbox_float64(I);
    cout << "Integral of x^3 is approx: " << IVal << endl;
    cout << "  Error: " << 1.0 / 4.0 - IVal << endl;

    jl_atexit_hook(0);
    return 0;
}
