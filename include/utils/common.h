#pragma once

#ifndef NODE_API
#ifdef _WIN64
#define NODE_API extern "C" __declspec(dllexport)
#else
#define NODE_API extern "C"
#endif
#endif