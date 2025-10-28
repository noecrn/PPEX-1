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

static char *find_in_var(char buf[], struct minimake *data)
{
    if (!data->variable)
        return "";

    // --- ITERATE TROUGH VARIABLES ---
    struct dlist_item *cur = data->variable->head;
    while (cur != NULL)
    {
        struct variable *temp_var = cur->data;

        // --- LOOK FOR GENERAL VARIABLE ---
        if (strcmp(temp_var->name, buf) == 0)
        {
            return temp_var->value;
        }
        cur = cur->next;
    }

    // --- NOTHING FOUND ---
    return "";
}

static char *get_var_name(const char *start, size_t *len)
{
    *len = 0;
    const char *end = NULL;

    if (start[1] == '(')
    {
        // --- INVALID CASE NO CLOSING BRACKET ---
        end = strchr(start+2, ')');
        if (!end)
            return NULL;

        size_t name_len = end - (start+2);
        *len = name_len + 3;
        char *name = malloc(name_len + 1);
        if (!name)
            return NULL;

        strncpy(name, start + 2, name_len);
        name[name_len] = '\0';

        return name;
    }

    return NULL;
}

char *expand_immediate(char *str, struct minimake *data)
{
    if (!str)
        return strdup("");

    char res[4096];
    int res_i = 0;
    res[0] = '\0';
    int i = 0;

    // --- PARSE THE VARIABLE ---
    while (str[i] != '\0')
    {
        if (str[i] == '$')
        {
            // --- SKIP $$ ---
            if (str[i+1] == '$')
            {
                res[res_i] = '$';
                res_i++;
                i += 2;
            }
            // --- CLASSIQUE CASE: $(VAR) ---
            else if (str[i+1] == '(')
            {
                size_t len = 0;
                char *var_name = get_var_name(&str[i], &len);

                if (var_name)
                {
                    // --- GET VALUE OF VARIABLE ---
                    char *value = find_in_var(var_name, data);

                    // --- RECURSIVE CALL IF VAR CONTAIN VAR ---
                    char *expanded_value = expand_immediate(value, data);

                    // --- COPY EXPANDED VALUE IN RES ---
                    if (expanded_value)
                    {
                        strcpy(res + res_i, expanded_value);
                        res_i += strlen(expanded_value);
                        free(expanded_value);
                    }

                    // --- JUMP $(VAR) LENGTH ---
                    free(var_name);
                    i += len;
                }
                // --- INVALID CASE NO CLOSING BRACKET ---
                else
                {
                    res[res_i] = str[i];
                    res_i++;
                    i++;
                }
            }
            // --- CASE &V, &{} OR $ ---
            else
            {
                res[res_i] = str[i];
                res_i++;
                i++;
            }
        }
        // --- NORMAL CASE ---
        else
        {
            res[res_i] = str[i];
            res_i++;
            i++;
        }
    }

    strcat(res, "\0");
    return strdup(res);
}

char *expand_recipe(char *str, struct rule *cur_rule, struct minimake *data)
{
    char *start = strchr(str, '$');
    if (!start)
        return str;

    // --- IF $@ RETURN TARGET ---
    if (*(start+1) == '@')
    {
        return cur_rule->target;
    }
    // --- IF $< RETURN THE FIRST DEPENDENCIE ---
    else if (*(start+1) == '<')
    {
        if (cur_rule->dependencies->head == NULL)
            return "";

        return cur_rule->dependencies->head->data;
    }
    // --- IF $^ RETURN ALL DEPENDENCIES ---
    else if (*(start+1) == '^')
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

    return expand_immediate(str, data);
}

char *expand(char *str, struct minimake *data)
{
    if (!data || strchr(str, '$') == NULL)
        return str;

    char *res = expand_immediate(str, data);

    return res;
}
