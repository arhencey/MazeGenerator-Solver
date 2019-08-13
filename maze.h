#ifndef __MAZE_INCLUDED__
#define __MAZE_INCLUDED__

#include <stdio.h>
#include "cell.h"

typedef struct maze MAZE;

extern void createMAZE(int rows,int cols,int randSeed,FILE *fp);
extern void solveMAZE(FILE *readFile,FILE *writeFILE);
extern void drawMAZE(FILE *fp);

#endif
