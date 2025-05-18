#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define CMD_LIST_SIZE 3
#define MAX_INPUT_LENGTH 100
#define MAX_COMMAND_LENGTH 1024

char *cmd_list[] = {"echo", "exit", "type"};

int is_command(char *command) {
    for (int i = 0; i < CMD_LIST_SIZE; i++) {
        if (strcmp(command, cmd_list[i]) == 0) {
            return i;
        } else if (strcmp(command, cmd_list[i]) < 0) {
            return -1;
        }
    }
    return -1;
}

int main(int argc, char *argv[]) {
    setbuf(stdout, NULL); // Flush after every printf

    while (true) {
        printf("$ ");
        fflush(stdout);

        char input[MAX_INPUT_LENGTH];
        if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) {
            printf("\n"); // Handle Ctrl+D (EOF)
            break;
        }

        // Remove trailing newline
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        char *command = strtok(input, " \n\t");
        while (command != NULL) {
            int cmd_int = is_command(command);
            switch (cmd_int) {
                case 0: // echo
                    fprintf(stdout, "%s\n", strtok(NULL, "\n"));
                    break;
                case 1: // exit
                    exit(atoi(strtok(NULL, " ")));
                    break;
                case 2: // type
                    command = strtok(NULL, "\n");
                    if (command != NULL) {
                        int is_builtin = is_command(command);
                        if (is_builtin >= 0) {
                            fprintf(stdout, "%s is a shell builtin\n", command);
                        } else {
                            char *path = strdup(getenv("PATH"));
                            bool found = false;
                            if (path != NULL) {
                                char *dir = strtok(path, ":");
                                while (dir != NULL) {
                                    char full_path[MAX_COMMAND_LENGTH];
                                    snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);
                                    if (access(full_path, X_OK) == 0) {
                                        printf("%s is %s\n", command, full_path);
                                        found = true;
                                        break; // Found it, so stop searching
                                    }
                                    dir = strtok(NULL, ":");
                                }
                                free(path);
                            }
                            if (!found) {
                                fprintf(stdout, "%s: not found\n", command);
                            }
                        }
                    }
                    command = NULL; // Ensure the inner loop terminates after 'type'
                    break;
                default:
                    fprintf(stdout, "%s: command not found\n", command);
                    break;
            }
            if (cmd_int != 2) {
                command = strtok(NULL, " \n\t");
            } else {
                // If it was 'type', don't try to get the next token
                command = NULL;
            }
        }
    }
    return 0;
}
