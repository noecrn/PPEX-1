#ifndef EXPANSION_H
#define EXPANSION_H

#include "minimake.h"

char *expand_recipe(char *str, struct rule *cur_rule, struct minimake *data);
char *expand_immediate(char *str, struct minimake *data);
char *expand(char *str, struct minimake *data);

#endif /* ! EXPANSION_H */
