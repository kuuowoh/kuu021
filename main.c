#include "game.h"

// =============================
// main() — 프로그램 시작점
// =============================
int main(int argc, char* argv[])
{
    if (!engine_init())  // SDL 초기화 실패 시 종료
        return 1;

    initGame();          // 게임 데이터 초기화

    // =============================
    // 게임 루프
    // 입력처리 → 게임업데이트 → 화면그리기
    // =============================
    while (running)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)  // 창 X 버튼
                running = false;

            handleInput(&event);         // 키보드 입력 처리
        }

        updateGame();    // 물 감소, 성장, 사망 판정
        renderGame();    // 화면 그리기

        SDL_Delay(100);  // 0.1초 대기 (약 10FPS)
    }

    cleanupGame();       // 자원 정리 후 종료

    return 0;
}