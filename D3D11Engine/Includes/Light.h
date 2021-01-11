#pragma once

#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Utils/Macro.h"

#include <D3DX10math.h>

class Light
{
public:
	GEN_CLASS_DEFAULT(Light)

public:
	void SetDiffuseColor(float red, float green, float blue, float alpha);
	void SetDirection(float x, float y, float z);

	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR3 GetDirection();

private:
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR3 m_direction;
};

#endif