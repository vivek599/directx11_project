#pragma once

#include "commoninclude.h"


class LightClass
{
public:
	LightClass();
	~LightClass();

	void SetSpecularColor(float r, float g, float b, float a);
	void SetSpecularPower(float power);
	void SetAmbientColor(float r, float g, float b, float a);
	void SetDiffuseColor(float r, float g, float b, float a);
	void SetDirection(float x, float y, float z );

	void SetPosition(float x, float y, float z);
	Vector3 GetPosition();
	void SetLookAt(float x, float y, float z);

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float screenNear, float screenDepth);
	void GenerateOrthoMatrix(float screenNear, float screenDepth);

	Mat4 GetViewMatrix();
	Mat4 GetProjectionMatrix();
	Mat4 GetOrthoMatrix();

	Vector4 GetAmbientColor();
	Vector4 GetDiffuseColor();
	Vector3 GetDirection();
	Vector4 GetSpecularColor();
	float GetSpecularPower();

private:
	Vector4 m_ambientColor;
	Vector4 m_diffuseColor;
	Vector3 m_direction;
	Vector4 m_specularColor;
	float m_specularPower;

	Vector3 m_position;
	Vector3 m_lookAt;
	Mat4 m_viewMatrix;
	Mat4 m_projectionMatrix;
	Mat4 m_orthoMatrix;

};

