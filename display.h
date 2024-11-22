/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// 표시할 색상 정의. 대충 맞춰 뒀는데, 취향껏 추가하거나 변경하기
#define COLOR_DEFAULT   15      // 기본 색상
#define COLOR_CURSOR   112      // 커서 색상
#define COLOR_RESOURCE 112      // 자원 표시 색상
#define COLOR_BLUE     0x1F     // 파란색 (아군 본진)
#define COLOR_RED      0x4F     // 빨간색 (적군 본진)
#define COLOR_ORANGE   0x6F     // 주황색 (스파이스)
#define COLOR_BLACK    0x0F     // 검은색 (테두리)
#define FLOOR_BEIGE    0xE0     // 베이지색 (장판)
#define COLOR_GRAY     0x8F     // 회색 (바위)
#define COLOR_YELLOW   14       // 노란색 (샌드웜)
#define COLOR_GREEN    0x2F     // 초록색 (병영)
#define COLOR_CYAN     0x3F     // 청록색 (숙소)
#define COLOR_MAGENTA  0x5F     // 자주색 (창고)
#define COLOR_WHITE    0x7F     // 흰색 (은신처)

// 지금은 자원, 맵, 커서만 표시
// 앞으로 화면에 표시할 내용들 여기에 추가하기
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor
);


#endifㄴㅁ