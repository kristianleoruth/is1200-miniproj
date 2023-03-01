#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "pic32mx.h"
#include "miniproj.h"

#define LEFT_WALL 0
#define RIGHT_WALL 30
#define MAX_GAME_HEIGHT 100
#define TETRIS_SCORE 2

/* 
Todo: 
1. Store block fix DONE
2. Rotate block fix (cancel rotation if ground next to curBlock) DONE
3. Profiles
*/

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

typedef struct Profile {
	int score;
	char name[4];
} Profile;

uint8_t ground[32][128];

uint8_t blockSprites[] = {
	0,1,1,0,
	0,0,1,0,
	0,0,1,0,
	0,0,0,0,
	
	0,0,0,0,
	1,1,1,1,
	0,0,0,0,
	0,0,0,0,
	
	0,1,1,0,
	0,1,0,0,
	0,1,0,0,
	0,0,0,0,
	
	0,0,0,0,
	0,1,1,0,
	0,1,1,0,
	0,0,0,0,
	
	0,0,1,0,
	0,1,1,0,
	0,1,0,0,
	0,0,0,0,
	
	0,0,0,0,
	0,0,1,0,
	0,1,1,1,
	0,0,0,0,
	
	0,1,0,0,
	0,1,1,0,
	0,0,1,0,
	0,0,0,0,
};

Block curBlock;
Block storedBlock;
Block nextBlock;

uint8_t spawnNext = 1;
int score = 0;
int diff;
int isBlockStored = 0;
int MAX_SPEED = 1;

int moves = 0;
int rots = 0;

int menuDelay = 5;

Profile highScores[3];

/* 
moveAmt: Change to 1 for pixel by pixel movement, or 2 for the game to work correctly without unfillable holes
This variable allows us to fulfill the advanced project graphical requirement, according to Marco Chiesa

"The 2-by-2 pixel movement is OK as long as either:
 1) there is something else controlled by the user that moves in the x-y direction 1-by-1.
 2) you can decide the granularity of the game to be 1-by-1 (where you have some holes) or 2-by-2."
*/
int moveAmt = 2; 
// int rotDelay = 2;

int ticksPerFall = 5;
unsigned int ticks = 0;

typedef enum {
	Menu,
	DiffSelect,
	Scores,
	GameInit,
	Game,
	Pause,
	GameOver
} GameState;

GameState gameState = Menu;
int playSelect = 1;
int lost = 0;
uint8_t storedPrevious = 0;

int rand()
{
	return (ticks + rots + moves);
}

void WriteNumber(uint8_t num, uint8_t page, uint8_t col)
{
	char digits[4];
	digits[3] = 0xfe;

	int i, j = 0;
	for (i = 0; i < 3; i++) {
		digits[i] = (char) (num % 10 + 0x30);
		num /= 10;
		if (!num) {
			digits[i + 1] = 0xff;
			digits[3] = 0xff;
		}
	}
	i--;
	while (i >= 0) {
		char str[2];
		str[0] = digits[i--];
		str[1] = 0x00;
		disp_Text(str, page, col + j * 8);
		j++;
	}
}

void SpawnBlock()
{
	curBlock = nextBlock;
	curBlock.origin.x = 100;
	curBlock.origin.y = 16;

	// curBlock.shape = rand() & 0x7;
	nextBlock.shape = rand() % 7;
	nextBlock.origin.x = 110;
	nextBlock.origin.y = 26;

}




void StoreBlock()
{
	if (storedPrevious)
		return;
	
	if (isBlockStored) {
		Block t = curBlock;
		t.origin.x = 110;
		t.origin.y = 4;
		t.rot = 0;
		
		curBlock = storedBlock;
		storedBlock = t;
	}
	else {
		storedBlock = curBlock;
		storedBlock.rot = 0;
		storedBlock.origin.x = 110;
		storedBlock.origin.y = 4;
		spawnNext = 1;
		isBlockStored = 1;
	}

	storedPrevious = 1;
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
Helper for GetActivePixels, stores coordinate and row to check if pixel is the left-/rightmost pixel in the shape
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
	if (!isBlockStored)
		return;

	int i, j;
	_BlockPixel activePixels[4];
	GetActivePixels(activePixels, storedBlock);

	int leftb = 2, rightb = 11, lowb = 108, upb = 117;
	for (i = 0, j = 0; i < 9; i++, j++) {
		d_mat[i + leftb][upb] = 1;
		d_mat[i + leftb][lowb] = 1;
		d_mat[leftb][lowb + j] = 1;
		d_mat[rightb][lowb + j] = 1;
	}
	for (i = 0; i < 4; i++)
	{
		_BlockPixel pxl = activePixels[i];
		int _x, _y;
		_y = pxl.o.y;
		_x = pxl.o.x;

		d_mat[_y][_x] = 1;
		d_mat[_y][_x - 1] = 1;
		d_mat[_y + 1][_x] = 1;
		d_mat[_y + 1][_x - 1] = 1;
	}
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

void UpdateGround() {
	/*
	1. Write block to ground array
		- Check highest pixel for game loss condition
	2. Check ground for full rows (count and store row#)
	3. Loop over stored rows and shift higher rows into them
	4. Calc score
	*/
	// 1
	_BlockPixel activePixels[4];
	GetActivePixels(activePixels, curBlock);
	int i, j;
	_BlockPixel highestPxl = activePixels[0];
	for (i = 0; i < 4; i++) {
		_BlockPixel pxl = activePixels[i];
		if (pxl.o.x > highestPxl.o.x)
			highestPxl = pxl;
		ground[pxl.o.y][pxl.o.x] = 1;
		ground[pxl.o.y][pxl.o.x - 1] = 1;
		ground[pxl.o.y + 1][pxl.o.x] = 1;
		ground[pxl.o.y + 1][pxl.o.x - 1] = 1;
	}

	// 2
	// int _rows[8];
	int c = 0, r = 0;
	for (i = 0; i < 128; i++) {
		int full = 1;
		for (j = 0; j < 32; j++) {
			if (!ground[j][i]) {
				full = 0;
				break;
			}
		}
		if (!full)
			continue;
		if (!c)
			r = i;
		c++;
		// _rows[c++] = i;
	}

	// Check exit and loss condition
	if (!c) {
		if (highestPxl.o.x >= MAX_GAME_HEIGHT)
			lost = 1;
		return;
	}

	WriteNumber(c, 0, 50);

	// 3
	for (i = r; i < 128 - c; i++) {
		for (j = 0; j < 32; j++) {
			ground[j][i] = ground[j][i + c];
		}
	}

	// 4
	score += c / 2;
	if(ticksPerFall > MAX_SPEED) {
		ticksPerFall -= 1;
	}
	
	if (c == 8)
		score += TETRIS_SCORE;
}

void Fall()
{
	if (ticks % ticksPerFall == 0)
	{
		if (MoveCheckFall())
		{
			curBlock.origin.x -= 1;
		}
		else
		{
			storedPrevious = 0;
			UpdateGround();
			spawnNext = 1;
		}
	}
}

void RotateBlock()
{
	// if (!(ticks % rotDelay)) 
	// 	return;
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
		groundCollision |= 
			ground[pxl.o.y][pxl.o.x] || ground[pxl.o.y][pxl.o.x - 1] || 
			ground[pxl.o.y + 1][pxl.o.x] || ground[pxl.o.y + 1][pxl.o.x - 1];

		// groundCollision = groundCollision || ground[pxl.o.y - 2][pxl.o.x];
		// groundCollision = groundCollision || ground[pxl.o.y + 3][pxl.o.x];
		// groundCollision = groundCollision || ground[pxl.o.y - 2][pxl.o.x - 1];
		// groundCollision = groundCollision || ground[pxl.o.y + 3][pxl.o.x - 1];
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
		curBlock.origin.y += moveAmt;
	if (x < 0)
		curBlock.origin.y -= moveAmt;
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
	
	if (sw(1))
		gameState = Pause;

	if (btn(2))
	{
		RotateBlock();
	}

	if (btn(1))
		StoreBlock();
}

// Helper vars for Difficulty Menu
char diff_offset = 0;
char diff_currentLetter[2];
char name[] = {0,0,0,0};
int diff_index = 0;

void ClearProfile() {
	int i;
	for (i = 0; i < 4; i++) {
		name[i] = 0;
	}
	score = 0;
}

void StoreProfile() {
	// Check if we should update the high scores
	// Store score and name
	int i;
	for (i = 2; i >= 0; i--)
	{
		if (highScores[i].score > score) {
			i++;
			break;
		}
	}
	if (i == 3) {
		ClearProfile();
		return;
	}
	Profile p;
	for (i = 0; i < 4; i++) {
		p.name[i] = name[i];
	}
	p.score = score;
	highScores[i] = p;
	ClearProfile();
}

void DifficultyMenu() {
	int dialVal = adc_GetDial();
	PORTE = dialVal;
	float dialValF = (float)dialVal;
	dialValF = (dialValF/255.0)*128;
	dialVal = (int)dialValF;
	
	
	//display the currently chosen difficulty
	disp_Text("DIFFICULTY", 2, 25);
	int i, j;
	for (i = 24; i < 32; i++) {
		for(j = 0; j < 128; j++) {
			if(j < dialVal) {
				d_mat[i][j] = 1;
			} else {
				d_mat[i][j] = 0;
			}
			
		}
	}

	//offset calculation for currentLetter
	if(btn(2))
		diff_offset++;
	if(btn(3))
		diff_offset--;

	if(diff_offset > 25) {
		diff_offset = 0;
	} 
	if(diff_offset < 0) {
		diff_offset = 25;
	}
	

	diff_currentLetter[0] = 0x41 + diff_offset;
	
	//write Letter to name
	if(btn(4) && diff_index < 3 && !(ticks % menuDelay)) {
		name[diff_index] = diff_currentLetter[0];
		diff_index++;
	}	
	
	//display currentLetter and name
	disp_Text("NAME", 0, 0);
	disp_Text(diff_currentLetter, 0, 60);
	disp_Text(name, 0, 90);
	disp_Write();
	time_Tick();

	if(btn(1) && name[4] != 0x00) {
		diff_offset = 0;
		diff_currentLetter[2];
		diff_index = 0;
		if (sw(4))
			moveAmt = 1;
		else 
			moveAmt = 2;
		gameState = GameInit;
	}
}

void Init()
{
	nextBlock.shape = rand() % 7;
	nextBlock.origin.x = 100;
	nextBlock.origin.y = 18;

	score = 0;
	isBlockStored = 0;
	lost = 0;
	int i, j;
	for (i = 0; i < 32; i++)
	{
		for (j = 0; j < 128; j++)
		{
			ground[i][j] = 0;
		}
	}

	for (i = 0; i < 30; i++) {
		for (j = 0; j < 8; j++) {
			ground[i][j] = 1;
		}
	}
	SpawnBlock();
	gameState = Game;

}

int ln(int x)
{
	return (x - 1) - ((x - 1) ^ 2) / 2 + ((x - 1) ^ 3) / 3 - ((x - 1) ^ 4) / 4 + ((x - 1) ^ 5) / 5;
}

void ShowNextBlock()
{
	int i, j;
	_BlockPixel activePixels[4];
	GetActivePixels(activePixels, nextBlock);

	for (i = 0; i < 4; i++)
	{
		_BlockPixel pxl = activePixels[i];
		int _x, _y;
		_y = pxl.o.y;
		_x = pxl.o.x;

		d_mat[_y][_x] = 1;
		d_mat[_y][_x - 1] = 1;
		d_mat[_y + 1][_x] = 1;
		d_mat[_y + 1][_x - 1] = 1;
	}
}

void LoseMenu() {
	// Display name and score
	disp_Text("GAME OVER", 0, 32);
	disp_Text("SCORE", 2, 20);
	WriteNumber(score, 2, 70);
	disp_Text("BTN1 TO EXIT", 3, 20);

	if (btn(1) && !(ticks % menuDelay)) {
		gameState = Menu;
		StoreProfile();
		return;
	}
}

void ShowMenu(int playSelect)
{
	if(playSelect) {
			ResetDMat();
			disp_Text("O", 0, 30);
			disp_Text("PLAY", 0, 50);
			disp_Text("SCORES", 3, 50);

			if (btn(1) && !(ticks % menuDelay)) {
				name[0] = 0x00;
				gameState = DiffSelect;
			}
		}
		else {
			ResetDMat();
			disp_Text("O", 3, 30);
			disp_Text("PLAY", 0, 50);
			disp_Text("SCORES", 3, 50);

			if (btn(1) && !(ticks % menuDelay))
				gameState = Scores;
		}
}

void PauseMenu() {
	if (!sw(1) & !(ticks % 5)) {
		gameState = Game;
		return;
	}
	if (btn(1) && !(ticks % menuDelay)) {
		StoreProfile();
		gameState = Menu;
		return;
	}
	disp_Text("PAUSED", 0, 44);
	disp_Text("NAME", 1, 20);
	disp_Text(name, 1, 80);
	disp_Text("SCORE", 2, 20);
	WriteNumber(score, 2, 80);
	//for later implementation of pause menu graphics
	return;
}

void game_InitProfiles() {
	Profile p;
	p.score = 0;
	highScores[0] = p;
	highScores[1] = p;
	highScores[2] = p;
}

void ScoresMenu() {
	if (btn(1) && !(ticks % menuDelay)) {
		gameState = Menu;
		return;
	}

	if (!highScores[0].score) {
		disp_Text("NO SCORE", 1, 40);
		disp_Text("AVAILABLE", 2, 32);
		disp_Text("BTN1 RETURN", 3, 20);
		return;
	}

	disp_Text("SCORES", 0, 40);
	int i;
	for (i = 0; i < 3; i++) {
		disp_Text(highScores[i].name, i, 30);
		WriteNumber(highScores[i].score, i, 80);
	}
}

void GameUpdate() {
	if (lost)
		gameState = GameOver;
	ticks++;
	ResetDMat();
	if (spawnNext)
	{
		SpawnBlock();
		spawnNext = 0;
	}

	Fall();
	InputHandler();
	RenderBlock();
	ShowStoredBlock();
	ShowNextBlock();
	RenderGround();
}

void game_Loop()
{
	ResetDMat();
	ticks++;
	switch (gameState)
	{
		case Menu:
			ShowMenu(playSelect);
			if (btn(2))
				playSelect = !playSelect;	
			break;
		case DiffSelect:
			DifficultyMenu();
			break;
		case Scores:
			ScoresMenu();
			break;
		case GameInit:
			Init();
			break;
		case Game:
			GameUpdate();
			break;
		case Pause:
			PauseMenu();
			break;
		case GameOver:
			LoseMenu();
			break;
		default:
			break;
	}
	disp_Write();
}
