#pragma once
class Timer
{
private:
	int timer_;
	int timeMem_;

public:
	Timer() { timer_ = timeMem_ = 0; }
	Timer(int timer) { timer_ = timeMem_ = timer; }
	bool CountDown();
	// c‚èŠÔ
	int GetRemainTime() { return timeMem_ - timer_; }
	float GetTimeRate() { return (float)timer_ / (float)timeMem_; }
	float GetRemainTimeRate() { return (float)GetRemainTime() / (float)timeMem_; }
	int GetInterval() { return timeMem_; }
};