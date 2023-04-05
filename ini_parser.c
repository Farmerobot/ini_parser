#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define STARTING_SECTION_LINE_COUNT 10;

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
    char *end = str + strlen(str) - 1;
    while (isspace(*end))
    {
        end--;
    }
    end[1] = '\0';
    return str;
}

bool is_valid_name(char *str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        char cur = str[i];
        if (!isalnum(cur) && cur != '-')
        {
            return false;
        }
    }
    return true;
}

bool validate_section_memory_allocation(Section *section)
{
    if (section->name == NULL)
    {
        printf("Error allocation memory for section name\n");
        return 1;
    }
    if (section->keys == NULL || section->values == NULL)
    {
        printf("Error allocation memory for keys and values\n");
        return 1;
    }
    return 0;
}

void print_list(Section *head)
{
    Section *current = head;
    while (current != NULL)
    {
        printf("[ ");
        printf("Section: %s\n", current->name);
        printf("Number: %d\n", current->number_of_keys);
        for (int i = 0; i < current->number_of_keys; i++)
        {
            printf("key.val: %s.%s\n", current->keys[i], current->values[i]);
        }
        printf("]\n");
        current = current->next;
    }
}

void free_list(Section *head)
{
    Section *current = head;
    while (current != NULL)
    {
        free(current->name);
        for (int i = 0; i < current->number_of_keys; i++)
        {
            free(current->keys[i]);
            free(current->values[i]);
        }
        free(current->keys);
        free(current->values);

        Section *temp = current;
        current = current->next;
        free(temp);
    }
}

Section *section_factory(char *name)
{
    Section *new_section;
    new_section = malloc(sizeof(Section));
    new_section->name = malloc(strlen(name) + 1);
    strcpy(new_section->name, name);
    // Same as 2 lines above
    // new_section -> name = strdup(line);
    new_section->number_of_keys = 0;

    int line_count = STARTING_SECTION_LINE_COUNT;

    new_section->keys = malloc(line_count * sizeof(char *));
    new_section->values = malloc(line_count * sizeof(char *));

    return new_section;
}

char *find_expression_operator(char *section_key, char *operators)
{
    for (int i = 0; i < strlen(operators); i++)
    {
        char buffer[4];
        sprintf(buffer, " %c ", operators[i]);
        char *separator = strstr(section_key, buffer);
        if (separator != NULL)
        {
            return separator;
        };
    }
    return NULL;
}

char *parse_section_key_from_separator(char *section_key)
{
    // Find dot in ini query
    char *separator = strchr(section_key, '.');
    if (separator == NULL)
    {
        printf("Invalid query format\n");
        return NULL;
    }
    // Stop the first string at the separator
    separator[0] = '\0';
    // *separator = '\0';
    return separator + 1;
}

bool is_num(char *str)
{
    char *endptr;
    long n = strtol(str, &endptr, 10);
    // trim spaces
    while (isspace(*endptr))
        endptr++;
    return *endptr == '\0';
}

int main(int argc, char *argv[])
{
    // Check for `expression` cli argument
    char *expression = NULL;

    // Goals to find
    char *section_key = NULL;
    char *target_section_0 = "";
    char *target_section_1 = "";
    char *target_key_0 = NULL;
    char *target_key_1 = NULL;
    char operator= ' ';

    // Track found values
    bool found_section_0 = false;
    bool found_section_1 = false;
    char *target_value_0 = "";
    char *target_value_1 = "";

    // Check for number of arguments
    if (argc <= 2)
    {
        printf("Too few parameters\n");
        return 1;
    }
    // find section.key
    else if (argc == 3)
    {
        section_key = argv[2];

        target_section_0 = section_key;
        // Start the next string after the separator
        target_key_0 = parse_section_key_from_separator(section_key);
        if (target_key_0 == NULL)
        {
            printf("Invalid query format\n");
            return 1;
        }
    }
    // evaluate expression
    else if (argc == 4)
    {
        expression = argv[2];
        section_key = argv[3];

        if (strcmp(expression, "expression") != 0)
        {
            printf("Invalid command line parameter '%s'\n", expression);
            return 1;
        }

        char *separator = find_expression_operator(section_key, "+-*/");
        if (separator == NULL)
        {
            printf("Invalid query format\n");
            return 1;
        }
        operator= separator[1];
        separator[0] = '\0';
        separator += 2;

        char *section_key0 = section_key;
        target_section_0 = section_key0;
        target_key_0 = parse_section_key_from_separator(section_key0);

        char *section_key1 = separator + 1;
        target_section_1 = section_key1;
        target_key_1 = parse_section_key_from_separator(section_key1);

        // printf("section: '%s'\nkey: '%s'\n", target_section_0, target_key_0);
        // printf("section: '%s'\nkey: '%s'\n", target_section_1, target_key_1);

        // return 1;
        // char separator = find_expression_operator(section_key, "+-*/");
    }
    // argc > 4
    else
    {
        printf("Too many parameters\n");
        return 1;
    }
    printf("\n\n STARTING \n\n");
    // Extract arguments
    char *ini_file_name = argv[1];

    // Initiate read-only file
    FILE *ini_file = fopen(ini_file_name, "r");

    if (ini_file == NULL)
    {
        printf("Error opening ini file\n");
        return 1;
    }

    Section *head = NULL;

    char *buffer = NULL;
    char *line = NULL;
    size_t line_size = 0;
    int len;

    size_t max_section_line_count = STARTING_SECTION_LINE_COUNT;

    // -1 on EOF or error
    while ((len = getline(&buffer, &line_size, ini_file)) > 0)
    {
        line = trim_whitespace(buffer);
        if (line == NULL) {
            printf("Memory error\n");
            return 1;
        }
        // len = strlen(line);
        int length = strlen(line);

        char *equals = strchr(line, '=');

        // Ignore empty
        if (line[0] == ';' || line[0] == '\0')
        {
            continue;
        }
        // found section
        else if (line[0] == '[' && line[length - 1] == ']')
        {
            // Remove brackets
            line[length - 1] = '\0';
            line++;
            length -= 2;

            // Create struct and copy its name from line
            Section *new_section = section_factory(line);
            max_section_line_count = STARTING_SECTION_LINE_COUNT;

            if (strcmp(new_section->name, target_section_0) == 0)
            {
                found_section_0 = true;
            }
            if (strcmp(new_section->name, target_section_1) == 0)
            {
                found_section_1 = true;
            }

            // Init head
            new_section->next = head;
            head = new_section;

            if (validate_section_memory_allocation(head))
            {
                return 1;
            }

            if (!is_valid_name(new_section->name))
            {
                printf("Invalid section name: [%s]\n", new_section->name);
                continue;
            }

            // Reset counter for the purpose of memory allocation
            // printf("Head:    %s\n", head->name);
            // printf("Section: %s\n", new_section->name);
        }
        // found key
        else if (equals != NULL)
        {
            // printf("E: %s\n", equals);
            *equals = '\0';
            char *new_key = trim_whitespace(line);
            char *new_value = trim_whitespace(equals + 1);
            // printf("val: %s\n", new_value);

            if (head == NULL)
            {
                printf("Invalid key - no section found for '%s'\n", new_key);
                // len = getline(&line, &line_size, ini_file);
                continue;
            }
            if (!is_valid_name(new_key))
            {
                printf("Invalid key name: [%s]\n", new_key);
                continue;
            }
            // Print out target if found
            if (strcmp(target_section_0, head->name) == 0 && strcmp(new_key, target_key_0) == 0)
            {
                target_value_0 = strdup(new_value);
                if (expression == NULL)
                {
                    printf("The value of key '%s.%s' is: '%s'\n", target_section_0, target_key_0, new_value);
                }
            }
            // Look for expression target
            if (strcmp(target_section_1, head->name) == 0 && strcmp(new_key, target_key_1) == 0)
            {
                target_value_1 = strdup(new_value);
            }

            // Resize section struct string array
            if (head->number_of_keys >= max_section_line_count)
            {
                max_section_line_count += 10;
                head->keys = realloc(head->keys, max_section_line_count * sizeof(char *));
                head->values = realloc(head->values, max_section_line_count * sizeof(char *));

                if (head->keys == NULL || head->values == NULL)
                {
                    printf("Error allocation memory for keys and values\n");
                    return 1;
                }
            }
            head->keys[head->number_of_keys] = strdup(new_key);
            head->values[head->number_of_keys] = strdup(new_value);
            // head->keys[head->number_of_keys] = malloc(strlen(new_key) * sizeof(char) + 1);

            head->number_of_keys++;
        }
        // printf("%d\n", length);
        // printf("length: %zd, length: %zd\n", line_size, length);
        // len = getline(&line, &line_size, ini_file);
        // printf("%s\n", line);
    }
    // print_list(head);

    bool evaluate_expression = false;

    // Handle not found
    if (!found_section_0)
    {
        printf("Failed to find section [%s]\n", target_section_0);
    }
    else if (strcmp(target_value_0, "") == 0)
    {
        printf("Failed to find key \"%s\" in section [%s]\n", target_key_0, target_section_0);
    }
    else
    {
        // found no errors
        evaluate_expression = true;
    }

    if (expression != NULL)
    {
        if (!found_section_1)
        {
            printf("Failed to find section [%s]\n", target_section_1);
        }
        else if (strcmp(target_value_1, "") == 0)
        {
            printf("Failed to find key \"%s\" in section [%s]\n", target_key_1, target_section_1);
        }
        // Check for any not found values
        else if (evaluate_expression)
        {
            bool is_int_0 = is_num(target_value_0);
            bool is_int_1 = is_num(target_value_1);

            // Error
            if (is_int_0 != is_int_1)
            {
                printf("Invalid expression involving operands of different types: \"%s %c %s\"\n", target_value_0, operator, target_value_1);
            }
            // Evaluate 2 integers
            else if (is_int_0)
            {
                int result;
                int value_0 = atoi(target_value_0);
                int value_1 = atoi(target_value_1);
                switch (operator)
                {
                case '+':
                    result = value_0 + value_1;
                    break;
                case '-':
                    result = value_0 - value_1;
                    break;
                case '*':
                    result = value_0 * value_1;
                    break;
                case '/':
                    result = value_0 / value_1;
                    break;
                default:
                    printf("Invalid operation\n");
                    break;
                }
                printf("The value of the expression is: [%d]\n", result);
            }
            // Evaluate 2 strings
            else
            {
                if (operator == '+')
                {
                    printf("The value of the expression is: [%s%s]\n", target_value_0, target_value_1);
                }
                else
                {
                    printf("Invalid operation\n");
                }
            }
        }
    }

    fclose(ini_file);
    free(buffer);
    free(target_value_0);
    free(target_value_1);
    free_list(head);

    return 0;
}
