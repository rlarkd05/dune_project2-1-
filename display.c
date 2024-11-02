/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

// 출력할 내용들의 좌상단(topleft) 좌표
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
    // display_system_message()
    // display_object_info()
    // display_commands()
    // ...
}

void display_resource(RESOURCE resource) {
    set_color(COLOR_RESOURCE);
    gotoxy(resource_pos);
    printf("spice = %d/%d, population=%d/%d\n",
        resource.spice, resource.spice_max,
        resource.population, resource.population_max
    );
}

// subfunction of draw_map()
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

int get_color_at(POSITION pos) {
    char ch = backbuf[pos.row][pos.column];

    // 문자에 따른 색상 반환
    switch (ch) {
    case 'B':
        return (pos.row >= 15 && pos.column <= 2) ? COLOR_BLUE : COLOR_RED;
    case '5':
        return COLOR_ORANGE;
    case 'P':
        return COLOR_BLACK;
    case 'R':
        return COLOR_GRAY;
    case '#':
        return COLOR_BLACK;
    case 'W':
        return COLOR_YELLOW;
    default:
        return COLOR_DEFAULT; // 기본 색상
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
    POSITION prev = cursor.previous;
    POSITION curr = cursor.current;

    // 이전 위치의 문자를 backbuf에서 가져옵니다.
    char prev_char = backbuf[prev.row][prev.column];
    int prev_color = get_color_at(prev); // 이전 위치의 색상

    // 현재 위치의 문자를 backbuf에서 가져옵니다.
    char curr_char = backbuf[curr.row][curr.column];

    // 이전 위치를 원래 문자와 색상으로 복원합니다.
    printc(padd(map_pos, prev), prev_char, prev_color); // 이전 위치 복원

    // 현재 위치에 커서 표시 (커서 색상으로 출력)
    printc(padd(map_pos, curr), curr_char, COLOR_CURSOR); // 현재 위치에 커서 출력

    // frontbuf 업데이트 (커서가 현재 위치에 올바르게 표시되도록)
    frontbuf[prev.row][prev.column] = prev_char; // 이전 위치의 frontbuf 업데이트
    frontbuf[curr.row][curr.column] = COLOR_CURSOR; // 현재 위치의 frontbuf 업데이트
}


