#pragma once
#include <cstdint>
#include <cstddef>

namespace globals
{
	inline std::uintptr_t clientAdress = 0;
	inline std::uintptr_t engineAdress = 0;
}

namespace offsets
{
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDE7964;
	constexpr ::std::ptrdiff_t dwGlowObjectManager = 0x5357948;
	constexpr ::std::ptrdiff_t dwEntityList = 0x4DFCE74;
	constexpr ::std::ptrdiff_t dwForceJump = 0x52B8BFC;
	constexpr ::std::ptrdiff_t dwForceAttack = 0x322AC7C;
	constexpr ::std::ptrdiff_t dwClientState = 0x59F194;
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
	constexpr ::std::ptrdiff_t m_hMyWeapons = 0x2E08;
	constexpr ::std::ptrdiff_t m_hActiveWeapon = 0x2F08;
	constexpr ::std::ptrdiff_t m_nFallbackPaintKit = 0x31D8;

	constexpr ::std::ptrdiff_t model_ambient_min = 0x5A118C;
}