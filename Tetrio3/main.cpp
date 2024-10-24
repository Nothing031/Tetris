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
void LoopReadKeyboardState(bool* run);
void ReadKeyState(KeyState* _state, const int& keyCode);

MinoType GetRandomMino();
BlockState NULLoneBlockLoop(Block& b);
void NULLgameLoop();

int main()
{

	
	bool run = true;
	Init();
	ShowTitle();
	thread t(LoopReadKeyboardState, &run);
	_getch();
	while(true) {

		NULLgameLoop();
		

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
	std::cout << "    " << "�����"    << " �����"    << " �����"    <<" �����"   <<" ����"   <<" �����" << endl;
	std::cout << "    " << "   ��   " << " ��      " << "    ��   " <<" ��    ��" <<"   ��  " <<" ��" << endl;
	std::cout << "    " << "   ��   " << " �����"    << "    ��   " <<" �����"   <<"   ��  " <<" �����" << endl;
	std::cout << "    " << "   ��   " << " ��      " << "    ��   " <<" ��  ��  " <<"   ��  " <<"       ��" << endl;
	std::cout << "    " << "   ��   " << " �����"    << "    ��   " <<" ��    ��" <<" ����"   <<" �����" << endl;
	#pragma endregion


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
void LoopReadKeyboardState(bool * run)
{
	while(*run) {
		// move
		ReadKeyState(&KeyboardState::ArrowRight, InputKeyNums::A_Right);
		ReadKeyState(&KeyboardState::ArrowLeft, InputKeyNums::A_Left);
		ReadKeyState(&KeyboardState::SoftDrop, InputKeyNums::SoftDrop);
		// spin
		ReadKeyState(&KeyboardState::SpinRight, InputKeyNums::SpinRight);
		ReadKeyState(&KeyboardState::SpinLeft, InputKeyNums::SpinLeft);
		ReadKeyState(&KeyboardState::SpinFlip, InputKeyNums::SpinFilp);
		// etc command
		ReadKeyState(&KeyboardState::Hold, InputKeyNums::Hold);
		ReadKeyState(&KeyboardState::HardDrop, InputKeyNums::HardDrop);
		this_thread::sleep_for(chrono::milliseconds(1));
	}
}

void ReadKeyState(KeyState* _state, const int& keyCode)
{
	int _input = (GetAsyncKeyState(keyCode) & 0x8000);
	if (_input && *_state != KeyState::Pressed)
		*_state = KeyState::Pressing;
	else if (_input && *_state == KeyState::Pressed)
		*_state = KeyState::Pressed;
	else
		*_state = KeyState::Released;
}

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
BlockState NULLoneBlockLoop(Block& b) {
	bool processNext = true;
#pragma region Tick
	//forcedrop
	bool ForceDropCounting = false;
	bool ForceDrop = false;
	Tick ForceDropTick(2000, &ForceDrop, &processNext);
	//gravity
	bool gravityDrop = false;
	Tick GravityTick(1000, &gravityDrop, &processNext);
	GravityTick.Start();
#pragma endregion
	//DAS system
	InputKey currentArrow = k_Null;
	bool fDAS = false;
	bool fARR = false;
	bool fSDF = false;
	Tick DAS_Tick(Handling::DAS, &fDAS, &processNext);
	Tick ARR_Tick(Handling::ARR, &fARR, &processNext);
	Tick SDF_Tick(Handling::SDF, &fSDF, &processNext);

	BlockState _state = b.state;
	queue<InputKey> inputQueue;
	while (true) {
#pragma region ReadKey
		//if (KeyboardState::Hold || KeyboardState::HardDrop) {
		//	processNext = true;
		//	if (KeyboardState::HardDrop == KeyState::Pressing) {
		//		KeyboardState::HardDrop = KeyState::Pressed;
		//		inputQueue.push(InputKey::HardDrop);
		//	}
		//	else if (KeyboardState::Hold == KeyState::Pressing) {
		//		KeyboardState::Hold = KeyState::Pressed;
		//		inputQueue.push(InputKey::Hold);
		//	}
		//}
		//// spin
		//if (KeyboardState::SpinFlip == KeyState::Pressing) {
		//	processNext = true;
		//	KeyboardState::SpinFlip = KeyState::Pressed;
		//	inputQueue.push(InputKey::SpinFilp);
		//}
		//else if (KeyboardState::SpinLeft == KeyState::Pressing) {
		//	processNext = true;
		//	KeyboardState::SpinLeft = KeyState::Pressed;
		//	inputQueue.push(InputKey::SpinLeft);
		//}
		//else if (KeyboardState::SpinRight == KeyState::Pressing) {
		//	processNext = true;
		//	KeyboardState::SpinRight = KeyState::Pressed;
		//	inputQueue.push(InputKey::SpinRight);
		//}
		//// move
		//if (KeyboardState::ArrowLeft) {
		//	if (KeyboardState::ArrowLeft == KeyState::Pressing) {
		//		processNext = true;
		//		KeyboardState::ArrowLeft = KeyState::Pressed;
		//		DAS_Tick.Stop();
		//		DAS_Tick.Start();
		//		inputQueue.push(InputKey::A_Left);
		//	}
		//	else if (KeyboardState::ArrowLeft == KeyState::Pressed) {
		//		if (fDAS) {
		//			fDAS = false;
		//			inputQueue.push(InputKey::A_Left);
		//		}
		//	}
		//}
		//else if (KeyboardState::ArrowRight) {
		//	if (KeyboardState::ArrowRight == KeyState::Pressing) {
		//		processNext = true;
		//		KeyboardState::ArrowRight = KeyState::Pressed;
		//		DAS_Tick.Stop();
		//		DAS_Tick.Start();
		//		inputQueue.push(InputKey::A_Right);
		//	}
		//	else if (KeyboardState::ArrowRight == KeyState::Pressed) {
		//		if (fDAS) {
		//			fDAS = false;
		//			inputQueue.push(InputKey::A_Right);
		//		}
		//	}
		//}
		//else {
		//	DAS_Tick.Stop();
		//	fDAS = false;
		//}		// soft drop

		//if (KeyboardState::SoftDrop) {
		//	if (SDF_Tick.running) {
		//		if (fSDF) {
		//			fSDF = false;
		//			inputQueue.push(InputKey::SoftDrop);
		//		}
		//	}
		//	else {
		//		SDF_Tick.Start();
		//		inputQueue.push(InputKey::SoftDrop);
		//	}
		//}
		//else {
		//	SDF_Tick.Stop();
		//}
#pragma endregion
		if (processNext) {
			processNext = false;
#pragma region TickCheck
			// gravity
			if (gravityDrop) {
				gravityDrop = false;
				b.TryGravityDrop(gameMap);
			}
			// forcedrop
			if (ForceDrop)
				b.ForceDrop();
#pragma endregion
#pragma region BlockProcess
			if (!inputQueue.empty()) {
				while (!inputQueue.empty()) {
					_state = b.Process(gameMap, inputQueue.front());
					inputQueue.pop();
				}
			}
#pragma endregion
#pragma region StateCheck
			if (_state == BlockState::Droped) {
				GravityTick.Stop();
				DrawMap();
				return _state;
			}
			else if (_state == BlockState::Q) {
				GravityTick.Stop();
				return _state;
			}
#pragma endregion
#pragma region Force drop check
			if (b.pos.Y == b.predictedPos.Y && !ForceDropCounting) {
				ForceDropTick.Start();
				ForceDropCounting = true;
			}
			else if (b.pos.Y != b.predictedPos.Y && ForceDropCounting) {
				ForceDropTick.Stop();
				ForceDropCounting = false;
			}
#pragma endregion 
			DrawMap();
		}
		else
			this_thread::sleep_for(chrono::milliseconds(1));
	}
}
void NULLgameLoop() {
	system("cls");
	// initiallize Map
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			gameMap[i][j] = Color::Black;
		}
	}

	DrawBorder();

	for (int i = 0; i < 6; i++) {
		Block b(GetRandomMino());
		blockQueue.push(b);
	}
	int lineClear = 0;
	while (true) { // fullGameLoop;
#pragma region block spawn process
		if (blockQueue.size() < 6) {
			Block b(GetRandomMino());
			blockQueue.push(b);
		}
		// Clearline;
		lineClear += bingoCheck();
		SetConsoleTextAttribute(handle, Color::White);
		gotoxy(17 * 2, 24);
		std::cout << "Lines :                ";
		gotoxy(17 * 2, 24);
		std::cout << "Lines : " << lineClear;
		// draw queue block
		DrawQueueBlocks();

		// get block
		Block block = blockQueue.front();
		blockQueue.pop();
		
		// if collision from spawn point
		if (!block.TrySpawn(gameMap)) {
			break;
		}
#pragma endregion
#pragma region One Block Loop
		BlockState bs = BlockState::Running;// = oneBlockLoop(block);
		if (bs == BlockState::Droped)
			continue;
		if (bs == BlockState::Q) {
			if (!holdQ.empty()) {
				Block _b = holdQ.front();
				holdQ.pop();
				queue<Block> _tempQueue;
				_tempQueue.push(_b);
				while (!blockQueue.empty()) {
					_tempQueue.push(blockQueue.front());
					blockQueue.pop();
				}
				blockQueue = _tempQueue;

				holdQ.push(block);
				continue;
			}
			else {
				holdQ.push(block);
				continue;
			}
		}
#pragma endregion
	}

	SetConsoleTextAttribute(handle, Color::White);
	std::cout << "GAME OVER" << endl;
	int __loop__ = _getch();
	system("cls");
}




