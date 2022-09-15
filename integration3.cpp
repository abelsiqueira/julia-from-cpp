#include "aux.h"
#include <julia.h>
#include <cmath>
#include <iostream>
JULIA_DEFINE_FAST_TLS // only define this once, in an executable (not in a shared library) if you want fast code.

using namespace std;

int main() {
    jl_init();

    handle_eval_string("include(\"aux.jl\")");
    jl_function_t *trapezoid = jl_get_function(jl_main_module, "trapezoid");
    handle_julia_exception();
    handle_eval_string("using QuadGK");
    jl_value_t *integrator = handle_eval_string("(f, a, b, n) -> quadgk(f, a, b, maxevals=n)[1]");
    handle_julia_exception();
    jl_function_t *foo = handle_eval_string("x -> 1 / (1 + x^2)");

    jl_value_t *args[4] = { foo, jl_box_float64(0.0), jl_box_float64(1.0), jl_box_int64(100) };
    jl_function_t *integrators[2] = { trapezoid, integrator };

    for (size_t i = 0; i < 2; i++) {
        jl_value_t *I1 = jl_call(integrators[i], args, 4);
        handle_julia_exception();
        double IVal = jl_unbox_float64(I1);
        cout << "Integral of 1 / (1 + x^2) is approx: " << IVal << endl;
        cout << "  Error: " << M_PI / 4 - IVal << endl;
    }

    jl_atexit_hook(0);
    return 0;
}
