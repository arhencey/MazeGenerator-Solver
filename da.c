/***************************************************************************
* This dynamic array class is a fillable array that can grow in capacity   *
* should the array become completely filled, thus making room for more     *
* additions. Conversely, it can shrink in capacity if it becomes mostly    *
* empty.                                                                   *
*                                                                          *
* Author: Alan Hencey                                                      *
***************************************************************************/

#include "da.h"
#include <stdlib.h>
#include <assert.h>

struct da
{
    int capacity;
    int size;
    void **array;
    void (*displayFunc)(void *,FILE *);
    void (*freeFunc)(void *);
    /* Flag toggles display function printing number of empty slots. */
    int displayFlag;
};

static void doubleCap(DA *items);
static void halveCap(DA *items);

DA *newDA(void)
{
    DA *p = (DA *)malloc(sizeof(DA));
    assert(p != 0);

    p->capacity = 1;
    p->size = 0;
    p->displayFlag = 0;
    p->displayFunc = 0;
    p->freeFunc = 0;
    p->array = malloc(sizeof(void *));

    return p;
}

void setDAdisplay(DA *items,void (*df)(void *,FILE *))
{
    items->displayFunc = df;
}

void setDAfree(DA *items,void (*ff)(void *))
{
    items->freeFunc = ff;
}

void insertDA(DA *items,int index,void *value)
{
    assert(index <= sizeDA(items));

    /* Resize array if necissary. */
    if (sizeDA(items) == items->capacity) {doubleCap(items);}
    items->size++;

    /* Shift array and insert new value. */
    for (int i = sizeDA(items)-1; i > index; --i)
        setDA(items,i,getDA(items,i-1));
    setDA(items,index,value);
}

void *removeDA(DA *items,int index)
{
    assert(sizeDA(items) > 0 && index < sizeDA(items));

    /* Store item to be removed and shift array. */
    void *removedItem = getDA(items,index);
    for (int i = index; i < sizeDA(items)-1; ++i)
        setDA(items,i,getDA(items,i+1));
    items->size--;

    /* Resize array if necissary. */
    if (sizeDA(items) < items->capacity / 4 || sizeDA(items) == 0)
        halveCap(items);
    return removedItem;
}

/* Appends donor to recipient. After union donor will be empty. */
void unionDA(DA *recipient,DA *donor)
{
    /* This implementation runs in O(N^2) due to shifting donor array after
       each front removal.
    while (sizeDA(donor) != 0)
        setDA(recipient,sizeDA(recipient),removeDA(donor,0));
    */

    /* This implementation runs in O(N) because donor array values are
       removed from the back. */
    for (int i = 0; i < sizeDA(donor); ++i)
        setDA(recipient,sizeDA(recipient),getDA(donor,i));

    while (sizeDA(donor) > 0)
        removeDAback(donor);
}

void *getDA(DA *items,int index)
{
    assert(index >= 0 && index < sizeDA(items));
    return items->array[index];
}

void *setDA(DA *items,int index,void *value)
{
    assert(index >= 0 && index <= sizeDA(items));
    void *replaced = 0;
    if (index == sizeDA(items)) {insertDA(items,index,value);}
    else
    {
        replaced = getDA(items,index);
        items->array[index] = value;
    }
    /* Returns null pointer if no value was replaced. */
    return replaced;
}

int sizeDA(DA *items) {return items->size;}

void displayDA(DA *items,FILE *fp)
{
    fprintf(fp,"[");

    /* Default display method. */
    if (items->displayFunc == 0)
    {
        for (int i = 0; i < sizeDA(items); ++i)
        {
            fprintf(fp,"@%p",getDA(items,i));
            if (i < sizeDA(items)-1) {fprintf(fp,",");}
        }
    }

    /* User passed display method. */
    else
    {
        for (int i = 0; i < sizeDA(items); ++i)
        {
            items->displayFunc(getDA(items,i),fp);
            if (i < sizeDA(items)-1) {fprintf(fp,",");}
        }
    }

    /* If displayFlag is set, print debug values. */
    if (items->displayFlag > 0)
    {
        int empty = items->capacity - sizeDA(items);
        if (sizeDA(items) > 0) {fprintf(fp,",");}
        fprintf(fp,"[%d]]",empty);
    }
    else fprintf(fp,"]");
}

int debugDA(DA *items,int level)
{
    int prev = items->displayFlag;
    items->displayFlag = level;
    return prev;
}

void freeDA(DA *items)
{
    if (items->freeFunc != 0)
    {
        for (int i = 0; i < sizeDA(items); ++i)
            items->freeFunc(getDA(items,i));
    }
    free(items->array);
    free(items);
}

static void doubleCap(DA *items)
{
    items->capacity *= 2;
    items->array = realloc(items->array,(sizeof(void *) * items->capacity));
    assert(items != 0);
}

static void halveCap(DA *items)
{
    /* If array is empty, capacity = 1 */
    if (sizeDA(items) == 0) {items->capacity = 1;}
    else {items->capacity /= 2;}

    items->array = realloc(items->array,(sizeof(void *) * items->capacity));
    assert(items != 0);
}
