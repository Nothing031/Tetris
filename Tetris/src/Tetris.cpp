#define NOMINMAX
#include <Windows.h>
#include <conio.h>

#include <iostream>
#include <chrono>
#include <random>
#include <queue>
#include <algorithm>
#include <condition_variable>
#include <mutex>

#include "Tetris.h"
#include "Block.h"
#include "gameData.h"
#include "Tick.hpp"

void Tetris::UpdateDisplay(bool* pLoopFlag, bool* pCV_ReadyFlag, condition_variable* pCV)
{
    *pLoopFlag = true;
    *pCV_ReadyFlag = false;
    mutex _m;
    unique_lock<mutex> lock(_m);
    bool* pFlag = pCV_ReadyFlag;
    int prevMap[MAP_HEIGHT][MAP_WIDTH];
    
    memcpy(prevMap, gameMap, CpySize::map);
    
    int cpyMap[MAP_HEIGHT][MAP_WIDTH];

    while (*pLoopFlag) {
        pCV->wait(lock, [pFlag] {return *pFlag; });
        *pFlag = false;
        memcpy(cpyMap, this->gameMap, CpySize::map);
        
        this->consoleMutex.lock();
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                if (cpyMap[y][x] != prevMap[y][x]) {
                    int block = cpyMap[y][x];
                    SetConsoleTextAttribute(handle, block);
                    if (block == -1)
                        block = Color::DarkGray;

                    gotoxy((x + Offsets::MAP_X) * 2, y + Offsets::MAP_Y);

                    if (block != Color::Black && block != Color::DarkGray)
                        wcout << MAP_BLOCK;
                    else
                        wcout << ((block == Color::Black) ? MAP_VOID : MAP_GHOST);
                } // endif
            } // endfor x
        } // endfor y
        this->consoleMutex.unlock();

        memcpy(prevMap, cpyMap, CpySize::map);
    } // endwhile
}

void Tetris::gameLoopInfinity()
{
#pragma region tons of... of... idk ticks and threads and wtf
    // ReadKey thread, tick
    bool readkeyFlag = true;
    bool readkeyCV_readyFlag = false;
    condition_variable readkeyCV;
    Tick LoopReadKeyTick(1, &readkeyCV, &readkeyCV_readyFlag, nullptr);
    LoopReadKeyTick.Start();
    thread ReadKeyThread(&Tetris::LoopUpdateInput, this, &readkeyFlag, &readkeyCV_readyFlag, &readkeyCV);

    // UpdateDisplay thread
    bool updateDisplayFlag = true;
    bool updateDisplayCV_readyFlag = false;
    condition_variable updateDisplayCV;
    thread UpdateDisplayThread(&Tetris::UpdateDisplay, this, &updateDisplayFlag, &updateDisplayCV_readyFlag, &updateDisplayCV);

    // UpdateGame tick
    bool updateGameCV_readyFlag = false;
    condition_variable updateGameCV;
    Tick updateGameTick(1, &updateGameCV, &updateGameCV_readyFlag, nullptr);
    updateGameTick.Start();
    mutex updateGameMutex;
    unique_lock<mutex> updateGameLock(updateGameMutex);

    // gravity tick
    bool gravityFlag = false;
    Tick gravityTick(1000, nullptr, &gravityFlag, nullptr);

    // forceDrop tick
    bool forceDropFlag = false;
    Tick ForceDropTick(1500, nullptr, &forceDropFlag, nullptr);

    // reset tick
    bool resetFlag = false;
    Tick resetTick(1000, nullptr, &resetFlag, nullptr);


    // move tick
    //// left
    bool leftDasFlag = false;
    Tick leftDasTick(Handling::DAS, nullptr, &leftDasFlag, nullptr);
    bool leftArrFlag = false;
    Tick leftArrTick(Handling::ARR, nullptr, &leftArrFlag, nullptr);
    //// right
    bool rightDasFlag = false;
    Tick rightDasTick(Handling::DAS, nullptr, &rightDasFlag, nullptr);
    bool rightArrFlag = false;
    Tick rightArrTick(Handling::ARR, nullptr, &rightArrFlag, nullptr);
    //// soft drop
    bool sdrrFlag = false;
    Tick sdrrTick(Handling::SDRR, nullptr, &sdrrFlag, nullptr);

    // this massive shit is longer than my dick wtf
#pragma endregion
    bool exitGame = false;
    bool nextUpdateDisplayFlag = true;

    DrawBorder();

    while (!exitGame) {
        // push random mino to the block queue
        while (this->nextShitQueue.size() < 7) {
            this->nextShitQueue.push(GetRandomMino());
        }
        
        // set next block
        this->CurrentShit = this->nextShitQueue.front();
        this->nextShitQueue.pop();

        // draw queue blocks
        DrawQueueBlocks();

        // try spawn
        if (!this->TrySpawn()) {
            system("cls");
            break; //failed
        }

        CalculateGhostPos();
        UpdateBlockOnMap();
        gravityTick.Start();
        
        updateDisplayCV_readyFlag = true;
        updateDisplayCV.notify_one();

        while (!exitGame) {
            updateGameCV.wait(updateGameLock, [&updateGameCV_readyFlag] {return updateGameCV_readyFlag; });
            updateGameCV_readyFlag = false;

            // force drop check
            if (this->CurrentShit.pos.Y == this->CurrentShit.ghostPos.Y) {
                if (!ForceDropTick.running) {
                    ForceDropTick.Start();
                }
            }
            else {
                ForceDropTick.Stop();
                forceDropFlag = false;
            }
            if (forceDropFlag) {
                this->CurrentShit.pos = this->CurrentShit.ghostPos;
                CurrentShit.state = BlockState::Droped;
                break;
            }

            // gravity check
            if (gravityFlag) {
                gravityFlag = false;
                nextUpdateDisplayFlag = true;
                SoftDrop();
            }

            // reset check
            if (resetFlag) {
                resetTick.Stop();
                break;
            }

#pragma region Move
            // left
            if (Keyboard::ArrowLeft != KeyState::Released && Keyboard::ArrowRight != KeyState::Released) {
                leftDasTick.Stop();
                leftArrTick.Stop();
                rightDasTick.Stop();
                rightArrTick.Stop();
            }
            else if (Keyboard::ArrowLeft == KeyState::Pressing) {
                nextUpdateDisplayFlag = true;
                MoveLeft();
                leftDasFlag = false;
                Keyboard::ArrowLeft = KeyState::Pressed;
                if (!leftDasTick.running) {
                    leftDasTick.Start();
                }
                if (!leftArrTick.running) {
                    leftArrTick.Start();
                }
            }
            else if (Keyboard::ArrowLeft == KeyState::Pressed) {
                if (leftDasFlag && leftArrFlag) {
                    leftArrFlag = false;
                    nextUpdateDisplayFlag = true;
                    MoveLeft();
                }
            }
            else {
                if (leftDasFlag || leftDasTick.running || leftArrTick.running) {
                    leftDasFlag = false;
                    leftDasTick.Stop();
                    leftArrTick.Stop();
                }
            }

            // right
            if (Keyboard::ArrowLeft != KeyState::Released && Keyboard::ArrowRight != KeyState::Released) {
                leftDasTick.Stop();
                leftArrTick.Stop();
                rightDasTick.Stop();
                rightArrTick.Stop();
            }
            else if (Keyboard::ArrowRight == KeyState::Pressing) {
                nextUpdateDisplayFlag = true;
                MoveRight();
                rightDasFlag = false;
                Keyboard::ArrowRight = KeyState::Pressed;
                if (!rightDasTick.running) {
                    rightDasTick.Start();
                }
            }
            else if (Keyboard::ArrowRight == KeyState::Pressed) {
                if (rightDasFlag) {
                    if (!rightArrTick.running) rightArrTick.Start();
                    if (rightArrFlag) {
                        rightArrFlag = false;
                        nextUpdateDisplayFlag = true;
                        MoveRight();
                    }
                }
            }
            else {
                if (rightDasFlag || rightDasTick.running || rightArrTick.running) {
                    rightDasFlag = false;
                    rightDasTick.Stop();
                    rightArrTick.Stop();
                }
            }

            // softdrop
            if (Handling::SDRR == 0) {
                if (Keyboard::SoftDrop == KeyState::Pressing) {
                    nextUpdateDisplayFlag = true;
                    this->CurrentShit.pos = this->CurrentShit.ghostPos;
                    UpdateBlockOnMap();
                }
            }
            else {
                if (Keyboard::SoftDrop == KeyState::Pressing) {
                    nextUpdateDisplayFlag = true;
                    SoftDrop();
                    sdrrFlag = false;
                    Keyboard::SoftDrop = KeyState::Pressed;
                }
                else if (Keyboard::SoftDrop == KeyState::Pressed) {
                    if (!sdrrTick.running) sdrrTick.Start();
                    if (sdrrFlag) {
                        sdrrFlag = false;
                        nextUpdateDisplayFlag = true;
                        SoftDrop();
                    }
                }
                else {
                    if (sdrrFlag || sdrrTick.running) {
                        sdrrFlag = false;
                        sdrrTick.Stop();
                    }
                }
            }
#pragma endregion
#pragma region Spin
            if (Keyboard::SpinLeft == KeyState::Pressing) {
                Keyboard::SpinLeft = KeyState::Pressed;
                nextUpdateDisplayFlag = true;
                SpinLeft();
            }
            if (Keyboard::SpinRight == KeyState::Pressing) {
                Keyboard::SpinRight = KeyState::Pressed;
                nextUpdateDisplayFlag = true;
                SpinRight();
            }
            if (Keyboard::SpinFlip == KeyState::Pressing) {
                Keyboard::SpinFlip = KeyState::Pressed;
                nextUpdateDisplayFlag = true;
                Flip();
            }
#pragma endregion
#pragma region ETC
            if (Keyboard::HardDrop == KeyState::Pressing) {
                Keyboard::HardDrop = KeyState::Pressed;
                this->CurrentShit.pos.Y = this->CurrentShit.ghostPos.Y;
                UpdateBlockOnMap();
                this->CurrentShit.state = BlockState::Droped;
                break;
            }
            if (Keyboard::Hold == KeyState::Pressing) {
                Keyboard::Hold = KeyState::Pressed;
                if (!DisableNextHold) {
                    this->CurrentShit.state = BlockState::Hold; 
                    break;
                }
            }
            if (Keyboard::Escape == KeyState::Pressing) {
                Keyboard::Escape = KeyState::Pressed;
                exitGame = Pause();
            }
            if (Keyboard::Reset == KeyState::Pressing) {
                Keyboard::Reset = KeyState::Pressed;
            }
            if (Keyboard::Reset == KeyState::Pressed) {
                if (!resetTick.running) {
                    resetTick.Start();
                    this->consoleMutex.lock();
                    gotoxy(2, 1);
                    SetConsoleTextAttribute(this->handle, Color::White);
                    cout << "RESETTING...";
                    this->consoleMutex.unlock();
                }
            }
            else {
                if (resetTick.running) {
                    resetTick.Stop();
                    this->consoleMutex.lock();
                    gotoxy(2, 1);
                    SetConsoleTextAttribute(this->handle, Color::White);
                    cout << "            ";
                    this->consoleMutex.unlock();
                }
            }




#pragma endregion

            // notify to display thread
            if (nextUpdateDisplayFlag) {
                nextUpdateDisplayFlag = false;
                updateDisplayCV_readyFlag = true;
                updateDisplayCV.notify_one();
            }
        }// endwhile BlockLoop

        gravityTick.Stop();
        ForceDropTick.Stop();
        leftDasTick.Stop();
        leftArrTick.Stop();
        rightDasTick.Stop();
        rightArrTick.Stop();
        sdrrTick.Stop();
        resetTick.Stop();

        if (CurrentShit.state == BlockState::Droped) {
            Drop();
        }
        if (CurrentShit.state == BlockState::Hold) {
            Hold();
        }
        if (resetFlag) {
            resetFlag = false;
            Reset();
        }
    }// endwhile GameLoop

    updateGameTick.Stop();
    LoopReadKeyTick.Stop();
    CloseloopThread(&ReadKeyThread, &readkeyFlag, &readkeyCV_readyFlag, &readkeyCV);
    CloseloopThread(&UpdateDisplayThread, &updateDisplayFlag, &updateDisplayCV_readyFlag, &updateDisplayCV);
    
}

void Tetris::Drop() {
    this->ClearLine();
    this->DisableNextHold = false;
    memcpy(this->collisionMap, this->gameMap, CpySize::map);
}
void Tetris::Hold() {
    this->DisableNextHold = true;
    memcpy(this->gameMap, this->collisionMap, CpySize::map);
    if (this->HoldShit.minoType == EMino::NULL_MINO) {
        this->HoldShit = this->CurrentShit;
    }
    else {
        queue<Block> tempQueue;
        tempQueue.push(this->HoldShit);
        while (!this->nextShitQueue.empty()) {
            tempQueue.push(this->nextShitQueue.front());
            this->nextShitQueue.pop();
        }
        this->nextShitQueue = tempQueue;
        this->HoldShit = this->CurrentShit;
    }
}
void Tetris::Reset() {
    this->consoleMutex.lock();
    Init(this->handle, this->hwnd);
    SetConsoleTextAttribute(this->handle, Color::Black);
    for (int i = 0; i < 4; i++) {
        gotoxy((1 + i) * 2, 5);
        std::wcout << MAP_VOID;
        gotoxy((1 + i) * 2, 6);
        std::wcout << MAP_VOID;
    }
    this->consoleMutex.unlock();
}

void Tetris::Init(HANDLE &_handle, HWND &_hwnd)
{
	this->gen = mt19937(rd());
	this->handle = _handle;
	this->hwnd = _hwnd;

    // init map
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            gameMap[y][x] = Color::Black;
        }
    }
    memcpy(collisionMap, gameMap, CpySize::map);

    //init minobag
    this->minoBag[0] = EMino::I_MINO;
    this->minoBag[1] = EMino::O_MINO;
    this->minoBag[2] = EMino::J_MINO;
    this->minoBag[3] = EMino::L_MINO;
    this->minoBag[4] = EMino::S_MINO;
    this->minoBag[5] = EMino::T_MINO;
    this->minoBag[6] = EMino::Z_MINO;
    this->minoBagNum = 0;

    while (!this->nextShitQueue.empty()) {
        this->nextShitQueue.pop();
    }
    // init etc
    for (int i = 0; i < 7; i++) {
        this->nextShitQueue.push(GetRandomMino());
    }
    this->HoldShit = Block(EMino::NULL_MINO);
    this->DisableNextHold = false;
}

// block things
void Tetris::CalculateGhostPos()
{
    COORD tempPos = this->CurrentShit.pos;
    for (int y = this->CurrentShit.pos.Y; y < 24; y++) {
        tempPos.Y++;
        if (!CollisionCheck(this->CurrentShit.minoOffset, tempPos)) {
            tempPos.Y--;
            this->CurrentShit.ghostPos = tempPos;
            return;
        }
    }
}

void Tetris::MoveLeft() {
    COORD tempPos = this->CurrentShit.pos;
    tempPos.X--;
    if (CollisionCheck(this->CurrentShit.minoOffset, tempPos)) {
        this->CurrentShit.pos.X--;
        this->CalculateGhostPos();
        this->UpdateBlockOnMap();
    }
}
void Tetris::MoveRight() {
    COORD tempPos = this->CurrentShit.pos;
    tempPos.X++;
    if (CollisionCheck(this->CurrentShit.minoOffset, tempPos)) {
        this->CurrentShit.pos.X++;
        this->CalculateGhostPos();
        this->UpdateBlockOnMap();
    }
}
void Tetris::SoftDrop() {
    COORD tempPos = this->CurrentShit.pos;
    tempPos.Y++;
    if (CollisionCheck(this->CurrentShit.minoOffset, tempPos)) {
        this->CurrentShit.pos.Y++;
        this->UpdateBlockOnMap();
    }
}

void Tetris::SpinLeft()
{
    BlockState tempState;
    EStateChanges changes;
    COORD tempPos = this->CurrentShit.pos;
    int tempForm[4][4];
    int tempOffset[4][2];

    // get spined state
    tempState = this->CurrentShit.state - 1 < 0 ? BlockState::L : (BlockState)(this->CurrentShit.state - 1);

    // get state changes
    changes = Tetris::GetStateChanges(this->CurrentShit.state, tempState);

    // get spined mino form
    if (BlockState::S <= tempState && tempState < BlockState::Q)
        memcpy(tempForm, MinoForms[this->CurrentShit.minoType][tempState], CpySize::form);
    else
        throw new exception("WTFFFF");
    Tetris::FormToOffset(tempForm, tempOffset);

    if (KickCheck(tempOffset, &tempPos, changes)) {
        memcpy(this->CurrentShit.minoOffset, tempOffset, CpySize::offset);
        this->CurrentShit.pos = tempPos;
        this->CurrentShit.state = tempState;
        this->CalculateGhostPos();
        this->UpdateBlockOnMap();
    }
}
void Tetris::SpinRight()
{
    BlockState tempState;
    EStateChanges changes;
    COORD tempPos = this->CurrentShit.pos;
    int tempForm[4][4];
    int tempOffset[4][2];

    // get spined state
    tempState = this->CurrentShit.state + 1 > 3 ? BlockState::S : (BlockState)(this->CurrentShit.state + 1);

    // get state changes
    changes = Tetris::GetStateChanges(this->CurrentShit.state, tempState);

    // get spined mino form
    if (BlockState::S <= tempState && tempState < BlockState::Q)
        memcpy(tempForm, MinoForms[this->CurrentShit.minoType][tempState], CpySize::form);
    else
        throw new exception("WTFFFF");
    Tetris::FormToOffset(tempForm, tempOffset);

    if (KickCheck(tempOffset, &tempPos, changes)) {
        memcpy(this->CurrentShit.minoOffset, tempOffset, CpySize::offset);
        this->CurrentShit.pos = tempPos;
        this->CurrentShit.state = tempState;
        this->CalculateGhostPos();
        this->UpdateBlockOnMap();
    }
}
void Tetris::Flip()
{
    BlockState tempState;
    EStateChanges changes;
    COORD tempPos = this->CurrentShit.pos;
    int tempForm[4][4];
    int tempOffset[4][2];

    // get spined state
    tempState = this->CurrentShit.state + 2 < 4 ? (BlockState)(this->CurrentShit.state + 2) : (BlockState)(this->CurrentShit.state - 2);

    // get state changes
    changes = Tetris::GetStateChanges(this->CurrentShit.state, tempState);

    // get spined mino form
    if (BlockState::S <= tempState && tempState < BlockState::Q)
        memcpy(tempForm, MinoForms[this->CurrentShit.minoType][tempState], CpySize::form);
    else
        throw new exception("WTFFFF");
    Tetris::FormToOffset(tempForm, tempOffset);

    if (KickCheck(tempOffset, &tempPos, changes)) {
        memcpy(this->CurrentShit.minoOffset, tempOffset, CpySize::offset);
        this->CurrentShit.pos = tempPos;
        this->CurrentShit.state = tempState;
        this->CalculateGhostPos();
        this->UpdateBlockOnMap();
    }
}

bool Tetris::TrySpawn()
{
    // set offset
    this->FormToOffset(MinoForms[this->CurrentShit.minoType][BlockState::S], this->CurrentShit.minoOffset);

    // set spawn point
    if (this->CurrentShit.minoType == EMino::O_MINO) {
        this->CurrentShit.pos = { 4, 1 };
    }
    else {
        this->CurrentShit.pos = { 3, 1 };
    }
    // set predicted pos
    this->CalculateGhostPos();

    // set state
    this->CurrentShit.state = BlockState::S;

    // collision check
    if (CollisionCheck(this->CurrentShit.minoOffset, this->CurrentShit.pos)) {
        this->UpdateBlockOnMap();
        return true;
    }
    else {
        return false;
    }
}

void Tetris::gotoxy(short x, short y) {
    COORD _pos = { x, y };
    SetConsoleCursorPosition(this->handle, _pos);
}
void Tetris::UpdateBlockOnMap() {
// Delete
    int x;
    int y;
    for (int i = 0; i < 4; i++) {
        x = this->CurrentShit.prevMinoOffset[i][0] + this->CurrentShit.prevGhostPos.X;
        y = this->CurrentShit.prevMinoOffset[i][1] + this->CurrentShit.prevGhostPos.Y;
        gameMap[y][x] = Color::Black;
    }
    for (int i = 0; i < 4; i++) {
        x = this->CurrentShit.prevMinoOffset[i][0] + this->CurrentShit.prevPos.X;
        y = this->CurrentShit.prevMinoOffset[i][1] + this->CurrentShit.prevPos.Y;
        gameMap[y][x] = Color::Black;
    }
// Draw
    // ghost first
    for (int i = 0; i < 4; i++) {
        x = this->CurrentShit.minoOffset[i][0] + this->CurrentShit.ghostPos.X;
        y = this->CurrentShit.minoOffset[i][1] + this->CurrentShit.ghostPos.Y;
        gameMap[y][x] = Color::DarkGray;
    }
    // block later
    for (int i = 0; i < 4; i++) {
        x = this->CurrentShit.minoOffset[i][0] + this->CurrentShit.pos.X;
        y = this->CurrentShit.minoOffset[i][1] + this->CurrentShit.pos.Y;
        gameMap[y][x] = this->CurrentShit.minoColor;
    }

    // set prev data
    memcpy(this->CurrentShit.prevMinoOffset, this->CurrentShit.minoOffset, CpySize::offset);
    this->CurrentShit.prevPos = this->CurrentShit.pos;
    this->CurrentShit.prevGhostPos = this->CurrentShit.ghostPos;
}

/// <summary>
/// return exitGame(true) or not
/// </summary>
/// <returns></returns>
bool Tetris::Pause()
{
    bool escapeGame = false;

    // show pause display
    this->consoleMutex.lock();	
    
    // clear input buffer
    while (_kbhit()) {
        _getch();
    }

    SetConsoleTextAttribute(this->handle, Color::White);
    gotoxy(10, 7);
    wcout << L"┌─────────────────────┐ " << endl;
    gotoxy(10, 8);
    wcout << L"│        Paused       │ " << endl;
    wcout << L"          │                     │ " << endl;
    wcout << L"          │    EXIT│    CONTINUE│ " << endl;
    wcout << L"          └─────────────────────┘ " << endl;
    gotoxy(21, 10);
    wcout << MAP_BLOCK;

    // get input
    bool BreakInput = false;
    int selectedTarget = 1;
    int input;
    while (!BreakInput) {
        input = _getch();
        if (input == ARROW) {
            input = _getch();
        }
        switch (input) {
        case ARROW_LEFT:
            selectedTarget = 0;
            // erase
            SetConsoleTextAttribute(this->handle, Color::Black);
            gotoxy(21, 10);
            wcout << MAP_BLOCK;
            // darw
            SetConsoleTextAttribute(this->handle, Color::White);
            gotoxy(12, 10);
            wcout << MAP_BLOCK;
            break;
        case ARROW_RIGHT:
            selectedTarget = 1;
            // erase
            SetConsoleTextAttribute(this->handle, Color::Black);
            gotoxy(12, 10);
            wcout << MAP_BLOCK;
            // darw
            SetConsoleTextAttribute(this->handle, Color::White);
            gotoxy(21, 10);
            wcout << MAP_BLOCK;
            break;
        case VK_SPACE:
            BreakInput = true;
            break;
        }
    }
    // reDraw Map
    this->consoleMutex.unlock();
    // must unlock before draw functions to prevent deadlock
    this->DrawBorder();
    this->DrawQueueBlocks();
    this->consoleMutex.lock();
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int block = this->gameMap[y][x];
            SetConsoleTextAttribute(handle, block);
            if (block == -1)
                block = Color::DarkGray;
            gotoxy((x + Offsets::MAP_X) * 2, y + Offsets::MAP_Y);
            if (block != Color::Black && block != Color::DarkGray)
                wcout << MAP_BLOCK;
            else
                wcout << ((block == Color::Black) ? MAP_VOID : MAP_GHOST);
        }
    }
    Keyboard::HardDrop = KeyState::Pressed;
    this->consoleMutex.unlock();
    return selectedTarget == 1 ? false : true;
}

void Tetris::DrawBorder()
{
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
    consoleMutex.lock();
    SetConsoleTextAttribute(handle, Color::White);
    // draw horizontal border
    for (int x = 0; x < 6; x++) {
        gotoxy(x * 2, 4);
        std::wcout << MAP_BLOCK;
        gotoxy(x * 2, 8);
        std::wcout << MAP_BLOCK;
        gotoxy((x + 16) * 2, 4);
        std::wcout << MAP_BLOCK;
        gotoxy((x + 16) * 2, 19);
        std::wcout << MAP_BLOCK;
    }
    for (int x = 5; x < 17; x++) {
        gotoxy(x * 2, 24);
        std::wcout << MAP_BLOCK;
    }
    // draw virtical border
    for (int y = 4; y < 25; y++) {
        gotoxy(5 * 2, y);
        std::wcout << MAP_BLOCK;
        gotoxy(16 * 2, y);
        std::wcout << MAP_BLOCK;
    }
    for (int y = 4; y < 9; y++) {
        gotoxy(0, y);
        std::wcout << MAP_BLOCK;
    }
    for (int y = 4; y < 20; y++) {
        gotoxy(21 * 2, y);
        std::wcout << MAP_BLOCK;
    }
    consoleMutex.unlock();
}
void Tetris::DrawInfo()
{
    // time elapsed
    // line clear
    

}
void Tetris::DrawQueueBlocks()
{
    // Draw queue blocks
    queue<Block> tempQueue = nextShitQueue;
    consoleMutex.lock();
    for (int i = 0; i < 5; i++) {
        Block b = tempQueue.front();
        tempQueue.pop();
        // clear
        SetConsoleTextAttribute(handle, Color::Black);
        for (int x = 0; x < 4; x++) {
            gotoxy((17 + x) * 2, (i * 3) + 5);
            std::wcout << MAP_VOID;
            gotoxy((17 + x) * 2, (i * 3) + 6);
            std::wcout << MAP_VOID;
        }
        // draw
        SetConsoleTextAttribute(handle, b.minoColor);
        for (int j = 0; j < 4; j++) {
            int _x = b.minoOffset[j][0];
            int _y = b.minoOffset[j][1];
            gotoxy((17 + _x) * 2, (i * 3) + 5 + _y);
            std::wcout << MAP_BLOCK;
        }
    }
    // draw holding block
    if (this->HoldShit.minoType != EMino::NULL_MINO) {
        // clear
        SetConsoleTextAttribute(handle, Color::Black);
        for (int x = 0; x < 4; x++) {
            gotoxy((1 + x) * 2, 5);
            std::wcout << MAP_VOID;
            gotoxy((1 + x) * 2, 6);
            std::wcout << MAP_VOID;
        }
        // draw
        int _offset[4][2];
        FormToOffset(MinoForms[this->HoldShit.minoType][BlockState::S], _offset);
        if (this->DisableNextHold) {
            SetConsoleTextAttribute(handle, Color::Gray);
            for (int j = 0; j < 4; j++) {
                int _x = _offset[j][0];
                int _y = _offset[j][1];
                gotoxy((1 + _x) * 2, 5 + _y);
                std::wcout << MAP_BLOCK;
            }
        }
        else {
            SetConsoleTextAttribute(handle, this->HoldShit.minoColor);
            for (int j = 0; j < 4; j++) {
                int _x = _offset[j][0];
                int _y = _offset[j][1];
                gotoxy((1 + _x) * 2, 5 + _y);
                std::wcout << MAP_BLOCK;
            }
        }
    }
    consoleMutex.unlock();
}

int Tetris::ClearLine()
{
    int lineCount = 0;
    for (int y = 0; y < MAP_HEIGHT; y++) {
        int blockCount = 0;
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (gameMap[y][x] != Color::Black && gameMap[y][x] != Color::DarkGray) {
                blockCount++;
            }
            if (blockCount == 10) {
                lineCount++;
                appendLine(y);
            }
        }
    }
    return lineCount;
}
void Tetris::appendLine(int _y)
{
    consoleMutex.lock();
    for (int y = _y; y >= 1; y--) {
        for (int x = 0; x < 10; x++) {
            gameMap[y][x] = gameMap[y - 1][x];
        }
    }
    consoleMutex.unlock();
}

bool Tetris::CollisionCheck(int tempOffset[4][2], COORD tempPos){
    // map collision check
    int _0x = tempOffset[0][0] + tempPos.X;
    int _1x = tempOffset[1][0] + tempPos.X;
    int _2x = tempOffset[2][0] + tempPos.X;
    int _3x = tempOffset[3][0] + tempPos.X;

    int _0y = tempOffset[0][1] + tempPos.Y;
    int _1y = tempOffset[1][1] + tempPos.Y;
    int _2y = tempOffset[2][1] + tempPos.Y;
    int _3y = tempOffset[3][1] + tempPos.Y;

    int _maxX = std::max({ _0x, _1x, _2x, _3x });
    int _minX = std::min({ _0x, _1x, _2x, _3x });
    int _maxY = std::max({ _0y, _1y, _2y, _3y });
    int _minY = std::min({ _0y, _1y, _2y, _3y });
    if (_maxX > MAP_WIDTH -1 || _minX < 0 || _maxY > MAP_HEIGHT - 1 || _minY < 0)
        return false;
    
    // block collision check
    for (int i = 0; i < 4; i++) {
        int x = tempOffset[i][0] + tempPos.X;
        int y = tempOffset[i][1] + tempPos.Y;
        if (collisionMap[y][x] != Color::Black && collisionMap[y][x] != Color::DarkGray) // block collision!!
            return false;
    }
    return true;
}
bool Tetris::KickCheck(int tempOffset[4][2], COORD*derefTempPos, EStateChanges changes)
{
    int checkList[5][2];
    memcpy(checkList, CheckList_SRS[changes], CpySize::checkList);

    int passedTest = -1;
    for (int test = 0; test < 5; test++) {
        COORD tempPos = *derefTempPos;
        tempPos.X += checkList[test][0];
        tempPos.Y -= checkList[test][1]; // game +Y = down, -Y = up ; checkList +Y = up, -Y down;
        if (Tetris::CollisionCheck(tempOffset, tempPos)) {
            passedTest = test;
            *derefTempPos = tempPos;
            return true;
        }
    }
    return false;
}

void Tetris::LoopUpdateInput(bool* pLoopFlag, bool* pCV_ReadyFlag, condition_variable* pCV)
{
    *pLoopFlag = true;

    while (*pLoopFlag) {
        // move
        UpdateKeyState(&Keyboard::ArrowRight, InputKeySetting::ArrowRight);
        UpdateKeyState(&Keyboard::ArrowLeft, InputKeySetting::ArrowLeft);
        UpdateKeyState(&Keyboard::SoftDrop, InputKeySetting::SoftDrop);
        // spin
        UpdateKeyState(&Keyboard::SpinRight, InputKeySetting::SpinRight);
        UpdateKeyState(&Keyboard::SpinLeft, InputKeySetting::SpinLeft);
        UpdateKeyState(&Keyboard::SpinFlip, InputKeySetting::SpinFilp);
        // etc command
        UpdateKeyState(&Keyboard::Hold, InputKeySetting::Hold);
        UpdateKeyState(&Keyboard::HardDrop, InputKeySetting::HardDrop);
        UpdateKeyState(&Keyboard::Escape, VK_ESCAPE);
        UpdateKeyState(&Keyboard::Reset, 'R');
        this_thread::sleep_for(chrono::microseconds(100)); //0.1ms
    }
}
void Tetris::CloseloopThread(thread* pThread ,bool* pLoopFlag, bool* pWaitFlag, condition_variable* pCV)
{
    *pLoopFlag = false;
    *pWaitFlag = true;
    pCV->notify_one();
    if (pThread->joinable()) {
        pThread->join();
    }
}
inline void Tetris::UpdateKeyState(KeyState* _state, const int& keyCode)
{
    int _input = (GetAsyncKeyState(keyCode) & 0x8000);
    if (_input && *_state != KeyState::Pressed)
        *_state = KeyState::Pressing;
    else if (_input && *_state == KeyState::Pressed)
        *_state = KeyState::Pressed;
    else
        *_state = KeyState::Released;
}



EStateChanges Tetris::GetStateChanges(BlockState currentState, BlockState newState)
{
    EStateChanges changes = EStateChanges::O;
    switch (currentState) {
    case BlockState::S: changes = newState == BlockState::R ? EStateChanges::S_R : EStateChanges::S_L; break;
    case BlockState::R: changes = newState == BlockState::F ? EStateChanges::R_F : EStateChanges::R_S; break;
    case BlockState::F: changes = newState == BlockState::L ? EStateChanges::F_L : EStateChanges::F_R; break;
    case BlockState::L: changes = newState == BlockState::S ? EStateChanges::L_S : EStateChanges::L_F; break;
    default: break;
    }
    if (this->CurrentShit.minoType == EMino::I_MINO) changes = static_cast<EStateChanges>(static_cast<int>(changes) + 8); // I mino
    return changes;
}
void Tetris::FormToOffset(const int minoForm[4][4], int outMinoOffset[4][2])
{
    int count = 0;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (minoForm[y][x] != 0) {
                outMinoOffset[count][0] = x;
                outMinoOffset[count][1] = y;
                count++;
            }
        }
    }
}
Block Tetris::GetRandomMino() {
    if (this->minoBagNum == 0)
        shuffle(this->minoBag, this->minoBag + sizeof(this->minoBag) / sizeof(this->minoBag[0]), gen);

    EMino type = this->minoBag[this->minoBagNum];

    if (this->minoBagNum == 6)
        this->minoBagNum = 0;
    else
        this->minoBagNum++;

    return type;
}