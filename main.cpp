// ============================================================
//  main.cpp  -  Entry point
//  CSE142 OOP Project - Spring 2026
// ============================================================
#include "Game.h"

int main() {
    Game game;

    while (!game.shouldClose()) {
        game.update();
        game.draw();
    }

    return 0;
}
