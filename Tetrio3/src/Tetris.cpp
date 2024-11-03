#define NOMINMAX
#include <iostream>
#include <Windows.h>
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


void Tetris::gameLoopInfinity()
{
    DrawBorder();
    // start update input
    this->runInput = true;
    this->updateInputThread = thread(&Tetris::LoopUpdateInput, this, &this->runInput, &this->inputFlag);
    this->inputTick.Start();


    // full game loop
    while (true) {
        memcpy(this->mapExceptBlock, this->gameMap, CpySize::map);

        this->DrawQueueBlocks();

        // refill block
        while (this->nextBlockQueue.size() < 7) {
            this->nextBlockQueue.push(GetRandomMino());
        }
        // set currnet block
        this->CurrentBlock = nextBlockQueue.front();
        nextBlockQueue.pop();
        
        // try spawn
        if (!this->TrySpawn()) {
            system("cls");
            break; //failed
        }


        // run block loop
        BlockState state = this->blockLoop();
        BlockNextHold = false;
        //Hold or pass
        if (state == BlockState::Hold) {
            this->TryHold();
            this->DrawWholeMap();
            memcpy(this->gameMap, this->mapExceptBlock, CpySize::map);
            continue;
        }

        ClearLine();


        memcpy(mapExceptBlock, gameMap, CpySize::map);
        this->DrawWholeMap();
        continue;
        



    }

    // stop inputTick
    this->inputTick.Stop();
    this->CloseLoopUpdateInput(&this->runInput, &this->inputFlag);
}

BlockState Tetris::blockLoop()
{
    this->CalculateGhostPos();
    this->DrawBlock();
    bool* pFlag = &this->gameUpdateFlag;
    gameUpdateTick.Start();
    gravityTick.Start();

    mutex _m;
    unique_lock<mutex> lock(_m);

    BlockState returnState = BlockState::Q;
    
    while (true) {
        memcpy(this->CurrentBlock.prevMinoOffset, this->CurrentBlock.minoOffset, CpySize::offset);
        this->CurrentBlock.prevPos = this->CurrentBlock.pos;
        this->CurrentBlock.prevGhostPos = this->CurrentBlock.ghostPos;

        this->gameUpdateCV.wait(lock, [pFlag] {return *pFlag; });
        this->gameUpdateFlag = false;

        if (this->CurrentBlock.pos.Y == this->CurrentBlock.ghostPos.Y) {
            if (!forceDropTicking) {
                forceDropTicking = true;
                forceDropTick.Start();
            }
                
        }
        else {
            forceDropTick.Stop();
            forceDropFlag = false;
            forceDropTicking = false;
        }

        if (forceDropFlag) {
            this->HardDrop();
            UpdateBlockOnMap();
            this->gameUpdateTick.Stop();
            this->gravityTick.Stop();
            this->leftArrTick.Stop();
            this->leftDasTick.Stop();
            this->rightArrTick.Stop();
            this->rightDasTick.Stop();
            this->sdrrTick.Stop();
            this->forceDropTick.Stop();
            returnState = BlockState::Droped;
            break;
        }

        if (this->gravityFlag) {
            this->gravityFlag = false;
            this->SoftDrop();
        }
        // process input
#pragma region Move
        // left
        if (Keyboard::ArrowLeft == KeyState::Pressing) {
            this->MoveLeft();
            this->leftDasFlag = false;
            Keyboard::ArrowLeft = KeyState::Pressed;
            if (!leftDasTick.running) {
                this->leftDasTick.Start();
            }
            if (!leftArrTick.running) {
                this->leftArrTick.Start();
            }
        }
        else if (Keyboard::ArrowLeft == KeyState::Pressed) {
            if (this->leftDasFlag && leftArrFlag) {
                this->leftArrFlag = false;
                this->MoveLeft();
            }
        }
        else {
            if (leftDasFlag || leftDasTick.running || leftArrTick.running) {
                this->leftDasFlag = false;
                this->leftDasTick.Stop();
                this->leftArrTick.Stop();
            }
        }
        // right
        if (Keyboard::ArrowRight == KeyState::Pressing) {
            this->MoveRight();
            this->rightDasFlag = false;
            Keyboard::ArrowRight = KeyState::Pressed;
            if (!rightDasTick.running) {
                this->rightDasTick.Start();
            }
        }
        else if (Keyboard::ArrowRight == KeyState::Pressed) {
            if (this->rightDasFlag) {
                if (!this->rightArrTick.running) this->rightArrTick.Start();
                if (this->rightArrFlag) {
                    this->rightArrFlag = false;
                    this->MoveRight();
                }
            }
        }
        else {
            if (rightDasFlag || rightDasTick.running || rightArrTick.running) {
                this->rightDasFlag = false;
                this->rightDasTick.Stop();
                this->rightArrTick.Stop();
            }
        }

        // softdrop
        if (Handling::SDRR == 0) {
            if (Keyboard::Keyboard::SoftDrop == KeyState::Pressing) {
                this->CurrentBlock.pos = this->CurrentBlock.ghostPos;
            }
        }
        else {
            if (Keyboard::SoftDrop == KeyState::Pressing) {
                this->SoftDrop();
                this->sdrrFlag = false;
                Keyboard::SoftDrop = KeyState::Pressed;
            }
            else if (Keyboard::SoftDrop == KeyState::Pressed) {
                if (!this->sdrrTick.running) this->sdrrTick.Start();
                if (this->sdrrFlag) {
                    this->sdrrFlag = false;
                    this->SoftDrop();
                }
            }
            else {
                if (sdrrFlag || sdrrTick.running) {
                    this->sdrrFlag = false;
                    this->sdrrTick.Stop();
                }
            }
        }

#pragma endregion
#pragma region Spin
        if (Keyboard::SpinLeft == KeyState::Pressing) {
            Keyboard::SpinLeft = KeyState::Pressed;
            this->SpinLeft();
        }
        if (Keyboard::SpinRight == KeyState::Pressing) {
            Keyboard::SpinRight = KeyState::Pressed;
            this->SpinRight();
        }
        if (Keyboard::SpinFlip == KeyState::Pressing) {
            Keyboard::SpinFlip = KeyState::Pressed;
            this->Flip();
        }
#pragma endregion
#pragma region ETC
        if (Keyboard::HardDrop == KeyState::Pressing) {
            Keyboard::HardDrop = KeyState::Pressed;
            this->HardDrop();
            UpdateBlockOnMap();
            this->gameUpdateTick.Stop();
            this->gravityTick.Stop();
            this->leftArrTick.Stop();
            this->leftDasTick.Stop();
            this->rightArrTick.Stop();
            this->rightDasTick.Stop();
            this->sdrrTick.Stop();
            this->forceDropTick.Stop();
            returnState =  BlockState::Droped;
            break;
        }
        if (Keyboard::Hold == KeyState::Pressing) {
            if (!BlockNextHold) {
                this->gameUpdateTick.Stop();
                this->gravityTick.Stop();
                this->leftArrTick.Stop();
                this->leftDasTick.Stop();
                this->rightArrTick.Stop();
                this->rightDasTick.Stop();
                this->sdrrTick.Stop();
                this->forceDropTick.Stop();
                returnState = BlockState::Hold;
                break;
            }
        } 
#pragma endregion
        this->UpdateBlockOnMap();
        this->DrawBlock();
    }
    UpdateBlockOnMap();
    return returnState;
}

void Tetris::Init(HANDLE &_handle, HWND &_hwnd)
{
	this->gen = mt19937(rd());
	this->handle = _handle;
	this->hwnd = _hwnd;

    // init map
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            gameMap[i][j] = Color::Black;
        }
    }
    memcpy(mapExceptBlock, gameMap, CpySize::map);

    //init minobag
    this->minoBag[0] = EMino::I_MINO;
    this->minoBag[1] = EMino::O_MINO;
    this->minoBag[2] = EMino::J_MINO;
    this->minoBag[3] = EMino::L_MINO;
    this->minoBag[4] = EMino::S_MINO;
    this->minoBag[5] = EMino::T_MINO;
    this->minoBag[6] = EMino::Z_MINO;

    // init etc
    for (int i = 0; i < 7; i++) {
        this->nextBlockQueue.push(GetRandomMino());
    }
    this->HoldBlock = Block(EMino::NULL_MINO);
    this->BlockNextHold = false;

    // init tick
    this->gameUpdateTick = Tick(10, &this->gameUpdateCV, &this->gameUpdateFlag,  nullptr);
    this->forceDropTick = Tick(2000, nullptr, &this->forceDropFlag, nullptr);
    this->inputTick = Tick(10, &this->inputCV, &this->inputFlag, nullptr);
    this->gravityTick = Tick(1000, nullptr, &this->gravityFlag, nullptr);
    this->leftArrTick = Tick(Handling::ARR, nullptr, &this->leftArrFlag, nullptr);
    this->leftDasTick = Tick(Handling::DAS, nullptr, &this->leftDasFlag, nullptr);
    this->rightArrTick = Tick(Handling::ARR, nullptr, &this->rightArrFlag, nullptr);
    this->rightDasTick = Tick(Handling::DAS, nullptr, &this->rightDasFlag, nullptr);
    this->sdrrTick = Tick(Handling::SDRR, nullptr, &this->sdrrFlag, nullptr);
}



void Tetris::CalculateGhostPos()
{
    COORD tempPos = this->CurrentBlock.pos;
    for (int y = this->CurrentBlock.pos.Y; y < 24; y++) {
        tempPos.Y++;
        if (!CollisionCheck(this->CurrentBlock.minoOffset, tempPos)) {
            tempPos.Y--;
            this->CurrentBlock.ghostPos = tempPos;
            return;
        }
    }
}
void Tetris::MoveLeft() {
    COORD tempPos = this->CurrentBlock.pos;
    tempPos.X--;
    if (CollisionCheck(this->CurrentBlock.minoOffset, tempPos)) {
        this->CurrentBlock.pos.X--;
        Tetris::CalculateGhostPos();
    }
}
void Tetris::MoveRight() {
    COORD tempPos = this->CurrentBlock.pos;
    tempPos.X++;
    if (CollisionCheck(this->CurrentBlock.minoOffset, tempPos)) {
        this->CurrentBlock.pos.X++;
        Tetris::CalculateGhostPos();
    }
}
void Tetris::SoftDrop() {
    COORD tempPos = this->CurrentBlock.pos;
    tempPos.Y++;
    if (CollisionCheck(this->CurrentBlock.minoOffset, tempPos)) {
        this->CurrentBlock.pos.Y++;
    }
}
void Tetris::SpinLeft()
{
    BlockState tempState;
    EStateChanges changes;
    COORD tempPos = this->CurrentBlock.pos;
    int tempForm[4][4];
    int tempOffset[4][2];

    // get spined state
    tempState = this->CurrentBlock.state - 1 < 0 ? BlockState::L : (BlockState)(this->CurrentBlock.state - 1);

    // get state changes
    changes = Tetris::GetStateChanges(this->CurrentBlock.state, tempState);

    // get spined mino form
    if (BlockState::S <= tempState && tempState < BlockState::Q)
        memcpy(tempForm, MinoForms[this->CurrentBlock.minoType][tempState], CpySize::form);
    else
        throw new exception("WTFFFF");
    Tetris::FormToOffset(tempForm, tempOffset);

    if (KickCheck(tempOffset, &tempPos, changes)) {
        memcpy(this->CurrentBlock.minoOffset, tempOffset, CpySize::offset);
        this->CurrentBlock.pos = tempPos;
        this->CurrentBlock.state = tempState;
        this->CalculateGhostPos();
    }
}
void Tetris::SpinRight()
{
    BlockState tempState;
    EStateChanges changes;
    COORD tempPos = this->CurrentBlock.pos;
    int tempForm[4][4];
    int tempOffset[4][2];

    // get spined state
    tempState = this->CurrentBlock.state + 1 > 3 ? BlockState::S : (BlockState)(this->CurrentBlock.state + 1);

    // get state changes
    changes = Tetris::GetStateChanges(this->CurrentBlock.state, tempState);

    // get spined mino form
    if (BlockState::S <= tempState && tempState < BlockState::Q)
        memcpy(tempForm, MinoForms[this->CurrentBlock.minoType][tempState], CpySize::form);
    else
        throw new exception("WTFFFF");
    Tetris::FormToOffset(tempForm, tempOffset);

    if (KickCheck(tempOffset, &tempPos, changes)) {
        memcpy(this->CurrentBlock.minoOffset, tempOffset, CpySize::offset);
        this->CurrentBlock.pos = tempPos;
        this->CurrentBlock.state = tempState;
        this->CalculateGhostPos();
    }
}
void Tetris::Flip()
{
    BlockState tempState;
    EStateChanges changes;
    COORD tempPos = this->CurrentBlock.pos;
    int tempForm[4][4];
    int tempOffset[4][2];

    // get spined state
    tempState = this->CurrentBlock.state + 2 < 4 ? (BlockState)(this->CurrentBlock.state + 2) : (BlockState)(this->CurrentBlock.state - 2);

    // get state changes
    changes = Tetris::GetStateChanges(this->CurrentBlock.state, tempState);

    // get spined mino form
    if (BlockState::S <= tempState && tempState < BlockState::Q)
        memcpy(tempForm, MinoForms[this->CurrentBlock.minoType][tempState], CpySize::form);
    else
        throw new exception("WTFFFF");
    Tetris::FormToOffset(tempForm, tempOffset);

    if (KickCheck(tempOffset, &tempPos, changes)) {
        memcpy(this->CurrentBlock.minoOffset, tempOffset, CpySize::offset);
        this->CurrentBlock.pos = tempPos;
        this->CurrentBlock.state = tempState;
        this->CalculateGhostPos();
    }
}
void Tetris::HardDrop()
{
    this->CurrentBlock.pos.Y = this->CurrentBlock.ghostPos.Y;
    this->CurrentBlock.state = BlockState::Droped;
}
bool Tetris::TrySpawn()
{
    // set offset
    this->FormToOffset(MinoForms[this->CurrentBlock.minoType][BlockState::S], this->CurrentBlock.minoOffset);

    // set spawn point
    if (this->CurrentBlock.minoType == EMino::O_MINO) {
        this->CurrentBlock.pos = { 4, 1 };
    }
    else {
        this->CurrentBlock.pos = { 3, 1 };
    }
    // set predicted pos
    this->CalculateGhostPos();

    // set state
    this->CurrentBlock.state = BlockState::S;

    // collision check
    if (CollisionCheck(this->CurrentBlock.minoOffset, this->CurrentBlock.pos)) {
        return true;
    }
    else {
        return false;
    }
}
void Tetris::TryHold()
{
    this->CurrentBlock.state = BlockState::Hold;
    if (!this->BlockNextHold) {
        this->BlockNextHold = true;

        if (HoldBlock.minoType == EMino::NULL_MINO) {
            this->HoldBlock = this->CurrentBlock;
        }
        else {
            queue<Block> tempQueue;
            tempQueue.push(this->HoldBlock);
            while (!this->nextBlockQueue.empty()) {
                tempQueue.push(this->nextBlockQueue.front());
                this->nextBlockQueue.pop();
            }
            this->nextBlockQueue = tempQueue;
            this->HoldBlock = this->CurrentBlock;
        }
    }
    else {
        return;
    }
}


void Tetris::gotoxy(short x, short y) {
    COORD _pos = { x, y };
    SetConsoleCursorPosition(this->handle, _pos);
}
void Tetris::UpdateBlockOnMap() {
// Delete
    for (int i = 0; i < 4; i++) {
        int x = this->CurrentBlock.prevMinoOffset[i][0] + this->CurrentBlock.prevPos.X;
        int y = this->CurrentBlock.prevMinoOffset[i][1] + this->CurrentBlock.prevPos.Y;
        gameMap[y][x] = Color::Black;
    }
    for (int i = 0; i < 4; i++) {
        int x = this->CurrentBlock.prevMinoOffset[i][0] + this->CurrentBlock.prevGhostPos.X;
        int y = this->CurrentBlock.prevMinoOffset[i][1] + this->CurrentBlock.prevGhostPos.Y;
        gameMap[y][x] = Color::Black;
    }
// Draw
    // ghost first
    for (int i = 0; i < 4; i++) {
        int x = this->CurrentBlock.minoOffset[i][0] + this->CurrentBlock.ghostPos.X;
        int y = this->CurrentBlock.minoOffset[i][1] + this->CurrentBlock.ghostPos.Y;
        gameMap[y][x] = Color::DarkGray;
    }
    // block later
    for (int i = 0; i < 4; i++) {
        int x = this->CurrentBlock.minoOffset[i][0] + this->CurrentBlock.pos.X;
        int y = this->CurrentBlock.minoOffset[i][1] + this->CurrentBlock.pos.Y;
        gameMap[y][x] = this->CurrentBlock.minoColor;
    }
}
void Tetris::Pause()
{

}

void Tetris::DrawBlock() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (this->gameMap[y][x] != this->prevGameMap[y][x]) {
                int block = gameMap[y][x];
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
    }
    memcpy(this->prevGameMap, this->gameMap, CpySize::map);
}
void Tetris::DrawWholeMap()
{
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int color = gameMap[y][x];
            gotoxy((x + Offsets::MAP_X) * 2, y + Offsets::MAP_Y);
            SetConsoleTextAttribute(handle, color);
            if (color != Color::Black && color != Color::DarkGray)
                wcout << MAP_BLOCK;
            else
                wcout << ((color == Color::Black) ? MAP_VOID : MAP_GHOST);
        }
    }
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
}
void Tetris::DrawInfo()
{
    // time elapsed
    // line clear
    

}
void Tetris::DrawQueueBlocks()
{
    // Draw queue blocks
    queue<Block> tempQueue = nextBlockQueue;
    tempQueue.pop();
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
    if (this->HoldBlock.minoType != EMino::NULL_MINO) {
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
        FormToOffset(MinoForms[this->HoldBlock.minoType][BlockState::S], _offset);
        if (this->BlockNextHold) {
            SetConsoleTextAttribute(handle, Color::Gray);
            for (int j = 0; j < 4; j++) {
                int _x = _offset[j][0];
                int _y = _offset[j][1];
                gotoxy((1 + _x) * 2, 5 + _y);
                std::wcout << MAP_BLOCK;
            }
        }
        else {
            SetConsoleTextAttribute(handle, this->HoldBlock.minoColor);
            for (int j = 0; j < 4; j++) {
                int _x = _offset[j][0];
                int _y = _offset[j][1];
                gotoxy((1 + _x) * 2, 5 + _y);
                std::wcout << MAP_BLOCK;
            }
        }
    }
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
    for (int y = _y; y >= 1; y--) {
        for (int x = 0; x < 10; x++) {
            gameMap[y][x] = gameMap[y - 1][x];
        }
    }
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
        if (mapExceptBlock[y][x] != Color::Black && mapExceptBlock[y][x] != Color::DarkGray) // block collision!!
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


/// <summary>
/// must notify one after close runflag
/// </summary>
/// <param name="_run"></param>
/// <param name="ready"></param>
/// 
void Tetris::LoopUpdateInput(bool* run, bool* ready)
{
    mutex m;
    unique_lock<mutex> lock(m);
    while (*run) {
        this->inputCV.wait(lock, [ready] {return *ready; });
        *ready = false;
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
    }
}
void Tetris::CloseLoopUpdateInput(bool* run, bool * ready)
{
    *run = false;
    *ready = true;
    this->inputCV.notify_one();
    if (updateInputThread.joinable()) {
        updateInputThread.join();
    }
}
void Tetris::UpdateKeyState(KeyState* _state, const int& keyCode)
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
    if (this->CurrentBlock.minoType == EMino::I_MINO) changes = static_cast<EStateChanges>(static_cast<int>(changes) + 8); // I mino
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

