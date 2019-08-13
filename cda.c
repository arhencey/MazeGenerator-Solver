/***************************************************************************
* This circular dynamic array class functions the same as the dynamic      *
* array class except values may be added and removed from the front of     *
* the array in constant time.                                              *
*                                                                          *
* Author: Alan Hencey                                                      *
***************************************************************************/

#include "cda.h"
#include <stdlib.h>
#include <assert.h>

struct cda
{
    int capacity;
    int size;
    int startIndex;
    void **array;
    void (*displayFunc)(void *,FILE *);
    void (*freeFunc)(void *);
    /* Flag toggles display function printing number of empty slots. */
    int displayFlag;
};

static void doubleCap(CDA *items);
static void halveCap(CDA *items);
static int  incrementIndex(CDA *items,int index);
static int  decrementIndex(CDA *items,int index);

CDA *newCDA(void)
{
    CDA *p = (CDA *)malloc(sizeof(CDA));
    assert(p != 0);

    p->capacity = 1;
    p->size = 0;
    p->startIndex = 0;
    p->displayFlag = 0;
    p->displayFunc = 0;
    p->freeFunc = 0;
    p->array = malloc(sizeof(void *));

    return p;
}

void setCDAdisplay(CDA *items, void (*df)(void *,FILE *))
{
    items->displayFunc = df;
}

void setCDAfree(CDA *items, void (*ff)(void *))
{
    items->freeFunc = ff;
}

void insertCDA(CDA *items,int index,void *value)
{
    assert(index >= 0 && index <= sizeCDA(items));

    /* Resize array if necissary. */
    if (sizeCDA(items) == items->capacity) {doubleCap(items);}
    items->size++;

    /* If index is in first half of array, shift left. */
    if (index <= sizeCDA(items) / 2)
    {
        items->startIndex = decrementIndex(items,items->startIndex);
        for (int i = 1; i <= index; ++i)
            setCDA(items,i-1,getCDA(items,i));
        setCDA(items,index,value);
    }

    /* If index is in second half of array, shift right. */
    else
    {
        for (int i = sizeCDA(items)-1; i > index; --i)
            setCDA(items,i,getCDA(items,i-1));
        setCDA(items,index,value);
    }
}

void *removeCDA(CDA *items,int index)
{
    assert(sizeCDA(items) > 0);
    assert(index >= 0 && index < sizeCDA(items));
    void *removedItem = getCDA(items,index);

    /* If index is in first half of array, shift right. */
    if (index <= sizeCDA(items) / 2)
    {
        for (int i = index; i > 0; --i)
            setCDA(items,i,getCDA(items,i-1));
        items->startIndex = incrementIndex(items,items->startIndex);
    }

    /* If index is in second half of array, shift left. */
    else
    {
        for (int i = index; i < sizeCDA(items)-1; ++i)
            setCDA(items,i,getCDA(items,i+1));
    }

    /* Resize array if necissary. */
    items->size--;
    if (sizeCDA(items) < items->capacity / 4 || sizeCDA(items) == 0)
        halveCap(items);
    return removedItem;
}

/* Appends donor to recipient. After union donor will be empty. */
void unionCDA(CDA *recipient,CDA *donor)
{
    while (sizeCDA(donor) > 0)
        insertCDAback(recipient,removeCDAfront(donor));
}

void *getCDA(CDA *items,int index)
{
    assert(index >= 0 && index < sizeCDA(items));
    int fixedIndex = items->startIndex + index;
    if (fixedIndex >= items->capacity) {fixedIndex -= items->capacity;}
    return items->array[fixedIndex];
}

void *setCDA(CDA *items,int index,void *value)
{
    assert(index >= -1 && index <= sizeCDA(items));
    void *replacedValue = 0;
    if (index > -1 && index < sizeCDA(items))
        replacedValue = getCDA(items,index);

    /* If the given index is the front or back of the array, insert value. */
    if (index == sizeCDA(items)) {insertCDAback(items,value);}
    else if (index == -1) {insertCDAfront(items,value);}

    else
    {
        int fixedIndex = items->startIndex + index;
        if (fixedIndex >= items->capacity) {fixedIndex -= items->capacity;}
        items->array[fixedIndex] = value;
    }
    return replacedValue;
}

int sizeCDA(CDA *items) {return items->size;}

void displayCDA(CDA *items,FILE *fp)
{
    fprintf(fp,"(");

    /* Default display method. */
    if (items->displayFunc == 0)
    {
        for (int i = 0; i < sizeCDA(items); ++i)
        {
            fprintf(fp,"@%p",getCDA(items,i));
            if (i < sizeCDA(items)-1) {fprintf(fp,",");}
        }
    }

    /* User passed display function. */
    else
    {
        for (int i = 0; i < sizeCDA(items); ++i)
        {
            items->displayFunc(getCDA(items,i),fp);
            if (i < sizeCDA(items)-1) {fprintf(fp,",");}
        }
    }

    /* If displayFlag is set, print the remaining capacity at end. */
    if (items->displayFlag > 0)
    {
        int empty = items->capacity - sizeCDA(items);
        if (sizeCDA(items) > 0) {fprintf(fp,",");}
        fprintf(fp,"(%d))",empty);
    }
    else fprintf(fp,")");
}

int debugCDA(CDA *items,int level)
{
    int prevFlag = items->displayFlag;
    items->displayFlag = level;
    return prevFlag;
}

void freeCDA(CDA *items)
{
    if (items->freeFunc != 0)
    {
        for (int i = 0; i < sizeCDA(items); ++i)
            items->freeFunc(getCDA(items,i));
    }
    free(items->array);
    free(items);
}

static void doubleCap(CDA *items)
{
    void **newArray = malloc(sizeof(void *) * items->capacity * 2);
    for (int i = 0; i < sizeCDA(items); ++i)
        newArray[i] = getCDA(items,i);

    free(items->array);
    items->array = newArray;
    items->capacity *= 2;
    items->startIndex = 0;
}

static void halveCap(CDA *items)
{
    /* If array is empty, capacity = 1 */
    int newCap = 0;
    if (sizeCDA(items) == 0) {newCap = 1;}
    else {newCap = items->capacity / 2;}

    void **newArray = malloc(sizeof(void *) * newCap);
    for (int i = 0; i < sizeCDA(items); ++i)
        newArray[i] = getCDA(items,i);

    free(items->array);
    items->array = newArray;
    items->capacity = newCap;
    items->startIndex = 0;
}

/* If incrementing will put index past the end of the array, loop index back
   to 0. */
static int incrementIndex(CDA *items,int index)
{
    index++;
    if (index > items->capacity) {index -= items->capacity;}
    return index;
}

/* If decrementing will put index past the begenning of the array, loop
   index back to 0. */
static int decrementIndex(CDA *items,int index)
{
    index--;
    if (index < 0) {index += items->capacity;}
    return index;
}
