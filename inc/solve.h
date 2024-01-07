#pragma once

#include <stdio.h>
#include <stdbool.h>

extern size_t solve_count;

typedef struct Solve Solve;

Solve *read_solves(FILE*);
void free_solves(Solve*, bool);
