// EUC-KR cp949 encoding
#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include "Block.h"
#include "gameData.h"
#include "Tick.h"
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

void DrawMap();
void DrawQueueBlocks();
void DrawBorder();

void gotoxy(int x, int y);
void ClearLine(int _y);
int bingoCheck();
void LoopUpdateInput(bool* run);
void UpdateKeyState(KeyState* _state, const int& keyCode);

MinoType GetRandomMino();

int main()
{

	
	bool run = true;
	Init();
	ShowTitle();
	thread t(LoopUpdateInput, &run);

	while(true) {

		//NULLgameLoop();
		
		Sleep(100);
	}
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
void ClearLine(int _y) {
	for (int x = 0; x < 10; x++)
		gameMap[_y][x] = Color::Black;

	for (int y = _y; y >= 1; y--) {
		for (int x = 0; x < 10; x++) {
			gameMap[y][x] = gameMap[y - 1][x];
		}
	}
}
int bingoCheck() {
	int lineCount = 0;
	for (int y = 0; y < 24; y++) {
		int blockCount = 0;
		for (int x = 0; x < 10; x++) {
			if (gameMap[y][x] != Color::Black && gameMap[y][x] != Color::DarkGray)
				blockCount++;
		}
		if (blockCount == 10) {
			lineCount++;
			ClearLine(y);
		}
	}
	return lineCount;
}

/// <summary>
/// 125 hz polling rate
/// </summary>
/// <param name="run flag"></param>


void DrawMap() {
	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			int block = gameMap[y][x];
			if (block == -1)
				block = Color::DarkGray;
			gotoxy((x + offsets.MAP_X) * 2 , y + offsets.MAP_Y);
			SetConsoleTextAttribute(handle, block);
			if (block != Color::Black && block != Color::DarkGray)
				std::cout << MAP_BLOCK;
			else
				std::cout << ((block == Color::Black) ? MAP_VOID : MAP_PREDICTED);
		}
	}
}
void DrawBorder() {
	//0      5               16       21
	//////////4             ///////////
	//      //              //       //
	//////////8              ///////////7
	        //              //       //
	        //              ///////////10
			//              //       //
			//              /////////// 13
			//              //       //
			//              ///////////16
			//              //       //
	        //              ///////////19
	        //              //
	        //              //
			//              //
			//////////////////24

	SetConsoleTextAttribute(handle, Color::White);
	// draw horizontal border
	for (int x = 0; x < 6; x++) {
		gotoxy(x * 2, 4);
		std::cout << MAP_BLOCK;
		gotoxy(x * 2, 8);
		std::cout << MAP_BLOCK;
		gotoxy((x + 16) * 2, 4);
		std::cout << MAP_BLOCK;
		gotoxy((x + 16) * 2, 19);
		std::cout << MAP_BLOCK;
	}
	for (int x = 5; x < 17; x++) {
		gotoxy(x * 2, 24);
		std::cout << MAP_BLOCK;
	}
	// draw virtical border
	for (int y = 4; y < 25; y++) {
		gotoxy(5 * 2, y);
		std::cout << MAP_BLOCK;
		gotoxy(16 * 2, y);
		std::cout << MAP_BLOCK;
	}
	for (int y = 4; y < 9; y++) {
		gotoxy(0, y);
		std::cout << MAP_BLOCK;
	}
	for (int y = 4; y < 20; y++) {
		gotoxy(21 * 2, y);
		std::cout << MAP_BLOCK;
	}
}
void DrawQueueBlocks() {
	// draw queue blocks
	queue<Block> _tempQueue = blockQueue;
	_tempQueue.pop();
	for (int i = 0; i < 5; i++) {
		Block b = _tempQueue.front();
		_tempQueue.pop();
		// clear
		SetConsoleTextAttribute(handle, Color::Black);
		for (int x = 0; x < 4; x++) {
			gotoxy((17 + x) * 2, (i * 3) + 5);
			std::cout << MAP_VOID;
			gotoxy((17 + x) * 2, (i * 3) + 6);
			std::cout << MAP_VOID;
		}
		// draw
		SetConsoleTextAttribute(handle, b.minoColor);
		for (int j = 0; j < 4; j++) {
			int _x = b.minoOffset[j][0];
			int _y = b.minoOffset[j][1];
			gotoxy((17 + _x) * 2, (i * 3) + 5 + _y);
			std::cout << MAP_BLOCK;
		}
	}

	// draw hold block
	if (!holdQ.empty()) {
		Block b = holdQ.front();
		// clear
		SetConsoleTextAttribute(handle, Color::Black);
		for (int x = 0; x < 4; x++) {
			gotoxy((1 + x) * 2, 5);
			std::cout << MAP_VOID;
			gotoxy((1 + x) * 2, 6);
			std::cout << MAP_VOID;
		}
		// draw
		SetConsoleTextAttribute(handle, b.minoColor);
		for (int j = 0; j < 4; j++) {
			int _x = b.minoOffset[j][0];
			int _y = b.minoOffset[j][1];
			gotoxy((1 + _x) * 2,5 + _y);
			std::cout << MAP_BLOCK;
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




