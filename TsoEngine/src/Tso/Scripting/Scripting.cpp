#include "TPch.h"
#include "mono/metadata/assembly.h"
#include "Scripting.h"
#include <fstream>

namespace Utils {
    char* ReadBytes(const std::string& filepath, uint32_t* outSize)
    {
        std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

        if (!stream)
        {
            TSO_CORE_ERROR("Failed to open the file:" + filepath);
            return nullptr;
        }

        std::streampos end = stream.tellg();
        stream.seekg(0, std::ios::beg);
        uint32_t size = (uint32_t)(end - stream.tellg());

        if (!size)
        {
            TSO_CORE_ERROR("File is empty:" + filepath);
            return nullptr;
        }

        char* buffer = new char[size];
        stream.read((char*)buffer, size);
        stream.close();

        *outSize = size;
        return buffer;
    }
}

namespace Tso
{
    static MonoDomain* s_CSharpDomain;

    // ��ȡһ��C# dll��Mono��, Ȼ�󷵻ض�Ӧ��Assemblyָ��
    MonoAssembly* Scripting::LoadCSharpAssembly(const std::string& assemblyPath)
    {
        // InitMono����
        // Let Mono know where the .NET libraries are located.
        mono_set_assemblies_path("../TsoEngine/third_party/mono/DotNetLibs/4.5");

        MonoDomain* rootDomain = mono_jit_init("MyScriptRuntime");
        if (rootDomain == nullptr)
        {
            // Maybe log some error here
            return nullptr;
        }

        // Create an App Domain
        s_CSharpDomain = mono_domain_create_appdomain("MyAppDomain", nullptr);
        mono_domain_set(s_CSharpDomain, true);

        uint32_t fileSize = 0;
        // ����ֱ�Ӷ�ȡC#��.dll�ļ�, ��������bytes����
        char* fileData = Utils::ReadBytes(assemblyPath, &fileSize);

        // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
        MonoImageOpenStatus status;
        // �Ѷ�ȡ��dll����Mono, �õ���assembly�����Mono���, ��ʱ����Ҫ����
        MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, true, &status, false);

        if (status != MONO_IMAGE_OK)
        {
            const char* errorMessage = mono_image_strerror(status);
            // Log some error message using the errorMessage data
            return nullptr;
        }

        // ��image���ȡassemblyָ��
        MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
        mono_image_close(image);

        // Don't forget to free the file data
        delete[] fileData;

        return assembly;
    }

    // iterate through all the type definitions in our assembly
    void Scripting::PrintAssemblyTypes(MonoAssembly* assembly)
    {
        MonoImage* image = mono_assembly_get_image(assembly);

        // ��assembly��meta��Ϣ���ȡmeta data table, �����ȡ����Type��Ӧ��Table, �����ÿһ��
        // ����һ��Type
        const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

        // ����Table���ÿ��, �����numTypes��СΪ1, ��ΪC#��DLL��EXEsĬ�϶�����һ��Module���͵�Type, ��������
        // assembly��module
        for (int32_t i = 1; i < numTypes; i++)
        {
            // ÿһ�е�ÿ��Ԫ�ؼ�¼��Type�������Ϣ, ����namespace��type name
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

            // �����Ի�ȡfield list��method list��
            const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

            printf("%s.%s\n", nameSpace, name);
        }
    }

    MonoClass* Scripting::GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className)
    {
        MonoImage* image = mono_assembly_get_image(assembly);
        MonoClass* klass = mono_class_from_name(image, namespaceName, className);

        if (!klass)
            return nullptr;

        return klass;
    }

    MonoObject* Scripting::CreateInstance(MonoClass* p)
    {
        if (!p) return nullptr;

        MonoObject* classInstance = mono_object_new(s_CSharpDomain, p);

        // Call the parameterless (default) constructor
        mono_runtime_object_init(classInstance);

        return classInstance;
    }

    void Scripting::CallMethod(MonoObject* objectInstance, const char* methodName)
    {
        // Get the MonoClass pointer from the instance
        MonoClass* instanceClass = mono_object_get_class(objectInstance);

        // Get a reference to the method in the class
        MonoMethod* method = mono_class_get_method_from_name(instanceClass, methodName, 0);

        if (!method)
            return;

        // Call the C# method on the objectInstance instance, and get any potential exceptions
        MonoObject* exception = nullptr;
        mono_runtime_invoke(method, objectInstance, nullptr, &exception);

        // TODO: Handle the exception
    }

    // ע��, MonoClassField������Field����, �����������������object��offset
    MonoClassField* Scripting::GetFieldRef(MonoObject* objInstance, const char* fieldName)
    {
        MonoClass* testingClass = mono_object_get_class(objInstance);

        // Get a reference to the public field called "MyPublicFloatVar"
        return mono_class_get_field_from_name(testingClass, fieldName);
    }

    MonoProperty* Scripting::GetPropertyRef(MonoObject* objInstance, const char* propertyName)
    {
        MonoClass* testingClass = mono_object_get_class(objInstance);

        // Get a reference to the public field called "MyPublicFloatVar"
        return mono_class_get_property_from_name(testingClass, propertyName);
    }
}
