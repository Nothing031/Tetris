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
	~Tetris() {

	}


	void gameLoopInfinity();

private:
	Block CurrentBlock;
	queue<Block> nextBlockQueue;
	Block HoldBlock;
	bool BlockNextHold;

	EMino minoBag[7];
	int minoBagNum = 0;
	int gameMap[MAP_HEIGHT][MAP_WIDTH];
	int prevGameMap[MAP_HEIGHT][MAP_WIDTH];
	int mapExceptBlock[MAP_HEIGHT][MAP_WIDTH];

	HANDLE handle;
	HWND hwnd;

	random_device rd;
	mt19937 gen;

	BlockState blockLoop();

	// tick
	Tick gameUpdateTick;
	bool gameUpdateFlag = false;

	Tick gravityTick;
	bool gravityFlag = false;

	Tick forceDropTick;
	bool forceDropFlag = false;
	bool forceDropTicking = false;

	Tick inputTick;
	bool inputFlag = false;
	bool runInput = false;

	Tick leftArrTick;
	Tick leftDasTick;
	Tick rightArrTick;
	Tick rightDasTick;
	bool leftArrFlag = false;
	bool leftDasFlag = false;
	bool rightArrFlag = false;
	bool rightDasFlag = false;

	Tick sdrrTick;//soft drop repeat rate
	bool sdrrFlag = false;

	thread updateInputThread;

	condition_variable gameUpdateCV;
	condition_variable inputCV;


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
	void TryHold();


	bool CollisionCheck(int tempOffset[4][2], COORD tempPos);
	bool KickCheck(int tempOffset[4][2], COORD*derefTempPos, EStateChanges changes);

	void gotoxy(short x, short y);
	void UpdateBlockOnMap();
	void Pause();

	void DrawBlock();
	void DrawWholeMap();
	void DrawBorder();
	void DrawInfo();
	void DrawQueueBlocks();


	

	




	int ClearLine();
	void appendLine(int _y);

	Block GetRandomMino();


	void CalculateGhostPos();

	EStateChanges GetStateChanges(BlockState currentState, BlockState newState);
	void FormToOffset(const int minoForm[4][4], int outMoniOffset[4][2]);

	void LoopUpdateInput(bool* run, bool* ready);
	void CloseLoopUpdateInput(bool* run, bool* ready);
	void UpdateKeyState(KeyState* _state, const int& keyCode);


	void Init(HANDLE& _handle, HWND& _hwnd);
};