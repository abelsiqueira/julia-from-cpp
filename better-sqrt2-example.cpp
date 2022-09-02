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
    cout << "Testing Julia from C++" << endl;

    jl_init();

    checked_eval_string("include(\"aux.jl\")");
    jl_function_t *sqrt = jl_get_function(jl_main_module, "my_sqrt"); // Note main instead of base
    for (double x = 2.0; x <= 4.0; x += 0.1) {
        jl_value_t *argument = jl_box_float64(x);
        jl_value_t *ret = jl_call1(sqrt, argument);
        cout << "sqrt of " << x << " is " << jl_unbox_float64(ret) << endl;
    }

    // Exception in Julia
    jl_call1(sqrt, jl_box_float64(-1.0));
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
