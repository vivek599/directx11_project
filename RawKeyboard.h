#pragma once

#include "commoninclude.h"

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif

#ifndef HID_USAGE_GENERIC_KEYBOARD
#define HID_USAGE_GENERIC_KEYBOARD        ((USHORT) 0x06)
#endif

class RawKeyboard 
{
public:
	RawKeyboard();
	~RawKeyboard();
	
	void Initialize(HWND hwnd);
	void update(LPARAM lparam);
	void ResetAllKeyData();

	bool IsEscapeKeyDown() { return m_bEscDown; }
	bool IsAlphabeticKeyDown(int nLetter) { return m_bAlphabet[0][nLetter - 'A']; }
	bool IsAlphabeticKeyDownOnce(int nLetter) { return m_bAlphabet[0][nLetter - 'A'] && !m_bAlphabet[1][nLetter - 'A']; }
	bool IsArrowKeyDown(int nArrowDirection) { return m_bArrows[0][nArrowDirection - VK_LEFT]; }
	void updateKeyData();

	bool IsSpaceKeyDown();

	bool IsSpaceKeyUp();
private:
	RAWINPUTDEVICE ridKeyboard;
	RAWINPUT m_keyboardRaw;
	bool  m_bAlphabet[2][26];
	bool  m_bArrows[2][4];
	bool m_bEscDown = false;
	bool m_bSpaceDown = false;
	vector<byte> rawData;
	bool m_bSpaceUp = false;
};

