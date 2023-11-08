#pragma once
#include <Windows.h>
#include <API/IL2CPP.hpp>
#include <API/Memory.hpp>
#include <Config.hpp>
#include <Libraries/Detours/detours.h>

template<typename Function>
inline static void ApplyHook(Function*& func, void* detour) {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)func, detour);
	DetourTransactionCommit();
}

enum LightShadows
{
	None,
	Hard,
	Soft
};

class Light {
public:
	static inline void(*SetShadows_)(Light*, LightShadows) = nullptr;

	void SetShadows(LightShadows shadows)
	{
		SetShadows_(this, shadows);
	}
};

class SunSettings {
public:
	static inline void(*Update_)(SunSettings*) = nullptr;

	void Update() {
		return Update_(this);
	}

	Light* get_Light()
	{
		return reinterpret_cast<Light*>(this + 0x18); // private Light light; = 0x18
	}
};

class FoliageCell {
public:
	static inline float (*CalculateLOD_)(FoliageCell*) = nullptr;

	float CalculateLOD()
	{
		return CalculateLOD_(this);
	}
};

inline static void OnShadowsUpdate(SunSettings* instance)
{
	if (Config::NoShadows) {
		Light* light = instance->get_Light();
		if (!light) return;

		light->SetShadows(LightShadows::None);
	}
	else {
		instance->Update();
	}
}

inline static float OnGrassUpdate(FoliageCell* instance)
{
	if (!Config::NoGrass) {
		return instance->CalculateLOD();
	}

	return 0.f;
}

inline static void HooksInitialize()
{
	Il2CppDomain* domain = Il2CppDomain::get();
	Il2CppAssembly** assemblies = domain->assemblies();

	for (int i = 0; i < domain->assemblyCount(); i++) {
		Il2CppImage* image = *reinterpret_cast<Il2CppImage**>(*reinterpret_cast<uint64_t*>(std::uint64_t(assemblies) + (0x8 * i)));

		std::string image_name(image->assemblyName);
		image_name.erase(image_name.find(".dll"), 4);

		if (image_name == "Assembly-CSharp") {

			Il2CppClass* sunSettings_ = image->get_class("", "SunSettings");
			Il2CppClass* folliageCell_ = image->get_class("", "SunSettings");

			SunSettings::Update_ = reinterpret_cast<decltype(SunSettings::Update_)>(sunSettings_->method("Update"));
			FoliageCell::CalculateLOD_ = reinterpret_cast<decltype(FoliageCell::CalculateLOD_)>(sunSettings_->method("CalculateLOD"));

			ApplyHook(SunSettings::Update_, OnShadowsUpdate);
			ApplyHook(FoliageCell::CalculateLOD_, OnGrassUpdate);
		}

		if (image_name == "UnityEngine.CoreModule") {
			Il2CppClass* light_ = image->get_class("UnityEngine", "Light");

			Light::SetShadows_ = reinterpret_cast<decltype(Light::SetShadows_)>(light_->method("set_shadows", 1));
		}
	}
}