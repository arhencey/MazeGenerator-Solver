/***************************************************************************
* This unbounded queue class is built upon the circular dynamic array      *
* class.                                                                   *
*                                                                          *
* Author: Alan Hencey                                                      *
***************************************************************************/

#include "queue.h"
#include "cda.h"
#include <stdlib.h>
#include <assert.h>

struct queue
{
    CDA *array;
    void (*displayFunc)(void *,FILE *);
    /* Flag toggles display function printing number of empty slots. */
    int displayFlag;
};

QUEUE *newQUEUE(void)
{
    QUEUE *p = malloc(sizeof(QUEUE));
    assert(p != 0);

    p->array = newCDA();
    p->displayFunc = 0;
    p->displayFlag = 0;

    return p;
}

void setQUEUEdisplay(QUEUE *items,void (*df)(void *,FILE *))
{
    items->displayFunc = df;
    setCDAdisplay(items->array,df);
}

void setQUEUEfree(QUEUE *items,void (*ff)(void *))
{
    setCDAfree(items->array,ff);
}

void enqueue(QUEUE *items,void *value)
{
    insertCDAback(items->array,value);
}

void *dequeue(QUEUE *items)
{
    assert(sizeCDA(items->array) > 0);
    return removeCDAfront(items->array);
}

void *peekQUEUE(QUEUE *items)
{
    assert(sizeCDA(items->array) > 0);
    return getCDA(items->array,0);
}

void displayQUEUE(QUEUE *items,FILE *fp)
{
    /* If displayFlag set to 1, use displayCDA. */
    if (items->displayFlag == 1)
    {
        debugCDA(items->array,0);
        displayCDA(items->array,fp);
    }

    /* If display flag is set to 2, use displayCDA with CDA debug flag set. */
    else if (items->displayFlag == 2)
    {
        debugCDA(items->array,1);
        displayCDA(items->array,fp);
    }

    /* If displayFlag is anything else, do standard queue display. */
    else
    {
        fprintf(fp,"<");
        for (int i = 0; i < sizeCDA(items->array); ++i)
        {
            items->displayFunc(getCDA(items->array,i),fp);
            if (i < sizeCDA(items->array) - 1) {fprintf(fp,",");}
        }
        fprintf(fp,">");
    }
}

int debugQUEUE(QUEUE *items,int level)
{
    int oldFlag = items->displayFlag;
    items->displayFlag = level;
    return oldFlag;
}

void freeQUEUE(QUEUE *items)
{
    freeCDA(items->array);
    free(items);
}

int sizeQUEUE(QUEUE *items) {return sizeCDA(items->array);}
