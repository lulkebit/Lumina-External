#include "hacks.h"
#include "gui.h"
#include "globals.h"
#include "vector.h"
#include "config/config.hpp"

#include <thread>
#include <array>


constexpr Vector3 CalculateAngle(
	const Vector3& localPosition,
	const Vector3& enemyPosition,
	const Vector3& viewAngles) noexcept
{
	return ((enemyPosition - localPosition).ToAngle() - viewAngles);
}

struct Color
{
	std::uint8_t r{ }, g{ }, b{ };
};

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
			//{
				//mem.Write<std::uint8_t>(player + offsets::m_clrRender, { config_system.item.fTeamColor[0], config_system.item.fTeamColor[1], config_system.item.fTeamColor[2] });
			//}

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

			if (config_system.item.bChams)
			{

			}
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
			}
		}
	}
}

// set skins to apply here
constexpr const int GetWeaponPaint(const short& itemDefinition)
{
	switch (itemDefinition)
	{
	case 1: return 711;		// deagle
	case 2: return 396;		// duals
	case 3: return 427;		// five seven
	case 4: return 38;		// glock
	case 7: return 490;		// ak
	case 8: return 455;		// aug
	case 9: return 344;		// awp
	case 10: return 429;	// famas
	case 11: return 493;	// g3sg1
	case 13: return 494;	// galil
	case 14: return 496;	// m249
	case 16: return 309;	// m4a4
	case 17: return 433;	// mac10
	case 19: return 636;	// p90
	case 23: return 254;	// mp5-sd
	case 24: return 556;	// ump45
	case 25: return 616;	// xm1014
	case 26: return 676;	// pp bizon
	case 27: return 703;	// mag7
	case 28: return 610;	// negev
	case 29: return 256;	// sawed off
	case 30: return 644;	// tec9
	case 32: return 389;	// p2000
	case 33: return 481;	// mp7
	case 34: return 679;	// mp9
	case 35: return 62;		// nova
	case 36: return 678;	// p250
	case 38: return 165;	// scar-20
	case 39: return 686;	// sg553
	case 40: return 624;	// ssg08
	case 60: return 548;	// m4a1-s
	case 61: return 653;	// usp
	default: return 0;
	}
}

void hacks::SkinChangerThread(const Memory& mem) noexcept
{
	while (gui::isRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(2));

		const auto& localPlayer = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer);
		const auto& weapons = mem.Read<std::array<unsigned long, 8>>(localPlayer + offsets::m_hMyWeapons);

		for (const auto& handle : weapons)
		{
			const auto& weapon = mem.Read<std::uintptr_t>((globals::clientAdress + offsets::dwEntityList + (handle & 0xFFF) * 0x10) - 0x10);

			if (!weapon)
				continue;

			if (const auto paint = GetWeaponPaint(mem.Read<short>(weapon + offsets::m_iItemDefinitionIndex)))
			{
				const bool shouldUpdate = mem.Read<std::uintptr_t>(weapon + offsets::m_nFallbackPaintKit) != paint;

				mem.Write<std::uintptr_t>(weapon + offsets::m_iItemIDHigh, -1);

				mem.Write<std::uintptr_t>(weapon + offsets::m_nFallbackPaintKit, paint);
				mem.Write<float>(weapon + offsets::m_flFallbackWear, 0.1f);

				if (shouldUpdate)
					mem.Write<std::uintptr_t>(mem.Read<std::uintptr_t>(globals::engineAdress + offsets::dwClientState) + 0x174, -1);
			}
		}
	}
}