#include "Normal.h"

bool Normal::distributions_loaded = false;
jl_function_t *Normal::_Normal = 0;
jl_function_t *Normal::_pdf = 0;
jl_function_t *Normal::_cdf = 0;

Normal::Normal(double mean, double standard_deviation) {
    Normal::initialize();
    this->normal = jl_call2(_Normal, jl_box_float64(mean), jl_box_float64(standard_deviation));
}

Normal::~Normal() {}

double Normal::pdf(double x) {
    return jl_unbox_float64(jl_call2(_pdf, this->normal, jl_box_float64(x)));
}

double Normal::cdf(double x) {
    return jl_unbox_float64(jl_call2(_cdf, this->normal, jl_box_float64(x)));
}

