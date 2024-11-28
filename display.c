/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

extern HARVESTER harvesters[MAX_HARVESTERS];
extern int harvester_count;
extern UNIT Harverster;
extern BUILDING_STATE building_state;

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

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            dest[i][j] = ' ';

            if (src[1][i][j] >= 0) {
                dest[i][j] = src[1][i][j];
                continue;
            }

            if (src[0][i][j] != 0) {
                dest[i][j] = src[0][i][j];
            }
        }
    }
}

int get_color_at(POSITION pos) {
    char ch = backbuf[pos.row][pos.column];

    switch (ch) {
    case 'B':
        if (pos.row >= 15 && pos.row <= 16 && pos.column >= 1 && pos.column <= 2) {
            return COLOR_BLUE;
        }
        else if (pos.row >= 1 && pos.row <= 2 && pos.column >= 57 && pos.column <= 58) {
            return COLOR_RED;
        }
        else {
            return COLOR_GREEN;
        }

    case 'H':
        for (int i = 0; i < harvester_count; i++) {
            if (harvesters[i].pos.row == pos.row &&
                harvesters[i].pos.column == pos.column &&
                harvesters[i].is_ally) {
                return COLOR_BLUE;
            }
        }
        // 초기 아군 하베스터 체크 
        if (pos.row == Harverster.pos1.row && pos.column == Harverster.pos1.column) {
            return COLOR_BLUE;
        }
        return COLOR_RED;

        // 스파이스 색상 처리 수정
    case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8':
        return COLOR_ORANGE;  // 모든 숫자를 스파이스와 같은 색상으로 표시

    case 'P':
        return FLOOR_BEIGE;
    case 'R':
        return COLOR_GRAY;
    case '#':
        return COLOR_BLACK;
    case 'W':
        return COLOR_YELLOW;
    case 'D':
        return COLOR_CYAN;
    case 'G':
        return COLOR_MAGENTA;
    case 'S':
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

            if (frontbuf[i][j] != current_char ||
                current_char == 'W' ||
                current_char == 'H') {
                POSITION pos = padd(map_pos, (POSITION) { i, j });
                printc(pos, current_char, color);
                frontbuf[i][j] = current_char;
            }
        }
    }
}

void display_cursor(CURSOR cursor) {
    if (building_state.cursor_size == 2) {
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
        POSITION prev = cursor.previous;
        char prev_char = backbuf[prev.row][prev.column];
        int prev_color = get_color_at(prev);
        printc(padd(map_pos, prev), prev_char, prev_color);
        frontbuf[prev.row][prev.column] = prev_char;
    }

    if (building_state.cursor_size == 2) {
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
        POSITION curr = cursor.current;
        char curr_char = backbuf[curr.row][curr.column];
        printc(padd(map_pos, curr), curr_char, COLOR_CURSOR);
        frontbuf[curr.row][curr.column] = COLOR_CURSOR;
    }
}