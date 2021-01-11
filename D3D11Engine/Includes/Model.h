#pragma once

#ifndef _MODEL_H_
#define _MODEL_H_

#include "Utils/Macro.h"

#include <d3d11.h>
#include <D3DX10math.h>

class Model
{
private:

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};
	
public:
	GEN_CLASS_DEFAULT(Model)

public:

	bool Initialize(ID3D11Device* device);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

#endif