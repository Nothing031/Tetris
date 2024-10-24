#pragma once
#define NOMINMAX
#include <Windows.h>
#include <chrono>
#include <random>
#include <queue>
#include <algorithm>
#include "Tick.h"
#include "Block.h"

using namespace std;

class Tetris
{
private:
	Block CurrentBlock;
	queue<Block> NextBlockQueue;
	Block HoldBlock;

	MinoType minoBag[7];
	int minoBagNum = 0;
	int gameMap[24][10];
	int mapExceptBlock[24][10];

	HANDLE handle = NULL;
	HWND hwnd = NULL;

	random_device rd;
	mt19937 gen;


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
	void Queue();
#pragma endregion

	void CalculatePredictedPos();

	bool CollisionCheck(int tempOffset[4][2], COORD tempPos);
	bool KickCheck(int tempOffset[4][2], COORD*derefTempPos, StateChanges changes);

	void GetCheckList(StateChanges changes, int _derefCheckList[5][2]);
	StateChanges GetStateChanges(BlockState currentState, BlockState newState);






	Tetris(HANDLE handle, HWND hwnd) {
		Init(handle, hwnd);
	}
	~Tetris() {

	}
	void Init(HANDLE _handle, HWND _hwnd);
};




