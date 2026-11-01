#pragma once
#include <memory>

class CFSNHandler {
public:

	void OnPreOriginal(EClientFrameStage nStage) {
		switch (nStage)
		{
		case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
		{
			InventoryChanger::Run();

			break;
		}
		case FRAME_NET_UPDATE_END:
		{
			g_ParticleMgr->UpdateParticles();
			break;
		}
		default:
			break;
		}
	}

};

inline auto g_FSN = std::make_unique<CFSNHandler>();