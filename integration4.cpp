#include "aux.h"
#include <julia.h>
#include <cmath>
#include <iostream>
#include <iomanip>
JULIA_DEFINE_FAST_TLS // only define this once, in an executable (not in a shared library) if you want fast code.

using namespace std;

int main() {
    jl_init();

    handle_julia_exception();
    handle_eval_string("using Distributions, QuadGK");
    jl_value_t *integrator = handle_eval_string("(f, a, b) -> quadgk(f, a, b)[1]");
    handle_julia_exception();

    handle_eval_string("normal = Normal()");
    jl_function_t *pdf = handle_eval_string("x -> pdf(normal, x)");
    jl_function_t *cdf = handle_eval_string("x -> cdf(normal, x)");

    for (double xVal = -4.0; xVal <= 4.0; xVal += 1.0) {
        jl_value_t *x = jl_box_float64(xVal);
        double pdfVal = jl_unbox_float64(jl_call1(pdf, x));
        double cdfVal = jl_unbox_float64(jl_call1(cdf, x));

        jl_value_t *I = jl_call3(integrator, pdf, jl_box_float64(-16.0), x);
        handle_julia_exception();
        double IVal = jl_unbox_float64(I);
        cout << std::scientific << std::setprecision(2) << std::showpos
             << "x: " << xVal
             << "  pdf: " << pdfVal
             << "  cdf: " << cdfVal
             << "  cdf - integral: " << cdfVal - IVal << endl;
    }

    jl_atexit_hook(0);
    return 0;
}
