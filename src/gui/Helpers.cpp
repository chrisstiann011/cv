#include "../Precompiled.h"
#define STB_IMAGE_IMPLEMENTATION
#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>
#include <stb_image.h>

bool Helpers::LoadTextureFromFile( const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height )
{

    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load( filename, &image_width, &image_height, NULL, 4 );
    if (image_data == NULL)
        return false;

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory( &desc, sizeof( desc ) );
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    Interfaces::m_pDevice->CreateTexture2D( &desc, &subResource, &pTexture );

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory( &srvDesc, sizeof( srvDesc ) );
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    Interfaces::m_pDevice->CreateShaderResourceView( pTexture, &srvDesc, out_srv );
    pTexture->Release( );

    *out_width = image_width;
    *out_height = image_height;
    stbi_image_free( image_data );

    return true;
}

bool Helpers::LoadTextureFromMemory( void* pData, unsigned int uSize, ID3D11ShaderResourceView** pOutTexture, int* iOutWidth, int* iOutHeight )
{

    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load_from_memory( reinterpret_cast<stbi_uc*>( pData ), uSize, &image_width, &image_height, NULL, 4 );
    if (image_data == NULL)
        return false;

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory( &desc, sizeof( desc ) );
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    Interfaces::m_pDevice->CreateTexture2D( &desc, &subResource, &pTexture );

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory( &srvDesc, sizeof( srvDesc ) );
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    Interfaces::m_pDevice->CreateShaderResourceView( pTexture, &srvDesc, pOutTexture );
    pTexture->Release( );

    *iOutWidth = image_width;
    *iOutHeight = image_height;
    stbi_image_free( image_data );

    return true;
}

ID3D11ShaderResourceView* Helpers::CreateShaderResourceView(ID3D11Texture2D* pTexture)
{
    if (pTexture == nullptr)
		return nullptr;

    ID3D11ShaderResourceView* shaderResourceView = nullptr;

    D3D11_TEXTURE2D_DESC textureDesc;
    pTexture->GetDesc(&textureDesc);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    HRESULT hr = Interfaces::m_pDevice->CreateShaderResourceView(pTexture, &srvDesc, &shaderResourceView);

    if (SUCCEEDED(hr))
        return shaderResourceView;

    return nullptr;
}