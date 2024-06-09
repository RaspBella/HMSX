#include <stdlib.h>
#include <solve.h>

struct solve {
    size_t number;
    char *result;
    unsigned int penalty: 2;
    char *comment;
    char *scramble;
    char *date;
    float seconds;
};

solve *new_solves_from_bytes(char *bytes, size_t count) {
}

void free_solves(solve *solves) {
}
