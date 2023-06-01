#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define DEFAULT_TARGET 9.85
#define DATE_FORMAT "%Y-%m-%d %H:%M:%S"

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
    bool plus2;
    bool dnf;
    struct tm* time;
    float result;
}
Solve;

// For checking the solve whilst in development
void print_solve(Solve solve)
{
    printf("%d-%d-%d\n", solve.time->tm_year+1900, solve.time->tm_mon, solve.time->tm_mday);
    if (solve.dnf) printf("DNF(%.2f)\n", solve.result);
    else if (solve.plus2) printf("%.2f+\n", solve.result);
    else printf("%.2f\n", solve.result);
}

int main(int argc, char** argv)
{
    /*
        ###########################
            Argument validation
        ###########################
    */

    // #### checking arg count

    if (argc > 3)
    {
        printf("Too many arguements\n");
        return EXIT_FAILURE;
    }
    else if (argc == 1)
    {
        printf("Too little arguments\nPlease provide a file and optionally a target\n");
        return EXIT_FAILURE;
    }



    // #### checking arguments

    // Checking provided file
    FILE* fp = fopen(argv[1], "r");

    if (fp == NULL)
    {
        printf("Provided file not read successfully\n");
        return EXIT_FAILURE;
    }

    // Checking optionally provided target that is a number
    float target;
    if (argc == 3) // if provided
    {
        bool decimal = false;
        for (size_t i = 0; i < strlen(argv[2]); i++)
        {
            if (argv[2][i] == '.') // if indexly decimal point
            {
                if (decimal) // if already decimal point
                {
                    printf("Invalid number\nToo many decimal points\n");
                    return EXIT_FAILURE;
                }
                else decimal = true;
            }
            else if (!(argv[2][i] >= '0' && argv[2][i] <= '9')) // if indexly not digit
            {
                printf("Invalid number\nInvalid digit: %c found in %s\n", argv[2][i], argv[2]);
                for (size_t j = 0; j < i; j++) printf(" ");
                printf("                          ^\n");
                return EXIT_FAILURE;
            }
        }
        target = atof(argv[2]); // set target since validated
    }
    else target = DEFAULT_TARGET; // use default if not provided



    /*
        ############################
            Where the fun begins
        ############################
    */

    // Initialize incrementing variables
    count.total = 0;
    count.achieved = 0;
    count.lost_to_plus2 = 0;
    count.lost_to_dnf = 0;

    // Count newlines in file to get count total;
    while (!feof(fp))
    {
        if(fgetc(fp) == '\n') count.total++;
    }

    // Allocate solves array
    Solve* solves = malloc(sizeof(Solve) * count.total);



    // #### Fill array

    rewind(fp); // Go back to the start of the file

    struct
    {
        char* value; // Chars
        size_t size; // How many?
    }
    line = {.value = NULL, .size = 0}; // Var for each line in file

    getline(&line.value, &line.size, fp); // Skip header line

    while (getline(&line.value, &line.size, fp) != EOF) // Go through lines in file
    {
        char* token = strtok(line.value, ";"); // Get first token in line
        if (token == NULL) continue; // If token is empty continue to next line

        size_t index = atol(token)-1; // Index is one less than solve number

        // Initializing the current solve
        solves[index].plus2 = false;
        solves[index].dnf = false;

        // Time shenanigans
        time_t rawtime;
        time(&rawtime);

        // Continuing on with initialization of the current solve
        solves[index].time = localtime(&rawtime);
        solves[index].result = 0.00;

        for (size_t i = 1; i < 6; i++) // Loop through for rest of tokens using a counter
        {
            if (token == NULL) break; // If empty escape

            if (i == 1) // Token 1 of line is for "Time"
            {
                switch (token[strlen(token)-1]) // Check last charater of "Time"
                {
                    case ')': // DNF's have bracket as last like DNF(9.23)
                        solves[index].dnf = true;
                        break;

                    case '+': // Plus 2's simply have a + like 11.23+
                        solves[index].plus2 = true;
                        break;
                }
            }

            if (i == 4) // Token 4 of line is for "Date"
            {
                if (strptime(token, DATE_FORMAT, solves[index].time) == NULL) // Uh oh if this is true but otherwise amazing
                {
                    printf("Error scanning date\n");
                    // Free memory, close file and then exit
                    free(line.value);
                    fclose(fp);
                    free(solves);
                    return EXIT_FAILURE;
                }
            }

            if (i == 5) solves[index].result = atof(token); // Token 5 of line is for "P.1"
            // Which is used to set result

            token = strtok(NULL, ";"); // Get next token
        }
    }

    free(line.value); // Free memory after finnished reading lines
    fclose(fp); // Close file after finnished reading it



    /*
        #################
            Counting!
        #################
    */



    /*
        ##########################
            Displaying results
        ##########################
    */

    // Display information like below
    /*
        from {DATE} to {DATE}:
        {count} / {total} met the target equating to about {x.yz}%
        sadly {lost_to_plus2} didn't meet the target because of a plus 2
        sadly {lost_to_dnf} didn't meet the target because of a dnf
    */

    free(solves); // Free memory after finnished
}