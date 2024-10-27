#pragma once
#define NOMINMAX
#include <Windows.h>
#include <algorithm>
#include <conio.h>
#include <iostream>
#include <future>
#include "gameData.h"
#include "Tick.h"
#include "Block.h"


class Block
{
public:
    BlockState state;
    MinoType type;
    Color minoColor;
    COORD pos;
    COORD predictedPos;
    
    int minoOffset[4][2]; // [count][x, y]
    int minoForm_All[4][4][4]; //[state][y][x]

    Block() {
        Init(MinoType::Mino_O);
    }
    Block(const Block& other) {
        Init(other.type);
    }
    Block(const MinoType type) {
        Init(type);
    }
    ~Block(){
    }
    Block& operator=(const Block& other) {
        if (this != &other) {
            this->state = other.state;
            this->type = other.type;
            this->minoColor = other.minoColor;
            this->pos = other.pos;
            this->predictedPos = other.predictedPos;
            memcpy(this->minoOffset, other.minoOffset, sizeof(minoOffset));
            memcpy(this->minoForm_All, other.minoForm_All, sizeof(minoForm_All));
        }
        return *this;
    }

    void Init(MinoType initType);

};


