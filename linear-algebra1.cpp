#include "aux.h"
#include <iostream>
#include <julia.h>

JULIA_DEFINE_FAST_TLS // only define this once, in an executable

using namespace std;

int main() {
    int n = 100;

    jl_init();

    jl_value_t *array_type = jl_apply_array_type((jl_value_t *)
    jl_float64_type, 1);
    jl_value_t *matrix_type = jl_apply_array_type((jl_value_t *) jl_float64_type, 2);
    jl_array_t *x = jl_alloc_array_1d(array_type, n);
    jl_array_t *A = jl_alloc_array_2d(matrix_type, n, n);
    jl_array_t *y = jl_alloc_array_1d(array_type, n);

    JL_GC_PUSH3(&x, &A, &y);

    double *xData = (double *) jl_array_data(x);
    double *AData = (double *) jl_array_data(A);

    for (size_t i = 0; i < jl_array_len(x); i++) {
        xData[i] = 1.0;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < i; j++)
            AData[i + j * n] = 1.0;
        AData[i + i * n] = n;
        for (size_t j = i + 1; j < n; j++)
            AData[i + j * n] = -1.0;
    }

    handle_eval_string("using LinearAlgebra");
    jl_function_t *prod = jl_get_function(jl_base_module, "*");
    jl_array_t *Ax = (jl_array_t *) jl_call2(prod, (jl_value_t *) A, (jl_value_t *) x);

    double *AxData = (double *) jl_array_data(Ax);
    cout << "Ax =";
    for (size_t i = 0; i < n; i++) {
        cout << " " << AxData[i];
    }
    cout << endl;

    for (size_t i = 0; i < n; i++) {
        xData[i] = i + 1;
    }
    prod = jl_get_function(jl_main_module, "mul!");
    jl_call3(prod, (jl_value_t *) Ax, (jl_value_t *) A, (jl_value_t *) x);
    cout << "Ax =";
    for (size_t i = 0; i < n; i++) {
        cout << " " << AxData[i];
    }
    cout << endl;

    jl_function_t *lu_fact = jl_get_function(jl_main_module, "lu");
    handle_julia_exception();
    jl_value_t *LU = jl_call1(lu_fact, (jl_value_t *) A);
    handle_julia_exception();
    jl_function_t *ldiv = jl_get_function(jl_main_module, "ldiv!");
    jl_call3(ldiv, (jl_value_t *) y, LU, (jl_value_t *) Ax); // Solve y = A \ (Ax)
    handle_julia_exception();

    double *yData = (double *) jl_array_data(y);
    double norm2 = 0.0;
    for (size_t i = 0; i < n; i++) {
        double dif = yData[i] - xData[i];
        norm2 += dif * dif;
    }
    cout << "|x - y|² = " << norm2 << endl;

    JL_GC_POP();

    handle_julia_exception();

    jl_atexit_hook(0);

    return 0;
}
