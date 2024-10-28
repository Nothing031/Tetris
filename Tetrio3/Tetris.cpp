#include "Tetris.h"
#include <Windows.h>
#include <vector>


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
        memcpy(this->prevGameMap, this->gameMap, CpySize::map);

        // refill block
        while (this->nextBlockQueue.size() < 7) {
            this->nextBlockQueue.push(GetRandomMino());
        }
        // set currnet block
        this->CurrentBlock = nextBlockQueue.front();
        nextBlockQueue.pop();
        
        // try spawn
        if (!this->TrySpawn()) {
            break; //failed
        }
        this->UpdateBlockOnMap();
        this->ReDrawBlock();
        // run block loop
        BlockState state = this->blockLoop();
        BlockNextHold = false;
        //Hold or pass
        if (state == BlockState::Hold) {
            this->TryHold();
            // reset gameMap to previuse map;
            memcpy(gameMap, mapExceptBlock, CpySize::map);
            this->ReDrawBlock();
            continue;
        }
        else{
            this->UpdateBlockOnMap();
            this->ReDrawBlock();
            continue;
        }
    }

    // stop inputTick
    this->inputTick.Stop();
    this->CloseLoopUpdateInput(&this->runInput);
}

BlockState Tetris::blockLoop()
{
    int fpscount = 0;
     
    bool* pFlag = &this->gameUpdateFlag;
    gameUpdateTick.Start();

    mutex _m;
    unique_lock<mutex> lock(_m);

    while (true) {
        memcpy(this->prevGameMap, this->gameMap, CpySize::map);
        memcpy(this->prevOffset, this->CurrentBlock.minoOffset, CpySize::offset);
        this->prevPos = this->CurrentBlock.pos;
        this->prevPredictedPos = this->CurrentBlock.predictedPos;

        // process input
#pragma region Move
        if (KeyboardState::ArrowLeft == KeyState::Pressing) {
            this->MoveLeft();
            this->leftDasFlag = false;
            KeyboardState::ArrowLeft = KeyState::Pressed;
            if (!leftDasTick.running) {
                this->leftDasTick.Start();
            }
            if (!leftArrTick.running) {
                this->leftArrTick.Start();
            }
        }
        else if (KeyboardState::ArrowLeft == KeyState::Pressed) {
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

        if (KeyboardState::ArrowRight == KeyState::Pressing) {
            this->MoveRight();
            this->rightDasFlag = false;
            KeyboardState::ArrowRight = KeyState::Pressed;
            if (!rightDasTick.running) {
                this->rightDasTick.Start();
            }
        }
        else if (KeyboardState::ArrowRight == KeyState::Pressed) {
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

        if (KeyboardState::SoftDrop == KeyState::Pressing) {
            this->SoftDrop();
            this->sdrrFlag = false;
            KeyboardState::SoftDrop = KeyState::Pressed;
        }
        else if (KeyboardState::SoftDrop == KeyState::Pressed) {
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
#pragma endregion
#pragma region Spin
        if (KeyboardState::SpinLeft == KeyState::Pressing) {
            KeyboardState::SpinLeft = KeyState::Pressed;
            this->SpinLeft();
        }
        if (KeyboardState::SpinRight == KeyState::Pressing) {
            KeyboardState::SpinRight = KeyState::Pressed;
            this->SpinRight();
        }
        if (KeyboardState::SpinFlip == KeyState::Pressing) {
            KeyboardState::SpinFlip = KeyState::Pressed;
            this->Flip();
        }
#pragma endregion
#pragma region ETC
        if (KeyboardState::HardDrop == KeyState::Pressing) {
            KeyboardState::HardDrop = KeyState::Pressed;
            this->HardDrop();
            UpdateBlockOnMap();
            this->gameUpdateTick.Stop();
            this->leftArrTick.Stop();
            this->leftDasTick.Stop();
            this->rightArrTick.Stop();
            this->rightDasTick.Start();
            this->sdrrTick.Stop();
            return BlockState::Droped;
        }
        if (KeyboardState::Hold == KeyState::Pressing) {
            if (!BlockNextHold) {
                this->gameUpdateTick.Stop();
                this->leftArrTick.Stop();
                this->leftDasTick.Stop();
                this->rightArrTick.Stop();
                this->rightDasTick.Start();
                this->sdrrTick.Stop();
                return BlockState::Hold;
            }
        } 
#pragma endregion


        
        this->CalculatePredictedPos();
        this->UpdateBlockOnMap();
        this->ReDrawBlock();
    }



    return BlockState();
}




void Tetris::Init(HANDLE &_handle, HWND &_hwnd)
{
	this->gen = mt19937(rd());
	this->handle = _handle;
	this->hwnd = _hwnd;
#pragma region Init Map
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            gameMap[i][j] = Color::Black;
        }
    }
    memcpy(mapExceptBlock, gameMap, CpySize::map);
#pragma endregion
#pragma region Init MinoBag
    this->minoBag[0] = MinoType::Mino_I;
    this->minoBag[1] = MinoType::Mino_J;
    this->minoBag[2] = MinoType::Mino_L;
    this->minoBag[3] = MinoType::Mino_O;
    this->minoBag[4] = MinoType::Mino_S;
    this->minoBag[5] = MinoType::Mino_Z;
    this->minoBag[6] = MinoType::Mino_T;
#pragma endregion
#pragma region Init ETC
    for (int i = 0; i < 7; i++) {
        this->nextBlockQueue.push(GetRandomMino());
    }
    this->HoldBlock = Block(MinoType::Mino_NULL);
    this->BlockNextHold = false;
#pragma endregion
#pragma region Init Tick
    this->gameUpdateTick = Tick(10, &this->gameUpdateFlag, &this->gameUpdateCV);
    this->inputTick = Tick(10, &this->inputFlag, &this->inputCV);

    this->leftArrTick = Tick(Handling::ARR, &this->leftArrFlag);
    this->leftDasTick = Tick(Handling::DAS, &this->leftDasFlag);

    this->rightArrTick = Tick(Handling::ARR, &this->rightArrFlag);
    this->rightDasTick = Tick(Handling::DAS, &this->rightDasFlag);

    this->sdrrTick = Tick(Handling::SDRR, &this->sdrrFlag);
#pragma endregion

    




}

#pragma region Move
void Tetris::MoveLeft() {
    COORD tempPos = this->CurrentBlock.pos;
    tempPos.X--;
    if (CollisionCheck(this->CurrentBlock.minoOffset, tempPos)) {
        this->CurrentBlock.pos.X--;
        Tetris::CalculatePredictedPos();
    }
}
void Tetris::MoveRight() {
    COORD tempPos = this->CurrentBlock.pos;
    tempPos.X++;
    if (CollisionCheck(this->CurrentBlock.minoOffset, tempPos)) {
        this->CurrentBlock.pos.X++;
        Tetris::CalculatePredictedPos();
    }
}
void Tetris::SoftDrop() {
    COORD tempPos = this->CurrentBlock.pos;
    tempPos.Y++;
    if (CollisionCheck(this->CurrentBlock.minoOffset, tempPos)) {
        this->CurrentBlock.pos.Y++;
    }
}
#pragma endregion
#pragma region Spin
void Tetris::SpinLeft()
{
    BlockState tempState;
    StateChanges changes;
    COORD tempPos = this->CurrentBlock.pos;
    int tempForm[4][4];
    int tempOffset[4][2];

    // get spined state
    tempState = this->CurrentBlock.state - 1 < 0 ? BlockState::L : (BlockState)(this->CurrentBlock.state - 1);

    // get state changes
    changes = Tetris::GetStateChanges(this->CurrentBlock.state, tempState);

    // get spined mino form
    if (BlockState::S <= tempState && tempState < BlockState::Q)
        memcpy(tempForm, this->CurrentBlock.minoForm_All[tempState], CpySize::form);
    else
        throw new exception("WTFFFF");
    Tetris::FormToOffset(tempForm, tempOffset);

    if (KickCheck(tempOffset, &tempPos, changes)) {
        memcpy(this->CurrentBlock.minoOffset, tempOffset, CpySize::offset);
        this->CurrentBlock.pos = tempPos;
        this->CurrentBlock.state = tempState;
    }
}
void Tetris::SpinRight()
{
    BlockState tempState;
    StateChanges changes;
    COORD tempPos = this->CurrentBlock.pos;
    int tempForm[4][4];
    int tempOffset[4][2];

    // get spined state
    tempState = this->CurrentBlock.state + 1 > 3 ? BlockState::S : (BlockState)(this->CurrentBlock.state + 1);

    // get state changes
    changes = Tetris::GetStateChanges(this->CurrentBlock.state, tempState);

    // get spined mino form
    if (BlockState::S <= tempState && tempState < BlockState::Q)
        memcpy(tempForm, this->CurrentBlock.minoForm_All[tempState], CpySize::form);
    else
        throw new exception("WTFFFF");
    Tetris::FormToOffset(tempForm, tempOffset);

    if (KickCheck(tempOffset, &tempPos, changes)) {
        memcpy(this->CurrentBlock.minoOffset, tempOffset, CpySize::offset);
        this->CurrentBlock.pos = tempPos;
        this->CurrentBlock.state = tempState;
    }
}
void Tetris::Flip()
{
    BlockState tempState;
    StateChanges changes;
    COORD tempPos = this->CurrentBlock.pos;
    int tempForm[4][4];
    int tempOffset[4][2];

    // get spined state
    tempState = this->CurrentBlock.state + 2 < 4 ? (BlockState)(this->CurrentBlock.state + 2) : (BlockState)(this->CurrentBlock.state - 2);

    // get state changes
    changes = Tetris::GetStateChanges(this->CurrentBlock.state, tempState);

    // get spined mino form
    if (BlockState::S <= tempState && tempState < BlockState::Q)
        memcpy(tempForm, this->CurrentBlock.minoForm_All[tempState], CpySize::form);
    else
        throw new exception("WTFFFF");
    Tetris::FormToOffset(tempForm, tempOffset);

    if (KickCheck(tempOffset, &tempPos, changes)) {
        memcpy(this->CurrentBlock.minoOffset, tempOffset, CpySize::offset);
        this->CurrentBlock.pos = tempPos;
        this->CurrentBlock.state = tempState;
    }
}
#pragma endregion
#pragma region ETC
void Tetris::HardDrop()
{
    this->CurrentBlock.pos.Y = this->CurrentBlock.predictedPos.Y;
    this->CurrentBlock.state = BlockState::Droped;
}
void Tetris::TryGravityDrop()
{
    COORD tempPos = this->CurrentBlock.pos;
    tempPos.Y++;
    if (CollisionCheck(this->CurrentBlock.minoOffset, tempPos)) {
        this->CurrentBlock.pos = tempPos;
    }
}
bool Tetris::TrySpawn()
{
    // set offset
    this->FormToOffset(this->CurrentBlock.minoForm_All[BlockState::S], this->CurrentBlock.minoOffset);

    // set spawn point
    if (this->CurrentBlock.type == MinoType::Mino_O) {
        this->CurrentBlock.pos = { 4, 1 };
    }
    else {
        this->CurrentBlock.pos = { 3, 1 };
    }
    // set predicted pos
    this->CalculatePredictedPos();

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

        if (HoldBlock.type == MinoType::Mino_NULL) {
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
#pragma endregion


#pragma region Draw on/Delete from map
void Tetris::gotoxy(short x, short y) {
    COORD _pos = { x, y };
    SetConsoleCursorPosition(this->handle, _pos);
}
void Tetris::UpdateBlockOnMap() {
// Delete
    for (int i = 0; i < 4; i++) {
        int x = this->prevOffset[i][0] + this->prevPos.X;
        int y = this->prevOffset[i][1] + this->prevPos.Y;
        gameMap[y][x] = Color::Black;
    }
    for (int i = 0; i < 4; i++) {
        int x = this->prevOffset[i][0] + this->prevPredictedPos.X;
        int y = this->prevOffset[i][1] + this->prevPredictedPos.Y;
        gameMap[y][x] = Color::Black;
    }
// Draw
    // predicted block first
    for (int i = 0; i < 4; i++) {
        int x = this->CurrentBlock.minoOffset[i][0] + this->CurrentBlock.predictedPos.X;
        int y = this->CurrentBlock.minoOffset[i][1] + this->CurrentBlock.predictedPos.Y;
        gameMap[y][x] = Color::DarkGray;
    }
    // block later
    for (int i = 0; i < 4; i++) {
        int x = this->CurrentBlock.minoOffset[i][0] + this->CurrentBlock.pos.X;
        int y = this->CurrentBlock.minoOffset[i][1] + this->CurrentBlock.pos.Y;
        gameMap[y][x] = this->CurrentBlock.minoColor;
    }
}
void Tetris::ReDrawBlock() {

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
                    wcout << ((block == Color::Black) ? MAP_VOID : MAP_PREDICTED);
            }
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
    if (this->HoldBlock.type != MinoType::Mino_NULL) {
        // clear
        SetConsoleTextAttribute(handle, Color::Black);
        for (int x = 0; x < 4; x++) {
            gotoxy((1 + x) * 2, 5);
            std::wcout << MAP_VOID;
            gotoxy((1 + x) * 2, 6);
            std::wcout << MAP_VOID;
        }
        // draw
        if (this->BlockNextHold) {
            SetConsoleTextAttribute(handle, Color::Gray);
            for (int j = 0; j < 4; j++) {
                int _x = this->HoldBlock.minoOffset[j][0];
                int _y = this->HoldBlock.minoOffset[j][1];
                gotoxy((1 + _x) * 2, 5 + _y);
                std::wcout << MAP_BLOCK;
            }
        }
        else {
            SetConsoleTextAttribute(handle, this->HoldBlock.minoColor);
            for (int j = 0; j < 4; j++) {
                int _x = this->HoldBlock.minoOffset[j][0];
                int _y = this->HoldBlock.minoOffset[j][1];
                gotoxy((1 + _x) * 2, 5 + _y);
                std::wcout << MAP_BLOCK;
            }
        }
    }
}

void Tetris::Pause()
{

}

#pragma endregion

int Tetris::ClearLine()
{
    int lineCount = 0;
    for (int y = 0; y < MAP_WIDTH; y++) {
        int blockCount = 0;
        for (int x = 0; x < 10; x++) {
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

#pragma region Collision check
bool Tetris::CollisionCheck(int tempOffset[4][2], COORD tempPos){
#pragma region MapEscapeCheck
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
    if (_maxX > 9 || _minX < 0 || _maxY > 23 || _minY < 0)
        return false;
#pragma endregion
#pragma region block collsion check
    for (int i = 0; i < 4; i++) {
        int x = tempOffset[i][0] + tempPos.X;
        int y = tempOffset[i][1] + tempPos.Y;
        if (mapExceptBlock[y][x] != Color::Black && mapExceptBlock[y][x] != Color::DarkGray) // block collision!!
            return false;
    }
#pragma endregion
    return true;
}
bool Tetris::KickCheck(int tempOffset[4][2], COORD*derefTempPos, StateChanges changes)
{
    int checkList[5][2];
    GetCheckList(changes, checkList);

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
#pragma endregion

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
        UpdateKeyState(&KeyboardState::ArrowRight, InputKeySetting::ArrowRight);
        UpdateKeyState(&KeyboardState::ArrowLeft, InputKeySetting::ArrowLeft);
        UpdateKeyState(&KeyboardState::SoftDrop, InputKeySetting::SoftDrop);
        // spin
        UpdateKeyState(&KeyboardState::SpinRight, InputKeySetting::SpinRight);
        UpdateKeyState(&KeyboardState::SpinLeft, InputKeySetting::SpinLeft);
        UpdateKeyState(&KeyboardState::SpinFlip, InputKeySetting::SpinFilp);
        // etc command
        UpdateKeyState(&KeyboardState::Hold, InputKeySetting::Hold);
        UpdateKeyState(&KeyboardState::HardDrop, InputKeySetting::HardDrop);
    }
}
void Tetris::CloseLoopUpdateInput(bool* run)
{
    *run = false;
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




void Tetris::CalculatePredictedPos()
{
    COORD tempPos = this->CurrentBlock.pos;
    for (int y = this->CurrentBlock.pos.Y; y < 24; y++) {
        tempPos.Y++;
        if (!CollisionCheck(this->CurrentBlock.minoOffset, tempPos)) {
            tempPos.Y--;
            this->CurrentBlock.predictedPos = tempPos;
            return;
        }
    }
}

void Tetris::GetCheckList(StateChanges changes, int _derefCheckList[5][2])
{
    switch (changes) {
    case StateChanges::S_to_R: memcpy(_derefCheckList, CheckList_SRS::JLSTZ::S_to_R, CpySize::checkList); break;
    case StateChanges::R_to_S: memcpy(_derefCheckList, CheckList_SRS::JLSTZ::R_to_S, CpySize::checkList); break;
    case StateChanges::R_to_F: memcpy(_derefCheckList, CheckList_SRS::JLSTZ::R_to_F, CpySize::checkList); break;
    case StateChanges::F_to_R: memcpy(_derefCheckList, CheckList_SRS::JLSTZ::F_to_R, CpySize::checkList); break;
    case StateChanges::F_to_L: memcpy(_derefCheckList, CheckList_SRS::JLSTZ::F_to_L, CpySize::checkList); break;
    case StateChanges::L_to_F: memcpy(_derefCheckList, CheckList_SRS::JLSTZ::L_to_F, CpySize::checkList); break;
    case StateChanges::L_to_S: memcpy(_derefCheckList, CheckList_SRS::JLSTZ::L_to_S, CpySize::checkList); break;
    case StateChanges::S_to_L: memcpy(_derefCheckList, CheckList_SRS::JLSTZ::S_to_L, CpySize::checkList); break;

    case StateChanges::I_S_to_R: memcpy(_derefCheckList, CheckList_SRS::I::S_to_R, CpySize::checkList); break;
    case StateChanges::I_R_to_S: memcpy(_derefCheckList, CheckList_SRS::I::R_to_S, CpySize::checkList); break;
    case StateChanges::I_R_to_F: memcpy(_derefCheckList, CheckList_SRS::I::R_to_F, CpySize::checkList); break;
    case StateChanges::I_F_to_R: memcpy(_derefCheckList, CheckList_SRS::I::F_to_R, CpySize::checkList); break;
    case StateChanges::I_F_to_L: memcpy(_derefCheckList, CheckList_SRS::I::F_to_L, CpySize::checkList); break;
    case StateChanges::I_L_to_F: memcpy(_derefCheckList, CheckList_SRS::I::L_to_F, CpySize::checkList); break;
    case StateChanges::I_L_to_S: memcpy(_derefCheckList, CheckList_SRS::I::L_to_S, CpySize::checkList); break;
    case StateChanges::I_S_to_L: memcpy(_derefCheckList, CheckList_SRS::I::S_to_L, CpySize::checkList); break;
    default:
        for (int i = 0; i < 5; i++) {
            _derefCheckList[i][0] = 0;
            _derefCheckList[i][1] = 0;
        }
        break;
    }
}
StateChanges Tetris::GetStateChanges(BlockState currentState, BlockState newState)
{
    if (currentState == newState)
        return StateChanges::Default;
    StateChanges changes = StateChanges::Default;
    switch (currentState) {
    case BlockState::S: changes = newState == BlockState::R ? StateChanges::S_to_R : StateChanges::S_to_L; break;
    case BlockState::R: changes = newState == BlockState::F ? StateChanges::R_to_F : StateChanges::R_to_S; break;
    case BlockState::F: changes = newState == BlockState::L ? StateChanges::F_to_L : StateChanges::F_to_R; break;
    case BlockState::L: changes = newState == BlockState::S ? StateChanges::L_to_S : StateChanges::L_to_F; break;
    default: break;
    }
    if (this->CurrentBlock.type == MinoType::Mino_I)
        changes = static_cast<StateChanges>(static_cast<int>(changes) + 8); // I mino
    else if (this->CurrentBlock.type == MinoType::Mino_O)
        changes = StateChanges::Default;
    return changes;
}
void Tetris::FormToOffset(int minoForm[4][4], int outMinoOffset[4][2])
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
    if (minoBagNum == 0)
        shuffle(minoBag, minoBag + sizeof(minoBag) / sizeof(minoBag[0]), gen);

    MinoType type = minoBag[minoBagNum];

    if (minoBagNum == 6)
        minoBagNum = 0;
    else
        minoBagNum++;

    return type;
}

