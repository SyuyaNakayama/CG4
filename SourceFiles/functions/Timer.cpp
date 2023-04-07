#include "Timer.h"

bool Timer::CountDown()
{
	if (--timer_ <= 0) 
	{ 
		timer_ = timeMem_;
		return true; 
	}
	return false;
}