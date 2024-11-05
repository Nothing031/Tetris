// EUC-KR cp949 encoding
#define NOMINMAX
#include <Windows.h>
#include <conio.h>

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>


#include "Block.h"
#include "gameData.h"
#include "Tetris.h"

#define ARROW 224
#define ARROW_UP 72
#define ARROW_DOWN 80
#define ARROW_LEFT 75
#define ARROW_RIGHT 77
#define SPACE 32

#define DATANAME "data"

// init
KeyState Keyboard::ArrowLeft = KeyState::Released;
KeyState Keyboard::ArrowRight = KeyState::Released;
KeyState Keyboard::SpinLeft = KeyState::Released;
KeyState Keyboard::SpinRight = KeyState::Released;
KeyState Keyboard::SpinFlip = KeyState::Released;
KeyState Keyboard::SoftDrop = KeyState::Released;
KeyState Keyboard::HardDrop = KeyState::Released;
KeyState Keyboard::Hold = KeyState::Released;

int InputKeySetting::ArrowLeft = 0x25;
int InputKeySetting::ArrowRight = 0x27;
int InputKeySetting::SpinLeft = 0x5A;
int InputKeySetting::SpinRight = 0x58;
int InputKeySetting::SpinFilp = 0x41;
int InputKeySetting::Hold = 0x43;
int InputKeySetting::SoftDrop = 0x28;
int InputKeySetting::HardDrop = 0x20;

int Handling::ARR = 40;
int Handling::DAS = 160;
int Handling::SDRR = 40;


using namespace std;

HANDLE handle = NULL;
HWND hwnd = NULL;

enum ETitleSelectItem {
	Play,
	Settings
};
enum ESettingSelectItem {
	E_START = 0,
	DAS = E_START,
	ARR,
	SDRR,
	E_STARTBIND,
	SPIN_RIGHT = E_STARTBIND,
	SPIN_LEFT,
	FLIP,
	HARD_DROP,
	HOLD
};
int SettingSelectItemTable[8] = {
	3,
	6,
	9,
	14,
	15,
	16,
	17,
	18
};


void ReadConfig();
void WriteConfig();

void Init();
void SetCursorVisible(bool visible);



ETitleSelectItem TitleHome();
void SettingsHome();

void gotoxy(int x, int y);
int ReadKey();
void ClearKeyBuffer();
int GetKeybindInput();

int main()
{
	Init();

	ETitleSelectItem selectedItem;

	while (true) {
		selectedItem = TitleHome();

		if (selectedItem == ETitleSelectItem::Play) {
			system("cls");
			Tetris tetris(handle, hwnd);
			Keyboard::HardDrop = KeyState::Pressed;
			tetris.gameLoopInfinity();
		}
		if (selectedItem == ETitleSelectItem::Settings) {
			system("cls");
			SettingsHome();
		}
		system("cls");
	}
}

void Init() {
	setlocale(LC_ALL, "");
	hwnd = GetConsoleWindow();
	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTitle(TEXT("Tetris"));
	SetCursorVisible(false);
	ReadConfig();
}


ETitleSelectItem TitleHome()
{
	SetConsoleTextAttribute(handle, Color::White);
	std::wcout << "    " << L"■■■■" << L" ■■■■" << L" ■■■■" << L" ■■■■" << L" ■■■" << L" ■■■■" << endl;
	std::wcout << "    " << L"   ■   " << L" ■      " << L"    ■   " << L" ■    ■" << L"   ■  " << L" ■" << endl;
	std::wcout << "    " << L"   ■   " << L" ■■■■" << L"    ■   " << L" ■■■■" << L"   ■  " << L" ■■■■" << endl;
	std::wcout << "    " << L"   ■   " << L" ■      " << L"    ■   " << L" ■  ■  " << L"   ■  " << L"       ■" << endl;
	std::wcout << "    " << L"   ■   " << L" ■■■■" << L"    ■   " << L" ■    ■" << L" ■■■" << L" ■■■■" << endl;
	std::wcout << endl << endl;
	std::wcout << "                        " << "PLAY" << endl;
	std::wcout << "                        " << "SETTINGS" << endl;
	gotoxy(20, 7);
	wcout << MAP_BLOCK;

	int input;
	ETitleSelectItem item = ETitleSelectItem::Play;

	ClearKeyBuffer();
	while (true) {
		input = ReadKey();
		if (input == ARROW_UP) {
			item = ETitleSelectItem::Play;
			gotoxy(20, 8);
			cout << "  " << endl;
			gotoxy(20, 7);
			wcout << MAP_BLOCK;
		}
		else if (input == ARROW_DOWN) {
			item = ETitleSelectItem::Settings;
			gotoxy(20, 7);
			cout << "  " << endl;
			gotoxy(20, 8);
			wcout << MAP_BLOCK;
		}
		else if (input == SPACE) {
			return item;
		}
	}

}
void SettingsHome() {
	SetConsoleTextAttribute(handle, Color::White);
	cout << endl;
	cout << "      " << "esc : save and go back to title" << endl;
	cout << endl;
	cout << "      " << "DAS (delay auto shift)" << endl;
	cout << "          " << DasMax << " < " << Handling::DAS << "ms" << " > " << DasMin << endl;
	cout << endl;
	cout << "      " << "ARR (auto repeat rate)" << endl;
	cout << "          " << ArrMax << " < " << Handling::ARR << "ms" << " > " << ArrMin << endl;
	cout << endl;
	cout << "      " << "SDRR (soft drop repeat rate)" << endl;
	cout << "          " << SdrrMax << " < " << Handling::SDRR << "ms" << " > " << SdrrMin << endl;
	cout << endl;
	cout << "      " << "Key binds" << endl;
	cout << endl;
	cout << "      " << "Spin Right" << endl;
	cout << "      " << "Spin Left" << endl;
	cout << "      " << "Flip" << endl;
	cout << "      " << "Hard Drop" << endl;
	cout << "      " << "Hold" << endl;
	
	ESettingSelectItem selectedItem = DAS;
	gotoxy(4, 3);
	wcout << MAP_BLOCK;

	int input;
	while (true) {
		input = ReadKey();

		switch (input) {
		case VK_ESCAPE:
#pragma region CASE_ESCAPE
			WriteConfig();
			return;
#pragma endregion
		case ARROW_UP:
#pragma region CASE_ARROW_UP
			if (selectedItem != ESettingSelectItem::E_START) {
				gotoxy(4, SettingSelectItemTable[selectedItem]);
				cout << "  ";
				selectedItem = static_cast<ESettingSelectItem>(selectedItem - 1);
				gotoxy(4, SettingSelectItemTable[selectedItem]);
				wcout << MAP_BLOCK;
			}
			break; 
#pragma endregion
		case ARROW_DOWN:
#pragma region CASE_ARROW_DOWN
			if (selectedItem != ESettingSelectItem::HOLD) {
				gotoxy(4, SettingSelectItemTable[selectedItem]);
				cout << "  ";
				selectedItem = static_cast<ESettingSelectItem>(selectedItem + 1);
				gotoxy(4, SettingSelectItemTable[selectedItem]);
				wcout << MAP_BLOCK;
			}
			break;
#pragma endregion
		case ARROW_LEFT:
#pragma region CASE_ARROW_LEFT
			if (selectedItem < ESettingSelectItem::E_STARTBIND) {
				if (selectedItem == ESettingSelectItem::DAS) {
					if ((Handling::DAS + 10) <= DasMax) {
						Handling::DAS += 10;
						gotoxy(0, 4);
						cout << "\r          " << DasMax << " < " << Handling::DAS << "ms" << " > " << DasMin << "     ";
					}
				}
				else if (selectedItem == ESettingSelectItem::ARR) {
					if ((Handling::ARR + 10) <= ArrMax) {
						Handling::ARR += 10;
						gotoxy(0, 7);
						cout << "\r          " << ArrMax << " < " << Handling::ARR << "ms" << " > " << ArrMin << "     ";
					}
				}
				else if (selectedItem == ESettingSelectItem::SDRR) {
					if ((Handling::SDRR + 10) <= SdrrMax) {
						Handling::SDRR += 10;
						gotoxy(0, 10);
						cout << "\r          " << SdrrMax << " < " << Handling::SDRR << "ms" << " > " << SdrrMin << "     ";
					}
				}
			}
			break;
#pragma endregion
		case ARROW_RIGHT:
#pragma region CASE_ARROW_RIGHT
			if (selectedItem < ESettingSelectItem::E_STARTBIND) {
				if (selectedItem == ESettingSelectItem::DAS) {
					if ((Handling::DAS - 10) >= DasMin) {
						Handling::DAS -= 10;
						gotoxy(0, 4);
						cout << "\r          " << DasMax << " < " << Handling::DAS << "ms" << " > " << DasMin << "     ";
					}
				}
				else if (selectedItem == ESettingSelectItem::ARR) {
					if ((Handling::ARR - 10) >= ArrMin) {
						Handling::ARR -= 10;
						gotoxy(0, 7);
						cout << "\r          " << ArrMax << " < " << Handling::ARR << "ms" << " > " << ArrMin << "     ";
					}
				}
				else if (selectedItem == ESettingSelectItem::SDRR) {
					if ((Handling::SDRR - 10) >= SdrrMin) {
						Handling::SDRR -= 10;
						gotoxy(0, 10);
						cout << "\r          " << SdrrMax << " < " << Handling::SDRR << "ms" << " > " << SdrrMin << "     ";
					}
				}
			}
			break;
#pragma endregion
		} // end switch

		if (selectedItem >= ESettingSelectItem::E_STARTBIND) {
			gotoxy(4, 20);
			cout << "enter to change key bind";
		}
		else {
			gotoxy(4, 20);
			cout << "                                   ";
		}

		if (input == VK_RETURN) {
			if (selectedItem >= ESettingSelectItem::E_STARTBIND) {
				int bindInput;
				gotoxy(4, 20);
				cout << "                                   ";
				cout << "\r    press key to bind            ";
				bindInput = GetKeybindInput();

				switch (selectedItem) {
				case ESettingSelectItem::SPIN_LEFT:
					InputKeySetting::SpinLeft = bindInput;
					break;
				case ESettingSelectItem::SPIN_RIGHT:
					InputKeySetting::SpinRight = bindInput;
					break;
				case ESettingSelectItem::FLIP:
					InputKeySetting::SpinFilp = bindInput;
					break;
				case ESettingSelectItem::HARD_DROP:
					InputKeySetting::HardDrop = bindInput;
					break;
				case ESettingSelectItem::HOLD:
					InputKeySetting::Hold = bindInput;
					break;
				} // end switch
				cout << "\r    key bind set                  ";
			}
		}
	} // end while
}


void ReadConfig() {
	if (filesystem::exists(".\\data")) {
		try {
			// read
			ifstream file(".\\data");
			file.read(reinterpret_cast<char*>(&Handling::DAS), sizeof(int));
			file.read(reinterpret_cast<char*>(&Handling::ARR), sizeof(int));
			file.read(reinterpret_cast<char*>(&Handling::SDRR), sizeof(int));
			file.read(reinterpret_cast<char*>(&InputKeySetting::SpinLeft), sizeof(int));
			file.read(reinterpret_cast<char*>(&InputKeySetting::SpinRight), sizeof(int));
			file.read(reinterpret_cast<char*>(&InputKeySetting::SpinFilp), sizeof(int));
			file.read(reinterpret_cast<char*>(&InputKeySetting::HardDrop), sizeof(int));
			file.read(reinterpret_cast<char*>(&InputKeySetting::Hold), sizeof(int));
			file.close();
		}
		catch (exception e){
			WriteConfig();
		}
	}
	else {
		// not exists
		WriteConfig();
	}
}
void WriteConfig() {
	ofstream file(".\\data", ios::binary | ios::trunc);
	file.write(reinterpret_cast<const char*>(&Handling::DAS), sizeof(int));
	file.write(reinterpret_cast<const char*>(&Handling::ARR), sizeof(int));
	file.write(reinterpret_cast<const char*>(&Handling::SDRR), sizeof(int));
	file.write(reinterpret_cast<const char*>(&InputKeySetting::SpinLeft), sizeof(int));
	file.write(reinterpret_cast<const char*>(&InputKeySetting::SpinRight), sizeof(int));
	file.write(reinterpret_cast<const char*>(&InputKeySetting::SpinFilp), sizeof(int));
	file.write(reinterpret_cast<const char*>(&InputKeySetting::HardDrop), sizeof(int));
	file.write(reinterpret_cast<const char*>(&InputKeySetting::Hold), sizeof(int));
	file.close();
}

void SetCursorVisible(bool visible) {
	CONSOLE_CURSOR_INFO     cursorInfo;
	GetConsoleCursorInfo(handle, &cursorInfo);
	cursorInfo.bVisible = visible;
	SetConsoleCursorInfo(handle, &cursorInfo);
}
int ReadKey() {
	int input;
	input = _getch();
	if (input == ARROW) {
		input = _getch();
	}
	return input;
}
void ClearKeyBuffer() {
	while (_kbhit()) {
		_getch();
	}
}

void gotoxy(int x, int y) {
	COORD pos = { (short)x, (short)y };
	SetConsoleCursorPosition(handle, pos);
}
int GetKeybindInput() {
	int input;
	ClearKeyBuffer();
	while (true) {

		if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) {
			input = VK_LSHIFT;
			break;
		}
		else if (GetAsyncKeyState(VK_RSHIFT) & 0x8000) {
			input = VK_RSHIFT;
			break;
		}
		else if (GetAsyncKeyState(VK_CAPITAL) & 0x8000) {
			input = VK_CAPITAL;
			break;
		}
		else if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
			input = VK_CONTROL;
			break;
		}
		else {
			if (_kbhit()) {
				input = VkKeyScan(_getch()) & 0xFF;
				break;
			}
		}
		Sleep(10);
	}
	ClearKeyBuffer();
	return input;
}





