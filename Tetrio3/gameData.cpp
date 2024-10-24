#include "gameData.h";


KeyState KeyboardState::SpinRight = KeyState::Released;
KeyState KeyboardState::SpinLeft = KeyState::Released;
KeyState KeyboardState::SpinFlip = KeyState::Released;
KeyState KeyboardState::ArrowLeft = KeyState::Released;
KeyState KeyboardState::ArrowRight = KeyState::Released;
KeyState KeyboardState::SoftDrop = KeyState::Released;
KeyState KeyboardState::Hold = KeyState::Released;
KeyState KeyboardState::HardDrop = KeyState::Released;

// default key
int InputKeyNums::A_Left = 0x25; //
int InputKeyNums::A_Right = 0x27;
int InputKeyNums::SoftDrop = 0x28;
int InputKeyNums::SpinRight = 0x58;
int InputKeyNums::SpinLeft = 0x5A;
int InputKeyNums::SpinFilp = 0x41;
int InputKeyNums::Hold = 0x43;
int InputKeyNums::HardDrop = 0x20;

int Handling::ARR = 10;
int Handling::DAS = 1;
int Handling::SDF = 10;

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
