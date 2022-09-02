#include <iostream>
#include <julia.h>
JULIA_DEFINE_FAST_TLS

using namespace std;

int main() {
    cout << "Testing Julia from C++" << endl;

    jl_init();

    jl_eval_string("println(sqrt(2.0))");

    jl_atexit_hook(0);

    return 0;
}
