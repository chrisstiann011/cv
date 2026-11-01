#pragma once

class CGameEntitySystem;
class IGameResourceService
{
public:
	MEM_PAD( 0x58 );
	CGameEntitySystem* pGameEntitySystem;
};