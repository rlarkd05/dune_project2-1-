#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"


void init(void); //맵 함수
void intro(void); //인트로 함수
void Construction(void); //구조물 함수
void Biome(void); //지형 함수
void unit(void); //유닛 함수
void outro(void); //아웃트로
void cursor_move(DIRECTION dir); //커서 움직임 함수
void cursor_double_move(DIRECTION dir, int times); //커서 더블클릭 함수
void sandworm_move(void); //샌드웜1 함수
void sandworm1_move(void); //샌드웜 2 함수
void StatusWindow(void); //상태창 함수
void system_message(void); //시스템 메시지창 함수
void command_message(void); //명령창 함수
void print_terrain(void); //상태창에 지형 출력 함수
void print_unit_info(void); //상태창에 유닛 출력 함수
void clean_status(void); //esc키를 눌렀을때 상태창 비워주는 함수
void select_building(void); //건물에 스페이스바를 눌렀을때 명령창에 실행할 명령 뜨게 해주는 함수
void select_unit(void); // 유닛 선택 함수 선언 추가
void process_command(KEY key); //키보드 입력에 따른 명령을 처리하는 함수
void print_command_message(const char* message); //명령창 출력 함수
void print_system_message(const char* message); // 시스템창 출력함수
bool can_produce_harvester(void); //하베스터 생산 조건 함수
POSITION find_nearest_unit(POSITION current_pos); // 샌드웜이 근처 유닛 찾는 함수
void spawn_spice(POSITION pos); //샌드웜임 일정 확률로 스파이스 배출 함수


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

//오브젝트 샘플 샌드웜 될 예정
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
                print_unit_info();
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
        sandworm_move();
        sandworm1_move();

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
    POSITION pos;
    int message_start_row = MAP_HEIGHT + 2; // 시스템 메시지 창의 시작 행
    int message_width = MAP_WIDTH - 1;      // 시스템 메시지 창 너비
    int message_height = 8;                 // 시스템 메시지 창 높이

    // 테두리와 내부 공백 출력
    for (int row = message_start_row; row < message_start_row + message_height; row++) {
        for (int col = 0; col <= message_width; col++) {
            pos.row = row;
            pos.column = col;
            gotoxy(pos);

            // 테두리인 경우 '#' 출력
            if (row == message_start_row || row == message_start_row + message_height - 1 ||
                col == 0 || col == message_width) {
                printf("#");
            }
            // 내부인 경우 공백 출력
            else {
                printf(" ");
            }
        }
    }
}

//명령창

void command_message() {
    POSITION pos;
    int command_start_row = MAP_HEIGHT + 2;  // 명령창의 시작 행 (상태창 바로 아래)
    int command_width = 58;                  // 명령창 너비 (상태창 너비와 동일)
    int command_height = 8;                  // 명령창 높이

    // 테두리와 내부 공백 출력
    for (int row = command_start_row; row < command_start_row + command_height; row++) {
        for (int col = MAP_WIDTH + 2; col < MAP_WIDTH + 2 + command_width; col++) {
            pos.row = row;
            pos.column = col;
            gotoxy(pos);

            // 테두리인 경우 '#' 출력
            if (row == command_start_row || row == command_start_row + command_height - 1 ||
                col == MAP_WIDTH + 2 || col == MAP_WIDTH + 2 + command_width - 1) {
                printf("#");
            }
            // 내부인 경우 공백 출력
            else {
                printf(" ");
            }
        }
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


POSITION sandworm_next_position(void) {
    POSITION nearest_unit = find_nearest_unit(obj.pos);
    POSITION diff = psub(nearest_unit, obj.pos);
    DIRECTION dir;

    // 유닛이 없으면 랜덤하게 움직임
    if (nearest_unit.row == obj.pos.row && nearest_unit.column == obj.pos.column) {
        dir = rand() % 4;  // 0~3 사이의 랜덤한 방향
    }
    // 유닛이 있으면 유닛 방향으로 이동
    else {
        if (abs(diff.row) >= abs(diff.column)) {
            dir = (diff.row >= 0) ? d_down : d_up;
        }
        else {
            dir = (diff.column >= 0) ? d_right : d_left;
        }
    }

    POSITION next_pos = pmove(obj.pos, dir);

    // 맵 범위 체크
    if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 &&
        1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2) {
        // 바위를 만났을 때 방향 전환
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

    // 유닛이 없으면 랜덤하게 움직임
    if (nearest_unit.row == obj1.pos.row && nearest_unit.column == obj1.pos.column) {
        dir = rand() % 4;  // 0~3 사이의 랜덤한 방향
    }
    // 유닛이 있으면 유닛 방향으로 이동
    else {
        if (abs(diff.row) >= abs(diff.column)) {
            dir = (diff.row >= 0) ? d_down : d_up;
        }
        else {
            dir = (diff.column >= 0) ? d_right : d_left;
        }
    }

    POSITION next_pos = pmove(obj1.pos, dir);

    // 맵 범위 체크
    if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 &&
        1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2) {
        // 바위를 만났을 때 방향 전환
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

    // 하베스터를 만났는지 체크
    if (map[1][next_pos.row][next_pos.column] == 'H') {
        map[1][next_pos.row][next_pos.column] = -1;
    }

    // 30% 확률로 스파이스 생성
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

    // 하베스터를 만났는지 체크
    if (map[1][next_pos.row][next_pos.column] == 'H') {
        map[1][next_pos.row][next_pos.column] = -1;
    }

    // 30% 확률로 스파이스 생성
    if (rand() % 100 < 30) {
        spawn_spice(obj1.pos);
    }

    obj1.pos = next_pos;
    map[1][obj1.pos.row][obj1.pos.column] = obj1.repr;
    obj1.next_move_time = sys_clock + obj1.speed;
}

//어떤 지형인지 나오게함
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

    //지형 정보 출력
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

    // 지형 특징 출력
    pos.row = 8;
    pos.column = MAP_WIDTH + 4;
    gotoxy(pos);
    printf("특징: ");
    if (terrain == 'B') {
        printf("유닛 생산 건물");
    }
    else if (terrain == 'P') {
        printf("장판 위에 건설 가능");
    }
    else if (terrain == 'R') {
        printf("샌드웜이 통과할 수 없음");
    }
    else if (terrain == '5') {
        printf("수집 가능한 자원");
    }
    else {
        printf("기본 지형, 건물을 지을 수 없음");
    }
}

void select_unit(void) {
    char unit = map[1][cursor.current.row][cursor.current.column];

    selected_building.is_selected = false;
    selected_building.type = ' ';

    // 하베스터 선택
    if (unit == 'H') {
        selected_building.is_selected = true;

        if (cursor.current.row == Harverster.pos1.row &&
            cursor.current.column == Harverster.pos1.column) {
            clean_status();
            print_unit_info();
            print_system_message("아군 하베스터를 선택했습니다");
        }
        else if (cursor.current.row == Haconen.pos2.row &&
            cursor.current.column == Haconen.pos2.column) {
            clean_status();
            print_unit_info();
            print_system_message("적군 하베스터를 선택했습니다");
        }
    }
    // 샌드웜 선택
    else if (unit == 'W') {
        selected_building.is_selected = true;
        clean_status();
        print_unit_info();
        print_system_message("샌드웜을 선택했습니다");
    }
}
void print_unit_info(void) {
    POSITION pos;
    char unit = map[1][cursor.current.row][cursor.current.column];

    if (unit != -1) {
        // 제목 출력
        pos.row = 2;  // 지형 정보와 같은 위치에서 시작
        pos.column = MAP_WIDTH + 4;
        gotoxy(pos);
        printf("=== 유닛 정보 ===");

        // 현재 위치 출력
        pos.row = 4;
        pos.column = MAP_WIDTH + 4;
        gotoxy(pos);
        printf("커서 위치: (%d, %d)", cursor.current.row, cursor.current.column);

        // 유닛 종류 출력
        pos.row = 6;
        pos.column = MAP_WIDTH + 4;
        gotoxy(pos);
        printf("유닛: ");

        if (unit == 'H') {
            // 아군 하베스터 정보
            if (cursor.current.row == Harverster.pos1.row &&
                cursor.current.column == Harverster.pos1.column) {
                printf("하베스터 (아군)");

                pos.row = 8;
                pos.column = MAP_WIDTH + 4;
                gotoxy(pos);
                printf("상태: 정상                                       ");

            }
            // 적군 하코넨 정보
            else if (cursor.current.row == Haconen.pos2.row &&
                cursor.current.column == Haconen.pos2.column) {
                printf("하베스터 (적군)");

                pos.row = 8;
                pos.column = MAP_WIDTH + 4;
                gotoxy(pos);
                printf("소속: 하코넨                                    ");
            }
        }
        else if (unit == 'W') {
            printf("샌드웜   ");

            pos.row = 8;
            pos.column = MAP_WIDTH + 4;
            gotoxy(pos);
            printf("특성: 유닛 추적 및 공격                        ");
        }
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
    POSITION nearest = { -1, -1 };  // 초기값을 -1, -1로 변경
    int min_distance = MAP_WIDTH * MAP_HEIGHT;
    bool found_unit = false;  // 유닛 발견 여부 체크

    // 모든 맵을 순회하면서 유닛 찾기
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

    // 유닛을 못 찾았을 경우 랜덤한 목적지 반환
    if (!found_unit) {
        nearest.row = 1 + (rand() % (MAP_HEIGHT - 2));
        nearest.column = 1 + (rand() % (MAP_WIDTH - 2));
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

    //esc키를 눌렀을때
    if (key == k_esc) {
        selected_building.is_selected = false;
        selected_building.type = ' ';
        print_command_message("                                                 ");  // 명령창 비우기
        print_system_message("선택이 취소되었습니다");
        clean_status();  // 상태창도 비우기
        return;
    }

    if (selected_building.type == 'B') {
        if (key == k_h) {
            if (can_produce_harvester()) {
                map[1][ally_base.pos2.row - 1][ally_base.pos2.column] = 'H';
                resource.spice -= 5; //스파이스 저장공간 += 5
                resource.population += 5; //인구공간 += 5
                print_system_message("하베스터가 생산되었습니다!");
            }
            else {
                print_system_message("스파이스가 부족합니다!");
            }
        }
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
void print_command_message(const char* message) { //명령창 출력
    POSITION pos = { MAP_HEIGHT + 3, MAP_WIDTH + 4 };
    gotoxy(pos);
    printf("                                                   ");  // 이전 메시지 지우기
    gotoxy(pos);
    printf("%s", message);
}

void select_building(void) {
    char terrain = map[0][cursor.current.row][cursor.current.column];
    char unit = map[1][cursor.current.row][cursor.current.column];

    selected_building.is_selected = false;
    selected_building.type = ' ';

    // 유닛이 있는 경우 유닛 선택 함수 호출
    if (unit != -1) {
        select_unit();
        return;
    }

    clean_status();
    print_terrain();

    // 건물 선택 및 명령 처리
    if (terrain == 'B' &&
        cursor.current.row >= ally_base.pos1.row &&
        cursor.current.row <= ally_base.pos4.row &&
        cursor.current.column >= ally_base.pos1.column &&
        cursor.current.column <= ally_base.pos4.column) {
        //본진에 대고 스페이스바 눌렀는지 체크
        selected_building.type = 'B';
        selected_building.position = cursor.current;
        selected_building.is_selected = true;
        selected_building.is_ally = true;

        print_command_message("=본진= H:하베스터 생산 (스파이스 5 필요) | ESC:취소");
        print_system_message("아군 본진을 선택했습니다");
    }
    // 나머지 지형/건물 선택
    else if (terrain == 'B') {
        print_system_message("적군 본진을 선택했습니다");
    }
    else if (terrain == 'P') {
        print_system_message("장판을 선택했습니다");
    }
    else if (terrain == 'R') {
        print_system_message("바위를 선택했습니다");
    }
    else if (terrain == '5') {
        print_system_message("스파이스를 선택했습니다");
    }
    else {
        print_system_message("사막을 선택했습니다");
    }
}

bool can_produce_harvester(void) {
    return (resource.spice >= 5 && resource.population + 5 <= resource.population_max);
}