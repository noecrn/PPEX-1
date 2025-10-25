#ifndef EXPANSION_H
#define EXPANSION_H

#include "minimake.h"

char *expand(char *token, struct minimake *data, struct rule *cur_rule);

#endif /* ! EXPANSION_H */
