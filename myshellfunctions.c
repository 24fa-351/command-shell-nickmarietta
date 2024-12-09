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

void parse_arguments(const char *input, char **args)
{
    int ix = 0;
    char *token = strtok(strdup(input), " ");
    printf("Now begin parsing the arguments\n");
    while (token != NULL)
    {
        args[ix] = token;
        token = strtok(NULL, " ");
        ix++;
    }
    // Set the last argument to NULL
    args[ix] = NULL;
    printf("Finished parsing the arguments\n");
}

void execute_command(char **args)
{
    if (execvp(args[0], args) == -1)
    {
        perror("Error executing command");
        exit(EXIT_FAILURE);
    }
}

// this function will handle the redirection for the commands
void handle_redirect(char **args)
{
    printf("Now handling redirect\n");
    int input_filedescriptor = -1;
    int output_filedescriptor = -1;
    char *inp_file = NULL;
    char *out_file = NULL;

    for (int ix = 0; args[ix] != NULL; ix++)
    {
        if (strcmp(args[ix], "<") == 0)
        {
            inp_file = args[ix + 1];
            // reassign the file descriptor
            args[ix] = NULL;
        }
        else if (strcmp(args[ix], ">") == 0)
        {
            out_file = args[ix + 1];
            args[ix] = NULL;
        }
    }

    if (inp_file != NULL)
    {
        input_filedescriptor = open(inp_file, O_RDONLY);
        if (input_filedescriptor == -1)
        {
            perror("Error opening input file");
            exit(EXIT_FAILURE);
        }
        dup2(input_filedescriptor, STDIN_FILENO);
        close(input_filedescriptor);

        if (dup2(input_filedescriptor, STDIN_FILENO) == -1)
        {
            perror("Error redirecting input file");
            exit(EXIT_FAILURE);
        }
        // handled the input file and now closing :)
        close(input_filedescriptor);
    }

    if (out_file != NULL)
    {
        output_filedescriptor = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_filedescriptor == -1)
        {
            perror("Error opening output file");
            exit(EXIT_FAILURE);
        }

        if (dup2(output_filedescriptor, STDOUT_FILENO) == -1)
        {
            perror("Error redirecting output file");
            exit(EXIT_FAILURE);
        }
        // once again handled and now closing :)
        close(output_filedescriptor);
    }
}

// this function will handle the pipe for the commands
void handle_pipe(char *input) {
    printf("Now handling pipe\n");
    char *commands[2];
    commands[0] = strtok(input, "|");
    commands[1] = strtok(NULL, "|");

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Error creating pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (commands[1] == NULL) {
        fprintf(stderr, "Error: no command to pipe to\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (strchr(commands[0], '/') != NULL) {
            handle_pipe(commands[0]);
        } else {
            char *args[MAX_TOKENS];
            parse_arguments(commands[0], args);
            handle_redirect(args);
            execute_command(args);
        }
    } else {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        if (strchr(commands[1], '/') != NULL) {
            handle_pipe(commands[1]);
        } else {
            char *args[MAX_TOKENS];
            parse_arguments(commands[1], args);
            handle_redirect(args);
            execute_command(args);
        }
    }
}

// handles commands that are not built-in (search with execvp)
void handle_cd(int argc, char *args[]) {
    printf("Now handling cd\n");
    for (int ix = 0; args[ix] != NULL; ix++) {
        if (chdir(args[ix]) == -1) { // check for error
            perror("Error changing directory");
            exit(EXIT_FAILURE);
        }
    }

    if (execvp(args[0], args) == -1) {
        perror("Error executing command");
        exit(EXIT_FAILURE);
    }
}