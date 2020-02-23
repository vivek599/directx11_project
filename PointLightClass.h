#pragma once

#include "commoninclude.h"


class PointLightClass
{
public:
	PointLightClass();
	~PointLightClass();

	void SetDiffuseColor(float r, float g, float b, float a);
	void SetPosition(float x, float y, float z);

	Vector4 GetDiffuseColor();
	Vector4 GetPosition();

private:
	Vector4 m_diffuseColor;
	Vector4 m_position;
};

