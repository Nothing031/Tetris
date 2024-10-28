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



std::random_device rd;
std::mt19937 gen(rd());

HANDLE handle = NULL;
HWND hwnd = NULL;

queue<Block> blockQueue; // ready : 6; show : 5
queue<Block> holdQ; // actually not a queue

int gameMap[24][10];
MinoType minoBag[7] = { MinoType::Mino_I, MinoType::Mino_J, MinoType::Mino_L , MinoType::Mino_O , MinoType::Mino_S , MinoType::Mino_Z , MinoType::Mino_T };
int minoBagNum = 0;



void Init();
void SetCursorVisible(bool visible);

void ShowTitle();

void gotoxy(int x, int y);


MinoType GetRandomMino();

int main()
{
	bool run = true;
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

void DrawMap() {
	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			int block = gameMap[y][x];
			if (block == -1)
				block = Color::DarkGray;
			gotoxy((x + Offsets::MAP_X) * 2 , y + Offsets::MAP_Y);
			SetConsoleTextAttribute(handle, block);
			if (block != Color::Black && block != Color::DarkGray)
				std::cout << MAP_BLOCK;
			else
				std::cout << ((block == Color::Black) ? MAP_VOID : MAP_PREDICTED);
		}
	}
}


MinoType GetRandomMino() {
	if (minoBagNum == 0)
		shuffle(minoBag, minoBag + sizeof(minoBag) / sizeof(minoBag[0]), gen);

	MinoType type = minoBag[minoBagNum];

	if (minoBagNum == 6)
		minoBagNum = 0;
	else
		minoBagNum++;

	return type;
}




