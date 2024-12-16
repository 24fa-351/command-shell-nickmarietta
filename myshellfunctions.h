#ifndef MYSHELL_H
#define MYSHELL_H

void parse_arguments(const char *input, char **args);

void execute_command(char **args, int input_re, int output_re, char *input_file, char *output_file, int backgroundNum);

void handle_cd(char **args);

void handle_pwd();

void handle_set(char **args);

void handle_unset(char **args);

void handle_echo(char **args);

#endif