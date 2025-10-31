#ifndef DLIST_H
#define DLIST_H

#include <stddef.h>

struct dlist_item
{
    void *data;
    struct dlist_item *next;
    struct dlist_item *prev;
};

struct dlist
{
    size_t size;
    struct dlist_item *head;
    struct dlist_item *tail;
};

struct dlist *dlist_init(void);
int dlist_push_back(struct dlist *list, void *element);
void dlist_clear(struct dlist *list);

#endif /* ! DLIST_H */
