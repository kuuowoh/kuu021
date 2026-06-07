#include "game.h"
#include <stdio.h>

// =============================
// 전역 변수 실제 정의
// =============================
FishTank fishTanks[NUM];

int level = 1;
int position = 0;

bool running = true;
bool gameOver = false;
bool gameWin = false;

long startTime = 0;
long lastUpdateTime = 0;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;

SDL_Texture* fishNormalTexture = NULL;
SDL_Texture* fishFastTexture = NULL;
SDL_Texture* fishBigTexture = NULL;
SDL_Texture* fishDeadTexture = NULL;
SDL_Texture* fishSlowTexture = NULL;
SDL_Texture* fishSpecialTexture = NULL;

int survivedCount = 0;
int deadCount = 0;
int score = 0;
int highScore = 0;      // 최고 점수
int totalLevelUps = 0;      // 전체 레벨업 횟수

// =============================
// 내부 함수 원형
// =============================
void renderText(const char* text, int x, int y);
void renderFishTanks();

// =============================
// engine_init() — SDL 초기화
// =============================
bool engine_init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return false;

    window = SDL_CreateWindow("Raising Fishes",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    if (TTF_Init() != 0)
        return false;

    font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 18);
    if (!font) return false;

    // fish.bmp 하나로 모든 어항 이미지 공유
    fishNormalTexture = loadTexture("fish.bmp");
    fishDeadTexture = loadTexture("fish_dead.bmp");
    fishFastTexture = fishNormalTexture;
    fishBigTexture = fishNormalTexture;
    fishSlowTexture = fishNormalTexture;
    fishSpecialTexture = fishNormalTexture;

    return true;
}

// =============================
// initGame() — 게임 데이터 초기화
// =============================
void initGame()
{
    FishType types[NUM] = {
        NORMAL_FISH, FAST_FISH, BIG_FISH, SLOW_FISH, SPECIAL_FISH
    };

    for (int i = 0; i < NUM; i++)
    {
        fishTanks[i].fishType = types[i];
        fishTanks[i].fish = 10;
        fishTanks[i].water = 100;
        fishTanks[i].health = 100;
        fishTanks[i].growth = 0;
        fishTanks[i].level = 1;
        fishTanks[i].isAlive = 1;
        fishTanks[i].texture = fishNormalTexture;
    }

    startTime = SDL_GetTicks();
    lastUpdateTime = startTime;
}

// =============================
// updateGame() — 게임 논리 업데이트
// =============================
void updateGame()
{
    long now = SDL_GetTicks();
    long elapsed = (now - lastUpdateTime) / 1000;

    if (elapsed > 0)
    {
        int alive = 0;
        survivedCount = 0;
        deadCount = 0;

        for (int i = 0; i < NUM; i++)
        {
            if (!fishTanks[i].isAlive)
            {
                deadCount++;
                continue;
            }

            // 종류별 물 소비량 / 성장속도
            int consume = 1;
            int growRate = 1;

            switch (fishTanks[i].fishType)
            {
            case NORMAL_FISH:
                consume = 1;
                growRate = 1;
                break;
            case FAST_FISH:
                consume = 2;
                growRate = 3;
                break;
            case BIG_FISH:
                consume = 3;
                growRate = 2;
                break;
            case SLOW_FISH:
                consume = 1;
                growRate = 1;
                break;
            case SPECIAL_FISH:
                consume = 2;
                growRate = 4;
                break;
            }

            // 물 감소
            fishTanks[i].water -= level * consume * (int)elapsed;
            if (fishTanks[i].water < 0) fishTanks[i].water = 0;

            // 물 부족 시 체력 감소
            if (fishTanks[i].water <= 0)
            {
                fishTanks[i].health -= 20 * (int)elapsed;
                if (fishTanks[i].health < 0) fishTanks[i].health = 0;
            }

            // 체력 0 → 사망
            if (fishTanks[i].health <= 0)
            {
                fishTanks[i].isAlive = 0;
                fishTanks[i].texture = fishDeadTexture;
                deadCount++;
                continue;
            }

            // 물 있으면 성장
            if (fishTanks[i].water > 0)
            {
                fishTanks[i].growth += growRate * (int)elapsed;
                fishTanks[i].fish += growRate * (int)elapsed;
            }

            // 성장 수치 100마다 레벨업
            if (fishTanks[i].growth >= 100 * fishTanks[i].level)
            {
                fishTanks[i].level++;
                fishTanks[i].growth = 0;
                totalLevelUps++;     // 레벨업 횟수 누적
            }

            survivedCount++;
            alive++;
        }

        // 점수 계산 (레벨업 횟수도 반영)
        score = survivedCount * 100 + level * 50 + totalLevelUps * 30;
        if (score > highScore) highScore = score;   // 최고 점수 갱신

        if (alive == 0)
            running = false;

        lastUpdateTime = now;
    }
}

// =============================
// renderGame() — 화면 그리기
// =============================
void renderGame()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    renderFishTanks();

    // 좌측 상단 게임 정보 표시
    char buf[64], buf2[64], buf3[64], buf4[64];
    sprintf_s(buf, sizeof(buf), "Level: %d  Score: %d", level, score);
    sprintf_s(buf2, sizeof(buf2), "Best Score: %d", highScore);
    sprintf_s(buf3, sizeof(buf3), "LevelUps: %d", totalLevelUps);
    sprintf_s(buf4, sizeof(buf4), "Dead: %d  Alive: %d", deadCount, survivedCount);
    renderText(buf, 10, 10);
    renderText(buf2, 10, 32);
    renderText(buf3, 10, 54);
    renderText(buf4, 10, 76);

    // 게임 오버 화면
    if (!running)
    {
        char a[64], b[64], c[64], d[64];
        sprintf_s(a, sizeof(a), "GAME OVER");
        sprintf_s(b, sizeof(b), "FINAL SCORE: %d", score);
        sprintf_s(c, sizeof(c), "BEST SCORE: %d", highScore);
        sprintf_s(d, sizeof(d), "Survived: %d  Dead: %d  LevelUps: %d",
            survivedCount, deadCount, totalLevelUps);
        renderText(a, 320, 200);
        renderText(b, 320, 235);
        renderText(c, 320, 270);
        renderText(d, 270, 305);
    }

    SDL_RenderPresent(renderer);
}

// =============================
// handleInput() — 키보드 입력 처리
// =============================
void handleInput(SDL_Event* e)
{
    if (e->type == SDL_KEYDOWN)
    {
        switch (e->key.keysym.sym)
        {
        case SDLK_j:
            if (position > 0) position--;
            break;

        case SDLK_l:
            if (position < NUM - 1) position++;
            break;

        case SDLK_k:
            if (fishTanks[position].isAlive)
            {
                fishTanks[position].water += 20;
                if (fishTanks[position].water > 100)
                    fishTanks[position].water = 100;

                fishTanks[position].health += 5;
                if (fishTanks[position].health > 100)
                    fishTanks[position].health = 100;
            }
            break;

        case SDLK_ESCAPE:
            running = false;
            break;
        }
    }
}

// =============================
// cleanupGame() — 자원 정리
// =============================
void cleanupGame()
{
    if (fishNormalTexture) SDL_DestroyTexture(fishNormalTexture);
    if (fishDeadTexture)   SDL_DestroyTexture(fishDeadTexture);
    TTF_Quit();
    SDL_Quit();
}

// =============================
// loadTexture() — BMP 이미지 로드
// =============================
SDL_Texture* loadTexture(const char* path)
{
    SDL_Surface* s = SDL_LoadBMP(path);
    if (!s) return NULL;
    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
    SDL_FreeSurface(s);
    return t;
}

// =============================
// renderText() — 텍스트 화면 출력
// =============================
void renderText(const char* text, int x, int y)
{
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* s = TTF_RenderText_Solid(font, text, white);
    if (!s) return;
    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
    SDL_Rect r = { x, y, s->w, s->h };
    SDL_RenderCopy(renderer, t, NULL, &r);
    SDL_FreeSurface(s);
    SDL_DestroyTexture(t);
}

// =============================
// renderFishTanks() — 어항 전체 그리기
// =============================
void renderFishTanks()
{
    const char* typeNames[5] = {
        "NORMAL", "FAST", "BIG", "SLOW", "SPECIAL"
    };

    for (int i = 0; i < NUM; i++)
    {
        int x = 30 + i * (FISHTANK_WIDTH + 30);

        // 어항 테두리
        SDL_Rect bowl = { x, 250, FISHTANK_WIDTH, FISHTANK_HEIGHT };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &bowl);

        // 물 (비율 반영, 30% 이하 빨간색)
        int waterHeight = fishTanks[i].water * FISHTANK_HEIGHT / 100;
        if (fishTanks[i].water <= 30)
            SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
        else
            SDL_SetRenderDrawColor(renderer, 50, 120, 200, 255);

        SDL_Rect water = {
            x + 1,
            250 + FISHTANK_HEIGHT - waterHeight,
            FISHTANK_WIDTH - 2,
            waterHeight
        };
        SDL_RenderFillRect(renderer, &water);

        // 물고기 이미지
        SDL_Texture* tex = fishTanks[i].texture;
        if (!fishTanks[i].isAlive) tex = fishDeadTexture;

        if (tex)
        {
            int size = 40 + fishTanks[i].level * 5;
            SDL_Rect fishRect = {
                x + (FISHTANK_WIDTH - size) / 2,
                310,
                size, size / 2
            };
            SDL_RenderCopy(renderer, tex, NULL, &fishRect);
        }

        // 선택된 어항 노란 테두리
        if (i == position)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderDrawRect(renderer, &bowl);
        }

        // 정보 텍스트
        char info1[32], info2[32], info3[32];
        sprintf_s(info1, sizeof(info1), "%s Lv.%d",
            typeNames[fishTanks[i].fishType], fishTanks[i].level);
        sprintf_s(info2, sizeof(info2), "W:%d HP:%d",
            fishTanks[i].water, fishTanks[i].health);
        sprintf_s(info3, sizeof(info3), "Fish:%d", fishTanks[i].fish);

        renderText(info1, x, 460);
        renderText(info2, x, 478);
        renderText(info3, x, 496);

        // 경고 표시
        if (!fishTanks[i].isAlive)
            renderText("DEAD", x + 25, 350);
        else if (fishTanks[i].water <= 30)
            renderText("!WARNING!", x + 5, 232);
        else if (fishTanks[i].health <= 30)
            renderText("! LOW HP", x + 5, 232);
    }
}