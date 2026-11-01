#pragma once

struct SceneLightObjectData_t
{
	MEM_PAD(164);
	Vector m_vDirectionNormalized;
};

class CSceneLightObject
{
public:
	MEM_PAD(224);
	SceneLightObjectData_t m_Data;
};

class CLightBinnerGpu
{
public:
	MEM_PAD(57968);
	CSceneLightObject** m_ppSceneLightObject;
};

class c_light_data_queue
{
public:
    char pad_0000[24];
    void* light_data;
    char pad_0020[4];
};

class ISceneSystem
{
public:
    char pad_0000[10760];
    c_light_data_queue* light_data_queue;

    void DeleteSceneObject(void* object) {
        if (!object)
            return;

        Memory::CallVFunc<void, 92>(this, object);
    }
};