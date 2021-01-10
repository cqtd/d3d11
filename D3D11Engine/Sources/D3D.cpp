#include "../Includes/D3D.h"

D3D::D3D()
{
	m_swapChain = nullptr;

	m_device = nullptr;
	m_deviceContext = nullptr;

	m_renderTargetView = nullptr;

	m_depthStencilBuffer = nullptr;
	m_depthStencilState = nullptr;
	m_depthStencilView = nullptr;

	m_rasterState = nullptr;
}

D3D::D3D(const D3D&)
{
}

D3D::~D3D()
{
}

bool D3D::Initialize(int screenWidth, int screenHeight,
	bool vsync, HWND hwnd,
	bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;

	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;

	unsigned int numModes, i, numerator, denominator;
	unsigned long long stringLength;

	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;

	int error;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;

	float fieldOfView, screenAspect;


	m_vsync_enabled = vsync;

	// DX factory 생성
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// 아답터 생성을 위해 팩토리 사용
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// 첫번째 아답터 아웃풋
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
		&numModes, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	// 가능한 디스플레이 모드를 모두 저장해두기 위한 리스트
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// 위 리스트 채우기
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
		&numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// 전체 컴비네이션을 검사
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// 비디오카드 아답터 디스크립션 가져오기
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// megabyte 단위로 비디오 메모리 저장
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 비디오 카드 이름 저장
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	/*
	 * 사용한 그래픽 카드 관련 데이터 해제
	 */

	 // 디스플레이 모드 메모리 해제
	delete[] displayModeList;
	displayModeList = nullptr;

	// 아답터 아웃풋 메모리 해제
	adapterOutput->Release();
	adapterOutput = nullptr;

	// 아답터 메모리 해제
	adapter->Release();
	adapter = nullptr;

	// 팩토리 메모리 해제
	factory->Release();
	factory = nullptr;


	/*
	 * 스왑 체인 파트
	 */

	 // 스왑 체인 초기화
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// 싱글 백 버퍼
	swapChainDesc.BufferCount = 1;

	// 백 버퍼 크기
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// 백버퍼는 레귤러 32비트 서피스로
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// 백버퍼 용도 설정
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 렌더링 아웃풋 핸들
	swapChainDesc.OutputWindow = hwnd;

	// 멀티샘플링 끄기
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// 스캔 라인 순서와 스케일링 지정안함
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// present 후에 백버퍼 날림
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 고급 플래그 사용 안함
	swapChainDesc.Flags = 0;


	/*
	 * 피쳐 레벨 설정 해줌
	 */

	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// 스왑 체인과 dx11 디바이스 생성
	result = D3D11CreateDeviceAndSwapChain(nullptr,
		D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, nullptr, &m_deviceContext);

	if (FAILED(result))
	{
		return false;
	}

	// 백버퍼 포인터 가져오기
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// 백 버퍼 포인터와 렌더 타겟뷰를 생성
	result = m_device->CreateRenderTargetView(backBufferPtr, nullptr, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// 백 버퍼 메모리 해제
	backBufferPtr->Release();
	backBufferPtr = nullptr;


	/*
	 * 뎁스 버퍼 파트
	 */

	 // 뎁스 버퍼 디스크립션 초기화
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 뎁스 버퍼에 사용할 텍스쳐 2d 만듦
	result = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 뎁스 스텐실 초기화
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// 프론트 페이스인 경우
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 백 페이스
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 뎁스 스텐실 스테이스 생성
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// 뎁스 스텐실 스테이트 설정
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);


	// 뎁스 스텐실 뷰 초기화
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 뎁스 스텐실 뷰 생성
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// 렌더타겟뷰와 스텐실 버퍼를 아웃풋 렌더 파이프라인에 바인딩
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// 래스터 라이즈 디스크립션 세팅
	// 어떻게 폴리곤들이 그려질지 결정함
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// 디바이스에서 생성
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// 래스터라이저 스테이트 세팅
	m_deviceContext->RSSetState(m_rasterState);

	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;

	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	m_deviceContext->RSSetViewports(1, &viewport);

	/*
	 * 매트릭스 생성 파트
	 */
	
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// 3d 렌더링을 위한 프로젝션 매트릭스 생성
	D3DXMatrixPerspectiveLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	// 월드 매트릭스를 아이덴티티로
	D3DXMatrixIdentity(&m_worldMatrix);

	// 2d 렌더링을 위한 Ortho 매트릭스
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenHeight);

	return true;
}

void D3D::Shutdown()
{
	// 끄기전에 윈도우 모드로 바꾼 다음 꺼야 스왑체인 예외 안 생김
	// 등록 역순으로 해제
	
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, nullptr);
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = nullptr;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = nullptr;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = nullptr;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = nullptr;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = nullptr;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = nullptr;
	}

	return;
}

void D3D::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// 백 버퍼 클리어
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// 뎁스 버퍼 클리어
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3D::EndScene()
{
	if (m_vsync_enabled)
	{
		m_swapChain->Present(1, 0);
	}
	else
	{
		m_swapChain->Present(0, 0);
	}
}

ID3D11Device* D3D::GetDevice()
{
	return this->m_device;
}

ID3D11DeviceContext* D3D::GetDeviceContext()
{
	return this->m_deviceContext;
}

void D3D::GetProjectionMatrix(D3DXMATRIX& projection)
{
	projection = this->m_projectionMatrix;
}

void D3D::GetWorldMatrix(D3DXMATRIX& world)
{
	world = this->m_worldMatrix;
}

void D3D::GetOrthoMatrix(D3DXMATRIX& ortho)
{
	ortho = this->m_orthoMatrix;
}

void D3D::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
}