#pragma once

class IGlobalVariabless
{
public:
	float m_flRealTime;
	int32_t m_nFrameCount;
	float m_flFrameTime;
	float m_flFrameTime2;
	int32_t m_nMaxClients;
	int16_t m_nTickBase;
	char pad_1a[26];
	float m_flIntervalPerSubTick;
	float m_flCurrentTime;
	float m_flCurrentTime2;
	char pad_c[4];
	float m_flSomeTickFrac;
	char pad_d[4];
	int32_t m_nTickCount;
	float m_flTickFraction;
	char pad[4];
	_DWORD m_nUnkFlag;
	char pad_134[300];
	char* m_szCurrentMap;
	char* m_szCurrentMapName;
};
class IGlobalVariables
{
public:
    float m_flRealTime;
    int32_t m_nFrameCount;
    float m_flFrameTime;
    float m_flFrameTime2;
    int32_t m_nMaxClients;
    bool N000000DE;
    char pad_0015[23];
    float m_flIntervalPerSubTick;
    float m_flCurrentTime;
    float m_flCurrentTime2;
    float client_tick_fraction;
    float next_tick_fraction;
    float client_fraction;
    int32_t m_nTickCount;
    int32_t client_tick;
    float m_flTickFraction;
    char pad_0050[8];
    void* net_channel;
    char pad_0060[288];
    char* VpkMapPath;
    char* MapName;
};