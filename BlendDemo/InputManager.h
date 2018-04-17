#pragma once

#include "../Common/d3dUtil.h"
#include "IInputManager.h"
#include "InputProcessor.h"
#include "DigitalInputProcessor.h"
#include "AnalogInputProcessor.h"

enum InputEvent
{
	MOUSE_X, MOUSE_Y, MOUSE_LEFT_CLICK, MOUSE_RIGHT_CLICK,
	W, A, S, D,
	F4, F5
};

struct InputState
{
	InputState(InputEvent inputEvent, float scale) :
		mInputEvent(inputEvent),
		mValue(0.0f),
		mScale(scale),
		mIsActivated(false),
		mPrevIsActivated(false) {}

	InputEvent mInputEvent;
	float mValue;
	float mScale;
	bool mIsActivated;
	bool mPrevIsActivated;
};

class InputManager : IInputManager
{
public:
	InputManager();

	void AttachActionMapping(InputState& inputState, DigitalInputProcessor& processor);
	void AttachAxisMapping(InputState& inputState, AnalogInputProcessor& processor);
	LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void UpdateInputState(InputEvent inputEvent, float value, bool isActivated);
	void TriggerAxisInputEvents();
	void TriggerActionInputEvents();

private:
	const float KEY_DOWN_VALUE = 1.0f;
	const float KEY_UP_VALUE = 0.0f;

	std::unordered_map<DigitalInputProcessor*, std::vector<InputState*>> mActionInputMapping;	// Triggers on key/mouse press/release
	std::unordered_map<AnalogInputProcessor*, std::vector<InputState*>> mAxisInputMapping;		// Triggers every frame
	std::unordered_map<InputEvent, std::vector<InputState*>> mInputStates;
};