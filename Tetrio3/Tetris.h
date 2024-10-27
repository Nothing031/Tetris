#pragma once
#define NOMINMAX
#include <iostream>
#include <Windows.h>
#include <chrono>
#include <random>
#include <queue>
#include <algorithm>
#include "Tick.h"
#include "Block.h"
#include "gameData.h"

using namespace std;

class Tetris
{
public:
	void gameLoopInfinity();

private:
	Block CurrentBlock;
	queue<Block> nextBlockQueue;
	Block HoldBlock;
	bool BlockNextHold;

	MinoType minoBag[7];
	int minoBagNum = 0;
	int gameMap[24][10];
	int mapExceptBlock[24][10];

	HANDLE handle = NULL;
	HWND hwnd = NULL;

	random_device rd;
	mt19937 gen;

	BlockState blockLoop();

#pragma region Move
	void MoveLeft();
	void MoveRight();
	void SoftDrop();
#pragma endregion
#pragma region Spin
	void SpinLeft();
	void SpinRight();
	void Flip();
#pragma endregion
#pragma region ETC
	void HardDrop();
#pragma endregion

	void TryHold();
#pragma region Collision check
	bool CollisionCheck(int tempOffset[4][2], COORD tempPos);
	bool KickCheck(int tempOffset[4][2], COORD*derefTempPos, StateChanges changes);
#pragma endregion

#pragma region DrawDeleteETC
	void gotoxy(short x, short y);
	void UpdateBlockOnMap();
	void DrawMap();
	void DrawBorder();
	void DrawInfo();
	void DrawQueueBlocks();
	void Pause();
#pragma endregion

	int ClearLine();
	void appendLine(int _y);

	Block GetRandomMino();


	void CalculatePredictedPos();

	void GetCheckList(StateChanges changes, int _derefCheckList[5][2]);
	StateChanges GetStateChanges(BlockState currentState, BlockState newState);
	void FormToOffset(int minoForm[4][4], int outMoniOffset[4][2]);

	void LoopReadKeyboardState(bool* _run);

	void ReadKeyState(KeyState* _state, const int& keyCode);


	Tetris(HANDLE handle, HWND hwnd) {
		Init(handle, hwnd);
	}
	~Tetris() {

	}
	void Init(HANDLE _handle, HWND _hwnd);
};




