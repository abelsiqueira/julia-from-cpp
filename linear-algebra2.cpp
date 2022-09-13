#include "aux.h"
#include <iostream>
#include <julia.h>

JULIA_DEFINE_FAST_TLS // only define this once, in an executable

using namespace std;

double poisson_f(double x) {
    return 5 * exp(-5 * x * x);
}

int main() {
    int n = 100;
    double a = -1.0, b = 1.0;
    double u_a = 0.0, u_b = 1.0;
    double h = (b - a) / n;

    /* Discretized Poisson equation
    ** uₓₓ = 5 * exp(-5x²), u(-1) = 0, u(b) = 1
    ** x⁰ = -1, xⁿ⁺¹ = 1, xⁱ = x⁰ + ih, h = (xⁿ⁺¹ - x⁰) / (n + 1)
    ** vⁱ ≈ u(xⁱ)
    ** uₓₓ(xⁱ) ≈ (vⁱ⁻¹ - 2vⁱ + vⁱ⁺¹) / h²
    ** Equation:                -vⁱ⁻¹ + 2vⁱ - vⁱ⁺¹ = h² × exp(-(xⁱ)²)
    ** At i = 0: v⁰ = 0 ⇒               2v¹ - v²   = h² × exp(-(x¹)²)
    ** At i = n+1: vⁿ⁺¹ = 1 ⇒   -vⁿ⁻¹ + 2vⁿ        = h² × exp(-(xⁿ)²) + 1
    */

    jl_init();

    jl_value_t *array_type = jl_apply_array_type((jl_value_t *) jl_float64_type, 1);
    jl_value_t *matrix_type = jl_apply_array_type((jl_value_t *) jl_float64_type, 2);
    jl_array_t *rhs = jl_alloc_array_1d(array_type, n);
    jl_array_t *A = jl_alloc_array_2d(matrix_type, n, n);
    jl_array_t *u = jl_alloc_array_1d(array_type, n);

    JL_GC_PUSH3(&rhs, &A, &u);

    double *rhsData = (double *) jl_array_data(rhs);
    double *AData = (double *) jl_array_data(A);

    for (size_t i = 0; i < n; i++) {
        rhsData[i] = h * h * poisson_f(a + (i + 1) * h);
        if (i == 0) {
            rhsData[i] += u_a;
        } else if (i == n - 1) {
            rhsData[i] += u_b;
        }
    }

    for (size_t i = 0; i < n; i++) {
        AData[i * n + i] = 2.0;
        if (i < n - 1)
            AData[(i + 1) * n + i] = -1.0;
        if (i > 0)
            AData[(i - 1) * n + i] = -1.0;
    }

    handle_eval_string("using LinearAlgebra");
    jl_function_t *chol_fact = jl_get_function(jl_main_module, "cholesky");
    handle_julia_exception();
    jl_value_t *CholObj = jl_call1(chol_fact, (jl_value_t *) A);
    {
        JL_GC_PUSH1(&CholObj);
        handle_julia_exception();
        jl_function_t *ldiv = jl_get_function(jl_main_module, "ldiv!");
        jl_call3(ldiv, (jl_value_t *) u, CholObj, (jl_value_t *) rhs);
        handle_julia_exception();
        JL_GC_POP();
    }

    cout << "Plotting... this may take a while" << endl;
    handle_eval_string("using Plots");
    jl_function_t *plot = jl_get_function(jl_main_module, "plot");
    jl_call1(plot, (jl_value_t *) u);
    handle_eval_string("png(\"linear-algebra2\")");

    JL_GC_POP();

    handle_julia_exception();

    jl_atexit_hook(0);

    return 0;
}
