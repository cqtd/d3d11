#include "Graphics.h"

Graphics::Graphics()
{
	m_d3d = nullptr;
	m_camera = nullptr;
	m_model = nullptr;
	
	//m_colorShader = nullptr;
	//m_textureShader = nullptr;
	m_lightShader = nullptr;
	m_light = nullptr;
}

Graphics::Graphics(const Graphics&)
{
}

Graphics::~Graphics()
{
}

bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	m_d3d = new D3D;
	if (!m_d3d)
	{
		return false;
	}

	result = m_d3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED,
		hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Direct3D 초기화에 실패했습니다.", L"오류", MB_OK);
		return false;
	}

	m_camera = new Camera;
	if (!m_camera)
	{
		return false;
	}

	m_camera->SetPosition(0.0f, 0.0f, -10.0f);

	m_model = new Model;
	if (!m_model)
	{
		return false;
	}

	char* mesh = (char*)"Meshes/cube.txt";
	WCHAR tex[] = L"Textures/seafloor.dds";
	
	result = m_model->Initialize(m_d3d->GetDevice(), mesh, tex);
	if (!result)
	{
		MessageBox(hwnd, L"Model 초기화에 실패했습니다.", L"오류", MB_OK);
		return false;
	}

	//m_colorShader = new ColorShader;
	//if (!m_colorShader)
	//{
	//	return false;
	//}

	//result = m_colorShader->Initialize(m_d3d->GetDevice(), hwnd);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"ColorShader 초기화에 실패했습니다.", L"오류", MB_OK);
	//	return false;
	//}

	//m_textureShader = new TextureShader;
	//if (!m_textureShader)
	//{
	//	return false;
	//}

	//result = m_textureShader->Initialize(m_d3d->GetDevice(), hwnd);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"TextureShader 초기화에 실패했습니다.", L"오류", MB_OK);
	//	return false;
	//}

	m_lightShader = new LightShader;
	if (!m_lightShader)
	{
		return false;
	}

	result = m_lightShader->Initialize(m_d3d->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Light Shader 초기화에 실패했습니다.", L"Error", MB_OK);
		return false;
	}

	m_light = new Light;
	if (!m_light)
	{
		return false;
	}

	m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}

void Graphics::Shutdown()
{
	//if (m_colorShader)
	//{
	//	m_colorShader->Shutdown();
	//	delete m_colorShader;
	//	m_colorShader = nullptr;
	//}

	//if (m_textureShader)
	//{
	//	m_textureShader->Shutdown();
	//	delete m_textureShader;
	//	m_textureShader = nullptr;
	//}

	if (m_light)
	{
		delete m_light;
		m_light = nullptr;
	}

	// Release the light shader object.
	if (m_lightShader)
	{
		m_lightShader->Shutdown();
		delete m_lightShader;
		m_lightShader = nullptr;
	}
	
	if (m_model)
	{
		m_model->Shutdown();
		delete m_model;
		m_model = nullptr;
	}

	if (m_camera)
	{
		delete m_camera;
		m_camera = nullptr;
	}
	
	if (m_d3d)
	{
		m_d3d->Shutdown();
		delete m_d3d;
		m_d3d = nullptr;
	}
}

bool Graphics::Frame()
{
	bool result;

	static float rotation = 0.0f;

	// 매 프레임 로테이션 업데이트
	rotation += (float)D3DX_PI * 0.01f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	result = Render(rotation);
	if (!result)
	{
		return false;
	}
	
	return true;
}

bool Graphics::Render(float rotation)
{
	D3DXMATRIX view, world, projection;
	bool result;

	m_d3d->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_camera->Render();

	m_camera->GetViewMatrix(view);
	m_d3d->GetWorldMatrix(world);
	m_d3d->GetProjectionMatrix(projection);

	D3DXMatrixRotationY(&world, rotation);

	m_model->Render(m_d3d->GetDeviceContext());

	//result = m_colorShader->Render(m_d3d->GetDeviceContext(), m_model->GetIndexCount(),
	//	world, view, projection);

	//result = m_textureShader->Render(m_d3d->GetDeviceContext(), m_model->GetIndexCount(),
	//	world, view, projection, m_model->GetTexture());

	result = m_lightShader->Render(m_d3d->GetDeviceContext(), m_model->GetIndexCount(),
		world, view, projection, m_model->GetTexture(), m_light->GetDirection(), m_light->GetDiffuseColor());

	if (!result)
	{
		return false;
	}
	
	m_d3d->EndScene();
	
	return true;
}

