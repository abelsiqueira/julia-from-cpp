#ifndef NORMAL_H
#define NORMAL_H

#include "aux.h" // includes julia.h

class Normal {
    private:
        jl_value_t *normal;
        static bool distributions_loaded;
    public:
        Normal();
        ~Normal();

        static void initialize() {
            if (distributions_loaded)
                return;
            handle_eval_string("using Distributions");
            distributions_loaded = 1;
        }

        double pdf(double x);
        double cdf(double x);
};

#endif
