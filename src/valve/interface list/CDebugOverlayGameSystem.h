#pragma once

class CDebugOverlayGameSystem
{
public:
    void RenderWithoutDots(bool bShouldDot)
    {
        return Memory::CallVFunc<void, 10U>(this, bShouldDot);
    }

    void AddLineOverlay(const Vector& vecStart, const Vector& vecEnd, const Color& colColor, bool bNoDethTest, float flDuration)
    {
        return Memory::CallVFunc<void, 12U>(this, vecStart, vecEnd, colColor, bNoDethTest, static_cast<double>(flDuration));
    }

    void AddBoxOverlay(const Vector& vecOrigin, const Vector& vecMins, const Vector& vecMaxs, const QAngle& angRotation, const Color& colColor, float flDuration)
    {
        return Memory::CallVFunc<void, 51U>(this, vecOrigin, vecMins, vecMaxs, angRotation, colColor.Get<COLOR_R>(), colColor.Get<COLOR_G>(), colColor.Get<COLOR_B>(), colColor.Get<COLOR_A>(), static_cast<double>(flDuration));
    }

    void AddTextOverlay(const Vector& vecOrigin, float flDuration, int nLineOffset, const char* szText, ...)
    {
        return Memory::CallVFunc<void, 58U>(this, vecOrigin, nLineOffset, static_cast<double>(flDuration), szText);
    }

    void AddTextOverlay(const Vector& vecOrigin, float flDuration, const char* szText, ...)
    {
        return Memory::CallVFunc<void, 59U>(this, vecOrigin, static_cast<double>(flDuration), szText);
    }

    void AddTextOverlay(const Vector& vecOrigin, float flDuration, int nLineOffset, const Color& colColor, const char* szText...)
    {
        return Memory::CallVFunc<void, 60U>(this, vecOrigin, nLineOffset, static_cast<double>(flDuration), colColor.Get<COLOR_R>(), colColor.Get<COLOR_G>(), colColor.Get<COLOR_B>(), colColor.Get<COLOR_A>(), szText);
    }
};