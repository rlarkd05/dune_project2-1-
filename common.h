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
#define MAP_WIDTH   60 //����
#define MAP_HEIGHT   18

#define MAX_HARVESTERS 10
#define HARVEST_TIME 4000    // 4��
#define MIN_SPICE_HARVEST 2  // �ּ� ��Ȯ��
#define MAX_SPICE_HARVEST 4  // �ִ� ��Ȯ��

#define DOUBLE_PRESS_INTERVAL 200
#define MAX_MSG_LINES 6  
#define MAX_UNITS 10  // population_max�� �ִ밪
/* ================= ��ġ�� ���� =================== */
// �ʿ��� ��ġ�� ��Ÿ���� ����ü
typedef struct {
	int row, column;
} POSITION;

// Ŀ�� ��ġ
typedef struct {
	POSITION previous;  // ���� ��ġ
	POSITION current;   // ���� ��ġ
} CURSOR;

// �Է� ������ Ű ����.
// �������� enum�� �����ߴµ�, ũ�� ����� ������ ���� �˻�
typedef enum {
	// k_none: �Էµ� Ű�� ����. d_stay(�� �����̴� ���)�� ����
	k_none = 0, k_up, k_right, k_left, k_down,
	k_quit,
	k_undef, // ���ǵ��� ���� Ű �Է�   
	k_space, // �����̽���
	k_esc, // escŰ
	k_h,
	k_b, // bŰ
	k_p, //pŰ
	k_d,
	k_g,
	k_s,
	k_m,
} KEY;

/* ================= ����ü ���� =================== */
typedef struct {
	POSITION pos1; // �ǹ� ũ�� 1
	POSITION pos2; // �ǹ� ũ�� 2
	POSITION pos3; // �ǹ� ũ�� 3
	POSITION pos4; // �ǹ� ũ�� 4
	char repr;     // �ǹ� �̸�
	int layer;     // ��� ° ���̾ ���°��� ���� <-- �ʿ��ϸ� ���
} OBJECT_BUILDING;

// DIRECTION�� KEY�� �κ�����������, �ǹ̸� ��Ȯ�ϰ� �ϱ� ���ؼ� �ٸ� Ÿ������ ����
typedef enum {
	d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION;


/* ================= ��ġ�� ����(2) =================== */
// ���Ǽ��� ���� �Լ���. KEY, POSITION, DIRECTION ����ü���� ���������� ��ȯ

// ���Ǽ� �Լ�
inline POSITION padd(POSITION p1, POSITION sp2) {
	POSITION p = { p1.row + sp2.row, p1.column + sp2.column };  // sp2�� ����
	return p;
}

// p1 - p2
inline POSITION psub(POSITION p1, POSITION p2) {
	POSITION p = { p1.row - p2.row, p1.column - p2.column };
	return p;
}

// ����Ű���� Ȯ���ϴ� �Լ�
#define is_arrow_key(k)      (k_up <= (k) && (k) <= k_down)

// ȭ��ǥ 'Ű'(KEY)�� '����'(DIRECTION)���� ��ȯ. ���� ���� �Ȱ����� Ÿ�Ը� �ٲ��ָ� ��
#define ktod(k)      (DIRECTION)(k)

// DIRECTION�� POSITION ���ͷ� ��ȯ�ϴ� �Լ�
inline POSITION dtop(DIRECTION d) {
	static POSITION direction_vector[] = { {0, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 0} };
	return direction_vector[d];
}

// p�� d �������� �̵���Ų POSITION
#define pmove(p, d)      (padd((p), dtop(d)))

/* ================= game data =================== */
typedef struct {
	int spice;      // ���� ������ �����̽�
	int spice_max;  // �����̽� �ִ� ���差
	int population; // ���� �α� ��
	int population_max;  // ���� ������ �α� ��
} RESOURCE;


// �밭 ����� ����. ��� �߰��ϸ鼭 ���� ������ ��
typedef struct {
	POSITION pos;      // ���� ��ġ
	POSITION dest;     // ������
	char repr;         // ������Ʈ�� ǥ�� ����
	int speed;         // �̵� �ӵ�
	int next_move_time; // ���� �̵� �ð�
} OBJECT_SAMPLE;

typedef struct {
	POSITION pos;      // ���� ��ġ
	POSITION dest;     // ������
	char repr;         // ������Ʈ�� ǥ�� ����
	int speed;         // �̵� �ӵ�
	int next_move_time; // ���� �̵� �ð�
} OBJECT_SAMPLE1;

typedef struct {
	POSITION pos1; // �ǹ� ũ�� 1
	POSITION pos2; // �ǹ� ũ�� 2
	POSITION pos3; // �ǹ� ũ�� 3
	POSITION pos4; // �ǹ� ũ�� 4
	char repr;     // �ǹ� �̸�
	int layer;     // ��� ° ���̾ ���°��� ���� <-- �ʿ��ϸ� ���
} UNIT;

typedef struct {
	char type;              // ���õ� �ǹ� Ÿ��
	POSITION position;      // ���õ� �ǹ��� ��ġ
	bool is_selected;       // ���� ����
	bool is_ally;          // �Ʊ� �ǹ� ����
} SELECTED_BUILDING;

typedef enum {
	BD_PLATE = 'P',    // ����
	BD_DORM = 'D',     // ����
	BD_GARAGE = 'G',   // â��
	BD_BARRACKS = 'B', // ����
	BD_SHELTER = 'S'   // ����ó
} BUILDING_TYPE;

// �ǹ� ���� ����ü ������Ʈ
typedef struct {
	BUILDING_TYPE type;     // �ǹ� Ÿ��
	const char* name;       // �ǹ� �̸�
	const char* shortcut;   // ����Ű ����
	int cost;              // �Ǽ� ���
	int durability;        // ������
	const char* effect;     // ȿ�� ����
	const char* production; // ���� ���� ����
} BUILDING_INFO;

// �Ǽ� ���� ����ü
typedef struct {
	bool is_building_mode;  // �Ǽ� ��� ����
	char building_type;     // ���õ� �ǹ� Ÿ��
	int cursor_size;        // Ŀ�� ũ�� (1x1 or 2x2)
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

// �ܺ� ���� ����
extern const UnitInfo UNIT_INFO[];

// �Ϻ����� ���� ������ ����
typedef enum {
	H_IDLE,              // ��� ����
	H_MOVING,            // �̵� ��
	H_READY_TO_HARVEST,  // ��Ȯ �غ� ����
	H_HARVESTING,        // ��Ȯ ��
	H_RETURNING          // �������� ��ȯ ��
} HARVESTER_STATE;

// �Ϻ����� ����ü ����
typedef struct {
	POSITION pos;              // ���� ��ġ
	POSITION target_pos;       // ��ǥ ��ġ
	HARVESTER_STATE state;     // ���� ����
	int health;               // ü��
	int spice_carried;        // ���� ��� ���� �����̽� ��
	int harvest_start_time;   // ��Ȯ ���� �ð�
	bool is_selected;         // ���� ����
	bool is_ally;            // �Ʊ� ����
} HARVESTER;

#endif