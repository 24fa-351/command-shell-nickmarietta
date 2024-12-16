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

int main()
{
    char input[MAX_INPUT_SIZE];
    char *args[MAX_TOKENS];
    int backgroundNum = 0;
    int input_re = 0;
    int output_re = 0;
    char *input_file = NULL;
    char *output_file = NULL;

    // main loop that is infinite till we exit from user
    while (1)
    {
        printf("myshell> ");
        if (!fgets(input, MAX_INPUT_SIZE, stdin))
        {
            break;
        }

        parse_arguments(input, args);

        if (args[0] && (strcmp(args[0], "exit") == 0))
        {
            break;
        }
        if (args[0] && (strcmp(args[0], "cd") == 0))
        {
            handle_cd(args);
            continue;
        }
        if (args[0] && (strcmp(args[0], "pwd") == 0))
        {
            handle_pwd();
            continue;
        }
        if (args[0] && (strcmp(args[0], "set") == 0))
        {
            handle_set(args);
            continue;
        }
        if (args[0] && (strcmp(args[0], "unset") == 0))
        {
            handle_unset(args);
            continue;
        }
        if (args[0] && (strcmp(args[0], "echo") == 0))
        {
            handle_echo(args);
            continue;
        }

        backgroundNum = 0;
        input_re = 0;
        output_re = 0;
        input_file = NULL;
        output_file = NULL;

        if (strcmp(args[0], "&") == 0)
        {
            backgroundNum = 1;
            args[0] = NULL;
        }

        for (int ix = 0; args[ix] != NULL; ix++)
        {
            if (strcmp(args[ix], "<") == 0)
            {
                input_re = 1;
                input_file = args[ix + 1];
                args[ix] = NULL;
            }
            else if (strcmp(args[ix], ">") == 0)
            {
                output_re = 1;
                output_file = args[ix + 1];
                args[ix] = NULL;
            }

            if (input_re && output_re)
            {
                fprintf(stderr, "Can't have both input and output redirection\n");
                break;
            }
        }

        char *new_args[MAX_TOKENS];
        int pipe = 0;
        for (int ix = 0; args[ix] != NULL; ix++)
        {
            if (strcmp(args[ix], "|") == 0)
            {
                new_args[pipe] = NULL;
                pipe += 1;
                continue;
            }
            new_args[pipe++] = args[ix];
        }
        new_args[pipe] = NULL;

        if (pipe < 0)
        {
            execute_command(args, input_re, output_re, input_file, output_file, backgroundNum);
        }
        else
        {
            execute_command(new_args, input_re, output_re, input_file, output_file, backgroundNum);
        }
    }
    return 0;
}