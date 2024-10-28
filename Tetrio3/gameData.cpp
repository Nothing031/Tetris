#include "gameData.h";

KeyState KeyboardState::ArrowLeft = KeyState::Released;
KeyState KeyboardState::ArrowRight = KeyState::Released;
KeyState KeyboardState::SpinLeft = KeyState::Released;
KeyState KeyboardState::SpinRight = KeyState::Released;
KeyState KeyboardState::SpinFlip = KeyState::Released;
KeyState KeyboardState::Hold = KeyState::Released;
KeyState KeyboardState::SoftDrop = KeyState::Released;
KeyState KeyboardState::HardDrop = KeyState::Released;

// default key
int InputKeySetting::ArrowLeft = 0x25;
int InputKeySetting::ArrowRight = 0x27;
int InputKeySetting::SpinLeft = 0x5A;
int InputKeySetting::SpinRight = 0x58;
int InputKeySetting::SpinFilp = 0x41;
int InputKeySetting::Hold = 0x43;
int InputKeySetting::SoftDrop = 0x28;
int InputKeySetting::HardDrop = 0x20;

int Handling::ARR = 40;
int Handling::DAS = 160;
int Handling::SDRR = 10;