#pragma once
#include <Windows.h>
#include <iostream>
#include <d3d11.h>
#include <d3d9.h>
#include <D2D1.h>
#include <unordered_map>
#include <dwrite_1.h>
#include <string_view>
#include <memory>
#include <algorithm>
#include <cmath>
#include "Libraries/Detours/detours.h"
#pragma comment(lib, "Libraries/Detours/detours.lib")

#include "Libraries/ImGui/imgui.h"
#include "Libraries/ImGui/imgui_impl_win32.h"
#include "Libraries/ImGui/imgui_impl_dx11.h"

#pragma comment(lib, "dxgi")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dcomp")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "d3d11")

#include <Config.hpp>

inline D3D11_VIEWPORT viewport;
inline HWND game_window;
inline UINT vps = 1;

typedef HRESULT(__stdcall* D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef HRESULT(__stdcall* D3D11ResizeBuffersHook) (IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
typedef void(__stdcall* D3D11PSSetShaderResourcesHook) (ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews);
typedef void(__stdcall* D3D11DrawHook) (ID3D11DeviceContext* pContext, UINT VertexCount, UINT StartVertexLocation);
typedef void(__stdcall* D3D11DrawIndexedHook) (ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
typedef void(__stdcall* D3D11DrawIndexedInstancedHook) (ID3D11DeviceContext* pContext, UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation);
typedef void(__stdcall* D3D11CreateQueryHook) (ID3D11Device* pDevice, const D3D11_QUERY_DESC* pQueryDesc, ID3D11Query** ppQuery);
inline D3D11PresentHook phookD3D11Present = NULL;
inline D3D11ResizeBuffersHook phookD3D11ResizeBuffers = NULL;
inline D3D11PSSetShaderResourcesHook phookD3D11PSSetShaderResources = NULL;
inline D3D11DrawHook phookD3D11Draw = NULL;
inline D3D11DrawIndexedHook phookD3D11DrawIndexed = NULL;
inline D3D11DrawIndexedInstancedHook phookD3D11DrawIndexedInstanced = NULL;
inline D3D11CreateQueryHook phookD3D11CreateQuery = NULL;
inline ID3D11Device* pDevice = NULL;
inline ID3D11DeviceContext* pContext = NULL;
inline DWORD_PTR* pSwapChainVtable = NULL;
inline DWORD_PTR* pContextVTable = NULL;
inline DWORD_PTR* pDeviceVTable = NULL;
inline HWND window = nullptr;
inline ID3D11RenderTargetView* mainRenderTargetView;
inline WNDPROC oWndProc;
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef HRESULT(__fastcall* DX11PresentFn) (IDXGISwapChain* pSwapChain, UINT syncintreval, UINT flags);
inline DX11PresentFn oPresent = nullptr;
inline ID3D11Device* device = nullptr;
inline ID3D11DeviceContext* immediate_context = nullptr;
inline ID3D11RenderTargetView* render_target_view = nullptr;

typedef BOOL(__stdcall* __BitBlt)(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop);

inline HRESULT(*present_original)(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) = nullptr;
inline HRESULT(*resize_original)(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) = nullptr;
inline __BitBlt bitblt_trampoline = NULL;

inline WNDPROC original_windowproc = nullptr;

LRESULT WndProcHook(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProcA(original_windowproc, hWnd, uMsg, wParam, lParam);
}

static ImFont* SmallFont;
static ImFont* NormalFont;
static ImFont* LargeFont;

void InitFonts() {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImFontConfig font_config;
	font_config.OversampleH = 1; //or 2 is the same
	font_config.OversampleV = 1;
	font_config.PixelSnapH = 1;

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x044F, // Cyrillic
		0,
	};
	NormalFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 16.0f, &font_config, ranges);
	SmallFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 12.0f, &font_config, ranges);
	LargeFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 24.0f, &font_config, ranges);

	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_WindowBg] = { 0.1, 0.1, 0.175, 1 };
	style.Colors[ImGuiCol_CheckMark] = { 1, 1, 1, 1 };
	style.Colors[ImGuiCol_FrameBg] = { 0.4, 0.4, 1, 1 };
	style.Colors[ImGuiCol_FrameBgHovered] = { 0.4, 0.4, 1, 0.9 };
	style.Colors[ImGuiCol_FrameBgActive] = { 0.4, 0.4, 1, 0.8 };

	style.WindowPadding = { 20, 20 };
	style.FramePadding = { 10, 6 };

	style.ItemSpacing = { 20, 20 };

	style.WindowBorderSize = 0;
	style.ChildBorderSize = 0;
	style.PopupBorderSize = 0;
	style.FrameBorderSize = 0;
	style.TabBorderSize = 0;

	style.WindowRounding = 10;
	style.FrameRounding = 6;

	ImGui_ImplWin32_Init(game_window);
	ImGui_ImplDX11_Init(device, immediate_context);
}

bool isMenuOpen = true;

HRESULT present_hook(IDXGISwapChain* swapChain, UINT SyncInterval, UINT Flags) {
	if (!device) {
		swapChain->GetDevice(__uuidof(device), reinterpret_cast<PVOID*>(&device));
		device->GetImmediateContext(&immediate_context);
		ID3D11Texture2D* renderTarget = nullptr;
		swapChain->GetBuffer(0, __uuidof(renderTarget), reinterpret_cast<PVOID*>(&renderTarget));
		device->CreateRenderTargetView(renderTarget, nullptr, &render_target_view);
		renderTarget->Release();
		InitFonts();
	}

	immediate_context->OMSetRenderTargets(1, &render_target_view, nullptr);
	immediate_context->RSGetViewports(&vps, &viewport);
	
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	if (GetAsyncKeyState(VK_INSERT) & 0x1) {
		isMenuOpen = !isMenuOpen;
	}

	if (isMenuOpen) {
		ImGui::SetNextWindowSize({ 320, 200 });
		ImGui::Begin("LightBooster", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

		ImGui::PushFont(LargeFont);
		ImGui::Text(u8"Самый крутой бустер");
		ImGui::PopFont();

		ImGui::Checkbox(u8" Отключение теней", &Config::NoShadows);

		ImGui::Checkbox(u8" Отключение травы", &Config::NoGrass);

		ImGui::PushStyleColor(ImGuiCol_Text, { 0.5, 0.5, 0.5, 1 });
		ImGui::PushFont(SmallFont);

		ImGui::Text(u8"Закрыть меню - Insert");

		ImGui::PopFont();
		ImGui::PopStyleColor();

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return present_original(swapChain, SyncInterval, Flags);
}

HRESULT resize_hook(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
	if (render_target_view) render_target_view->Release();

	if (immediate_context) immediate_context->Release();
	if (device) device->Release();

	device = nullptr;

	return resize_original(swapChain, bufferCount, width, height, newFormat, swapChainFlags);
}

void RendererInitialize()
{
	while (!game_window) {
		EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
			DWORD pid = 0;
			GetWindowThreadProcessId(hWnd, &pid);
			if (pid == GetCurrentProcessId()) {
				*reinterpret_cast<HWND*>(lParam) = hWnd;
				return FALSE;
			}

			return TRUE;
			}, reinterpret_cast<LPARAM>(&game_window));
	}

	//DebugConsole::Attach();

	IDXGISwapChain* swapChain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	auto featureLevel = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC sd = { 0 };
	ZeroMemory(&sd, sizeof sd);
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = game_window;
	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, 0, &featureLevel, 1, D3D11_SDK_VERSION, &sd, &swapChain, &device, nullptr, &context))) {
		MessageBoxW(0, (L"Failed to create D3D11 device and swap chain"), (L"Failure"), MB_ICONERROR);
	}

	auto table = *reinterpret_cast<PVOID**>(swapChain);
	present_original = reinterpret_cast<HRESULT(__fastcall*)(IDXGISwapChain*, UINT, UINT)>(table[8]);
	resize_original = reinterpret_cast<HRESULT(__fastcall*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT)>(table[13]);

	context->Release();
	device->Release();
	swapChain->Release();

	original_windowproc = (WNDPROC)SetWindowLongPtrW(game_window, GWLP_WNDPROC, (LONG_PTR)WndProcHook);

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(LPVOID&)present_original, (PBYTE)present_hook);
	DetourTransactionCommit();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(LPVOID&)resize_original, (PBYTE)resize_hook);
	DetourTransactionCommit();
}