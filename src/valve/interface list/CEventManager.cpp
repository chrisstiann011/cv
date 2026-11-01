#include "../../Precompiled.h"

void CEventListener::Setup(const std::deque<const char*>& arrEvents)
{
	if (arrEvents.empty())
		return;

	for (const char* szEvent : arrEvents)
	{
		Interfaces::m_pGameEventManager->AddListener(this, szEvent, false);

		if (!Interfaces::m_pGameEventManager->FindListener(this, szEvent))
			throw std::runtime_error(X("failed add listener"));
	}
}

void CEventListener::Destroy()
{
	Interfaces::m_pGameEventManager->RemoveListener(this);
}

void CEventListener::FireGameEvent(CGameEvent* pEvent)
{
	if (pEvent == nullptr)
		return;

	Features::OnFireEvent(pEvent, false);
}