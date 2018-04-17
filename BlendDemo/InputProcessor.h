#pragma once

#include "../Common/d3dUtil.h"
//#include "InputObserver.h"

class InputProcessor
{
private:

public:
	InputProcessor();

	/*void Attach(InputObserver& observer);
	void Detach(InputObserver& observer);*/
	void Notify();

private:
	/*typedef std::function<void(void*, bool)> ActionBinding;
	typedef std::function<void(void*, float)> AxisBinding;
	std::vector<InputObserver*> mInputObservers;*/
};