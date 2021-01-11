#include "Light.h"

Light::Light()
{
}

Light::Light(const Light&)
{
}

Light::~Light()
{
}

void Light::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
}

void Light::SetDirection(float x, float y, float z)
{
	m_direction = D3DXVECTOR3(x, y, z);
}

D3DXVECTOR4 Light::GetDiffuseColor()
{
	return this->m_diffuseColor;
}

D3DXVECTOR3 Light::GetDirection()
{
	return this->m_direction;
}
