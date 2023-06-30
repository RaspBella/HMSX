#pragma once

#include <stdlib.h>
#include <string.h>

char** split(char* str, char delim, size_t* count)
{
    *count = 0;

    // Loop for char in str and change delim to null char and count null chars
    for (size_t i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == delim)
        {
            str[i] = '\0';
            (*count)++;
        }
    }

    char** list = malloc(sizeof(char*) * (++*count)); // Allocate for count many strings (count was 1 count of delims but becomes count of elems)
    if (list == NULL)
    {
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < *count; i++) // Loop through list before last elem
    {
        size_t len = strlen(str);
    
        list[i] = strdup(str);
        
        for (size_t j = 0; j < len+1; j++) str++; // Go to next "string" in string
    }

    return list;
}