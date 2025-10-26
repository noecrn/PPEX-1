#define _POSIX_C_SOURCE 200809L

#include "parser.h"
#include "dlist/dlist.h"
#include "destroy.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void destroy_struct(struct rule *rule)
{
    if (!rule)
        return;

    dlist_clear(rule->dependencies);
    free(rule);
}

void print_output(struct minimake *data)
{
    if (!data || !data->variable || !data->rule)
        return;

    // --- PRINT VARIABLES ---
    printf("# variables\n");
    struct dlist_item *cur_variable = data->variable->head;
    while (cur_variable != NULL)
    {
        struct variable *temp = cur_variable->data;
        printf("'%s' = '%s'\n",temp->name, temp->value);
        cur_variable = cur_variable->next;
    }

    // --- PRINT RULES ---
    printf("# rules\n");
    struct dlist_item *cur_rule = data->rule->head;
    while (cur_rule != NULL)
    {
        // --- PRINT TARGET ---
        struct rule *rule = cur_rule->data;
        printf("(%s):", rule->target);

        // --- PRINT DEPENDENCIES ---
        if (rule->dependencies)
        {
            struct dlist_item *cur_dep = rule->dependencies->head;
            while (cur_dep != NULL)
            {
                char *temp = cur_dep->data;
                printf(" [%s]", temp);
                cur_dep = cur_dep->next;
            }
        }

        printf("\n");

        // --- PRINT RECIPE ---
        if (rule->recipe)
        {
            struct dlist_item *cur_recipe = rule->recipe->head;
            while (cur_recipe != NULL)
            {
                char *temp = cur_recipe->data;
                printf("    '%s'\n", temp);
                cur_recipe = cur_recipe->next;
            }
        }

        cur_rule = cur_rule->next;
    }
}

int main(int argc, char *argv[])
{
    char *path = "../Makefile";
    bool flag_h = false;
    bool flag_p = false;

    // --- CHECK COMMAND LINE OPTIONS ---
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-f") == 0)
        {
            if (i >= argc-1)
            {
                fprintf(stderr, "Invalid argument, you must provide a path after -f");
                exit(2);
            }

            path = argv[i+1];
            i++;
        }
        else if (strcmp(argv[i], "-h") == 0)
        {
            flag_h = true;
        }
        else if (strcmp(argv[i], "-p") == 0)
        {
            flag_p = true;
        }
    }

    if (flag_h)
    {
        printf("[HELP MESSAGE]\n");
        return 0;
    }

    struct minimake *data = read_file(path);
    if (flag_p)
        print_output(data);

    destroy_minimake(data);
    return 0;
}
