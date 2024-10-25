#pragma once
#include <chrono>
#include <thread>
#include <time.h>
#include <ctime>

class Tick
{
public:
	int tickCount;
	bool running;
	Tick(int interval,bool* flag, bool* flag2);
	~Tick();
	void Start();
	void Stop();
private:
	int _interval; 
	bool* _externalFlag;
	bool* _externalFlag2;
	std::thread _thread;

	void Thread();
};

