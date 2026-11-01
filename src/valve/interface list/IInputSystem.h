#pragma once

class IInputSystem
{
public:
	bool IsRelativeMouseMode()
	{

		return *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + 0x4D);
	}

	double GetCurrentTickTime()
	{
		return *reinterpret_cast<double*>(reinterpret_cast<uintptr_t>(this) + 0x2538);
	}

	void* GetSDLWindow()
	{

		return *reinterpret_cast<void**>(reinterpret_cast<std::uintptr_t>(this) + 0x26A8);
	}
};