#include <stdlib.h>
#include <solves.h>
#include <stdio.h>
#include <string.h>

#define OK 0x1
#define PLUS2 0x2
#define DNF 0x3

struct solve {
    size_t number;
    char *result;
    unsigned int penalty: 2;
    char *comment;
    char *scramble;
    char *date;
    float seconds;
};

struct solves {
    size_t count;
    struct solve *array;
};

solves *new_solves_from_bytes(const char *bytes, size_t count) {
    const char *header_string = "No.;Time;Comment;Scramble;Date;P.1\r\n";
    solves *solves = malloc(sizeof(solves));
    size_t pos = count;



    if (strncmp(bytes, header_string, strlen(header_string))) {
	return NULL;
    }

    while (bytes[--pos-1] != '\n');

    if (sscanf(bytes + pos, "%zu", &solves->count) != 1) {
	return NULL;
    }

    solves->array = malloc(solves->count * sizeof(struct solve));

    pos = strlen(header_string);

    for (size_t i = 0; i < solves->count; i++) {
	solves->array[i].number = i + 1;
    }

    return solves;
}

void free_solves(solves *solves) {
    free(solves->array);
    free(solves);
}
