#pragma once

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Utils/Macro.h"
#include <D3DX10math.h>

class Camera
{
public:
	GEN_CLASS_DEFAULT(Camera)

public:
	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	void Render();
	void GetViewMatrix(D3DXMATRIX& matrix);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;

	D3DXMATRIX m_viewMatrix;
};

#endif