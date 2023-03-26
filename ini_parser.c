#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct
{
    char *name;
    int number_of_keys;
    char **keys;
    char **values;
} Section;

int main(int argc, char *argv[])
{
    // Check for number of arguments
    if (argc < 3)
    {
        printf("Too few parameters");
        return 1;
    }
    // Extract arguments
    char *ini_file_name = argv[1];
    char *section_key = argv[2];

    // Find dot in ini query
    char *separator = strchr(section_key, '.');
    if (separator == NULL)
    {
        printf("Invalid query format");
        return 1;
    }
    printf("\n\n STARTING \n\n");
    // printf("\n\n STARTING \n\n");
    // printf("\n\n STARTING \n\n");
    // Stop the first string at the separator
    separator[0] = '\0';
    // *separator = '\0';
    char *section = section_key;

    // Start the next string after the separator
    char *key = separator + 1;

    // Initiate read-only file
    FILE *ini_file = fopen(ini_file_name, "r");

    if (ini_file == NULL)
    {
        printf("Error opening ini file");
        return 1;
    }

    char *line = NULL;
    size_t line_size = 0;
    ssize_t len = getline(&line, &line_size, ini_file);

    // -1 on EOF or error
    while (len >= 0)
    {
        // trim front
        while (isspace(*line))
        {
            line++;
            len--;
        }

        // trim back
        char *end = line + len - 2;
        while (isspace(*end))
        {
            end--;
            len--;
        }
        end[1] = '\0';
        
        // found section
        if (line[0] == '[' && line[len - 2] == ']')
        {

            // Remove brackets
            line[len - 2] = '\0';
            line++;
            len -= 2;

            // Create struct
            Section *new_section = malloc(sizeof(Section));
            new_section->name = malloc(len * sizeof(line));
            strcpy(new_section->name, line);
            // Same as above
            // new_section -> name = strdup(line);

            printf("%s\n", new_section->name);
        }
        // printf("%d\n", len);
        // printf("Len: %zd, len: %zd\n", line_size, len);
        len = getline(&line, &line_size, ini_file);
    }

    fclose(ini_file);
    free(line);

    return 0;
}