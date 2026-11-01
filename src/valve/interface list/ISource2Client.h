#pragma once

class CEconItemSystem;
class ISource2Client
{
public:
    CEconItemSystem* GetEconItemSystem()
    {
        return Memory::CallVFunc<CEconItemSystem*, 127>(this);
    }
	void FrameStageNotify(int iStage)
	{
		return Memory::CallVFunc<void, 36U>(this, iStage);
	}
	CDebugOverlayGameSystem* GetDebugOverlay()
	{

		return Memory::CallVFunc<CDebugOverlayGameSystem*, 160U>(this);
	}

	float CalcClampedInterp()
	{
		return Memory::CallVFunc<float, 174U>(this);
	}

};