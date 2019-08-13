/***************************************************************************
* This unbounded stack class is built upon the dynamic array class.        *
*                                                                          *
* Author: Alan Hencey                                                      *
***************************************************************************/

#include "stack.h"
#include "da.h"
#include <stdlib.h>
#include <assert.h>

struct stack
{
    DA *array;
    void (*displayFunc)(void *,FILE *);
    void (*freeFunc)(void *);
    /* Flag toggles display function printing number of empty slots. */
    int displayFlag;
};

STACK *newSTACK(void)
{
    STACK *p = malloc(sizeof(STACK));
    assert(p != 0);

    p->array = newDA();
    p->displayFunc = 0;
    p->displayFlag = 0;

    return p;
}

void setSTACKdisplay(STACK *items,void (*df)(void *,FILE *))
{
    items->displayFunc = df;
    setDAdisplay(items->array,df);
}

void setSTACKfree(STACK *items,void (*ff)(void *))
{
    setDAfree(items->array,ff);
}

void push(STACK *items,void *value)
{
    setDA(items->array,sizeDA(items->array),value);
}

void *pop(STACK *items)
{
    assert(sizeDA(items->array) > 0);
    return removeDA(items->array,sizeDA(items->array) - 1);
}

void *peekSTACK(STACK *items)
{
    assert(sizeDA(items->array) > 0);
    return getDA(items->array,sizeDA(items->array) - 1);
}

void displaySTACK(STACK *items,FILE *fp)
{
    /* If displayFlag set to 1, use displayDA. */
    if (items->displayFlag == 1)
    {
        debugDA(items->array,0);
        displayDA(items->array,fp);
    }

    /* If display flag is set to 2, use displayDA with DA debug flag set. */
    else if (items->displayFlag == 2)
    {
        debugDA(items->array,1);
        displayDA(items->array,fp);
    }

    /* If displayFlag is anything else, do standard stack display. */
    else
    {
        fprintf(fp,"|");
        for (int i = sizeDA(items->array) - 1; i >= 0; --i)
        {
            items->displayFunc(getDA(items->array,i),fp);
            if (i > 0) {fprintf(fp,",");}
        }
        fprintf(fp,"|");
    }
}

int debugSTACK(STACK *items,int level)
{
    int oldFlag = items->displayFlag;
    items->displayFlag = level;
    return oldFlag;
}

void freeSTACK(STACK *items)
{
    freeDA(items->array);
    free(items);
}

int sizeSTACK(STACK *items) {return sizeDA(items->array);}
