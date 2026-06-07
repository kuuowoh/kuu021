#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

// =============================
// 화면 크기 설정
// =============================
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define NUM           5
#define FISHTANK_WIDTH  100
#define FISHTANK_HEIGHT 200

// =============================
// 물고기 종류 (열거형)
// =============================
typedef enum {
    NORMAL_FISH,
    FAST_FISH,
    BIG_FISH,
    SLOW_FISH,
    SPECIAL_FISH
} FishType;

// =============================
// 어항 구조체
// =============================
typedef struct {
    FishType fishType;     // 물고기 종류
    int fish;              // 물고기 수
    int water;             // 물의 양 (0~100)
    int health;            // 체력 (0~100)
    int growth;            // 성장 수치
    int level;             // 물고기 레벨
    int isAlive;           // 1 = 살아있음, 0 = 죽음
    SDL_Texture* texture;  // 현재 표시할 물고기 이미지
} FishTank;

// =============================
// 전역 변수 선언 (extern)
// engine.c에서 실제 정의됨
// =============================
extern FishTank fishTanks[NUM];
extern int level;
extern int position;
extern bool running;
extern bool gameOver;
extern bool gameWin;
extern long startTime;
extern long lastUpdateTime;
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;
extern SDL_Texture* fishNormalTexture;
extern SDL_Texture* fishFastTexture;
extern SDL_Texture* fishBigTexture;
extern SDL_Texture* fishDeadTexture;
extern SDL_Texture* fishSlowTexture;
extern SDL_Texture* fishSpecialTexture;
extern int survivedCount;
extern int deadCount;
extern int score;
extern int highScore;      // 최고 점수
extern int totalLevelUps;  // 전체 레벨업 횟수

// =============================
// 함수 원형 선언
// =============================
bool         engine_init();
void         initGame();
void         updateGame();
void         renderGame();
void         handleInput(SDL_Event* e);
void         cleanupGame();
SDL_Texture* loadTexture(const char* path);