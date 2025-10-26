#include "minimake.h"
#include "dlist/dlist.h"

#include <stdlib.h>

static void destroy_dependencies(struct dlist *data)
{
    if (!data || data->size == 0)
        return;

    struct dlist_item *cur = data->head;
    while (cur != NULL)
    {
        struct dlist_item *next = cur->next;
        free(cur->data);
        free(cur);
        cur = next;
    }
}

static void destroy_rules(struct dlist *data)
{
    if (!data || data->size == 0)
        return;

    struct dlist_item *cur = data->head;
    while (cur != NULL)
    {
        struct dlist_item *next = cur->next;
        struct rule *temp = cur->data;
        if (temp)
        {
            free(temp->target);
            destroy_dependencies(temp->dependencies);
            destroy_dependencies(temp->recipe);
            free(temp->dependencies);
            free(temp->recipe);
            free(temp);
        }
        free(cur);
        cur = next;
    }
}

static void destroy_variables(struct dlist *data)
{
    if (!data || data->size == 0)
        return;

    struct dlist_item *cur = data->head;
    while (cur != NULL)
    {
        struct dlist_item *next = cur->next;
        struct variable *temp = cur->data;
        if (temp)
        {
            free(temp->name);
            free(temp->value);
            free(temp);
        }
        free(cur);
        cur = next;
    }
}

void destroy_minimake(struct minimake *data)
{
    if (!data)
        return;

    destroy_variables(data->variable);
    destroy_rules(data->rule);

    free(data->variable);
    free(data->rule);

    data->variable = NULL;
    data->rule = NULL;

    free(data);
}
