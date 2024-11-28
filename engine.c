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
// 전역 변수 추가 (기존 전역변수 선언 부분에 추가)
UNIT units[10];  // population_max의 최대값(10)만큼 배열 크기 설정
int unit_count = 0;
//하베스터 전역변수 추가
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

//오브젝트 샘플 샌드웜 될 예정
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
   .pos1 = {1, 57},
   .pos2 = {1, 56},
   .pos3 = {2, 57},
   .pos4 = {2, 56},
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
   .pos1 = {1, 55},
   .pos2 = {1, 56},
   .pos3 = {0, 55},
   .pos4 = {0, 56},
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

BUILDING_STATE building_state = {
    .is_building_mode = false,
    .building_type = ' ',    // 빈 문자로 초기화
    .cursor_size = 1
};

const UnitInfo UNIT_INFO[] = {
    [UNIT_TYPE_HARVESTER] = {
        .type = UNIT_TYPE_HARVESTER,
        .name = "하베스터",
        .symbol = 'H',
        .can_be_ally = true,
        .can_be_enemy = true,
        .cost = 5
    },
    [UNIT_TYPE_FREMEN] = {
        .type = UNIT_TYPE_FREMEN,
        .name = "프레멘",
        .symbol = 'F',
        .can_be_ally = true,
        .can_be_enemy = false,
        .cost = 6
    },
    [UNIT_TYPE_SOLDIER] = {
        .type = UNIT_TYPE_SOLDIER,
        .name = "보병",
        .symbol = 'S',
        .can_be_ally = true,
        .can_be_enemy = false,
        .cost = 4
    },
    [UNIT_TYPE_TANK] = {
        .type = UNIT_TYPE_TANK,
        .name = "중전차",
        .symbol = 'T',
        .can_be_ally = false,
        .can_be_enemy = true,
        .cost = 7
    },
    [UNIT_TYPE_SANDWORM] = {
        .type = UNIT_TYPE_SANDWORM,
        .name = "샌드웜",
        .symbol = 'W',
        .can_be_ally = false,
        .can_be_enemy = false,
        .cost = 0
    }
};
// 건물 정보 배열 정의
const BUILDING_INFO buildings[] = {
    {
        .type = BD_PLATE,
        .name = "장판",
        .shortcut = "P: Plate",
        .cost = 100,
        .durability = 0,
        .effect = "건물 짓기 전에 깔기",
        .production = "없음"
    },
    {
        .type = BD_DORM,
        .name = "숙소",
        .shortcut = "D: Dormitory",
        .cost = 210,
        .durability = 0,
        .effect = "인구 최대치 증가(10)",
        .production = "없음"
    },
    {
        .type = BD_GARAGE,
        .name = "창고",
        .shortcut = "G: Garage",
        .cost = 410,
        .durability = 0,
        .effect = "스파이스 보관 최대치 증가(10)",
        .production = "없음"
    },
    {
        .type = BD_BARRACKS,
        .name = "아트레이디스 병영",
        .shortcut = "B: Barracks",
        .cost = 420,
        .durability = 0,
        .effect = "보병 생산",
        .production = "보병(S: Soldier)"
    },
    {
        .type = BD_SHELTER,
        .name = "은신처",
        .shortcut = "S: Shelter",
        .cost = 530,
        .durability = 0,
        .effect = "특수유닛 생산",
        .production = "프레멘(F: Fremen)"
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
    print_command_message("B: 건설");
    display(resource, map, cursor);

    while (1) {
        KEY key = get_key();

        if (is_arrow_key(key)) {
            DIRECTION dir = ktod(key);
            cursor_move(dir);
        }
        else {
            switch (key) {
            case k_b:  // 건설 메뉴
                process_command(key);
                break;

            case k_space:  // 스페이스바 - 선택 또는 위치 확정
                if (building_state.is_building_mode) {
                    process_command(key);
                }
                else {
                    // 하베스터가 선택된 상태에서 스페이스바 (위치 확정)
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

                    // 하베스터 명령이 처리되지 않았으면 건물/유닛 선택
                    if (!harvester_command_processed) {
                        clean_status();
                        select_building();
                    }
                }
                break;

            case k_m:  // 이동 명령
                if (harvester_selected) {
                    for (int i = 0; i < harvester_count; i++) {
                        if (harvesters[i].is_selected) {
                            process_harvester_command(&harvesters[i], k_m, cursor.current);
                            print_system_message("이동할 위치를 선택하고 스페이스바를 눌러주세요");
                            break;
                        }
                    }
                }
                break;

            case k_h:  // 수확 명령
                if (harvester_selected) {
                    for (int i = 0; i < harvester_count; i++) {
                        if (harvesters[i].is_selected) {
                            process_harvester_command(&harvesters[i], k_h, cursor.current);
                            break;
                        }
                    }
                }
                else if (selected_building.type == 'B' && selected_building.is_ally) {
                    process_command(key);  // 하베스터 생산
                }
                break;

            case k_p:  // 장판
            case k_d:  // 숙소
            case k_g:  // 창고
            case k_s:  // 은신처
                if (building_state.is_building_mode) {
                    process_command(key);
                }
                break;

            case k_esc:  // 선택 취소
                if (harvester_selected) {
                    for (int i = 0; i < harvester_count; i++) {
                        if (harvesters[i].is_selected) {
                            harvesters[i].is_selected = false;
                            harvester_selected = false;
                            clean_status();
                            print_command_message("B: 건설");
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
    // layer 0에 있는 맵 데이터 초기화 - 테두리 제외하고 초기화
    for (int i = 1; i < MAP_HEIGHT - 1; i++) {
        for (int j = 1; j < MAP_WIDTH - 1; j++) {
            map[0][i][j] = ' ';  // 테두리를 제외한 내부만 공백으로 설정
        }
    }

    // 아군 베이스
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            map[0][ally_base.pos1.row + i][ally_base.pos1.column + j] = ally_base.repr;
        }
    }
    // 적군 베이스
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            map[0][enemy_base.pos1.row + i][enemy_base.pos1.column + j] = enemy_base.repr;
        }
    }

    // 아군 장판
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            map[0][ally_pad.pos1.row + i][ally_pad.pos1.column + j] = ally_pad.repr;
        }
    }

    // 적군 장판
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            map[0][enemy_pad.pos1.row + i][enemy_pad.pos1.column + j] = enemy_pad.repr;
        }
    }

    // 아군 스파이스
    map[0][ally_spice.pos1.row][ally_spice.pos1.column] = ally_spice.repr;

    // 적군 스파이스
    map[0][enemy_spice.pos1.row][enemy_spice.pos1.column] = enemy_spice.repr;
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
    // 초기 하베스터 생성 (아군)
    POSITION ally_pos = { 14, 1 };
    init_harvester(ally_pos, true);
    resource.population += 5;

    // 초기 하베스터 생성 (적군)
    POSITION enemy_pos = { 3, 58 };
    init_harvester(enemy_pos, false);
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

    int time_diff = sys_clock - last_key_time;
    last_key_time = sys_clock;

    int move_distance = (time_diff < DOUBLE_PRESS_INTERVAL) ? 2 : 1;

    for (int i = 0; i < move_distance; i++) {
        // building_state의 cursor_size를 사용한 경계 체크
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

    // 하베스터가 있는 경우
    if (map[1][next_pos.row][next_pos.column] == 'H') {
        // units 배열에서도 제거
        remove_unit_at(next_pos);
    }

    // 스파이스 생성 시도
    spawn_spice(obj.pos);  // 이전 위치에 스파이스 생성 시도

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

    // 하베스터가 있는 경우
    if (map[1][next_pos.row][next_pos.column] == 'H') {
        // units 배열에서도 제거
        remove_unit_at(next_pos);
    }

    // 스파이스 생성 시도
    spawn_spice(obj1.pos);  // 이전 위치에 스파이스 생성 시도

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
    else if (terrain >= '1' && terrain <= '8') {  // 1~8까지 모두 스파이스로 처리
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
    else if (terrain >= '1' && terrain <= '8') {  // 1~8까지 모두 스파이스로 처리
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
    harvester_selected = false;  // 하베스터 선택 상태 초기화

    if (unit == 'H') {
        for (int i = 0; i < harvester_count; i++) {
            if (harvesters[i].pos.row == cursor.current.row &&
                harvesters[i].pos.column == cursor.current.column) {
                harvesters[i].is_selected = true;
                harvester_selected = true;
                print_harvester_info(&harvesters[i]);
                if (harvesters[i].is_ally) {
                    print_system_message("아군 하베스터를 선택했습니다.");
                    print_command_message("M: 이동 | H: 수확 | ESC: 취소");
                }
                else {
                    print_system_message("적군 하베스터를 선택했습니다.");
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
        // 제목 출력
        pos.row = 2;
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

                print_command_message("M: 이동 | H: 수확 | ESC: 취소");
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
    // 상태창 내부 지우기
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
    if (rand() % 100 < 15 && map[0][pos.row][pos.column] == ' ') {  // 15% 확률
        // 2에서 8 사이의 랜덤한 숫자를 문자로 변환
        char spice_amount = '0' + (2 + (rand() % 7));  // 2~8 랜덤 생성
        map[0][pos.row][pos.column] = spice_amount;
    }
}
// 건설 메뉴 처리 함수 수정
void process_command(KEY key) {
    if (!selected_building.is_selected && !building_state.is_building_mode) {
        if (key == k_b) {
            building_state.is_building_mode = true;
            building_state.cursor_size = 1;
            print_command_message("P:장판 D:숙소 G:창고 B:병영 S:은신처 ESC:취소");
        }
    }
    else if (building_state.is_building_mode) {
        switch (key) {
        case k_p:
            building_state.building_type = BD_PLATE;
            building_state.cursor_size = 2;  // 2x2로 변경
            print_command_message("장판 건설 - [Space]건설 [ESC]취소 (비용: 2 내구도: 10)");
            break;

        case k_d:
            building_state.building_type = BD_DORM;
            building_state.cursor_size = 2;
            print_command_message("숙소 건설 - [Space]건설 [ESC]취소 (비용: 3 내구도: 15)");
            break;

        case k_g:
            building_state.building_type = BD_GARAGE;
            building_state.cursor_size = 2;
            print_command_message("창고 건설 - [Space]건설 [ESC]취소 (비용: 4 내구도: 15)");
            break;

        case k_b:
            building_state.building_type = BD_BARRACKS;
            building_state.cursor_size = 2;
            print_command_message("병영 건설 - [Space]건설 [ESC]취소 (비용: 5 내구도: 20)");
            break;

        case k_s:
            building_state.building_type = BD_SHELTER;
            building_state.cursor_size = 2;
            print_command_message("은신처 건설 - [Space]건설 [ESC]취소 (비용: 6 내구도: 25)");
            break;

        case k_space:
            if (building_state.building_type != ' ') {
                if (can_build_here()) {
                    build_structure();
                    building_state.is_building_mode = false;
                    building_state.building_type = ' ';
                    building_state.cursor_size = 1;  // 건설 완료 후 1x1로 복귀
                    print_command_message("B: 건설                                           ");
                }
            }
            break;

        case k_esc:
            building_state.is_building_mode = false;
            building_state.building_type = ' ';
            building_state.cursor_size = 1;  // ESC로 취소 시 1x1로 복귀
            print_command_message("B: 건설                                                ");
            break;
        }
    }
    else if (selected_building.type == 'B' && selected_building.is_ally) {
        switch (key) {
            // 생산 위치를 초기 하베스터 근처로 수정
        case k_h:  // 하베스터 생산
            if (resource.spice < 5) {
                print_system_message("스파이스가 부족합니다!");
            }
            else if (resource.population + 5 > resource.population_max) {
                print_system_message("인구 수 제한을 초과했습니다!");
            }
            else if (harvester_count >= MAX_HARVESTERS) {
                print_system_message("더 이상 하베스터를 생산할 수 없습니다!");
            }
            else {
                // 생산 위치 설정 (초기 하베스터 근처)
                POSITION new_pos = {
                    Harverster.pos1.row,  // 초기 하베스터와 같은 row
                    Harverster.pos1.column + 1  // 초기 하베스터 옆에 생성
                };

                init_harvester(new_pos, true);
                resource.spice -= 5;
                resource.population += 5;
                print_system_message("하베스터가 생산되었습니다!");
            }
            break;

        case k_esc:
            selected_building.is_selected = false;
            selected_building.type = ' ';
            print_command_message("B: 건설");
            break;
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

        print_command_message("=본진= H:하베스터 생산 | ESC:취소");
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

bool can_build_here(void) {
    POSITION pos = cursor.current;
    int cost;

    // 각 건물 타입별 비용 설정
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

    // 스파이스 비용 체크
    if (resource.spice < cost) {
        print_system_message("스파이스가 부족합니다");
        return false;
    }

    // 맵 경계 체크
    if (pos.row < 1 || pos.row + building_state.cursor_size > MAP_HEIGHT - 1 ||
        pos.column < 1 || pos.column + building_state.cursor_size > MAP_WIDTH - 1) {
        print_system_message("맵 경계에 건설할 수 없습니다");
        return false;
    }

    // 장판이 아닌 건물 건설 시 장판 필요
    if (building_state.building_type != BD_PLATE) {
        for (int i = 0; i < building_state.cursor_size; i++) {
            for (int j = 0; j < building_state.cursor_size; j++) {
                if (map[0][pos.row + i][pos.column + j] != 'P') {
                    print_system_message("장판 위에만 건설할 수 있습니다");
                    return false;
                }
            }
        }
    }

    // 다른 건물과의 충돌 체크
    for (int i = 0; i < building_state.cursor_size; i++) {
        for (int j = 0; j < building_state.cursor_size; j++) {
            char terrain = map[0][pos.row + i][pos.column + j];
            if (building_state.building_type != BD_PLATE && terrain != 'P' && terrain != ' ') {
                print_system_message("다른 건물이 있어 건설할 수 없습니다");
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
        // 장판은 'P'로 2x2 채우기
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                map[0][pos.row + i][pos.column + j] = 'P';
            }
        }
        print_system_message("장판 설치 완료!");
        break;

    case BD_DORM:
        cost = 3;
        // 숙소는 'D'로 2x2 채우기
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                map[0][pos.row + i][pos.column + j] = 'D';
            }
        }
        resource.population_max += 10;
        print_system_message("숙소 건설 완료! 인구 수용량이 10 증가했습니다.");
        break;

    case BD_GARAGE:
        cost = 4;
        // 창고는 'G'로 2x2 채우기
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                map[0][pos.row + i][pos.column + j] = 'G';
            }
        }
        resource.spice_max += 10;
        print_system_message("창고 건설 완료! 스파이스 저장량이 10 증가했습니다.");
        break;

    case BD_BARRACKS:
        cost = 5;
        // 병영은 'B'로 2x2 채우기
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                map[0][pos.row + i][pos.column + j] = 'B';
            }
        }
        print_system_message("병영 건설 완료! 이제 보병을 생산할 수 있습니다.");
        break;

    case BD_SHELTER:
        cost = 6;
        // 은신처는 'S'로 2x2 채우기
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                map[0][pos.row + i][pos.column + j] = 'S';
            }
        }
        print_system_message("은신처 건설 완료! 이제 프레멘을 생산할 수 있습니다.");
        break;
    }

    // 자원 소비
    resource.spice -= cost;
}

bool create_new_unit(char type, POSITION pos, bool is_ally) {
    if (unit_count >= resource.population_max) {
        return false;  // 최대 유닛 수 초과
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
        return false;  // 인구 수 초과
    }

    units[unit_count].pos1 = pos;
    units[unit_count].repr = type;
    units[unit_count].layer = 1;

    map[units[unit_count].layer][pos.row][pos.column] = type;

    resource.population += unit_supply_cost;
    unit_count++;

    return true;
}

// 유닛 제거 함수
void remove_unit_at(POSITION pos) {
    // 하베스터 배열에서 해당 위치의 하베스터 찾아서 제거
    for (int i = 0; i < harvester_count; i++) {
        if (harvesters[i].pos.row == pos.row &&
            harvesters[i].pos.column == pos.column) {
            // 아군 하베스터인 경우 인구수 감소
            if (harvesters[i].is_ally) {
                resource.population -= 5;
            }

            // 하베스터의 상태 초기화
            harvesters[i].state = H_IDLE;
            harvesters[i].is_selected = false;
            harvesters[i].auto_harvest = false;
            harvesters[i].spice_carried = 0;

            // 하베스터 배열에서 제거 (마지막 하베스터를 현재 위치로 이동)
            if (i < harvester_count - 1) {
                harvesters[i] = harvesters[harvester_count - 1];
            }
            harvester_count--;

            // 선택 상태였다면 전역 선택 상태도 해제
            if (harvester_selected) {
                harvester_selected = false;
                clean_status();
                print_command_message("B: 건설");
            }
            break;
        }
    }

    // units 배열에서도 제거
    for (int i = 0; i < unit_count; i++) {
        if (units[i].pos1.row == pos.row && units[i].pos1.column == pos.column) {
            // 맵에서 유닛 제거
            map[units[i].layer][pos.row][pos.column] = -1;

            // 배열에서 유닛 제거 (마지막 유닛을 현재 위치로 이동)
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
    h->speed = 500;           // 샌드웜보다 더 빠르게 설정
    h->next_move_time = 500;  // 초기 이동 시간

    map[1][pos.row][pos.column] = 'H';
    harvester_count++;
}

// 하베스터 명령 처리
void process_harvester_command(HARVESTER* h, KEY key, POSITION target) {
    if (!h->is_selected) return;

    switch (key) {
    case k_m:  // 이동 명령
        h->state = H_WAIT_MOVE_POS;
        h->auto_harvest = false;  // 자동 수확 모드 해제
        print_system_message("이동할 위치를 선택하고 스페이스바를 눌러주세요.");
        break;

    case k_h:  // 수확 명령
        if (!is_at_base(h)) {  // 본진이 아닐 때만 수확 명령 가능
            h->state = H_WAIT_HARVEST_POS;
            print_system_message("수확할 스파이스를 선택하고 스페이스바를 눌러주세요.");
        }
        else {
            h->state = H_WAIT_HARVEST_POS;  // 본진에 있어도 수확 명령 가능하도록
            print_system_message("수확할 스파이스를 선택하고 스페이스바를 눌러주세요.");
        }
        break;

    case k_space:  // 위치 확정
        if (h->state == H_WAIT_MOVE_POS) {
            h->target_pos = cursor.current;
            h->state = H_MOVING;
            print_system_message("선택한 위치로 이동합니다.");
        }
        else if (h->state == H_WAIT_HARVEST_POS) {
            // ... (나머지 스페이스바 처리 코드는 그대로)
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
                    print_system_message("스파이스 매장지 근처로 이동합니다.");
                }
                else {
                    print_system_message("스파이스 근처에 접근할 수 있는 공간이 없습니다.");
                }
            }
            else {
                print_system_message("스파이스 매장지를 선택해주세요.");
            }
        }
        break;

    case k_esc:
        h->is_selected = false;
        harvester_selected = false;
        clean_status();
        print_command_message("B: 건설");
        break;
    }
}

// 하베스터 상태 업데이트 함수
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
            print_system_message("스파이스 수확을 시작합니다...");  // 수확 시작할 때만 메시지 출력
        }
        else if (h->pos.row == h->target_pos.row && h->pos.column == h->target_pos.column) {
            h->state = H_IDLE;
            print_system_message("이동이 완료되었습니다.");
        }
        else {
            if (sys_clock > h->next_move_time) {
                move_harvester(h);
            }
        }
    }
    break;

    case H_HARVESTING:
        // 수확 중일 때는 메시지를 출력하지 않음
        if (sys_clock - h->harvest_start_time >= HARVEST_TIME) {
            int current_spice = map[0][h->harvest_pos.row][h->harvest_pos.column] - '0';
            int intended_harvest = 2 + (rand() % 3);
            h->spice_carried = min(current_spice, intended_harvest);

            if (current_spice <= h->spice_carried) {
                map[0][h->harvest_pos.row][h->harvest_pos.column] = ' ';
                print_system_message("남은 스파이스를 모두 수확했습니다!");
            }
            else {
                map[0][h->harvest_pos.row][h->harvest_pos.column] = '0' + (current_spice - h->spice_carried);
                if (h->spice_carried == 2) print_system_message("수확 완료! 2 스파이스를 획득했습니다.");
                if (h->spice_carried == 3) print_system_message("수확 완료! 3 스파이스를 획득했습니다.");
                if (h->spice_carried == 4) print_system_message("수확 완료! 4 스파이스를 획득했습니다.");
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

        // update_harvester 함수의 H_RETURNING case 부분 수정
    case H_RETURNING:
    {
        bool near_base = false;
        if (h->is_ally) {
            // 본진 위쪽 한 칸에서 전달
            near_base = (h->pos.row == ally_base.pos1.row - 1 &&
                h->pos.column >= ally_base.pos1.column &&
                h->pos.column <= ally_base.pos4.column);

            // 목표 위치도 본진 바로 위로 설정
            if (h->target_pos.row == ally_base.pos1.row &&
                h->target_pos.column == ally_base.pos1.column) {
                h->target_pos.row = ally_base.pos1.row - 1;
            }
        }
        else {
            // 적군 본진 위쪽 한 칸에서 전달
            near_base = (h->pos.row == enemy_base.pos1.row - 1 &&
                h->pos.column >= enemy_base.pos1.column &&
                h->pos.column <= enemy_base.pos4.column);

            // 목표 위치도 본진 바로 위로 설정
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
                    print_system_message("창고가 가득 차서 일부 스파이스를 버렸습니다.");
                }
                else {
                    if (delivered == 2) print_system_message("2 스파이스를 창고에 저장했습니다.");
                    if (delivered == 3) print_system_message("3 스파이스를 창고에 저장했습니다.");
                    if (delivered == 4) print_system_message("4 스파이스를 창고에 저장했습니다.");
                }
            }
            h->spice_carried = 0;

            // 스파이스가 1 이상 남아있으면 계속 수확
            if (h->auto_harvest && map[0][h->harvest_pos.row][h->harvest_pos.column] >= '1' &&
                map[0][h->harvest_pos.row][h->harvest_pos.column] <= '8') {
                h->target_pos = h->harvest_pos;  // 같은 스파이스로 다시 이동
                h->state = H_MOVING;
                print_system_message("다시 스파이스 매장지로 이동합니다...");
            }
            else {
                h->state = H_IDLE;
                h->auto_harvest = false;  // 자동 수확 모드 해제
                h->target_pos = h->pos;  // 현재 위치를 목표 위치로 설정 (제자리에 멈춤)
                print_system_message("매장지가 고갈되었습니다. 현재 위치에서 대기합니다.");
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
// 하베스터 이동 처리
void move_harvester(HARVESTER* h) {
    if (sys_clock <= h->next_move_time) {
        return;
    }

    map[1][h->pos.row][h->pos.column] = -1;  // 현재 위치에서 제거

    if (h->pos.column != h->target_pos.column) {
        h->pos.column += (h->target_pos.column > h->pos.column) ? 1 : -1;
    }
    else if (h->pos.row != h->target_pos.row) {
        h->pos.row += (h->target_pos.row > h->pos.row) ? 1 : -1;
    }

    map[1][h->pos.row][h->pos.column] = 'H';  // 새 위치에 표시
    h->next_move_time = sys_clock + h->speed;  // 다음 이동 시간 설정
}


// 모든 하베스터 업데이트
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

// 스파이스 매장지인지 확인하는 함수
bool is_spice_deposit(POSITION pos) {
    char terrain = map[0][pos.row][pos.column];
    return (terrain >= '1' && terrain <= '8');  // 1~8 모두 스파이스로 처리
}
// 다른 하베스터가 있는지 확인하는 함수
bool is_occupied_by_harvester(POSITION pos) {
    for (int i = 0; i < harvester_count; i++) {
        if (harvesters[i].pos.row == pos.row &&
            harvesters[i].pos.column == pos.column) {
            return true;
        }
    }
    return false;
}

// 하베스터가 본진에 있는지 확인하는 함수
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

// 하베스터 정보 출력 함수
void print_harvester_info(HARVESTER* h) {
    POSITION pos;
    pos.row = 2;
    pos.column = MAP_WIDTH + 4;

    clean_status();

    gotoxy(pos);
    printf("=== 하베스터 정보 ===");

    pos.row += 2;
    gotoxy(pos);
    printf("위치: (%d, %d)", h->pos.row, h->pos.column);

    pos.row += 2;
    gotoxy(pos);
    printf("체력: %d/100", h->health);

    pos.row += 2;
    gotoxy(pos);
    printf("적재량: %d", h->spice_carried);

    pos.row += 2;
    gotoxy(pos);
    printf("상태: ");
    switch (h->state) {
    case H_IDLE: printf("대기 중"); break;
    case H_MOVING: printf("이동 중"); break;
    case H_HARVESTING: printf("수확 중"); break;
    case H_RETURNING: printf("귀환 중"); break;
    case H_WAIT_MOVE_POS: printf("이동 위치 대기 중"); break;
    case H_WAIT_HARVEST_POS: printf("수확 위치 대기 중"); break;
    }

    print_command_message("M: 이동 | H: 수확 | ESC: 취소");
}