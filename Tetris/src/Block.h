#pragma once
#define NOMINMAX
#include <Windows.h>

#include <algorithm>
#include <iostream>
#include <future>

#include "gameData.h"
#include "Block.h"


class Block
{
private:
    void Init(EMino initType) {
        this->minoType = initType;
        this->minoColor = static_cast<Color>(MinoColor[minoType]);
        this->pos = { 0, 0 };
        this->prevPos = { 0, 0 };
        this->ghostPos = { 0, 0 };
        this->prevGhostPos = { 0, 0 };
        this->state = BlockState::Q;

        if (initType == EMino::NULL_MINO) {
            for (int i = 0; i < 4; i++) {
                this->minoOffset[i][0] = 0;
                this->minoOffset[i][1] = 0;
            }
            memcpy(this->prevMinoOffset, this->minoOffset, CpySize::offset);
        }
        else {
            int count = 0;
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                    if (MinoForms[this->minoType][BlockState::S][y][x] != 0) {
                        this->minoOffset[count][0] = x;
                        this->minoOffset[count][1] = y;
                        count++;
                    }
                }
            }
            memcpy(this->prevMinoOffset, this->minoOffset, CpySize::offset);
        }
    }
public:
    BlockState state;
    EMino minoType;
    Color minoColor;
    COORD pos;
    COORD ghostPos;

    int minoOffset[4][2]; // [count][x, y]
    int prevMinoOffset[4][2];

    COORD prevPos;
    COORD prevGhostPos;

    

    Block() {
        Init(EMino::O_MINO);
    }
    Block(const Block& other) {
        Init(other.minoType);
    }
    Block(const EMino minoType) {
        Init(minoType);
    }
    ~Block(){
    }
    Block& operator=(const Block& other) {
        if (this != &other) {
            this->state = other.state;
            this->minoType = other.minoType;
            this->minoColor = other.minoColor;
            this->pos = other.pos;
            this->prevPos = other.prevPos;
            this->ghostPos = other.ghostPos;
            this->prevGhostPos = other.prevGhostPos;
            memcpy(this->minoOffset, other.minoOffset, CpySize::offset);
            memcpy(this->prevMinoOffset, other.prevMinoOffset, CpySize::offset);
        }
        return *this;
    }
};


