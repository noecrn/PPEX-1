#ifndef MINIMAKE_H
#define MINIMAKE_H

#include <stddef.h>

struct variable
{
    char *name;
    char *value;
};

struct rule
{
    char *target;
    struct dlist *dependencies;
    struct dlist *recipe;
};

struct minimake
{
    struct dlist *variable;
    struct dlist *rule;
    struct dlist *processed_targets;
};

void print_output(struct minimake *data);
void destroy_struct(struct rule *rule);

#endif /* ! MINIMAKE_H */
