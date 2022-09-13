#ifndef MY_JULIA_AUX
#define MY_JULIA_AUX

#include <julia.h>

void handle_exception(void);
jl_value_t *handle_eval_string(const char* code);

#endif // MY_JULIA_AUX
