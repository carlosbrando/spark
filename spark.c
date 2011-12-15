#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

bool received_bash_pipe(void);
int *split_string(char *string);
char *remove_unwanted_characters(char *string);
char *join_arguments(char *arguments[], int count);
void print_help_message(void);

int main(int argc, char *argv[]) {
    char pipe_args[4096];
    char *arguments;
    int i, current_value, count;
    int min = INT_MAX, max = INT_MIN;
    int *numerical_values;
    char *ticks[] = {"▁", "▂", "▃", "▄", "▅", "▆", "▇", "█"};
    
    if (argc <= 1) {
        if (received_bash_pipe()) {
            fread(pipe_args, sizeof(char), 4096, stdin);
            numerical_values = split_string(pipe_args);
        } else {
            print_help_message();
            return 0;
        }
    } else {
        arguments = join_arguments(++argv, argc - 1);
        numerical_values = split_string(arguments);
    }
    
    /* counts how many records were passed */
    for (count = 0; numerical_values[count] != INT_MIN; ++count);
    
    /* search for lowest and highest values */
    for (i = 0; i < count; ++i) {
        if (numerical_values[i] < min) min = numerical_values[i]; /* retrieves the lowest value */
        if (numerical_values[i] > max) max = numerical_values[i]; /* retrieves the highest value */
    }
    
    /* print the graphs */
    for (i = 0; i < count; ++i) {
        current_value = (numerical_values[i] * 7) / max;
        if (current_value < 0) current_value = 0;
        printf("%s", ticks[current_value]);
    }
    
    return 0;
}

/* Check if values were received via bash pipe */
bool received_bash_pipe(void) {
    long size;
    
    /* obtain file size: */
    fseek(stdin, 0, SEEK_END);
    size = ftell(stdin);
    rewind(stdin);
    
    return size != 0 ? true : false;
}

/* Join all parameters into a single string to enable 
 * the use of commas */
char *join_arguments(char *arguments[], int count) {
    char *buffer;
    register int i;
    
    buffer = (char *)malloc(sizeof(char));
    
    for (i = 0; i < count; ++i) {
        buffer = (char *)realloc(buffer, (strlen(buffer) + strlen(arguments[i]) + 1) * sizeof(char));
        strcat(buffer, arguments[i]);
        strcat(buffer, " "); /* adds a space after the value */
    }

    return buffer;
}

/* Split the string into an array of integers 
 * regardless of how they were passed. */
int *split_string(char *string) {
    register int i = 0;
    char *new_string, *p;
    int *result;

    new_string = remove_unwanted_characters(string);
    
    result = (int *)malloc(sizeof(int));
    p = strtok(new_string, " ");

    while (p != NULL) {
        result = (int *)realloc(result, ++i * sizeof(int));
        result[i-1] = atoi(p);
        p = strtok(NULL, " ,");
    }

    result = (int *)realloc(result, ++i * sizeof(int));
    result[i-1] = INT_MIN;

    return result;
}

/* Remove caracteres como vírgula e quebra de linhas */
char *remove_unwanted_characters(char *string) {
    char *buffer;
    size_t size;
    register int i;
    
    size = strlen(string);
    buffer = (char *)malloc(size * sizeof(char));
    
    for (i = 0; i < size; ++i) 
        buffer[i] = string[i] == '\n' || string[i] == ',' ? ' ' : string[i];
    
    return buffer;
}

/* Print in terminal a message explaining how to use 
 * the application */
void print_help_message(void) {
    printf("USAGE:\n"
		   "  spark [-h] VALUE,...\n\n"
		   "EXAMPLES:\n"
		   "  spark 1 5 22 13 53\n"
		   "  ▁▁▃▂█\n"
		   "  spark 0,30,55,80,33,150\n"
		   "  ▁▂▃▄▂█\n"
		   "  echo 9 13 5 17 1 | spark\n"
		   "  ▄▆▂█▁\n");
}