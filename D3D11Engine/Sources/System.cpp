#include "System.h"

System::System()
{
	m_input = nullptr;
	m_graphics = nullptr;
}

System::System(const System&)
{
}

System::~System()
{
}

bool System::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	screenWidth = 0;
	screenHeight = 0;

	if (ALLOW_CONSOLE)
	{
		result = InitializeConsole();
		if (!result)
		{
			return false;
		}
	}

	InitializeWindows(screenWidth, screenHeight);

	m_input = new Input;
	if (!m_input)
	{
		return false;
	}

	m_input->Initialize();

	m_graphics = new Graphics;
	if (!m_graphics)
	{
		return false;
	}

	result = m_graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void System::Shutdown()
{
	if (m_graphics)
	{
		m_graphics->Shutdown();
		delete m_graphics;
		m_graphics = nullptr;
	}

	if (m_input)
	{
		delete m_input;
		m_input = nullptr;
	}

	ShutdownWindows();
}

void System::Run()
{
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	while (!done)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}
}

LRESULT System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch(umsg)
	{
	case WM_KEYDOWN:
		{
		m_input->KeyDown((unsigned int)wparam);
		return 0;
		}
	case WM_KEYUP:
		{
		m_input->KeyUp((unsigned int)wparam);
		return 0;
		}

	default:
		{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

bool System::Frame()
{
	bool result;

	if (m_input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	result = m_graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

void System::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;

	int posX, posY;

	// �� ������Ʈ�� External ������ ��������
	ApplicationHandle = this;

	// �� ���ø����̼��� �ν��Ͻ� ��������
	m_hInstance = GetModuleHandle(nullptr);

	// ���ø����̼� �̸� �ο�
	m_applicationName = L"DX11";

	// ������ Ŭ���� �⺻ ����
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// ������ ���
	RegisterClassEx(&wc);

	// Ǯ��ũ�� ���ο� ���� ��ũ�� ����
	if (FULL_SCREEN)
	{
		// Ǯ��ũ���̸� �ִ� ũ��� 32��Ʈ ����ũ��
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));

		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// ���÷��� ������ Ǯ��ũ��Ʈ��
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// ��ġ ����
		posX = posY = 0;
	}
	else
	{
		// 1280 720 ���� ����
		screenWidth = 1280;
		screenHeight = 720;

		// ȭ�� �߾ӿ� ����
		// ����Ͱ� �������� ��������

		int sizeX, sizeY;
		if (FORCE_RESOLUTION)
		{
			sizeX = 3840;
			sizeY = 2160;
		}
		else
		{
			sizeX = GetSystemMetrics(SM_CXSCREEN);
			sizeY = GetSystemMetrics(SM_CYSCREEN);
			
		}

		posX = (sizeX - screenWidth) / 2;
		posY = (sizeY - screenHeight) / 2;
	}

	m_hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		m_applicationName,
		m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight,
		nullptr, nullptr, m_hInstance, nullptr);

	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	ShowCursor(false);
}

void System::ShutdownWindows()
{
	ShowCursor(true);

	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(nullptr, 0);

	}
	DestroyWindow(m_hwnd);
	m_hwnd = nullptr;

	UnregisterClass(m_applicationName, m_hInstance);

	ApplicationHandle = nullptr;
}

bool System::InitializeConsole()
{
	bool result = AllocConsole();
	if (!result)
	{
		return false;
	}

	FILE* file;

	freopen_s(&file, "CONIN$", "r", stdin);
	freopen_s(&file, "CONOUT$", "w", stderr);
	freopen_s(&file, "CONOUT$", "w", stdout);

	return true;
}

LRESULT WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
	}
	}
}
