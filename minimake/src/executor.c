#include "minimake.h"
#include "expansion.h"
#include "dlist/dlist.h"

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

static char *variable_expansion(char *str, struct rule *cur_rule, struct minimake *data)
{
    // --- CHECK IF THERE IS VARIABLE ---
    if (!strchr(str, '$'))
        return str;

    // --- MALLOC SIZE OF RECIPE AND MORE ---
    char res[4096];
    res[0] = '\0';
    int len = 0;

    // --- GO THROUGH RECIPE ---
    char *delim = " ";
    for (char *token = strtok(str, delim); token; token = strtok(NULL, delim))
    {
        // --- EXPAND VARIABLES ---
        char *exp = expand_recipe(token, cur_rule, data);
        if (!exp)
            exp = strdup("");

        len += strlen(exp) + 1;

        // --- REBUILD THE RECIPE ---
        if (token)
        {
            strcat(res, exp);
            strcat(res, " ");
        }
        //free(exp);
    }

    res[len+1] = '\0';
    return strdup(res);
}

static void exec_recipe(char *str)
{
    // --- INIT CHILD ---
    pid_t pid = fork();
    if (pid < 0)
        errx(2, "Fork fail");
    // --- EXEC COMMAND IN THE CHILD ---
    else if (pid == 0)
    {
        execl("/bin/sh", "supershell", "-c", str, NULL);
        errx(2, "Error during command execution");
    }

    // --- WAIT FOR THE CHILD TO FINISH ---
    int wstatus;
    waitpid(pid, &wstatus, 0);
}

void executor(struct minimake *data)
{
    if (!data)
        return;

    struct dlist *cur = data->rule;
    if (!cur || cur->size == 0)
        return;

    // --- GO THROUGH RULES ---
    struct dlist_item *cur_rule = cur->head;
    while (cur_rule != NULL)
    {
        struct dlist_item *next = cur_rule->next;
        struct rule *temp = cur_rule->data;
        if (temp)
        {
            if (!temp)
                return;

            // --- GO THROUGH RECIPES ---
            struct dlist_item *cur_recipe = temp->recipe->head;
            while (cur_recipe != NULL)
            {
                struct dlist_item *next = cur_recipe->next;

                // --- EXPAND VARIABLES ---
                char *recipe_data = cur_recipe->data;
                char *str = variable_expansion(recipe_data, temp, data);

                // --- EXECUTE RECIPE ---
                exec_recipe(str);
                cur_recipe = next;
                free(str);
            }
        }
        cur_rule = next;
    }
}
