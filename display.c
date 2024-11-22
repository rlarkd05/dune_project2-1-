/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

// 출력할 내용들 좌표
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);


void display(
    RESOURCE resource,
    char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
    CURSOR cursor)
{
    display_resource(resource);
    display_map(map);
    display_cursor(cursor);
}

void display_resource(RESOURCE resource) {
    set_color(COLOR_RESOURCE);
    gotoxy(resource_pos);
    printf("spice = %d/%d, population=%d/%d  \n",
        resource.spice, resource.spice_max,
        resource.population, resource.population_max
    );
}

//맵 함수
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            dest[i][j] = ' '; // 초기값 설정

            // Layer 1(오브젝트 레이어)을 먼저 체크
            // -1이 아닌 모든 값을 체크하도록 수정
            if (src[1][i][j] >= 0) {
                dest[i][j] = src[1][i][j];
                continue;  // Layer 1에 오브젝트가 있으면 Layer 0은 확인하지 않음
            }

            // Layer 0 체크
            if (src[0][i][j] != 0) {
                dest[i][j] = src[0][i][j];
            }
        }
    }
}

//색 함수
int get_color_at(POSITION pos) {
    char ch = backbuf[pos.row][pos.column];

    // 문자에 따른 색상 반환
    switch (ch) {
    case 'B':
        // 아군 본진 (14,1)~(15,2)
        if (pos.row >= 15 && pos.row <= 16 && pos.column >= 1 && pos.column <= 2) {
            return COLOR_BLUE;
        }
        // 적군 본진 (1,57)~(2,58)
        else if (pos.row >= 1 && pos.row <= 2 && pos.column >= 57 && pos.column <= 58) {
            return COLOR_RED;
        }
        // 그 외 병영
        else {
            return COLOR_GREEN;  // 또는 다른 색상
        }
    case 'H':
        return (pos.row >= 14 && pos.row <= 15 && pos.column >= 1 && pos.column <= 4) ? COLOR_BLUE : COLOR_RED;
    case '5':
        return COLOR_ORANGE;
    case 'P':
        return  FLOOR_BEIGE;
    case 'R':
        return COLOR_GRAY;
    case '#':
        return COLOR_BLACK;
    case 'W':
        return COLOR_YELLOW;
    case 'D':  // 숙소
        return COLOR_CYAN;
    case 'G':  // 창고
        return COLOR_MAGENTA;
    case 'S':  // 은신처
        return COLOR_WHITE;
    default:
        return COLOR_DEFAULT;
    }
}

void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    project(map, backbuf);

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            char current_char = backbuf[i][j];
            int color = get_color_at((POSITION) { i, j });

            // 이전 상태와 다르거나 오브젝트('W')인 경우 항상 출력
            if (frontbuf[i][j] != current_char || current_char == 'W') {
                POSITION pos = padd(map_pos, (POSITION) { i, j });
                printc(pos, current_char, color);
                frontbuf[i][j] = current_char;
            }
        }
    }
}
void display_cursor(CURSOR cursor) {
    // 이전 커서 위치 복원
    if (building_state.cursor_size == 2) {
        // 2x2 크기의 이전 커서 위치 복원
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                POSITION prev = { cursor.previous.row + i, cursor.previous.column + j };
                char prev_char = backbuf[prev.row][prev.column];
                int prev_color = get_color_at(prev);
                printc(padd(map_pos, prev), prev_char, prev_color);
                frontbuf[prev.row][prev.column] = prev_char;
            }
        }
    }
    else {
        // 1x1 크기의 이전 커서 위치 복원
        POSITION prev = cursor.previous;
        char prev_char = backbuf[prev.row][prev.column];
        int prev_color = get_color_at(prev);
        printc(padd(map_pos, prev), prev_char, prev_color);
        frontbuf[prev.row][prev.column] = prev_char;
    }

    // 현재 커서 위치 표시
    if (building_state.cursor_size == 2) {
        // 2x2 크기의 현재 커서 표시
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                POSITION curr = { cursor.current.row + i, cursor.current.column + j };
                char curr_char = backbuf[curr.row][curr.column];
                printc(padd(map_pos, curr), curr_char, COLOR_CURSOR);
                frontbuf[curr.row][curr.column] = COLOR_CURSOR;
            }
        }
    }
    else {
        // 1x1 크기의 현재 커서 표시
        POSITION curr = cursor.current;
        char curr_char = backbuf[curr.row][curr.column];
        printc(padd(map_pos, curr), curr_char, COLOR_CURSOR);
        frontbuf[curr.row][curr.column] = COLOR_CURSOR;
    }
}
