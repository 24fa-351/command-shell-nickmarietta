#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#include "myshellfunctions.h"

#define MAX_INPUT_SIZE 1024
#define MAX_TOKENS 100

void parse_arguments(const char* input, char **args) {
    int ix = 0;
    char *token = strtok(strdup(input), " ");
    printf("Now begin parsing the arguments\n");
    while (token != NULL) {
        args[ix] = token;
        token = strtok(NULL, " ");
        ix++;
    }
    // Set the last argument to NULL
    args[ix] = NULL;
    printf("Finished parsing the arguments\n");
}

void execute_command(char **args) {
    if (execvp(args[0], args) == -1) {
        perror("Error executing command");
        exit(EXIT_FAILURE);
    }
}


void handle_redirect(char **args) {
    printf("Now handling redirect\n");
    int lx = -1;
    int fx = -1;
    char *inp_file = NULL;
    char *out_file = NULL;

    for (int ix = 0; args[ix] != NULL; ix++) {
        if (strcmp(args[ix], "<") == 0) {
            lx = ix;
            inp_file = args[ix + 1];
        } else if (strcmp(args[ix], ">") == 0) {
            fx = ix;
            out_file = args[ix + 1];
        }
    }
}