#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    // Stop the first string at the separator
    separator[0] = '\0';
    // *separator = '\0';
    char *section = section_key;

    // Start the next string after the separator
    char *key = separator + 1;

    // Initiate read-only file
    FILE *ini_file = fopen(ini_file_name, "r");

    char buffer[16];

    size_t len = sizeof(buffer);
    // Read line by line
    // Pointer to buffer
    char *line = malloc(len);

    if (line == NULL)
    {
        perror("Out of memory");
        return 1;
    }

    line[0] = '\0';

    while (fgets(buffer, sizeof(buffer), ini_file) != NULL)
    {
        // Check if buffer is full
        size_t line_used = strlen(line);
        size_t buffer_used = strlen(buffer);

        if (len - line_used < buffer_used)
        {
            printf("full\n");
            len *= 2;
            if ((line = realloc(line, len)) == NULL)
            {
                perror("Out of memory");
                free(line);
                return 1;
            }
        }

        // Copy buffer to line
        strncpy(line + line_used, buffer, len - line_used);
        line_used += buffer_used;

        // check if the line is fully contained - the last char is a newline
        if (line[line_used - 1] == '\n')
        {
            printf(line);
            // printf("Length: %d\n", line_used);

            // Reset buffer
            line[0] = '\n';
        }

        // printf("Line: %d, buffer: %d\n", line_used, buffer_used);
    }

    printf("Max size: %d", len);

    fclose(ini_file);
    free(line);

    return 0;
}