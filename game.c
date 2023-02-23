#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "pic32mx.h"
#include "miniproj.h"

#define LEFT_WALL 1
#define RIGHT_WALL 30

void WriteNumber(uint8_t num, uint8_t page, uint8_t col)
{
	char digits[4];
	digits[3] = 0xfe;

	int i, j = 0;
	for (i = 0; i < 3; i++)
	{
		if (num == 0)
		{
			digits[i] = 0xff;
			digits[3] = 0xff;
			break;
		}
		digits[i] = (char)(num % 10 + 0x30); // Store each digit, reverse order
		num /= 10;
	}

	if (digits[3] == 0xff)
		i--;

	while (i >= 0)
	{
		char str[2];
		str[0] = digits[i];
		str[1] = 0x0;
		disp_Text(str, page, col + j * 8);
		i--;
		j++;
	}
}

typedef struct Coordinate
{
	signed char x;
	signed char y;
} Coordinate;

typedef struct Block
{
	uint8_t shape;
	uint8_t rot;
	Coordinate origin;
} Block;

uint8_t ground[32][128];

uint8_t blockSprites[] = {
	0, 1, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 0, 0,

	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	1, 1, 1, 1,

	0, 1, 1, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 0, 0, 0,

	0, 0, 0, 0,
	0, 1, 1, 0,
	0, 1, 1, 0,
	0, 0, 0, 0,

	0, 0, 1, 0,
	0, 1, 1, 0,
	0, 1, 0, 0,
	0, 0, 0, 0,

	0, 0, 0, 0,
	0, 0, 1, 0,
	0, 1, 1, 1,
	0, 0, 0, 0,

	0, 1, 0, 0,
	0, 1, 1, 0,
	0, 0, 1, 0,
	0, 0, 0, 0,

};

Block curBlock;
Block storedBlock;

uint8_t spawnNext = 1;
int score = 0;
int diff;
int isBlockStored = 0;

int moves = 0;
int rots = 0;

int ticksPerFall = 5;
unsigned int ticks = 0;

int rand() {
	return (ticks + rots + moves) % 7;
}

void SpawnBlock()
{
	curBlock.shape = rand() % 7;
	// curBlock.shape = 1;
	curBlock.rot = 0;
	curBlock.origin.x = 80;
	curBlock.origin.y = 16;
}

void StoreBlock()
{
	storedBlock.shape = curBlock.shape;
	storedBlock.rot = curBlock.rot;
	storedBlock.origin.x = 110;
	storedBlock.origin.y = 4;
	isBlockStored = 1;
	spawnNext = 1;
}

void UseStoredBlock() {
	curBlock.shape = storedBlock.shape;
	curBlock.rot = storedBlock.rot;
	isBlockStored = 0;
}

void ResetDMat()
{
	// Add code to render score and held block
	int i, j;
	for (i = 0; i < 32; i++)
	{
		for (j = 0; j < 128; j++)
		{
			d_mat[i][j] = 0;
		}
	}
}

/*
Param: 16 item uint8_t array, block whose array sprite is to be determined
Effect: Get a 16 item array (representing 4x4) sprite based on rotation and shape of a given block
*/
void GetShapeSprite(uint8_t s[16], Block block)
{
	// Calculate offset based on shape here
	int offset = block.shape * 16;

	int i, j;
	// Index manipulation based on rotation
	switch (block.rot)
	{
	/* Pattern for 90 deg rotation:
	index: col# * 4 + 3 - row#
	*/
	case 1:
		for (i = 0; i < 16; i++)
		{
			s[i] = blockSprites[(i % 4) * 4 + 3 - i / 4 + offset];
		}
		break;
	/* Pattern for 180 deg rot:
	index: 15 - elem#
	*/
	case 2:
		for (i = 0, j = 15; i < 16; i++, j--)
		{
			s[i] = blockSprites[15 - i + offset];
		}
		break;
	/* Pattern for 270 deg rot:
	index: 3 - col# + (row# * 4)
	*/
	case 3:
		for (i = 0; i < 16; i++)
		{
			s[i] = blockSprites[(3 - (i % 4)) * 4 + i / 4 + offset];
		}
		break;
	default:
		for (i = 0; i < 16; i++)
		{
			s[i] = blockSprites[i + offset];
		}
		break;
	}
}

/*
Helper for MoveBlock, stores coordinate and row to check if pixel is the left-/rightmost pixel in the shape
*/
typedef struct _BlockPixel
{
	Coordinate o;
	uint8_t row;
} _BlockPixel;

void GetActivePixels(_BlockPixel activePixels[4], Block block)
{
	uint8_t sprite[16];
	GetShapeSprite(sprite, block);
	int i, c = 0;
	for (i = 0; i < 16; i++)
	{
		if (!sprite[i])
			continue;
		// Calculate coordinate of pixel (origin + index)
		_BlockPixel pxl;
		pxl.o.x = block.origin.x + (i % 4) * 2;
		pxl.o.y = block.origin.y + (i / 4) * 2;
		pxl.row = i / 4;

		activePixels[c++] = pxl;
	}
}

/*
Effect: Draws curBlock to d_mat
*/
void RenderBlock()
{
	int i, j;
	_BlockPixel activePixels[4];
	GetActivePixels(activePixels, curBlock);

	for (i = 0; i < 4; i++)
	{
		_BlockPixel pxl = activePixels[i];

		d_mat[pxl.o.y][pxl.o.x] = 1;
		d_mat[pxl.o.y][pxl.o.x - 1] = 1;
		d_mat[pxl.o.y + 1][pxl.o.x] = 1;
		d_mat[pxl.o.y + 1][pxl.o.x - 1] = 1;
	}
}

void ShowStoredBlock()
{
	if (isBlockStored)
	{
		int i, j;
		_BlockPixel activePixels[4];
		GetActivePixels(activePixels, storedBlock);

		for (i = 0; i < 4; i++)
		{
			_BlockPixel pxl = activePixels[i];

			d_mat[pxl.o.y][pxl.o.x] = 1;
			d_mat[pxl.o.y][pxl.o.x - 1] = 1;
			d_mat[pxl.o.y + 1][pxl.o.x] = 1;
			d_mat[pxl.o.y + 1][pxl.o.x - 1] = 1;
		}
	}
	return;
}

void RenderGround()
{
	int i, j;
	for (i = 0; i < 32; i++)
	{
		for (j = 0; j < 128; j++)
		{
			d_mat[i][j] |= ground[i][j];
		}
	}
}

int MoveCheckFall()
{

	Block desired = curBlock;
	desired.origin.x = curBlock.origin.x - 1;
	_BlockPixel activePixels[4];
	GetActivePixels(activePixels, desired);
	int i;
	for (i = 0; i < 4; i++)
	{

		_BlockPixel pxl = activePixels[i];
		if (pxl.o.x == 0 || ground[pxl.o.y][pxl.o.x - 1])
		{
			return 0;
		}
	}
	return 1;
}

void UpdateGround()
{
	_BlockPixel activePixels[4];
	GetActivePixels(activePixels, curBlock);
	int x, y, i;
	for (i = 0; i < 4; i++)
	{
		_BlockPixel pxl = activePixels[i];
		ground[pxl.o.y][pxl.o.x] = 1;
		ground[pxl.o.y][pxl.o.x - 1] = 1;
		ground[pxl.o.y + 1][pxl.o.x] = 1;
		ground[pxl.o.y + 1][pxl.o.x - 1] = 1;
	}

	int deleteAmt = 0; // Going to be max 4
	uint8_t r = 255;

	for (x = 0; x < 128; x += 2) {
		int rowFull = 1;
		for (y = 0; y < 32; y += 2) {
			if (!ground[y][x]) {
				rowFull = 0;
				break;
			}
		}
		if (!rowFull)
			continue;

		r = x;
		deleteAmt = 1;
		break;
	}

	if (!deleteAmt)
		return;

	for (i = 0; i < 3; i++) {
		int rowFull = 1;
		x = r + 2 + i*2;
		for (y = 0; y < 32; y++) {
			if (!ground[y][x]) {
				rowFull = 0;
				break;
			}
		}

		if (!rowFull)
			break;
		deleteAmt++;
	}

	WriteNumber(deleteAmt, 0, 15);

	int shamt = deleteAmt * 2;

	for (x = r + shamt; x < 128; x++) {
		for (y = 0; y < 32; y++) {
			ground[y][x - shamt] = ground[y][x];
		}
	}
}

void Fall()
{
	ticksPerFall = 500/adc_GetDial();
	if (ticks % ticksPerFall == 0)
	{
		if (MoveCheckFall() == 1)
		{
			curBlock.origin.x -= 1;
		}
		else if (MoveCheckFall() == 0)
		{
			UpdateGround();
			spawnNext = 1;
		}
	}
}

void RotateBlock()
{

	/* Check if rotation will intersect with a wall or ground
	1. Copy curBlock, change rotation of the duplicate
	2. Get active pixels of desired rotation
	3. Check walls, queue a move away from the wall
	4. Check active pixels of desired rotation with ground, if collision then cancel
		- If a move is queued, then cancel the move
	*/

	Block desired = curBlock;
	if (desired.rot < 3)
		desired.rot += 1;
	else
		desired.rot = 0;

	_BlockPixel activePixels[4];
	GetActivePixels(activePixels, desired);

	int i;
	int wallCollision = 0; // 1 = left wall, 30 = right wall
	int groundCollision = 0;
	for (i = 0; i < 4; i++)
	{
		_BlockPixel pxl = activePixels[i];

		// Check walls
		if (pxl.o.y <= LEFT_WALL)
		{
			// Set wall collision flag -1
			wallCollision = -1;
		}

		if (pxl.o.y >= RIGHT_WALL)
		{
			// Set wall collision flag 1
			wallCollision = 1;
		}

		// Check ground
		groundCollision = groundCollision && ground[pxl.o.y][pxl.o.x];
		groundCollision = groundCollision && ground[pxl.o.y][pxl.o.x - 1];
		groundCollision = groundCollision && ground[pxl.o.y + 1][pxl.o.x];
		groundCollision = groundCollision && ground[pxl.o.y + 1][pxl.o.x - 1];
	}

	if (groundCollision)
		return;
	if (!wallCollision)
	{
		curBlock.rot = desired.rot;
		return;
	}

	if (wallCollision > 0)
	{
		/* Calc curBlock origin distance from right wall
		1. Get rightmost active pixel
		2. Calculate offset from right wall
		3. Move curBlock origin left by distance
		*/
		int dist = 0;
		for (i = 0; i < 4; i++)
		{
			_BlockPixel pxl = activePixels[i];
			if (pxl.o.y - RIGHT_WALL > dist && pxl.o.y >= RIGHT_WALL)
				dist = pxl.o.y - RIGHT_WALL;
		}
		curBlock.origin.y -= dist;
	}

	if (wallCollision < 0)
	{
		/* Calc curBlock origin distance from left wall
		1. Get leftmost active pixel
		2. Calculate offset from left wall
		3. Move curBlock origin left by distance
		*/
		int dist = 0;
		for (i = 0; i < 4; i++)
		{
			_BlockPixel pxl = activePixels[i];
			if ((pxl.o.y - LEFT_WALL) * -1 > dist && pxl.o.y <= LEFT_WALL)
				dist = (pxl.o.y - LEFT_WALL) * -1;
		}
		curBlock.origin.y += dist;
	}

	curBlock.rot = desired.rot;
	rots++;
}

/*
	1. Get all pixels that are ON for the block
		(16 item array, but 1:1 1 item:1 pixel, not a 4x4 sprite)
		Maybe this can be simplified to a 4 item array and we check the 2x2 square to the right and down of it
	2. Check for every item, that there is nothing to the left (allow left movement)
		or nothing to the right (allow right movement)
		Note: We need to take the right- and left-most values of each row to avoid conflicts
Notes:
- 30 is the y value for a pixel to be drawn right on the right wall
- 1 is the value to be drawn on the left wall
*/

int MoveCheck(int x)
{
	_BlockPixel activePixels[4];
	int i, j = 0;

	GetActivePixels(activePixels, curBlock);

	if (x < 0)
	{
		uint8_t canMoveLeft = 1;

		for (i = 0; i < 4; i++)
		{
			_BlockPixel pxl = activePixels[i];
			uint8_t leftmost = 1;
			// Check if there are any blocks further left
			for (j = 0; j < 4; j++)
			{
				_BlockPixel _pxl = activePixels[j];
				if (_pxl.row != pxl.row)
					continue;
				if (_pxl.o.y < pxl.o.y)
					leftmost = 0;
			}
			if (!leftmost)
				continue;
			if (pxl.o.y <= LEFT_WALL)
			{
				// Calculate distance from 1
				// Add to curBlock
				canMoveLeft = 0;
				break;
			}

			if (ground[pxl.o.y - 1][pxl.o.x])
			{
				canMoveLeft = 0;
				break;
			}
		}
		if (canMoveLeft)
			return 1;
		return 0;
	}
	// Right Check
	if (x > 0)
	{
		uint8_t canMoveRight = 1;

		for (i = 0; i < 4; i++)
		{
			_BlockPixel pxl = activePixels[i];
			uint8_t rightmost = 1;
			// Check if there are any blocks further left
			for (j = 0; j < 4; j++)
			{
				_BlockPixel _pxl = activePixels[j];
				if (_pxl.row != pxl.row)
					continue;
				if (_pxl.o.y > pxl.o.y)
					rightmost = 0;
			}
			if (!rightmost)
				continue;
			if (pxl.o.y >= RIGHT_WALL)
			{
				// Calculate distance from 24
				// Subtract distance from curBlock
				canMoveRight = 0;
				break;
			}

			if (ground[pxl.o.y + 2][pxl.o.x])
			{
				canMoveRight = 0;
				break;
			}
		}

		if (canMoveRight)
			return 1;
		return 0;
	}
}
/*
Param: integer less than or more than 0, positive means move right, negative is move left
*/
void MoveBlock(int x)
{
	if (!MoveCheck(x))
		return;

	if (x > 0)
		curBlock.origin.y += 1;
	if (x < 0)
		curBlock.origin.y -= 1;
	moves++;
}

void InputHandler()
{
	if (btn(4))
	{
		MoveBlock(1);
	}
	else if (btn(3))
	{
		MoveBlock(-1);
	}

	if (btn(2))
	{
		RotateBlock();
	}
	if (btn(1))
	{
		if (isBlockStored && (curBlock.origin.x > 60))
		{
			UseStoredBlock();
		}
		else if (!(isBlockStored))
		{
			StoreBlock();
		}
	}
}

void game_Init()
{
	diff = 1;
	int i, j;
	for (i = 0; i < 32; i++)
	{
		for (j = 0; j < 128; j++)
		{
			ground[i][j] = 0;
		}
	}

	// for (i = 0; i < 30; i++) {
	// 	for (j = 0; j < 8; j++) {
	// 		ground[i][j] = 1;
	// 	}
	// }
}

int ln(int x)
{
	return (x - 1) - ((x - 1) ^ 2) / 2 + ((x - 1) ^ 3) / 3 - ((x - 1) ^ 4) / 4 + ((x - 1) ^ 5) / 5;
}

void game_Update()
{
	ticks++;
	ResetDMat();
	// disp_VerticalText("123", 100, 0);
	if (spawnNext)
	{
		SpawnBlock();
		spawnNext = 0;
	}

	Fall();
	InputHandler();
	RenderBlock();
	ShowStoredBlock();
	RenderGround();

	/* 	WriteNumber(curBlock.origin.y, 0, 30);
		WriteNumber(curBlock.origin.x, 0, 60); */
	// WriteNumber(score, 0, 90);
	// WriteNumber(isBlockStored, 0, 60);
	//WriteNumber(adc_GetDial(), 3, 30);
	disp_Write();
}