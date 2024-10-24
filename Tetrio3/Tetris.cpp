#include "Tetris.h"

void Tetris::Init(HANDLE _handle, HWND _hwnd)
{
	this->gen = mt19937(rd());
	this->handle = _handle;
	this->hwnd = _hwnd;

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			gameMap[i][j] = Color::Black;
		}
	}
}



void Tetris::MoveLeft(int gameMap[24][10]) {

}
void Tetris::MoveRight(int gameMap[24][10]) {

}
void Tetris::SoftDrop(int gameMap[24][10]) {

}



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
    if (_maxX > 9 || _minX < 0 || _maxY > 24 || _minY < 0)
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
        tempPos.Y -= checkList[test][0]; // game +Y = down, -Y = up ; checkList +Y = up, -Y down;
        if (Tetris::CollisionCheck(tempOffset, tempPos)) {
            passedTest = test;
            *derefTempPos = tempPos;
            return true;
        }
    }
    return false;
}

void Tetris::GetCheckList(StateChanges changes, int _derefCheckList[5][2])
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
    default:
        for (int i = 0; i < 5; i++) {
            cpySizeArr[i][0] = 0;
            cpySizeArr[i][1] = 0;
        }
        memcpy(_derefCheckList, cpySizeArr, cpySize);
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
        changes = (StateChanges)((int)changes + 8); // I mino
    else if (this->CurrentBlock.type == MinoType::Mino_O)
        changes = StateChanges::Default;
    return changes;
}