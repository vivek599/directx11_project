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
	
	m_MouseX = 0;
	m_MouseY = 0;

	return true;
}

void InputClass::Shutdown()
{


}

bool InputClass::Frame(float deltaTime)
{
	GetMouseLocation();

	BaseClass::m_moveCameraForward = IsKeyDown('w');
	BaseClass::m_moveCameraBackward = IsKeyDown('s');
	BaseClass::m_moveCameraLeft = IsKeyDown('a');
	BaseClass::m_moveCameraRight = IsKeyDown('d');
	BaseClass::m_moveCameraUp = IsKeyDown('q');
	BaseClass::m_moveCameraDown = IsKeyDown('e');

	for (int i = 0; i < 256; i++)
	{
		if (IsKeyDownOnce(i))
		{
			switch (i)
			{
			case VK_ESCAPE:
			case VK_LSHIFT:
			case VK_RSHIFT:
			case VK_LCONTROL:
			case VK_RCONTROL:
			case VK_CAPITAL:
				break;

			case VK_LEFT:
			case VK_UP:
			case VK_RIGHT:
			case VK_DOWN:
				break;

			case VK_TAB:
				userInputString.push_back(' ');
				userInputString.push_back(' ');
				userInputString.push_back(' ');
				userInputString.push_back(' ');
				break;

			case VK_SPACE:
				//userInputString.push_back(' ');
				m_pauseGameLoop = !m_pauseGameLoop;
				break;

			//case VK_DECIMAL:
			//	userInputString.clear();
			//	break;

			case VK_BACK:
				if (userInputString.length() > 0)
				{
					userInputString.pop_back();
				}
				break;

			case 'w':
				break;
			case 'a':
				break;
			case 's':
				break;
			case 'd':
				break;

			case 'W':
				break;
			case 'A':
				break;
			case 'S':
				break;
			case 'D':
				break;

			default:
				userInputString.push_back(i);// += (char)i;
				break;
			}
		}
	}

	CopyKeyData();

	return true;
}

bool InputClass::IsEscPressed()
{
	return GetAsyncKeyState(VK_ESCAPE);
}

void InputClass::GetMouseLocation()
{
	POINT mp;
	GetCursorPos(&mp);
	ScreenToClient(GetActiveWindow(), &mp);

	m_MouseX = mp.x;
	m_MouseY = mp.y;

	if (m_MouseX < 0)
	{
		m_MouseX = 0;
	}

	if (m_MouseY < 0)
	{
		m_MouseY = 0;
	}

	if (m_MouseX > m_ScreenWidth)
	{
		m_MouseX = m_ScreenWidth;
	}

	if (m_MouseY > m_ScreenHeight)
	{
		m_MouseY = m_ScreenHeight;
	}
}

void InputClass::CopyKeyData()
{
	memcpy(m_WinKeys[1], m_WinKeys[0], sizeof(m_WinKeys[0]));
}

bool InputClass::IsKeyDown(unsigned int key)
{
	return m_WinKeys[0][key];
}

bool InputClass::IsKeyDownOnce(unsigned int key)
{
	return m_WinKeys[0][key] && !m_WinKeys[1][key];
}

void InputClass::KeyDown(unsigned int input)
{
	m_WinKeys[0][input] = true;
}

void InputClass::KeyUp(unsigned int input)
{
	m_WinKeys[0][input] = false;
	ResetWinKeys();
}

void InputClass::ResetWinKeys()
{
	for (int i = 0; i < 256; i++)
	{
		m_WinKeys[0][i] = false;
		m_WinKeys[1][i] = false;
	}
}

POINT InputClass::GetMouseLoc()
{
	POINT mp;
	mp.x = m_MouseX;
	mp.y = m_MouseY;
	return mp;
}
