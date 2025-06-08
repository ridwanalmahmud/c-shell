#include "../include/shell.h"

int main() {
    char input[MAX_INPUT_LENGTH];
    char *tokens[MAX_ARGS];

    setbuf(stdout, NULL); // Flush after every printf

    while (true) {
        printf("$ ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break; // Handle Ctrl+D (EOF)
        }

        // Skip empty lines
        char *processed_input = trim_leading_whitespace(input);
        if (*processed_input == '\0') {
            continue;
        }

        int token_count = tokenize_input(processed_input, tokens);
        if (token_count == 0) continue;

        CommandId cmd_id = identify_command(tokens[0]);

        switch (cmd_id) {
            case CMD_ECHO:
                execute_echo(tokens);
                break;

            case CMD_EXIT:
                execute_exit(tokens);
                break;

            case CMD_TYPE:
                execute_type(tokens);
                break;

            case CMD_NONE:
                printf("%s: command not found\n", tokens[0]);
                break;
        }
    }

    return 0;
}
