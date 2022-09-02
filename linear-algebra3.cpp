#include <iostream>
#include <julia.h>

JULIA_DEFINE_FAST_TLS // only define this once, in an executable

using namespace std;

jl_value_t *checked_eval_string(const char* code)
{
    jl_value_t *result = jl_eval_string(code);
    if (jl_exception_occurred()) {
        // none of these allocate, so a gc-root (JL_GC_PUSH) is not necessary
        jl_call2(jl_get_function(jl_base_module, "showerror"),
                 jl_stderr_obj(),
                 jl_exception_occurred());
        jl_printf(jl_stderr_stream(), "\n");
        jl_atexit_hook(1);
        exit(1);
    }
    assert(result && "Missing return value but no exception occurred!");
    return result;
}

int check_julia_exception()  {
    if (jl_exception_occurred()) {
        jl_printf(jl_stderr_stream(), "Exception: ");
        jl_call2(jl_get_function(jl_base_module, "showerror"),
            jl_stderr_obj(),
            jl_exception_occurred()
        );
        jl_printf(jl_stderr_stream(), "\n");
        jl_atexit_hook(1);
        exit(1);
    }
    return 0;
}

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

    long int rows[3 * n - 2], cols[3 * n - 2];
    double vals[3 * n - 2];

    for (size_t i = 0; i < n; i++) {
        rows[i] = i + 1;
        cols[i] = i + 1;
        vals[i] = 2.0;
        if (i < n - 1) {
            rows[n + i] = i + 1;
            cols[n + i] = i + 2;
            vals[n + i] = -1.0;
            rows[2 * n + i - 1] = i + 2;
            cols[2 * n + i - 1] = i + 1;
            vals[2 * n + i - 1] = -1.0;
        }
    }

    jl_init();

    jl_value_t *array_type = jl_apply_array_type((jl_value_t *) jl_float64_type, 1);
    jl_array_t *rhs = jl_alloc_array_1d(array_type, n);
    jl_array_t *u = jl_alloc_array_1d(array_type, n);

    JL_GC_PUSH2(&rhs, &u);

    double *rhsData = (double *) jl_array_data(rhs);

    for (size_t i = 0; i < n; i++) {
        rhsData[i] = h * h * poisson_f(a + (i + 1) * h);
        if (i == 0) {
            rhsData[i] += u_a;
        } else if (i == n - 1) {
            rhsData[i] += u_b;
        }
    }

    checked_eval_string("using SparseArrays");
    jl_function_t *sparse = jl_get_function(jl_main_module, "sparse");
    jl_value_t *array_index_type = jl_apply_array_type((jl_value_t *) jl_int64_type, 1);
    jl_value_t *A_sparse = jl_call3(
        sparse,
        (jl_value_t *) jl_ptr_to_array_1d(array_index_type, rows, 3 * n - 2, 0),
        (jl_value_t *) jl_ptr_to_array_1d(array_index_type, cols, 3 * n - 2, 0),
        (jl_value_t *) jl_ptr_to_array_1d(array_type, vals, 3 * n - 2, 0)
    );

    checked_eval_string("using LDLFactorizations");
    jl_function_t *ldl = jl_get_function(jl_main_module, "ldl");
    check_julia_exception();
    jl_value_t *ldlObj = jl_call1(ldl, (jl_value_t *) A_sparse);
    {
        JL_GC_PUSH1(&ldlObj);
        check_julia_exception();
        jl_function_t *ldiv = jl_get_function(jl_main_module, "ldiv!");
        jl_call3(ldiv, (jl_value_t *) u, ldlObj, (jl_value_t *) rhs);
        check_julia_exception();
        JL_GC_POP();
    }

    cout << "Plotting... this may take a while" << endl;
    checked_eval_string("using Plots");
    jl_function_t *plot = jl_get_function(jl_main_module, "plot");
    jl_call1(plot, (jl_value_t *) u);
    checked_eval_string("png(\"linear-algebra3\")");

    JL_GC_POP();

    check_julia_exception();

    jl_atexit_hook(0);

    return 0;
}
