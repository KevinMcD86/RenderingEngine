#pragma once

#include "InputProcessor.h"

class AnalogInputProcessor : public InputProcessor
{
public:
	AnalogInputProcessor() :
		mValue(0.0f)
	{
	}

	void SetValue(float val)
	{
		mValue = val;
	}

	float GetValue()
	{
		return mValue;
	}

private:
	float mValue;
};