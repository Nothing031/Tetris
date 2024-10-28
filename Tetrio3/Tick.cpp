#include "Tick.h"

Tick::Tick() {
    this->_interval = 1000000;
    this->tickCount = 0;
    this->running = false;
    this->_externalFlag = nullptr;
    this->externalCV = nullptr;
    SetTickMode();
}
Tick::Tick(const Tick& other) {
    this->_interval = other._interval;
    this->tickCount = other.tickCount;
    this->running = other.running;
    this->_externalFlag = other._externalFlag;
    this->externalCV = other.externalCV;
    SetTickMode();
}
Tick::Tick(int interval, bool* flag, std::condition_variable* cv)
{
    this->_interval = interval;
    this->tickCount = 0;
    this->running = false;
    this->_externalFlag = flag;
    this->externalCV = cv;
    SetTickMode();
}
Tick::Tick(int interval, bool* flag)
{
    this->_interval = interval;
    this->tickCount = 0;
    this->running = false;
    this->_externalFlag = flag;
    this->externalCV = nullptr;
    SetTickMode();
}
Tick::Tick(int interval, std::condition_variable* cv)
{
    this->_interval = interval;
    this->tickCount = 0;
    this->running = false;
    this->_externalFlag = nullptr;
    this->externalCV = cv;
    SetTickMode();
}
Tick::~Tick()
{
    this->Stop();
}



void Tick::Start()
{
    switch (this->mode) {
    case TickMode::CV_FLAG:
        this->running = true;
        _thread = std::thread(&Tick::StartTick_CV_FLAG, this);
        break;
    case TickMode::CV:
        this->running = true;
        _thread = std::thread(&Tick::StartTick_CV, this);
        break;
    case TickMode::FLAG:
        this->running = true;
        _thread = std::thread(&Tick::StartTick_FLAG, this);
        break;
    case TickMode::TickNull:
        break;
    }
}

void Tick::Stop()
{
    this->running = false;
    if (_thread.joinable())
        _thread.join();
}



void Tick::SetTickMode()
{
    if (this->externalCV != nullptr && this->_externalFlag != nullptr) this->mode = TickMode::CV_FLAG;
    else if (this->externalCV != nullptr) this->mode = TickMode::CV;
    else if (this->_externalFlag != nullptr) this->mode = TickMode::FLAG;
    else this->mode = TickMode::TickNull;
}

void Tick::StartTick_CV_FLAG() {
    int start = clock();
    int now;
    while (running) {
        now = clock();
        if (now - start >= this->_interval) {
            if (running) {
                start = clock();
                *this->_externalFlag = true;
                this->externalCV->notify_one();
            }
        }
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}
void Tick::StartTick_CV()
{
    int start = clock();
    int now;
    while (running) {
        now = clock();
        if (now - start >= this->_interval) {
            if (running) {
                this->externalCV->notify_one();
            }
        }
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
    }
}
void Tick::StartTick_FLAG()
{
    int start = clock();
    int now;
    while (running) {
        now = clock();
        if (now - start >= this->_interval) {
            if (running) {
                *this->_externalFlag = true;
            }
        }
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
    }
}
