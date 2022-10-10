#include "Normal.h"
#include <julia.h>
#include <cmath>
#include <iostream>
#include <iomanip>
JULIA_DEFINE_FAST_TLS // only define this once, in an executable (not in a shared library) if you want fast code.

using namespace std;

int main() {
    jl_init();

    Normal normal(3.27, 1.3);
    for (double x = -4.0; x <= 4.0; x += 1.0) {
        cout << std::scientific << std::setprecision(2) << std::showpos
             << "x: " << x
             << "  pdf: " << normal.pdf(x)
             << "  cdf: " << normal.cdf(x) << endl;
    }

    jl_atexit_hook(0);
    return 0;
}
