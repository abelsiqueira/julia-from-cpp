#include <cmath>

extern "C" {
    double my_c_func(double x) {
        return x * x * x;
    }
}
