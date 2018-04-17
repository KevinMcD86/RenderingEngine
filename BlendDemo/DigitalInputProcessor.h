#pragma once

#include "InputProcessor.h"

class DigitalInputProcessor
{
public:
	typedef std::function<void()> ActionBinding;

	DigitalInputProcessor() :
		mIsActivated(false)
	{
	}

	void SetIsActivated(bool isActivated)
	{
		mIsActivated = isActivated;
	}

	bool IsActivated()
	{
		return mIsActivated;
	}

	template <class C>
	void SubscribeToOnPressed(C* instance, void (C::*Function)())
	{
		//ActionBinding binding = static_cast<C*>(instance)->*Function;
		ActionBinding binding = std::bind(Function, instance);
		mPressedSubscribers.push_back(binding);
	}

	template <class C>
	void SubscribeToOnReleased(C* instance, void (C::*Function)())
	{
		ActionBinding binding = std::bind(Function, instance);
		mReleasedSubscribers.push_back(binding);
	}

	// TODO: Release subscribers from both lists

	void OnPressed()
	{
		//TODO
		for (auto subscriberFunction : mPressedSubscribers)
		{
			subscriberFunction();
		}
	}

	void OnReleased()
	{
		//TODO
		for (auto subscriberFunction : mReleasedSubscribers)
		{
			subscriberFunction();
		}
	}

private:
	std::vector<ActionBinding> mPressedSubscribers;
	std::vector<ActionBinding> mReleasedSubscribers;

	bool mIsActivated;
};