#include "gui.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

#include "globals.h"
#include "../imgui/imgui_internal.h"
#include <string>
#include <vector>

#include "config/config.hpp"

bool save_config = false;
bool load_config = false;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);

long __stdcall WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message)
	{
	case WM_SIZE: {
		if (gui::device && wideParameter != SIZE_MINIMIZED)
		{
			gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
			gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
			gui::ResetDevice();
		}
	}return 0;

	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}return 0;

	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(longParameter); // set click points
	}return 0;

	case WM_MOUSEMOVE: {
		if (wideParameter == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(longParameter);
			auto rect = ::RECT{ };

			GetWindowRect(gui::window, &rect);

			rect.left += points.x - gui::position.x;
			rect.top += points.y - gui::position.y;

			if (gui::position.x >= 0 &&
				gui::position.x <= gui::WIDTH &&
				gui::position.y >= 0 && gui::position.y <= 19)
				SetWindowPos(
					gui::window,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
		}

	}return 0;

	}

	return DefWindowProc(window, message, wideParameter, longParameter);
}

void Colors()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_WindowBg] = ImColor(16, 16, 16);
	style.Colors[ImGuiCol_ChildBg] = ImColor(24, 24, 24);
	style.Colors[ImGuiCol_Text] = ImColor(255, 255, 255);
	style.Colors[ImGuiCol_CheckMark] = ImColor(255, 255, 255);

	style.Colors[ImGuiCol_Header] = ImColor(30, 30, 30);
	style.Colors[ImGuiCol_HeaderActive] = ImColor(28, 28, 28);
	style.Colors[ImGuiCol_HeaderHovered] = ImColor(28, 28, 28);
}

void gui::CreateHWindow(const char* windowName) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = "class001";
	windowClass.hIconSm = 0;

	RegisterClassEx(&windowClass);

	window = CreateWindowEx(
		0,
		"class001",
		windowName,
		WS_POPUP,
		100,
		100,
		WIDTH,
		HEIGHT,
		0,
		0,
		windowClass.hInstance,
		0
	);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept
{
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParameters,
		&device) < 0)
		return false;

	return true;
}

void gui::ResetDevice() noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept
{
	if (device)
	{
		device->Release();
		device = nullptr;
	}

	if (d3d)
	{
		d3d->Release();
		d3d = nullptr;
	}
}

void gui::CreateImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Framd.ttf", 16.0f);

	 // ImGui::StyleColorsDark();
	Colors();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);

		if (message.message == WM_QUIT)
		{
			isRunning = !isRunning;
			return;
		}
	}

	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	// Handle loss of D3D9 device
	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}



void gui::Render() noexcept
{
	struct tab_data {
		const char* name;
	};

	static std::vector<tab_data> tabs = {
		{ "Legitbot" },
		{ "Visuals" },
		{ "Misc" },
		{ "Config" }
	};

	static int tab = 0;

	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin(
		"Lumina External Pre-Alpha",
		&isRunning,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse 
		//ImGuiWindowFlags_NoMove 
		//ImGuiWindowFlags_NoTitleBar
	);

	ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0, 0, 0, 255).Value);
	ImGui::BeginChild("##LeftSide", ImVec2(120, ImGui::GetContentRegionAvail().y), true);
	{
		for (unsigned int i = 0; i < tabs.size(); i++)
		{
			bool selected = ( tab == i );
			if (ImGui::Selectable(tabs[i].name, &selected))
				tab = i;

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
	}
	ImGui::EndChild();

	{
		ImGui::SameLine(0);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();
	}

	ImGui::BeginChild("##RightSide", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true);
	{
		auto& data = tabs[tab];
		if (data.name == "Legitbot")
		{

			ImGui::Checkbox("Triggerbot", &config_system.item.bTriggerbot);
			ImGui::Checkbox("Aimbot", &config_system.item.bAimbot);

			if (&config_system.item.bTriggerbot) {

				ImGui::Checkbox("MagnetTrigger", &config_system.item.bMagnetTrigger);

			}


			if (&config_system.item.bAimbot)
			{
				ImGui::Checkbox("Autoshoot", &config_system.item.bAutoshoot);
				ImGui::SliderFloat("Smoothing", &config_system.item.fSmooth, 1.f, 80.f);
				ImGui::SliderFloat("Fov", &config_system.item.fFov, 0.f, 360.f);
			}

		} else if (data.name == "Visuals")
		{

			ImGui::Checkbox("Glow", &config_system.item.bGlow);
			if (&config_system.item.bGlow)
			{
				ImGui::ColorEdit4("Glow color", config_system.item.fGlowColor);
			}
			ImGui::Checkbox("Thirdperson", &config_system.item.bThirdperson);
			ImGui::Checkbox("Model Ambient", &config_system.item.bModelAmbient);
			if (&config_system.item.bModelAmbient)
			{
				ImGui::SliderFloat("Value", &config_system.item.fModelAmbient, 1.f, 25.f);
			}
			ImGui::Checkbox("No Flash", &config_system.item.bNoFlash);
			if (&config_system.item.bNoFlash)
			{
				ImGui::SliderFloat("Flash Aplha", &config_system.item.fNoFlash, 0.0f, 255.0f);
			}

		} else if (data.name == "Misc")
		{

			ImGui::Checkbox("Radarhack", &config_system.item.bRadar);
			ImGui::Checkbox("Bunnyhop", &config_system.item.bBhop);

		}
		else if (data.name == "Config")
		{
			ImGui::BeginChild("config", ImVec2(279, 268), true); {
				constexpr auto& config_items = config_system.get_configs();
				static int current_config = -1;

				if (static_cast<size_t>(current_config) >= config_items.size())
					current_config = -1;

				static char buffer[16];

				if (ImGui::ListBox("", &current_config, [](void* data, int idx, const char** out_text) {
					auto& vector = *static_cast<std::vector<std::string>*>(data);
					*out_text = vector[idx].c_str();
					return true;
				}, &config_items, config_items.size(), 5) && current_config != -1)
					strcpy(buffer, config_items[current_config].c_str());

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
				ImGui::PushID(0);
				ImGui::PushItemWidth(178);
				if (ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
					if (current_config != -1)
						config_system.rename(current_config, buffer);
				}
				ImGui::PopID();

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
				if (ImGui::Button(("Create"), ImVec2(85, 20))) {
					config_system.add(buffer);
				}

				ImGui::SameLine();

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
				if (current_config != -1) {
					if (ImGui::Button(("Load"), ImVec2(85, 20))) {
						config_system.load(current_config);

						load_config = true;
					}

					if (ImGui::Button(("Save"), ImVec2(85, 20))) {
						config_system.save(current_config);

						save_config = true;
					}

					ImGui::SameLine();

					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
					if (ImGui::Button(("Remove"), ImVec2(85, 20))) {
						config_system.remove(current_config);
					}
				}
			}
			ImGui::EndChild();
		}
	}
	ImGui::EndChild();

	ImGui::PopStyleColor();


	ImGui::End();
}
