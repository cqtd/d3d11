#include "Graphics.h"

Graphics::Graphics()
{
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

	
	return true;
}

void Graphics::Shutdown()
{
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
	m_d3d->BeginScene(0.72f, 0.4f, 0.39f, 1.0f);
	m_d3d->EndScene();
	
	return true;
}

