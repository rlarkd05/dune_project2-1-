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
void init_harvester(POSITION pos, bool is_ally);
void update_harvester(HARVESTER* h);
void process_harvester_command(HARVESTER* h, KEY key, POSITION target);
void print_harvester_info(HARVESTER* h);
bool is_spice_deposit(POSITION pos);
bool is_occupied_by_harvester(POSITION pos);
bool is_at_base(HARVESTER* h);
void move_harvester(HARVESTER* h);
void update_all_harvesters(void);
POSITION get_spice_position(bool is_ally);

/* ================= control =================== */
int sys_clock = 0;
CURSOR cursor = { { 1, 1 }, {1, 1} };
int last_key_time = 0;
// ���� ���� �߰� (���� �������� ���� �κп� �߰�)
UNIT units[10];  // population_max�� �ִ밪(10)��ŭ �迭 ũ�� ����
int unit_count = 0;
//�Ϻ����� �������� �߰�
HARVESTER harvesters[MAX_HARVESTERS];
int harvester_count = 0;
bool harvester_selected = false;

/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };


SELECTED_BUILDING selected_building = {
    .type = ' ',
    .position = {0, 0},
    .is_selected = false,
    .is_ally = false
};

RESOURCE resource = {
   .spice = 40,
   .spice_max = 100,
   .population = 0,
   .population_max = 5
};

//������Ʈ ���� ����� �� ����
OBJECT_SAMPLE obj = {
   .pos = {3, 10},
   .dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
   .repr = 'W',
   .speed = 2000,
   .next_move_time = 2000
};

OBJECT_SAMPLE1 obj1 = {
    .pos = {16, 50},
    .dest = {(MAP_HEIGHT + 2) - MAP_HEIGHT, (MAP_WIDTH + 2) - MAP_HEIGHT},
    .repr = 'W',
    .speed = 2000,
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
            case k_b:  // �Ǽ� �޴�
                process_command(key);
                break;

            case k_space:  // �����̽��� - ���� �Ǵ� ��ġ Ȯ��
                if (building_state.is_building_mode) {
                    process_command(key);
                }
                else {
                    // �Ϻ����Ͱ� ���õ� ���¿��� �����̽��� (��ġ Ȯ��)
                    bool harvester_command_processed = false;
                    if (harvester_selected) {
                        for (int i = 0; i < harvester_count; i++) {
                            if (harvesters[i].is_selected) {
                                process_harvester_command(&harvesters[i], key, cursor.current);
                                harvester_command_processed = true;
                                break;
                            }
                        }
                    }

                    // �Ϻ����� ����� ó������ �ʾ����� �ǹ�/���� ����
                    if (!harvester_command_processed) {
                        clean_status();
                        select_building();
                    }
                }
                break;

            case k_m:  // �̵� ���
                if (harvester_selected) {
                    for (int i = 0; i < harvester_count; i++) {
                        if (harvesters[i].is_selected) {
                            process_harvester_command(&harvesters[i], k_m, cursor.current);
                            print_system_message("�̵��� ��ġ�� �����ϰ� �����̽��ٸ� �����ּ���");
                            break;
                        }
                    }
                }
                break;

            case k_h:  // ��Ȯ ���
                if (harvester_selected) {
                    for (int i = 0; i < harvester_count; i++) {
                        if (harvesters[i].is_selected) {
                            process_harvester_command(&harvesters[i], k_h, cursor.current);
                            break;
                        }
                    }
                }
                else if (selected_building.type == 'B' && selected_building.is_ally) {
                    process_command(key);  // �Ϻ����� ����
                }
                break;

            case k_p:  // ����
            case k_d:  // ����
            case k_g:  // â��
            case k_s:  // ����ó
                if (building_state.is_building_mode) {
                    process_command(key);
                }
                break;

            case k_esc:  // ���� ���
                if (harvester_selected) {
                    for (int i = 0; i < harvester_count; i++) {
                        if (harvesters[i].is_selected) {
                            harvesters[i].is_selected = false;
                            harvester_selected = false;
                            clean_status();
                            print_command_message("B: �Ǽ�");
                            break;
                        }
                    }
                }
                else if (selected_building.is_selected || building_state.is_building_mode) {
                    process_command(key);
                }
                break;

            case k_quit:
                outro();
                break;
            }
        }

        update_all_harvesters();
        sandworm_move();
        sandworm1_move();
        display(resource, map, cursor);
        Sleep(TICK);
        sys_clock += 10;
    }
    return 0;
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
    POSITION ally_pos = { 14, 1 };
    init_harvester(ally_pos, true);
    resource.population += 5;

    // �ʱ� �Ϻ����� ���� (����)
    POSITION enemy_pos = { 3, 58 };
    init_harvester(enemy_pos, false);
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

    // �Ϻ����Ͱ� �ִ� ���
    if (map[1][next_pos.row][next_pos.column] == 'H') {
        // units �迭������ ����
        remove_unit_at(next_pos);
    }

    // �����̽� ���� �õ�
    spawn_spice(obj.pos);  // ���� ��ġ�� �����̽� ���� �õ�

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

    // �Ϻ����Ͱ� �ִ� ���
    if (map[1][next_pos.row][next_pos.column] == 'H') {
        // units �迭������ ����
        remove_unit_at(next_pos);
    }

    // �����̽� ���� �õ�
    spawn_spice(obj1.pos);  // ���� ��ġ�� �����̽� ���� �õ�

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
    else if (terrain >= '1' && terrain <= '8') {  // 1~8���� ��� �����̽��� ó��
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
    else if (terrain >= '1' && terrain <= '8') {  // 1~8���� ��� �����̽��� ó��
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
    harvester_selected = false;  // �Ϻ����� ���� ���� �ʱ�ȭ

    if (unit == 'H') {
        for (int i = 0; i < harvester_count; i++) {
            if (harvesters[i].pos.row == cursor.current.row &&
                harvesters[i].pos.column == cursor.current.column) {
                harvesters[i].is_selected = true;
                harvester_selected = true;
                print_harvester_info(&harvesters[i]);
                if (harvesters[i].is_ally) {
                    print_system_message("�Ʊ� �Ϻ����͸� �����߽��ϴ�.");
                    print_command_message("M: �̵� | H: ��Ȯ | ESC: ���");
                }
                else {
                    print_system_message("���� �Ϻ����͸� �����߽��ϴ�.");
                }
                return;
            }
        }
    }
}
void print_unit_info(void) {
    POSITION pos;
    char unit = map[1][cursor.current.row][cursor.current.column];

    if (unit != -1) {
        // ���� ���
        pos.row = 2;
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

                print_command_message("M: �̵� | H: ��Ȯ | ESC: ���");
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
    if (rand() % 100 < 15 && map[0][pos.row][pos.column] == ' ') {  // 15% Ȯ��
        // 2���� 8 ������ ������ ���ڸ� ���ڷ� ��ȯ
        char spice_amount = '0' + (2 + (rand() % 7));  // 2~8 ���� ����
        map[0][pos.row][pos.column] = spice_amount;
    }
}
// �Ǽ� �޴� ó�� �Լ� ����
void process_command(KEY key) {
    if (!selected_building.is_selected && !building_state.is_building_mode) {
        if (key == k_b) {
            building_state.is_building_mode = true;
            building_state.cursor_size = 1;
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
        switch (key) {
            // ���� ��ġ�� �ʱ� �Ϻ����� ��ó�� ����
        case k_h:  // �Ϻ����� ����
            if (resource.spice < 5) {
                print_system_message("�����̽��� �����մϴ�!");
            }
            else if (resource.population + 5 > resource.population_max) {
                print_system_message("�α� �� ������ �ʰ��߽��ϴ�!");
            }
            else if (harvester_count >= MAX_HARVESTERS) {
                print_system_message("�� �̻� �Ϻ����͸� ������ �� �����ϴ�!");
            }
            else {
                // ���� ��ġ ���� (�ʱ� �Ϻ����� ��ó)
                POSITION new_pos = {
                    Harverster.pos1.row,  // �ʱ� �Ϻ����Ϳ� ���� row
                    Harverster.pos1.column + 1  // �ʱ� �Ϻ����� ���� ����
                };

                init_harvester(new_pos, true);
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

// ���� ���� �Լ�
void remove_unit_at(POSITION pos) {
    // �Ϻ����� �迭���� �ش� ��ġ�� �Ϻ����� ã�Ƽ� ����
    for (int i = 0; i < harvester_count; i++) {
        if (harvesters[i].pos.row == pos.row &&
            harvesters[i].pos.column == pos.column) {
            // �Ʊ� �Ϻ������� ��� �α��� ����
            if (harvesters[i].is_ally) {
                resource.population -= 5;
            }

            // �Ϻ������� ���� �ʱ�ȭ
            harvesters[i].state = H_IDLE;
            harvesters[i].is_selected = false;
            harvesters[i].auto_harvest = false;
            harvesters[i].spice_carried = 0;

            // �Ϻ����� �迭���� ���� (������ �Ϻ����͸� ���� ��ġ�� �̵�)
            if (i < harvester_count - 1) {
                harvesters[i] = harvesters[harvester_count - 1];
            }
            harvester_count--;

            // ���� ���¿��ٸ� ���� ���� ���µ� ����
            if (harvester_selected) {
                harvester_selected = false;
                clean_status();
                print_command_message("B: �Ǽ�");
            }
            break;
        }
    }

    // units �迭������ ����
    for (int i = 0; i < unit_count; i++) {
        if (units[i].pos1.row == pos.row && units[i].pos1.column == pos.column) {
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
void init_harvester(POSITION pos, bool is_ally) {
    if (harvester_count >= MAX_HARVESTERS) return;

    HARVESTER* h = &harvesters[harvester_count];
    h->pos = pos;
    h->target_pos = pos;
    h->state = H_IDLE;
    h->health = 100;
    h->spice_carried = 0;
    h->harvest_start_time = 0;
    h->is_selected = false;
    h->is_ally = is_ally;
    h->speed = 500;           // ��������� �� ������ ����
    h->next_move_time = 500;  // �ʱ� �̵� �ð�

    map[1][pos.row][pos.column] = 'H';
    harvester_count++;
}

// �Ϻ����� ��� ó��
void process_harvester_command(HARVESTER* h, KEY key, POSITION target) {
    if (!h->is_selected) return;

    switch (key) {
    case k_m:  // �̵� ���
        h->state = H_WAIT_MOVE_POS;
        h->auto_harvest = false;  // �ڵ� ��Ȯ ��� ����
        print_system_message("�̵��� ��ġ�� �����ϰ� �����̽��ٸ� �����ּ���.");
        break;

    case k_h:  // ��Ȯ ���
        if (!is_at_base(h)) {  // ������ �ƴ� ���� ��Ȯ ��� ����
            h->state = H_WAIT_HARVEST_POS;
            print_system_message("��Ȯ�� �����̽��� �����ϰ� �����̽��ٸ� �����ּ���.");
        }
        else {
            h->state = H_WAIT_HARVEST_POS;  // ������ �־ ��Ȯ ��� �����ϵ���
            print_system_message("��Ȯ�� �����̽��� �����ϰ� �����̽��ٸ� �����ּ���.");
        }
        break;

    case k_space:  // ��ġ Ȯ��
        if (h->state == H_WAIT_MOVE_POS) {
            h->target_pos = cursor.current;
            h->state = H_MOVING;
            print_system_message("������ ��ġ�� �̵��մϴ�.");
        }
        else if (h->state == H_WAIT_HARVEST_POS) {
            // ... (������ �����̽��� ó�� �ڵ�� �״��)
            if (is_spice_deposit(cursor.current)) {
                h->harvest_pos = cursor.current;
                const int dx[] = { 0, 0, -1, 1 };
                const int dy[] = { -1, 1, 0, 0 };
                bool found_spot = false;

                for (int i = 0; i < 4; i++) {
                    int new_row = cursor.current.row + dy[i];
                    int new_col = cursor.current.column + dx[i];

                    if (new_row < 0 || new_row >= MAP_HEIGHT ||
                        new_col < 0 || new_col >= MAP_WIDTH) continue;

                    if (map[0][new_row][new_col] == ' ' && map[1][new_row][new_col] == -1) {
                        h->target_pos.row = new_row;
                        h->target_pos.column = new_col;
                        found_spot = true;
                        break;
                    }
                }

                if (found_spot) {
                    h->state = H_MOVING;
                    h->auto_harvest = true;
                    print_system_message("�����̽� ������ ��ó�� �̵��մϴ�.");
                }
                else {
                    print_system_message("�����̽� ��ó�� ������ �� �ִ� ������ �����ϴ�.");
                }
            }
            else {
                print_system_message("�����̽� �������� �������ּ���.");
            }
        }
        break;

    case k_esc:
        h->is_selected = false;
        harvester_selected = false;
        clean_status();
        print_command_message("B: �Ǽ�");
        break;
    }
}

// �Ϻ����� ���� ������Ʈ �Լ�
void update_harvester(HARVESTER* h) {
    switch (h->state) {
    case H_MOVING:
    {
        bool near_spice = false;
        if (h->auto_harvest) {
            if ((h->pos.row + 1 == h->harvest_pos.row && h->pos.column == h->harvest_pos.column) ||
                (h->pos.row - 1 == h->harvest_pos.row && h->pos.column == h->harvest_pos.column) ||
                (h->pos.row == h->harvest_pos.row && h->pos.column + 1 == h->harvest_pos.column) ||
                (h->pos.row == h->harvest_pos.row && h->pos.column - 1 == h->harvest_pos.column)) {
                near_spice = true;
            }
        }

        if (near_spice) {
            h->state = H_HARVESTING;
            h->harvest_start_time = sys_clock;
            print_system_message("�����̽� ��Ȯ�� �����մϴ�...");  // ��Ȯ ������ ���� �޽��� ���
        }
        else if (h->pos.row == h->target_pos.row && h->pos.column == h->target_pos.column) {
            h->state = H_IDLE;
            print_system_message("�̵��� �Ϸ�Ǿ����ϴ�.");
        }
        else {
            if (sys_clock > h->next_move_time) {
                move_harvester(h);
            }
        }
    }
    break;

    case H_HARVESTING:
        // ��Ȯ ���� ���� �޽����� ������� ����
        if (sys_clock - h->harvest_start_time >= HARVEST_TIME) {
            int current_spice = map[0][h->harvest_pos.row][h->harvest_pos.column] - '0';
            int intended_harvest = 2 + (rand() % 3);
            h->spice_carried = min(current_spice, intended_harvest);

            if (current_spice <= h->spice_carried) {
                map[0][h->harvest_pos.row][h->harvest_pos.column] = ' ';
                print_system_message("���� �����̽��� ��� ��Ȯ�߽��ϴ�!");
            }
            else {
                map[0][h->harvest_pos.row][h->harvest_pos.column] = '0' + (current_spice - h->spice_carried);
                if (h->spice_carried == 2) print_system_message("��Ȯ �Ϸ�! 2 �����̽��� ȹ���߽��ϴ�.");
                if (h->spice_carried == 3) print_system_message("��Ȯ �Ϸ�! 3 �����̽��� ȹ���߽��ϴ�.");
                if (h->spice_carried == 4) print_system_message("��Ȯ �Ϸ�! 4 �����̽��� ȹ���߽��ϴ�.");
            }

            h->state = H_RETURNING;

            if (h->is_ally) {
                h->target_pos = ally_base.pos1;
            }
            else {
                h->target_pos = enemy_base.pos1;
            }
        }
        break;

        // update_harvester �Լ��� H_RETURNING case �κ� ����
    case H_RETURNING:
    {
        bool near_base = false;
        if (h->is_ally) {
            // ���� ���� �� ĭ���� ����
            near_base = (h->pos.row == ally_base.pos1.row - 1 &&
                h->pos.column >= ally_base.pos1.column &&
                h->pos.column <= ally_base.pos4.column);

            // ��ǥ ��ġ�� ���� �ٷ� ���� ����
            if (h->target_pos.row == ally_base.pos1.row &&
                h->target_pos.column == ally_base.pos1.column) {
                h->target_pos.row = ally_base.pos1.row - 1;
            }
        }
        else {
            // ���� ���� ���� �� ĭ���� ����
            near_base = (h->pos.row == enemy_base.pos1.row - 1 &&
                h->pos.column >= enemy_base.pos1.column &&
                h->pos.column <= enemy_base.pos4.column);

            // ��ǥ ��ġ�� ���� �ٷ� ���� ����
            if (h->target_pos.row == enemy_base.pos1.row &&
                h->target_pos.column == enemy_base.pos1.column) {
                h->target_pos.row = enemy_base.pos1.row - 1;
            }
        }

        if (near_base) {
            if (h->is_ally) {
                int space_available = resource.spice_max - resource.spice;
                int delivered = min(space_available, h->spice_carried);
                resource.spice += delivered;
                if (delivered < h->spice_carried) {
                    print_system_message("â�� ���� ���� �Ϻ� �����̽��� ���Ƚ��ϴ�.");
                }
                else {
                    if (delivered == 2) print_system_message("2 �����̽��� â�� �����߽��ϴ�.");
                    if (delivered == 3) print_system_message("3 �����̽��� â�� �����߽��ϴ�.");
                    if (delivered == 4) print_system_message("4 �����̽��� â�� �����߽��ϴ�.");
                }
            }
            h->spice_carried = 0;

            // �����̽��� 1 �̻� ���������� ��� ��Ȯ
            if (h->auto_harvest && map[0][h->harvest_pos.row][h->harvest_pos.column] >= '1' &&
                map[0][h->harvest_pos.row][h->harvest_pos.column] <= '8') {
                h->target_pos = h->harvest_pos;  // ���� �����̽��� �ٽ� �̵�
                h->state = H_MOVING;
                print_system_message("�ٽ� �����̽� �������� �̵��մϴ�...");
            }
            else {
                h->state = H_IDLE;
                h->auto_harvest = false;  // �ڵ� ��Ȯ ��� ����
                h->target_pos = h->pos;  // ���� ��ġ�� ��ǥ ��ġ�� ���� (���ڸ��� ����)
                print_system_message("�������� ���Ǿ����ϴ�. ���� ��ġ���� ����մϴ�.");
            }
        }
        else {
            if (sys_clock > h->next_move_time) {
                move_harvester(h);
            }
        }
        break;
    }


    case H_WAIT_MOVE_POS:
    case H_WAIT_HARVEST_POS:
    case H_IDLE:
        break;
    }
}
// �Ϻ����� �̵� ó��
void move_harvester(HARVESTER* h) {
    if (sys_clock <= h->next_move_time) {
        return;
    }

    map[1][h->pos.row][h->pos.column] = -1;  // ���� ��ġ���� ����

    if (h->pos.column != h->target_pos.column) {
        h->pos.column += (h->target_pos.column > h->pos.column) ? 1 : -1;
    }
    else if (h->pos.row != h->target_pos.row) {
        h->pos.row += (h->target_pos.row > h->pos.row) ? 1 : -1;
    }

    map[1][h->pos.row][h->pos.column] = 'H';  // �� ��ġ�� ǥ��
    h->next_move_time = sys_clock + h->speed;  // ���� �̵� �ð� ����
}


// ��� �Ϻ����� ������Ʈ
void update_all_harvesters(void) {
    for (int i = 0; i < harvester_count; i++) {
        update_harvester(&harvesters[i]);
    }
}

POSITION get_spice_position(bool is_ally) {
    POSITION spice_pos;

    if (is_ally) {
        spice_pos.row = ally_spice.pos1.row;
        spice_pos.column = ally_spice.pos1.column;
    }
    else {
        spice_pos.row = enemy_spice.pos1.row;
        spice_pos.column = enemy_spice.pos1.column;
    }

    return spice_pos;
}

// �����̽� ���������� Ȯ���ϴ� �Լ�
bool is_spice_deposit(POSITION pos) {
    char terrain = map[0][pos.row][pos.column];
    return (terrain >= '1' && terrain <= '8');  // 1~8 ��� �����̽��� ó��
}
// �ٸ� �Ϻ����Ͱ� �ִ��� Ȯ���ϴ� �Լ�
bool is_occupied_by_harvester(POSITION pos) {
    for (int i = 0; i < harvester_count; i++) {
        if (harvesters[i].pos.row == pos.row &&
            harvesters[i].pos.column == pos.column) {
            return true;
        }
    }
    return false;
}

// �Ϻ����Ͱ� ������ �ִ��� Ȯ���ϴ� �Լ�
bool is_at_base(HARVESTER* h) {
    if (h->is_ally) {
        if (h->pos.row >= ally_base.pos1.row &&
            h->pos.row <= ally_base.pos4.row &&
            h->pos.column >= ally_base.pos1.column &&
            h->pos.column <= ally_base.pos4.column) {
            return true;
        }
    }
    else {
        if (h->pos.row >= enemy_base.pos1.row &&
            h->pos.row <= enemy_base.pos4.row &&
            h->pos.column >= enemy_base.pos1.column &&
            h->pos.column <= enemy_base.pos4.column) {
            return true;
        }
    }
    return false;
}

// �Ϻ����� ���� ��� �Լ�
void print_harvester_info(HARVESTER* h) {
    POSITION pos;
    pos.row = 2;
    pos.column = MAP_WIDTH + 4;

    clean_status();

    gotoxy(pos);
    printf("=== �Ϻ����� ���� ===");

    pos.row += 2;
    gotoxy(pos);
    printf("��ġ: (%d, %d)", h->pos.row, h->pos.column);

    pos.row += 2;
    gotoxy(pos);
    printf("ü��: %d/100", h->health);

    pos.row += 2;
    gotoxy(pos);
    printf("���緮: %d", h->spice_carried);

    pos.row += 2;
    gotoxy(pos);
    printf("����: ");
    switch (h->state) {
    case H_IDLE: printf("��� ��"); break;
    case H_MOVING: printf("�̵� ��"); break;
    case H_HARVESTING: printf("��Ȯ ��"); break;
    case H_RETURNING: printf("��ȯ ��"); break;
    case H_WAIT_MOVE_POS: printf("�̵� ��ġ ��� ��"); break;
    case H_WAIT_HARVEST_POS: printf("��Ȯ ��ġ ��� ��"); break;
    }

    print_command_message("M: �̵� | H: ��Ȯ | ESC: ���");
}