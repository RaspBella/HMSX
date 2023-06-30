#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "split.h"

#define DEFAULT_TARGET 9.85

typedef long double double_t;

struct
{
    size_t total;
    size_t achieved;
    size_t lost_to_plus2;
    size_t lost_to_dnf;
}
count;

typedef struct
{
    size_t no;
    char* time;
    char* comment;
    char* scramble;
    struct
    {
        unsigned short year;
        unsigned short month;
        unsigned short day;
        unsigned short hour;
        unsigned short minute;
        unsigned short second;
    }
    date;
    double_t part_one;
}
cstimer_solve;

void print_cstimer_solve(const cstimer_solve solve)
{
    printf(
        "Date: %hu-%hu-%hu %hu:%hu:%hu\n%zu. %s",
        solve.date.year, solve.date.month, solve.date.day,
        solve.date.hour, solve.date.minute, solve.date.second,
        solve.no,
        solve.time
    );
    if (solve.comment[0] != '\0')
    {
        printf("[%s]", solve.comment);
    }
    printf("\t%s\n", solve.scramble);
}

void print_cstimer_solves(const cstimer_solve* solves, size_t start, size_t end)
{
    for (size_t i = start; i < end; i++)
    {
        print_cstimer_solve(solves[i]);
        if (i != end-1) printf("\n");
    }
}

int main(int argc, char** argv)
{
    if (argc > 3) // Too many args
    {
        fprintf(stderr, "Too many arguments provided\n");
        exit(EXIT_FAILURE);
    }
    else if (argc == 1) // Too little args
    {
        fprintf(stderr, "Usage: %s <filename> [target]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Try opening file in read mode
    FILE* fp = fopen(argv[1], "r");

    if (fp == NULL) // Check if above worked
    {
        fprintf(stderr, "File could not be read\n");
        exit(EXIT_FAILURE);
    }

    // Get target
    double_t target = DEFAULT_TARGET;
    if (argc == 3)
    {
        target = strtod(argv[2], NULL);
        if (!target) target = DEFAULT_TARGET; // !0 == true
    }

    // Init counters
    count.total = 0;
    count.achieved = 0;
    count.lost_to_plus2 = 0;
    count.lost_to_dnf = 0;

    struct
    {
        char* value;
        size_t length;
    }
    line = { NULL, 0 };

    size_t seek_value = getline(&line.value, &line.length, fp); // Go past 1st line (header)

    while (getline(&line.value, &line.length, fp) != EOF) // Count solves
    {
        count.total++;
    }

    fseek(fp, seek_value, SEEK_SET); // Go back to just before solves

    cstimer_solve* cstimer_solves = malloc(sizeof(cstimer_solve) * count.total);

    for (size_t i = 0; getline(&line.value, &line.length, fp) != EOF; i++) // Go through solves
    {
        size_t list_count;
        char** list = split(line.value, ';', &list_count);

        if (list_count != 6)
        {
            fprintf(stderr, "Count is: %zu instead of expected: 6\nAborting\n", list_count);
            
            // Freeing
            for (size_t j = 0; j < list_count; j++) free(list[j]);
            free(list);
            free(line.value);

            exit(EXIT_FAILURE);
        }

        cstimer_solves[i].no = strtoull(list[0], NULL, 10);
        cstimer_solves[i].time = strdup(list[1]);
        cstimer_solves[i].comment = strdup(list[2]);
        cstimer_solves[i].scramble = strdup(list[3]);
        sscanf(
            list[4], "%hu-%hu-%hu %hu:%hu:%hu",
            &cstimer_solves[i].date.year,
            &cstimer_solves[i].date.month,
            &cstimer_solves[i].date.day,
            &cstimer_solves[i].date.hour,
            &cstimer_solves[i].date.minute,
            &cstimer_solves[i].date.second
        );
        cstimer_solves[i].part_one = strtod(list[5], NULL);

        // Free the list
        for (size_t j = 0; j < list_count; j++) free(list[j]);
        free(list);
    }

    /*
    struct
    {
        size_t total;
        size_t achieved;
        size_t lost_to_plus2;
        size_t lost_to_dnf;
    }
    count;

    typedef struct
    {
        char* time;
        double_t part_one;
    }
    cstimer_solve;
    */


    for (size_t i = 0; i < count.total; i++)
    {
        size_t time_len = strlen(cstimer_solves[i].time);

        if (cstimer_solves[i].time[0] == 'D') // DNF
        {
            if (cstimer_solves[i].part_one < target) count.lost_to_dnf++;
        }
        else if (cstimer_solves[i].time[time_len-1] == '+') // Plus 2
        {
            if (cstimer_solves[i].part_one < target)
            {
                if (cstimer_solves[i].part_one+2 >= target) count.lost_to_plus2++;
                else count.achieved++;
            }
        }
        else // No penalty
        {
            if (cstimer_solves[i].part_one < target) count.achieved++;
        }
    }

    printf("%zu / %zu met the target, as a percentage(to 2 decimal places): %.2Lf%%\n", count.achieved, count.total, (double_t)count.achieved/count.total*100);

    if (count.lost_to_plus2) // Any value other than 0 is true
        printf("sadly %zu didn't meet the target because of a plus 2\n", count.lost_to_plus2);
    
    if (count.lost_to_dnf) // Any value other than 0 is true
        printf("sadly %zu didn't meet the target because of a dnf\n", count.lost_to_dnf);

    // Display solves
    print_cstimer_solves(cstimer_solves, 0, count.total);

    // Free cstimer solves
    for (size_t i = 0; i < count.total; i++)
    {
        free(cstimer_solves[i].time);
        free(cstimer_solves[i].comment);
        free(cstimer_solves[i].scramble);
    }
    free(cstimer_solves);

    free(line.value);

    fclose(fp);
}