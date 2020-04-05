#include "RawKeyboard.h"



RawKeyboard::RawKeyboard()
{
}


RawKeyboard::~RawKeyboard()
{
}

void RawKeyboard::Initialize(HWND hwnd)
{
	m_keyboardRaw = {};
	ridKeyboard.usUsagePage = HID_USAGE_PAGE_GENERIC;
	ridKeyboard.usUsage = HID_USAGE_GENERIC_KEYBOARD;
	ridKeyboard.dwFlags = RIDEV_NOLEGACY;
	ridKeyboard.hwndTarget = hwnd;

	MYASSERT(RegisterRawInputDevices(&ridKeyboard, 1, sizeof(ridKeyboard)) != FALSE);
	ResetAllKeyData();
}

void RawKeyboard::updateKeyData()
{
	memcpy(m_bAlphabet[1],	m_bAlphabet[0], sizeof(m_bAlphabet[0]));
	memcpy(m_bArrows[1],	m_bArrows[0],	sizeof(m_bArrows[0]));
	m_bSpaceDown = false;
	m_bSpaceUp = false;
}

bool RawKeyboard::IsSpaceKeyDown()
{
	return m_bSpaceDown;
}

bool RawKeyboard::IsAltF4()
{
	return m_bAltF4;
}

bool RawKeyboard::IsSpaceKeyUp()
{
	return m_bSpaceUp;
}

void RawKeyboard::ResetAllKeyData()
{
	// Set all the following to false to be safe

	for (int i = 0; i < 25; ++i)
	{
		m_bAlphabet[0][i] = false;
		m_bAlphabet[1][i] = false;
	}

	for (int i = 0; i < 4; ++i)
	{
		m_bArrows[0][i] = false;
		m_bArrows[1][i] = false;
	}
}

void RawKeyboard::update(LPARAM lparam)
{
	UINT dwSize;

	if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER)) == -1)
		return;

	rawData.resize(dwSize);

	if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, rawData.data(), &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
		OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

	RAWINPUT * raw = (RAWINPUT*)rawData.data();
	if (raw->header.dwType == RIM_TYPEKEYBOARD)
	{
		USHORT keyCode = raw->data.keyboard.VKey;
		bool keyUp = raw->data.keyboard.Flags & RI_KEY_BREAK;

		for (int i = 0; i < 26; ++i)
		{
			if (keyCode == i + 0x41)
			{
				m_bAlphabet[0][i] = !keyUp;
				m_lastkeyCode = keyCode;
				break;
			}
		}

		for (int i = 0; i < 4; ++i)
		{
			if (keyCode == i + VK_LEFT)
			{
				m_bArrows[0][i] = !keyUp;
				m_lastkeyCode = keyCode;
				break;
			}
		}

		m_bAltF4 = (m_lastkeyCode == VK_MENU) && keyCode == VK_F4 && !keyUp;

		switch (keyCode)
		{
			case VK_ESCAPE:
				m_bEscDown = !keyUp;
				m_lastkeyCode = keyCode;
				break;

			case VK_LSHIFT:
			case VK_RSHIFT:
			case VK_LCONTROL:
			case VK_RCONTROL:
			case VK_CAPITAL:
				m_lastkeyCode = keyCode;
				break;

			case VK_LEFT:
			case VK_UP:
			case VK_RIGHT:
			case VK_DOWN:
				m_lastkeyCode = keyCode;
				break;

			case VK_TAB:
				break;

			case VK_SPACE:
				//userInputString.push_back(' ');
				//m_pauseGameLoop = !m_pauseGameLoop;
				m_bSpaceDown = !keyUp;
				m_bSpaceUp = keyUp;
				m_lastkeyCode = keyCode;
				break;

			default:
				m_lastkeyCode = keyCode;
				break;
		}

	}
}

