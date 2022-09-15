#include <cstddef>

extern "C" {
    void poisson_mul(double *out, double *in, int n) {
        for (size_t i = 0; i < n; i++) {
            out[i] = 2 * in[i];
            if (i > 0)
                out[i] -= in[i - 1];
            if (i < n - 1)
                out[i] -= in[i + 1];
        }
    }
}
