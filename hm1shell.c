#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>  // Add this line for O_RDONLY
#include <unistd.h> // Add this line for open


#define MAX_LINE_LENGTH 1024
#define MAX_PARAMS 64
#define MAX_BG_PROCESSES 4

typedef struct {
    pid_t pid;
    char command[MAX_LINE_LENGTH];
} bg_process;

bg_process bg_processes[MAX_BG_PROCESSES];
int bg_process_count = 0;

void print_prompt() {
    printf("hw1shell$ ");
}

void errors_handler(const char* syscall_name) {
    printf("hw1shell: %s failed, errno is %d\n", syscall_name, errno);
}

void exit_handler() {
    // Iterate on the current processes,
    // wait for them to die and exit
    for (int i = 0; i < bg_process_count; i++) {
        waitpid(bg_processes[i].pid, NULL, 0);
    }
    exit(0);
}

void cd_handler(char *dir) {
    if (chdir(dir) != 0) {
        printf("hw1shell: invalid command\n");
        errors_handler("chdir");
    }
}

void jobs_handler() {
    // TODO: Test function
    // Printing background processes
    for (int i = 0; i < bg_process_count; i++) {
        printf("%d\t%s\n", bg_processes[i].pid, bg_processes[i].command);
    }
}

void command_handler(char **params, int background) {
    // TODO: Test fucntion for end cases
    if (params[0] == NULL) {
        printf("hw1shell: invalid command\n");
        return;
    }

    if (background && bg_process_count == MAX_BG_PROCESSES) {
        printf("hw1shell: too many background commands running\n");
        return;
    }

    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        // Execute command with parameters
        execvp(params[0], params);
        printf("hw1shell:   invalid command\n");
        errors_handler("child");
        exit(0);
    } else if (pid > 0) {
        // father process
        if (background) {
            bg_processes[bg_process_count].pid = pid;
            strcpy(bg_processes[bg_process_count].command, *params);
            bg_process_count++;
            printf("hw1shell: pid %d started\n", pid);
        } else {
            waitpid(pid, NULL, 0);
        }
    }
}

void reap_zombies() {
    // TODO: Test function
    for (int i = 0; i < bg_process_count; i++) {
        // Wait for a child matching PID to die
        if (waitpid(bg_processes[i].pid, NULL, WNOHANG) > 0) {
            // Declare finished process
            printf("hw1shell: pid %d finished\n", bg_processes[i].pid);
            // Remove the finished background process from array
            for (int j = i; j < bg_process_count - 1; j++) {
                // move bg processes to the left
                // in the bg processes array
                bg_processes[j] = bg_processes[j + 1];
            }
            bg_process_count--;
        }
    }
}

void get_command(char* line, char** params, int* param_count) {
    // Get a str from the user and return it splited to arguments in params[]
    
    if (*param_count > MAX_PARAMS) {
        printf("hw1shell: too many parameters\n");
        // Add any additional handling or return an error code if needed
        errors_handler("get_command");
    }
    
    fgets(line, MAX_LINE_LENGTH, stdin);
    line[strcspn(line, "\n")] = '\0'; // replace last chr \n with \0

    char *token = strtok(line, " ");
    while (token != NULL) {
        params[*param_count] = token;
        *param_count+=1;
        token = strtok(NULL, " ");
    }
    params[*param_count] = NULL;

}

int main() {
    char line[MAX_LINE_LENGTH];
    char *params[MAX_PARAMS];

    while (1) {
        print_prompt();
        
        int param_count = 0;

        get_command(line, params, &param_count);

        if (param_count == 0) { // No command from the user
            continue;
        }
        // Check if command is background
        int background;
        background = strcmp(params[param_count - 1], "&") ? 0 : 1;
        if (background) {
            // remove "&" from command
            params[param_count - 1] = NULL;
        }

        if (strcmp(params[0], "exit") == 0) {
            exit_handler();
        } else if (strcmp(params[0], "cd") == 0) {
            cd_handler(params[1]);
        } else if (strcmp(params[0], "jobs") == 0) {
            jobs_handler();
        } else {
            command_handler(params, background);
        }

        reap_zombies();
    }

    return 0;
}
