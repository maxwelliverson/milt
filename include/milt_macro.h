//
// Created by maxwell on 2019-12-01.
//

#ifndef MILT_MACRO_H
#define MILT_MACRO_H
/*
#ifdef NDEBUG
#define MILT_ASSERT(...)
#else
#define MILT_ASSERT(...) \
    if(!__VA_ARGS__) \

#endif
*/

#if defined(_WIN32)
    #define MILT_WINDOWS(...) __VA_ARGS__
    #define MILT_LINUX(...)
    #define ROOT_DIR "..\\..\\.."
#elif defined(__linux__)
    #define MILT_WINDOWS(...)
    #define MILT_LINUX(...) __VA_ARGS__
    #define ROOT_DIR ".."
#endif

#define MILT_ERR(msg) printf("ERROR: %s\n", msg)



#endif //MILT_MILT_MACRO_H
