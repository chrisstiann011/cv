#pragma once

class CNetworkedQuantizedFloat
{
public:
	std::float_t m_Value;
	uint16 m_nEncoder;
	bool m_bUnflattened;
	MEM_PAD(0x1);
};

class CNetworkVelocityVector
{
public:
	Vector m_vecValue;
	CNetworkedQuantizedFloat m_vecX;
	CNetworkedQuantizedFloat m_vecY;
	CNetworkedQuantizedFloat m_vecZ;
};

class CNetworkViewOffsetVector
{
public:
	Vector m_vecValue;
	CNetworkedQuantizedFloat m_vecX;
	CNetworkedQuantizedFloat m_vecY;
	CNetworkedQuantizedFloat m_vecZ;
};

class CNetworkOriginQuantizedVector
{
public:
	Vector m_vecValue;
	CNetworkedQuantizedFloat m_vecX;
	CNetworkedQuantizedFloat m_vecY;
	CNetworkedQuantizedFloat m_vecZ;
};

class CNetworkOriginCellCoordQuantizedVector
{
public:
	Vector m_vecValue;
	uint16 m_cellX;
	uint16 m_cellY;
	uint16 m_cellZ;
	uint16 m_nOutsideWorld;
	CNetworkedQuantizedFloat m_vecX;
	CNetworkedQuantizedFloat m_vecY;
	CNetworkedQuantizedFloat m_vecZ;
};