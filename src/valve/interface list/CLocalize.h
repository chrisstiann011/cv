#pragma once

class CLocalize
{
public:
    const char* FindSafe(const char* szTokenName)
    {
        return Memory::CallVFunc<const char*, 17U>( this, szTokenName);
    }
};

using FileHandle = void;
class CBaseFileSystem
{
public:
    void Close(FileHandle* pHandle)
    {
        return Memory::CallVFunc<void, 14U>(this, pHandle);
    }

    std::uint32_t Size(FileHandle* pHandle)
    {
        return Memory::CallVFunc<std::uint32_t, 18U>(this, pHandle);
    }

    FileHandle* Open(const char* szFileName, const char* szOptions, int iUnknown, const char* szPathID = nullptr)
    {
        return Memory::CallVFunc<FileHandle*, 78U>(this, szFileName, szOptions, iUnknown, szPathID);
    }

    int Read(void* pOutput, std::uint32_t uSize, int iCount, FileHandle* pHandle)
    {
		return Memory::CallVFunc<int, 79U>(this, pOutput, uSize, iCount, pHandle);
	}

    bool Exists(const char* pFileName, const char* a3)
    {
        return Memory::CallVFunc<bool, 21>(this, pFileName, a3);
    }
};
