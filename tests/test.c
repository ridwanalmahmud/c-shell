#include "../include/shell.h"
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define RESET   "\033[0m"

int tests_run = 0;
int tests_passed = 0;
int tests_failed = 0;

typedef struct {
    const char *name;
    void (*test_func)();
} TestCase;

void run_test(TestCase test) {
    printf(BLUE "[TEST] %s" RESET "\n", test.name);
    tests_run++;
    test.test_func();
}

void assert_str_equal(const char *expected, const char *actual, const char *message) {
    if (expected == NULL && actual == NULL) {
        printf(GREEN "  [PASS] %s" RESET "\n", message);
        tests_passed++;
        return;
    }

    if (expected == NULL || actual == NULL || strcmp(expected, actual) != 0) {
        printf(RED "  [FAIL] %s" RESET "\n", message);
        printf(YELLOW "    Expected: '%s'\n    Actual:   '%s'\n" RESET, 
               expected ? expected : "NULL", 
               actual ? actual : "NULL");
        tests_failed++;
    } else {
        printf(GREEN "  [PASS] %s" RESET "\n", message);
        tests_passed++;
    }
}

void assert_int_equal(int expected, int actual, const char *message) {
    if (expected == actual) {
        printf(GREEN "  [PASS] %s" RESET "\n", message);
        tests_passed++;
    } else {
        printf(RED "  [FAIL] %s" RESET "\n", message);
        printf(YELLOW "    Expected: %d\n    Actual:   %d\n" RESET, expected, actual);
        tests_failed++;
    }
}

char *capture_output(void (*func)(char **), char *args[]) {
    int pipefd[2];
    pid_t pid;
    char *buffer = malloc(4096);

    if (!buffer) return NULL;
    if (pipe(pipefd)) {
        free(buffer);
        return NULL;
    }

    pid = fork();
    if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);

        func(args);
        exit(0);
    } else {
        close(pipefd[1]);
        ssize_t count = read(pipefd[0], buffer, 4095);
        close(pipefd[0]);
        waitpid(pid, NULL, 0);

        if (count > 0) {
            buffer[count] = '\0';
        } else {
            buffer[0] = '\0';
        }
        return buffer;
    }
}

void create_test_executable(const char *path) {
    FILE *f = fopen(path, "w");
    if (f) {
        fprintf(f, "#!/bin/sh\necho 'test executable'\n");
        fclose(f);
        chmod(path, 0755);
    }
}

// Test cases
void test_echo_basic() {
    char *args[] = {"echo", "hello", "world", NULL};
    char *output = capture_output(execute_echo, args);
    assert_str_equal("hello world\n", output, "Basic echo command");
    free(output);
}

void test_echo_empty() {
    char *args[] = {"echo", NULL};
    char *output = capture_output(execute_echo, args);
    assert_str_equal("\n", output, "Empty echo command");
    free(output);
}

void test_exit_basic() {
    char *args[] = {"exit", "42", NULL};
    pid_t pid = fork();
    if (pid == 0) {
        execute_exit(args);
        exit(0);
    } else {
        int status;
        waitpid(pid, &status, 0);
        assert_int_equal(42, WEXITSTATUS(status), "Exit with status 42");
    }
}

void test_exit_default() {
    char *args[] = {"exit", NULL};
    pid_t pid = fork();
    if (pid == 0) {
        execute_exit(args);
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        assert_int_equal(0, WEXITSTATUS(status), "Default exit status");
    }
}

void test_type_builtin() {
    char *args[] = {"type", "echo", NULL};
    char *output = capture_output(execute_type, args);
    assert_str_equal("echo is a shell builtin\n", output, "Type builtin command");
    free(output);
}

void test_type_system() {
    char temp_dir[] = "/tmp/shell_test_XXXXXX";
    mkdtemp(temp_dir);
    char test_bin[256];
    snprintf(test_bin, sizeof(test_bin), "%s/testbin", temp_dir);
    create_test_executable(test_bin);

    char *orig_path = getenv("PATH");
    char new_path[512];
    snprintf(new_path, sizeof(new_path), "%s:%s", temp_dir, orig_path ? orig_path : "");
    setenv("PATH", new_path, 1);

    char *args[] = {"type", "testbin", NULL};
    char *output = capture_output(execute_type, args);
    char expected[512];
    snprintf(expected, sizeof(expected), "testbin is %s/testbin\n", temp_dir);
    assert_str_equal(expected, output, "Type system command");

    unlink(test_bin);
    rmdir(temp_dir);
    setenv("PATH", orig_path ? orig_path : "", 1);
    free(output);
}

void test_type_not_found() {
    char *args[] = {"type", "nonexistent_command", NULL};
    char *output = capture_output(execute_type, args);
    assert_str_equal("nonexistent_command: not found\n", output, "Type not found");
    free(output);
}

void test_type_missing_arg() {
    char *args[] = {"type", NULL};
    char *output = capture_output(execute_type, args);
    assert_str_equal("type: missing argument\n", output, "Type missing argument");
    free(output);
}

// Test runner
int main() {
    printf("\n" YELLOW "=== TEST SUITE ===\n" RESET);

    TestCase tests[] = {
        {"Echo command", test_echo_basic},
        {"Empty echo", test_echo_empty},
        {"Exit with status", test_exit_basic},
        {"Default exit", test_exit_default},
        {"Type builtin", test_type_builtin},
        {"Type system command", test_type_system},
        {"Type not found", test_type_not_found},
        {"Type missing argument", test_type_missing_arg}
    };

    size_t test_count = sizeof(tests) / sizeof(tests[0]);
    for (size_t i = 0; i < test_count; i++) {
        run_test(tests[i]);
    }

    printf("\n" YELLOW "=== TEST SUMMARY ===\n" RESET);
    printf("Tests Run:   %d\n", tests_run);
    printf(GREEN "Tests Passed: %d\n" RESET, tests_passed);
    printf(RED "Tests Failed: %d\n" RESET, tests_failed);

    return tests_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
