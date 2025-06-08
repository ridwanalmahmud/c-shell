#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_INPUT_LENGTH 1024
#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 64
#define CMD_LIST_SIZE 3

typedef enum { CMD_ECHO = 0, CMD_EXIT, CMD_TYPE, CMD_NONE = -1 } CommandId;

static const char *cmd_list[] = {"echo", "exit", "type"};

CommandId identify_command(const char *command);
bool is_whitespace(char c);
char *trim_leading_whitespace(char *str);
int tokenize_input(char *input, char *tokens[]);
void execute_echo(char *args[]);
void execute_exit(char *args[]);
void execute_type(char *args[]);

#endif // !SHELL_H
