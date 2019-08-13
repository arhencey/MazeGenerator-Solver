#ifndef __CELL_INCLUDED__
#define __CELL_INCLUDED__

typedef struct cell CELL;

extern CELL *newCELL(int r,int c);
extern int  getCELLwall(CELL *c,int wall);
extern void removeCELLwall(CELL *c,int wall);
extern void clearWALL(CELL *curCell,CELL *nextCell);
extern int  getCELLval(CELL *c);
extern int  setCELLval(CELL *c,int value);
extern int  getCELLrow(CELL *c);
extern int  getCELLcol(CELL *c);
extern int  isCELLvisited(CELL *c);
extern void setCELLvisited(CELL *c);
extern void freeCELL(CELL *c);

#endif
