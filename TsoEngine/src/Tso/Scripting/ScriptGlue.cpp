#include "TPch.h"
#include "ScriptGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/tabledefs.h"
#include "mono/metadata/mono-debug.h"
#include "mono/metadata/threads.h"


namespace Tso {

#define TSO_ADD_INTERNAL_FUNC(Name) mono_add_internal_call("Tso.InternalCalls::" #Name, Name)

	namespace Utils {

		static std::string MonoStringToString(MonoString* string)
		{
			char* cStr = mono_string_to_utf8(string);
			std::string str(cStr);
			mono_free(cStr);
			return str;
		}

	}

	static void NativeLOG(MonoString* msg) {
		std::string str = Utils::MonoStringToString(msg);
		TSO_CORE_INFO("{}", str.c_str());
	}

	void ScriptGlue::RegisterFunctions() {
		TSO_ADD_INTERNAL_FUNC(NativeLOG);
	}

}