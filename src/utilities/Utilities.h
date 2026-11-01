#pragma once

static float CCW(const Vector& a, const Vector& b, const Vector& c)
{
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

struct ccwSorter
{
	const Vector& pivot;

	ccwSorter(const Vector& inPivot) : pivot(inPivot) { }

	bool operator()(const Vector& a, const Vector& b) {
		return CCW(pivot, a, b) < 0;
	}
};

static bool IsLeftOfPoint(const Vector& a, const Vector& b)
{
	return (a.x < b.x || (a.x == b.x && a.y < b.y));
}

namespace Utilities
{

	void CopyToClipboard(std::string strData);

	std::vector<Vector> GiftWrappingAlgorithm(std::vector<Vector> vecVector);

	bool LineGoesThroughSmoke(const Vector& vecStart, const Vector& vecEnd, const float& flMaxDensity);

	void SetLocalPlayerReady();

	void FlashWindow(HWND pWindow);

	void* LoadFileForMe(const std::string_view strPath, int& nFileSize);

	std::uintptr_t* FindHudElement(const char* szName);

	void HudChatPrint(const char* szString);

	bool ForceHoldKnife();

	int HitboxToHitgroup(EHitBoxes index);

	std::string GetHitgroupName(int nHitgroupIndex);

	void PlayVSound(const char* szSoundPath, float flVolume);

	inline CEventListener m_EventListener;
}