#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <solve.h>
#include <stdbool.h>

FILE *fp;
float target;

void init(int argc, char **argv)
{
    if (argc < 3)
    {
	fprintf(stderr, "Usage: %s `file.csv` `9.85`\n", *argv);

	exit(EXIT_FAILURE);
    }



    fp = fopen(argv[1], "r");

    if (!fp) // check if NULL
    {
	fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));

	exit(EXIT_FAILURE);
    }



    target = atof(argv[2]);

    if (!target)
    {
	fprintf(stderr, "Please provide a float that is not 0\n");

	fclose(fp);

	exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv)
{
    init(argc, argv);

    Solve *solve_p = read_solves(fp);

    print_solves(solve_p);
    
    fclose(fp);

    if (solve_p)
    {
	free_solves(solve_p);
    }

    return 0;
}
