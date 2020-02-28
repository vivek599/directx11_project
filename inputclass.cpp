#include "inputclass.h"



InputClass::InputClass()
{

}

InputClass::~InputClass()
{
	Shutdown();
}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT hr;

	m_ScreenWidth = screenWidth;
	m_ScreenHeight = screenHeight;

	return true;
}

void InputClass::Shutdown()
{


}

bool InputClass::Frame(float deltaTime)
{
	if (BaseClass::KeyUp(VK_SPACE))
	{
		m_pauseGameLoop = !m_pauseGameLoop;
	}



	return true;
}

