/*
* raw(?) I/O
*/
#include "io.h"

void gotoxy(POSITION pos) {
    COORD coord = { pos.column, pos.row }; // 행, 열 반대로 전달
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

//색 입히는 함수
void set_color(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printc(POSITION pos, char ch, int color) {
    if (color >= 0) {
        set_color(color);
    }
    gotoxy(pos);
    printf("%c", ch);
}

//키를 눌렀을때
KEY get_key(void) {
    if (!_kbhit()) {  // 입력된 키가 있는지 확인
        return k_none;
    }
    int byte = _getch();    // 입력된 키를 전달 받기
    switch (byte) {
    case 'q': return k_quit;  // 'q'를 누르면 종료
    case 32: return k_space;  // 스페이스바 키 추가
    case 27: return k_esc;  //esc키 추가
    case 'p': return k_p;
    case 'd': return k_d;
    case 'g': return k_g;
    case 's': return k_s;
    case 'b': return k_b;
    case 'l': return k_l;
    case 'f': return k_f;
    case 't': return k_t;
    case 'm': return k_m;
    case 'v': return k_v;
    case 72:
    case 104: return k_h;
    case 224:
        byte = _getch();
        switch (byte) {
        case 72: return k_up;
        case 75: return k_left;
        case 77: return k_right;
        case 80: return k_down;
        default: return k_undef;
        }
    default: return k_undef;
    }
}