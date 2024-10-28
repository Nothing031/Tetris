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
	int gameMap[MAP_HEIGHT][MAP_WIDTH];
	int prevGameMap[MAP_HEIGHT][MAP_WIDTH];
	int mapExceptBlock[MAP_HEIGHT][MAP_WIDTH];

	int prevOffset[4][2];
	COORD prevPos;
	COORD prevPredictedPos;

	HANDLE handle;
	HWND hwnd;

	random_device rd;
	mt19937 gen;

	BlockState blockLoop();

	// tick
	Tick gameUpdateTick;
	Tick inputTick;
	Tick leftArrTick;
	Tick leftDasTick;
	Tick rightArrTick;
	Tick rightDasTick;

	Tick sdrrTick;//soft drop repeat rate

	bool gameUpdateFlag = false;
	bool inputFlag = false;
	bool runInput = false;


	bool leftArrFlag = false;
	bool leftDasFlag = false;
	bool rightArrFlag = false;
	bool rightDasFlag = false;


	bool sdrrFlag = false;

	thread updateInputThread;

	condition_variable gameUpdateCV;
	condition_variable inputCV;


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
	/// <summary>
	/// must called by gameLoop
	/// not the blockLoop
	/// </summary>
	void TryHold();
#pragma endregion


#pragma region Collision check
	bool CollisionCheck(int tempOffset[4][2], COORD tempPos);
	bool KickCheck(int tempOffset[4][2], COORD*derefTempPos, StateChanges changes);
#pragma endregion

#pragma region DrawDeleteETC
	void gotoxy(short x, short y);
	void UpdateBlockOnMap();
	void ReDrawBlock();
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

	void LoopUpdateInput(bool* run, bool* ready);
	void CloseLoopUpdateInput(bool* run);
	void UpdateKeyState(KeyState* _state, const int& keyCode);


	void Init(HANDLE& _handle, HWND& _hwnd);
};