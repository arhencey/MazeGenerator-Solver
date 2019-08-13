/***************************************************************************
 * This maze class uses the cell class to create a two-dimensional array   *
 * of cells that together form a maze. The maze structure also supports    *
 * the creation, solving, and drawing of a maze. A pseudo-random number    *
 * generator can also be seeded to generate different mazes of the same    *
 * dimensions. Mazes are created through a depth-first search and solved   *
 * through a breadth-first search.                                         *
 *                                                                         *
 * Author: Alan Hencey                                                     *
 **************************************************************************/

#include "maze.h"
#include "cell.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "stack.h"
#include "queue.h"
#include "da.h"

// Necissary for random() and srandom() to work.
long int random(void);
void srandom(unsigned int seed);

struct maze
{
    CELL ***cells;
    int rows;
    int cols;
};

static MAZE *newMAZE(int rows,int cols);
static DA *makePosWallsArray(MAZE *m,int curRow,int curCol);
static void writeMAZE(MAZE *m,FILE *fp);
static MAZE *readMAZE(FILE *fp);
static int isWallEligible(MAZE *m,CELL *c,int wall);
static int isPathClear(MAZE *m,CELL *c,int wall);
static void freeMAZE(MAZE *m);

void createMAZE(int rows,int cols,int randSeed,FILE *fp)
{
    /* use DA to store possible cells to index into. use stack to keep
       track of previously visited cells. top cell has lowest row number
       so it gets priority.*/
    assert(rows > 0 && cols > 0);

    srandom(randSeed);
    int curRow = 0;
    int curCol = 0;

    MAZE *m = newMAZE(rows,cols);
    STACK *s = newSTACK();

    removeCELLwall(m->cells[curRow][curCol],1);
    setCELLvisited(m->cells[curRow][curCol]);
    push(s,m->cells[curRow][curCol]);
    while (sizeSTACK(s))
    {
        DA *posWalls = makePosWallsArray(m,curRow,curCol);

        if (sizeDA(posWalls))
        {
            int index = random() % sizeDA(posWalls);
            CELL *nextCell = getDA(posWalls,index);
            clearWALL(m->cells[curRow][curCol],nextCell);
            setCELLvisited(nextCell);
            push(s,nextCell);
            curRow = getCELLrow(nextCell);
            curCol = getCELLcol(nextCell);
            freeDA(posWalls);
        }
        else
        {
            freeDA(posWalls);
            CELL *nextCell = peekSTACK(s);
            curRow = getCELLrow(nextCell);
            curCol = getCELLcol(nextCell);
            DA *nextWalls = makePosWallsArray(m,curRow,curCol);
            /* If previous cell has more eligible neighbors we don't want to
               pop it as it won't ever be able to be pushed back on the
               stack. Instead, peek the previous cell and check for eligible
               neighbors. */
            if (sizeDA(nextWalls))
            {
                int index = random() % sizeDA(nextWalls);
                nextCell = getDA(nextWalls,index);
                clearWALL(m->cells[curRow][curCol],nextCell);
                setCELLvisited(nextCell);
                freeDA(nextWalls);
                push(s,nextCell);
                curRow = getCELLrow(nextCell);
                curCol = getCELLcol(nextCell);
            }
            // If the previous cell has no eligible neighbors, then pop.
            else
            {
                freeDA(nextWalls);
                nextCell = pop(s);
            }
        }
    }
    removeCELLwall(m->cells[rows-1][cols-1],2);
    writeMAZE(m,fp);
    freeSTACK(s);
    freeMAZE(m);
}

void drawMAZE(FILE *fp)
{
    /* Maze will be stored in a file with the following format:

       rows cols
       left value left value left value right
       bottom bottom bottom
       left value left value left value right
       bottom bottom bottom
       left value left value left value right

       drawMAZE will use rows, cols, and cell wall values to draw the maze.

       For example:

       3 3                                 -------------
       0 0 1 3 0 4 1                         0 | 3   4 |
       0 0 0              ----------->     -   -   -   -
       1 1 0 2 1 5 1      ----------->     | 1   2 | 5 |
       1 1 0                               ---------   -
       1 -1 0 -1 0 6 0                     |         6
                                           -------------
    */

    int rows = 0;
    int cols = 0;
    int wall = 0;
    int value = 0;
    fscanf(fp,"%d",&rows);
    fscanf(fp,"%d",&cols);

    // Draw top wall of maze.
    for (int k = 0; k < cols; ++k)
        printf("----");
    printf("-\n");

    // Draw interior of maze.
    for (int i = 0; i < (rows * 2) - 1; ++i)
    {
        if (i % 2 == 0) // Draw left and right cell walls.
        {
            for (int j = 0; j < cols; ++j)
            {
                fscanf(fp,"%d",&wall);
                if (wall) {printf("|");}
                else {printf(" ");}

                fscanf(fp,"%d",&value);
                if (value > -1) {printf(" %d ",value);}
                else {printf("   ");}
            }
            fscanf(fp,"%d",&wall);
            if (wall) {printf("|");}
            else {printf(" ");}
        }
        else // Draw top/bottom cell walls.
        {
            printf("-");
            for (int j = 0; j < cols; ++j)
            {
                fscanf(fp,"%d",&wall);
                if (wall) {printf("----");}
                else {printf("   -");}
            }
        }
        printf("\n");
    }

    // Draw bottom wall of maze.
    for (int k = 0; k < cols; ++k)
        printf("----");
    printf("-\n");
}

void solveMAZE(FILE *readFile,FILE *writeFile)
{
    MAZE *m = readMAZE(readFile);
    QUEUE *q = newQUEUE();
    int curRow = 0, curCol = 0;
    CELL *curCell = m->cells[curRow][curCol];
    setCELLval(curCell,0);
    enqueue(q,curCell);

    while (curRow != m->rows - 1 || curCol != m->cols - 1)
    {
        curCell = dequeue(q);
        curRow = getCELLrow(curCell);
        curCol = getCELLcol(curCell);
        setCELLvisited(curCell);

        // If top cell is eligible to be visited, enqueue and set step val.
        if (isPathClear(m,curCell,0))
        {
            enqueue(q,m->cells[curRow-1][curCol]);
            setCELLval(m->cells[curRow-1][curCol],
                (getCELLval(curCell)+1)%10);
        }

        // If left cell is eligible to be visited, enqueue and set step val.
        if (isPathClear(m,curCell,1))
        {
            enqueue(q,m->cells[curRow][curCol-1]);
            setCELLval(m->cells[curRow][curCol-1],
                (getCELLval(curCell)+1)%10);
        }

        // If right cell is eligible to be visited, enqueue and set step val.
        if (isPathClear(m,curCell,2))
        {
            enqueue(q,m->cells[curRow][curCol+1]);
            setCELLval(m->cells[curRow][curCol+1],
                (getCELLval(curCell)+1)%10);
        }

        // If bottom cell eligible to be visited, enqueue and set step val.
        if (isPathClear(m,curCell,3))
        {
            enqueue(q,m->cells[curRow+1][curCol]);
            setCELLval(m->cells[curRow+1][curCol],
                (getCELLval(curCell)+1)%10);
        }
    }
    while(sizeQUEUE(q)) {setCELLval(dequeue(q),-1);}
    freeQUEUE(q);
    writeMAZE(m,writeFile);
    freeMAZE(m);
}

MAZE *newMAZE(int rows,int cols)
{
    MAZE *m = (MAZE *)malloc(sizeof(MAZE));
    assert(m != 0);

    m->cells = malloc(sizeof(CELL **) * rows);
    for (int i = 0; i < rows; ++i)
    {
        m->cells[i] = malloc(sizeof(CELL *) * cols);
        for (int j = 0; j < cols; ++j)
            m->cells[i][j] = newCELL(i,j);
    }

    m->rows = rows;
    m->cols = cols;

    return m;
}

DA *makePosWallsArray(MAZE *m,int curRow,int curCol)
{
    DA *posWalls = newDA();

    // If top wall is eligible for removal, add to array.
    if (isWallEligible(m,m->cells[curRow][curCol],0))
        insertDAback(posWalls,m->cells[curRow-1][curCol]);

    // If left wall is eligible for removal, add to array.
    if (isWallEligible(m,m->cells[curRow][curCol],1))
        insertDAback(posWalls,m->cells[curRow][curCol-1]);

    // If right wall is eligible for removal, add to array.
    if (isWallEligible(m,m->cells[curRow][curCol],2))
        insertDAback(posWalls,m->cells[curRow][curCol+1]);

    // If bottom wall is eligible for removal, add to array.
    if (isWallEligible(m,m->cells[curRow][curCol],3))
        insertDAback(posWalls,m->cells[curRow+1][curCol]);

    return posWalls;
}

void writeMAZE(MAZE *m,FILE *fp)
{
    /*
    *  Maze will be stored in a file with the following format:
    *
    *  rows cols
    *  left value left value left value right
    *  bottom bottom bottom
    *  left value left value left value right
    *  bottom bottom bottom
    *  left value left value left value right
    */

    fprintf(fp,"%d %d\n",m->rows,m->cols);
    for (int i = 0; i < m->rows; ++i)
    {
        for (int j = 0; j < m->cols; ++j)
        {
            fprintf(fp,"%d ",getCELLwall(m->cells[i][j],1));
            fprintf(fp,"%d ",getCELLval(m->cells[i][j]));
        }
        fprintf(fp,"%d\n",getCELLwall(m->cells[i][m->cols-1],2));

        if (i < m->rows - 1)
        {
            for (int j = 0; j < m->cols; ++j)
            {
                fprintf(fp,"%d",getCELLwall(m->cells[i][j],3));
                if (j < m->cols - 1) {fprintf(fp," ");}
            }
            fprintf(fp,"\n");
        }
    }
}

MAZE *readMAZE(FILE *fp)
{
    int rows, cols = 0;
    int wall, value = 0;
    fscanf(fp,"%d %d",&rows,&cols);
    MAZE *m = newMAZE(rows,cols);

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            fscanf(fp,"%d",&wall);
            if (!wall)
            {
                removeCELLwall(m->cells[i][j],1);
                if (j > 0) {removeCELLwall(m->cells[i][j-1],2);}
            }
            fscanf(fp,"%d",&value);
            setCELLval(m->cells[i][j],value);
        }
        fscanf(fp,"%d",&wall);
        if (!wall) {removeCELLwall(m->cells[i][cols-1],2);}

        if (i < rows -1)
        {
            for (int j = 0; j < cols; ++j)
            {
                fscanf(fp,"%d",&wall);
                if (!wall)
                {
                    removeCELLwall(m->cells[i][j],3);
                    if (i < rows - 1) {removeCELLwall(m->cells[i+1][j],0);}
                }
            }
        }
    }
    return m;
}

int isWallEligible(MAZE *m,CELL *c,int wall)
{
    assert(wall >= 0 && wall <= 3);

    if (wall == 0)
    {
        if (getCELLrow(c) == 0) {return 0;}
        else if (isCELLvisited(m->cells[getCELLrow(c)-1][getCELLcol(c)]))
            return 0;
        else {return 1;}
    }
    else if (wall == 1)
    {
        if (getCELLcol(c) == 0) {return 0;}
        else if (isCELLvisited(m->cells[getCELLrow(c)][getCELLcol(c)-1]))
            return 0;
        else {return 1;}
    }
    else if (wall == 2)
    {
        if (getCELLcol(c) == m->cols - 1) {return 0;}
        else if (isCELLvisited(m->cells[getCELLrow(c)][getCELLcol(c)+1]))
            return 0;
        else {return 1;}
    }
    else //wall == 3
    {
        if (getCELLrow(c) == m->rows - 1) {return 0;}
        else if (isCELLvisited(m->cells[getCELLrow(c)+1][getCELLcol(c)]))
            return 0;
        else {return 1;}
    }
}

int isPathClear(MAZE *m,CELL *c,int wall)
{
    /* If the cell wall is an edge of the maze, is a solid wall, or is
       already visited, return 0. Otherwise the path is eligible to move
       in to so return 1.*/
    assert(wall >= 0 && wall <= 3);

    if (wall == 0)
    {
        if (getCELLrow(c) == 0 || getCELLwall(c,0)) {return 0;}
        if (isCELLvisited(m->cells[getCELLrow(c)-1][getCELLcol(c)]))
            return 0;
        else {return 1;}
    }
    else if (wall == 1)
    {
        if (getCELLcol(c) == 0 || getCELLwall(c,1)) {return 0;}
        if (isCELLvisited(m->cells[getCELLrow(c)][getCELLcol(c)-1]))
            return 0;
        else {return 1;}
    }
    else if (wall == 2)
    {
        if (getCELLcol(c) == m->cols - 1 || getCELLwall(c,2)) {return 0;}
        if (isCELLvisited(m->cells[getCELLrow(c)][getCELLcol(c)+1]))
            return 0;
        else {return 1;}
    }
    else // wall == 3
    {
        if (getCELLrow(c) == m->rows - 1 || getCELLwall(c,3)) {return 0;}
        if (isCELLvisited(m->cells[getCELLrow(c)+1][getCELLcol(c)]))
            return 0;
        else {return 1;}
    }
}

void freeMAZE(MAZE *m)
{
    for (int i = 0; i < m->rows; ++i)
    {
        for (int j = 0; j < m->cols; ++j)
            freeCELL(m->cells[i][j]);
        free(m->cells[i]);
    }

    free(m->cells);
    free(m);
}
