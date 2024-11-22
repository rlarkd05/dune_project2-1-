/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// ǥ���� ���� ����. ���� ���� �״µ�, ���ⲯ �߰��ϰų� �����ϱ�
#define COLOR_DEFAULT   15      // �⺻ ����
#define COLOR_CURSOR   112      // Ŀ�� ����
#define COLOR_RESOURCE 112      // �ڿ� ǥ�� ����
#define COLOR_BLUE     0x1F     // �Ķ��� (�Ʊ� ����)
#define COLOR_RED      0x4F     // ������ (���� ����)
#define COLOR_ORANGE   0x6F     // ��Ȳ�� (�����̽�)
#define COLOR_BLACK    0x0F     // ������ (�׵θ�)
#define FLOOR_BEIGE    0xE0     // �������� (����)
#define COLOR_GRAY     0x8F     // ȸ�� (����)
#define COLOR_YELLOW   14       // ����� (�����)
#define COLOR_GREEN    0x2F     // �ʷϻ� (����)
#define COLOR_CYAN     0x3F     // û�ϻ� (����)
#define COLOR_MAGENTA  0x5F     // ���ֻ� (â��)
#define COLOR_WHITE    0x7F     // ��� (����ó)

// ������ �ڿ�, ��, Ŀ���� ǥ��
// ������ ȭ�鿡 ǥ���� ����� ���⿡ �߰��ϱ�
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor
);


#endif����