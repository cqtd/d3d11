#pragma once

#ifndef _LIGHTSHADER_H_
#define _LIGHTSHADER_H_

#include "Utils/Macro.h"

#include <d3d11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include <fstream>

using namespace std;

class LightShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct CameraBufferType
	{
		D3DXVECTOR3 cameraPosition;
		float padding;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float specularPower;
		D3DXVECTOR4 specularColor;
	};
	
public:
	GEN_CLASS_DEFAULT(LightShader)

public:
	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount,
		D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection,
		ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDir, 
		D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, 
		D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* filename);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection,
		ID3D11ShaderResourceView* texture,
		D3DXVECTOR3 lightDir, 
		D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, 
		D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;

	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_lightBuffer;
};

#endif