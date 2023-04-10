#pragma once

#ifdef TSO_PLATFORM_WINDOWS
	#ifdef TSO_BUILD_DLL
		#define TSO_API __declspec(dllexport)
	#else 
		#define TSO_API __declspec(dllimport)
	#endif
#elif defined TSO_PLATFORM_MACOSX
    #ifdef TSO_BUILD_DYLIB
        #define TSO_API __attribute__((visibility ("default")))
    #else
        #define TSO_API
    #endif
#else
#error only support windows & macos now!
#endif // TSO_PLATFORM_WINDOWS


#define BIT(x) (1 << (x))
