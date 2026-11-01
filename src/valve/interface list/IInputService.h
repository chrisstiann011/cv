#pragma once

class IInputService
{
public:
	double GetLastTickTime() noexcept
	{
		return *reinterpret_cast<double*>(reinterpret_cast<std::uintptr_t>(this) + 0x3EDD0);
	}
};