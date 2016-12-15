/*
 * tetris.c
 *
 *  Created on: Dec 1, 2014
 *      Author: taheller
 */


#include "tetris.h"

#if PL_CONFIG_HAS_TETRIS_GAME

#include "FDisp1.h"
#include "GDisp1.h"

const B_POSITION TETRIS_B[TETRIS_F_NUM][4]=
{
		{-1,0,0,0,1,0,2,0},
		{0,1,0,0,1,0,2,0},
		{-2,0,-1,0,0,0,0,1},
		{0,1,0,0,1,0,1,1},
		{-1,0,0,0,0,1,1,1},
		{-1,0,0,0,1,0,0,1},
		{-1,1,0,1,0,0,1,0}
};

char TETRIS_FIELD[TETRIS_X_BLOCKS][TETRIS_Y_BLOCKS];

uint8 run, countRND, resRND, numCurrent, smooth, buttons, buttonsHold, res;
int8 i, j;
B_POSITION currentB[4], rotationB[4];
F_DIMENSIONS currentF, rotationF;

void Tetris_DrawBlock(char x, char y, char color)
{
	GDisp1_DrawFilledBox(TETRIS_LEFTBOTTOM_Y+TETRIS_Y_DIRECTION+y*TETRIS_Y_DIRECTION*TETRIS_BLOCK_SIZE+smooth*TETRIS_Y_DIRECTION+TETRIS_BLOCK_SIZE*TETRIS_Y_DIRECTION_Z, TETRIS_LEFTBOTTOM_X+TETRIS_X_DIRECTION+x*TETRIS_X_DIRECTION*TETRIS_BLOCK_SIZE+TETRIS_BLOCK_SIZE*TETRIS_X_DIRECTION_Z, TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE, color);
}

void Tetris_DrawFigure(char color)
{
	for(i=0;i<4;i++)
	{
		Tetris_DrawBlock(currentF.x+currentF.blocks[i].bx,currentF.y+currentF.blocks[i].by,color);
	}
}

char Tetris_Move(void)
{
	signed char buf, change=0;
	rotationF.x=currentF.x;
	rotationF.y=currentF.y;
	for(i=0;i<4;i++)
	{
		rotationF.blocks[i].bx=currentF.blocks[i].bx;
		rotationF.blocks[i].by=currentF.blocks[i].by;
	}
	if(buttons==LEFT)
	{
		change=1;
		for(i=0;i<4;i++)
		{
			if(rotationF.x+rotationF.blocks[i].bx<=0)
			{
				change=0;
			}
		}
		if(change==1)
		{
			rotationF.x--;
		}
	}
	if(buttons==RIGHT)
	{
		change=1;
		for(i=0;i<4;i++)
		{
			if(rotationF.x+rotationF.blocks[i].bx>=TETRIS_X_BLOCKS-1)
			{
				change=0;
			}
		}
		if(change==1)
		{
			rotationF.x++;
		}
	}
	if(buttons==UP)
	{
		change=0;
		for(i=0;i<4;i++)
		{
			buf=rotationF.blocks[i].by;
			rotationF.blocks[i].by=-rotationF.blocks[i].bx;
			rotationF.blocks[i].bx=buf;
			if(rotationF.x+buf<change)
			{
				change=rotationF.x+buf;
			}
			if(rotationF.x+buf-TETRIS_X_BLOCKS+1>change)
			{
				change=rotationF.x+buf-TETRIS_X_BLOCKS+1;
			}
		}
		rotationF.x-=change;
	}
	change=1;
	for(i=0;i<4;i++)
	{
		if(rotationF.y+rotationF.blocks[i].by<change)
		{
			change=rotationF.y+rotationF.blocks[i].by;
		}
	}
	if(change==1)
	{
		for(i=0;i<4;i++)
		{
			if(TETRIS_FIELD[currentF.x+currentF.blocks[i].bx][currentF.y-1+currentF.blocks[i].by])
			{
				change=0;
			}
		}
		if(change==1)
		{
			rotationF.y--;
		}
	}
	if(change<0)
	{
		if(change<-1)
		{
			change=0;
		}
		else
		{
			change=1;
			currentF.y--;
		}
	}
	else
	{
		buf=1;
		for(i=0;i<4;i++)
		{
			if(TETRIS_FIELD[rotationF.x+rotationF.blocks[i].bx][rotationF.y+rotationF.blocks[i].by])
			{
				buf=0;
			}
		}
		currentF.y=rotationF.y;
		if(buf==1)
		{
			currentF.x=rotationF.x;
			currentF.y=rotationF.y;
			for(i=0;i<4;i++)
			{
				currentF.blocks[i].bx=rotationF.blocks[i].bx;
				currentF.blocks[i].by=rotationF.blocks[i].by;
			}
		}
	}
	if(change==0)
	{
		for(i=0;i<4;i++)
		{
			TETRIS_FIELD[currentF.x+currentF.blocks[i].bx][currentF.y+currentF.blocks[i].by]=1;
		}
	}
	return change;
}

void Tetris_Clear(void)
{
	char ymin,ymax,buf,jj;
	ymin=TETRIS_Y_BLOCKS;
	ymax=0;
	for(i=0;i<4;i++)
	{
		if(currentF.y+currentF.blocks[i].by<ymin)
		{
			ymin=currentF.y+currentF.blocks[i].by;
		}
		if(currentF.y+currentF.blocks[i].by>ymax)
		{
			ymax=currentF.y+currentF.blocks[i].by;
		}
	}
	for(j=ymax;j>=ymin;j--)
	{
		buf=0;
		for(i=0;i<TETRIS_X_BLOCKS;i++)
		{
			if(TETRIS_FIELD[i][j])buf++;
		}
		if(buf==TETRIS_X_BLOCKS)
		{
			for(jj=j+1;jj<TETRIS_Y_BLOCKS;jj++)
			{
				for(i=0;i<TETRIS_X_BLOCKS;i++)
				{
					if(TETRIS_FIELD[i][jj-1]!=TETRIS_FIELD[i][jj])
					{
						TETRIS_FIELD[i][jj-1]=TETRIS_FIELD[i][jj];
						if(TETRIS_FIELD[i][jj])
						{
							Tetris_DrawBlock(i,jj-1,GDisp1_COLOR_BLACK);
						}
						else
						{
							Tetris_DrawBlock(i,jj-1,GDisp1_COLOR_WHITE);
						}
					}
				}
			}
		}
	}
}

void Tetris_Init(void)
{
	countRND=0;
	run=1;
	smooth=255;
	buttons=0;
	buttonsHold=0;
	currentF.blocks=currentB;
	rotationF.blocks=rotationB;
    GDisp1_Clear();
	GDisp1_DrawBox(TETRIS_LEFTBOTTOM_Y+(TETRIS_Y_SIZE+2*TETRIS_BLOCK_SIZE)*TETRIS_Y_DIRECTION_Z, TETRIS_LEFTBOTTOM_X+TETRIS_X_SIZE*TETRIS_X_DIRECTION_Z, TETRIS_Y_SIZE+2*TETRIS_BLOCK_SIZE, TETRIS_X_SIZE, 1, SSD1307_PIXEL_BLACK);
	GDisp1_UpdateFull();
	for(i=0;i<TETRIS_X_BLOCKS;i++)
	{
		for(j=0;j<TETRIS_Y_BLOCKS;j++)
		{
			TETRIS_FIELD[i][j]=0;
		}
	}
}

void Tetris_DoG(OS_EVENT* KeyBoardQueue)
{
	if(run!=0)
	{
		if(smooth==255)
		{
			smooth=0;
			currentF.x=TETRIS_X_BLOCKS/2-1;
			currentF.y=TETRIS_Y_BLOCKS-1;
			numCurrent=resRND;
			resRND++;
			if(resRND>(TETRIS_F_NUM-1))
			{
				resRND=0;
			}
			for(i=0;i<4;i++)
			{
				currentF.blocks[i].bx=TETRIS_B[numCurrent][i].bx;
				currentF.blocks[i].by=TETRIS_B[numCurrent][i].by;
			}
			Tetris_DrawFigure(SSD1307_PIXEL_BLACK);
		}
		else
		{
			Tetris_DrawFigure(SSD1307_PIXEL_WHITE);
			if(smooth==0)
			{
				buttons=(uint8)OSQPend(KeyBoardQueue, 1, &res);
				if(res!=OS_NO_ERR) buttons=0;
				if(buttons==PUSH)
				{
					run=0;
				}
				else
				{
					if(!Tetris_Move())
					{
						//run=0;
						Tetris_DrawFigure(GDisp1_COLOR_BLACK);
						Tetris_Clear();
						smooth=255;//0;
					}
					else
					{
						smooth=TETRIS_BLOCK_SIZE-1;
						Tetris_DrawFigure(GDisp1_COLOR_BLACK);
					}
				}
			}
			else
			{
				smooth--;
				//Tetris_Move();
				Tetris_DrawFigure(GDisp1_COLOR_BLACK);
			}
		}
		GDisp1_UpdateFull();
	}
	else
	{
		buttons=(uint8)OSQPend(KeyBoardQueue, 1, &res);
		if(res!=OS_NO_ERR) buttons=0;
		if(buttons==PUSH)
		{
			Tetris_Init();
		}
	}
}

void Tetris_DoRND(void)
{
	if(countRND<(TETRIS_F_NUM-1))
	{
		countRND++;
	}
	else
	{
		countRND=0;
	}
	if(((PTGD & 0x07) != 0x07) && (buttonsHold == 0))
	{
		resRND=countRND;
		buttonsHold=1;
	}
	if(((PTGD & 0x07) == 0x07) && (buttonsHold == 1))
	{
		buttonsHold=0;
	}
}

#endif
