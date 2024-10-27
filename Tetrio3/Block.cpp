
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
    int count = 0;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (this->minoForm_All[BlockState::S][y][x] != 0) {
                this->minoOffset[count][0] = x;
                this->minoOffset[count][1] = y;
                count++;
            }
        }
    }
}







