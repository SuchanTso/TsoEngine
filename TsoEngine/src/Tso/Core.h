#pragma once

#ifdef TSO_PLATFORM_WINDOWS
	#ifdef TSO_BUILD_DLL
		#define TSO_API __declspec(dllexport)
	#else 
		#define TSO_API __declspec(dllimport)
	#endif
#else 
	#error TSO only support Windows now !
#endif // TSO_PLATFORM_WINDOWS


#define BIT(x) (1 << (x))
