#include "PointLightClass.h"



PointLightClass::PointLightClass()
{

}

PointLightClass::PointLightClass(const PointLightClass& obj)
{
	UNREFERENCED_PARAMETER(obj);

}

PointLightClass::~PointLightClass()
{
}

void PointLightClass::SetDiffuseColor(float r, float g, float b, float a)
{
	m_diffuseColor = Vector4(r, g, b, a);
}

void PointLightClass::SetPosition(float x, float y, float z)
{
	m_position = Vector4(x, y, z, 1.0f);
}

Vector4 PointLightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}

Vector4 PointLightClass::GetPosition()
{
	return m_position;
}
