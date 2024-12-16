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

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <output file> <command> [<arg1>, <arg2>, ...]\n");
        exit(EXIT_FAILURE);
    }

    char *output_file = argv[1];
    char *command = argv[2];
    char *args[MAX_TOKENS];

    // Copy the arguments into the args array
    args[0] = command;

    for (int ix = 3; ix < argc; ix++)
    {
        args[ix - 2] = argv[ix];
    }
    // Set the last argument to NULL
    args[argc - 2] = NULL;

    // time for thread
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd == -1)
        {
            perror("Failed to open output file");
            exit(EXIT_FAILURE);
        }
        if (dup2(output_fd, STDOUT_FILENO) == -1)
        {
            perror("Failed to redirect standard output");
            exit(EXIT_FAILURE);
        }
        close(output_fd);

        if (strchr(command, '/') != NULL)
        {
            // would mean it is a pipe
            handle_pipe(command);
        }
        else
        {
            // if not pipe, then command
            // argc - 2 because we are not counting the output file and the command
            handle_cd(args);
        }

        exit(EXIT_SUCCESS);
    }
    else
    {
        // parent process and we need to wait for the child process to finish
        wait(NULL);
    }
    return 0;
}