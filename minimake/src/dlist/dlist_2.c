#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "dlist.h"

char *dlist_get(const struct dlist *list, size_t index)
{
    if (!list || index > list->size || list->size == 0)
        return NULL;

    size_t idx = 0;
    struct dlist_item *cur = list->head;
    while (cur != NULL)
    {
        if (idx == index)
            return cur->data;
        cur = cur->next;
        idx++;
    }

    return NULL;
}

int dlist_insert_at(struct dlist *list, char *element, size_t index)
{
    if (index > list->size)
        return 0;
    if (index == list->size)
        return dlist_push_back(list, element);
    if (index == 0)
        return dlist_push_front(list, element);

    struct dlist_item *new_item = malloc(sizeof(struct dlist_item));
    if (!new_item)
        return 0;

    new_item->data = element;
    size_t idx = 0;
    struct dlist_item *cur = list->head;
    while (cur != NULL)
    {
        if (idx == index)
        {
            struct dlist_item *temp = cur->prev;
            cur->prev = new_item;
            if (temp)
                temp->next = new_item;
            new_item->next = cur;
            new_item->prev = temp;
            list->size++;
            return 1;
        }
        cur = cur->next;
        idx++;
    }

    return 0;
}

int dlist_find(const struct dlist *list, char *element)
{
    if (list->size == 0)
        return -1;

    size_t idx = 0;
    struct dlist_item *cur = list->head;
    while (cur != NULL)
    {
        if (cur->data == element)
            return idx;
        cur = cur->next;
        idx++;
    }

    return -1;
}

static char *aux(struct dlist *list)
{
    char *deleted_data = list->tail->data;
    struct dlist_item *deleted_node = list->tail;
    struct dlist_item *temp = list->tail;
    list->tail = temp->prev;
    if (list->tail)
        list->tail->next = NULL;
    list->size--;
    free(deleted_node);
    if (list->size == 0)
    {
        // free(list->head);
        list->tail = NULL;
        list->head = NULL;
    }
    return deleted_data;
}

static char *aux_2(struct dlist *list)
{
    char *deleted_data = list->head->data;
    struct dlist_item *deleted_node = list->head;
    struct dlist_item *temp = list->head;
    list->head = temp->next;
    if (list->head)
        list->head->prev = NULL;
    list->size--;
    free(deleted_node);
    if (list->size == 0)
    {
        list->tail = NULL;
        list->head = NULL;
    }
    return deleted_data;
}

char *dlist_remove_at(struct dlist *list, size_t index)
{
    if (index >= list->size || list->size == 0)
        return NULL;

    char *deleted_data = NULL;
    if (index == list->size - 1)
    {
        return aux(list);
    }
    else if (index == 0)
    {
        return aux_2(list);
    }

    size_t idx = 0;
    struct dlist_item *cur = list->head;
    while (cur != NULL)
    {
        if (idx == index)
        {
            deleted_data = cur->data;
            struct dlist_item *deleted_node = cur;
            struct dlist_item *temp_prev = cur->prev;
            struct dlist_item *temp_next = cur->next;
            temp_prev->next = temp_next;
            temp_next->prev = temp_prev;
            list->size--;
            free(deleted_node);
            if (list->size == 0)
            {
                list->tail = NULL;
                list->head = NULL;
            }
            return deleted_data;
        }
        cur = cur->next;
        idx++;
    }
    return deleted_data;
}

void dlist_clear(struct dlist *list)
{
    if (!list || list->size == 0)
        return;

    struct dlist_item *cur = list->head;
    while (cur != NULL)
    {
        cur = list->head->next;
        free(list->head);
        list->head = cur;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}
