#pragma once

#ifndef _MODEL_H_
#define _MODEL_H_

#include "Utils/Macro.h"

#include <d3d11.h>
#include <D3DX10math.h>

#include "Texture.h"

#include <fstream>
using namespace std;

class Model
{
private:

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
	
public:
	GEN_CLASS_DEFAULT(Model)

public:

	bool Initialize(ID3D11Device* device, char* , WCHAR* filename);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	bool LoadTexture(ID3D11Device* device, WCHAR* filename);
	void ReleaseTexture();

	bool LoadModel(char*);
	void ReleaseModel();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	Texture* m_texture;
	ModelType* m_model;
};

#endif