#pragma once

class CEconItemSystem
{
public:
    CEconItemSchema* GetEconItemSchema()
    {
        return *reinterpret_cast<CEconItemSchema**>(reinterpret_cast<std::uintptr_t>(this) + 0x8);
    }
};