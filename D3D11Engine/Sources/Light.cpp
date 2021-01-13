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

void Light::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = D3DXVECTOR4(red, green, blue, alpha);
}

D3DXVECTOR4 Light::GetAmbientColor()
{
	return this->m_ambientColor;
}

void Light::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
}

void Light::SetDirection(float x, float y, float z)
{
	m_direction = D3DXVECTOR3(x, y, z);
}

void Light::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = D3DXVECTOR4(red, green, blue, alpha);
}

void Light::SetSpecularPower(float power)
{
	m_specularPower = power;
}

D3DXVECTOR4 Light::GetSpecularColor()
{
	return this->m_specularColor;
}

float Light::GetSpecularPower()
{
	return this->m_specularPower;
}

D3DXVECTOR4 Light::GetDiffuseColor()
{
	return this->m_diffuseColor;
}

D3DXVECTOR3 Light::GetDirection()
{
	return this->m_direction;
}
