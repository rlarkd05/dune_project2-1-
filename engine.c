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


/* ================= control =================== */
int sys_clock = 0;
CURSOR cursor = { { 1, 1 }, {1, 1} };
int last_key_time = 0;
#define DOUBLE_PRESS_INTERVAL 200
#define MAX_MSG_LINES 6  

/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

SELECTED_BUILDING selected_building = {
    .type = ' ',
    .position = {0, 0},
    .is_selected = false,
    .is_ally = false
};

RESOURCE resource = {
   .spice = 5,
   .spice_max = 10,
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
   .pos1 = {1, 58},
   .pos2 = {1, 57},
   .pos3 = {2, 58},
   .pos4 = {2, 57},
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
   .pos1 = {2, 55},
   .pos2 = {2, 56},
   .pos3 = {1, 55},
   .pos4 = {1, 56},
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
    display(resource, map, cursor);

    while (1) {
        KEY key = get_key();

        if (is_arrow_key(key)) {
            DIRECTION dir = ktod(key);
            cursor_move(dir);
        }
        else {
            switch (key) {
            case k_space:
                select_building();
                print_terrain();
                print_unit_info();
                break;
            case k_h:  // 'h', 'H' ��� k_h ���
                if (selected_building.is_selected) {
                    process_command(key);
                }
                break;
            case k_esc:
                if (selected_building.is_selected) {
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

        // ���� ������Ʈ ����
        sandworm_move();
        sandworm1_move();

        // ȭ�� ���
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
    // �Ʊ� ���̽�
    map[ally_base.layer][ally_base.pos1.row][ally_base.pos1.column] = ally_base.repr;
    map[ally_base.layer][ally_base.pos2.row][ally_base.pos2.column] = ally_base.repr;
    map[ally_base.layer][ally_base.pos3.row][ally_base.pos3.column] = ally_base.repr;
    map[ally_base.layer][ally_base.pos4.row][ally_base.pos4.column] = ally_base.repr;

    // ���� ���̽�
    map[enemy_base.layer][enemy_base.pos1.row][enemy_base.pos1.column] = enemy_base.repr;
    map[enemy_base.layer][enemy_base.pos2.row][enemy_base.pos2.column] = enemy_base.repr;
    map[enemy_base.layer][enemy_base.pos3.row][enemy_base.pos3.column] = enemy_base.repr;
    map[enemy_base.layer][enemy_base.pos4.row][enemy_base.pos4.column] = enemy_base.repr;

    // �Ʊ� ����
    map[ally_pad.layer][ally_pad.pos1.row][ally_pad.pos1.column] = ally_pad.repr;
    map[ally_pad.layer][ally_pad.pos2.row][ally_pad.pos2.column] = ally_pad.repr;
    map[ally_pad.layer][ally_pad.pos3.row][ally_pad.pos3.column] = ally_pad.repr;
    map[ally_pad.layer][ally_pad.pos4.row][ally_pad.pos4.column] = ally_pad.repr;

    //���� ����
    map[enemy_pad.layer][enemy_pad.pos1.row][enemy_pad.pos1.column] = enemy_pad.repr;
    map[enemy_pad.layer][enemy_pad.pos2.row][enemy_pad.pos2.column] = enemy_pad.repr;
    map[enemy_pad.layer][enemy_pad.pos3.row][enemy_pad.pos3.column] = enemy_pad.repr;
    map[enemy_pad.layer][enemy_pad.pos4.row][enemy_pad.pos4.column] = enemy_pad.repr;

    //�Ʊ� �����̽�
    map[ally_spice.layer][ally_spice.pos1.row][ally_spice.pos1.column] = ally_spice.repr;

    //���� �����̽�
    map[enemy_spice.layer][enemy_spice.pos1.row][enemy_spice.pos1.column] = enemy_spice.repr;


    // �� �����̳� �߰� ������Ʈ�� ������ ������� �߰� ����
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
    //�Ʊ� �Ϻ�����
    map[Harverster.layer][Harverster.pos1.row][Harverster.pos1.column] = Harverster.repr;

    //���� ���ڳ�
    map[Haconen.layer][Haconen.pos2.row][Haconen.pos2.column] = Haconen.repr;
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

    // ���� �ð����� ���� �Է� ���θ� Ȯ��
    int time_diff = sys_clock - last_key_time;
    last_key_time = sys_clock; // Update last key time

    // ���� �Է� �� �̵� �Ÿ� ����
    int move_distance = (time_diff < DOUBLE_PRESS_INTERVAL) ? 2 : 1; // 2ĭ �̵� �Ǵ� 1ĭ �̵�

    for (int i = 0; i < move_distance; i++) {
        // validation check (���� ��ȿ�� ���� �������� �̵�)
        if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
            1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

            // ���� ��ġ�� ���� ��ġ�� ������Ʈ
            cursor.previous = cursor.current; //������ ��ġ ����
            cursor.current = new_pos; // ���� ��ġ �������

            // ���ο� ��ġ�� �̵�
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

    // �Ϻ����͸� �������� üũ
    if (map[1][next_pos.row][next_pos.column] == 'H') {
        map[1][next_pos.row][next_pos.column] = -1;
    }

    // 30% Ȯ���� �����̽� ����
    if (rand() % 100 < 30) {
        spawn_spice(obj.pos);
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

    // �Ϻ����͸� �������� üũ
    if (map[1][next_pos.row][next_pos.column] == 'H') {
        map[1][next_pos.row][next_pos.column] = -1;
    }

    // 30% Ȯ���� �����̽� ����
    if (rand() % 100 < 30) {
        spawn_spice(obj1.pos);
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
    // ����â ���� �����w
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

void process_command(KEY key) {
    if (!selected_building.is_selected || !selected_building.is_ally) {
        return;
    }

    //escŰ�� ��������
    if (key == k_esc) {
        selected_building.is_selected = false;
        selected_building.type = ' ';
        print_command_message("                                                 ");  // ���â ����
        print_system_message("������ ��ҵǾ����ϴ�");
        clean_status();  // ����â�� ����
        return;
    }

    if (selected_building.type == 'B') {
        if (key == k_h) {
            if (can_produce_harvester()) {
                map[1][ally_base.pos2.row - 1][ally_base.pos2.column] = 'H';
                resource.spice -= 5; //�����̽� ������� += 5
                resource.population += 5; //�α����� += 5
                print_system_message("�Ϻ����Ͱ� ����Ǿ����ϴ�!");
            }
            else {
                print_system_message("�����̽��� �����մϴ�!");
            }
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

        print_command_message("=����= H:�Ϻ����� ���� (�����̽� 5 �ʿ�) | ESC:���");
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