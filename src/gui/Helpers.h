#pragma once
namespace Helpers
{

    bool LoadTextureFromFile( const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height );

    bool LoadTextureFromMemory( void* pData, unsigned int uSize, ID3D11ShaderResourceView** pOutTexture, int* iOutWidth, int* iOutHeight );

    ID3D11ShaderResourceView * CreateShaderResourceView(ID3D11Texture2D * pTexture);
}