#include "LightShader.h"

LightShader::LightShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_sampleState = nullptr;

	m_cameraBuffer = nullptr;
	m_matrixBuffer = nullptr;
}

LightShader::LightShader(const LightShader&)
{
}

LightShader::~LightShader()
{
}

bool LightShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	WCHAR vs[] = L"Shaders/light.vs";
	WCHAR ps[] = L"Shaders/light.ps";
	
	result = InitializeShader(device, hwnd, vs, ps);
	if (!result)
	{
		return false;
	}

	return true;
}

void LightShader::Shutdown()
{
	ShutdownShader();
}

bool LightShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX world, D3DXMATRIX view,
	D3DXMATRIX projection, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDir, 
	D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor,
	D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower)
{
	bool result;

	result = SetShaderParameters(deviceContext, world, view, projection, texture, lightDir,
		ambientColor, diffuseColor, cameraPosition, specularColor, specularPower);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);
	return true;
}

bool LightShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;

	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];

	/*
	 * 쉐이더 컴파일
	 */
	
	result = D3DX11CompileFromFile(vsFilename, nullptr, nullptr, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr,
		&vertexShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"쉐이더 파일을 찾을 수 없음", MB_OK);
		}

		return false;
	}

	result = D3DX11CompileFromFile(psFilename, nullptr, nullptr, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr,
		&pixelShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"쉐이더 파일을 찾을 수 없음", MB_OK);
		}

		return false;
	}

	/*
	 * 쉐이더 생성
	 */

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	/*
	 * 레이아웃
	 */

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// 배열 길이
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 레이아웃 생성
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	if (FAILED(result))
	{
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC cameraBufferDesc;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&cameraBufferDesc, nullptr, &m_cameraBuffer);
	if (FAILED(result))
	{
		return false;
	}
	
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDesc, nullptr, &m_lightBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void LightShader::ShutdownShader()
{
	if (m_cameraBuffer)
	{
		m_cameraBuffer->Release();
		m_cameraBuffer = nullptr;
	}
	
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = nullptr;
	}

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = nullptr;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = nullptr;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}
}

void LightShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* filename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	compileErrors = (char*)(errorMessage->GetBufferPointer());

	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = nullptr;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"쉐이더 컴파일 에러. shader-error.txt를 확인하세요.", filename, MB_OK);
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX world, D3DXMATRIX view,
	D3DXMATRIX projection, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDir,
	D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, 
	D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower)
{
	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	unsigned int bufferNumber;

	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;
	CameraBufferType* dataPtr3;

	D3DXMatrixTranspose(&world, &world);
	D3DXMatrixTranspose(&view, &view);
	D3DXMatrixTranspose(&projection, &projection);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = projection;

	deviceContext->Unmap(m_matrixBuffer, 0);
	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr3 = (CameraBufferType*)mappedResource.pData;
	dataPtr3->cameraPosition = cameraPosition;
	dataPtr3->padding = 0.0f;

	deviceContext->Unmap(m_cameraBuffer, 0);

	bufferNumber = 1;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);
	
	deviceContext->PSSetShaderResources(0, 1, &texture);

	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr2 = (LightBufferType*)mappedResource.pData;

	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDir;
	dataPtr2->specularColor = specularColor;
	dataPtr2->specularPower = specularPower;

	deviceContext->Unmap(m_lightBuffer, 0);
	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);
	return true;
}

void LightShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);
	
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
	
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
