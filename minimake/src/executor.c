#include "minimake.h"
#include "expansion.h"
#include "destroy.h"
#include "dlist/dlist.h"

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

enum target_status
{
    NOTHING_TO_BE_DONE, // RECIPE ALREADY EMPTY AND UP-TO-DATE
    UP_TO_DATE, // FILE ALREADY EXIST AND UP-TO-DATE
    TO_BUILD, // TARGET HAS BEEN BUILT
    ERROR // ERROR OCCURED
};

static int exec_recipe(char *str, struct rule *rule, struct minimake *data)
{
    // --- EXPAND VARIABLES ---
    char *expanded = expand_recipe(str, rule, data);

    if (!expanded)
        return 1;

    // --- CHECK IF THERE IS A COMMAND ---
    char *cmd = expanded;
    if (expanded[0] == '@')
        cmd++;
    else
        printf("%s\n", cmd);

    // --- FORCE TO DISPLAY BEFORE EXECUTION ---
    fflush(stdout);

    // --- INIT CHILD ---
    pid_t pid = fork();
    if (pid < 0)
    {
        free(expanded);
        errx(2, "Fork fail. Stop");
    }
    // --- EXEC COMMAND IN THE CHILD ---
    else if (pid == 0)
    {
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        destroy_minimake(data);
        errx(2, "Error during command execution. Stop.");
    }

    // --- WAIT FOR THE CHILD TO FINISH ---
    int wstatus;
    waitpid(pid, &wstatus, 0);
    free(expanded);

    // --- IF COMMAND FAILED ---
    if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != 0)
    {
        fprintf(stderr, "minimake: Command failed (error code %d). Stop.\n", WEXITSTATUS(wstatus));
        return 1;
    }

    return 0;
}

static struct rule *find_rule(char *target_name, struct minimake *data)
{
    struct dlist_item *cur_rule = data->rule->head;

    // --- GO THROUGH RULES ---
    while (cur_rule != NULL)
    {
        struct rule *rule = cur_rule->data;
        if (strcmp(rule->target, target_name) == 0)
        {
            return rule;
        }
        cur_rule = cur_rule->next;
    }

    return NULL;
}

static time_t get_file_time(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) == 0)
    {
        return statbuf.st_mtime;
    }
    return 0;
}

static enum target_status build_target(char *target_name, struct minimake *data)
{
    // --- FIND THE RULE ---
    struct rule *rule = find_rule(target_name, data);
    if (!rule)
    {
        // --- IF THE FILE ALREADY EXIST ---
        if (get_file_time(target_name) != 0)
        {
            return UP_TO_DATE;
        }

        fprintf(stderr, "minimake: No rule to make target '%s'. Stop.\n", target_name);
        return ERROR;
    }

    // --- FLAG TO KNOW IF AT LEAST ONE DEPENDENCIE HAS BEEN BUILD ---
    bool flag = false;
    struct dlist_item *cur_dep = rule->dependencies->head;

    // --- GOT THROUGH DEPENDENCIES ---
    while (cur_dep != NULL)
    {
        // --- RECURSIVE CALL WITH DEPENDENCIE ---
        enum target_status dep_status = build_target(cur_dep->data, data);

        // --- IF RECURSIVE CALL FAIL ---
        if (dep_status == ERROR)
            return ERROR;

        // --- IF DEPENDENCIE WAS BUILT ---
        if (dep_status == TO_BUILD)
            flag = true;

        cur_dep = cur_dep->next;
    }

    // --- CHECK TARGET BUILD TIME ---
    time_t target_time = get_file_time(target_name);

    // --- CHECK IF WE MUST BUILD ---
    bool do_we_build = false;

    // --- IF TARGET DOESN'T EXIT ---
    if (target_time == 0)
    {
        do_we_build = true;
    }
    // --- AT LEAST ONE DEPENDENCIE HAS BEEN BUILD ---
    else if (flag)
    {
        do_we_build = true;
    }
    // --- TARGET EXIST BUT A DEPENDENCIE IS MORE RECENT ---
    else
    {
        cur_dep = rule->dependencies->head;
        while (cur_dep)
        {
            time_t dep_time = get_file_time(cur_dep->data);
            // --- IF DEPENDENCIE IS MORE RECENT THAN TARGET ---
            if (dep_time > target_time)
            {
                do_we_build = true;
                break;
            }
        }
    }

    // --- IF TARGET DON'T HAVE RECIPE ---
    if (rule->recipe->size == 0)
    {
        if (!do_we_build)
            printf("minimake: Nothing to be done for '%s'.\n", target_name);
        return NOTHING_TO_BE_DONE;
    }

    // --- NO NEED TO BUILD ---
    if (!do_we_build)
    {
        printf("minimake: '%s' is up to date.\n", target_name);
        return UP_TO_DATE;
    }

    // --- WE NEED TO BUILD ---
    struct dlist_item *cur_recipe = rule->recipe->head;
    while (cur_recipe != NULL)
    {
        // --- EXECUTE COMMAND ---
        if (exec_recipe(cur_recipe->data, rule, data))
            return ERROR;
        cur_recipe = cur_recipe->next;
    }

    return TO_BUILD;
}

int executor(int argc, char *argv[], struct minimake *data)
{
    if (!data || !data->rule)
        errx(2, "No targets. Stop.");

    struct dlist *targets = dlist_init();

    // --- GO TRHOUGH ARGUMENTS ---
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-f") == 0)
        {
            i++;
        }
        else if (strcmp(argv[i], "-p") == 0)
        {
            continue;
        }
        // --- IF TARGET FOUND, PUSH IT TO LIST ---
        else
        {
            dlist_push_back(targets, argv[i]);
        }
    }

    // --- IF NO TARGET FOUND, EXECUTE DEFAULT TARGET ---
    if (targets->size == 0)
    {
        struct rule *temp = data->rule->head->data;
        dlist_push_back(targets, temp->target);
    }

    // --- EXEC ARGUEMENTS ---
    for (struct dlist_item *cur = targets->head; cur != NULL; cur = cur->next)
    {
        if (build_target(cur->data, data) == ERROR)
            return 2;
    }

    return 0;
}
