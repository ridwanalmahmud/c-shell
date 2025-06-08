#include "../include/shell.h"

CommandId identify_command(const char *command) {
    for (int i = 0; i < CMD_LIST_SIZE; i++) {
        if (strcmp(command, cmd_list[i]) == 0) {
            return (CommandId)i;
        }
    }
    return CMD_NONE;
}

bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

char *trim_leading_whitespace(char *str) {
    while (is_whitespace(*str)) {
        str++;
    }
    return str;
}

int tokenize_input(char *input, char *tokens[]) {
    int count = 0;
    char *token = strtok(input, " \t\n");

    while (token != NULL && count < MAX_ARGS - 1) {
        tokens[count++] = token;
        token = strtok(NULL, " \t\n");
    }
    tokens[count] = NULL;
    return count;
}

void execute_echo(char *args[]) {
    if (args[1] != NULL) {
        // Print all arguments with spaces
        for (int i = 1; args[i] != NULL; i++) {
            printf("%s%s", args[i], (args[i+1] != NULL) ? " " : "");
        }
    }
    printf("\n");
}

void execute_exit(char *args[]) {
    int status = 0;
    if (args[1] != NULL) {
        status = atoi(args[1]);
    }
    exit(status);
}

void execute_type(char *args[]) {
    if (args[1] == NULL) {
        fprintf(stderr, "type: missing argument\n");
        return;
    }

    CommandId cmd_id = identify_command(args[1]);
    if (cmd_id != CMD_NONE) {
        printf("%s is a shell builtin\n", args[1]);
        return;
    }

    char *path_env = getenv("PATH");
    if (path_env == NULL) {
        printf("%s: not found\n", args[1]);
        return;
    }

    char *path_copy = strdup(path_env);
    if (path_copy == NULL) {
        perror("strdup");
        return;
    }

    bool found = false;
    char *dir = strtok(path_copy, ":");
    char full_path[MAX_COMMAND_LENGTH];

    while (dir != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, args[1]);
        if (access(full_path, X_OK) == 0) {
            printf("%s is %s\n", args[1], full_path);
            found = true;
            break;
        }
        dir = strtok(NULL, ":");
    }

    free(path_copy);

    if (!found) {
        printf("%s: not found\n", args[1]);
    }
}
