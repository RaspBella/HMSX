#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>

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
    double_t result;
}
Solve;

char** split_string(const char* str, char delim)
{
    size_t count = 0;
    const char* p = str;
    while (*p)
    {
        if (*p == delim) count++;
        p++;
    }
    count++; // Last or only item

    // Memory allocation for array of char*
    char** list = (char**)malloc((count+1) * sizeof(char*));
    if (list == NULL)
    {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Split the string and store the substrings
    size_t index = 0;
    const char* start = str;
    p = str;
    while (*p)
    {
        if (*p == delim)
        {
            size_t len = p - start;
            list[index] = (char*)malloc((len+1) * sizeof(char));
            if (list[index] == NULL)
            {
                printf("Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
            strncpy(list[index], start, len);
            list[index][len] = '\0';
            index++;
            start = p+1;
        }
        p++;
    }

    // Deal with the last or only char*
    size_t len = p - start;
    list[index] = (char*)malloc((len+1) * sizeof(char));
    if (list[index] == NULL)
    {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    strncpy(list[index], start, len);
    list[index][len] = '\0';

    // Set the final NULL pointer
    list[count] = NULL;

    return list;
}

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
    double_t target;
    if (argc == 3) // if provided
    {
        bool decimal = false;
        for (size_t i = 0; i < strlen(argv[2]); i++)
        {
            if (argv[2][i] == '.') // if currently decimal point
            {
                if (decimal) // if already decimal point
                {
                    printf("Invalid number\nToo many decimal points\n");
                    return EXIT_FAILURE;
                }
                else decimal = true;
            }
            else if (!(argv[2][i] >= '0' && argv[2][i] <= '9')) // if currently not digit
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
    if (count.total < 1)
    {
        printf("No solves!?\n");
        fclose(fp);
        return EXIT_FAILURE;
    }
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
        char** str_list = split_string(line.value, ';');

        // Solve number is elem 0
        size_t index = atol(str_list[0])-1; // Index is one less than solve number

        // Initializing the current solve
        solves[index].plus2 = false;
        solves[index].dnf = false;

        // Time shenanigans
        time_t rawtime;
        time(&rawtime);

        // Continuing on with initialization of the current solve
        solves[index].time = localtime(&rawtime);
        solves[index].result = 0.00;

        // Time is elem 1
        switch (str_list[1][strlen(str_list[1])-1]) // Check last charater of "Time"
        {
            case ')': // DNF's have bracket as last like DNF(9.23)
                solves[index].dnf = true;
                break;

            case '+': // Plus 2's simply have a + like 11.23+
                solves[index].plus2 = true;
                break;
        }
        
        // Date is elem 4
        if (strptime(str_list[4], DATE_FORMAT, solves[index].time) == NULL) // Uh oh if this is true but otherwise amazing
        {
            printf("Error scanning date for solve %ld\ndate: %s\n", index+1, str_list[4]);
            // Free memory, close file and then exit
            for (size_t i = 0; i < 6; i++) free(str_list[i]);
            free(str_list);
            free(line.value);
            fclose(fp);
            free(solves);
            return EXIT_FAILURE;
        }

        // P.1 is elem 5
        solves[index].result = atof(str_list[5]); // Token 5 of line is for "P.1"

        // Free the list
        for (size_t i = 0; i < 6; i++) free(str_list[i]);
        free(str_list);
    }

    free(line.value); // Free memory after finnished reading lines
    fclose(fp); // Close file after finnished reading it



    /*
        #################
            Counting!
        #################
    */

    for (size_t i = 0; i < count.total; i++) // Go through solves with an index i
    {
        if (solves[i].dnf) // If current solve is a DNF
        {
            // Check if it would've met the target without the DNF
            if (solves[i].result < target) count.lost_to_dnf++;
        }
        else if (solves[i].plus2) // If current solve is a plus 2
        {
            // Check result because doesn't include the 2 seconds
            if (solves[i].result < target) count.lost_to_plus2++;
        }
        else // Otherwise must be no penalty
        {
            // Just check result
            if (solves[i].result < target) count.achieved++;
        }
        if (solves[i].result < target) // Check wether result meets target
        {
            if (solves[i].dnf) count.lost_to_dnf++; // If DNF
            // If plus 2 would cause result to not meet target (result doesn't count +2's 🤣)
            else if (solves[i].plus2 && !(solves[i].result+2 < target)) count.lost_to_plus2++;
            else count.achieved++; // Otherwise great
        }
    }



    /*
        ##########################
            Displaying results
        ##########################
    */

    if (count.total > 1)
    {
        printf(
            "From %d-%d-%d to %d-%d-%d:\n",
            solves[0].time->tm_year+1900,
            solves[0].time->tm_mon,
            solves[0].time->tm_mday,
            solves[count.total-1].time->tm_year+1900,
            solves[count.total-1].time->tm_mon,
            solves[count.total-1].time->tm_mday
        );
    }
    else
    {
        printf(
            "From %d-%d-%d:\n",
            solves[0].time->tm_year+1900,
            solves[0].time->tm_mon,
            solves[0].time->tm_mday
        );
    }

    printf("%ld / %ld met the target, as a percentage(to 2 decimal places): %.2f%%\n", count.achieved, count.total, (double_t)count.achieved/count.total*100);

    if (count.lost_to_plus2) // Any value other than 0 is true
        printf("sadly %ld didn't meet the target because of a plus 2\n", count.lost_to_plus2);
    
    if (count.lost_to_dnf) // Any value other than 0 is true
        printf("sadly %ld didn't meet the target because of a dnf\n", count.lost_to_dnf);

    free(solves); // Free memory after finnished
}