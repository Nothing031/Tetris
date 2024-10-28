// EUC-KR cp949 encoding
#pragma once
//#define MAP_BLOCK "■"
//#define MAP_VOID "□"
//#define MAP_PREDICTED "▧"

constexpr wchar_t MAP_BLOCK = L'■';
constexpr wchar_t MAP_VOID = L'□';
constexpr wchar_t MAP_PREDICTED = L'▧';


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
enum StateChanges : int {
    S_to_R,
    R_to_S,
    R_to_F,
    F_to_R,
    F_to_L,
    L_to_F,
    L_to_S,
    S_to_L,

    I_S_to_R,
    I_R_to_S,
    I_R_to_F,
    I_F_to_R,
    I_F_to_L,
    I_L_to_F,
    I_L_to_S,
    I_S_to_L,

    Default
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

enum MinoType : int {
    Mino_NULL = Color::Black,
    Mino_I = Color::Cyan,
    Mino_O = Color::Yellow,
    Mino_Z = Color::Red,
    Mino_S = Color::Green,
    Mino_J = Color::DarkBlue,
    Mino_L = Color::DarkYellow,
    Mino_T = Color::Magenta
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
struct KeyboardState {
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


struct CheckList_SRS {
    struct JLSTZ { // [CheckList][XY];
        static constexpr int S_to_R[5][2] = {
            {0, 0},
            {-1, 0},
            {-1, +1},
            {0, -2},
            {-1, -2}
        };
        static constexpr int R_to_S[5][2] = {
            {0, 0},
            {+1, 0},
            {+1, -1},
            {0, +2},
            {+1, +2}
        };
        static constexpr int R_to_F[5][2] = {
            {0, 0},
            {+1, 0},
            {+1, -1},
            {0, +2},
            {+1, +2}
        };
        static constexpr int F_to_R[5][2] = {
            {0, 0},
            {-1, 0},
            {-1, +1},
            {0, -2},
            {-1, -2}
        };
        static constexpr int F_to_L[5][2] = {
            {0, 0},
            {+1, 0},
            {+1, +1},
            {0, -2},
            {+1, -2}
        };
        static constexpr int L_to_F[5][2] = {
            {0, 0},
            {-1, 0},
            {-1, -1},
            {0, +2},
            {-1, +2}
        };
        static constexpr int L_to_S[5][2] = {
            {0, 0},
            {-1, 0},
            {-1, -1},
            {0, +2},
            {-1, +2}
        };
        static constexpr int S_to_L[5][2] = {
            {0, 0},
            {+1, 0},
            {+1, +1},
            {0, -2},
            {+1, -2}
        };
    };
    struct I {
        static constexpr int S_to_R[5][2] = {
            {0, 0},
            {-2, 0},
            {+1, 0},
            {-2, -1},
            {+1, +2}
        };
        static constexpr int R_to_S[5][2] = {
            {0, 0},
            {+2, 0},
            {-1, 0},
            {+2, +1},
            {-1, -2}
        };
        static constexpr int R_to_F[5][2] = {
            {0, 0},
            {-1, 0},
            {+2, 0},
            {-1, +2},
            {+2, -1}
        };
        static constexpr int F_to_R[5][2] = {
            {0, 0},
            {+1, 0},
            {-2, 0},
            {+1, -2},
            {-2, +1}
        };
        static constexpr int F_to_L[5][2] = {
            {0, 0},
            {+2, 0},
            {-1, 0},
            {+2, +1},
            {-1, -2}
        };
        static constexpr int L_to_F[5][2] = {
            {0, 0},
            {-2, 0},
            {+1, 0},
            {-2, -1},
            {+1, +2}
        };
        static constexpr int L_to_S[5][2] = {
            {0, 0},
            {+1, 0},
            {-2, 0},
            {+1, -2},
            {-2, +1}
        };
        static constexpr int S_to_L[5][2] = {
            {0, 0},
            {-1, 0},
            {+2, 0},
            {-1, +2},
            {+2, -1}
        };
    };
};
struct MinoForms// [state][y][x] 
{
    static constexpr int Mino_O[4][4][4] = {
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
    };
    static constexpr int Mino_I[4][4][4] = {
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
    };
    static constexpr int Mino_T[4][4][4] = {
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
    };
    static constexpr int Mino_L[4][4][4] = {
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
    };
    static constexpr int Mino_J[4][4][4] = {
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
    };
    static constexpr int Mino_Z[4][4][4] = {
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
    };
    static constexpr int Mino_S[4][4][4] = {
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
    };
};

static int minoList[7] = { Mino_I, Mino_O, Mino_Z, Mino_S, Mino_J, Mino_L, Mino_T };