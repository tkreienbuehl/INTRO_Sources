/*
 * tetris.h
 *
 *  Created on: Dec 1, 2014
 *      Author: taheller
 */

#ifndef TETRIS_H_
#define TETRIS_H_

#include "Platform.h"

#if PL_CONFIG_HAS_TETRIS_GAME

void Tetris_Init(void);
void Tetris_DoG(OS_EVENT* KeyBoardQueue);
void Tetris_DoRND(void);

#define TETRIS_LEFTBOTTOM_X		(32)
#define TETRIS_LEFTBOTTOM_Y 	(96)
#define TETRIS_X_DIRECTION		(-1)
#define TETRIS_X_DIRECTION_Z	((TETRIS_X_DIRECTION-1)/2)
#define TETRIS_Y_DIRECTION		(-1)
#define TETRIS_Y_DIRECTION_Z	((TETRIS_Y_DIRECTION-1)/2)
#define TETRIS_BLOCK_SIZE		(3)
#define TETRIS_X_BLOCKS			(10)
#define TETRIS_Y_BLOCKS			(20)
#define TETRIS_X_SIZE			(TETRIS_BLOCK_SIZE*TETRIS_X_BLOCKS+2)
#define TETRIS_Y_SIZE			(TETRIS_BLOCK_SIZE*TETRIS_Y_BLOCKS+2)
#define TETRIS_F_NUM			(7)

enum BUTTONS_KEY
{
	LEFT	=	1,
	RIGHT	=	2,
	UP		=	3,
	DOWN	=	4,
	PUSH	=	5
};

typedef struct
{
	signed char bx;
	signed char by;
} B_POSITION;

typedef struct
{
	signed char x;
	signed char y;
	B_POSITION* blocks;
} F_DIMENSIONS;

#endif //PL_CONFIG_HAS_TETRIS_GAME

#endif /* TETRIS_H_ */
