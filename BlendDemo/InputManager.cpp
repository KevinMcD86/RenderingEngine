#include "InputManager.h"

#include <WindowsX.h>

InputManager::InputManager()
{

}

void InputManager::AttachActionMapping(InputState& inputState, DigitalInputProcessor& processor)
{
	mInputStates[inputState.mInputEvent].push_back(&inputState);
	mActionInputMapping[&processor].push_back(&inputState);
}

void InputManager::AttachAxisMapping(InputState& inputState, AnalogInputProcessor& processor)
{
	mInputStates[inputState.mInputEvent].push_back(&inputState);
	mAxisInputMapping[&processor].push_back(&inputState);
}

void InputManager::UpdateInputState(InputEvent inputEvent, float value, bool isActivated)
{
	auto inputStates = mInputStates[inputEvent];
	for (auto inputState : inputStates)
	{
		inputState->mValue = value * inputState->mScale;
		inputState->mPrevIsActivated = inputState->mIsActivated;
		inputState->mIsActivated = isActivated;
	}
}

LRESULT InputManager::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	bool handled = false;

	switch (msg)
	{
	case WM_LBUTTONDOWN:
		UpdateInputState(InputEvent::MOUSE_LEFT_CLICK, KEY_DOWN_VALUE, true);
		handled = true;
		break;
	case WM_LBUTTONUP:
		UpdateInputState(InputEvent::MOUSE_LEFT_CLICK, KEY_UP_VALUE, false);
		handled = true;
		break;
	case WM_RBUTTONDOWN:
		UpdateInputState(InputEvent::MOUSE_RIGHT_CLICK, KEY_DOWN_VALUE, true);
		handled = true;
		break;
	case WM_RBUTTONUP:
		UpdateInputState(InputEvent::MOUSE_RIGHT_CLICK, KEY_UP_VALUE, true);
		handled = true;
		break;

	case WM_KEYUP:
		handled = true;
		if ((int)wParam == VK_F4) {
			UpdateInputState(InputEvent::F4, KEY_UP_VALUE, false);
		} else if ((int)wParam == VK_F5) {
			UpdateInputState(InputEvent::F5, KEY_UP_VALUE, false);
		}
		else {
			handled = false;
		}
		break;

	case WM_KEYDOWN:
		handled = true;
		if ((int)wParam == VK_F4) {
			UpdateInputState(InputEvent::F4, KEY_DOWN_VALUE, true);
		}
		else if ((int)wParam == VK_F5) {
			UpdateInputState(InputEvent::F5, KEY_DOWN_VALUE, true);
		}
		else {
			handled = false;
		}
		break;

	case WM_MOUSEMOVE:
		int x = GET_X_LPARAM(lParam);
		int y = GET_X_LPARAM(lParam);
		UpdateInputState(InputEvent::MOUSE_X, x, x != 0);
		UpdateInputState(InputEvent::MOUSE_Y, y, y != 0);
		handled = true;
		break;
	}

	// Trigger the input events
	TriggerAxisInputEvents();
	TriggerActionInputEvents();

	if (handled)
	{
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void InputManager::TriggerAxisInputEvents()
{
	for (auto mapping : mAxisInputMapping)
	{
		auto processor = mapping.first;
		float value = 0.0f;
		for (auto inputState : mapping.second)
		{
			value += inputState->mValue;
		}

		processor->SetValue(value);
		processor->Notify();
	}
}

void InputManager::TriggerActionInputEvents()
{
	for (auto mapping : mActionInputMapping)
	{
		auto processor = mapping.first;
		bool hasStateChanged = false;
		bool newState = false;
		for (auto inputState : mapping.second)
		{
			if (inputState->mPrevIsActivated != inputState->mIsActivated)
			{
				hasStateChanged = true;
				newState = inputState->mIsActivated;
				inputState->mPrevIsActivated = inputState->mIsActivated;
				break;
			}
		}

		if (hasStateChanged)
		{
			processor->SetIsActivated(newState);
			if (newState)
			{
				processor->OnPressed();
			}
			else
			{
				processor->OnReleased();
			}
		}
	}
}