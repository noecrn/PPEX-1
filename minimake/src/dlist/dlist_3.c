#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "dlist.h"

static struct dlist_item *aux(struct dlist_item *list)
{
    if (!list)
        return NULL;

    struct dlist_item *temp = list->prev;
    list->prev = list->next;
    list->next = temp;

    if (!list->prev)
        return list;

    return aux(list->prev);
}

void dlist_reverse(struct dlist *list)
{
    if (!list || list->size == 0)
        return;

    aux(list->head);
    struct dlist_item *temp = list->head;
    list->head = list->tail;
    list->tail = temp;
}

static struct dlist *index_null(struct dlist *list)
{
    struct dlist *temp = dlist_init();

    temp->head = list->head;
    temp->tail = list->tail;
    temp->size = list->size;
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return temp;
}

struct dlist *aux_2(struct dlist *list, struct dlist_item *cur, int index)
{
    struct dlist *second_part = dlist_init();

    struct dlist_item *temp = cur->next;
    if (temp)
        temp->prev = NULL;
    second_part->head = cur->next;
    second_part->head->prev = NULL;
    struct dlist_item *second_cur = second_part->head;
    size_t count = 1;
    while (second_cur->next != NULL)
    {
        second_cur = second_cur->next;
        count++;
    }
    second_part->tail = list->tail;
    second_part->tail->next = NULL;
    second_part->size = count;
    list->tail = cur;
    // list->tail->prev = NULL;
    list->size = index;
    cur->next = NULL;

    return second_part;
}

struct dlist *dlist_split_at(struct dlist *list, size_t index)
{
    if (!list || index > list->size)
        return NULL;

    if (index == list->size)
    {
        struct dlist *second_part = dlist_init();
        return second_part;
    }
    if (index == 0)
    {
        return index_null(list);
    }

    size_t idx = 1;
    struct dlist_item *cur = list->head;
    while (cur != NULL)
    {
        if (idx == index)
        {
            return aux_2(list, cur, index);
        }
        cur = cur->next;
        idx++;
    }
    return NULL;
}

void dlist_concat(struct dlist *list1, struct dlist *list2)
{
    if (!list1 || !list2)
        return;

    if (list1->size == 0 && list2->size == 0)
    {
        list1->head = NULL;
        list1->tail = NULL;
        list1->size = 0;
    }
    if (list1->size == 0)
    {
        list1->head = list2->head;
        list1->tail = list2->tail;
        list1->size = list2->size;
    }
    if (list2->size != 0)
    {
        struct dlist_item *cur = list1->tail;
        cur->next = list2->head;
        if (list2->size > 0)
            list2->head->prev = cur;
        list1->size += list2->size;
        list1->tail = list2->tail;
    }

    list2->head = NULL;
    list2->tail = NULL;
    list2->size = 0;
}
