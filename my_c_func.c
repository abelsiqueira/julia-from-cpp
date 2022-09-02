#include <cmath>

extern "C" {
    double my_c_func(double x) {
        return exp(-x * x / 2) / sqrt(2 * M_PI);
    }
}
