#pragma once


#define MAP_BLOCK "■"
#define MAP_VOID "□"
#define MAP_PREDICTED "▧"

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
    Running = 6
};

enum KeyState : int {
    Released = 0,
    Pressing = 1,
    Pressed = 2
};

struct KeyboardState {
    static KeyState SpinLeft;
    static KeyState SpinRight;
    static KeyState SpinFlip;
    static KeyState Hold;
    static KeyState HardDrop;
    static KeyState ArrowLeft;
    static KeyState ArrowRight;
    static KeyState SoftDrop;
};

enum MinoType : int {
    Mino_NULL = Color::Black,
    Mino_I = Color::Cyan,
    Mino_O = Color::Yellow,
    Mino_Z = Color::Red,
    Mino_S = Color::Green,
    Mino_J = Color::DarkBlue,
    Mino_L = Color::DarkYellow,
    Mino_T = Color::Magenta,
};
static int minoList[7] = { Mino_I, Mino_O, Mino_Z, Mino_S, Mino_J, Mino_L, Mino_T };

struct {
    const int MAP_X = 6;
    const int MAP_Y = 0;
    const int MINO_SPAWN_X = 3;
    const int MINO_SPAWN_Y = 0;
}offsets;

enum InputKey : int {
    k_Null,
    A_Left,
    A_Right,
    SoftDrop,
    HardDrop,
    SpinLeft,
    SpinRight,
    SpinFilp,
    Hold,
};

struct Handling {
    static int ARR;
    static int DAS;
    static int DCS;
    static int SDF;
};

struct InputKeyNums {
    static int A_Left;
    static int A_Right;
    static int SoftDrop;
    static int HardDrop;
    static int SpinLeft;
    static int SpinRight;
    static int SpinFilp;
    static int Hold;
};

<<<<<<< HEAD
struct CheckList_SRS {
    struct JLSTZ { // [CheckList][XY];
        static const int S_to_R[5][2];
        static const int R_to_S[5][2];
        static const int R_to_F[5][2];
        static const int F_to_R[5][2];
        static const int F_to_L[5][2];
        static const int L_to_F[5][2];
        static const int L_to_S[5][2];
        static const int S_to_L[5][2];
    };
    static struct I {
        static const int S_to_R[5][2];
        static const int R_to_S[5][2];
        static const int R_to_F[5][2];
        static const int F_to_R[5][2];
        static const int F_to_L[5][2];
        static const int L_to_F[5][2];
        static const int L_to_S[5][2];
        static const int S_to_L[5][2];
    };
};

struct MinoForms// [state][y][x] 
{
    static const int Mino_O[4][4][4];
    static const int Mino_I[4][4][4];
    static const int Mino_T[4][4][4];
    static const int Mino_L[4][4][4];
    static const int Mino_J[4][4][4];
    static const int Mino_Z[4][4][4];
    static const int Mino_S[4][4][4];
};

// define checklist
#pragma region CheckList
const int CheckList_SRS::JLSTZ::S_to_R[5][2] = {
    {0, 0},
    {-1, 0},
    {-1, +1},
    {0, -2},
    {-1, -2}
};
const int CheckList_SRS::JLSTZ::R_to_S[5][2] = {
    {0, 0},
    {+1, 0},
    {+1, -1},
    {0, +2},
    {+1, +2}
};
const int CheckList_SRS::JLSTZ::R_to_F[5][2] = {
    {0, 0},
    {+1, 0},
    {+1, -1},
    {0, +2},
    {+1, +2}
};
const int CheckList_SRS::JLSTZ::F_to_R[5][2] = {
    {0, 0},
    {-1, 0},
    {-1, +1},
    {0, -2},
    {-1, -2}
};
const int CheckList_SRS::JLSTZ::F_to_L[5][2] = {
    {0, 0},
    {+1, 0},
    {+1, +1},
    {0, -2},
    {+1, -2}
};
const int CheckList_SRS::JLSTZ::L_to_F[5][2] = {
    {0, 0},
    {-1, 0},
    {-1, -1},
    {0, +2},
    {-1, +2}
};
const int CheckList_SRS::JLSTZ::L_to_S[5][2] = {
    {0, 0},
    {-1, 0},
    {-1, -1},
    {0, +2},
    {-1, +2}
};
const int CheckList_SRS::JLSTZ::S_to_L[5][2] = {
    {0, 0},
    {+1, 0},
    {+1, +1},
    {0, -2},
    {+1, -2}
};
const int CheckList_SRS::I::S_to_R[5][2] = {
            {0, 0},
            {-2, 0},
            {+1, 0},
            {-2, -1},
            {+1, +2}
};
const int CheckList_SRS::I::R_to_S[5][2] = {
    {0, 0},
    {+2, 0},
    {-1, 0},
    {+2, +1},
    {-1, -2}
};
const int CheckList_SRS::I::R_to_F[5][2] = {
    {0, 0},
    {-1, 0},
    {+2, 0},
    {-1, +2},
    {+2, -1}
};
const int CheckList_SRS::I::F_to_R[5][2] = {
    {0, 0},
    {+1, 0},
    {-2, 0},
    {+1, -2},
    {-2, +1}
};
const int CheckList_SRS::I::F_to_L[5][2] = {
    {0, 0},
    {+2, 0},
    {-1, 0},
    {+2, +1},
    {-1, -2}
};
const int CheckList_SRS::I::L_to_F[5][2] = {
    {0, 0},
    {-2, 0},
    {+1, 0},
    {-2, -1},
    {+1, +2}
};
const int CheckList_SRS::I::L_to_S[5][2] = {
    {0, 0},
    {+1, 0},
    {-2, 0},
    {+1, -2},
    {-2, +1}
};
const int CheckList_SRS::I::S_to_L[5][2] = {
    {0, 0},
    {-1, 0},
    {+2, 0},
    {-1, +2},
    {+2, -1}
};
#pragma endregion
// define mino form
#pragma region Forms
const int MinoForms::Mino_O[4][4][4] = {
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
const int MinoForms::Mino_I[4][4][4] = {
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
const int MinoForms::Mino_T[4][4][4] = {
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
const int MinoForms::Mino_L[4][4][4] = {
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
const int MinoForms::Mino_J[4][4][4] = {
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
const int MinoForms::Mino_Z[4][4][4] = {
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
const int MinoForms::Mino_S[4][4][4] = {
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
#pragma endregion
=======
>>>>>>> 8c32f6d18a61d882ae7c3c943cb3900fe78635c1
