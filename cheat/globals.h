#pragma once
#include <cstdint>
#include <cstddef>

namespace globals
{
	inline std::uintptr_t clientAdress = 0;
	inline std::uintptr_t engineAdress = 0;

	// Visuals
	inline bool glow = false;
	inline float glowColor[] = { 1.f, 0.f, 0.f, 1.f };
	inline bool thirdperson = false;
	inline bool model_ambient_tog = false;
	inline float model_ambient_val = 1.f;
	inline bool noFlash = false;
	inline float flashAplha = 0.f;

	// Legitbot
	inline bool triggerbot = false;
	inline bool aimbot = false;
	inline bool autoshoot = false;
	inline float smoothing = 3.f;
	inline float bestfov = 5.f;
	inline bool magnettrigger = false;

	//Misc
	inline bool bhop = false;
	inline bool radar = false;
	inline bool clantag = false;
}

namespace offsets
{
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDBA5BC;
	constexpr ::std::ptrdiff_t dwGlowObjectManager = 0x531F608;
	constexpr ::std::ptrdiff_t dwEntityList = 0x4DD69DC;
	constexpr ::std::ptrdiff_t dwForceJump = 0x5280924;
	constexpr ::std::ptrdiff_t dwForceAttack = 0x3206E9C;
	constexpr ::std::ptrdiff_t dwClientState = 0x58BFDC;
	constexpr ::std::ptrdiff_t dwClientState_ViewAngles = 0x4D90;
	constexpr ::std::ptrdiff_t dwClientState_GetLocalPlayer = 0x180;

	constexpr ::std::ptrdiff_t m_iTeamNum = 0xF4;
	constexpr ::std::ptrdiff_t m_lifeState = 0x25F;
	constexpr ::std::ptrdiff_t m_iGlowIndex = 0x10488;
	constexpr ::std::ptrdiff_t m_bSpotted = 0x93D;
	constexpr ::std::ptrdiff_t m_fFlags = 0x104;
	constexpr ::std::ptrdiff_t m_iHealth = 0x100;
	constexpr ::std::ptrdiff_t m_iCrosshairId = 0x11838;
	constexpr ::std::ptrdiff_t m_iObserverMode = 0x3388;
	constexpr ::std::ptrdiff_t m_dwBoneMatrix = 0x26A8;
	constexpr ::std::ptrdiff_t m_vecOrigin = 0x138;
	constexpr ::std::ptrdiff_t m_vecViewOffset = 0x108;
	constexpr ::std::ptrdiff_t m_aimPunchAngle = 0x303C;
	constexpr ::std::ptrdiff_t m_bSpottedByMask = 0x980;
	constexpr ::std::ptrdiff_t m_bDormant = 0xED;
	constexpr ::std::ptrdiff_t m_flFlashMaxAlpha = 0x1046C;

	constexpr ::std::ptrdiff_t model_ambient_min = 0x58F054;
	//clantag

	constexpr ::std::ptrdiff_t dwSetClanTag = 0x8A320;
}