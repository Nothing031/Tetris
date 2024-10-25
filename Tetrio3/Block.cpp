
#include "Block.h"


void Block::Init(MinoType initType) {
    this->type = initType;
    this->minoColor = (Color)type;
    this->pos = { 0, 0 };
    this->predictedPos = { 0, 0 };
    this->state = BlockState::Q;
    // mino form
    int sizeint[4][4][4];
    size_t cpySize = sizeof(sizeint);
    switch (this->type) {
    case MinoType::Mino_I: memcpy(this->minoForm_All, MinoForms::Mino_I, cpySize); break;
    case MinoType::Mino_J: memcpy(this->minoForm_All, MinoForms::Mino_J, cpySize); break;
    case MinoType::Mino_L: memcpy(this->minoForm_All, MinoForms::Mino_L, cpySize); break;
    case MinoType::Mino_O: memcpy(this->minoForm_All, MinoForms::Mino_O, cpySize); break;
    case MinoType::Mino_S: memcpy(this->minoForm_All, MinoForms::Mino_S, cpySize); break;
    case MinoType::Mino_T: memcpy(this->minoForm_All, MinoForms::Mino_T, cpySize); break;
    case MinoType::Mino_Z: memcpy(this->minoForm_All, MinoForms::Mino_Z, cpySize); break;
    }
    // mino offset
    FormToOffset(this->minoForm_All[BlockState::S], this->minoOffset);
}

bool Block::TrySpawn(int gameMap[24][10])
{
#pragma region set spawn pos
    if (this->type == MinoType::Mino_I)
        this->pos = { 3, 1 };
    else if (this->type == MinoType::Mino_O)
        this->pos = { 4, 1 };
    else
        this->pos = { 3, 1 };
#pragma endregion
#pragma region set states
    this->state = S;
#pragma endregion
#pragma region Collision Check
    // unique collsion check for this one
    // without self delete
    for (int i = 0; i < 4; i++) {
        int x = this->minoOffset[i][0] + this->pos.X;
        int y = this->minoOffset[i][1] + this->pos.Y;
        if (x >= 10 || x < 0 || y >= 24 || y < 0) // map collision
            return false;
        else if (gameMap[y][x] != 0 && gameMap[y][x] != -1) // block collision!!
            return false;
    }
#pragma endregion // return false when failed collision check
    this->CalculatePredictedPos(gameMap);
    this->DrawOnMap(gameMap);
    return true;
}

BlockState Block::Process(int gameMap[24][10], InputKey input)
{
    if (input != InputKey::k_Null) {
        DeleteFromMap(gameMap);
        BlockState bState = this->MoveSpinBlock(gameMap, input);
        CalculatePredictedPos(gameMap);
        DrawOnMap(gameMap);
        if (bState == BlockState::Q)
            DeleteFromMap(gameMap);
        return bState;
    }
    return this->state;
}

BlockState Block::MoveSpinBlock(int gameMap[24][10], InputKey input)
{
    if (input == InputKey::A_Left || input == InputKey::A_Right || input == InputKey::SoftDrop) {
        COORD tempPos = this->pos;
        if (input == InputKey::A_Left)
            tempPos.X--;
        else if (input == InputKey::A_Right)
            tempPos.X++;
        else if (input == InputKey::SoftDrop)
            tempPos.Y++;
        // collsion check
        if (CollisionCheck(this->minoOffset, tempPos, gameMap))
            this->pos = tempPos;
        return this->state;
    }
    else if (input == InputKey::SpinLeft || input == InputKey::SpinRight || input == InputKey::SpinFilp) {
        BlockState tempState;
        COORD tempPos = this->pos;
        int tempForm[4][4];
        int tempOffset[4][2];

        if (input == InputKey::SpinFilp)
            tempState = this->state + 2 < 4 ? (BlockState)(this->state + 2) : (BlockState)(this->state - 2);
        else {
            if (input == InputKey::SpinLeft)
                tempState = this->state - 1 < 0 ? BlockState::L : (BlockState)(this->state - 1);
            else  // Spin Right
                tempState = this->state + 1 > 3 ? BlockState::S : (BlockState)(this->state + 1);
        }

        // get spined mino form
        if (BlockState::S <= tempState && tempState < BlockState::Q)
            memcpy(tempForm, this->minoForm_All[tempState], sizeof(tempForm));
        else
            abort(); //WTF????????

        // form to offset
        Block::FormToOffset(tempForm, tempOffset);

        // get checkList
        int _checkList[5][2];
        StateChanges changes = Block::GetStaifteChanges(this->state, tempState);
        if (changes == StateChanges::Default)
            return this->state;
        Block::GetCheckList(changes, _checkList);
        
        // spin collision check
        if (SpinCollisionCheck(tempOffset, &tempPos, _checkList, gameMap)) {
            this->pos = tempPos;
            this->state = tempState;
            memcpy(this->minoOffset, tempOffset, sizeof(tempOffset));
            return this->state;
        }
        else
            return this->state;
    }
    else if (input == InputKey::HardDrop) {
        this->pos = this->predictedPos;
        this->state = BlockState::Droped;
        return BlockState::Droped;
    }
    else if (input == InputKey::Hold) {
        this->state = BlockState::Q;
        return BlockState::Q;
    }
    return this->state;
}

void Block::DeleteFromMap(int gameMap[24][10])
{
    for (int i = 0; i < 4; i++) {
        int x = this->minoOffset[i][0] + this->predictedPos.X;
        int y = this->minoOffset[i][1] + this->predictedPos.Y;
        gameMap[y][x] = Color::Black;
    }
    for (int i = 0; i < 4; i++) {
        int x = this->minoOffset[i][0] + this->pos.X;
        int y = this->minoOffset[i][1] + this->pos.Y;
        gameMap[y][x] = Color::Black;
    }
}
void Block::DrawOnMap(int gameMap[24][10])
{
    // predicted block first
    for (int i = 0; i < 4; i++) {
        int x = this->minoOffset[i][0] + this->predictedPos.X;
        int y = this->minoOffset[i][1] + this->predictedPos.Y;
        gameMap[y][x] = Color::DarkGray;
    }

    // block later
    for (int i = 0; i < 4; i++) {
        int x = this->minoOffset[i][0] + this->pos.X;
        int y = this->minoOffset[i][1] + this->pos.Y;
        gameMap[y][x] = this->minoColor;
    }
}

bool Block::SpinCollisionCheck(int _derefTempOffset[4][2], COORD* _derefTempPos, int _checkList[5][2], int _gameMap[24][10])
{
    int passedTestNum = -1;

    // CollisionCheck
    for (int test = 0; test < 5; test++) {
        COORD _tempPos = *_derefTempPos;
        _tempPos.X += _checkList[test][0];
        _tempPos.Y -= _checkList[test][1];
        if (this->CollisionCheck(_derefTempOffset, _tempPos, _gameMap)) {
            passedTestNum = test;
            break;
        }
    }

    if (passedTestNum == -1) {
        return false;
    }
    else {
        // set out coordnance
        _derefTempPos->X += _checkList[passedTestNum][0];
        _derefTempPos->Y -= _checkList[passedTestNum][1];
        return true;
    }
}

bool Block::CollisionCheck(int _tempOffset[4][2], COORD _tempPos, int gameMap[24][10])
{
    int tempGameMap[24][10];
    memcpy(tempGameMap, gameMap, sizeof(tempGameMap));
#pragma region delete ownself to avoid self collision
    // delete ownself to avoid self collision
    for (int i = 0; i < 4; i++) {
        int x = this->minoOffset[i][0] + this->pos.X;
        int y = this->minoOffset[i][1] + this->pos.Y;
        tempGameMap[y][x] = 0;
    }
#pragma endregion

#pragma region MapEscapeCheck
    int _0x = _tempOffset[0][0] + _tempPos.X;
    int _1x = _tempOffset[1][0] + _tempPos.X;
    int _2x = _tempOffset[2][0] + _tempPos.X;
    int _3x = _tempOffset[3][0] + _tempPos.X;

    int _0y = _tempOffset[0][1] + _tempPos.Y;
    int _1y = _tempOffset[1][1] + _tempPos.Y;
    int _2y = _tempOffset[2][1] + _tempPos.Y;
    int _3y = _tempOffset[3][1] + _tempPos.Y;

    int _maxX = std::max({ _0x, _1x, _2x, _3x });
    int _minX = std::min({ _0x, _1x, _2x, _3x });
    int _maxY = std::max({ _0y, _1y, _2y, _3y });
    int _minY = std::min({ _0y, _1y, _2y, _3y });
    
    if (_maxX > 9 || _minX < 0 || _maxY > 24 || _minY < 0)
        return false;
#pragma endregion

#pragma region block collsion check
    for (int i = 0; i < 4; i++) {
        int x = _tempOffset[i][0] + _tempPos.X;
        int y = _tempOffset[i][1] + _tempPos.Y;
        if (tempGameMap[y][x] != Color::Black && tempGameMap[y][x] != Color::DarkGray) // block collision!!
            return false;
    }
    return true;
#pragma endregion
}

void Block::CalculatePredictedPos(int gameMap[24][10])
{
    COORD tempPos = this->pos;
    for (int y = this->pos.Y; y < 24; y++) {
        tempPos.Y ++;
        if (!CollisionCheck(this->minoOffset, tempPos, gameMap)) {
            tempPos.Y--;
            this->predictedPos = tempPos;
            return;
        }
    }
}

void Block::TryGravityDrop(int gameMap[24][10])
{
    int tempOffset[4][2];
    memcpy(tempOffset, this->minoOffset, sizeof(tempOffset));
    COORD tempPos = this->pos;
    tempPos.Y++;
    if (CollisionCheck(tempOffset, tempPos, gameMap)) {
        this->DeleteFromMap(gameMap);
        this->pos = tempPos;
        this->DrawOnMap(gameMap);
    }
        
}

void Block::GetCheckList(StateChanges changes, int _derefCheckList[5][2])
{
    int cpySizeArr[5][2];
    size_t cpySize = sizeof(cpySizeArr);
    switch (changes) {
    case StateChanges::S_to_R: memcpy(_derefCheckList, CheckList_SRS::JLSTZ::S_to_R, cpySize); break;
    case StateChanges::R_to_S: memcpy(_derefCheckList, CheckList_SRS::JLSTZ::R_to_S, cpySize); break;
    case StateChanges::R_to_F: memcpy(_derefCheckList, CheckList_SRS::JLSTZ::R_to_F, cpySize); break;
    case StateChanges::F_to_R: memcpy(_derefCheckList, CheckList_SRS::JLSTZ::F_to_R, cpySize); break;
    case StateChanges::F_to_L: memcpy(_derefCheckList, CheckList_SRS::JLSTZ::F_to_L, cpySize); break;
    case StateChanges::L_to_F: memcpy(_derefCheckList, CheckList_SRS::JLSTZ::L_to_F, cpySize); break;
    case StateChanges::L_to_S: memcpy(_derefCheckList, CheckList_SRS::JLSTZ::L_to_S, cpySize); break;
    case StateChanges::S_to_L: memcpy(_derefCheckList, CheckList_SRS::JLSTZ::S_to_L, cpySize); break;

    case StateChanges::I_S_to_R: memcpy(_derefCheckList, CheckList_SRS::I::S_to_R, cpySize); break;
    case StateChanges::I_R_to_S: memcpy(_derefCheckList, CheckList_SRS::I::R_to_S, cpySize); break;
    case StateChanges::I_R_to_F: memcpy(_derefCheckList, CheckList_SRS::I::R_to_F, cpySize); break;
    case StateChanges::I_F_to_R: memcpy(_derefCheckList, CheckList_SRS::I::F_to_R, cpySize); break;
    case StateChanges::I_F_to_L: memcpy(_derefCheckList, CheckList_SRS::I::F_to_L, cpySize); break;
    case StateChanges::I_L_to_F: memcpy(_derefCheckList, CheckList_SRS::I::L_to_F, cpySize); break;
    case StateChanges::I_L_to_S: memcpy(_derefCheckList, CheckList_SRS::I::L_to_S, cpySize); break;
    case StateChanges::I_S_to_L: memcpy(_derefCheckList, CheckList_SRS::I::S_to_L, cpySize); break;
    default : 
        for (int i = 0; i < 5; i++) {
            cpySizeArr[i][0] = 0;
            cpySizeArr[i][1] = 0;
        }
        memcpy(_derefCheckList, cpySizeArr, cpySize);
        break;
    }
}
StateChanges Block::GetStateChanges(BlockState currentState, BlockState newState)
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
    if (this->type == MinoType::Mino_I)
        changes = (StateChanges)((int)changes + 8); // I mino
    else if (this->type == MinoType::Mino_O)
        changes = StateChanges::Default;

    return changes;
}
void Block::FormToOffset(int minoForm[4][4], int outMinoOffset[4][2])
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

#pragma region Move
void Block::MoveLeft(int gameMap[24][10]) {
    COORD tempPos = this->pos;
    tempPos.X--;
    if (CollisionCheck(this->minoOffset, tempPos, gameMap))
        this->pos.X--;
}
void Block::MoveRight(int gameMap[24][10]) {
    COORD tempPos = this->pos;
    tempPos.X++;
    if (CollisionCheck(this->minoOffset, tempPos, gameMap))
        this->pos.X++;
}
void Block::SoftDrop(int gameMap[24][10]) {
    COORD tempPos = this->pos;
    tempPos.Y++;
    if (CollisionCheck(this->minoOffset, tempPos, gameMap))
        this->pos.Y++;
}
#pragma endregion
#pragma region Spin
void Block::SpinLeft(int gameMap[24][10]) {
    COORD tempPos = this->pos;
    int tempForm[4][4];
    int tempOffset[4][2];
    int _checkList[5][2];

    BlockState tempState = this->state - 1 < 0 ? BlockState::L : (BlockState)(this->state - 1);
    StateChanges changes = Block::GetStateChanges(this->state, tempState);

    if (changes == StateChanges::Default) {
        return;
    } 
    if (BlockState::S <= tempState && tempState < BlockState::Q) {
        memcpy(tempForm, this->minoForm_All[tempState], sizeof(tempForm));
    } 
    else {
        abort(); //WTF????????
    }
        
    Block::FormToOffset(tempForm, tempOffset);
    Block::GetCheckList(changes, _checkList);
    if (SpinCollisionCheck(tempOffset, &tempPos, _checkList, gameMap)) {
        this->pos = tempPos;
        this->state = tempState;
        memcpy(this->minoOffset, tempOffset, sizeof(tempOffset));
    }
}
void Block::SpinRight(int gameMap[24][10]) {
    COORD tempPos = this->pos;
    int tempForm[4][4];
    int tempOffset[4][2];
    int _checkList[5][2];

    BlockState tempState = this->state + 1 > 3 ? BlockState::S : (BlockState)(this->state + 1);
    StateChanges changes = Block::GetStateChanges(this->state, tempState);

    if (changes == StateChanges::Default) {
        return;
    }
    if (BlockState::S <= tempState && tempState < BlockState::Q) {
        memcpy(tempForm, this->minoForm_All[tempState], sizeof(tempForm));
    }
    else {
        abort(); //WTF????????
    }

    Block::FormToOffset(tempForm, tempOffset);
    Block::GetCheckList(changes, _checkList);
    if (SpinCollisionCheck(tempOffset, &tempPos, _checkList, gameMap)) {
        this->pos = tempPos;
        this->state = tempState;
        memcpy(this->minoOffset, tempOffset, sizeof(tempOffset));
    }
}
void Block::Flip(int gameMap[24][10]) {
    COORD tempPos = this->pos;
    int tempForm[4][4];
    int tempOffset[4][2];
    int _checkList[5][2];

    BlockState tempState = this->state + 2 < 4 ? (BlockState)(this->state + 2) : (BlockState)(this->state - 2);
    StateChanges changes = Block::GetStateChanges(this->state, tempState);

    if (changes == StateChanges::Default) {
        return;
    }
    if (BlockState::S <= tempState && tempState < BlockState::Q) {
        memcpy(tempForm, this->minoForm_All[tempState], sizeof(tempForm));
    }
    else {
        abort(); //WTF????????
    }

    Block::FormToOffset(tempForm, tempOffset);
    Block::GetCheckList(changes, _checkList);
    if (SpinCollisionCheck(tempOffset, &tempPos, _checkList, gameMap)) {
        this->pos = tempPos;
        this->state = tempState;
        memcpy(this->minoOffset, tempOffset, sizeof(tempOffset));
    }
}
#pragma endregion
#pragma region ETC
void Block::HardDrop() {
    this->pos.Y = this->predictedPos.Y;
    this->state = BlockState::Droped;
}
void Block::Queue() {
    this->state = BlockState::Q;
}
#pragma endregion

