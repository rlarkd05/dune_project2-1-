#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void intro(void);
void Construction(void);
void Biome(void);
void unit(void);
void outro(void);
void cursor_move(DIRECTION dir);
void cursor_double_move(DIRECTION dir, int times);
void sample_obj_move(void);
void sample_obj1_move(void);
void StatusWindow(void);
void system_message(void);
void command_message(void);
void print_terrain(void);
void clean_status(void);
void select_building(void);
void process_command(KEY key);
void print_command_message(const char* message);
void print_system_message(const char* message);
bool can_produce_harvester(void);
POSITION sample_obj_next_position(void);
POSITION find_nearest_unit(POSITION current_pos);
void spawn_spice(POSITION pos);


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
   .spice = 0,
   .spice_max = 0,
   .population = 0,
   .population_max = 5
};

//������Ʈ ���� ����� �� ����
OBJECT_SAMPLE obj = {
   .pos = {5, 5},
   .dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
   .repr = 'W',
   .speed = 600,
   .next_move_time = 600
};

OBJECT_SAMPLE1 obj1 = {
    .pos = {15, 50},
    .dest = {(MAP_HEIGHT + 2) - MAP_HEIGHT,(MAP_WIDTH + 2) - MAP_HEIGHT},
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
        sample_obj_move();
        sample_obj1_move();

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

    // ���� ������Ʈ �ʱ�ȭ - Layer 1�� ��ġ
    obj.pos.row = 1;
    obj.pos.column = 1;
    map[1][obj.pos.row][obj.pos.column] = obj.repr;  // 'o' ���� ��ġ
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
    // �޽��� â ���̿� ���� ��ġ ����
    POSITION pos;
    int message_start_row = MAP_HEIGHT + 2; // �ý��� �޽��� â�� ���� ��
    int message_width = MAP_WIDTH - 1;          // �ý��� �޽��� â �ʺ�
    int message_height = 8;                 // �ý��� �޽��� â ����

    // ���� ���� �׵θ� ���
    for (int row = message_start_row; row < message_start_row + message_height; row++) {
        pos.row = row;
        pos.column = 0; // �� ���� �׵θ� ��ġ
        gotoxy(pos);
        printf("#");
    }

    // ��� ���� �׵θ� ���
    for (int col = 0; col < message_width + 1; col++) {
        pos.row = message_start_row;
        pos.column = col;
        gotoxy(pos);
        printf("#");
    }

    // �ϴ� ���� �׵θ� ���
    for (int col = 0; col < message_width + 1; col++) {
        pos.row = message_start_row + message_height - 1;
        pos.column = col;
        gotoxy(pos);
        printf("#");
    }

    // ������ ���� �׵θ� ���
    for (int row = message_start_row; row < message_start_row + message_height; row++) {
        pos.row = row;
        pos.column = message_width;
        gotoxy(pos);
        printf("#");
    }
}

//���â
void command_message() {
    POSITION pos;
    int command_start_row = MAP_HEIGHT + 2;  // ���â�� ���� �� (����â �ٷ� �Ʒ�)
    int command_width = 58;                 // ���â �ʺ� (����â �ʺ�� ����)
    int command_height = 8;                 // ���â ����

    // ���� ���� �׵θ� ���
    for (int row = command_start_row; row < command_start_row + command_height; row++) {
        pos.row = row;
        pos.column = MAP_WIDTH + 2;         // ����â �����ʰ� ��ġ�ϰ� ��ġ ����
        gotoxy(pos);
        printf("#");
    }

    // ��� ���� �׵θ� ���
    for (int col = MAP_WIDTH + 2; col < MAP_WIDTH + 2 + command_width; col++) {
        pos.row = command_start_row;
        pos.column = col;
        gotoxy(pos);
        printf("#");
    }

    // �ϴ� ���� �׵θ� ���
    for (int col = MAP_WIDTH + 2; col < MAP_WIDTH + 2 + command_width; col++) {
        pos.row = command_start_row + command_height - 1;
        pos.column = col;
        gotoxy(pos);
        printf("#");
    }

    // ������ ���� �׵θ� ���
    for (int row = command_start_row; row < command_start_row + command_height; row++) {
        pos.row = row;
        pos.column = MAP_WIDTH + 2 + command_width - 1;
        gotoxy(pos);
        printf("#");
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


POSITION sample_obj_next_position(void) {
    // ���� ����� ������ ��ġ ã��
    POSITION nearest_unit = find_nearest_unit(obj.pos);

    // ������ ������ ���� ������ ����
    if (nearest_unit.row == -1) {
        return obj.pos;
    }

    // ���� ����� ������ ���� �̵�
    POSITION diff = psub(nearest_unit, obj.pos);
    DIRECTION dir;

    // ������, ������ �Ÿ��� ���ؼ� �� �� �� ������ �̵�
    if (abs(diff.row) >= abs(diff.column)) {
        dir = (diff.row >= 0) ? d_down : d_up;
    }
    else {
        dir = (diff.column >= 0) ? d_right : d_left;
    }

    POSITION next_pos = pmove(obj.pos, dir);

    // �̵� ������ ��ġ���� Ȯ��
    if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 &&
        1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2) {

        // ������ �������� Ȯ��
        if (map[1][next_pos.row][next_pos.column] == 'H') {
            // ���� ����
            map[1][next_pos.row][next_pos.column] = -1;
            // 5% Ȯ���� �����̽� ����
            if (rand() % 100 < 5) {
                spawn_spice(next_pos);
            }
        }

        // 2% Ȯ���� �̵��ϸ鼭 �����̽� ����
        if (rand() % 100 < 30) {
            spawn_spice(obj.pos);
        }

        return next_pos;
    }

    return obj.pos;
}

// obj1(�� ��° �����)�� ���ؼ��� ������ ���� ����
POSITION sample_obj1_next_position(void) {
    POSITION nearest_unit = find_nearest_unit(obj1.pos);

    if (nearest_unit.row == -1) {
        return obj1.pos;
    }

    POSITION diff = psub(nearest_unit, obj1.pos);
    DIRECTION dir;

    if (abs(diff.row) >= abs(diff.column)) {
        dir = (diff.row >= 0) ? d_down : d_up;
    }
    else {
        dir = (diff.column >= 0) ? d_right : d_left;
    }

    POSITION next_pos = pmove(obj1.pos, dir);

    if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 &&
        1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2) {

        if (map[1][next_pos.row][next_pos.column] == 'H') {
            map[1][next_pos.row][next_pos.column] = -1;
            if (rand() % 100 < 5) {
                spawn_spice(next_pos);
            }
        }

        if (rand() % 100 < 30) {
            spawn_spice(obj1.pos);
        }

        return next_pos;
    }

    return obj1.pos;
}

void sample_obj_move(void) {
    if (sys_clock <= obj.next_move_time) {
        // ���� �ð��� �� ����
        return;
    }

    // ������Ʈ�� layer1(map[1])�� ����
    map[1][obj.pos.row][obj.pos.column] = -1;
    obj.pos = sample_obj_next_position();
    map[1][obj.pos.row][obj.pos.column] = obj.repr;

    obj.next_move_time = sys_clock + obj.speed;
}

void sample_obj1_move(void) {
    if (sys_clock <= obj1.next_move_time) {
        // ���� �ð��� �� ����
        return;
    }

    // ������Ʈ�� layer1(map[1])�� ����
    map[1][obj1.pos.row][obj1.pos.column] = -1;
    obj1.pos = sample_obj1_next_position();
    map[1][obj1.pos.row][obj1.pos.column] = obj1.repr;

    obj1.next_move_time = sys_clock + obj1.speed;
}

//� �������� ��������.
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

    //���� ����
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
            // �Ʊ� ���̽��� ��� �ڿ� ������ ǥ��
            pos.row = 8;
            pos.column = MAP_WIDTH + 4;
            gotoxy(pos);
            printf("Spice: %d/%d", resource.spice, resource.spice_max);
            pos.row = 9;
            pos.column = MAP_WIDTH + 4;
            gotoxy(pos);
            printf("Population: %d/%d", resource.population, resource.population_max);
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
    POSITION nearest = { 0, 0 }; //�ʱⰪ 0,0
    int min_distance = MAP_WIDTH * MAP_HEIGHT; // �ִ� �Ÿ��� �ʱ�ȭ

    // ��� ���� ��ȸ�ϸ鼭 ���� ã��
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            // 'H'�� �Ϻ����ͳ� ���ڳ� ������ ��Ÿ�� �׽�Ʈ�� ���ڳٰ� �Ϻ����ͷ� ��������
            if (map[1][i][j] == 'H') {
                POSITION unit_pos = { i, j }; //���� ���� ��ġ
                int distance = abs(current_pos.row - i) + abs(current_pos.column - j); //abs(����)(���� ����� ��ġ ������ - i) + (���� ����� ��ġ ������ - j) // �� �� ������ �Ÿ� ���� ���
                if (distance < min_distance && distance > 0) { // �ڱ� �ڽ� ����
                    min_distance = distance; // ������ �����Ǹ� min_distance�� distance�� �����ϰ� nearest�� unit_pos�� �����Ѵ�. (�Ÿ�����)
                    nearest = unit_pos;//�̷��� �ϸ� ���� �� ����� ������ ã���鼭 nearest�� ���� ����� ������ ��ġ�� ����ϰ� �˴ϴ�. (��ġ ����)
                }
            }
        }
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
    if (key == k_esc) {
        selected_building.is_selected = false;
        selected_building.type = ' ';
        print_command_message("");
        print_system_message("��ҵ�                                 ");
        return;
    }
    if (selected_building.type == 'B' && key == k_h) {  // k_h ���
        if (can_produce_harvester()) {  // �����̽��� �α��� üũ
            POSITION spawn_pos = { ally_base.pos1.row - 1, ally_base.pos1.column };
            if (map[1][spawn_pos.row][spawn_pos.column] == -1) {  // ���� ��ġ üũ
                map[1][spawn_pos.row][spawn_pos.column] = 'H';
                resource.spice -= 5;
                resource.population += 5;
                print_system_message("A new harvester is ready!");
            }
            else {
                print_system_message("������ ������ �� �����ϴ�.");
            }
        }
        else if (resource.spice < 5) {  // �����̽� ���� üũ
            print_system_message("�����̽��� ������� �ʽ��ϴ�!");
        }
        else {  // �α��� �ʰ� üũ
            print_system_message("�α��� ����");
        }
        return;
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
void print_command_message(const char* message) {
    POSITION pos = { MAP_HEIGHT + 3, MAP_WIDTH + 4 };
    gotoxy(pos);
    printf("                                                   ");  // ���� �޽��� �����
    gotoxy(pos);
    printf("%s", message);
}
void select_building(void) {
    char terrain = map[0][cursor.current.row][cursor.current.column];

    // ���� ���� �ʱ�ȭ
    selected_building.is_selected = false;
    selected_building.type = ' ';

    // �Ʊ� ���� ����
    if (terrain == 'B' &&
        cursor.current.row >= ally_base.pos1.row &&
        cursor.current.row <= ally_base.pos4.row &&
        cursor.current.column >= ally_base.pos1.column &&
        cursor.current.column <= ally_base.pos4.column) {

        selected_building.type = 'B';
        selected_building.position = cursor.current;
        selected_building.is_selected = true;
        selected_building.is_ally = true;

        print_command_message("=== ���̽� === H: �Ϻ����� ���� (Cost: 5) ESC: ���");
        print_system_message("���̽� ���õ�                          ");
    }
}

bool can_produce_harvester(void) {
    return (resource.spice >= 5 && resource.population + 5 <= resource.population_max);
}