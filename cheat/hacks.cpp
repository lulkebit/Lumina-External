#include "hacks.h"
#include "gui.h"
#include "globals.h"
#include "vector.h"

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

			if (globals::glow)
			{
				const auto glowIndex = mem.Read<std::int32_t>(player + offsets::m_iGlowIndex);

				mem.Write(glowManager + (glowIndex * 0x38) + 0x8, globals::glowColor[0]); // red
				mem.Write(glowManager + (glowIndex * 0x38) + 0xC, globals::glowColor[1]); // blue
				mem.Write(glowManager + (glowIndex * 0x38) + 0x10, globals::glowColor[2]);// green
				mem.Write(glowManager + (glowIndex * 0x38) + 0x14, globals::glowColor[3]);// alpha

				mem.Write(glowManager + (glowIndex * 0x38) + 0x28, true);
				mem.Write(glowManager + (glowIndex * 0x38) + 0x29, false);
			}

			if (globals::radar)
				mem.Write(player + offsets::m_bSpotted, true);
		}

		if (globals::thirdperson)
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

		if (globals::model_ambient_tog)
		{
			const auto _this = static_cast<std::uintptr_t>(globals::engineAdress + offsets::model_ambient_min - 0x2c);
			mem.Write<std::int32_t>(globals::engineAdress + offsets::model_ambient_min, *reinterpret_cast<std::uintptr_t*>(&globals::model_ambient_val) ^ _this);
		}
		else
		{
			mem.Write<int>(globals::engineAdress + offsets::model_ambient_min, 0);
		}

		if (globals::noFlash)
		{
			mem.Write<float>(localPlayer + offsets::m_flFlashMaxAlpha, globals::flashAplha);
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

		if (globals::bhop)
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
		if (globals::triggerbot) 
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
		if (globals::aimbot)
		{
			if (globals::magnettrigger == false) {

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

			auto bestFov = globals::bestfov;
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
				mem.Write<Vector3>(clientState + offsets::dwClientState_ViewAngles, viewAngles + bestAngle / globals::smoothing); // smoothing

				// Autoshoot
				if (globals::autoshoot)
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