#pragma once
#include <chrono>
#include <thread>
#include <time.h>
#include <ctime>
#include <condition_variable>

enum TickMode :int {
	CV_FLAG,
	CV,
	FLAG,
	TickNull
};

class Tick
{
public:
	int tickCount;
	bool running;
	Tick();
	Tick(const Tick& other);
	Tick(int interval, bool* flag, std::condition_variable* cv);
	Tick(int interval, bool* flag);
	Tick(int interval, std::condition_variable* cv);
	~Tick();
	Tick& operator=(const Tick& other) {
		if (this != &other) {
			this->_interval = other._interval;
			this->tickCount = other.tickCount;
			this->running = other.running;
			this->_externalFlag = other._externalFlag;
			this->externalCV = other.externalCV;
			SetTickMode();
		}
		return *this;
	}

	void Start();
	void Stop();
private:
	TickMode mode;
	int _interval; 
	bool* _externalFlag;
	std::condition_variable* externalCV;
	std::thread _thread;

	void SetTickMode();

	void StartTick_CV_FLAG();
	void StartTick_CV();
	void StartTick_FLAG();
};

