#include "gui.h"
#include "hacks.h"
#include "globals.h"

#include <thread>

template <typename T>
T* GetInterface(const char* interfaceName, const char* moduleName)
{
	const auto handle = GetModuleHandle(moduleName);

	if (!handle)
	{
		return nullptr;
	}

	using Fn = T * (*)(const char*, int*);
	const auto CreateInterface = reinterpret_cast<Fn>(GetProcAddress(handle, "CreateInterface"));

	if (!CreateInterface)
	{
		return nullptr;
	}

	return CreateInterface(interfaceName, nullptr);
}

int __stdcall wWinMain(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	PWSTR arguments,
	int commandShow)
{
	Memory mem{ "csgo.exe" };

	globals::clientAdress = mem.GetModuleAddress("client.dll");
	globals::engineAdress = mem.GetModuleAddress("engine.dll");

	std::thread(hacks::VisualsThread, mem).detach();
	std::thread(hacks::MovementThread, mem).detach();
	std::thread(hacks::LegitBotThread, mem).detach();
	//std::thread(hacks::AimBotThread, mem).detach();

	// create gui
	gui::CreateHWindow("Lumina External");
	gui::CreateDevice();
	gui::CreateImGui();

	while (gui::isRunning)
	{
		gui::BeginRender();
		gui::Render();
		gui::EndRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	// destroy gui
	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();

	return EXIT_SUCCESS;
}
