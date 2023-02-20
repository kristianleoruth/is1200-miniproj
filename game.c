#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pic32mx.h>
#include "miniproj.h"

struct Coordinate {
    uint8_t x;
    uint8_t y;
};

struct Block {
    uint8_t shape;
    uint8_t rot;
    struct Coordinate origin;
};

char itoa (int i) {
    return i >> 29;
}

struct Block SpawnBlock() {
    struct Block b;
    srand(time(0));
    b.shape = rand() % 8 + 1;
    b.rot = 0;
    b.origin.x = 0;
    b.origin.y = 0;
    return b;
}

void GameUpdate() {
    struct Block curBlock = SpawnBlock();
    disp_Text(itoa(curBlock.shape), 0, 0);
    
    disp_Write();
}

