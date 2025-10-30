#include "parser.h"
#include "dlist/dlist.h"
#include "minimake.h"
#include "expansion.h"

#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
                        //free(expanded_value);
                    }

                    // --- JUMP $(VAR) LENGTH ---
                    //free(var_name);
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

    res[res_i] = '\0';
    return strdup(res);
}

static char *get_var_name_recipe(const char *start, size_t *len)
{
    *len = 0;
    const char *end = NULL;
    char open_char = start[1];
    char close_char = (open_char == '(') ? ')' : '}';

    // --- FIND THE CLOSING BRACKET ---
    end = strchr(start + 2, close_char);
    if (!end)
        return NULL;

    // --- MALLOC AND COPY THE NAME ---
    size_t name_len = end - (start + 2);
    *len = name_len + 3;
    char *name = malloc(name_len + 1);
    if (!name)
        return NULL;

    strncpy(name, start + 2, name_len);
    name[name_len] = '\0';

    return name;
}

char *expand_recipe(char *str, struct rule *cur_rule, struct minimake *data)
{
    if (!str)
        return strdup("");

    // --- CHECK IF THERE IS VARIABLE ---
    if (!strchr(str, '$'))
        return strdup(str);

    char res[4096];
    res[0] = '\0';
    int res_i = 0;
    int i = 0;

    while (str[i] != '\0')
    {
        // --- IF NO VARIABLE, JUST COPY CHAR ---
        if (str[i] != '$')
        {
            res[res_i] = str[i];
            res_i++;
            i++;
        }
        // --- HANDLE $$ ---
        else if (str[i+1] == '$')
        {
            res[res_i] = '$';
            res_i++;
            i += 2;
        }
        else
        {
            // --- IF $@ ADD TARGET ---
            if (str[i+1] == '@')
            {
                strcpy(res + res_i, cur_rule->target);
                res_i += strlen(cur_rule->target);
                i += 2;
            }
            // --- IF $< ADD THE FIRST DEPENDENCIE ---
            else if (str[i+1] == '<')
            {
                if (cur_rule->dependencies->head != NULL)
                {
                    char *first_dep = cur_rule->dependencies->head->data;
                    strcpy(res + res_i, first_dep);
                    res_i += strlen(first_dep);
                }
                i += 2;
            }
            // --- IF $^ RETURN ALL DEPENDENCIES ---
            else if (str[i+1] == '^')
            {
                struct dlist_item *cur = cur_rule->dependencies->head;
                while (cur != NULL)
                {
                    // Add space between dependencies
                    if (cur != cur_rule->dependencies->head)
                    {
                        res[res_i] = ' ';
                        res_i++;
                    }
                    char *dep_data = cur->data;
                    strcpy(res + res_i, dep_data);
                    res_i += strlen(dep_data);
                    cur = cur->next;
                }
                i += 2;
            }
            // --- HANDLE $(VAR) OR ${VAR} ---
            else if (str[i + 1] == '(' || str[i + 1] == '{')
            {
                size_t len = 0;
                char *var_name = get_var_name_recipe(&str[i], &len);

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
                        //free(expanded_value);
                    }

                    // --- JUMP $(VAR) LENGTH ---
                    //free(var_name);
                    i += len;
                }
                // --- INVALID CASE NO CLOSING BRACKET ---
                else
                {
                    res[res_i] = '$';
                    res_i++;
                    i++;
                }
            }
            else if (isalnum(str[i + 1]))
            {
                char var_name[2] = { str[i + 1], '\0' };
                char *value = find_in_var(var_name, data);
                char *expanded_value = expand_immediate(value, data);

                if (expanded_value)
                {
                    strcpy(res + res_i, expanded_value);
                    res_i += strlen(expanded_value);
                    //free(expanded_value);
                }
                i += 2;
            }
            else
            {
                res[res_i] = '$';
                res_i++;
                i++;
            }
        }
    }

    res[res_i] = '\0';
    return strdup(res);
}

char *expand(char *str, struct minimake *data)
{
    if (!data || strchr(str, '$') == NULL)
        return str;

    char *res = expand_immediate(str, data);

    return res;
}
