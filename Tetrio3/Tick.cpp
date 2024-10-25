#include "Tick.h"


Tick::Tick(int interval, bool* flag, bool* flag2)
{
    this->_interval = interval;
    this->tickCount = 0;
    this->running = false;
    this->_externalFlag = flag;
    this->_externalFlag2 = flag2;
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
                *this->_externalFlag2 = true;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
