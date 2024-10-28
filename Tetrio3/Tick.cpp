#include "Tick.h"


Tick::Tick() {
    this->_interval = 1000000;
    this->tickCount = 0;
    this->running = false;
    this->_externalFlag = nullptr;
    this->externalCV = nullptr;
}
Tick::Tick(const Tick& other) {
    this->_interval = other._interval;
    this->tickCount = other.tickCount;
    this->running = other.running;
    this->_externalFlag = other._externalFlag;
    this->externalCV = other.externalCV;
}
Tick::Tick(int interval, bool* flag, std::condition_variable* cv)
{
    this->_interval = interval;
    this->tickCount = 0;
    this->running = false;
    this->_externalFlag = flag;
    this->externalCV = cv;
}
Tick::~Tick()
{
    this->Stop();
}

void Tick::Start()
{
    this->running = true;
    _thread = std::thread(&Tick::Thread, this);
}

void Tick::Stop()
{
    this->running = false;
    if (_thread.joinable())
        _thread.join();
}

void Tick::Thread() {
    int start = clock();
    int now;
    while (running) {
        now = clock();
        if (now - start >= this->_interval) {
            if (running) {
                *this->_externalFlag = true;
                this->externalCV->notify_one();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
