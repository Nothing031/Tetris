// EUC-KR cp949 encoding
#define NOMINMAX
#include "Block.h"
#include "gameData.h"
#include "Tetris.h"

KeyState Keyboard::ArrowLeft = KeyState::Released;
KeyState Keyboard::ArrowRight = KeyState::Released;
KeyState Keyboard::SpinLeft = KeyState::Released;
KeyState Keyboard::SpinRight = KeyState::Released;
KeyState Keyboard::SpinFlip = KeyState::Released;
KeyState Keyboard::SoftDrop = KeyState::Released;
KeyState Keyboard::HardDrop = KeyState::Released;
KeyState Keyboard::Hold = KeyState::Released;


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
int Handling::SDRR = 0;

#include <Windows.h>
#include <iostream>
#include <thread>
#include <conio.h>
#include <chrono>
#include <random>
#include <algorithm>
#include <queue>
#include <future>

using namespace std;


HANDLE handle = NULL;
HWND hwnd = NULL;


void Init();
void SetCursorVisible(bool visible);

void ShowTitle();

void gotoxy(int x, int y);


int main()
{



	
	Init();






	Tetris tetris(handle, hwnd);

	tetris.gameLoopInfinity();
	
	


}

void Init() {
	setlocale(LC_ALL, "");
	hwnd = GetConsoleWindow();
	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTitle(TEXT("Tetris"));
	SetCursorVisible(false);


	// endtemp Init

}

void SetCursorVisible(bool visible) {
	CONSOLE_CURSOR_INFO     cursorInfo;
	GetConsoleCursorInfo(handle, &cursorInfo);
	cursorInfo.bVisible = visible;
	SetConsoleCursorInfo(handle, &cursorInfo);
}

void ShowTitle()
{
	#pragma region TETRIS TITLE
	std::cout << "    " << "■■■■"    << " ■■■■"    << " ■■■■"    <<" ■■■■"   <<" ■■■"   <<" ■■■■" << endl;
	std::cout << "    " << "   ■   " << " ■      " << "    ■   " <<" ■    ■" <<"   ■  " <<" ■" << endl;
	std::cout << "    " << "   ■   " << " ■■■■"    << "    ■   " <<" ■■■■"   <<"   ■  " <<" ■■■■" << endl;
	std::cout << "    " << "   ■   " << " ■      " << "    ■   " <<" ■  ■  " <<"   ■  " <<"       ■" << endl;
	std::cout << "    " << "   ■   " << " ■■■■"    << "    ■   " <<" ■    ■" <<" ■■■"   <<" ■■■■" << endl;
}

void gotoxy(int x, int y) {
	COORD pos = { (short)x, (short)y };
	SetConsoleCursorPosition(handle, pos);
}






