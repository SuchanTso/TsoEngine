#pragma once

#include <memory>

#ifdef TSO_PLATFORM_WINDOWS
#if TSO_DYLIB
	#ifdef TSO_BUILD_DLL
		#define TSO_API __declspec(dllexport)
	#else 
		#define TSO_API __declspec(dllimport)
	#endif
#else
	#define TSO_API
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


#ifdef TSO_ENABLE_ASSERTS
    #ifdef TSO_PLATFORM_WINDOWS
        #define TSO_ASSERT(x, ...) {if(!(x)){TSO_ERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak();}}
        #define TSO_CORE_ASSERT(x, ...){if(!(x)){TSO_CORE_ERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak();}}
    #elif defined TSO_PLATFORM_MACOSX
        #define TSO_ASSERT(x, ...) {if(!(x)){TSO_ERROR("Assertion Failed: {0}",__VA_ARGS__); __builtin_trap();}}
        #define TSO_CORE_ASSERT(x, ...){if(!(x)){TSO_CORE_ERROR("Assertion Failed: {0}",__VA_ARGS__); __builtin_trap();}}
    #else
    #endif
#else
#define TSO_ASSERT(x, ...) 
#define TSO_CORE_ASSERT(x, ...) 
#endif // TSO_ENABLE_ASSERTS


#define BIT(x) (1 << (x))

#define BIND_EVENT_FN(x) std::bind(&x, this , std::placeholders::_1)


namespace Tso {

	template<typename T>
	using Scope = std::unique_ptr<T>;

    template<typename T , typename... Arg>
    constexpr Scope<T> CreateScope(Arg&& ...arg){
        return std::make_unique<T>(std::forward<Arg>(arg)...);
    }

	template<typename T>
	using Ref = std::shared_ptr<T>;

    template<typename T , typename... Arg>
    constexpr Ref<T> CreateRef(Arg&& ...arg){
        return std::make_shared<T>(std::forward<Arg>(arg)...);
    }
}
