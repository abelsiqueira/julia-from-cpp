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

int main() {
    int n = 100;

    jl_init();

    jl_value_t *array_type = jl_apply_array_type((jl_value_t *) jl_float64_type, 1);
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
        AData[i * n + i] = 2.0;
        if (i < n - 1)
            AData[(i + 1) * n + i] = 1.0;
        if (i > 0)
            AData[(i - 1) * n + i] = 1.0;
    }

    checked_eval_string("using LinearAlgebra");
    jl_function_t *dot = jl_get_function(jl_main_module, "dot");
    cout << jl_unbox_float64(jl_call2(dot, (jl_value_t *) x, (jl_value_t *) x)) << endl;

    jl_function_t *prod = jl_get_function(jl_base_module, "*");
    jl_array_t *Ax = (jl_array_t *) jl_call2(prod, (jl_value_t *) A, (jl_value_t *) x);

    double *AxData = (double *) jl_array_data(Ax);
    cout << "Ax =";
    for (size_t i = 0; i < n; i++) {
        cout << " " << AxData[i];
    }
    cout << endl;

    jl_function_t *lu_fact = jl_get_function(jl_main_module, "lu");
    check_julia_exception();
    jl_value_t *LU = jl_call1(lu_fact, (jl_value_t *) A);
    check_julia_exception();
    jl_function_t *ldiv = jl_get_function(jl_main_module, "ldiv!");
    jl_call3(ldiv, (jl_value_t *) y, LU, (jl_value_t *) Ax); // Solve y = A \ (Ax)
    check_julia_exception();

    double *yData = (double *) jl_array_data(y);
    double norm2 = 0.0;
    for (size_t i = 0; i < n; i++) {
        double dif = yData[i] - xData[i];
        norm2 += dif * dif;
    }
    cout << "|x - y|² = " << norm2 << endl;

    JL_GC_POP();

    check_julia_exception();

    jl_atexit_hook(0);

    return 0;
}
