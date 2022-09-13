#include "aux.h"

void handle_exception(void) {
    if (jl_value_t *ex = jl_exception_occurred()) {
        jl_printf(jl_stderr_stream(), "Exception:\n");
        jl_call2(
            jl_get_function(jl_base_module, "showerror"),
            jl_stderr_obj(),
            ex
        );
        jl_printf(jl_stderr_stream(), "\nI quit!\n");
        jl_atexit_hook(1);
        exit(1);
    }
}

jl_value_t *handle_eval_string(const char* code) {
    jl_value_t *result = jl_eval_string(code);
    handle_exception();
    assert(result && "Missing return value but no exception occurred!");
    return result;
}
