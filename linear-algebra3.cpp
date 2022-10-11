#include "aux.h"
#include <iostream>
#include <julia.h>

JULIA_DEFINE_FAST_TLS // only define this once, in an executable

using namespace std;

double starting_value(double x) {
    return 5 * exp(-50 * x * x);
}

int main() {
    int n = 1000;
    double NT = 100;
    double a = -1.0, b = 1.0;
    double u_a = 0.0, u_b = 1.0;
    double h = (b - a) / n;
    double delta = 0.01;
    double kappa = delta / h / h;

    long int rows[3 * n - 2], cols[3 * n - 2];
    double vals[3 * n - 2];

    for (size_t i = 0; i < n; i++) {
        rows[i] = i + 1;
        cols[i] = i + 1;
        vals[i] = (1 + 2 * kappa);
        if (i < n - 1) {
            rows[n + i] = i + 1;
            cols[n + i] = i + 2;
            vals[n + i] = -kappa;
            rows[2 * n + i - 1] = i + 2;
            cols[2 * n + i - 1] = i + 1;
            vals[2 * n + i - 1] = -kappa;
        }
    }

    jl_init();

    handle_eval_string("using LDLFactorizations, SparseArrays, Plots");

    jl_value_t *array_type = jl_apply_array_type((jl_value_t *) jl_float64_type, 1);
    jl_value_t *array_index_type = jl_apply_array_type((jl_value_t *) jl_int64_type, 1);
    jl_function_t *sparse = jl_get_function(jl_main_module, "sparse");
    handle_julia_exception();

    jl_value_t *A_sparse = jl_call3(
        sparse,
        (jl_value_t *) jl_ptr_to_array_1d(array_index_type, rows, 3 * n - 2, 0),
        (jl_value_t *) jl_ptr_to_array_1d(array_index_type, cols, 3 * n - 2, 0),
        (jl_value_t *) jl_ptr_to_array_1d(array_type, vals, 3 * n - 2, 0)
    );
    jl_array_t *u = jl_alloc_array_1d(array_type, n);
    jl_array_t *unew = jl_alloc_array_1d(array_type, n);
    JL_GC_PUSH3(&u, &unew, &A_sparse);

    double *uData = (double *) jl_array_data(u);
    double *unewData = (double *) jl_array_data(unew);

    for (size_t i = 0; i < n; i++)
        uData[i] = starting_value(a + (i + 1) * h);

    jl_function_t *plot = handle_eval_string("(args...) -> plot!(args...; leg=false)");
    jl_function_t *ldl = jl_get_function(jl_main_module, "ldl");
    jl_function_t *ldiv = jl_get_function(jl_main_module, "ldiv!");

    handle_eval_string("plot()");
    jl_call1(plot, (jl_value_t *) u);
    handle_julia_exception();

    jl_value_t *ldlObj = jl_call1(ldl, (jl_value_t *) A_sparse);
    {
        JL_GC_PUSH1(&ldlObj);
        handle_julia_exception();

        size_t time_interval = NT / 5;
        for (size_t k = 0; k < NT; k++) {
            jl_call3(ldiv, (jl_value_t *) unew, ldlObj, (jl_value_t *) u);
            handle_julia_exception();
            if (k % time_interval == 0 || k == NT - 1) {
                jl_call1(plot, (jl_value_t *) unew);
                handle_julia_exception();
            }
            for (size_t i = 0; i < n; i++)
                uData[i] = unewData[i];
        }
        JL_GC_POP();
    }

    handle_eval_string("png(\"linear-algebra3\")");

    JL_GC_POP();

    handle_julia_exception();

    jl_atexit_hook(0);

    return 0;
}
