// EUC-KR cp949 encoding
#pragma once

constexpr wchar_t MAP_BLOCK = L'■';
constexpr wchar_t MAP_VOID = L'□';
constexpr wchar_t MAP_GHOST = L'▧';

#define MAP_WIDTH 10
#define MAP_HEIGHT 24

enum Color : int {
    Black,
    DarkBlue,
    DarkGreen,
    DarkCyan,
    DarkRed,
    DarkMagenta,
    DarkYellow,
    Gray,
    DarkGray,
    Blue,
    Green,
    Cyan,
    Red,
    Magenta,
    Yellow,
    White
};

enum BlockState : int {
    S = 0, //spawn state
    R = 1, //right spin from first state
    F = 2, //fliped from first state
    L = 3, //left spin from first state
    Q = 4, //block is in queue
    Droped = 5,  //block has been stucked in bottom
    Hold = 6
};

static constexpr int MinoColor[8] = {
    Color::Cyan, // I
    Color::Yellow, // O
    Color::DarkBlue, // J
    Color::DarkYellow, // L
    Color::Green, // S
    Color::Magenta, // T
    Color::Red, // Z
    Color::Black // NULL
};

struct CpySize {
    static constexpr size_t offset = sizeof(int) * 4 * 2;
    static constexpr size_t form = sizeof(int) * 4 * 4;
    static constexpr size_t map = sizeof(int) * MAP_WIDTH     * MAP_HEIGHT;
    static constexpr size_t checkList = sizeof(int) * 5 * 2;
};

struct Offsets{
    static const int MAP_X = 6;
    static const int MAP_Y = 0;
    static const int MINO_SPAWN_X = 3;
    static const int MINO_SPAWN_Y = 0;
};


// key

enum KeyState : int {
    Released = 0,
    Pressing = 1,
    Pressed = 2
};
enum InputKey : int {
    K_Null,
    K_ArrowLeft,
    K_ArrowRight,
    K_SpinLeft,
    K_SpinRight,
    K_SpinFilp,
    K_SoftDrop,
    K_HardDrop,
    K_Hold
};
struct Keyboard {
    static KeyState ArrowLeft;
    static KeyState ArrowRight;
    static KeyState SpinLeft;
    static KeyState SpinRight;
    static KeyState SpinFlip;
    static KeyState SoftDrop;
    static KeyState HardDrop;
    static KeyState Hold;
};


struct InputKeySetting {
    static int ArrowLeft;
    static int ArrowRight;
    static int SpinLeft;
    static int SpinRight;
    static int SpinFilp;
    static int SoftDrop;
    static int HardDrop;
    static int Hold;
};



struct Handling {
    static int ARR;
    static int DAS;
    static int SDRR;
};





enum EStateChanges : int {
    S_R,
    R_S,
    R_F,
    F_R,
    F_L,
    L_F,
    L_S,
    S_L,
    IS_R,
    IR_S,
    IR_F,
    IF_R,
    IF_L,
    IL_F,
    IL_S,
    IS_L,
    O
};

// I, O, J, L, S, T, Z
static constexpr int CheckList_SRS[17][5][2] = {
    {
        {0, 0},
        {-1, 0},
        {-1, +1},
        {0, -2},
        {-1, -2}
    },
    {
        {0, 0},
        {+1, 0},
        {+1, -1},
        {0, +2},
        {+1, +2}
    },
    {
        {0, 0},
        {+1, 0},
        {+1, -1},
        {0, +2},
        {+1, +2}
    },
    {
        {0, 0},
        {-1, 0},
        {-1, +1},
        {0, -2},
        {-1, -2}
    },
    {
        {0, 0},
        {+1, 0},
        {+1, +1},
        {0, -2},
        {+1, -2}
    },
    {
        {0, 0},
        {-1, 0},
        {-1, -1},
        {0, +2},
        {-1, +2}
    },
    {
        {0, 0},
        {-1, 0},
        {-1, -1},
        {0, +2},
        {-1, +2}
    },
    {
        {0, 0},
        {+1, 0},
        {+1, +1},
        {0, -2},
        {+1, -2}
    },
    {
        {0, 0},
        {-2, 0},
        {+1, 0},
        {-2, -1},
        {+1, +2}
    },
    {
        {0, 0},
        {+2, 0},
        {-1, 0},
        {+2, +1},
        {-1, -2}
    },
    {
        {0, 0},
        {-1, 0},
        {+2, 0},
        {-1, +2},
        {+2, -1}
    },
    {
        {0, 0},
        {+1, 0},
        {-2, 0},
        {+1, -2},
        {-2, +1}
    },
    {
        {0, 0},
        {+2, 0},
        {-1, 0},
        {+2, +1},
        {-1, -2}
    },
    {
        {0, 0},
        {-2, 0},
        {+1, 0},
        {-2, -1},
        {+1, +2}
    },
    {
        {0, 0},
        {+1, 0},
        {-2, 0},
        {+1, -2},
        {-2, +1}
    },
    {
        {0, 0},
        {-1, 0},
        {+2, 0},
        {-1, +2},
        {+2, -1}
    },
    {
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0}
    }
};

enum EMino : int {
    I_MINO = 0,
    O_MINO = 1,
    J_MINO = 2,
    L_MINO = 3,
    S_MINO = 4,
    T_MINO = 5,
    Z_MINO = 6,
    NULL_MINO = 7
};

// [EMino][BlockState][y][x]
static constexpr int MinoForms[7][4][4][4] = {
    {// I
        {
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 1, 0}
        },
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0}
        }
    }, // I
    { // O
        {
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        }

    }, // O
    { // J
        {
            {1, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        }
    }, // J
    { // L
        {
            {0, 0, 1, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {1, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        }
    }, // L
    { // S
        {
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {1, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        }
    }, // S
    { // T

        {
            {0, 1, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        }
    }, // T
    { // Z
        {
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 1, 0},
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {1, 0, 0, 0},
            {0, 0, 0, 0}
        }
    }
};