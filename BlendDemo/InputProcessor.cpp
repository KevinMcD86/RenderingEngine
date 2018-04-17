#include "InputProcessor.h"

InputProcessor::InputProcessor()
{

}

//void InputProcessor::Attach(InputObserver& observer)
//{
//	mInputObservers.push_back(&observer);
//}
//
//void InputProcessor::Detach(InputObserver& observer)
//{
//	mInputObservers.erase(std::remove(mInputObservers.begin(), mInputObservers.end(), &observer));
//}

void InputProcessor::Notify()
{
	/*for (auto observer : mInputObservers)
	{
		observer->InputModified();
	}*/
}