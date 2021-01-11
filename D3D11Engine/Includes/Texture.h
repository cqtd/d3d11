#pragma once

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Utils/Macro.h"

#include <d3d11.h>
#include <D3DX11tex.h>

class Texture
{
public:
	GEN_CLASS_DEFAULT(Texture)

public:
	bool Initialize(ID3D11Device* device, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* m_texture;
};

#endif