#pragma once

#include "commoninclude.h"

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

class RawMouse 
{
public:
	RawMouse();
	~RawMouse();
	void Initialize(HWND hwnd);
	void update(LPARAM lparam);
	void Clear();

	LONG GetDeltaX();
	LONG GetDeltaY();

	bool LeftMouseDown();
	bool RightMouseDown();
	bool LeftMouseUp();
	bool RightMouseUp();

	string m_mouseDataString;

private:
	RAWINPUTDEVICE ridMouse;
	RAWINPUT m_mouseRaw;
	bool m_buttonHeld;
	vector<byte> rawData;
};

