#pragma once

#include <stddef.h>

typedef struct solves solves;

solves *new_solves_from_bytes(const char*, size_t);
void free_solves(solves*);
