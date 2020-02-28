#pragma once

#include "BaseClass.h"

#define DIRECTINPUT_VERSION 0x0800


class InputClass : BaseClass
{
	
public:
	InputClass();
	~InputClass();

	bool Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
	void Shutdown();
	bool Frame(float deltaTime);

private:

	int m_ScreenWidth, m_ScreenHeight, m_MouseX, m_MouseY;

};



