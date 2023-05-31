#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define DEFAULT_TARGET 9.85

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
    size_t count = 0;
    size_t total = 0;
    size_t lost_to_plus2 = 0;
    size_t lost_to_dnf = 0;
    // Loop through for each line in the file
    // Then increment count if current solve from line is faster than target
    // Then increment total
    // Then increment lost_to_plus2 if it's a plus 2
    // Then increment lost_to_dnf if it's a dnf
    // Display information like below
    /*
        from {DATE} to {DATE}:
        {count} / {total} met the target equating to about {x.yz}%
        sadly {lost_to_plus2} didn't meet the target because of a plus 2
        sadly {lost_to_dnf} didn't meet the target because of a dnf
    */

    //printf("%.2f\n", target);

    fclose(fp); // close file after finnished
}