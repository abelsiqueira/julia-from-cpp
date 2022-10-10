#include "Normal.h"

bool Normal::distributions_loaded = false;

Normal::Normal() {
    Normal::initialize();
    this->normal = handle_eval_string("Normal()");
}

Normal::~Normal() {}

double Normal::pdf(double x) {
    jl_function_t *pdf = jl_get_function(jl_main_module, "pdf");
    return jl_unbox_float64(jl_call2(pdf, this->normal, jl_box_float64(x)));
}

double Normal::cdf(double x) {
    jl_function_t *cdf = jl_get_function(jl_main_module, "cdf");
    return jl_unbox_float64(jl_call2(cdf, this->normal, jl_box_float64(x)));
}

