#pragma once

namespace VTABLE
{
    namespace D3D
    {
        enum
        {
            PRESENT = 8U,
            RESIZEBUFFERS = 13U,
        };
    }

    namespace DXGI
    {
        enum
        {
            CREATESWAPCHAIN = 10U,
        };
    }

    namespace CLIENT
    {
        enum
        {
            CREATEMOVEPREPREDICTION = 5U,
            MOUSEINPUTENABLED = 23U,
            CREATEMOVEPOSTPREDICTION = 21U,
            FRAMESTAGENOTIFY = 36U,
        };
    }

    namespace CLIENTMODE
    {
        enum
        {
			OVERRIDEVIEW = 15U,
            LEVELINIT = 23U,
            LEVELSHUTDOWN = 24U,
		};
	}

    namespace ENTITYSYSTEM
    {
        enum
        {
            ONADDENTITY = 15U,
            ONREMOVEENTITY = 16U,
        };
    }

    namespace STEAMGAMECOORDINATOR
    {
        enum
        {
            SENDMESSAGEGC = 0U,
            RETRIEVEMESSAGEGC = 2U,
        };
    }

    namespace CCSPLAYERPAWN
    {
        enum
        {
			SHOULDDRAW = 97U,
		};
	}

    namespace CCSMOVEMENTSERVICES
    {
        enum
        {
            QUANTIZEINPUT = 62U,
        };
    }
}

namespace Detours
{
    inline CDetourHook Present = CDetourHook(X("Present"));
    inline CDetourHook ResizeBuffers = CDetourHook(X("ResizeBuffers"));
    inline CDetourHook CreateSwapChain = CDetourHook(X("CreateSwapChain"));

    inline CDetourHook CreateMovePrePrediction = CDetourHook(X("CreateMovePrePrediction"));
    inline CDetourHook CreateMovePostPrediction = CDetourHook(X("CreateMovePostPrediction"));

    inline CDetourHook MouseInputEnabled = CDetourHook(X("MouseInputEnabled"));

    inline CDetourHook FrameStageNotify = CDetourHook(X("FrameStageNotify"));

    inline CDetourHook LevelInit = CDetourHook(X("LevelInit"));
    inline CDetourHook LevelShutDown = CDetourHook(X("LevelShutDown"));

    inline CDetourHook OverrideView = CDetourHook(X("OverrideView"));

    inline CDetourHook OnAddEntity = CDetourHook(X("OnAddEntity"));
    inline CDetourHook OnRemoveEntity = CDetourHook(X("OnRemoveEntity"));

    inline CDetourHook GetMatrixForView = CDetourHook(X("GetMatrixForView"));
    inline CDetourHook OnRenderStart = CDetourHook(X("OnRenderStart"));

    inline CDetourHook SetWorldFov = CDetourHook(X("SetWorldFov"));

    inline CDetourHook DrawParticleEffect = CDetourHook(X("DrawParticleEffect"));
    inline CDetourHook InputParser = CDetourHook(X("InputParser"));

    inline CDetourHook DrawSmokeVertex = CDetourHook(X("DrawSmokeVertex"));

    inline CDetourHook NiggerKickFunction = CDetourHook(X("NiggerKickFunction"));

    inline CDetourHook IsLoadoutAllowed = CDetourHook(X("IsLoadoutAllowed"));

    inline CDetourHook QuantizeMovement = CDetourHook(X("QuantizeMovement"));

    inline CDetourHook SendMessageGC = CDetourHook(X("SendMessageGC"));
    inline CDetourHook RetrieveMessageGC = CDetourHook(X("RetrieveMessageGC"));

    inline CDetourHook PanoramaEvent = CDetourHook(X("PanoramaEvent"));

    inline CDetourHook CVarGetValue = CDetourHook(X("CVarGetValue"));

    inline CDetourHook ShouldDraw = CDetourHook(X("ShouldDraw"));
    inline CDetourHook UpdatePostProcessing = CDetourHook(X("UpdatePostProcessing"));
    inline CDetourHook RenderBatchList = CDetourHook(X("DrawArray"));
    inline CDetourHook PrecacheDefaultBeam = CDetourHook(X("PrecacheDefaultBeam"));
    inline CDetourHook SortSceneObjects = CDetourHook(X("SortSceneObjects"));
    inline CDetourHook DrawPrimitive = CDetourHook(X("DrawPrimitive"));
    inline CDetourHook GeneratePrimitives = CDetourHook(X("GeneratePrimitives"));
    inline CDetourHook DrawArray = CDetourHook(X("DrawArray"));
    inline CDetourHook DrawArrayLight = CDetourHook(X("DrawArrayLight"));
    inline CDetourHook UpdateLightShadowMap = CDetourHook(X("UpdateLightShadowMap"));
    inline CDetourHook CreateAnimatableSceneObject = CDetourHook(X("CreateAnimatableSceneObject"));
    inline CDetourHook SkyboxDrawarray = CDetourHook(X("SkyboxDrawarray"));
    inline CDetourHook ParticleDrawArray = CDetourHook(X("ParticleDrawArray"));
    inline CDetourHook SetPostProcessVec = CDetourHook(X("SetPostProcessVec"));
    inline CDetourHook EmitSound = CDetourHook(X("EmitSound"));
    inline CDetourHook SomeUtlSymbolFunc = CDetourHook(X("SomeUtlSymbolFunc"));
    inline CDetourHook BloodEffectHit = CDetourHook(X("BloodEffectHit"));
    inline CDetourHook SomeRainShit = CDetourHook(X("SomeRainShit"));
    inline CDetourHook PlayVSound = CDetourHook(X("PlayVSound"));
    inline CDetourHook DecreaseSpeed = CDetourHook(X("DecreaseSpeed"));
    inline CDetourHook RunCommand = CDetourHook(X("RunCommand"));
    inline CDetourHook UpdateSkybox = CDetourHook(X("UpdateSkybox"));
    inline CDetourHook DrawParticleChildrenSeparateFromParents = CDetourHook(X("DrawParticleChildrenSeparateFromParents"));
    inline CDetourHook GetParticleColor = CDetourHook(X("GetParticleColor"));
    inline CDetourHook AODrawArray = CDetourHook(X("AODrawArray"));
    inline CDetourHook DrawArray2 = CDetourHook(X("DrawArray2"));

    inline CDetourHook DrawArraySceneObject = CDetourHook(X("DrawArraySceneObject"));

    inline CDetourHook DrawArrayFire = CDetourHook(X("DrawArrayFire"));

    inline CDetourHook FlashOverlay = CDetourHook(X("FlashOverlay"));
    inline CDetourHook CalcViewmodel = CDetourHook(X("CalcViewmodel"));
    inline CDetourHook GetFlashOpacity = CDetourHook(X("GetFlashOpacity"));

    inline CDetourHook RemoveLegs = CDetourHook(X("RemoveLegs"));

    inline CDetourHook Skybox = CDetourHook(X("Skybox"));
    inline CDetourHook DrawAggregateSceneObject = CDetourHook(X("DrawAggregateSceneObject"));
    inline CDetourHook AllowCameraAngleChange = CDetourHook(X("AllowCameraAngleChange"));

    inline CDetourHook IsRelativeMouseMode = CDetourHook(X("IsRelativeMouseMode"));

    inline CDetourHook DisableServerAnimation = CDetourHook(X("DisableServerAnimation"));

    inline CDetourHook GetInfernoType = CDetourHook(X("GetInfernoType"));

    inline CDetourHook MergeBones = CDetourHook(X("MergeBones"));
    inline CDetourHook WhatTheFuckIsThisEven = CDetourHook(X("WhatTheFuckIsThisEven"));

    inline CDetourHook Firebullet = CDetourHook(X("Firebullet"));
    inline CDetourHook PlayGunSound = CDetourHook(X("PlayGunSound"));

    inline CDetourHook UpdateAggeregateObject = CDetourHook(X("UpdateAggeregateObject"));
    inline CDetourHook DrawAggeregateObject = CDetourHook(X("DrawAggeregateObject"));

    inline CDetourHook IsGlowing = CDetourHook(X("IsGlowing"));
    inline CDetourHook GetGlowColor = CDetourHook(X("GetGlowColor"));
    inline CDetourHook CreateMeshInstance = CDetourHook(X("CreateMeshInstance"));
    inline CDetourHook FreeMeshInstance = CDetourHook(X("FreeMeshInstance"));
    inline CDetourHook ReportHit = CDetourHook(X("ReportHit"));
    inline CDetourHook GetInterpolatedShootPosition = CDetourHook(X("GetInterpolatedShootPosition"));
    inline CDetourHook CAnimatableSceneObjectDesc__FindOrAllocateTransforms = CDetourHook(X("CAnimatableSceneObjectDesc__FindOrAllocateTransforms"));
    inline CDetourHook ButtonsShit = CDetourHook(X("ButtonsShit"));
    inline CDetourHook GaySexualShit = CDetourHook(X("GaySexualShit"));
    inline CDetourHook DrawOverHead = CDetourHook(X("DrawOverHead"));
    inline CDetourHook ToneMap = CDetourHook(X("ToneMap"));
    inline CDetourHook DrawCrosshair = CDetourHook(X("DrawCrosshair"));
    inline CDetourHook FirstPersonLegs = CDetourHook( X( "FirstPersonLegs" ) );
    inline CDetourHook HandleTeamIntro = CDetourHook( X( "HandleTeamIntro" ) );
    inline CDetourHook PrepareSceneMaterial = CDetourHook( X( "PrepareSceneMaterial" ) );
    inline CDetourHook CreateSceneObjectFromModelHandle = CDetourHook( X( "CreateSceneObjectFromModelHandle" ) );
    inline CDetourHook DrawScopeOverlay = CDetourHook(X("DrawScopeOverlay"));
    inline CDetourHook ConsoleLog = CDetourHook(X("ConsoleLog"));
}

#define FASTCALL __fastcall
#define STDCALL __stdcall

namespace Hooks
{
    bool Setup();
    void Destroy();

    HRESULT WINAPI hkPresent(IDXGISwapChain* pSwapChain, UINT uSyncInterval, UINT uFlags);
    HRESULT FASTCALL hkResizeBuffers(IDXGISwapChain* pSwapChain, std::uint32_t nBufferCount, std::uint32_t nWidth, std::uint32_t nHeight, DXGI_FORMAT newFormat, std::uint32_t nFlags);
    HRESULT WINAPI hkCreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain);
    long CALLBACK hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static __int64 hkCAnimatableSceneObjectDesc__FindOrAllocateTransforms(__int64 CAnimatableSceneObjectDesc, cs2::animatable_scene_object* animatable_scene_object, cs2::mesh_instance* CMeshInstance_Imp, __int64 a4, __int64 a5, int a6, const char* a7, __int64* a8, __int64* a9);

    void FASTCALL hkCreateMovePrePrediction(CCSGOInput* pInput, int nSlot, bool bActive);
    bool FASTCALL hkCreateMovePostPrediction(CCSGOInput* pInput, unsigned int nSlot, CUserCmd* pCmd);
    bool FASTCALL hkMouseInputEnabled(void* pRcx);
    void FASTCALL hkFrameStageNotify(void* pRcx, EClientFrameStage nFrameStage);

    void STDCALL hkLevelInit(IClientMode* pClientModeShared, const char* szNewMap);
    void STDCALL hkLevelShutDown(IClientMode* pClientModeShared);

    void FASTCALL hkOverrideView(void* pClientModeCSNormal, CViewSetup* pSetup);

    void* FASTCALL hkOnAddEntity(void* pRcx, CEntityInstance* pInstance, CBaseHandle uHandle);
    void* FASTCALL hkOnRemoveEntity(void* pRcx, CEntityInstance* pInstance, CBaseHandle uHandle);

    ViewMatrix_t* FASTCALL hkGetMatrixForView( CRenderGameSystem* pRenderGameSystem, IViewRender* pViewRender, ViewMatrix_t* pOutWorldToView, ViewMatrix_t* pOutViewToProjection, ViewMatrix_t* pOutWorldToProjection, ViewMatrix_t* pOutWorldToPixels );

    bool FASTCALL hkIsLoadoutAllowed();
    cs2::mesh_instance* FASTCALL hkCreateMeshInstance(__int64 CMeshSystem, __int64 CBaseSceneObjectDesc, int maybe_spawn_tick, __int64 model_instance_thing, __int16 zero, __int64 null_mem_addr, __int64 CSceneObject, __int64 minus_one, char minus_one2, __int64 zero_val);
    void* __fastcall hkFreeMeshInstance(void* a1, char a2);
    _QWORD* FASTCALL hkReportHit(__int64 a1);

    void FASTCALL hkQuantizeMovement(uintptr_t a1, uintptr_t a2);
    void FASTCALL hkDrawScopeOverlay(void* a1, void* a2);
    int FASTCALL hkConsoleLog(__int64 a1, int channel, int serverity, __int64 color_mb, const char* text, __int64* a6);
    int	FASTCALL hkSendMessageGC(ISteamGameCoordinator* thisptr, std::uint32_t uMsgType, const void* pData, std::uint32_t uData);
    int	FASTCALL hkRetrieveMessageGC(ISteamGameCoordinator* thisptr, std::uint32_t* puMsgType, void* pDest, std::uint32_t uDest, std::uint32_t* puMsgSize);

    __int64 FASTCALL hkPanoramaEvent(void* pUnk, const char* szEventName, void* pUnk1, float flUnk);

    void* FASTCALL hkCVarGetValue(void* pRcx, const char* szCharacter, int iDefaultValue);

    void* FASTCALL hkUpdateAggeregateObject(void* a1, void* a2,CAggregateSceneObjectWorld* a3);
    void* FASTCALL hkDrawAggeregateObject(void* a1, void* a2, CDrawMesh* a3, int a4, int a5, void* a6, void* a7, void* a8);
    void FASTCALL DrawAggregateSceneObject(void* a1, void* a2, c_aggregate_object_array* a3);
    void* FASTCALL hkRenderBatchList(void* a1);

    void FASTCALL hkSortSceneObjects(__int64 a1, __int64 _pSceneLayer, __int64 _pDrawMesh, unsigned int nDataCount);
    void* FASTCALL hkDrawPrimitive(void* a1, void* pDx11, void* pViewport, CDrawMesh* pDrawMesh, CMaterial2* pMaterial, CMaterial2* pMaterial2, void** pDrawParam, void* a8, void* a9, void* pSceneLayer);
    void FASTCALL hkGeneratePrimitives(CAnimatableSceneObjectDesc* pDesc, CSceneAnimatableObject* pObject, void* a3, CMeshPrimitiveOutputBuffer* pRenderBuffer);
    void FASTCALL hkDrawArray(CAnimatableSceneObjectDesc* pAnimatableSceneObjectDesc, void* pDx11, CDrawMesh* pMaterialData, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2);
    void FASTCALL hkDrawArrayLight(void* pLightBinnerGPU, CAggregateSceneObject* pAggregateSceneObject, void* pUnk);
    void FASTCALL hkUpdateLightShadowMap(CLightBinnerGpu* pLightBinnerGPU, void* a2);
    void FASTCALL hkCreateAnimatableSceneObject(CModelState* pModelState, void* a2, void* a3);
    void hkTonemapSystemRenderDebug(void* a1, void* a2, void* a3);
    bool FASTCALL hkDrawCrosshair(void* a1);
    double FASTCALL hkPrecacheDefaultBeam(void* pBeam, void* pUnk);
    void FASTCALL hkSkyboxDrawarray(__int64 pObjectDesc, __int64 a2, __int64 pSceneData, int iDataCount, int a5, __int64 a6, __int64 a7);
    void* FASTCALL hkParticleDrawArray(void* pParticleObjectDesc, void* a2, void* a3, int nDataCount, void* a5, void* a6, void* a7);
    void FASTCALL hkSetPostProcessVec(void* a1, unsigned int uHash, Vector4D* vVal);
    _DWORD* FASTCALL hkEmitSound(_DWORD* a1, __int64 a2, unsigned int iEntIndex, __int64 pEmitSound);
    __int64 FASTCALL hkSomeUtlSymbolFunc(__int64 pVSoundTypeManager, int a2);
    double FASTCALL hkBloodEffectHit(void* pUnk);
    void FASTCALL hkSomeRainShit(void* pPrecipitation, void* pLocalEntity, int nSomethingAlways0);
    void FASTCALL hkPlayVSound(const char* szSound);
    void FASTCALL hkDecreaseSpeed(void* pMovementServices, void* pUnk);
    void* FASTCALL hkDrawParticleChildrenSeparateFromParents(void* unkptr, void* pDrawArray);
    CParticleColor FASTCALL hkGetParticleColor(CParticleColor pOutputColor, unsigned __int8* pOriginalParticleColor);
    void* FASTCALL hkAODrawArray(void* pAmbientOcclusionProxySceneObjectDesc, void* a2, void* a3, int nDataCount, void* a5, void* a6, void* a7);

    void FASTCALL hkDrawArraySceneObject(void* a1, void* a2);

    void* FASTCALL hkDrawArrayFire(void* a1, void* a2, CDrawMesh* a3, int a4, void* a5, void* a6);

    void* FASTCALL hkFlashOverlay(__int64 a1, int a2, __int64* a3, __int64 a4, float a5[4]);

    void* FASTCALL hkCalcViewmodel(float* flUnk, float* flOffsets, float* flFov);

    __int64 FASTCALL hkGetInterpolatedShootPosition(CPlayer_WeaponServices* pWeaponServices, Vector* vecEyePosition, int nShootTick, float flFraction);

    static bool FASTCALL hkRemoveLegs(C_CSPlayerPawn pPawn);
    void FASTCALL hkGaySexualShit(__int64 a1, float* a2);
    void FASTCALL hkAllowCameraAngleChange(CCSGOInput* pCSGOInput, int a2);
    void FASTCALL hkUpdatePostProcessing(C_PostProcessingVolume* a1, int a2);
    void* FASTCALL hkIsRelativeMouseMode(void* pThisptr, bool bActive);

    void FASTCALL hkOnRenderStart(CViewRender* pViewRender);

    float FASTCALL hkSetWorldFov(void* pRcx);

    void* FASTCALL hkDrawParticleEffect(const char* szEffect, __int64 a2, __int64* angSomeAngles_1, __int64 a4, char a5, unsigned int a6, __int64 a7, bool a8, __int64 a9, __int64 a10, __int64 a11, __int64 a12);
    void* FASTCALL hkInputParser(CCSInputMessage* pInputMessage, CSGOInputHistoryEntryPB* pInputHistory, bool bSomeBool, TimeStamp_t a4, TimeStamp_t a5, C_CSPlayerPawn* pLocalPlayer);

    void* FASTCALL hkDrawSmokeVertex(void* a1, void* a2, int a3, int a4, void* a5, void* a6, void* a7, void* a8, void* a9, void* a10);

    const char* FASTCALL hkGetInfernoType(__int64 a1);
    __int64 FASTCALL hkButtonsShit(_QWORD* a1, __int64 a2, char a3);
    void FASTCALL hkMergeBones(CSkeletonInstance* pSkeletonInstance, uint32_t iUnk);
    bool FASTCALL hkWhatTheFuckIsThisEven(CCSGOInput* input, __int64 unk, CUserCmd* a3, CUserCmd* a4);

    void FASTCALL hkRunCommand(CCSPlayer_MovementServices* pMovementServices, CUserCmd* pUserCmd);

    void* FASTCALL hkUpdateSkybox(ISceneSystem* pSceneSystem, CMaterial2** pMaterial, void* pSceneWorld);

    __int64 FASTCALL hkFirebullet(CCSPlayer_BulletServices* pBulletServices, Vector* vecStart, int* a3, float flRange, float flPenetration, int iPenCount, unsigned __int8 nPrimaryAmmoType, unsigned int nDamage, int nFirebulletCounter, float flRangeModifier, C_CSPlayerPawn* pPawn, unsigned __int8 unk_always_1, float flUnkArg, float flUnkArg2, C_BasePlayerWeapon* pWeapon, CCSWeaponBaseVData* pWeaponData, int a17, int a18, __int64 a19);

    int FASTCALL hkPlayGunSound(int a1, void* a2, int a3, __int64 a4, int a5, int a6, int a7);

    void FASTCALL hkNiggerKickFunction(__int64 a1, unsigned int a2);

    void* FASTCALL hkDisableServerAnimation(void* a1, void* a2, void* a3);

    bool FASTCALL hkIsGlowing(CGlowProperty* pGlow);

    void FASTCALL hkGetGlowColor(CGlowProperty* pGlow, float* pColor);

    bool FASTCALL hkDrawOverHead(C_CSPlayerPawn* a1);

    void* FASTCALL hkDrawFirstPersonLegs( void* a1, void* a2, void* a3, void* a4, void* a5 );
    void FASTCALL hkHandleTeamIntro( void* a1, void* a2, bool* pbTeamIntroEnd );
    float FASTCALL hkPrepareSceneMaterial( CMaterial2* pMaterial, void* pData, float flNull );
    void* FASTCALL hkCreateSceneObjectFromModelHandle( void* pMeshSystem, int** pp_hModel_arg, float* pUnkStruct, const char* szSceneObjectType_arg, __int64 someValBasedOnWorldGroupId, __int64 someMagicNum, __int64 pSomeRenderValuePointer );

    inline std::vector<CDetourHook> vecDetours = {};
    
    inline void SubscribeHook(CDetourHook& detour, const bool create = true)
    {

        if (detour.bIsHooked)
            return;

        bool bFound = false;

        for (auto& d : vecDetours)
        {

            if (&d != &detour)
                continue;

            bFound = true;

            const bool bReplaceFn = d.pReplaceFn == detour.pReplaceFn;
            const bool bBaseFn = d.pBaseFn == detour.pBaseFn;
            const bool bOriginalFn = d.pOriginalFn == detour.pOriginalFn;

            if (create && bReplaceFn && (!bBaseFn || !bOriginalFn || d.pBaseFn == nullptr || d.pOriginalFn == nullptr))
                d.Create();

            else throw std::runtime_error(X("unknown error while subscribing hook"));
        }

        if (!bFound)
        {
            vecDetours.emplace_back(detour);
            if (create)
                detour.Create();
        }
    }

    inline void RevokeHook(CDetourHook& detour)
    {
        for (const auto& d : vecDetours)
        {
            const bool bReplaceFn = d.pReplaceFn == detour.pReplaceFn;
            const bool bBaseFn = d.pBaseFn == detour.pBaseFn;
            const bool bOriginalFn = d.pOriginalFn == detour.pOriginalFn;

            if (!bReplaceFn || !bBaseFn || !bOriginalFn)
                continue;

            detour.Restore();
            detour.~CDetourHook();
        }
    }
}

extern bool m_bConsoleLog;