#include "gameData.h";


KeyState KeyboardState::SpinRight = KeyState::Released;
KeyState KeyboardState::SpinLeft = KeyState::Released;
KeyState KeyboardState::SpinFlip = KeyState::Released;
KeyState KeyboardState::ArrowLeft = KeyState::Released;
KeyState KeyboardState::ArrowRight = KeyState::Released;
KeyState KeyboardState::SoftDrop = KeyState::Released;
KeyState KeyboardState::Hold = KeyState::Released;
KeyState KeyboardState::HardDrop = KeyState::Released;

// default key
int InputKeyNums::A_Left = 0x25; //
int InputKeyNums::A_Right = 0x27;
int InputKeyNums::SoftDrop = 0x28;
int InputKeyNums::SpinRight = 0x58;
int InputKeyNums::SpinLeft = 0x5A;
int InputKeyNums::SpinFilp = 0x41;
int InputKeyNums::Hold = 0x43;
int InputKeyNums::HardDrop = 0x20;

int Handling::ARR = 10;
int Handling::DAS = 1;
int Handling::SDF = 10;

