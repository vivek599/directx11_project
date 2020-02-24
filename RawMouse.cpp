#include "RawMouse.h"



RawMouse::RawMouse()
{
}


RawMouse::~RawMouse()
{
}

void RawMouse::Initialize(HWND hwnd)
{
	m_mouseRaw = {};
	ridMouse.usUsagePage = HID_USAGE_PAGE_GENERIC;
	ridMouse.usUsage = HID_USAGE_GENERIC_MOUSE;
	ridMouse.dwFlags = RIDEV_NOLEGACY;
	ridMouse.hwndTarget = hwnd;

	assert(RegisterRawInputDevices(&ridMouse, 1, sizeof(ridMouse)) != FALSE);
}

void RawMouse::update(LPARAM lparam)
{
	UINT dwSize;

	if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER)) == -1)
		return;

	rawData.resize(dwSize);

	if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, rawData.data(), &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
		OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

	RAWINPUT * raw = (RAWINPUT*)rawData.data();
	if (raw->header.dwType == RIM_TYPEMOUSE)
	{
		//if ( raw->data.mouse.lLastX != 0 || raw->data.mouse.lLastY != 0)
		{
			m_mouseRaw = *raw;
			char mData[7][260];
			sprintf_s(mData[0], "Mouse: usFlags=%04x",			raw->data.mouse.usFlags);
			sprintf_s(mData[1], "\nulButtons=%d",				raw->data.mouse.ulButtons);
			sprintf_s(mData[2], "\nusButtonFlags=%d",			raw->data.mouse.usButtonFlags);
			sprintf_s(mData[3], "\nusButtonData=%d",			raw->data.mouse.usButtonData);
			sprintf_s(mData[4], "\nulRawButtons=%d",			raw->data.mouse.ulRawButtons);
			sprintf_s(mData[5], "\nlLastX=%d lLastY=%d",		raw->data.mouse.lLastX, raw->data.mouse.lLastY);
			sprintf_s(mData[6], "\nulExtraInformation=%04x",	raw->data.mouse.ulExtraInformation);

			m_mouseDataString = string(mData[0]) + string(mData[1]) + string(mData[2]) + string(mData[3]) + string(mData[4]) + string(mData[5]) + string(mData[6]);
		}
	}
}

void RawMouse::Clear()
{
	m_mouseRaw.data.mouse.lLastX = 0;
	m_mouseRaw.data.mouse.lLastY = 0;
}

LONG RawMouse::GetDeltaX()
{
	return m_mouseRaw.data.mouse.lLastX;
}

LONG RawMouse::GetDeltaY()
{
	return m_mouseRaw.data.mouse.lLastY;
}

bool RawMouse::LeftMouseDown()
{
	return m_mouseRaw.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN;
}

bool RawMouse::RightMouseDown()
{
	return m_mouseRaw.data.mouse.usButtonFlags& RI_MOUSE_RIGHT_BUTTON_DOWN;
}

bool RawMouse::LeftMouseUp()
{
	return m_mouseRaw.data.mouse.usButtonFlags& RI_MOUSE_LEFT_BUTTON_UP;
}

bool RawMouse::RightMouseUp()
{
	return m_mouseRaw.data.mouse.usButtonFlags& RI_MOUSE_RIGHT_BUTTON_UP;
}
