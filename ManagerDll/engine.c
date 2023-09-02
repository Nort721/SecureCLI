#include "engine.h"
#include <stdio.h>
#include <string.h>
#include "clicntrl.h"
#include <Windows.h>

extern int running = 1;

void example_cmd(char* args) {
    int index = 0;
    char* context = NULL;

    // Print each arguments separately
    char* token = strtok_s(args, " ", &context);
    while (token != NULL) {
        printf("%s\n", token);
        index++;
        token = strtok_s(NULL, " ", &context);
    }
}

void sensitive_cmd(char* args) {
    char* context = NULL;
    char* parameter1 = strtok_s(args, " ", &context);

    printf("%s\n", parameter1);
}

void admin_cmd(char* args) {
    char* context = NULL;
    char* parameter1 = strtok_s(args, " ", &context);

    printf("%s\n", parameter1);
}

void help_cmd(char* args) {
    printf("============\n- exit\n- example <>...<>...<>\n- sensitive <param1>\n- admin <param1>\n============\n");
}

void exit_cmd(char* args) {
    printf("exiting . . .\n");
    running = 0;
}

typedef struct {
    char* cmd_name;
    void (*fun_ptr)(char* args);
} registered_command;

void StartCLI() {
    printf("\n=+= ================= =+=\nAdminCLI (UNLOCKED)\n=+= ================= =+=\n\n");

    // registering commands
    registered_command example_cmd_reg = { "example", example_cmd };
    registered_command sensitive_cmd_reg = { "sensitive", sensitive_cmd };
    registered_command admin_cmd_reg = { "admin", admin_cmd };
    registered_command help_cmd_reg = { "help", help_cmd };
    registered_command exit_cmd_reg = { "exit", exit_cmd };

    registered_command commands_list[9];

    commands_list[0] = example_cmd_reg;
    commands_list[1] = sensitive_cmd_reg;
    commands_list[2] = admin_cmd_reg;
    commands_list[3] = help_cmd_reg;
    commands_list[4] = exit_cmd_reg;

    char input[100];

    printf("Type 'help' to see commands list\n");
    while (running)
    {
        printf(">> ");
        fgets(input, sizeof(input), stdin);
        int input_length = strlen(input);

        // checks if the user wrote anything minus the null byte
        if (input_length > 1)
        {
            char command_name[40];
            int cmd_str_index = 0;

            // gets the first word in a string (the command name)
            for (size_t i = 0; i < input_length; i++) {
                if (input[i] == ' ') {
                    command_name[cmd_str_index] = '\0';
                    break;
                }
                else {
                    command_name[cmd_str_index] = input[i];
                    cmd_str_index++;
                }
            }

            // check if the command name exists in the commands list
            int cmd_index = -1;
            size_t array_size = sizeof(commands_list) / sizeof(commands_list[0]);
            for (int i = 0; i < array_size; i++) {
                // checks if the input's command name exists inside the registered command entry
                if (strstr(command_name, commands_list[i].cmd_name) != NULL) {
                    cmd_index = i;
                    break;
                }
            }

            // if the name is of a registered command, execute the command's function
            if (cmd_index != -1) {
                char* args = input + strlen(command_name) + 1; // Skip the command name and the space
                commands_list[cmd_index].fun_ptr(args);
            }
            else
            {
                printf("Unknown command. Type help for help.\n");
            }
        }
    }
}
