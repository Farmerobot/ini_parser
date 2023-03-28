#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Section
{
    char *name;
    int number_of_keys;
    char **keys;
    char **values;

    struct Section *next;
} Section;

char *trim_whitespace(char *str)
{
    // trim front
    while (isspace(*str))
    {
        str++;
    }

    // trim back
    char *end = str + strlen(str) - 2;
    while (isspace(*end))
    {
        end--;
    }
    end[1] = '\0';
    return str;
}

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

    Section *head = NULL;

    char *line = NULL;
    size_t line_size = 0;
    ssize_t len = getline(&line, &line_size, ini_file);

    // -1 on EOF or error
    while (len >= 0)
    {
        line = trim_whitespace(line);
        len = strlen(line);

        char *equals = strchr(line, '=');

        // Ignore empty
        if (line[0] == ';' || line[0] == '\0')
        {

            // found section
        }
        else if (line[0] == '[' && line[len - 1] == ']')
        {
            // Remove brackets
            line[len - 1] = '\0';
            line++;
            len -= 2;

            // Create struct
            Section *new_section;
            new_section = malloc(sizeof(Section));
            new_section->name = malloc(len * sizeof(line));
            new_section->number_of_keys = 0;

            if (head == NULL)
            {
                head = new_section;
            }
            else
            {
                head->next = new_section;
                new_section = head;
            }
            strcpy(new_section->name, line);

            // Same as above
            // new_section -> name = strdup(line);

            for (int i = 0; i < len; i++)
            {
                char cur = new_section->name[i];
                if (!isalnum(cur) && cur != '-')
                {
                    printf("Invalid section name: [%s]\n", new_section->name);
                    len = getline(&line, &line_size, ini_file);
                }
            }

            printf("Head:    %s\n", head->name);
            printf("Section: %s\n", new_section->name);
            // found key
        }
        else if (equals != NULL)
        {
            *equals = '\0';
            char *new_key = trim_whitespace(line);
            char *new_value = trim_whitespace(equals + 1);
            
            if (head == NULL) {
                printf("Invalid key - no section found for '%s'\n", new_key);
                len = getline(&line, &line_size, ini_file);
                continue;
            }
        }
        // printf("%d\n", len);
        // printf("Len: %zd, len: %zd\n", line_size, len);
        len = getline(&line, &line_size, ini_file);
    }

    fclose(ini_file);
    free(line);

    return 0;
}