#include "Texture.h"

Texture::Texture()
{
	m_texture = nullptr;
}

Texture::Texture(const Texture&)
{
}

Texture::~Texture()
{
}

bool Texture::Initialize(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result;

	result = D3DX11CreateShaderResourceViewFromFile(device, filename, nullptr, nullptr,
		&m_texture, nullptr);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Texture::Shutdown()
{
	if (m_texture)
	{
		m_texture->Release();
		m_texture = nullptr;
	}
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return this->m_texture;
}
