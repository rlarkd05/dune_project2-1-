#pragma once
#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <conio.h>
#include <assert.h>

/* ================= system parameters =================== */
#define TICK 10      // time unit(ms)


#define N_LAYER 2
#define MAP_WIDTH   60 //가로
#define MAP_HEIGHT   18

#define MAX_HARVESTERS 10
#define HARVEST_TIME 4000    // 4초
#define MIN_SPICE_HARVEST 2  // 최소 수확량
#define MAX_SPICE_HARVEST 4  // 최대 수확량

#define DOUBLE_PRESS_INTERVAL 200
#define MAX_MSG_LINES 6  
#define MAX_UNITS 10  // population_max의 최대값
/* ================= 위치와 방향 =================== */
// 맵에서 위치를 나타내는 구조체
typedef struct {
	int row, column;
} POSITION;

// 커서 위치
typedef struct {
	POSITION previous;  // 직전 위치
	POSITION current;   // 현재 위치
} CURSOR;

// 입력 가능한 키 종류.
// 수업에서 enum은 생략했는데, 크게 어렵지 않으니 예제 검색
typedef enum {
	// k_none: 입력된 키가 없음. d_stay(안 움직이는 경우)에 대응
	k_none = 0, k_up, k_right, k_left, k_down,
	k_quit,
	k_undef, // 정의되지 않은 키 입력   
	k_space, // 스페이스바
	k_esc, // esc키
	k_h,
	k_b, // b키
	k_p, //p키
	k_d,
	k_g,
	k_s,
	k_m,
} KEY;

/* ================= 구조체 정의 =================== */
typedef struct {
	POSITION pos1; // 건물 크기 1
	POSITION pos2; // 건물 크기 2
	POSITION pos3; // 건물 크기 3
	POSITION pos4; // 건물 크기 4
	char repr;     // 건물 이름
	int layer;     // 몇번 째 레이어에 들어가는건지 구분 <-- 필요하면 사용
} OBJECT_BUILDING;

// DIRECTION은 KEY의 부분집합이지만, 의미를 명확하게 하기 위해서 다른 타입으로 정의
typedef enum {
	d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION;


/* ================= 위치와 방향(2) =================== */
// 편의성을 위한 함수들. KEY, POSITION, DIRECTION 구조체들을 유기적으로 변환

// 편의성 함수
inline POSITION padd(POSITION p1, POSITION sp2) {
	POSITION p = { p1.row + sp2.row, p1.column + sp2.column };  // sp2로 수정
	return p;
}

// p1 - p2
inline POSITION psub(POSITION p1, POSITION p2) {
	POSITION p = { p1.row - p2.row, p1.column - p2.column };
	return p;
}

// 방향키인지 확인하는 함수
#define is_arrow_key(k)      (k_up <= (k) && (k) <= k_down)

// 화살표 '키'(KEY)를 '방향'(DIRECTION)으로 변환. 정수 값은 똑같으니 타입만 바꿔주면 됨
#define ktod(k)      (DIRECTION)(k)

// DIRECTION을 POSITION 벡터로 변환하는 함수
inline POSITION dtop(DIRECTION d) {
	static POSITION direction_vector[] = { {0, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 0} };
	return direction_vector[d];
}

// p를 d 방향으로 이동시킨 POSITION
#define pmove(p, d)      (padd((p), dtop(d)))

/* ================= game data =================== */
typedef struct {
	int spice;      // 현재 보유한 스파이스
	int spice_max;  // 스파이스 최대 저장량
	int population; // 현재 인구 수
	int population_max;  // 수용 가능한 인구 수
} RESOURCE;


// 대강 만들어 봤음. 기능 추가하면서 각자 수정할 것
typedef struct {
	POSITION pos;      // 현재 위치
	POSITION dest;     // 목적지
	char repr;         // 오브젝트의 표현 문자
	int speed;         // 이동 속도
	int next_move_time; // 다음 이동 시간
} OBJECT_SAMPLE;

typedef struct {
	POSITION pos;      // 현재 위치
	POSITION dest;     // 목적지
	char repr;         // 오브젝트의 표현 문자
	int speed;         // 이동 속도
	int next_move_time; // 다음 이동 시간
} OBJECT_SAMPLE1;

typedef struct {
	POSITION pos1; // 건물 크기 1
	POSITION pos2; // 건물 크기 2
	POSITION pos3; // 건물 크기 3
	POSITION pos4; // 건물 크기 4
	char repr;     // 건물 이름
	int layer;     // 몇번 째 레이어에 들어가는건지 구분 <-- 필요하면 사용
} UNIT;

typedef struct {
	char type;              // 선택된 건물 타입
	POSITION position;      // 선택된 건물의 위치
	bool is_selected;       // 선택 상태
	bool is_ally;          // 아군 건물 여부
} SELECTED_BUILDING;

typedef enum {
	BD_PLATE = 'P',    // 장판
	BD_DORM = 'D',     // 숙소
	BD_GARAGE = 'G',   // 창고
	BD_BARRACKS = 'B', // 병영
	BD_SHELTER = 'S'   // 은신처
} BUILDING_TYPE;

// 건물 정보 구조체 업데이트
typedef struct {
	BUILDING_TYPE type;     // 건물 타입
	const char* name;       // 건물 이름
	const char* shortcut;   // 단축키 설명
	int cost;              // 건설 비용
	int durability;        // 내구도
	const char* effect;     // 효과 설명
	const char* production; // 생산 유닛 정보
} BUILDING_INFO;

// 건설 상태 구조체
typedef struct {
	bool is_building_mode;  // 건설 모드 여부
	char building_type;     // 선택된 건물 타입
	int cursor_size;        // 커서 크기 (1x1 or 2x2)
} BUILDING_STATE;

extern BUILDING_STATE building_state;

typedef enum {
	UNIT_TYPE_HARVESTER,
	UNIT_TYPE_FREMEN,
	UNIT_TYPE_SOLDIER,
	UNIT_TYPE_TANK,
	UNIT_TYPE_SANDWORM
} UnitType;

typedef struct {
	UnitType type;
	const char* name;
	char symbol;
	bool can_be_ally;
	bool can_be_enemy;
	int cost;
} UnitInfo;

// 외부 참조 선언
extern const UnitInfo UNIT_INFO[];

// 하베스터 상태 열거형 정의
typedef enum {
	H_IDLE,              // 대기 상태
	H_MOVING,            // 이동 중
	H_READY_TO_HARVEST,  // 수확 준비 상태
	H_HARVESTING,        // 수확 중
	H_RETURNING          // 본진으로 귀환 중
} HARVESTER_STATE;

// 하베스터 구조체 정의
typedef struct {
	POSITION pos;              // 현재 위치
	POSITION target_pos;       // 목표 위치
	HARVESTER_STATE state;     // 현재 상태
	int health;               // 체력
	int spice_carried;        // 현재 운반 중인 스파이스 양
	int harvest_start_time;   // 수확 시작 시간
	bool is_selected;         // 선택 여부
	bool is_ally;            // 아군 여부
} HARVESTER;

#endif