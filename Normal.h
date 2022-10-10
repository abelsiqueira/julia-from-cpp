#ifndef NORMAL_H
#define NORMAL_H

#include "aux.h" // includes julia.h

class Normal {
    private:
        jl_value_t *normal;
        double mean, standard_deviation;
        static bool distributions_loaded;
        static jl_function_t *_Normal, *_pdf, *_cdf;
    public:
        Normal(double mean = 0.0, double standard_deviation = 1.0);
        ~Normal();

        static void initialize() {
            if (distributions_loaded)
                return;
            handle_eval_string("using Distributions");
            distributions_loaded = 1;
            _Normal = jl_get_function(jl_main_module, "Normal");
            _pdf = jl_get_function(jl_main_module, "pdf");
            _cdf = jl_get_function(jl_main_module, "cdf");
            handle_julia_exception();
        }

        double pdf(double x);
        double cdf(double x);
};

#endif
