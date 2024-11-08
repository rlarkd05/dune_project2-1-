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

//오브젝트 샘플 샌드웜 될 예정
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

//아군베이스
OBJECT_BUILDING ally_base = {
   .pos1 = {15, 1},
   .pos2 = {15, 2},
   .pos3 = {16, 1},
   .pos4 = {16, 2},
   .repr = 'B',
   .layer = 0
};

//적베이스
OBJECT_BUILDING enemy_base = {
   .pos1 = {1, 58},
   .pos2 = {1, 57},
   .pos3 = {2, 58},
   .pos4 = {2, 57},
   .repr = 'B',
   .layer = 0
};

//아군 장판
OBJECT_BUILDING ally_pad = {
   .pos1 = {15, 3},
   .pos2 = {15, 4},
   .pos3 = {16, 3},
   .pos4 = {16, 4},
   .repr = 'P',
   .layer = 0
};

//적군 장판
OBJECT_BUILDING enemy_pad = {
   .pos1 = {2, 55},
   .pos2 = {2, 56},
   .pos3 = {1, 55},
   .pos4 = {1, 56},
   .repr = 'P',  // E는 적군 장판을 나타내는 문자
   .layer = 0
};

//아군 스파이스
OBJECT_BUILDING ally_spice = {
   .pos1 = {13, 1},  // 아군 스파이스 위치
   .repr = '5',      // 스파이스를 나타내는 문자
   .layer = 0        // 첫 번째 레이어
};

OBJECT_BUILDING enemy_spice = {
   .pos1 = {4, 58},
   .repr = '5', // 스파이스를 나타내는 문자
   .layer = 0   // 첫 번째 레이어
};

// 바위 구조체 정의
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
   .pos1 = {11, 5},  // 하나의 위치만 있는 바위도 구조체로 정의 가능
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
            case k_h:  // 'h', 'H' 대신 k_h 사용
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

        // 샘플 오브젝트 동작
        sample_obj_move();
        sample_obj1_move();

        // 화면 출력
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
    // 아군 베이스
    map[ally_base.layer][ally_base.pos1.row][ally_base.pos1.column] = ally_base.repr;
    map[ally_base.layer][ally_base.pos2.row][ally_base.pos2.column] = ally_base.repr;
    map[ally_base.layer][ally_base.pos3.row][ally_base.pos3.column] = ally_base.repr;
    map[ally_base.layer][ally_base.pos4.row][ally_base.pos4.column] = ally_base.repr;

    // 적군 베이스
    map[enemy_base.layer][enemy_base.pos1.row][enemy_base.pos1.column] = enemy_base.repr;
    map[enemy_base.layer][enemy_base.pos2.row][enemy_base.pos2.column] = enemy_base.repr;
    map[enemy_base.layer][enemy_base.pos3.row][enemy_base.pos3.column] = enemy_base.repr;
    map[enemy_base.layer][enemy_base.pos4.row][enemy_base.pos4.column] = enemy_base.repr;

    // 아군 장판
    map[ally_pad.layer][ally_pad.pos1.row][ally_pad.pos1.column] = ally_pad.repr;
    map[ally_pad.layer][ally_pad.pos2.row][ally_pad.pos2.column] = ally_pad.repr;
    map[ally_pad.layer][ally_pad.pos3.row][ally_pad.pos3.column] = ally_pad.repr;
    map[ally_pad.layer][ally_pad.pos4.row][ally_pad.pos4.column] = ally_pad.repr;

    //적군 장판
    map[enemy_pad.layer][enemy_pad.pos1.row][enemy_pad.pos1.column] = enemy_pad.repr;
    map[enemy_pad.layer][enemy_pad.pos2.row][enemy_pad.pos2.column] = enemy_pad.repr;
    map[enemy_pad.layer][enemy_pad.pos3.row][enemy_pad.pos3.column] = enemy_pad.repr;
    map[enemy_pad.layer][enemy_pad.pos4.row][enemy_pad.pos4.column] = enemy_pad.repr;

    //아군 스파이스
    map[ally_spice.layer][ally_spice.pos1.row][ally_spice.pos1.column] = ally_spice.repr;

    //적군 스파이스
    map[enemy_spice.layer][enemy_spice.pos1.row][enemy_spice.pos1.column] = enemy_spice.repr;


    // 적 장판이나 추가 오브젝트도 동일한 방식으로 추가 가능
}

void Biome(void) {
    // 바위 1 추가
    map[ROCK_1.layer][ROCK_1.pos1.row][ROCK_1.pos1.column] = ROCK_1.repr;
    map[ROCK_1.layer][ROCK_1.pos2.row][ROCK_1.pos2.column] = ROCK_1.repr;
    map[ROCK_1.layer][ROCK_1.pos3.row][ROCK_1.pos3.column] = ROCK_1.repr;
    map[ROCK_1.layer][ROCK_1.pos4.row][ROCK_1.pos4.column] = ROCK_1.repr;

    // 바위 2 추가
    map[ROCK_2.layer][ROCK_2.pos1.row][ROCK_2.pos1.column] = ROCK_2.repr;
    map[ROCK_2.layer][ROCK_2.pos2.row][ROCK_2.pos2.column] = ROCK_2.repr;
    map[ROCK_2.layer][ROCK_2.pos3.row][ROCK_2.pos3.column] = ROCK_2.repr;
    map[ROCK_2.layer][ROCK_2.pos4.row][ROCK_2.pos4.column] = ROCK_2.repr;

    // 바위 3 추가
    map[ROCK_3.layer][ROCK_3.pos1.row][ROCK_3.pos1.column] = ROCK_3.repr;

    // 바위 4 추가
    map[ROCK_4.layer][ROCK_4.pos1.row][ROCK_4.pos1.column] = ROCK_4.repr;

    // 바위 5 추가
    map[ROCK_5.layer][ROCK_5.pos1.row][ROCK_5.pos1.column] = ROCK_5.repr;
}

void unit(void) {
    //아군 하베스터
    map[Harverster.layer][Harverster.pos1.row][Harverster.pos1.column] = Harverster.repr;

    //적군 하코넨
    map[Haconen.layer][Haconen.pos2.row][Haconen.pos2.column] = Haconen.repr;
}

//#으로 맵 틀 만듬
void init(void) {
    // layer 0(map[0])에 지형 생성
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

    // layer 1(map[1]) 초기화 - 모든 위치를 -1로 설정
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            map[1][i][j] = -1;
        }
    }

    // 샘플 오브젝트 초기화 - Layer 1에 배치
    obj.pos.row = 1;
    obj.pos.column = 1;
    map[1][obj.pos.row][obj.pos.column] = obj.repr;  // 'o' 문자 배치
}

//상태창
void StatusWindow() {
    POSITION pos;

    // 왼쪽 세로 테두리 출력
    for (int row = 1; row < MAP_HEIGHT + 1; row++) {
        pos.row = row;
        pos.column = MAP_WIDTH + 2; // 맵 오른쪽에 상태창 위치 설정
        gotoxy(pos);
        printf("#");
    }

    // 상단 가로 테두리 출력
    for (int col = MAP_WIDTH + 2; col < MAP_WIDTH + 60; col++) {
        pos.row = 1;
        pos.column = col;
        gotoxy(pos);
        printf("#");
    }

    // 하단 가로 테두리 출력
    for (int col = MAP_WIDTH + 2; col < MAP_WIDTH + 60; col++) {
        pos.row = MAP_HEIGHT;
        pos.column = col;
        gotoxy(pos);
        printf("#");
    }

    // 오른쪽 세로 테두리 출력
    for (int row = 1; row < MAP_HEIGHT + 1; row++) {
        pos.row = row;
        pos.column = MAP_WIDTH + 59; // 오른쪽 벽 위치
        gotoxy(pos);
        printf("#");
    }
}

//시스템 메시지
void system_message() {
    // 메시지 창 높이와 시작 위치 설정
    POSITION pos;
    int message_start_row = MAP_HEIGHT + 2; // 시스템 메시지 창의 시작 행
    int message_width = MAP_WIDTH - 1;          // 시스템 메시지 창 너비
    int message_height = 8;                 // 시스템 메시지 창 높이

    // 왼쪽 세로 테두리 출력
    for (int row = message_start_row; row < message_start_row + message_height; row++) {
        pos.row = row;
        pos.column = 0; // 맨 왼쪽 테두리 위치
        gotoxy(pos);
        printf("#");
    }

    // 상단 가로 테두리 출력
    for (int col = 0; col < message_width + 1; col++) {
        pos.row = message_start_row;
        pos.column = col;
        gotoxy(pos);
        printf("#");
    }

    // 하단 가로 테두리 출력
    for (int col = 0; col < message_width + 1; col++) {
        pos.row = message_start_row + message_height - 1;
        pos.column = col;
        gotoxy(pos);
        printf("#");
    }

    // 오른쪽 세로 테두리 출력
    for (int row = message_start_row; row < message_start_row + message_height; row++) {
        pos.row = row;
        pos.column = message_width;
        gotoxy(pos);
        printf("#");
    }
}

//명령창
void command_message() {
    POSITION pos;
    int command_start_row = MAP_HEIGHT + 2;  // 명령창의 시작 행 (상태창 바로 아래)
    int command_width = 58;                 // 명령창 너비 (상태창 너비와 동일)
    int command_height = 8;                 // 명령창 높이

    // 왼쪽 세로 테두리 출력
    for (int row = command_start_row; row < command_start_row + command_height; row++) {
        pos.row = row;
        pos.column = MAP_WIDTH + 2;         // 상태창 오른쪽과 일치하게 위치 설정
        gotoxy(pos);
        printf("#");
    }

    // 상단 가로 테두리 출력
    for (int col = MAP_WIDTH + 2; col < MAP_WIDTH + 2 + command_width; col++) {
        pos.row = command_start_row;
        pos.column = col;
        gotoxy(pos);
        printf("#");
    }

    // 하단 가로 테두리 출력
    for (int col = MAP_WIDTH + 2; col < MAP_WIDTH + 2 + command_width; col++) {
        pos.row = command_start_row + command_height - 1;
        pos.column = col;
        gotoxy(pos);
        printf("#");
    }

    // 오른쪽 세로 테두리 출력
    for (int row = command_start_row; row < command_start_row + command_height; row++) {
        pos.row = row;
        pos.column = MAP_WIDTH + 2 + command_width - 1;
        gotoxy(pos);
        printf("#");
    }
}

// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir) {
    POSITION curr = cursor.current;
    POSITION new_pos = pmove(curr, dir);

    // 현재 시간에서 연속 입력 여부를 확인
    int time_diff = sys_clock - last_key_time;
    last_key_time = sys_clock; // Update last key time

    // 연속 입력 시 이동 거리 결정
    int move_distance = (time_diff < DOUBLE_PRESS_INTERVAL) ? 2 : 1; // 2칸 이동 또는 1칸 이동

    for (int i = 0; i < move_distance; i++) {
        // validation check (맵의 유효한 영역 내에서만 이동)
        if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
            1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

            // 현재 위치를 이전 위치로 업데이트
            cursor.previous = cursor.current; //이전의 위치 갱신
            cursor.current = new_pos; // 현재 위치 업데잍느

            // 새로운 위치로 이동
            new_pos = pmove(new_pos, dir);
        }
        else {
            break;
        }
    }
}


POSITION sample_obj_next_position(void) {
    // 가장 가까운 유닛의 위치 찾기
    POSITION nearest_unit = find_nearest_unit(obj.pos);

    // 유닛이 없으면 기존 움직임 유지
    if (nearest_unit.row == -1) {
        return obj.pos;
    }

    // 가장 가까운 유닛을 향해 이동
    POSITION diff = psub(nearest_unit, obj.pos);
    DIRECTION dir;

    // 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
    if (abs(diff.row) >= abs(diff.column)) {
        dir = (diff.row >= 0) ? d_down : d_up;
    }
    else {
        dir = (diff.column >= 0) ? d_right : d_left;
    }

    POSITION next_pos = pmove(obj.pos, dir);

    // 이동 가능한 위치인지 확인
    if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 &&
        1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2) {

        // 유닛을 만났는지 확인
        if (map[1][next_pos.row][next_pos.column] == 'H') {
            // 유닛 제거
            map[1][next_pos.row][next_pos.column] = -1;
            // 5% 확률로 스파이스 생성
            if (rand() % 100 < 5) {
                spawn_spice(next_pos);
            }
        }

        // 2% 확률로 이동하면서 스파이스 생성
        if (rand() % 100 < 30) {
            spawn_spice(obj.pos);
        }

        return next_pos;
    }

    return obj.pos;
}

// obj1(두 번째 샌드웜)에 대해서도 동일한 동작 구현
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
        // 아직 시간이 안 됐음
        return;
    }

    // 오브젝트는 layer1(map[1])에 저장
    map[1][obj.pos.row][obj.pos.column] = -1;
    obj.pos = sample_obj_next_position();
    map[1][obj.pos.row][obj.pos.column] = obj.repr;

    obj.next_move_time = sys_clock + obj.speed;
}

void sample_obj1_move(void) {
    if (sys_clock <= obj1.next_move_time) {
        // 아직 시간이 안 됐음
        return;
    }

    // 오브젝트는 layer1(map[1])에 저장
    map[1][obj1.pos.row][obj1.pos.column] = -1;
    obj1.pos = sample_obj1_next_position();
    map[1][obj1.pos.row][obj1.pos.column] = obj1.repr;

    obj1.next_move_time = sys_clock + obj1.speed;
}

//어떤 지형인지 나오게함.
void print_terrain(void) {
    POSITION pos;
    char terrain = map[0][cursor.current.row][cursor.current.column];

    clean_status();  // 상태창 초기화

    // 제목 출력
    pos.row = 2;
    pos.column = MAP_WIDTH + 4;
    gotoxy(pos);
    printf("=== 지형 정보 ===");

    //커서 위치 출력
    pos.row = 4;
    pos.column = MAP_WIDTH + 4;
    gotoxy(pos);
    printf("커서 위치: (%d, %d)", cursor.current.row, cursor.current.column);

    //지형 정보
    pos.row = 6;
    pos.column = MAP_WIDTH + 4;
    gotoxy(pos);
    printf("지형: ");
    if (terrain == 'B') {
        if (cursor.current.row >= ally_base.pos1.row &&
            cursor.current.row <= ally_base.pos4.row &&
            cursor.current.column >= ally_base.pos1.column &&
            cursor.current.column <= ally_base.pos4.column) {
            printf("Base (아군)");
            // 아군 베이스인 경우 자원 정보도 표시
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
            printf("Base (적군)");
        }
    }
    else if (terrain == 'P') {
        printf("장판");
    }
    else if (terrain == 'R') {
        printf("바위");
    }
    else if (terrain == '5') {
        printf("스파이스");
    }
    else {
        printf("사막 지형");
    }
}

void clean_status(void) {
    POSITION pos;
    // 상태창 내부 지우기w
    for (int row = 2; row < MAP_HEIGHT; row++) {
        for (int col = MAP_WIDTH + 3; col < MAP_WIDTH + 58; col++) {
            pos.row = row;
            pos.column = col;
            gotoxy(pos);
            printf(" ");
        }
    }
}

// 가장 가까운 유닛을 찾는 함수
POSITION find_nearest_unit(POSITION current_pos) {
    POSITION nearest = { 0, 0 }; //초기값 0,0
    int min_distance = MAP_WIDTH * MAP_HEIGHT; // 최대 거리로 초기화

    // 모든 맵을 순회하면서 유닛 찾기
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            // 'H'는 하베스터나 하코넨 유닛을 나타냄 테스트를 하코넨과 하베스터로 진행했음
            if (map[1][i][j] == 'H') {
                POSITION unit_pos = { i, j }; //현재 유닛 위치
                int distance = abs(current_pos.row - i) + abs(current_pos.column - j); //abs(제곱)(현재 샌드웜 위치 가로줄 - i) + (현재 샌드웜 위치 세로줄 - j) // 두 점 사이의 거리 공식 사용
                if (distance < min_distance && distance > 0) { // 자기 자신 제외
                    min_distance = distance; // 조건이 만족되면 min_distance를 distance로 갱신하고 nearest를 unit_pos로 설정한다. (거리저장)
                    nearest = unit_pos;//이렇게 하면 점점 더 가까운 유닛을 찾으면서 nearest에 가장 가까운 유닛의 위치를 기록하게 됩니다. (위치 저장)
                }
            }
        }
    }
    return nearest;
}

// 샌드웜이 스파이스를 생성하는 함수
void spawn_spice(POSITION pos) {
    // 15% 확률로 스파이스 생성
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
        print_system_message("취소됨                                 ");
        return;
    }
    if (selected_building.type == 'B' && key == k_h) {  // k_h 사용
        if (can_produce_harvester()) {  // 스파이스와 인구수 체크
            POSITION spawn_pos = { ally_base.pos1.row - 1, ally_base.pos1.column };
            if (map[1][spawn_pos.row][spawn_pos.column] == -1) {  // 생성 위치 체크
                map[1][spawn_pos.row][spawn_pos.column] = 'H';
                resource.spice -= 5;
                resource.population += 5;
                print_system_message("A new harvester is ready!");
            }
            else {
                print_system_message("유닛을 배포할 수 없습니다.");
            }
        }
        else if (resource.spice < 5) {  // 스파이스 부족 체크
            print_system_message("스파이스가 충분하지 않습니다!");
        }
        else {  // 인구수 초과 체크
            print_system_message("인구가 가득");
        }
        return;
    }
}

// 시스템 메시지를 누적해서 표시하는 전역 변수 추가

#define MAX_MESSAGES 5  // 맨 아래 줄 제외하고 5개만 저장
const char* message_log[MAX_MESSAGES];  // 이전 메시지들을 저장할 배열
const char* last_message = NULL;        // 이전에 입력된 메시지 저장

void print_system_message(const char* message) {
    POSITION pos;
    pos.column = 2;

    // 시스템 메시지창 범위 지우기
    for (int i = 0; i < 6; i++) {  // 6줄 전체 지우기
        pos.row = MAP_HEIGHT + 3 + i;
        gotoxy(pos);
        printf("                                             ");
    }

    // 이전 메시지가 있으면 배열에 저장
    if (last_message != NULL) {
        // 이전 메시지들을 한 칸씩 위로 이동
        for (int i = 0; i < MAX_MESSAGES - 1; i++) {
            message_log[i] = message_log[i + 1];
        }
        message_log[MAX_MESSAGES - 1] = last_message;
    }

    // 이전 메시지들 출력 (MAP_HEIGHT + 3부터 시작)
    for (int i = 0; i < MAX_MESSAGES; i++) {
        if (message_log[i] != NULL) {
            pos.row = MAP_HEIGHT + 3 + i;
            gotoxy(pos);
            printf("%s", message_log[i]);
        }
    }

    // 새 메시지는 항상 맨 아래(MAP_HEIGHT + 8)에 출력
    pos.row = MAP_HEIGHT + 8;
    gotoxy(pos);
    printf("%s", message);

    last_message = message;
}
void print_command_message(const char* message) {
    POSITION pos = { MAP_HEIGHT + 3, MAP_WIDTH + 4 };
    gotoxy(pos);
    printf("                                                   ");  // 이전 메시지 지우기
    gotoxy(pos);
    printf("%s", message);
}
void select_building(void) {
    char terrain = map[0][cursor.current.row][cursor.current.column];

    // 이전 선택 초기화
    selected_building.is_selected = false;
    selected_building.type = ' ';

    // 아군 본진 선택
    if (terrain == 'B' &&
        cursor.current.row >= ally_base.pos1.row &&
        cursor.current.row <= ally_base.pos4.row &&
        cursor.current.column >= ally_base.pos1.column &&
        cursor.current.column <= ally_base.pos4.column) {

        selected_building.type = 'B';
        selected_building.position = cursor.current;
        selected_building.is_selected = true;
        selected_building.is_ally = true;

        print_command_message("=== 베이스 === H: 하베스터 생산 (Cost: 5) ESC: 취소");
        print_system_message("베이스 선택됨                          ");
    }
}

bool can_produce_harvester(void) {
    return (resource.spice >= 5 && resource.population + 5 <= resource.population_max);
}