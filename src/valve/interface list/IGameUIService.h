#pragma once

class IGameUIServices
{
public:
    bool IsConsoleVisible()
    {
		return Memory::CallVFunc<bool, 25U>(this, 25);
	}

    void ShowConsole(bool bState)
    {
		Memory::CallVFunc<void, 27U>(this, bState);
	}

    void ClearConsole()
    {
        Memory::CallVFunc<void, 32U>(this);
    }
};