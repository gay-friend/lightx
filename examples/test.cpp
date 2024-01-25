#define API extern "C" __declspec(dllexport)
#include <iostream>

API void hello()
{
    std::cout << "hello world" << std::endl;
}

API int my_var = 10;