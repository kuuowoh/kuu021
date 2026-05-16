#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

#define NUM 6
#define MAX_WATER 100
#define WATER_AMOUNT 3

// 구조체 선언
typedef struct {
    int water;         // 물 높이
    int isAlive;       // 생존 여부
    int fishSize;      // 물고기 크기
} FishTank;

// 전역 변수 선언
int level;
FishTank tanks[NUM];

long prevElapsedTime;
long totalElapsedTime;
long startTime = 0;

FishTank* cursor;

int position = 0;

// 함수 프로토타입 선언
void initData();
void update();
void render();
void decreaseWater(long elapsedTime);
int checkFishAlive();
int checkWin();
void gotoxy(int x, int y);
int nonBlockingGetch();

// 입력 처리 관련 함수
void processInput();
void moveCursorLeft();
void moveCursorRight();
void giveWater();

int main(void) {

    int gameOver = 0;

    initData();

    startTime = clock();

    cursor = tanks;

    while (!gameOver) {

        position = cursor - tanks;

        // 키 입력 처리
        processInput();

        // 게임 데이터 업데이트
        update();

        position = cursor - tanks;

        // 화면 출력
        render();

        // 레벨업 체크
        if (totalElapsedTime / 20 > level - 1) {

            level++;

            // 물고기 성장
            for (int i = 0; i < NUM; i++) {
                tanks[i].fishSize++;
            }

            gotoxy(10, 3);
            printf("축하합니다. 레벨업이 되었습니다.");
        }

        // 승리 체크
        if (checkWin()) {
            gameOver = 1;
        }

        // 패배 체크
        if (checkFishAlive() == 0) {

            gotoxy(2, 3);
            printf("모든 물고기가 죽었습니다. 게임 종료");

            gameOver = 1;
        }

        prevElapsedTime = totalElapsedTime;

        Sleep(100);
    }

    getchar();

    system("cls");

    return 0;
}

// 게임 초기화 함수
void initData() {

    level = 1;

    prevElapsedTime = 0;

    totalElapsedTime = 0;

    for (int i = 0; i < NUM; i++) {

        tanks[i].water = 100;

        tanks[i].isAlive = 1;

        tanks[i].fishSize = 1;
    }
}

// 입력 처리 함수
void processInput() {

    int input;

    input = nonBlockingGetch();

    if (input == -1) {
        return;
    }

    switch (input) {

    case 'j':

        moveCursorLeft();

        break;

    case 'l':

        moveCursorRight();

        break;

    case 'k':

        giveWater();

        break;

    default:

        gotoxy(2, 12);

        printf("잘못된 입력입니다.");

        break;
    }
}

// 왼쪽 이동
void moveCursorLeft() {

    if (cursor > tanks) {
        cursor--;
    }
}

// 오른쪽 이동
void moveCursorRight() {

    if (cursor < tanks + NUM - 1) {
        cursor++;
    }
}

// 물 주기
void giveWater() {

    if (cursor->isAlive == 0) {
        return;
    }

    cursor->water += WATER_AMOUNT;

    if (cursor->water > MAX_WATER) {
        cursor->water = MAX_WATER;
    }
}

// 데이터 업데이트
void update() {

    // 총 경과 시간
    totalElapsedTime = (clock() - startTime) / CLOCKS_PER_SEC;

    // 최근 경과 시간
    prevElapsedTime = totalElapsedTime - prevElapsedTime;

    decreaseWater(prevElapsedTime);
}

// 화면 출력
void render() {

    system("cls");

    // 제목
    gotoxy(2, 2);
    printf("@물고기 키우기 프로젝트@");

    // 레벨
    gotoxy(2, 3);
    printf("Level : %d", level);

    // 총 시간
    gotoxy(2, 4);
    printf("총 경과 시간 : %ld초", totalElapsedTime);

    // 최근 시간
    gotoxy(2, 5);
    printf("최근 경과 시간 : %ld초", prevElapsedTime);

    // 번호
    gotoxy(2, 6);

    for (int i = 0; i < NUM; i++) {
        printf(" %3d", i);
    }

    // 물 높이
    gotoxy(2, 7);

    for (int i = 0; i < NUM; i++) {
        printf(" %3d", tanks[i].water);
    }

    // 물고기 크기
    gotoxy(2, 8);

    for (int i = 0; i < NUM; i++) {
        printf(" %3d", tanks[i].fishSize);
    }

    // 생존 여부
    gotoxy(2, 9);

    for (int i = 0; i < NUM; i++) {

        if (tanks[i].isAlive == 1) {
            printf("  O ");
        }
        else {
            printf("  X ");
        }
    }

    // 커서 위치
    gotoxy(2, 10);

    printf("   ");

    for (int i = 0; i < position; i++) {
        printf("    ");
    }

    printf("*");

    // 조작 방법
    gotoxy(2, 12);

    printf("왼쪽(j)  물주기(k)  오른쪽(l)");
}

// 물 감소 함수
void decreaseWater(long elapsedTime) {

    for (int i = 0; i < NUM; i++) {

        // 죽은 물고기는 제외
        if (tanks[i].isAlive == 0) {
            continue;
        }

        // 크기가 클수록 물 더 빨리 감소
        tanks[i].water -= (level * tanks[i].fishSize) * (int)elapsedTime;

        // 물이 0 이하라면 죽음 처리
        if (tanks[i].water <= 0) {

            tanks[i].water = 0;

            tanks[i].isAlive = 0;
        }
    }
}

// 승리 체크
int checkWin() {

    if (level == 5) {

        gotoxy(2, 3);

        printf("최고 레벨 달성! 게임 종료");

        return 1;
    }

    return 0;
}

// 생존 체크
int checkFishAlive() {

    for (int i = 0; i < NUM; i++) {

        if (tanks[i].isAlive == 1) {
            return 1;
        }
    }

    return 0;
}

// 커서 위치 이동 함수
void gotoxy(int x, int y) {

    COORD CursorPosition = { x, y };

    SetConsoleCursorPosition(
        GetStdHandle(STD_OUTPUT_HANDLE),
        CursorPosition
    );
}

// 키 입력 함수
int nonBlockingGetch() {

    if (_kbhit()) {

        return _getch();
    }

    return -1;
}
