#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"


void init(void); //�� �Լ�
void intro(void); //��Ʈ�� �Լ�
void Construction(void); //������ �Լ�
void Biome(void); //���� �Լ�
void unit(void); //���� �Լ�
void outro(void); //�ƿ�Ʈ��
void cursor_move(DIRECTION dir); //Ŀ�� ������ �Լ�
void cursor_double_move(DIRECTION dir, int times); //Ŀ�� ����Ŭ�� �Լ�
void sandworm_move(void); //�����1 �Լ�
void sandworm1_move(void); //����� 2 �Լ�
void StatusWindow(void); //����â �Լ�
void system_message(void); //�ý��� �޽���â �Լ�
void command_message(void); //���â �Լ�
void print_terrain(void); //����â�� ���� ��� �Լ�
void print_unit_info(void); //����â�� ���� ��� �Լ�
void clean_status(void); //escŰ�� �������� ����â ����ִ� �Լ�
void select_building(void); //�ǹ��� �����̽��ٸ� �������� ���â�� ������ ��� �߰� ���ִ� �Լ�
void select_unit(void); // ���� ���� �Լ� ���� �߰�
void process_command(KEY key); //Ű���� �Է¿� ���� ����� ó���ϴ� �Լ�
void print_command_message(const char* message); //���â ��� �Լ�
void print_system_message(const char* message); // �ý���â ����Լ�
bool can_produce_harvester(void); //�Ϻ����� ���� ���� �Լ�
POSITION find_nearest_unit(POSITION current_pos); // ������� ��ó ���� ã�� �Լ�
void spawn_spice(POSITION pos); //������� ���� Ȯ���� �����̽� ���� �Լ�
bool can_build_here(void);
void build_structure(void);


/* ================= control =================== */
int sys_clock = 0;
CURSOR cursor = { { 1, 1 }, {1, 1} };
int last_key_time = 0;
#define DOUBLE_PRESS_INTERVAL 200
#define MAX_MSG_LINES 6  
#define MAX_UNITS 10  // population_max�� �ִ밪
// ���� ���� �߰� (���� �������� ���� �κп� �߰�)
UNIT units[10];  // population_max�� �ִ밪(10)��ŭ �迭 ũ�� ����
int unit_count = 0;
/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

SELECTED_BUILDING selected_building = {
    .type = ' ',
    .position = {0, 0},
    .is_selected = false,
    .is_ally = false
};

RESOURCE resource = {
   .spice = 50,
   .spice_max = 50,
   .population = 0,
   .population_max = 5
};

//������Ʈ ���� ����� �� ����
OBJECT_SAMPLE obj = {
   .pos = {3, 10},
   .dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
   .repr = 'W',
   .speed = 600,
   .next_move_time = 600
};

OBJECT_SAMPLE1 obj1 = {
    .pos = {16, 50},
    .dest = {(MAP_HEIGHT + 2) - MAP_HEIGHT, (MAP_WIDTH + 2) - MAP_HEIGHT},
    .repr = 'W',
    .speed = 600,
    .next_move_time = 600
};

//�Ʊ����̽�
OBJECT_BUILDING ally_base = {
   .pos1 = {15, 1},
   .pos2 = {15, 2},
   .pos3 = {16, 1},
   .pos4 = {16, 2},
   .repr = 'B',
   .layer = 0
};

//�����̽�
OBJECT_BUILDING enemy_base = {
   .pos1 = {1, 57},
   .pos2 = {1, 56},
   .pos3 = {2, 57},
   .pos4 = {2, 56},
   .repr = 'B',
   .layer = 0
};

//�Ʊ� ����
OBJECT_BUILDING ally_pad = {
   .pos1 = {15, 3},
   .pos2 = {15, 4},
   .pos3 = {16, 3},
   .pos4 = {16, 4},
   .repr = 'P',
   .layer = 0
};

//���� ����
OBJECT_BUILDING enemy_pad = {
   .pos1 = {1, 55},
   .pos2 = {1, 56},
   .pos3 = {0, 55},
   .pos4 = {0, 56},
   .repr = 'P',  // E�� ���� ������ ��Ÿ���� ����
   .layer = 0
};

//�Ʊ� �����̽�
OBJECT_BUILDING ally_spice = {
   .pos1 = {13, 1},  // �Ʊ� �����̽� ��ġ
   .repr = '5',      // �����̽��� ��Ÿ���� ����
   .layer = 0        // ù ��° ���̾�
};

OBJECT_BUILDING enemy_spice = {
   .pos1 = {4, 58},
   .repr = '5', // �����̽��� ��Ÿ���� ����
   .layer = 0   // ù ��° ���̾�
};

// ���� ����ü ����
OBJECT_BUILDING ROCK_1 = {
   .pos1 = {4, 25},
   .pos2 = {3, 25},
   .pos3 = {4, 26},
   .pos4 = {3, 26},
   .repr = 'R',
   .layer = 0
};

OBJECT_BUILDING ROCK_2 = {
   .pos1 = {12, 32},
   .pos2 = {13, 32},
   .pos3 = {12, 33},
   .pos4 = {13, 33},
   .repr = 'R',
   .layer = 0
};

OBJECT_BUILDING ROCK_3 = {
   .pos1 = {11, 5},  // �ϳ��� ��ġ�� �ִ� ������ ����ü�� ���� ����
   .repr = 'R',
   .layer = 0
};

OBJECT_BUILDING ROCK_4 = {
   .pos1 = {6, 48},
   .repr = 'R',
   .layer = 0
};

OBJECT_BUILDING ROCK_5 = {
   .pos1 = {13, 50},
   .repr = 'R',
   .layer = 0
};

UNIT Harverster = {
    .pos1 = {14, 1},
    .repr = 'H',
    .layer = 1
};

UNIT Haconen = {
    .pos2 = {3, 58},
    .repr = 'H',
    .layer = 1
};

BUILDING_STATE building_state = {
    .is_building_mode = false,
    .building_type = ' ',    // �� ���ڷ� �ʱ�ȭ
    .cursor_size = 1
};

const UnitInfo UNIT_INFO[] = {
    [UNIT_TYPE_HARVESTER] = {
        .type = UNIT_TYPE_HARVESTER,
        .name = "�Ϻ�����",
        .symbol = 'H',
        .can_be_ally = true,
        .can_be_enemy = true,
        .cost = 5
    },
    [UNIT_TYPE_FREMEN] = {
        .type = UNIT_TYPE_FREMEN,
        .name = "������",
        .symbol = 'F',
        .can_be_ally = true,
        .can_be_enemy = false,
        .cost = 6
    },
    [UNIT_TYPE_SOLDIER] = {
        .type = UNIT_TYPE_SOLDIER,
        .name = "����",
        .symbol = 'S',
        .can_be_ally = true,
        .can_be_enemy = false,
        .cost = 4
    },
    [UNIT_TYPE_TANK] = {
        .type = UNIT_TYPE_TANK,
        .name = "������",
        .symbol = 'T',
        .can_be_ally = false,
        .can_be_enemy = true,
        .cost = 7
    },
    [UNIT_TYPE_SANDWORM] = {
        .type = UNIT_TYPE_SANDWORM,
        .name = "�����",
        .symbol = 'W',
        .can_be_ally = false,
        .can_be_enemy = false,
        .cost = 0
    }
};
// �ǹ� ���� �迭 ����
const BUILDING_INFO buildings[] = {
    {
        .type = BD_PLATE,
        .name = "����",
        .shortcut = "P: Plate",
        .cost = 100,
        .durability = 0,
        .effect = "�ǹ� ���� ���� ���",
        .production = "����"
    },
    {
        .type = BD_DORM,
        .name = "����",
        .shortcut = "D: Dormitory",
        .cost = 210,
        .durability = 0,
        .effect = "�α� �ִ�ġ ����(10)",
        .production = "����"
    },
    {
        .type = BD_GARAGE,
        .name = "â��",
        .shortcut = "G: Garage",
        .cost = 410,
        .durability = 0,
        .effect = "�����̽� ���� �ִ�ġ ����(10)",
        .production = "����"
    },
    {
        .type = BD_BARRACKS,
        .name = "��Ʈ���̵� ����",
        .shortcut = "B: Barracks",
        .cost = 420,
        .durability = 0,
        .effect = "���� ����",
        .production = "����(S: Soldier)"
    },
    {
        .type = BD_SHELTER,
        .name = "����ó",
        .shortcut = "S: Shelter",
        .cost = 530,
        .durability = 0,
        .effect = "Ư������ ����",
        .production = "������(F: Fremen)"
    }
};

/* ================= main() =================== */
int main(void) {
    srand((unsigned int)time(NULL));

    intro();
    init();
    Construction();
    Biome();
    unit();
    StatusWindow();
    system_message();
    command_message();
    print_terrain();
    print_command_message("B: �Ǽ�");
    display(resource, map, cursor);

    while (1) {
        KEY key = get_key();

        if (is_arrow_key(key)) {
            DIRECTION dir = ktod(key);
            cursor_move(dir);
        }
        else {
            switch (key) {
            case k_b:  // BŰ ó��
                process_command(key);
                break;
            case k_p:  // ����
            case k_d:  // ����
            case k_g:  // â��
            case k_s:  // ����ó
                if (building_state.is_building_mode) {
                    process_command(key);
                }
                break;
            case k_space:
                if (building_state.is_building_mode) {
                    process_command(key);
                }
                else {
                    select_building();
                    print_terrain();
                    print_unit_info();
                }
                break;
            case k_h:
                if (selected_building.is_selected) {
                    process_command(key);
                }
                break;
            case k_esc:
                if (selected_building.is_selected || building_state.is_building_mode) {
                    process_command(key);
                }
                break;
            case k_quit:
                outro();
                break;
            default:
                if (selected_building.is_selected) {
                    process_command(key);
                }
                break;
            }
        }

        sandworm_move();
        sandworm1_move();
        display(resource, map, cursor);
        Sleep(TICK);
        sys_clock += 10;
    }
}
/* ================= subfunctions =================== */
void intro(void) {
    printf("DUNE 1.5\n");
    Sleep(2000);
    system("cls");
}

void outro(void) {
    printf("exiting...\n");
    exit(0);
}

void cursor_double_move(DIRECTION dir, int times) {
    for (int i = 0; i < times; i++) {
        cursor_move(dir);
    }
}

void Construction(void) {
    // layer 0�� �ִ� �� ������ �ʱ�ȭ - �׵θ� �����ϰ� �ʱ�ȭ
    for (int i = 1; i < MAP_HEIGHT - 1; i++) {
        for (int j = 1; j < MAP_WIDTH - 1; j++) {
            map[0][i][j] = ' ';  // �׵θ��� ������ ���θ� �������� ����
        }
    }

    // �Ʊ� ���̽�
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            map[0][ally_base.pos1.row + i][ally_base.pos1.column + j] = ally_base.repr;
        }
    }
    // ���� ���̽�
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            map[0][enemy_base.pos1.row + i][enemy_base.pos1.column + j] = enemy_base.repr;
        }
    }

    // �Ʊ� ����
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            map[0][ally_pad.pos1.row + i][ally_pad.pos1.column + j] = ally_pad.repr;
        }
    }

    // ���� ����
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            map[0][enemy_pad.pos1.row + i][enemy_pad.pos1.column + j] = enemy_pad.repr;
        }
    }

    // �Ʊ� �����̽�
    map[0][ally_spice.pos1.row][ally_spice.pos1.column] = ally_spice.repr;

    // ���� �����̽�
    map[0][enemy_spice.pos1.row][enemy_spice.pos1.column] = enemy_spice.repr;
}

void Biome(void) {
    // ���� 1 �߰�
    map[ROCK_1.layer][ROCK_1.pos1.row][ROCK_1.pos1.column] = ROCK_1.repr;
    map[ROCK_1.layer][ROCK_1.pos2.row][ROCK_1.pos2.column] = ROCK_1.repr;
    map[ROCK_1.layer][ROCK_1.pos3.row][ROCK_1.pos3.column] = ROCK_1.repr;
    map[ROCK_1.layer][ROCK_1.pos4.row][ROCK_1.pos4.column] = ROCK_1.repr;

    // ���� 2 �߰�
    map[ROCK_2.layer][ROCK_2.pos1.row][ROCK_2.pos1.column] = ROCK_2.repr;
    map[ROCK_2.layer][ROCK_2.pos2.row][ROCK_2.pos2.column] = ROCK_2.repr;
    map[ROCK_2.layer][ROCK_2.pos3.row][ROCK_2.pos3.column] = ROCK_2.repr;
    map[ROCK_2.layer][ROCK_2.pos4.row][ROCK_2.pos4.column] = ROCK_2.repr;

    // ���� 3 �߰�
    map[ROCK_3.layer][ROCK_3.pos1.row][ROCK_3.pos1.column] = ROCK_3.repr;

    // ���� 4 �߰�
    map[ROCK_4.layer][ROCK_4.pos1.row][ROCK_4.pos1.column] = ROCK_4.repr;

    // ���� 5 �߰�
    map[ROCK_5.layer][ROCK_5.pos1.row][ROCK_5.pos1.column] = ROCK_5.repr;
}

void unit(void) {

    // �ʱ� �Ϻ����� ���� (�Ʊ�)
    units[unit_count].pos1 = (POSITION){ 14, 1 };
    units[unit_count].repr = 'H';
    units[unit_count].layer = 1;
    map[units[unit_count].layer][units[unit_count].pos1.row][units[unit_count].pos1.column] = units[unit_count].repr;
    unit_count++;
    resource.population += 5;  // �Ϻ����ʹ� �α��� 5 �Ҹ�

    // �ʱ� �Ϻ����� ���� (����)
    units[unit_count].pos1 = (POSITION){ 3, 58 };
    units[unit_count].repr = 'H';
    units[unit_count].layer = 1;
    map[units[unit_count].layer][units[unit_count].pos1.row][units[unit_count].pos1.column] = units[unit_count].repr;
    unit_count++;
}
//#���� �� Ʋ ����
void init(void) {
    // layer 0(map[0])�� ���� ����
    for (int j = 0; j < MAP_WIDTH; j++) {
        map[0][0][j] = '#';
        map[0][MAP_HEIGHT - 1][j] = '#';
    }

    for (int i = 1; i < MAP_HEIGHT - 1; i++) {
        map[0][i][0] = '#';
        map[0][i][MAP_WIDTH - 1] = '#';
        for (int j = 1; j < MAP_WIDTH - 1; j++) {
            map[0][i][j] = ' ';
        }
    }

    // layer 1(map[1]) �ʱ�ȭ - ��� ��ġ�� -1�� ����
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            map[1][i][j] = -1;
        }
    }
}

//����â
void StatusWindow() {
    POSITION pos;

    // ���� ���� �׵θ� ���
    for (int row = 1; row < MAP_HEIGHT + 1; row++) {
        pos.row = row;
        pos.column = MAP_WIDTH + 2; // �� �����ʿ� ����â ��ġ ����
        gotoxy(pos);
        printf("#");
    }

    // ��� ���� �׵θ� ���
    for (int col = MAP_WIDTH + 2; col < MAP_WIDTH + 60; col++) {
        pos.row = 1;
        pos.column = col;
        gotoxy(pos);
        printf("#");
    }

    // �ϴ� ���� �׵θ� ���
    for (int col = MAP_WIDTH + 2; col < MAP_WIDTH + 60; col++) {
        pos.row = MAP_HEIGHT;
        pos.column = col;
        gotoxy(pos);
        printf("#");
    }

    // ������ ���� �׵θ� ���
    for (int row = 1; row < MAP_HEIGHT + 1; row++) {
        pos.row = row;
        pos.column = MAP_WIDTH + 59; // ������ �� ��ġ
        gotoxy(pos);
        printf("#");
    }
}

//�ý��� �޽���
void system_message() {
    POSITION pos;
    int message_start_row = MAP_HEIGHT + 2; // �ý��� �޽��� â�� ���� ��
    int message_width = MAP_WIDTH - 1;      // �ý��� �޽��� â �ʺ�
    int message_height = 8;                 // �ý��� �޽��� â ����

    // �׵θ��� ���� ���� ���
    for (int row = message_start_row; row < message_start_row + message_height; row++) {
        for (int col = 0; col <= message_width; col++) {
            pos.row = row;
            pos.column = col;
            gotoxy(pos);

            // �׵θ��� ��� '#' ���
            if (row == message_start_row || row == message_start_row + message_height - 1 ||
                col == 0 || col == message_width) {
                printf("#");
            }
            // ������ ��� ���� ���
            else {
                printf(" ");
            }
        }
    }
}

//���â

void command_message() {
    POSITION pos;
    int command_start_row = MAP_HEIGHT + 2;  // ���â�� ���� �� (����â �ٷ� �Ʒ�)
    int command_width = 58;                  // ���â �ʺ� (����â �ʺ�� ����)
    int command_height = 8;                  // ���â ����

    // �׵θ��� ���� ���� ���
    for (int row = command_start_row; row < command_start_row + command_height; row++) {
        for (int col = MAP_WIDTH + 2; col < MAP_WIDTH + 2 + command_width; col++) {
            pos.row = row;
            pos.column = col;
            gotoxy(pos);

            // �׵θ��� ��� '#' ���
            if (row == command_start_row || row == command_start_row + command_height - 1 ||
                col == MAP_WIDTH + 2 || col == MAP_WIDTH + 2 + command_width - 1) {
                printf("#");
            }
            // ������ ��� ���� ���
            else {
                printf(" ");
            }
        }
    }
}
// (�����ϴٸ�) ������ �������� Ŀ�� �̵�
void cursor_move(DIRECTION dir) {
    POSITION curr = cursor.current;
    POSITION new_pos = pmove(curr, dir);

    int time_diff = sys_clock - last_key_time;
    last_key_time = sys_clock;

    int move_distance = (time_diff < DOUBLE_PRESS_INTERVAL) ? 2 : 1;

    for (int i = 0; i < move_distance; i++) {
        // building_state�� cursor_size�� ����� ��� üũ
        int max_row = MAP_HEIGHT - building_state.cursor_size;
        int max_col = MAP_WIDTH - building_state.cursor_size;

        if (1 <= new_pos.row && new_pos.row <= max_row &&
            1 <= new_pos.column && new_pos.column <= max_col) {
            cursor.previous = cursor.current;
            cursor.current = new_pos;
            new_pos = pmove(new_pos, dir);
        }
        else {
            break;
        }
    }
}


POSITION sandworm_next_position(void) {
    POSITION nearest_unit = find_nearest_unit(obj.pos);
    POSITION diff = psub(nearest_unit, obj.pos);
    DIRECTION dir;

    // ������ ������ �����ϰ� ������
    if (nearest_unit.row == obj.pos.row && nearest_unit.column == obj.pos.column) {
        dir = rand() % 4;  // 0~3 ������ ������ ����
    }
    // ������ ������ ���� �������� �̵�
    else {
        if (abs(diff.row) >= abs(diff.column)) {
            dir = (diff.row >= 0) ? d_down : d_up;
        }
        else {
            dir = (diff.column >= 0) ? d_right : d_left;
        }
    }

    POSITION next_pos = pmove(obj.pos, dir);

    // �� ���� üũ
    if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 &&
        1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2) {
        // ������ ������ �� ���� ��ȯ
        if (map[0][next_pos.row][next_pos.column] == 'R') {
            if (dir == d_up || dir == d_down) {
                dir = (diff.column >= 0) ? d_right : d_left;
            }
            else {
                dir = (diff.row >= 0) ? d_down : d_up;
            }
            return pmove(obj.pos, dir);
        }
        return next_pos;
    }
    return obj.pos;
}

POSITION sandworm1_next_position(void) {
    POSITION nearest_unit = find_nearest_unit(obj1.pos);
    POSITION diff = psub(nearest_unit, obj1.pos);
    DIRECTION dir;

    // ������ ������ �����ϰ� ������
    if (nearest_unit.row == obj1.pos.row && nearest_unit.column == obj1.pos.column) {
        dir = rand() % 4;  // 0~3 ������ ������ ����
    }
    // ������ ������ ���� �������� �̵�
    else {
        if (abs(diff.row) >= abs(diff.column)) {
            dir = (diff.row >= 0) ? d_down : d_up;
        }
        else {
            dir = (diff.column >= 0) ? d_right : d_left;
        }
    }

    POSITION next_pos = pmove(obj1.pos, dir);

    // �� ���� üũ
    if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 &&
        1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2) {
        // ������ ������ �� ���� ��ȯ
        if (map[0][next_pos.row][next_pos.column] == 'R') {
            if (dir == d_up || dir == d_down) {
                dir = (diff.column >= 0) ? d_right : d_left;
            }
            else {
                dir = (diff.row >= 0) ? d_down : d_up;
            }
            return pmove(obj1.pos, dir);
        }
        return next_pos;
    }
    return obj1.pos;
}

void sandworm_move(void) {
    if (sys_clock <= obj.next_move_time) {
        return;
    }

    POSITION next_pos = sandworm_next_position();
    map[1][obj.pos.row][obj.pos.column] = -1;

    // �Ϻ����Ͱ� �ִ� ��쿡��(-1�� �ƴ� ����) ����
    if (map[1][next_pos.row][next_pos.column] == 'H') {
        if (next_pos.row <= ally_base.pos4.row + 1 && next_pos.column <= ally_base.pos4.column + 3) {
            resource.population -= 5;  // �Ʊ� �Ϻ������� ��츸 �α��� ����
        }
        map[1][next_pos.row][next_pos.column] = -1;  // �ʿ����� ����
    }

    obj.pos = next_pos;
    map[1][obj.pos.row][obj.pos.column] = obj.repr;
    obj.next_move_time = sys_clock + obj.speed;
}

void sandworm1_move(void) {
    if (sys_clock <= obj1.next_move_time) {
        return;
    }

    POSITION next_pos = sandworm1_next_position();
    map[1][obj1.pos.row][obj1.pos.column] = -1;

    // �Ϻ����Ͱ� �ִ� ��쿡��(-1�� �ƴ� ����) ����
    if (map[1][next_pos.row][next_pos.column] == 'H') {
        if (next_pos.row <= ally_base.pos4.row + 1 && next_pos.column <= ally_base.pos4.column + 3) {
            resource.population -= 5;  // �Ʊ� �Ϻ������� ��츸 �α��� ����
        }
        map[1][next_pos.row][next_pos.column] = -1;  // �ʿ����� ����
    }

    obj1.pos = next_pos;
    map[1][obj1.pos.row][obj1.pos.column] = obj1.repr;
    obj1.next_move_time = sys_clock + obj1.speed;
}
//� �������� ��������
void print_terrain(void) {
    POSITION pos;
    char terrain = map[0][cursor.current.row][cursor.current.column];

    clean_status();  // ����â �ʱ�ȭ

    // ���� ���
    pos.row = 2;
    pos.column = MAP_WIDTH + 4;
    gotoxy(pos);
    printf("=== ���� ���� ===");

    //Ŀ�� ��ġ ���
    pos.row = 4;
    pos.column = MAP_WIDTH + 4;
    gotoxy(pos);
    printf("Ŀ�� ��ġ: (%d, %d)", cursor.current.row, cursor.current.column);

    //���� ���� ���
    pos.row = 6;
    pos.column = MAP_WIDTH + 4;
    gotoxy(pos);
    printf("����: ");
    if (terrain == 'B') {
        if (cursor.current.row >= ally_base.pos1.row &&
            cursor.current.row <= ally_base.pos4.row &&
            cursor.current.column >= ally_base.pos1.column &&
            cursor.current.column <= ally_base.pos4.column) {
            printf("Base (�Ʊ�)");
        }
        else {
            printf("Base (����)");
        }
    }
    else if (terrain == 'P') {
        printf("����");
    }
    else if (terrain == 'R') {
        printf("����");
    }
    else if (terrain == '5') {
        printf("�����̽�");
    }
    else {
        printf("�縷 ����");
    }

    // ���� Ư¡ ���
    pos.row = 8;
    pos.column = MAP_WIDTH + 4;
    gotoxy(pos);
    printf("Ư¡: ");
    if (terrain == 'B') {
        printf("���� ���� �ǹ�");
    }
    else if (terrain == 'P') {
        printf("���� ���� �Ǽ� ����");
    }
    else if (terrain == 'R') {
        printf("������� ����� �� ����");
    }
    else if (terrain == '5') {
        printf("���� ������ �ڿ�");
    }
    else {
        printf("�⺻ ����, �ǹ��� ���� �� ����");
    }
}

void select_unit(void) {
    char unit = map[1][cursor.current.row][cursor.current.column];

    selected_building.is_selected = false;
    selected_building.type = ' ';

    // �Ϻ����� ����
    if (unit == 'H') {
        selected_building.is_selected = true;

        if (cursor.current.row == Harverster.pos1.row &&
            cursor.current.column == Harverster.pos1.column) {
            clean_status();
            print_unit_info();
            print_system_message("�Ʊ� �Ϻ����͸� �����߽��ϴ�");
        }
        else if (cursor.current.row == Haconen.pos2.row &&
            cursor.current.column == Haconen.pos2.column) {
            clean_status();
            print_unit_info();
            print_system_message("���� �Ϻ����͸� �����߽��ϴ�");
        }
    }
    // ����� ����
    else if (unit == 'W') {
        selected_building.is_selected = true;
        clean_status();
        print_unit_info();
        print_system_message("������� �����߽��ϴ�");
    }
}
void print_unit_info(void) {
    POSITION pos;
    char unit = map[1][cursor.current.row][cursor.current.column];

    if (unit != -1) {
        // ���� ���
        pos.row = 2;  // ���� ������ ���� ��ġ���� ����
        pos.column = MAP_WIDTH + 4;
        gotoxy(pos);
        printf("=== ���� ���� ===");

        // ���� ��ġ ���
        pos.row = 4;
        pos.column = MAP_WIDTH + 4;
        gotoxy(pos);
        printf("Ŀ�� ��ġ: (%d, %d)", cursor.current.row, cursor.current.column);

        // ���� ���� ���
        pos.row = 6;
        pos.column = MAP_WIDTH + 4;
        gotoxy(pos);
        printf("����: ");

        if (unit == 'H') {
            // �Ʊ� �Ϻ����� ����
            if (cursor.current.row == Harverster.pos1.row &&
                cursor.current.column == Harverster.pos1.column) {
                printf("�Ϻ����� (�Ʊ�)");

                pos.row = 8;
                pos.column = MAP_WIDTH + 4;
                gotoxy(pos);
                printf("����: ����                                       ");

            }
            // ���� ���ڳ� ����
            else if (cursor.current.row == Haconen.pos2.row &&
                cursor.current.column == Haconen.pos2.column) {
                printf("�Ϻ����� (����)");

                pos.row = 8;
                pos.column = MAP_WIDTH + 4;
                gotoxy(pos);
                printf("�Ҽ�: ���ڳ�                                    ");
            }
        }
        else if (unit == 'W') {
            printf("�����   ");

            pos.row = 8;
            pos.column = MAP_WIDTH + 4;
            gotoxy(pos);
            printf("Ư��: ���� ���� �� ����                        ");
        }
    }
}

void clean_status(void) {
    POSITION pos;
    // ����â ���� �����
    for (int row = 2; row < MAP_HEIGHT; row++) {
        for (int col = MAP_WIDTH + 3; col < MAP_WIDTH + 58; col++) {
            pos.row = row;
            pos.column = col;
            gotoxy(pos);
            printf(" ");
        }
    }
}

// ���� ����� ������ ã�� �Լ�
POSITION find_nearest_unit(POSITION current_pos) {
    POSITION nearest = { -1, -1 };  // �ʱⰪ�� -1, -1�� ����
    int min_distance = MAP_WIDTH * MAP_HEIGHT;
    bool found_unit = false;  // ���� �߰� ���� üũ

    // ��� ���� ��ȸ�ϸ鼭 ���� ã��
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (map[1][i][j] == 'H') {
                POSITION unit_pos = { i, j };
                int distance = abs(current_pos.row - i) + abs(current_pos.column - j);
                if (distance < min_distance && distance > 0) {
                    min_distance = distance;
                    nearest = unit_pos;
                    found_unit = true;
                }
            }
        }
    }

    // ������ �� ã���� ��� ������ ������ ��ȯ
    if (!found_unit) {
        nearest.row = 1 + (rand() % (MAP_HEIGHT - 2));
        nearest.column = 1 + (rand() % (MAP_WIDTH - 2));
    }

    return nearest;
}
// ������� �����̽��� �����ϴ� �Լ�
void spawn_spice(POSITION pos) {
    // 15% Ȯ���� �����̽� ����
    if (rand() % 100 < 15 && map[0][pos.row][pos.column] == ' ') {
        map[0][pos.row][pos.column] = '5';
    }
}

// �Ǽ� �޴� ó�� �Լ� ����
void process_command(KEY key) {
    if (!selected_building.is_selected && !building_state.is_building_mode) {
        if (key == k_b) {
            building_state.is_building_mode = true;
            building_state.cursor_size = 1;  // �ʱ� ���´� 1x1
            print_command_message("P:���� D:���� G:â�� B:���� S:����ó ESC:���");
        }
    }
    else if (building_state.is_building_mode) {
        switch (key) {
        case k_p:
            building_state.building_type = BD_PLATE;
            building_state.cursor_size = 2;  // 2x2�� ����
            print_command_message("���� �Ǽ� - [Space]�Ǽ� [ESC]��� (���: 2 ������: 10)");
            break;

        case k_d:
            building_state.building_type = BD_DORM;
            building_state.cursor_size = 2;
            print_command_message("���� �Ǽ� - [Space]�Ǽ� [ESC]��� (���: 3 ������: 15)");
            break;

        case k_g:
            building_state.building_type = BD_GARAGE;
            building_state.cursor_size = 2;
            print_command_message("â�� �Ǽ� - [Space]�Ǽ� [ESC]��� (���: 4 ������: 15)");
            break;

        case k_b:
            building_state.building_type = BD_BARRACKS;
            building_state.cursor_size = 2;
            print_command_message("���� �Ǽ� - [Space]�Ǽ� [ESC]��� (���: 5 ������: 20)");
            break;

        case k_s:
            building_state.building_type = BD_SHELTER;
            building_state.cursor_size = 2;
            print_command_message("����ó �Ǽ� - [Space]�Ǽ� [ESC]��� (���: 6 ������: 25)");
            break;

        case k_space:
            if (building_state.building_type != ' ') {
                if (can_build_here()) {
                    build_structure();
                    building_state.is_building_mode = false;
                    building_state.building_type = ' ';
                    building_state.cursor_size = 1;  // �Ǽ� �Ϸ� �� 1x1�� ����
                    print_command_message("B: �Ǽ�                                           ");
                }
            }
            break;

        case k_esc:
            building_state.is_building_mode = false;
            building_state.building_type = ' ';
            building_state.cursor_size = 1;  // ESC�� ��� �� 1x1�� ����
            print_command_message("B: �Ǽ�                                                ");
            break;
        }
    }
    else if (selected_building.type == 'B' && selected_building.is_ally) {
        print_command_message("H:�Ϻ����� ���� | ESC:���");

        switch (key) {
        case k_h:
            if (resource.spice < 5) {
                print_system_message("�����̽��� �����մϴ�!");
            }
            else if (resource.population + 5 > resource.population_max) {
                print_system_message("�α� �� ������ �ʰ��߽��ϴ�!");
            }
            else {
                map[1][ally_base.pos2.row - 1][ally_base.pos2.column] = 'H';
                resource.spice -= 5;
                resource.population += 5;
                print_system_message("�Ϻ����Ͱ� ����Ǿ����ϴ�!");
            }
            break;

        case k_esc:
            selected_building.is_selected = false;
            selected_building.type = ' ';
            print_command_message("B: �Ǽ�");
            break;
        }
    }
}
// �ý��� �޽����� �����ؼ� ǥ���ϴ� ���� ���� �߰�

#define MAX_MESSAGES 5  // �� �Ʒ� �� �����ϰ� 5���� ����
const char* message_log[MAX_MESSAGES];  // ���� �޽������� ������ �迭
const char* last_message = NULL;        // ������ �Էµ� �޽��� ����

void print_system_message(const char* message) {
    POSITION pos;
    pos.column = 2;

    // �ý��� �޽���â ���� �����
    for (int i = 0; i < 6; i++) {  // 6�� ��ü �����
        pos.row = MAP_HEIGHT + 3 + i;
        gotoxy(pos);
        printf("                                             ");
    }

    // ���� �޽����� ������ �迭�� ����
    if (last_message != NULL) {
        // ���� �޽������� �� ĭ�� ���� �̵�
        for (int i = 0; i < MAX_MESSAGES - 1; i++) {
            message_log[i] = message_log[i + 1];
        }
        message_log[MAX_MESSAGES - 1] = last_message;
    }

    // ���� �޽����� ��� (MAP_HEIGHT + 3���� ����)
    for (int i = 0; i < MAX_MESSAGES; i++) {
        if (message_log[i] != NULL) {
            pos.row = MAP_HEIGHT + 3 + i;
            gotoxy(pos);
            printf("%s", message_log[i]);
        }
    }

    // �� �޽����� �׻� �� �Ʒ�(MAP_HEIGHT + 8)�� ���
    pos.row = MAP_HEIGHT + 8;
    gotoxy(pos);
    printf("%s", message);

    last_message = message;
}
void print_command_message(const char* message) { //���â ���
    POSITION pos = { MAP_HEIGHT + 3, MAP_WIDTH + 4 };
    gotoxy(pos);
    printf("                                                   ");  // ���� �޽��� �����
    gotoxy(pos);
    printf("%s", message);
}

void select_building(void) {
    char terrain = map[0][cursor.current.row][cursor.current.column];
    char unit = map[1][cursor.current.row][cursor.current.column];

    selected_building.is_selected = false;
    selected_building.type = ' ';

    // ������ �ִ� ��� ���� ���� �Լ� ȣ��
    if (unit != -1) {
        select_unit();
        return;
    }

    clean_status();
    print_terrain();

    // �ǹ� ���� �� ��� ó��
    if (terrain == 'B' &&
        cursor.current.row >= ally_base.pos1.row &&
        cursor.current.row <= ally_base.pos4.row &&
        cursor.current.column >= ally_base.pos1.column &&
        cursor.current.column <= ally_base.pos4.column) {
        //������ ��� �����̽��� �������� üũ
        selected_building.type = 'B';
        selected_building.position = cursor.current;
        selected_building.is_selected = true;
        selected_building.is_ally = true;

        print_command_message("=����= H:�Ϻ����� ���� | ESC:���");
        print_system_message("�Ʊ� ������ �����߽��ϴ�");
    }
    // ������ ����/�ǹ� ����
    else if (terrain == 'B') {
        print_system_message("���� ������ �����߽��ϴ�");
    }
    else if (terrain == 'P') {
        print_system_message("������ �����߽��ϴ�");
    }
    else if (terrain == 'R') {
        print_system_message("������ �����߽��ϴ�");
    }
    else if (terrain == '5') {
        print_system_message("�����̽��� �����߽��ϴ�");
    }
    else {
        print_system_message("�縷�� �����߽��ϴ�");
    }
}

bool can_produce_harvester(void) {
    return (resource.spice >= 5 && resource.population + 5 <= resource.population_max);
}

bool can_build_here(void) {
    POSITION pos = cursor.current;
    int cost;

    // �� �ǹ� Ÿ�Ժ� ��� ����
    switch (building_state.building_type) {
    case BD_PLATE:
        cost = 2;
        break;
    case BD_DORM:
        cost = 3;
        break;
    case BD_GARAGE:
        cost = 4;
        break;
    case BD_BARRACKS:
        cost = 5;
        break;
    case BD_SHELTER:
        cost = 6;
        break;
    default:
        return false;
    }

    // �����̽� ��� üũ
    if (resource.spice < cost) {
        print_system_message("�����̽��� �����մϴ�");
        return false;
    }

    // �� ��� üũ
    if (pos.row < 1 || pos.row + building_state.cursor_size > MAP_HEIGHT - 1 ||
        pos.column < 1 || pos.column + building_state.cursor_size > MAP_WIDTH - 1) {
        print_system_message("�� ��迡 �Ǽ��� �� �����ϴ�");
        return false;
    }

    // ������ �ƴ� �ǹ� �Ǽ� �� ���� �ʿ�
    if (building_state.building_type != BD_PLATE) {
        for (int i = 0; i < building_state.cursor_size; i++) {
            for (int j = 0; j < building_state.cursor_size; j++) {
                if (map[0][pos.row + i][pos.column + j] != 'P') {
                    print_system_message("���� ������ �Ǽ��� �� �ֽ��ϴ�");
                    return false;
                }
            }
        }
    }

    // �ٸ� �ǹ����� �浹 üũ
    for (int i = 0; i < building_state.cursor_size; i++) {
        for (int j = 0; j < building_state.cursor_size; j++) {
            char terrain = map[0][pos.row + i][pos.column + j];
            if (building_state.building_type != BD_PLATE && terrain != 'P' && terrain != ' ') {
                print_system_message("�ٸ� �ǹ��� �־� �Ǽ��� �� �����ϴ�");
                return false;
            }
        }
    }

    return true;
}
void build_structure(void) {
    POSITION pos = cursor.current;
    char type = building_state.building_type;
    int cost = 0;

    switch (type) {
    case BD_PLATE:
        cost = 2;
        // ������ 'P'�� 2x2 ä���
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                map[0][pos.row + i][pos.column + j] = 'P';
            }
        }
        print_system_message("���� ��ġ �Ϸ�!");
        break;

    case BD_DORM:
        cost = 3;
        // ���Ҵ� 'D'�� 2x2 ä���
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                map[0][pos.row + i][pos.column + j] = 'D';
            }
        }
        resource.population_max += 10;
        print_system_message("���� �Ǽ� �Ϸ�! �α� ���뷮�� 10 �����߽��ϴ�.");
        break;

    case BD_GARAGE:
        cost = 4;
        // â��� 'G'�� 2x2 ä���
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                map[0][pos.row + i][pos.column + j] = 'G';
            }
        }
        resource.spice_max += 10;
        print_system_message("â�� �Ǽ� �Ϸ�! �����̽� ���差�� 10 �����߽��ϴ�.");
        break;

    case BD_BARRACKS:
        cost = 5;
        // ������ 'B'�� 2x2 ä���
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                map[0][pos.row + i][pos.column + j] = 'B';
            }
        }
        print_system_message("���� �Ǽ� �Ϸ�! ���� ������ ������ �� �ֽ��ϴ�.");
        break;

    case BD_SHELTER:
        cost = 6;
        // ����ó�� 'S'�� 2x2 ä���
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                map[0][pos.row + i][pos.column + j] = 'S';
            }
        }
        print_system_message("����ó �Ǽ� �Ϸ�! ���� �������� ������ �� �ֽ��ϴ�.");
        break;
    }

    // �ڿ� �Һ�
    resource.spice -= cost;
}

bool create_new_unit(char type, POSITION pos, bool is_ally) {
    if (unit_count >= resource.population_max) {
        return false;  // �ִ� ���� �� �ʰ�
    }

    int unit_supply_cost = 0;
    switch (type) {
    case 'H':  // Harvester
        unit_supply_cost = 5;
        break;
    case 'F':  // Fremen
        unit_supply_cost = 6;
        break;
    case 'S':  // Soldier
        unit_supply_cost = 4;
        break;
    default:
        return false;
    }

    if (resource.population + unit_supply_cost > resource.population_max) {
        return false;  // �α� �� �ʰ�
    }

    units[unit_count].pos1 = pos;
    units[unit_count].repr = type;
    units[unit_count].layer = 1;

    map[units[unit_count].layer][pos.row][pos.column] = type;

    resource.population += unit_supply_cost;
    unit_count++;

    return true;
}

// ���� ���� �Լ� �߰�
void remove_unit_at(POSITION pos) {
    for (int i = 0; i < unit_count; i++) {
        if (units[i].pos1.row == pos.row && units[i].pos1.column == pos.column) {
            // �Ʊ� �Ϻ����� ��ġ üũ ����
            // �Ʊ� ���̽� �ֺ����� ����� �Ϻ����͵� ����
            bool is_ally_unit = (units[i].pos1.row <= ally_base.pos4.row + 1 &&
                units[i].pos1.column <= ally_base.pos4.column + 3);

            // �Ʊ� ������ ��쿡�� �α��� ����
            if (is_ally_unit) {
                if (units[i].repr == 'H') resource.population -= 5;
                else if (units[i].repr == 'F') resource.population -= 6;
                else if (units[i].repr == 'S') resource.population -= 4;
            }

            // �ʿ��� ���� ����
            map[units[i].layer][pos.row][pos.column] = -1;

            // �迭���� ���� ���� (������ ������ ���� ��ġ�� �̵�)
            if (i < unit_count - 1) {
                units[i] = units[unit_count - 1];
            }
            unit_count--;
            break;
        }
    }
}