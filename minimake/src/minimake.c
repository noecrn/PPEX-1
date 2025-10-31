#define _POSIX_C_SOURCE 200809L

#include "parser.h"
#include "dlist/dlist.h"
#include "destroy.h"
#include "executor.h"

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

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
        printf("(%s): ", rule->target);

        // --- PRINT DEPENDENCIES ---
        if (rule->dependencies)
        {
            struct dlist_item *cur_dep = rule->dependencies->head;
            while (cur_dep != NULL)
            {
                char *temp = cur_dep->data;
                printf("[%s] ", temp);
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

static char *set_path(void)
{
    // --- CHECK IF makefile EXIST ---
    if (access("makefile", F_OK) == 0)
        return "makefile";
    // --- CHECK IF Makefile EXIST ---
    else if (access("Makefile", F_OK) == 0)
        return "Makefile";
    // --- NO MAKEFILE EXIST ---
    else
        errx(2, "*** No targets specified and no makefile found. Stop.");
}

int main(int argc, char *argv[])
{
    char *path = NULL;
    bool flag_h = false;
    bool flag_p = false;

    // --- CHECK COMMAND LINE OPTIONS ---
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "") == 0)
            errx(2, "*** empty string invalid as argument. Stop.");

        if (strcmp(argv[i], "-f") == 0)
        {
            if (i >= argc-1)
            {
                errx(2, "Invalid argument, you must provide a path after -f. Stop.");
            }

            // --- UPDATE FILEPATH ---
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

    // --- USE DEFAULT PATH ---
    if (!path)
        path = set_path();

    // --- LOAD PARSED MAKEFILE ---
    struct minimake *data = read_file(path);
    if (!data)
        return 2;

    data->processed_targets = dlist_init();

    // --- PRINT HELP MESSAGE ---
    if (flag_h)
    {
        printf("[HELP MESSAGE]\n");
        destroy_minimake(data);
        return 0;
    }

    // --- PRETTY PRINT ---
    if (flag_p)
        print_output(data);
        
    // --- EXECUTE ALL MAKEFILE
    else
    {
        if (executor(argc, argv, data) == 2)
        {
            destroy_minimake(data);
            return 2;
        }
    }

    // --- FREE ALL STRUCT ---
    destroy_minimake(data);
    return 0;
}
