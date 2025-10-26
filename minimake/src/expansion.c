#include "parser.h"
#include "dlist/dlist.h"
#include "minimake.h"

#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int get_dep_len(struct rule *cur_rule)
{
    int total_len = 0;
    int count = 0;

    // --- COUNT EACH DEPENDENCIES LENGTH ---
    struct dlist_item *cur = cur_rule->dependencies->head;
    while (cur != NULL)
    {
        total_len += strlen(cur->data);
        cur = cur->next;
        count++;
    }

    // --- ADD SPACE COUNT ---
    if (count > 0)
        total_len += count - 1;

    return total_len;
}

static char *find_in_var(char buf[], int size, struct minimake *data)
{
    if (!data->variable)
        return "";

    // --- ITERATE TROUGH VARIABLES ---
    struct dlist_item *cur = data->variable->head;
    while (cur != NULL)
    {
        struct variable *temp_var = cur->data;

        // --- LOOK FOR SINGLE VARIABLE ---
        if (size == 2)
        {
        }
        // --- LOOK FOR GENERAL VARIABLE ---
        else if (strcmp(temp_var->name, buf))
        {
            return temp_var->value;
        }
        cur = cur->next;
    }

    // --- NOTHING FOUND ---
    return "";
}

char *expand(char *str, struct minimake *data, struct rule *cur_rule)
{
    if (!data || strchr(str, '$') == NULL)
        return str;

    char *res = "";

    char *start = strchr(str, '$');
    if (*(start+1) == '(')
    {
        // --- PARSE THE VARIABLE ---
        char buf[1024];
        int i = 1;
        while (*(start + i) != ')')
        {
            if (*(start + i) == '(')
              buf[i-1] = *(start + i);
            i++;
        }
        buf[i-1] = '\0';

        // --- FIND THE VARIABLE ---
        res = find_in_var(buf, i, data);
    }
    else
    {
        // --- IF $@ RETURN TARGET ---
        if (*(start+2) == '@')
        {
            return cur_rule->target;
        }
        // --- IF $< RETURN THE FIRST DEPENDENCIE ---
        else if (*(start+2) == '<')
        {
            if (cur_rule->dependencies->head != NULL)
                return "";

            return cur_rule->dependencies->head->data;
        }
        // --- IF $^ RETURN ALL DEPENDENCIES ---
        else if (*(start+2) == '^')
        {
            char *res = malloc(get_dep_len(cur_rule) + 1);
            res[0] = '\0';

            // --- ITERATE THROUGH DEPENDENCIES ---
            struct dlist_item *cur = cur_rule->dependencies->head;
            while (cur != NULL)
            {
                // --- ADD SPACE AT THE BEGINNING ---
                if (res[0] != '\0')
                    strcat(res, " ");

                // --- ADD DEPENDENCIE ---
                strcat(res, cur->data);
                cur = cur->next;
            }

            return res;
        }
    }

    return res;
}
