#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "pic32mx.h"
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

uint8_t ground[32][128];

uint8_t blockSprites[] = {
	0,1,1,0,
	0,0,1,0,
	0,0,1,0,
	0,0,0,0,

	0,0,1,0,
	0,0,1,0,
	0,0,1,0,
	0,0,1,0,
};

struct Block curBlock;
uint8_t spawnNext = 1;

int queueRotation = 0;
int ticksPerFall = 10;
int ticks = 0;

char itoa(int i) {
	return i >> 29;
}
void SpawnBlock() {
	curBlock.shape = 1;
	curBlock.rot = 0;
	curBlock.origin.x = 100;
	curBlock.origin.y = 11;
}

void ResetDMat() {
	// Add code to render score and held block
	int i, j;
	for (i = 0; i < 32; i++) {
		for (j = 0; j < 128; j++) {
			d_mat[i][j] = 0;
		}
	}
}

/*
Param: 16 item uint8_t array
Effect: Get a 16 item array (representing 4x4) sprite based on rotation and shape of curBlock
*/
void GetShapeSprite(uint8_t s[16]) {
	// Calculate offset based on shape here 
	// Add code
	int offset = curBlock.shape * 16;


	int i, j;
	// Index manipulation based on rotation
	switch(curBlock.rot) {
		/* Pattern for 90 deg rotation:
		index: col# * 4 + 3 - row#
		*/
		case 1:
			for (i = 0; i < 16; i++) {
				s[i] = blockSprites[(i % 4) * 4 + 3 - i/4 + offset];
			}
			break;
		/* Pattern for 180 deg rot:
		index: 15 - elem#
		*/
		case 2: 
			for (i = 0, j = 15; i < 16; i++, j--) {
				s[i] = blockSprites[15 - i + offset];
			}
			break;
		/* Pattern for 270 deg rot:
		index: 3 - col# + (row# * 4)
		*/
		case 3:
			for (i = 0; i < 16; i++) {
				s[i] = blockSprites[(3 - (i % 4)) * 4 + i/4 + offset]; 
			}
			break;
		default: 
			for (i = 0; i < 16; i++) {
				s[i] = blockSprites[i + offset]; 
			}
			break;
	}
}

/*
Effect: Draws curBlock to d_mat
*/
void RenderBlock() {
	uint8_t sprite[16];
	int i, j;
	GetShapeSprite(sprite);

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			int _y = curBlock.origin.y + i*2, 
				_x = curBlock.origin.x + j*2;
			d_mat[_y][_x] = sprite[i*4 + j] || ground[_y][_x];
			d_mat[_y][_x + 1] = sprite[i*4 + j] || ground[_y][_x + 1];
			d_mat[_y + 1][_x] = sprite[i*4 + j] || ground[_y + 1][_x];
			d_mat[_y + 1][_x + 1] = sprite[i*4 + j] || ground[_y + 1][_x + 1];

			/* 3x3 block size
			// d_mat[curBlock.origin.y + i*3][curBlock.origin.x + j*3] = sprite[i*4 + j];
			// d_mat[curBlock.origin.y + i*3][curBlock.origin.x + j*3 + 1] = sprite[i*4 + j];
			// d_mat[curBlock.origin.y + i*3][curBlock.origin.x + j*3 + 2] = sprite[i*4 + j];

			// d_mat[curBlock.origin.y + i*3 + 1][curBlock.origin.x + j*3] = sprite[i*4 + j];
			// d_mat[curBlock.origin.y + i*3 + 1][curBlock.origin.x + j*3 + 1] = sprite[i*4 + j];
			// d_mat[curBlock.origin.y + i*3 + 1][curBlock.origin.x + j*3 + 2] = sprite[i*4 + j];

			// d_mat[curBlock.origin.y + i*3 + 2][curBlock.origin.x + j*3] = sprite[i*4 + j];
			// d_mat[curBlock.origin.y + i*3 + 2][curBlock.origin.x + j*3 + 1] = sprite[i*4 + j];
			// d_mat[curBlock.origin.y + i*3 + 2][curBlock.origin.x + j*3 + 2] = sprite[i*4 + j];

			 */
		}
	}
}

void Fall() {
	if (ticks == ticksPerFall) {
		curBlock.origin.x -= 1;
		ticks = 0;
	}
}

void RotateBlock() {
	/* Need to make more sophisticated with a rotatecheck */
	if (!queueRotation)
		return;

	if (curBlock.rot < 4)
		curBlock.rot += 1;
	else
		curBlock.rot = 0;

	queueRotation = 0;
}

/*
Helper struct for MoveBlock, stores coordinate and row to check if pixel is the left-/rightmost pixel in the shape
*/
struct _BlockCheck {
	struct Coordinate o;
	uint8_t row;
};

/*

*/
void MoveBlock(int x) {
	/*
		1. Get all pixels that are ON for the block 
			(16 item array, but 1:1 1 item:1 pixel, not a 4x4 sprite)
			Maybe this can be simplified to a 4 item array and we check the 2x2 square to the right and down of it
		2. Check for every item, that there is nothing to the left (allow left movement) 
			or nothing to the right (allow right movement)
			Note: We need to take the right- and left-most values of each row to avoid conflicts
		3. Move block's origin
	*/

	uint8_t sprite[16];
	GetShapeSprite(sprite);

	struct _BlockCheck activePixels[4];
	int i, c = 0;

	// Step 1
	for (i = 0; i < 16; i++) {
		if (!sprite[i])
			continue;
		// Calculate coordinate of pixel (origin + index)
		struct _BlockCheck pxl;
		pxl.o.x = curBlock.origin.x + (i % 4);
		pxl.o.y = curBlock.origin.y + (i/4);
		pxl.row = i/4;

		activePixels[c++] = pxl;
	}

	// Left Movement Check
	if (x < 0) {
		uint8_t canMoveLeft = 1;
		for (i = 0; i < 4; i++) {
			// Origin of 2x2 block to be checked
			struct _BlockCheck pxl = activePixels[i];
			uint8_t leftmost = 1;

			// Check that there are no blocks to the left (on the same row)
			for (c = 0; c < 4; c++) {
				struct _BlockCheck _pxl = activePixels[c];
				if (_pxl.row != pxl.row)
					continue;
				if (_pxl.o.y < pxl.o.y) // Is _pxl further left?
					leftmost = 0;
			}

			if (!leftmost)
				continue;
			
			if (pxl.o.y <= 0) {
				// Calculate offset from curBlock origin ?
				canMoveLeft = 0;
				break;
			}
			
			if (ground[pxl.o.y - 1][pxl.o.x]) { // Check pixel to the left
				canMoveLeft = 0;
				break;
			}
		}

		if (canMoveLeft)
			curBlock.origin.y -= 1;
	}

	// Right Movement Check
	if (x > 0) {
		uint8_t canMoveRight = 1;
		for (i = 0; i < 4; i++) {
			// Origin of 2x2 block to be checked
			struct _BlockCheck pxl = activePixels[i];
			uint8_t rightmost = 1;

			// Check that there are no blocks to the right (on the same row)
			for (c = 0; c < 4; c++) {
				struct _BlockCheck _pxl = activePixels[c];
				if (_pxl.row != pxl.row)
					continue;
				if (_pxl.o.y > pxl.o.y) // Is _pxl further right?
					rightmost = 0;
			}

			if (!rightmost)
				continue;
			
			if (pxl.o.y >= 26) {
				canMoveRight = 0;
				break;
			}
			
			if (ground[pxl.o.y + 2][pxl.o.x]) { // Check pixel to the right
				canMoveRight = 0;
				break;
			}
		}

		if (canMoveRight)
			curBlock.origin.y += 1;
	}
}

void InputHandler() {
	if (btn(4)) {
		MoveBlock(1);
	}
	else if (btn(3)) {
		MoveBlock(-1);
	}

	if (btn(2)) {
		queueRotation = 1;
	}
}

void game_Init() {
	int i, j;
	for (i = 0; i < 32; i++) {
		for (j = 0; j < 128; j++) {
			ground[i][j] = 0;
		}
	}
}

int ln(int x) {
	return (x - 1) - ((x - 1)^2)/2 + ((x - 1)^3)/3 - ((x - 1)^4)/4 + ((x-1)^5)/5;
}

void WriteNumber(uint8_t num, uint8_t page, uint8_t col) {
	char digits[4];
	digits[3] = 0xfe;

	int i, j = 0;
	for (i = 0; i < 3; i++) {
		if (num == 0) {
			digits[i] = 0xff;
			digits[3] = 0xff;
			break;
		}
		digits[i] = (char) (num % 10 + 0x30); // Store each digit, reverse order
		num /= 10;
	}

	if (digits[3] == 0xff)
		i--;

	while (i >= 0) {
		char str[2];
		str[0] = digits[i];
		str[1] = 0x0;
		disp_Text(str, page, col + j*8);
		i--;
		j++;
	}
}

void game_Update() {
	ticks++;
	ResetDMat();
	if (spawnNext) {
		SpawnBlock();
		spawnNext = 0;
	}

	Fall();
	InputHandler();
	RotateBlock();
	RenderBlock();

	WriteNumber(curBlock.origin.y, 0, 0);
	disp_Write();
}