#pragma once

#include <filesystem>
#include <string>

class c_config final {
public:
	void run(const char*) noexcept;
	void load(size_t) noexcept;
	void save(size_t) const noexcept;
	void add(const char*) noexcept;
	void remove(size_t) noexcept;
	void rename(size_t, const char*) noexcept;

	constexpr auto& get_configs() noexcept {
		return configs;
	}

	struct {
		// LegitBot
		bool bAimbot{ false };
		bool bTriggerbot{ false };
		bool bMagnetTrigger{ false };
		float fFov{ 5.0f };
		float fSmooth{ 3.f };


		// Visuals
		bool bGlow{ false };
		float fGlowColor[4]{ 1.0f, 0.0f, 0.0f, 1.0f };
		bool bThirdperson{ false };
		bool bModelAmbient{ false };
		float fModelAmbient{ 1.f };
		bool bNoFlash{ false };
		float fNoFlash{ 0.f };
		bool bChams{ false };
		float fTeamColor[3]{ 0, 0, 255 };
		float fEnemyColor[3]{ 255, 0, 0 };
		

		// Misc
		bool bBhop{ false };
		bool bRadar{ false };
		
	} item;

private:
	std::filesystem::path path;
	std::vector<std::string> configs;
};

extern c_config config_system;