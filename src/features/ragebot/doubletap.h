#pragma once

namespace Doubletap
{
    int GetPlayerTick();
    void SetHistoryTick(int nTick);
    void Process();
    void SetupInputHistory();
    void FixAnglesInHistory(QAngle angTarget);
    int GetCharge();
    void OnShot();
}