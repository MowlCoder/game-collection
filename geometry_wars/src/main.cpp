#include "Game.h"
#include "Random.h"

int main(int argc, char* argv[]) {
    Random::init();

    Game g("config.txt");
    g.run();

    return 0;
}