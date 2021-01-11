#include "Graphics.h"

Graphics::Graphics()
{
	m_d3d = nullptr;
	m_camera = nullptr;
	m_model = nullptr;
	m_colorShader = nullptr;
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

	result = m_model->Initialize(m_d3d->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Model 초기화에 실패했습니다.", L"오류", MB_OK);
		return false;
	}

	m_colorShader = new ColorShader;
	if (!m_colorShader)
	{
		return false;
	}

	result = m_colorShader->Initialize(m_d3d->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"ColorShader 초기화에 실패했습니다.", L"오류", MB_OK);
		return false;
	}
	
	return true;
}

void Graphics::Shutdown()
{
	if (m_colorShader)
	{
		m_colorShader->Shutdown();
		delete m_colorShader;
		m_colorShader = nullptr;
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

	result = Render();
	if (!result)
	{
		return false;
	}
	
	return true;
}

bool Graphics::Render()
{
	D3DXMATRIX view, world, projection;
	bool result;

	m_d3d->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_camera->Render();

	m_camera->GetViewMatrix(view);
	m_d3d->GetWorldMatrix(world);
	m_d3d->GetProjectionMatrix(projection);

	m_model->Render(m_d3d->GetDeviceContext());

	result = m_colorShader->Render(m_d3d->GetDeviceContext(), m_model->GetIndexCount(),
		world, view, projection);

	if (!result)
	{
		return false;
	}
	
	m_d3d->EndScene();
	
	return true;
}

