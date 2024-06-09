#pragma once

#include <stddef.h>

typedef struct solve solve;

solve *new_solves_from_bytes(char*, size_t);
void free_solves(solve *solves);
