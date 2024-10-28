#pragma once
#include <chrono>
#include <thread>
#include <time.h>
#include <ctime>
#include <condition_variable>

class Tick
{
public:
	int tickCount;
	bool running;
	Tick();
	Tick(const Tick& other);
	Tick(int interval,bool* flag, std::condition_variable* cv);
	~Tick();
	Tick& operator=(const Tick& other) {
		if (this != &other) {
			this->_interval = other._interval;
			this->tickCount = other.tickCount;
			this->running = other.running;
			this->_externalFlag = other._externalFlag;
			this->externalCV = other.externalCV;
		}
		return *this;
	}

	void Start();
	void Stop();
private:
	int _interval; 
	bool* _externalFlag;
	std::condition_variable* externalCV;
	std::thread _thread;

	void Thread();
};

