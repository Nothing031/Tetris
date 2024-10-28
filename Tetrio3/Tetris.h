#pragma once
#define NOMINMAX
#include <iostream>
#include <Windows.h>
#include <chrono>
#include <random>
#include <queue>
#include <algorithm>
#include <condition_variable>
#include <mutex>
#include "Tick.h"
#include "Block.h"
#include "gameData.h"

using namespace std;

class Tetris
{
public:
	Tetris(HANDLE& _handle, HWND& hwnd) {
		Init(_handle, hwnd);
	}
	~Tetris() {

	}

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

	HANDLE handle;
	HWND hwnd;

	random_device rd;
	mt19937 gen;

	BlockState blockLoop();

	Tick gameTick;
	Tick inputTick;
	Tick arrTick;
	Tick dasTick;
	Tick sdrrTick;//soft drop repeat rate

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
	void TryGravityDrop();
	bool TrySpawn();
	void TryHold();
#pragma endregion


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

	void LoopUpdateInput(bool* _run, condition_variable * cv, bool* ready);
	void UpdateKeyState(KeyState* _state, const int& keyCode);


	void Init(HANDLE& _handle, HWND& _hwnd);
};




