#include "../../Precompiled.h"

static const char szLatexVmatBuffer[] = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
    shader = "csgo_character.vfx"
    F_DISABLE_Z_PREPASS = 1
    F_DISABLE_Z_WRITE = 1
    F_BLEND_MODE = 1
	F_RENDER_BACKFACES = 0

    g_vColorTint = [1.0, 1.0, 1.0, 1.0]
    g_bFogEnabled = 0
    g_flMetalness = 0.000
    g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
    g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_79a2e0d0.vtex"
    g_tNormal = resource:"materials/default/default_normal_tga_1b833b2a.vtex"
    g_tMetalness = resource:"materials/default/default_metal_tga_8fbc2820.vtex"
} )";

static const char szInvisLatexVmatBuffer[] = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
    shader = "csgo_character.vfx"
    F_DISABLE_Z_BUFFERING = 1
    F_DISABLE_Z_PREPASS = 1
    F_DISABLE_Z_WRITE = 1
    F_BLEND_MODE = 1
	F_RENDER_BACKFACES = 0

    g_vColorTint = [1.0, 1.0, 1.0, 1.0]
    g_bFogEnabled = 0
    g_flMetalness = 0.000
    g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
    g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_79a2e0d0.vtex"
    g_tNormal = resource:"materials/default/default_normal_tga_1b833b2a.vtex"
    g_tMetalness = resource:"materials/default/default_metal_tga_8fbc2820.vtex"
} )";

static const char szFlatVmatBuffer[] = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	shader = "csgo_unlitgeneric.vfx"
    g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
    g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
    g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
    g_tAmbientOcclusion = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
    g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
    F_RENDER_BACKFACES = 1
    F_DISABLE_Z_BUFFERING = 0
	F_PAINT_VERTEX_COLORS = 1
    F_TRANSLUCENT = 1
    F_BLEND_MODE = 1
    g_vColorTint = [1.0, 1.0, 1.0, 1.0]
} )";

static const char szInvisFlatVmatBuffer[] = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	shader = "csgo_unlitgeneric.vfx"
    g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
    g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
    g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
    g_tAmbientOcclusion = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
    g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
    F_RENDER_BACKFACES = 1
    F_DISABLE_Z_BUFFERING = 1
	F_PAINT_VERTEX_COLORS = 1
    F_TRANSLUCENT = 1
    F_BLEND_MODE = 1
    g_vColorTint = [1.0, 1.0, 1.0, 1.0]
} )";

static const char szBloomVmatBuffer[] = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	shader = "solidcolor.vfx"
	g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
	g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
	g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
	g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
	g_tAmbientOcclusion = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"

	F_IGNOREZ = 0
	F_DISABLE_Z_WRITE = 0
	F_RENDER_BACKFACES = 0
	g_vColorTint = [9.0, 9.0, 9.0, 9.0]
} )";

static const char szInvisBloomVmatBuffer[] = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	shader = "solidcolor.vfx"
	g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
	g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
	g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
	g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
	g_tAmbientOcclusion = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"

	F_IGNOREZ = 1
	F_DISABLE_Z_WRITE = 1
	F_DISABLE_Z_BUFFERING = 1
	F_RENDER_BACKFACES = 0
	g_vColorTint = [9.0, 9.0, 9.0, 9.0]
} )";

static const char szXrayVisible[] =
R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
		{
			shader = "csgo_effects.vfx"
			F_ADDITIVE_BLEND = 1
			F_BLEND_MODE = 1
			F_TRANSLUCENT = 1

			g_flOpacityScale = 0.45
			g_flFresnelExponent = 0.75
			g_flFresnelFalloff = 1.0
			g_flFresnelMax = 0.0
			g_flFresnelMin = 1.0
			g_flToolsVisCubemapReflectionRoughness = 1.0
			g_flBeginMixingRoughness = 1.0

			g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			g_tMask1 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			g_tMask2 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			g_tMask3 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			g_tSceneDepth = resource:"materials/default/default_mask_tga_fde710a5.vtex"

			g_vColorTint = [ 1.000000, 1.000000, 1.000000, 0 ]
		})";

static const char szXrayInvisible[] = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
		{
			shader = "csgo_effects.vfx"
			F_ADDITIVE_BLEND = 1
			F_BLEND_MODE = 1
			F_TRANSLUCENT = 1
			F_DISABLE_Z_BUFFERING = 1

			g_flOpacityScale = 0.45
			g_flFresnelExponent = 0.75
			g_flFresnelFalloff = 1.0
			g_flFresnelMax = 0.0
			g_flFresnelMin = 1.0
			g_flToolsVisCubemapReflectionRoughness = 1.0
			g_flBeginMixingRoughness = 1.0

			g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			g_tMask1 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			g_tMask2 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			g_tMask3 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			g_tSceneDepth = resource:"materials/default/default_mask_tga_fde710a5.vtex"

			g_vColorTint = [ 1.000000, 1.000000, 1.000000, 0 ]
		})";

static const char szGlowVmatBuffer[] = R"(
	<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
	{
		"shader" = "csgo_effects.vfx"

		 "g_tColor" = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"

		 "g_tMask1" = resource:"materials/default/default_mask_tga_344101f8.vtex"
		 "g_tMask2" = resource:"materials/default/default_mask_tga_344101f8.vtex"
		 "g_tMask3" = resource:"materials/default/default_mask_tga_344101f8.vtex"

		 "g_flColorBoost" = 20
		 "g_flOpacityScale" = 0.6999999
		 "g_flFresnelExponent" = 10
		 "g_flFresnelFalloff" = 10
		 "g_flFresnelMax" = 0
		 "g_flFresnelMin" = 1

		 "F_ADDITIVE_BLEND" = 1
		 "F_BLEND_MODE" = 1
		 "F_TRANSLUCENT" = 1
		 "F_IGNOREZ" = 0

		 "F_DISABLE_Z_BUFFERING" = 0

		 "F_RENDER_BACKFACES" = 0

		 "g_vColorTint" = [1.00000, 1.00000, 1.00000]
	}
)";

static const char szInvisGlowVmatBuffer[] = R"(
	<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
	{
		"shader" = "csgo_effects.vfx"

		 "g_tColor" = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"

		 "g_tMask1" = resource:"materials/default/default_mask_tga_344101f8.vtex"
		 "g_tMask2" = resource:"materials/default/default_mask_tga_344101f8.vtex"
		 "g_tMask3" = resource:"materials/default/default_mask_tga_344101f8.vtex"

		 "g_flColorBoost" = 20
		 "g_flOpacityScale" = 0.6999999
		 "g_flFresnelExponent" = 10
		 "g_flFresnelFalloff" = 10
		 "g_flFresnelMax" = 0
		 "g_flFresnelMin" = 1

		 "F_ADDITIVE_BLEND" = 1
		 "F_BLEND_MODE" = 1
		 "F_TRANSLUCENT" = 1
		 "F_IGNOREZ" = 1

		 "F_DISABLE_Z_BUFFERING" = 1

		 "F_RENDER_BACKFACES" = 0

		 "g_vColorTint" = [1.00000, 1.00000, 1.00000]
	}
)";
static constexpr char szVMatBufferFatalVisible[] =R"(
	<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
	{
		"shader" = "csgo_effects.vfx"

		"g_tColor" = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
		"g_tNormal" = resource:"materials/default/default_normal_tga_7652cb.vtex"
		"g_tMask1" = resource:"materials/default/default_mask_tga_344101f8.vtex"
		"g_tMask2" = resource:"materials/default/default_mask_tga_344101f8.vtex"
		"g_tMask3" = resource:"materials/default/default_mask_tga_344101f8.vtex"
		"g_tRoughness" = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
		"g_tMetalness" = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
		"g_tAmbientOcclusion" = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"

		"g_flColorBoost" = 30
		"g_flOpacityScale" = 245.55
		"g_flFresnelExponent" = 7.75
		"g_flFresnelFalloff" = 5
		"g_flFresnelMax" = 0.0
		"g_flFresnelMin" = 9

		"F_ADDITIVE_BLEND" = 1
		"F_BLEND_MODE" = 1
		"F_TRANSLUCENT" = 1
		"F_IGNOREZ" = 0
		"F_DISABLE_Z_WRITE" = 0
		"F_DISABLE_Z_BUFFERING" = 0
		"F_RENDER_BACKFACES" = 0
		"g_vColorTint" = [7.0, 7.0, 7.0, 0.37522]
	}
)";

static constexpr char szVMatBufferFatalInvisible[] =R"(
	<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
	{
		"shader" = "csgo_effects.vfx"

		"g_tColor" = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
		"g_tNormal" = resource:"materials/default/default_normal_tga_7652cb.vtex"
		"g_tMask1" = resource:"materials/default/default_mask_tga_344101f8.vtex"
		"g_tMask2" = resource:"materials/default/default_mask_tga_344101f8.vtex"
		"g_tMask3" = resource:"materials/default/default_mask_tga_344101f8.vtex"
		"g_tRoughness" = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
		"g_tMetalness" = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
		"g_tAmbientOcclusion" = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"

		"g_flColorBoost" = 30
		"g_flOpacityScale" = 245.55
		"g_flFresnelExponent" = 7.75
		"g_flFresnelFalloff" = 5
		"g_flFresnelMax" = 0.0
		"g_flFresnelMin" = 9

		"F_ADDITIVE_BLEND" = 1
		"F_BLEND_MODE" = 1
		"F_TRANSLUCENT" = 1
		"F_IGNOREZ" = 0
		"F_DISABLE_Z_WRITE" = 0
		"F_DISABLE_Z_BUFFERING" = 1
		"F_RENDER_BACKFACES" = 0
		"g_vColorTint" = [7.0, 7.0, 7.0, 0.37522]
	}
)";

constexpr const char* CHAM_CLASS_ARMS = "C_ViewmodelAttachmentModel";
constexpr const char* CHAM_CLASS_WEAPON = "C_CSGOViewModel";
constexpr const char* CHAM_CLASS_WEAPON_CS2 = "C_CS2HudModelWeapon";
constexpr const char* CHAM_CLASS_ARMS_CS2 = "C_CS2HudModelArms";
constexpr const char* CHAM_CLASS_PAWN = "C_CSPlayerPawn";

constexpr auto CHAM_HASH_ARMS = FNV1A::HashConst(CHAM_CLASS_ARMS);
constexpr auto CHAM_HASH_WEAPON = FNV1A::HashConst(CHAM_CLASS_WEAPON);
constexpr auto CHAM_HASH_WEAPON_CS2 = FNV1A::HashConst(CHAM_CLASS_WEAPON_CS2);
constexpr auto CHAM_HASH_ARMS_CS2 = FNV1A::HashConst(CHAM_CLASS_ARMS_CS2);
constexpr auto CHAM_HASH_PAWN = FNV1A::HashConst(CHAM_CLASS_PAWN);

static std::string random( size_t len )
{
	static constexpr char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	static std::mt19937 generator( std::random_device{}() );
	static std::uniform_int_distribution<> distr( 0, sizeof( alphanum ) - 2 );

	std::string result;
	result.reserve( len );

	for ( size_t i = 0; i < len; ++i )
		result += alphanum[distr( generator )];

	return result;
}

CStrongHandle<CMaterial2> CChams::CreateMaterial( const char* szMaterialName, const char szVmatBuffer[] )
{
	CKeyValues3* pKeyValues3 = CKeyValues3::CreateMaterialResource( );
	pKeyValues3->LoadFromBuffer( szVmatBuffer );

	CStrongHandle<CMaterial2> pCustomMaterial = {};
	auto asd = random( 12 ) + "_chlenixx.vmat";
	Functions::fnCreateMaterial( Interfaces::m_pMaterialSystem2, &pCustomMaterial, asd.c_str( ), pKeyValues3, 0, 1 );

	return pCustomMaterial;
}

bool CChams::Init( )
{
	if ( m_bInit )
		return m_bInit;

	m_Materials[VISUAL_MATERIAL_LATEX].m_Occluded = CreateMaterial( X( "generic_invis_latex" ), szInvisLatexVmatBuffer );
	m_Materials[VISUAL_MATERIAL_BLOOM].m_Occluded = CreateMaterial( X( "generic_invis_bloom" ), szInvisBloomVmatBuffer );
	m_Materials[VISUAL_MATERIAL_GLOW].m_Occluded = CreateMaterial( X( "generic_invis_glow" ), szInvisGlowVmatBuffer );
	m_Materials[VISUAL_MATERIAL_FLAT].m_Occluded = CreateMaterial( X( "generic_invis_flat" ), szInvisFlatVmatBuffer );
	m_Materials[4 ].m_Occluded = CreateMaterial( X( "xray_invis" ), szXrayInvisible );
	m_Materials[VISUAL_MATERIAL_FATAL].m_Occluded = CreateMaterial(X("fatal_invis"), szVMatBufferFatalInvisible);

	m_Materials[VISUAL_MATERIAL_LATEX].m_Visible = CreateMaterial( X( "generic_latex" ), szLatexVmatBuffer );
	m_Materials[VISUAL_MATERIAL_BLOOM].m_Visible = CreateMaterial( X( "generic_bloom" ), szBloomVmatBuffer );
	m_Materials[VISUAL_MATERIAL_GLOW].m_Visible = CreateMaterial( X( "generic_glow" ), szGlowVmatBuffer );
	m_Materials[VISUAL_MATERIAL_FLAT].m_Visible = CreateMaterial( X( "generic_flat" ), szFlatVmatBuffer );
	m_Materials[4 ].m_Visible = CreateMaterial( X( "xray" ), szXrayVisible );
	m_Materials[VISUAL_MATERIAL_FATAL].m_Visible = CreateMaterial(X("fatal_vis"), szVMatBufferFatalVisible);

	m_bInit = true;

	for ( auto& [pMaterial, pMaterialInvisible] : m_Materials )
	{
		if ( pMaterial == nullptr || pMaterialInvisible == nullptr )
			m_bInit = false;
	}

	return m_bInit;
}

auto fnSetMaterialAndColor = [&]( CSceneData* pSceneData, Color cColor, CMaterial2* pMaterial )
	{
		pSceneData->m_pMaterial = pMaterial;
		pSceneData->m_pMaterial2 = pMaterial;
		pSceneData->m_cColor = cColor;
	};

CSceneAnimatableObject* CreateSceneAnimatableObject( C_CSPlayerPawn* pPawn )
{
	static void* g_pRenderGameSystem = *(void**)(Memory::ResolveRelativeAddress( Memory::FindPattern( CLIENT_DLL, "48 8B 0D ? ? ? ? 8B 10 E8 ? ? ? ? 48 8B 94 24 ? ? ? ? 48 8D 8C 24" ), 3, 7 ));

	CSkeletonInstance* pSkeletonInstance = pPawn->m_pGameSceneNode( )->GetSkeletonInstance( );

	using fnGetWorldGroupID = int* (__fastcall*)(CSkeletonInstance*, int*);
	static fnGetWorldGroupID GetWorldGroupID = (fnGetWorldGroupID)(Memory::FindPattern( CLIENT_DLL, "48 8B 41 ? 48 85 C0 74 ? 48 8B 40 ? 8B 48" ));

	int nWorldGroupID_temp = 0;
	int* v57 = GetWorldGroupID( pSkeletonInstance, &nWorldGroupID_temp );

	using fn_sub_180221C00 = __int64( __fastcall* )(void*, unsigned int);
	static fn_sub_180221C00 sub_180221C00 = (fn_sub_180221C00)(Memory::GetAbsoluteAddress( Memory::FindPattern( CLIENT_DLL, "E8 ? ? ? ? 49 8B 8F ? ? ? ? 4C 8D 0D" ), 1 ));

	auto v58 = sub_180221C00( g_pRenderGameSystem, (unsigned int)*v57 );

	float* nig = (float*)((__int64)pPawn->m_pGameSceneNode( )->GetSkeletonInstance( ) + 16);

	return Memory::CallVFunc<CSceneAnimatableObject*, 20>(
		Interfaces::m_pMeshSystem,
		&pPawn->m_pGameSceneNode( )->GetSkeletonInstance( )->m_modelState( ).m_hModel( ),
		nig,
		"AnimatableSceneObjectDesc",
		8LL,
		0x220100000001LL,
		v58
	);
}

struct ModelsContainer_t;

static std::unordered_map<C_CSPlayerPawn*, ModelsContainer_t>      m_PawnToModelContainer;
static std::unordered_map<C_CSPlayerPawn*, CSceneAnimatableObject*> m_PawnToBTSceneObject;

static constexpr float MAX_RECORD_TIME = 0.2f;
struct ModelRenderState_t {
	std::array<Matrix3x4_t, 128> m_arrRenderBones;
	float m_flSimulationTime;
	bool m_bHasBeenRendered = false;
};

struct ModelsContainer_t {
	std::vector<ModelRenderState_t> m_ModelStates;
	std::mutex m_mutex;

	void AddState( C_CSPlayerPawn* pPawn, CSceneAnimatableObject* pActualSceneObj ) {

		std::lock_guard<std::mutex> lock( m_mutex );
		if ( !pPawn || !pActualSceneObj )
			return;

		float flSimTime = pPawn->m_flSimulationTime( );
		Matrix3x4_t* pBonesSrc = pActualSceneObj->GetRenderBones( );
		if ( !pBonesSrc )
			return;

		ModelRenderState_t newState;
		newState.m_flSimulationTime = flSimTime;
		for ( int i = 0; i < pActualSceneObj->GetBoneCount( ); i++ ) {
			newState.m_arrRenderBones[i] = pBonesSrc[i];
		}
		m_ModelStates.push_back( newState );

		float flCutoff = flSimTime - MAX_RECORD_TIME;
		while ( !m_ModelStates.empty( ) && m_ModelStates.front( ).m_flSimulationTime < flCutoff ) {
			m_ModelStates.erase( m_ModelStates.begin( ) );
		}

		if ( m_PawnToBTSceneObject.find( pPawn ) == m_PawnToBTSceneObject.end( ) || m_PawnToBTSceneObject[pPawn] == nullptr ) {
			m_PawnToBTSceneObject[pPawn] = CreateSceneAnimatableObject( pPawn );
		}

	}

	bool GetBestBTBones( float targetTime, Matrix3x4_t outBonesOut[ 128 ] ) {
		if ( m_ModelStates.empty( ) )
			return false;
		int closestIndex = 0;
		float closestDiff = abs( m_ModelStates[0].m_flSimulationTime - targetTime );
		for ( int i = 1; i < m_ModelStates.size( ); i++ ) {
			float diff = abs( m_ModelStates[i].m_flSimulationTime - targetTime );
			if ( diff < closestDiff ) {
				closestDiff = diff;
				closestIndex = i;
			}
		}

		for ( int i = 0; i < m_ModelStates[ closestIndex ].m_arrRenderBones.size( ); i++ ) {
			outBonesOut[i] = m_ModelStates[closestIndex].m_arrRenderBones[i];
		}

		return true;
	}
};

void DrawBacktrackChams(
	C_CSPlayerPawn* pPawn,
	CAnimatableSceneObjectDesc* pDesc,
	void* a3,
	CMeshPrimitiveOutputBuffer* pRenderBuffer,
	std::function<void( CAnimatableSceneObjectDesc*, CSceneAnimatableObject*, void*, CMeshPrimitiveOutputBuffer* )> oOriginal_GeneratePrimitives
) {
	if ( !pPawn || !pDesc || !pRenderBuffer )
		return;

	auto itContainer = m_PawnToModelContainer.find( pPawn );
	if ( itContainer == m_PawnToModelContainer.end( ) || itContainer->second.m_ModelStates.empty( ) )
		return;

	Matrix3x4_t closestBones[128];
	bool bHasSnapshot = itContainer->second.GetBestBTBones( g_LagCompensation->m_flOldestValidTime, closestBones );
	if ( !bHasSnapshot )
		return;

	CSceneAnimatableObject* pBTObj = m_PawnToBTSceneObject[pPawn];
	if ( !pBTObj )
			return;

	Matrix3x4_t* pBTObjBones = pBTObj->GetRenderBones( );
	for ( int i = 0; i < pBTObj->GetBoneCount( ); i++ ) {
		pBTObjBones[i] = closestBones[i];
	}

	if ( Config::b( g_Variables.m_Visuals.m_bBacktrackChamsIgnoreZ ) ) {

		Color clrBT = Config::c( g_Variables.m_Visuals.m_colBacktrackChamsIgnoreZ );
		int   nMatIdx = Config::i( g_Variables.m_Visuals.m_iBacktrackChamMaterialEnemy );

		CMaterial2* pMaterial = g_Chams->m_Materials[nMatIdx].m_Occluded;
		if ( !pMaterial )
			return;

		int nPrevCount = pRenderBuffer->m_nCount;
		oOriginal_GeneratePrimitives( pDesc, pBTObj, a3, pRenderBuffer );

		for ( int i = nPrevCount; i < pRenderBuffer->m_nCount; i++ ) {
			CSceneData* pSceneData = pRenderBuffer->GetPrimitive( i );
			if ( !pSceneData )
				continue;

			fnSetMaterialAndColor(
				pSceneData,
				clrBT,
				pMaterial
			);
		}

	}

	if ( Config::b( g_Variables.m_Visuals.m_bBacktrackChams ) ) {

		Color clrBT = Config::c( g_Variables.m_Visuals.m_colBacktrackChams );
		int   nMatIdx = Config::i( g_Variables.m_Visuals.m_iBacktrackChamMaterialEnemy );

		CMaterial2* pMaterial = g_Chams->m_Materials[nMatIdx].m_Visible;
		if ( !pMaterial )
			return;

		int nPrevCount = pRenderBuffer->m_nCount;
		oOriginal_GeneratePrimitives( pDesc, pBTObj, a3, pRenderBuffer );

		for ( int i = nPrevCount; i < pRenderBuffer->m_nCount; i++ ) {
			CSceneData* pSceneData = pRenderBuffer->GetPrimitive( i );
			if ( !pSceneData )
				continue;

			fnSetMaterialAndColor(
				pSceneData,
				clrBT,
				pMaterial
			);
		}

	}
}

bool CChams::OnGeneratePrimitives( CAnimatableSceneObjectDesc* pDesc, CSceneAnimatableObject* pObject, void* a3, CMeshPrimitiveOutputBuffer* pRenderBuffer ) const
{
	const auto oOriginal = Detours::GeneratePrimitives.GetOriginal<decltype(&Hooks::hkGeneratePrimitives)>( );

	if ( !pObject )
		return false;

	CBaseHandle hOwner = pObject->m_hOwnerHandle;
	if ( !hOwner.IsValid( ) )
		return false;

	auto* pEnt = Interfaces::m_pGameResourceService->pGameEntitySystem->Get<C_BaseEntity>( hOwner );
	if ( !pEnt )
		return false;

	auto fnSetChams = [&]( Color cColor, int nChamMaterialIndex, bool bInvisible )
		{
			CMaterial2* pMaterial = bInvisible ? static_cast<CMaterial2*>(g_Chams->m_Materials[nChamMaterialIndex].m_Occluded) : static_cast<CMaterial2*>(g_Chams->m_Materials[nChamMaterialIndex].m_Visible);

			if ( !pMaterial )
				return;

			int nPrevCount = pRenderBuffer->m_nCount;
			oOriginal( pDesc, pObject, a3, pRenderBuffer );

			for ( auto i = nPrevCount; i < pRenderBuffer->m_nCount; i++ )
			{
				CSceneData* pSceneData = pRenderBuffer->GetPrimitive( i );

				if ( !pSceneData )
					continue;

				fnSetMaterialAndColor( pSceneData, cColor, pMaterial );
			}
		};

	C_BaseEntity* pEntity = Interfaces::m_pGameResourceService->pGameEntitySystem->Get<C_BaseEntity>( hOwner );
	if ( !pEntity )
		return false;

	SchemaClassInfoData_t* pClassInfo;
	pEnt->GetSchemaClassInfo( &pClassInfo );

	FNV1A_t uHashedName = FNV1A::Hash( pClassInfo->szName );

	if ((uHashedName == CHAM_HASH_ARMS || uHashedName == CHAM_HASH_ARMS_CS2)
		&& Config::b( g_Variables.m_Visuals.m_bEnableSelfArmsChams ) )
	{
		fnSetChams(
			Config::c( g_Variables.m_Visuals.m_colSelfArmsChams ),
			Config::i( g_Variables.m_Visuals.m_iChamMaterialLocalArms ),
			false
		);

		return true;
	}

	if ((uHashedName == CHAM_HASH_WEAPON || uHashedName == CHAM_HASH_WEAPON_CS2)
		&& Config::b( g_Variables.m_Visuals.m_bEnableSelfWeaponChams ) )
	{
		fnSetChams(
			Config::c( g_Variables.m_Visuals.m_colSelfWeaponChams ),
			Config::i( g_Variables.m_Visuals.m_iChamMaterialLocalWeapon ),
			false
		);

		return true;
	}

	if ( Config::b( g_Variables.m_Visuals.m_bEnableSelfAttachmentChams )
		&& pEntity->IsWeapon( )
		&& Config::b( g_Variables.m_Visuals.m_bEnableThirdPerson )
		&& Input::HandleInput( Config::kb( g_Variables.m_Visuals.m_iThirdPersonKeybind ) )
		)
	{
		if ( pEntity->m_hOwnerEntity( ).Get( )
			&& pEntity->m_hOwnerEntity( ).Get( ) == Globals::m_pLocalPlayerPawn
			)
		{
			fnSetChams(
				Config::c( g_Variables.m_Visuals.m_colAttachmentChams ),
				Config::i( g_Variables.m_Visuals.m_iChamMaterialLocalAttachments ),
				false
			);

			return true;
		}
	}

	if ( Config::b( g_Variables.m_Visuals.m_bEnableDroppedWeaponChams ) )
	{
		if ( !pEntity->m_hOwnerEntity( ).Get( ) && pEntity->IsWeapon( ) )
		{
			fnSetChams(
				Config::c( g_Variables.m_Visuals.m_colDroppedWeaponChams ),
				Config::i( g_Variables.m_Visuals.m_iChamMaterialDroppedWeapons ),
				false
			);

			return true;
		}
	}

	C_CSPlayerPawn* pPawn = Interfaces::m_pGameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>( hOwner );
	if ( !pPawn )
		return false;

	if ( uHashedName != CHAM_HASH_PAWN )
		return false;

	if ( Config::b( g_Variables.m_Visuals.m_bEnableSelfChams ) && pPawn == Globals::m_pLocalPlayerPawn )
	{
		fnSetChams(
			Config::c( g_Variables.m_Visuals.m_colSelfChams ),
			Config::i( g_Variables.m_Visuals.m_iChamMaterialLocal ),
			false
		);

		return true;
	}
	if (!Globals::m_pLocalPlayerPawn)
		return false;

	if ( !Globals::m_pLocalPlayerPawn->IsEnemy( pPawn ) )
		return false;

	if ( pPawn->m_iHealth( ) <= 0 && !Config::b( g_Variables.m_Visuals.m_bRagdollChamsEnemy ) )
		return false;

	if ( Config::b( g_Variables.m_Visuals.m_bEnableEnemyChamsIgnoreZ ) )
	{
		fnSetChams(
			Config::c( g_Variables.m_Visuals.m_colEnemyChamsIgnoreZ ),
			Config::i( g_Variables.m_Visuals.m_iChamMaterialEnemyIgnoreZ ),
			true
		);
	}

	if ( Config::b( g_Variables.m_Visuals.m_bBacktrackChamsIgnoreZ ) || Config::b( g_Variables.m_Visuals.m_bBacktrackChams ) ) {

		m_PawnToModelContainer[pPawn].AddState( pPawn, pObject );
		DrawBacktrackChams(
			pPawn,
			pDesc,
			a3,
			pRenderBuffer,
			oOriginal
		);

	}

	if ( Config::b( g_Variables.m_Visuals.m_bEnableEnemyChams ) )
	{
		fnSetChams(
			Config::c( g_Variables.m_Visuals.m_colEnemyChams ),
			Config::i( g_Variables.m_Visuals.m_iChamMaterialEnemy ),
			false
		);
	}

	return Config::b( g_Variables.m_Visuals.m_bEnableEnemyChams );
}
