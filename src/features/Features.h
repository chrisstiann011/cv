#pragma once

static inline CStrongHandle<CMaterial2> skymat;

void ChangeSkyboxMaterial();

struct Hitmarker_t
{
	Vector m_vecWorldPosition;
	float m_flTimeAdded;
};

inline std::deque<Hitmarker_t> m_deqHitmarkerPositions;

struct HitLogEntry {
	std::string message;
	float startTime;
	float fadeStartTime;
	float fadeDuration;
};

inline std::vector<HitLogEntry> hitLogs;

inline void AddHitLog(const std::string& message) {
	HitLogEntry entry;
	entry.message = message;
	entry.startTime = Interfaces::m_pGlobalVariables->m_flCurrentTime;
	entry.fadeStartTime = entry.startTime + 3.0f;
	entry.fadeDuration = 0.5f;
	hitLogs.push_back(entry);
}

inline void DrawCircle(const Vector& center, float radius, Color color)
{
	std::vector<ImVec2> circlePoints;

	for (int i = 0; i < 50; i++) {
		float theta = 2.0f * M_PI * float(i) / float(50);
		float x = radius * cosf(theta);
		float y = radius * sinf(theta);

		Vector point3D = center + Vector(x, y, 0);
		ImVec2 point2D;
		if (Draw::WorldToScreen(point3D, point2D)) {
			circlePoints.push_back(point2D);
		}
	}

	for (size_t i = 0; i < circlePoints.size(); i++) {
		size_t next = (i + 1) % circlePoints.size();
		Draw::AddLine(circlePoints[i], circlePoints[next], color);
	}
}

namespace Features
{

	void OnFireEvent(CGameEvent* pEvent, bool bServerSideOnly);
	void DrawHitmarkers();
	void DrawSpeedometer();
	void OnRenderStart(CViewRender* pViewRender);
}