#pragma once
#define NOMINMAX
#include <Windows.h>

#include <iostream>
#include <chrono>
#include <random>
#include <queue>
#include <algorithm>
#include <condition_variable>
#include <mutex>

#include "Block.h"
#include "gameData.h"
#include "Tick.hpp"

using namespace std;

class Tetris
{
public:
	Tetris(HANDLE& _handle, HWND& hwnd) {
		Init(_handle, hwnd);
	}

	void gameLoopInfinity();

private:
	// new tick

	//Tick newUpdateGameTick;
	//condition_variable newUpdateGameCV;
	//bool newUpdateGameFlag;

	//bool newRunning;


	Block CurrentShit;
	queue<Block> nextShitQueue;
	Block HoldShit;
	bool DisableNextHold;

	EMino minoBag[7];
	int minoBagNum = 0;
	int gameMap[MAP_HEIGHT][MAP_WIDTH];
	int collisionMap[MAP_HEIGHT][MAP_WIDTH];

	HANDLE handle;
	HWND hwnd;

	random_device rd;
	mt19937 gen;

	void MoveLeft();
	void MoveRight();
	void SoftDrop();
	void SpinLeft();
	void SpinRight();
	void Flip();
	void HardDrop();
	bool TrySpawn();
	/// <summary>
	/// must called by gameLoop
	/// not the blockLoop
	/// </summary>
	void Hold();


	bool CollisionCheck(int tempOffset[4][2], COORD tempPos);
	bool KickCheck(int tempOffset[4][2], COORD*derefTempPos, EStateChanges changes);

	void gotoxy(short x, short y);
	void UpdateBlockOnMap();
	void Pause();

	void UpdateDisplay(bool* pLoopFlag, bool* pWaitFlag, condition_variable* pCV);
	void DrawWholeMap();
	void DrawBorder();
	void DrawInfo();
	void DrawQueueBlocks();

	int ClearLine();
	void appendLine(int _y);

	Block GetRandomMino();

	inline void CalculateGhostPos();

	EStateChanges GetStateChanges(BlockState currentState, BlockState newState);
	void FormToOffset(const int minoForm[4][4], int outMoniOffset[4][2]);

	void LoopUpdateInput(bool* pLoopFlag, bool* pWaitFlag, condition_variable* pCV);
	void CloseloopThread(thread* pThread, bool* pLoopFlag, bool* pWaitFlag, condition_variable* pCV);
	inline void UpdateKeyState(KeyState* _state, const int& keyCode);


	void Init(HANDLE& _handle, HWND& _hwnd);
};