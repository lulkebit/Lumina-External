#include "hacks.h"
#include "gui.h"
#include "globals.h"
#include "vector.h"
#include "config/config.hpp"

#include <thread>


constexpr Vector3 CalculateAngle(
	const Vector3& localPosition,
	const Vector3& enemyPosition,
	const Vector3& viewAngles) noexcept
{
	return ((enemyPosition - localPosition).ToAngle() - viewAngles);
}

void hacks::VisualsThread(const Memory& mem) noexcept
{
	while (gui::isRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		const auto localPlayer = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer);

		if (!localPlayer)
			continue;

		const auto glowManager = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwGlowObjectManager);

		if (!glowManager)
			continue;

		const auto localTeam = mem.Read<std::int32_t>(localPlayer + offsets::m_iTeamNum);

		if (!localTeam)
			continue;

		for (auto i = 1; i <= 32; i++)
		{
			const auto player = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwEntityList + i * 0x10);

			if (!player)
				continue;

			const auto team = mem.Read<std::int32_t>(player + offsets::m_iTeamNum);

			if (team == localTeam)
				continue;

			const auto lifeState = mem.Read<std::int32_t>(player + offsets::m_lifeState);

			if (lifeState != 0)
				continue;

			if (config_system.item.bGlow)
			{
				const auto glowIndex = mem.Read<std::int32_t>(player + offsets::m_iGlowIndex);

				mem.Write(glowManager + (glowIndex * 0x38) + 0x8, config_system.item.fGlowColor[0]); // red
				mem.Write(glowManager + (glowIndex * 0x38) + 0xC, config_system.item.fGlowColor[1]); // blue
				mem.Write(glowManager + (glowIndex * 0x38) + 0x10, config_system.item.fGlowColor[2]);// green
				mem.Write(glowManager + (glowIndex * 0x38) + 0x14, config_system.item.fGlowColor[3]);// alpha

				mem.Write(glowManager + (glowIndex * 0x38) + 0x28, true);
				mem.Write(glowManager + (glowIndex * 0x38) + 0x29, false);
			}

			if (config_system.item.bRadar)
				mem.Write(player + offsets::m_bSpotted, true);
		}

		if (config_system.item.bThirdperson)
		{
			static bool once = false;
			static bool toggled = false;

			if (GetAsyncKeyState(0x43) != 0) //C Key
			{
				once = true;
			}
			else if (once)
			{
				once = false;
				toggled = !toggled;
			}

			if (toggled)
				mem.Write<int>(localPlayer + offsets::m_iObserverMode, 1);
			else
				mem.Write<int>(localPlayer + offsets::m_iObserverMode, 0);
		}

		const auto _ogThis = static_cast<std::uintptr_t>(globals::engineAdress + offsets::model_ambient_min - 0x2c);

		if (config_system.item.bModelAmbient)
		{
			const auto _this = static_cast<std::uintptr_t>(globals::engineAdress + offsets::model_ambient_min - 0x2c);
			mem.Write<std::int32_t>(globals::engineAdress + offsets::model_ambient_min, *reinterpret_cast<std::uintptr_t*>(&config_system.item.fModelAmbient) ^ _this);
		}
		else
		{
			mem.Write<std::int32_t>(globals::engineAdress + offsets::model_ambient_min, _ogThis);
		}

		if (config_system.item.bNoFlash)
		{
			mem.Write<float>(localPlayer + offsets::m_flFlashMaxAlpha, config_system.item.fNoFlash);
		}
		else
		{
			mem.Write<float>(localPlayer + offsets::m_flFlashMaxAlpha, 255.0f);
		}
	}
}

void hacks::MovementThread(const Memory& mem) noexcept
{
	while (gui::isRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (config_system.item.bBhop)
		{
			const auto localPlayer = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer);

			if (localPlayer)
			{
				const auto onGround = mem.Read<bool>(localPlayer + offsets::m_fFlags);

				if (GetAsyncKeyState(VK_SPACE) && onGround & (1 << 0))
					mem.Write<BYTE>(globals::clientAdress + offsets::dwForceJump, 6);
			}
		}
	}
}

void hacks::LegitBotThread(const Memory& mem) noexcept
{
	while (gui::isRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));


		// Triggerbot
		if (config_system.item.bTriggerbot)
		{
			if (!GetAsyncKeyState(VK_SHIFT))
				continue;

			const auto localPlayer = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer);

			if (!localPlayer)
				continue;

			const auto localHealth = mem.Read<std::int32_t>(localPlayer + offsets::m_iHealth);

			if (!localHealth)
				continue;

			const auto crosshairId = mem.Read<std::int32_t>(localPlayer + offsets::m_iCrosshairId);

			if (!crosshairId || crosshairId > 64)
				continue;

			const auto player = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwEntityList + (crosshairId - 1) * 0x10);

			if (!mem.Read<std::int32_t>(player + offsets::m_iHealth))
				continue;

			if (mem.Read<std::int32_t>(player + offsets::m_iTeamNum) ==
				mem.Read<std::int32_t>(localPlayer + offsets::m_iTeamNum))
				continue;

			mem.Write<std::uint32_t>(globals::clientAdress + offsets::dwForceAttack, 6);
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			mem.Write<std::uint32_t>(globals::clientAdress + offsets::dwForceAttack, 4);
		}
		

		// Aimbot
		if (config_system.item.bAimbot)
		{
			if (config_system.item.bMagnetTrigger == false) {

				if (!GetAsyncKeyState(VK_LBUTTON))
					continue;

			}
			
			
			const auto localPlayer = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer);
			const auto localTeam = mem.Read<std::int32_t>(localPlayer + offsets::m_iTeamNum);

			const auto localEyePosition = mem.Read<Vector3>(localPlayer + offsets::m_vecOrigin) +
				mem.Read<Vector3>(localPlayer + offsets::m_vecViewOffset);

			const auto clientState = mem.Read<std::uintptr_t>(globals::engineAdress + offsets::dwClientState);

			const auto localPlayerId =
				mem.Read<std::int32_t>(clientState + offsets::dwClientState_GetLocalPlayer);

			const auto viewAngles = mem.Read<Vector3>(clientState + offsets::dwClientState_ViewAngles);
			const auto aimPunch = mem.Read<Vector3>(localPlayer + offsets::m_aimPunchAngle) * 2;

			auto bestFov = config_system.item.fFov;
			auto bestAngle = Vector3{ };

			for (auto i = 1; i <= 32; ++i)
			{
				const auto player = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwEntityList + i * 0x10);

				if (mem.Read<std::int32_t>(player + offsets::m_iTeamNum) == localTeam)
					continue;

				if (mem.Read<bool>(player + offsets::m_bDormant))
					continue;

				if (mem.Read<std::int32_t>(player + offsets::m_lifeState))
					continue;

				if (mem.Read<std::int32_t>(player + offsets::m_bSpottedByMask) & (1 << localPlayerId))
				{
					const auto boneMatrix = mem.Read<std::uintptr_t>(player + offsets::m_dwBoneMatrix);

					// pos of player head in 3d space
					// 8 is the head bone index :)
					const auto playerHeadPosition = Vector3{
						mem.Read<float>(boneMatrix + 0x30 * 8 + 0x0C),
						mem.Read<float>(boneMatrix + 0x30 * 8 + 0x1C),
						mem.Read<float>(boneMatrix + 0x30 * 8 + 0x2C)
					};

					const auto angle = CalculateAngle(
						localEyePosition,
						playerHeadPosition,
						viewAngles + aimPunch
					);

					const auto fov = std::hypot(angle.x, angle.y);

					if (fov < bestFov)
					{
						bestFov = fov;
						bestAngle = angle;
					}
				}
			}

			// if we have a best angle, do aimbot
			if (!bestAngle.IsZero())
			{
				mem.Write<Vector3>(clientState + offsets::dwClientState_ViewAngles, viewAngles + bestAngle / config_system.item.fSmooth); // smoothing

				// Autoshoot
				if (config_system.item.bAutoshoot)
				{
					const auto crosshairId = mem.Read<std::int32_t>(localPlayer + offsets::m_iCrosshairId);

					if (!crosshairId || crosshairId > 64)
						continue;

					const auto player = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwEntityList + (crosshairId - 1) * 0x10);

					if (!mem.Read<std::int32_t>(player + offsets::m_iHealth))
						continue;

					if (mem.Read<std::int32_t>(player + offsets::m_iTeamNum) ==
						mem.Read<std::int32_t>(localPlayer + offsets::m_iTeamNum))
						continue;

					mem.Write<std::uint32_t>(globals::clientAdress + offsets::dwForceAttack, 6);
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
					mem.Write<std::uint32_t>(globals::clientAdress + offsets::dwForceAttack, 4);
				}
			}
		}
	}
}

//void hacks::SkinChangerThread(const Memory& mem) noexcept
//{
//	enum AllWeaponsIDs
//	{
//		WEAPON_AWP = 9,
//		WEAPON_G3SG1 = 11,
//		WEAPON_SCAR20 = 38,
//		WEAPON_SSG = 40,
//		WEAPON_AK47 = 7,
//		WEAPON_AUG = 8,
//		WEAPON_FAMAS = 10,
//		WEAPON_GALILAR = 13,
//		WEAPON_M4A1 = 60,
//		WEAPON_M4A4 = 16,
//		WEAPON_SG553 = 39,
//		WEAPON_USP = 61,
//		WEAPON_REVOLVER = 64,
//		WEAPON_CZ75 = 63,
//		WEAPON_DEAGLE = 1,
//		WEAPON_BERETTAS = 2,
//		WEAPON_FIVESEVEN = 3,
//		WEAPON_GLOCK = 4,
//		WEAPON_P2000 = 32,
//		WEAPON_P250 = 36,
//		WEAPON_TEC9 = 30,
//		WEAPON_MAC10 = 17,
//		WEAPON_MP7 = 33,
//		WEAPON_MP9 = 34,
//		WEAPON_MP5 = 23,
//		WEAPON_PPBIZON = 26,
//		WEAPON_P90 = 19,
//		WEAPON_UMP45 = 24,
//		WEAPON_MAG7 = 27,
//		WEAPON_NOVA = 35,
//		WEAPON_SAWEDOFF = 29,
//		WEAPON_XM1014 = 25,
//		WEAPON_M249 = 14,
//		WEAPON_NEGEV = 28,
//
//		WEAPON_KNIFE_T = 59,
//		WEAPON_KNIFE_CT = 42,
//		WEAPON_KNIFE_KARAM = 507,
//		WEAPON_KNIFE_BAYONET = 500,
//		WEAPON_KNIFE_CLASSIC = 503,
//		WEAPON_KNIFE_FLIP = 505,
//		WEAPON_KNIFE_GUT = 506,
//		WEAPON_KNIFE_M9 = 508,
//		WEAPON_KNIFE_HUNTSMAN = 509,
//		WEAPON_KNIFE_FALCHION = 512,
//		WEAPON_KNIFE_BOWIE = 514,
//		WEAPON_KNIFE_BUTTERFLY = 515,
//		WEAPON_KNIFE_DAGGERS = 516,
//		WEAPON_KNIFE_PARACORD = 517,
//		WEAPON_KNIFE_SURVIVAL = 518,
//		WEAPON_KNIFE_URSUS = 519,
//		WEAPON_KNIFE_NAVAJA = 520,
//		WEAPON_KNIFE_NOMAD = 521,
//		WEAPON_KNIFE_STILETTO = 522,
//		WEAPON_KNIFE_TALON = 523,
//		WEAPON_KNIFE_SKELETON = 525,
//	};
//
//	for (int i = 0; i < 3; i++)
//	{
//		// Get Weapon Index
//		DWORD WeaponIndex = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer + offsets::m_hMyWeapons + i * 0x4) & 0xFFFF;
//		// Get EntityList
//		DWORD EntityList = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwEntityList + (WeaponIndex - 1) * 0x10);
//		// Get Current Weapon ID in loop
//		DWORD CurrentWeaponID = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwEntityList + offsets::m_hActiveWeapon);
//
//		if (CurrentWeaponID == WEAPON_AK47)
//		{
//			// Get current weapon skin
//			DWORD CurrentWeaponSkin = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwEntityList + offsets::m_nFallbackPaintKit);
//			mem.Write(offsets::dwEntityList + offsets::m_nFallbackPaintKit, WEAPON_AK47);
//		}
//	}
//}