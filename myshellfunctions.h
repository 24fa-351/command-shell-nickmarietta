#define MYSHELL_H
#ifndef MYSHELL_H

void parse_arguments(const char* input, char **args);

void handle_redirect(char **args);

void handle_pipe(char *input);

void handle_cd(int argc, char *args[]);

#endif