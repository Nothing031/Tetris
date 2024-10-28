// EUC-KR cp949 encoding
#define NOMINMAX
#include "Block.h"
#include "gameData.h"
#include "Tick.h"
#include "Tetris.h"

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
	setlocale(LC_ALL, "");
	Init();






	Tetris tetris(handle, hwnd);

	tetris.gameLoopInfinity();
	



}

void Init() {
	hwnd = GetConsoleWindow();
	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTitle(TEXT("Tetris"));

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






