#define _POSIX_C_SOURCE 200809L

#include "minimake.h"
#include "dlist/dlist.h"
#include "expansion.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void trim_str_recipe(char line[])
{
    // --- GET LENGTH ---
    size_t len = strlen(line);
    if (len == 0)
        return;

    // --- REMOVE SPACES AT THE BEGINNING ---
    char *start = line;
    while (isspace(*start))
        start++;

    // --- REALLOC THE RIGHT SIZE ---
    if (start != line)
    {
        size_t new_size = strlen(start) + 1;
        memmove(line, start, new_size);
    }
}

static void trim_str(char *line)
{
    // --- GET LENGTH ---
    size_t len = strlen(line);
    if (len == 0)
        return;

    // --- REMOVE SPACES AT THE END ---
    char *end = line + len - 1;
    while (end >= line && isspace(*end))
        end--;
    *(end + 1) = '\0';

    // --- REMOVE SPACES AT THE BEGINNING ---
    char *start = line;
    while (isspace(*start))
        start++;

    // --- REALLOC THE RIGHT SIZE ---
    if (start != line)
    {
        size_t new_size = strlen(start) + 1;
        memmove(line, start, new_size);
    }
}

static void process_rule(char *line, struct rule *data, struct minimake *minimake)
{
    // --- END THE LINE CORRECTLY ---
    line[strcspn(line, "\n")] = '\0';

    // --- SPLIT THE LINE ---
    char *dep = strchr(line, ':');
    *dep = '\0';
    dep++;

    // --- REMOVE SPACE ---
    trim_str(line);
    data->target = strdup(line);
    if (!data->target)
    {
        fprintf(stderr, "Target is missing");
        destroy_struct(data);
        exit(2);
    }

    // --- REMOVE SPACE & COMMENT ---
    char *temp = strchr(dep, '#');
    if (temp)
        *temp = '\0';
    trim_str(dep);

    // --- SPLIT DEPENDENCIES ---
    char *delim = " \t\n";
    for (char *token = strtok(dep, delim); token; token = strtok(NULL, delim))
    {
        // --- EXPAND VARIABLES ---
        char *exp = expand(token, minimake, data);
        if (!exp)
            exp = strdup("");

        for (char *sub = strtok(exp, delim); sub; sub = strtok(NULL, delim))
        {
            if (*sub == '\0')
                continue;

            char *temp = strdup(sub);
            if (!temp)
            {
                free(exp);
                fprintf(stderr, "Strdup failed");
                exit(2);
            }
            dlist_push_back(data->dependencies, temp);
        }
    }
}

static void process_variable(char *line, struct variable *data)
{
    // --- END THE LINE CORRECTLY ---
    line[strcspn(line, "\n")] = '\0';

    // --- SPLIT THE LINE ---
    char *var = strchr(line, '=');
    *var = '\0';
    var++;

    // --- REMOVE SPACE ---
    trim_str(line);
    data->name = strdup(line);
    if (!data->name)
    {
        fprintf(stderr, "Name is missing");
        //destroy_struct(data);
        exit(2);
    }

    // --- REMOVE SPACE & COMMENT ---
    char *temp = strchr(var, '#');
    if (temp)
        *temp = '\0';
    trim_str(var);
    data->value = strdup(var);
    if (!data->value)
    {
        fprintf(stderr, "Value is missing");
        //destroy_struct(data);
        exit(2);
    }
}

static void recipe(struct rule *last_rule, char *line)
{
    if (!last_rule)
    {
        fprintf(stderr, "Recipe outside of rule");
        exit(2);
    }

    // --- END THE LINE CORRECTLY ---
    line[strcspn(line, "\n")] = '\0';

    // --- REMOVE SPACE AT THE BEGINNING ---
    trim_str_recipe(line);

    // --- ADD TO MINIMAKE ---
    dlist_push_back(last_rule->recipe, strdup(line));
}

static struct rule *rule(char *line, struct minimake *data)
{
    // --- INIT NEW RULE ---
    struct rule *new_rule = malloc(sizeof(struct rule));
    if (!new_rule)
    {
        fprintf(stderr, "Malloc error");
        exit(2);
    }
    new_rule->dependencies = dlist_init();
    new_rule->recipe = dlist_init();

    // --- PROCESS NEW RULE & ADD IT TO MINIMAKE ---
    process_rule(line, new_rule, data);
    dlist_push_back(data->rule, new_rule);

    return new_rule;
}

static struct rule *variable(char *line, struct minimake *data)
{
    // --- INIT NEW RULE ---
    struct variable *new_var = malloc(sizeof(struct variable));
    if (!new_var)
    {
        fprintf(stderr, "Malloc error");
        exit(2);
    }

    // --- PROCESS NEW RULE & ADD IT TO MINIMAKE ---
    process_variable(line, new_var);
    dlist_push_back(data->variable, new_var);

    return NULL;
}

struct minimake *read_file(char *argv)
{
    // --- OPEN THE FILE ---
    FILE *file = fopen(argv, "r");
    if (file == NULL)
    {
        fprintf(stderr, "No Makefile was found");
        exit(2);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    struct minimake *data = malloc(sizeof(struct minimake));
    if (!data)
    {
        fprintf(stderr, "Malloc error");
        free(line);
        exit(2);
    }

    data->variable = dlist_init();
    data->rule = dlist_init();
    struct rule *last_rule = NULL;

    // --- READ FILE LINE BY LINE ---
    while ((read = getline(&line, &len, file)) != -1)
    {
        if (strlen(line) == 0 || strspn(line, " \t") == strlen(line) || line[0] == '#' || line[0] == '\n')
        {
            continue;
        }
        // --- CHECK IF RECIPE ---
        else if (line[0] == '\t')
        {
            recipe(last_rule, line);
        }
        // --- CHECK IF RULE ---
        else if (strchr(line, ':'))
        {
            last_rule = rule(line, data);
        }
        // --- CHECK IF VARIABLE ---
        else if (strchr(line, '='))
        {
            last_rule = variable(line, data);
        }
    }

    // --- CLOSE THE FILE ---
    free(line);
    fclose(file);
    return data;
}
