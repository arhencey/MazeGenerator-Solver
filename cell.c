/***************************************************************************
 * This cell class is meant to be used in conjunction with the maze class  *
 * to create and solve mazes. The cell structure contains information      *
 * about the existance of four walls, whether or not it has been visited,  *
 * and its value. The value of a cell is used to show its order in the     *
 * bredth first search used when solving a maze.                           *
 *                                                                         *
 * Author: Alan Hencey                                                     *
 **************************************************************************/

#include "cell.h"
#include <stdlib.h>
#include <assert.h>

struct cell
{
    /* 0 means there is no wall on that side. 1 means there is. */
    int top;
    int right;
    int bottom;
    int left;

    int row;
    int col;
    int value;
    int visited;
};

CELL *newCELL(int row,int col)
{
    CELL *c = (CELL *)malloc(sizeof(CELL));
    assert(c != 0);

    c->top = 1;
    c->right = 1;
    c->bottom = 1;
    c->left = 1;
    c->row = row;
    c->col = col;
    c->value = -1;
    c->visited = 0;

    return c;
}

int getCELLwall(CELL *c,int wall)
{
    /* |---|---|---|
       |   |   |   |
       |---|-0-|---|
       |   1 X 2   |
       |---|-3-|---|
       |   |   |   |
       |---|---|---| */

    assert(wall >= 0 && wall <= 3);

    if (wall == 0)      {return c->top;}
    else if (wall == 1) {return c->left;}
    else if (wall == 2) {return c->right;}
    else                {return c->bottom;}
}

void removeCELLwall(CELL *c,int wall)
{
    /* |---|---|---|
       |   |   |   |
       |---|-0-|---|
       |   1 X 2   |
       |---|-3-|---|
       |   |   |   |
       |---|---|---| */

    assert(wall >= 0 && wall <= 3);

    if (wall == 0)      {c->top = 0;}
    else if (wall == 1) {c->left = 0;}
    else if (wall == 2) {c->right = 0;}
    else if (wall == 3) {c->bottom = 0;}
}

void clearWALL(CELL *curCell,CELL *nextCell)
{
    if (getCELLrow(curCell) == getCELLrow(nextCell))
    {
        if (getCELLcol(curCell) < getCELLcol(nextCell))
        {
            removeCELLwall(curCell,2);
            removeCELLwall(nextCell,1);
        }
        else
        {
            removeCELLwall(curCell,1);
            removeCELLwall(nextCell,2);
        }
    }
    else
    {
        if (getCELLrow(curCell) < getCELLrow(nextCell))
        {
            removeCELLwall(curCell,3);
            removeCELLwall(nextCell,0);
        }
        else
        {
            removeCELLwall(curCell,0);
            removeCELLwall(nextCell,3);
        }
    }
}

int getCELLval(CELL *c) {return c->value;}

int setCELLval(CELL *c,int value)
{
    int oldVal = c->value;
    c->value = value;
    return oldVal;
}

int getCELLrow(CELL *c) {return c->row;}

int getCELLcol(CELL *c) {return c->col;}

int isCELLvisited(CELL *c) {return c->visited;}

void setCELLvisited(CELL *c) {c->visited = 1;}

void freeCELL(CELL *c)
{
    free(c);
}
