#pragma once

class CZeusBot
{
public:
	void Run();

private:
	bool CanAttack();
	bool IsEnemyInRange(C_CSPlayerPawn* pEnemy, float flRange);
	C_CSPlayerPawn* GetBestTarget();

	C_CSPlayerPawn* m_pLastTarget = nullptr;
};

inline std::unique_ptr<CZeusBot> g_ZeusBot = std::make_unique<CZeusBot>();

