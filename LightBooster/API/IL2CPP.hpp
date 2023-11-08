#pragma once

#include <Windows.h>
#include <iostream>

inline static auto gameAssembly = GetModuleHandleA("GameAssembly.dll");

#define ProcAddr(func) GetProcAddress(gameAssembly, func)

template<typename T, typename... Args>
inline static T call(const char* func, Args... args) {
	return reinterpret_cast<T(__fastcall*)(Args...)>(ProcAddr(func))(args...);
}

namespace System {
	class Object_ {
	public:

	};
	template<typename T = void*>
	class Array {
	public:
		uint32_t size() {
			if (!this) return 0;
			return *reinterpret_cast<uint32_t*>(this + 0x18);
		}
		T get(int idx) {
			if (!this) return T{};
			return *reinterpret_cast<T*>(this + (0x20 + (idx * 0x8)));
		}
		void add(int idx, T value) {
			if (!this) return;
			*reinterpret_cast<T*>(this + (0x20 + (idx * 0x8))) = value;
		}
	};
	class String : public Object_ {
	public:
		char pad_0000[0x10];
		int len;
		wchar_t buffer[0];

		static String* New(const char* str) {
			return call<String*, const char*>("il2cpp_string_new", str);
		}
	};
	template<typename T = void*>
	struct List {
	public:
		char pad_0000[0x10];
		void* buffer;
		uint32_t size;

		T* get(uint32_t idx) {
			if (!this) return nullptr;

			if (idx > this->size) return nullptr;

			void* items = this->buffer;

			if (!items) return nullptr;

			return *reinterpret_cast<T**>((uint64_t)items + (0x20 + (idx * 0x8)));
		}
	};
}

class Il2CppType
{
public:
	char* name() {
		return call<char*, Il2CppType*>("il2cpp_type_get_name", this);
	}
};

class Il2CppMethod {
public:
	uint32_t paramCount() {
		return call<uint32_t, Il2CppMethod*>("il2cpp_method_get_param_count", this);
	}

	Il2CppType* retType() {
		return call<Il2CppType*, Il2CppMethod*>("il2cpp_method_get_return_type", this);
	}

	Il2CppType* getParam(uint32_t idx) {
		return call<Il2CppType*, Il2CppMethod*, uint32_t>("il2cpp_method_get_param", this, idx);
	}

	char* name() {
		return call<char*, Il2CppMethod*>("il2cpp_method_get_name", this);
	}
};

class Il2CppClass
{
public:
	class Il2CppImage* image; //0x0000
	char pad_0008[8]; //0x0008
	char* name; //0x0010
	char* namespaze; //0x0018
	char pad_0020[152]; //0x0020
	void* static_fields; //0x00B8

	Il2CppMethod* method(const char* name, int argsCount = 0) {
		return call<Il2CppMethod*, Il2CppClass*, const char*, int>("il2cpp_class_get_method_from_name", this, name, argsCount);
	}
}; //Size: 0x00C0

class Il2CppImage
{
public:
	char* assemblyName; //0x0000
	char* name; //0x0008

	Il2CppClass* get_class(const char* ns, const char* name)
	{
		return call<Il2CppClass*, Il2CppImage*, const char*, const char*>("il2cpp_class_from_name", this, ns, name);
	}
}; 

class Il2CppAssembly {
public:
	uint64_t buffer;
};

class Il2CppDomain {
public:
	size_t assemblyCount() {
		size_t size = 0;
		auto assemblies = call<Il2CppAssembly**, Il2CppDomain*, void*>("il2cpp_domain_get_assemblies", this, &size);

		return size;

	}

	Il2CppAssembly** assemblies() {
		size_t size = 0;

		return call<Il2CppAssembly**, Il2CppDomain*, void*>("il2cpp_domain_get_assemblies", this, &size);
	}


	inline static Il2CppDomain* get() {
		return call<Il2CppDomain*>("il2cpp_domain_get");
	}
};

static uintptr_t il2cpp_string_new(const char* str) {
	return call<uintptr_t, const char*>("il2cpp_string_new", str);
}