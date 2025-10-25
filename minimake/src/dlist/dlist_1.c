#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "dlist.h"

struct dlist *dlist_init(void)
{
    struct dlist *new_list = malloc(sizeof(struct dlist));
    if (!new_list)
        return NULL;

    new_list->size = 0;
    new_list->head = NULL;
    new_list->tail = NULL;
    return new_list;
}

int dlist_push_front(struct dlist *list, void *element)
{
    struct dlist_item *new_item = malloc(sizeof(struct dlist_item));
    if (!new_item)
        return 0;

    new_item->data = element;
    new_item->next = list->head;
    new_item->prev = NULL;
    if (list->size != 0)
        list->head->prev = new_item;
    list->head = new_item;
    if (list->size == 0)
        list->tail = new_item;
    list->size++;

    return 1;
}

void dlist_print(const struct dlist *list)
{
    if (!list || list->size == 0)
        return;

    struct dlist_item *cur = list->head;
    while (cur != NULL)
    {
        char *temp = cur->data;
        printf("%s\n", temp);
        cur = cur->next;
    }
}

int dlist_push_back(struct dlist *list, void *element)
{
    struct dlist_item *new_item = malloc(sizeof(struct dlist_item));
    if (!new_item)
        return 0;

    new_item->data = element;
    new_item->prev = list->tail;
    new_item->next = NULL;

    if (list->size != 0)
        list->tail->next = new_item;
    list->tail = new_item;
    if (list->size == 0)
        list->head = new_item;
    list->size++;

    return 1;
}

size_t dlist_size(const struct dlist *list)
{
    return list->size;
}
