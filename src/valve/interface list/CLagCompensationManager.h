#pragma once

class CLagCompensationManager
{
public:
	void StartLagCompensation(void* a2, int a3, void* a4)
	{
		return Memory::CallVFunc<void, 58>(this, a2, a3, a4);
	}
};