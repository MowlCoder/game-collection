#include "Random.h"

#include "time.h"
#include <cstdlib>

void Random::init() {
    srand(time(0));
}

int Random::get(int max) {
    return rand() % max + 1;
}

int Random::getInRange(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}