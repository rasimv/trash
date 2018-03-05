#ifdef CUDA_LIB_EXPORTS
#   define CUDA_LIB_API __declspec(dllexport)
#else
#   define CUDA_LIB_API __declspec(dllimport)
#endif
