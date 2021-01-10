#pragma once

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>

#include "Input.h"
#include "Graphics.h"
#include "Utils/Macro.h"

const bool ALLOW_CONSOLE = false;
const bool FORCE_RESOLUTION = true;

class System
{
public:
	GEN_CLASS_DEFAULT(System)

public:
	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
	
private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

	bool InitializeConsole();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hInstance;
	HWND m_hwnd;

	Input* m_input;
	Graphics* m_graphics;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static System* ApplicationHandle = nullptr;

#endif