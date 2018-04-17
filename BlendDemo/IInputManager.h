#pragma once

#include "../Common/d3dUtil.h"

class IInputManager
{
public:
	virtual LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
};