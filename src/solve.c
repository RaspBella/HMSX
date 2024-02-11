#include <solve.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define SOLVED 0x1
#define PLUS2 0x2
#define DNF 0x3

size_t solve_count = 0;

struct Solve
{
    float time;
    unsigned short state;
};

static size_t count_solves(FILE *fp)
{
    size_t newline_count = 0;
    int c = getc(fp);

    while ((c = getc(fp)) != EOF)
	if (c == '\n') newline_count++;
    
    rewind(fp);

    while ((c = getc(fp)) != EOF)
	if (c == '\n')
	{
	    fseek(fp, -1, SEEK_CUR);
	    break;
	}

    return newline_count;
}

static bool parse_solve(Solve *solve, const char *str)
{
    const char *start_dnf = "DNF(";

    const char *is_dnf = strstr(str, start_dnf);

    if (is_dnf)
    {
	solve->time = atof(str + strlen(start_dnf));
	solve->state = DNF;
    }
    else
    {
	solve->time = atof(str);

	if (str[strlen(str) - 1] == '+')
	    solve->state = PLUS2;
	else
	    solve->state = SOLVED;
    }

    return true;
}

Solve *read_solves(FILE *fp)
{
    solve_count = count_solves(fp);

    if (!solve_count) return NULL;


    Solve *solve_p = malloc(sizeof(Solve) * solve_count);


    for (size_t i = 0; i < solve_count; i++)
    {
	char *buffer = NULL;

	fscanf(fp, "%*zu;%m[^;]", &buffer);

	if (buffer)
	{
	    if (!parse_solve(solve_p + i, buffer))
		printf("Parse error for solve %zu\n", i+1);

	    free(buffer);
	}

	int c = getc(fp);

	while ((c = getc(fp)) != EOF)
	    if (c == '\n')
		break;
    }



    return solve_p;
}

static void print_solve(const Solve *solve, size_t n)
{
    switch (solve->state)
    {
	case SOLVED:
	    printf("%zu: %.2f\n", n, solve->time);
	    break;

	case PLUS2:
	    printf("%zu: %.2f+\n", n, solve->time);
	    break;

	case DNF:
	    printf("%zu: DNF(%.2f)\n", n, solve->time);
	    break;
    }
}

static void print_solves(const Solve *solve_p)
{
    for (size_t i = 0; i < solve_count; i++)
	print_solve(solve_p + i, i+1);
}

void evaluate_solves(Solve *solve_p, float target)
{
    // some counters
    size_t pass = 0;
    size_t miss_by_plus2 = 0;
    size_t miss_by_dnf = 0;
    size_t miss = 0;

    for (size_t i = 0; i < solve_count; i++)
    {
	switch (solve_p[i].state)
	{
	    case SOLVED:
		if (solve_p[i].time < target) pass++;
		else miss++;
		break;

	    case PLUS2:
		if (solve_p[i].time < target) pass++;
		else if ((solve_p[i].time - 2) < target) miss_by_plus2++;
		else miss++;
		break;

	    case DNF:
		if (solve_p[i].time < target) miss_by_dnf++;
		else miss++;
		break;
	}
    }

    printf("%zu/%zu met the target\n", pass, solve_count);
    if (miss_by_plus2) printf("%zu would've met target if not plus 2\n", miss_by_plus2);
    if (miss_by_dnf) printf("%zu would've met target if not dnf\n", miss_by_dnf);
    if (miss) printf("%zu non penalty solves don't meet the target\n", miss);
}

void free_solves(Solve *solve_p)
{
    free(solve_p);
}
