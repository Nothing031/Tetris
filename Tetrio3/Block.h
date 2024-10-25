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

    bool TrySpawn(int gameMap[24][10]);

    BlockState Process(int gameMap[24][10], InputKey input);

    void ForceDrop() {
        this->state = BlockState::Droped;
    }

    void CalculatePredictedPos(int gameMap[24][10]);
    void TryGravityDrop(int gameMap[24][10]);
    
    
private:
    // move spin or something
    void MoveLeft(int gameMap[24][10]);
    void MoveRight(int gameMap[24][10]);
    void SoftDrop(int gameMap[24][10]);
    void SpinLeft(int gameMap[24][10]);
    void SpinRight(int gameMap[24][10]);
    void Flip(int gameMap[24][10]);
    void HardDrop();
    void Queue();

    bool SpinCollisionCheck(int _tempOffset[4][2], COORD* _derefTempPos, int _checkList[5][2], int _gameMap[24][10]);
    bool CollisionCheck(int tempOffset[4][2], COORD InOutTempPos, int gameMap[24][10]);

    StateChanges GetStateChanges(BlockState currentState, BlockState newState);
    static void GetCheckList(StateChanges changes, int _derefCheckList[5][2]);
    static void FormToOffset(int minoForm[4][4], int outMinoOffset[4][2]);
 
    BlockState MoveSpinBlock(int gameMap[24][10], InputKey input);

    void DeleteFromMap(int gameMap[24][10]);
    void DrawOnMap(int gameMap[24][10]);

};


