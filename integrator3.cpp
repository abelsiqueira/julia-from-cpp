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

int main() {
    int n = 100000;

    jl_init();

    checked_eval_string("include(\"aux.jl\")");
    jl_function_t *integrator = jl_get_function(jl_main_module, "trapezoid"); // Note main instead of base
    jl_function_t *my_c_func_ptr = checked_eval_string(
        "x -> ccall((:my_c_func, \"libmy_c_func.so\"), Cdouble, (Cdouble,), x)"
    );
    cout << jl_unbox_float64(jl_call1(my_c_func_ptr, jl_box_float64(0.3))) << endl;
    cout << "Importing the Julia package" << endl;
    checked_eval_string(
        "using Distributions"
    );
    cout << "Defining the cdf function" << endl;
    jl_value_t *normal = checked_eval_string("Normal()");
    jl_function_t *cdf = jl_get_function(jl_main_module, "cdf");

    size_t N = 6;
    for (size_t i = 0; i <= N; i++) {
        double endpoint = -3.0 + i * 6.0 / N;
        jl_value_t *args[4] = { my_c_func_ptr, jl_box_float64(-6.0), jl_box_float64(endpoint), jl_box_int32(n) };
        jl_value_t *I = jl_call(integrator, args, 4);
        jl_value_t *I_cdf = jl_call2(cdf, normal, jl_box_float64(endpoint));

        cout << "Prob(Normal(0, 1) <= " << endpoint << ") ~ " << jl_unbox_float64(I) << endl;
        cout << "   cdf(Normal(0, 1), " << endpoint << ") = " << jl_unbox_float64(I_cdf) << endl;
    }

    // Exception in Julia
    if (jl_exception_occurred()) {
        jl_printf(jl_stderr_stream(), "Exception: ");
        jl_call2(jl_get_function(jl_base_module, "showerror"),
            jl_stderr_obj(),
            jl_exception_occurred()
        );
        jl_printf(jl_stderr_stream(), "\n");
    }

    jl_atexit_hook(0);

    return 0;
}
