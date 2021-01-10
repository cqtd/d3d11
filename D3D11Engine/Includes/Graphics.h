#pragma once

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <Windows.h>

#include "Utils/Macro.h"
#include "D3D.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public:
	GEN_CLASS_DEFAULT(Graphics)

public:
	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	D3D* m_d3d;
};

#endif