/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "io.h"

// ����� ����� ��ǥ
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

//�� �Լ�
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            dest[i][j] = ' '; // �ʱⰪ ����

            // Layer 1(������Ʈ ���̾�)�� ���� üũ
            // -1�� �ƴ� ��� ���� üũ�ϵ��� ����
            if (src[1][i][j] >= 0) {
                dest[i][j] = src[1][i][j];
                continue;  // Layer 1�� ������Ʈ�� ������ Layer 0�� Ȯ������ ����
            }

            // Layer 0 üũ
            if (src[0][i][j] != 0) {
                dest[i][j] = src[0][i][j];
            }
        }
    }
}

//�� �Լ�
int get_color_at(POSITION pos) {
    char ch = backbuf[pos.row][pos.column];

    // ���ڿ� ���� ���� ��ȯ
    switch (ch) {
    case 'B':
        // �Ʊ� ���� (14,1)~(15,2)
        if (pos.row >= 15 && pos.row <= 16 && pos.column >= 1 && pos.column <= 2) {
            return COLOR_BLUE;
        }
        // ���� ���� (1,57)~(2,58)
        else if (pos.row >= 1 && pos.row <= 2 && pos.column >= 57 && pos.column <= 58) {
            return COLOR_RED;
        }
        // �� �� ����
        else {
            return COLOR_GREEN;  // �Ǵ� �ٸ� ����
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
    case 'D':  // ����
        return COLOR_CYAN;
    case 'G':  // â��
        return COLOR_MAGENTA;
    case 'S':  // ����ó
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

            // ���� ���¿� �ٸ��ų� ������Ʈ('W')�� ��� �׻� ���
            if (frontbuf[i][j] != current_char || current_char == 'W') {
                POSITION pos = padd(map_pos, (POSITION) { i, j });
                printc(pos, current_char, color);
                frontbuf[i][j] = current_char;
            }
        }
    }
}
void display_cursor(CURSOR cursor) {
    // ���� Ŀ�� ��ġ ����
    if (building_state.cursor_size == 2) {
        // 2x2 ũ���� ���� Ŀ�� ��ġ ����
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
        // 1x1 ũ���� ���� Ŀ�� ��ġ ����
        POSITION prev = cursor.previous;
        char prev_char = backbuf[prev.row][prev.column];
        int prev_color = get_color_at(prev);
        printc(padd(map_pos, prev), prev_char, prev_color);
        frontbuf[prev.row][prev.column] = prev_char;
    }

    // ���� Ŀ�� ��ġ ǥ��
    if (building_state.cursor_size == 2) {
        // 2x2 ũ���� ���� Ŀ�� ǥ��
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
        // 1x1 ũ���� ���� Ŀ�� ǥ��
        POSITION curr = cursor.current;
        char curr_char = backbuf[curr.row][curr.column];
        printc(padd(map_pos, curr), curr_char, COLOR_CURSOR);
        frontbuf[curr.row][curr.column] = COLOR_CURSOR;
    }
}
