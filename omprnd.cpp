#include <random>
#include "omp.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

//define _SUPER_UGLY_WORKAROUND


#if defined(MKL)
#include "mkl.h"
#include <dlfcn.h>
#endif

#if defined(TINA)
#include <trng/yarn2.hpp>
#include <trng/uniform01_dist.hpp>
#endif

using namespace std;
namespace py = pybind11;

template <typename T>
void parrnd(const size_t N, T * out)
{
    std::random_device rd;
    long unsigned int seed = rd();

#if defined(MKL)

#if defined(_SUPER_UGLY_WORKAROUND)
void *handle = dlopen("libmkl_core.so", RTLD_LAZY | RTLD_GLOBAL); void *handle1 = dlopen("libmkl_gnu_thread.so", RTLD_LAZY | RTLD_GLOBAL); void *handle2 = dlopen("libmkl_intel_lp64.so", RTLD_LAZY | RTLD_GLOBAL); 
#endif

    VSLStreamStatePtr stream;

    mkl_set_num_threads_local( omp_get_max_threads() );
    vslNewStream(&stream,VSL_BRNG_MT19937,seed);
    if (typeid(T) == typeid(double)) {
        vdRngUniform(VSL_RNG_METHOD_UNIFORM_STD,stream,N, ((double*) &out[0]), 0.0d,1.0d);
    } else {
        vsRngUniform(VSL_RNG_METHOD_UNIFORM_STD,stream,N, ((float*) &out[0]), 0.0d,1.0d);
    }
    

    vslDeleteStream(&stream);

#elif defined(TINA)

#pragma omp parallel shared(out, N, seed) default(none)
{
    trng::yarn2 r;                          // random number engine
    const int size{omp_get_num_threads()};  // get total number of processes
    const int rank{omp_get_thread_num()};   // get rank of current process
    trng::uniform01_dist<T> u;               // random number distribution
    r.seed(seed);
    r.jump(2 * (rank * N / size));    // jump ahead

    for (size_t i{rank * N / size}; i < (rank + 1) * N / size; ++i)
      out[i] = u(r);
}

#else
#error Do not use this code!
#pragma omp parallel default(none) shared(out,N,seed)
{
	mt19937 g = mt19937(1654 + (17*(omp_get_thread_num() + seed));
	uniform_real_distribution<T> f = uniform_real_distribution<T>(0.0,1.0);
        #pragma omp for
	for (size_t i =0; i < N; i++)
            out[i] = f(g);
}

#endif
    
}



PYBIND11_MODULE(omprnd, m){
    m.def("unif01", [](const size_t N)
    {
        // Allocate and initialize some data; make this big so
        // we can see the impact on the process memory use:
        float *foo = new float[N];
        parrnd(N,foo);

        // Create a Python object that will free the allocated
        // memory when destroyed:
        py::capsule free_when_done(foo, [](void *f) {
            float *foo = reinterpret_cast<float *>(f);
            delete[] foo;
        });

        return py::array_t<float>(
            {N}, // shape
            {8}, // C-style contiguous strides for float
            foo, // the data pointer
            free_when_done); // numpy array references this parent
    }, "Simple parallel random number generator (float)");
    m.def("unid01", [](const size_t N)
    {
        // Allocate and initialize some data; make this big so
        // we can see the impact on the process memory use:
        double *foo = new double[N];
        parrnd(N,foo);

        // Create a Python object that will free the allocated
        // memory when destroyed:
        py::capsule free_when_done(foo, [](void *f) {
            double *foo = reinterpret_cast<double *>(f);
            delete[] foo;
        });

        return py::array_t<double>(
            {N}, // shape
            {8}, // C-style contiguous strides for double
            foo, // the data pointer
            free_when_done); // numpy array references this parent
    }, "Simple parallel random number generator (double)");
}
