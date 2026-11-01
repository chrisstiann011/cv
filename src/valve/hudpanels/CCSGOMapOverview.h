#pragma once

class CTextureDx11
{
public:
    MEM_PAD(0x10);
    ID3D11ShaderResourceView* m_pTextureSRV0;
    ID3D11ShaderResourceView* m_pTextureSRV1;
};

class CSource2UITexture
{
public:
    class Data
    {
    public:
        CTextureDx11* m_pDx11Texture;
    };

    MEM_PAD(0x28);
    Data* m_pData;
};

class CImageData
{
public:
    MEM_PAD(0x30);
    CSource2UITexture* m_pUITexture;
};

class CImageProxySource
{
public:
    MEM_PAD(0x10);
    CImageData* m_pImageData;
};

class CImagePanel
{
public:
    MEM_PAD(0xD0);
    CImageProxySource* m_pImageProxy;

    inline ID3D11ShaderResourceView* GetNativeTexture()
    {
        CImageProxySource* pImageProxy = m_pImageProxy;
        if (!pImageProxy)
            return nullptr;

        CImageData* pImageData = pImageProxy->m_pImageData;
        if (!pImageData)
            return nullptr;

        CSource2UITexture* pUITexture = pImageData->m_pUITexture;
        if (!pUITexture)
            return nullptr;

        CSource2UITexture::Data* pUITextureData = pUITexture->m_pData;
        if (!pUITextureData)
            return nullptr;

        CTextureDx11* pDX11Texture = pUITextureData->m_pDx11Texture;
        if (!pDX11Texture)
            return nullptr;

        return pDX11Texture->m_pTextureSRV0;
    }
};

class CCSGO_MapOverview
{
public:

    CImagePanel* m_pMap2DPanel() {
        return *(CImagePanel**)(std::uintptr_t(this) + 0x2b0);
    }

    Vector2D m_pMapPos()
    {
        return *(Vector2D*)(std::uintptr_t(this) + 0x190);
    }

    float m_flMapScale() {
        return *(float*)(std::uintptr_t(this) + 0x1a4);
    }

    float m_flMapHeight() {
        return *(float*)(std::uintptr_t(this) + 0x1a8);
    }

    float m_flMapWidth() {
        return *(float*)(std::uintptr_t(this) + 0x1a8 + sizeof(float));
    }
};