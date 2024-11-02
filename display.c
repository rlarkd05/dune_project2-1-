/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "io.h"

// ����� ������� �»��(topleft) ��ǥ
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
    printf("spice = %d/%d, population=%d/%d\n",
        resource.spice, resource.spice_max,
        resource.population, resource.population_max
    );
}

// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            dest[i][j] = ' '; // �ʱⰪ ����
            for (int k = 0; k < N_LAYER; k++) {
                if (src[k][i][j] >= 0) { // ��ȿ�� ������ ����
                    dest[i][j] = src[k][i][j]; // ���� ���� �ִ� ���̾��� �� ����
                    break; // ù ��°�� �߰ߵ� ���ڸ� ���
                }
            }
        }
    }
}

int get_color_at(POSITION pos) {
    char ch = backbuf[pos.row][pos.column];

    // ���ڿ� ���� ���� ��ȯ
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
    case 'o':
        return COLOR_YELLOW;
    default:
        return COLOR_DEFAULT; // �⺻ ����
    }
}


void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    project(map, backbuf); // ���ο� ���� backbuf�� ������Ʈ�մϴ�.

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            char current_char = backbuf[i][j];

            // ���� ��ġ�� ������ �����ɴϴ�.
            int color = get_color_at((POSITION) { i, j });

            // ����� �κи� ȭ�鿡 ���
            if (frontbuf[i][j] != current_char) {
                // 1. ������ ��� �ƹ� ���� �������� ����
                if (current_char == ' ') {
                    void printc(POSITION pos, char ch, int color);
                }
                else {
                    // 2. ���ڿ��� ���� �������� ���
                    printc(padd(map_pos, (POSITION) { i, j }), current_char, color);
                }

                // frontbuf ������Ʈ
                frontbuf[i][j] = current_char; // frontbuf ������Ʈ
            }
        }
    }
}
void display_cursor(CURSOR cursor) {
    POSITION prev = cursor.previous;
    POSITION curr = cursor.current;

    // ���� ��ġ�� ���ڸ� backbuf���� �����ɴϴ�.
    char prev_char = backbuf[prev.row][prev.column];
    int prev_color = get_color_at(prev); // ���� ��ġ�� ����

    // ���� ��ġ�� ���ڸ� backbuf���� �����ɴϴ�.
    char curr_char = backbuf[curr.row][curr.column];

    // ���� ��ġ�� ���� ���ڿ� �������� �����մϴ�.
    printc(padd(map_pos, prev), prev_char, prev_color); // ���� ��ġ ����

    // ���� ��ġ�� Ŀ�� ǥ�� (Ŀ�� �������� ���)
    printc(padd(map_pos, curr), curr_char, COLOR_CURSOR); // ���� ��ġ�� Ŀ�� ���

    // frontbuf ������Ʈ (Ŀ���� ���� ��ġ�� �ùٸ��� ǥ�õǵ���)
    frontbuf[prev.row][prev.column] = prev_char; // ���� ��ġ�� frontbuf ������Ʈ
    frontbuf[curr.row][curr.column] = COLOR_CURSOR; // ���� ��ġ�� frontbuf ������Ʈ
}


