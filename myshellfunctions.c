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
    char *arg = strtok(input, " \n\t");
    int ix = 0;
    while (arg != NULL)
    {
        args[ix++] = arg;
        arg = strtok(NULL, " \n\t");
    }
    args[ix] = NULL;
}

void execute_command(char **args, int input_re, int output_re, char *input_file, char *output_file, int backgroundNum)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        if (input_re)
        {
            int input_fd = open(input_file, O_RDONLY);
            if (input_fd < 0)
            {
                fprintf(stderr, "invalid input file");
                exit(1);
            }
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        if (output_re)
        {
            int output_fd = open(output_file, O_WRONLY || O_CREAT || O_TRUNC, 0644);
            if (output_fd < 0)
            {
                fprintf(stderr, "invalid output file");
                exit(1);
            }
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        if (execvp(args[0], args) < 0)
        {
            fprintf(stderr, "Execution failed/can't find path");
            exit(1);
        }
    }
    else if (pid > 0)
    {
        if (!backgroundNum)
        {
            waitpid(pid, NULL, 0);
        }
    }
    else
    {
        fprintf(stderr, "the fork has failed.");
        exit(1);
    }
}

// handles commands that are not built-in (search with execvp)
void handle_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "no argument given\n");
    }
    else if (chdir(args[1]) != 0)
    {
        fprintf(stderr, "cd not working");
    }
}

// this is to handle the working directory
void handle_pwd()
{
    char command[MAX_INPUT_SIZE];
    if (getcwd(command, sizeof(command)) != NULL)
    {
        printf("%s\n", command);
    }
    else
    {
        fprintf(stderr, "pwd error");
    }
}

// this is to handle the set with env. variables
void handle_set(char **args)
{
    if (args[1] != NULL && args[2] != NULL)
    {
        if (setenv(args[1], args[2], 1) != 0)
        {
            fprintf(stderr, "setenv error");
        }
    }
    else
    {
        fprintf(stderr, "wrong usage of set");
    }
}

// this is to handle the unset for env. variables
void handle_unset(char **args)
{
    if (args[1] != NULL)
    {
        if (unsetenv(args[1]) != 0)
        {
            fprintf(stderr, "unsetenv error");
        }
    }
    else
    {
        fprintf(stderr, "wrong usage of unset");
    }
}

// this is for the echo variables
void handle_echo(char **args)
{
    for (int ix = 1; args[ix] != NULL; ++ix)
    {
        if (args[ix][0] == "$")
        {
            char *environment_val = getenv(args[ix] + 1);
            if (environment_val)
            {
                printf("this is the environment value: %s ", environment_val);
            }
            else
            {
                printf("this is not defined... :(");
            }
        }
        else
        {
            printf("total amount of arguments: %s", args[ix]);
        }
    }
    printf("\n");
}