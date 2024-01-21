#pragma once
#include <string>
#include "mono/metadata/image.h"
#include "mono/jit/jit.h"

namespace Tso
{
	// ������Unity, C#��ߵĽű����Ϊ���Ĳ���û�������
	// ���Ĳ�Ĵ���(C#��ߵ�Դ��)Ӧ���Ǻ�C++�Ĵ��������໥���õ������
	class Scripting
	{
	public:
		MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath);

		void PrintAssemblyTypes(MonoAssembly* assembly);

		// ����C++��������class name, ���ض�Ӧ��MonoClass, �������C++�˴���C#�ϵĶ���, ��Ҫ������API
		MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);

		MonoObject* CreateInstance(MonoClass* p);

		// Mono gives us two ways of calling C# methods: mono_runtime_invoke and Unmanaged Method Thunks. 
		// This Api will only cover mono_runtime_invoke

		// Using mono_runtime_invoke is slower compared to Unmanaged Method Thunks, but it's also safe and more flexible. 
		// mono_runtime_invoke can invoke any method with any parameters, and from what I understand mono_runtime_invoke also does a lot more error checking and validation on the object you pass, as well as the parameters.

		// �ڱ����ڲ�֪��Methodǩ��ʱ, �ʺ���mono_runtime_invoke, ÿ���Ƶ�ʵ���(10fps)��Method�ʺ���Unmanaged Method Thunks, 
		void CallMethod(MonoObject* instance, const char* methodName);

		// Field can be public or private
		MonoClassField* GetFieldRef(MonoObject* instance, const char* fieldName);

		template<class T>
		const T& GetFieldValue(MonoObject* instance, MonoClassField* field)
		{
			T value;
			mono_field_get_value(instance, field, &value);
			return value;
		}

		MonoProperty* GetPropertyRef(MonoObject* instance, const char* fieldName);

		template<class T>
		const T& GetPropertyValue(MonoObject* instance, MonoProperty* prop)
		{
			T value;
			mono_property_get_value(instance, prop, &value);
			return value;
		}
	};
}
