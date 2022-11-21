#pragma once
#include "memory.h"

namespace hacks
{
	// run visual hacks
	void VisualsThread(const Memory& mem) noexcept;

	// run movement hacks
	void MovementThread(const Memory& mem) noexcept;

	// run legitbot hacks
	void LegitBotThread(const Memory& mem) noexcept;

	//void SkinChangerThread(const Memory& mem) noexcept;
}