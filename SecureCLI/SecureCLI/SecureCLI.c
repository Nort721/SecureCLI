#include "CliCntrl.h"
#include "Net.h"

extern int running = 1;

void login_cmd(char* args) {

	char username[MAX_USERNAME_LENGTH];
	printf("username: ");
	ReadUsername(username);

	char password[MAX_PASSWORD_LENGTH];
	printf("password: ");
	ReadPassword(password);

	// Concatenate username and password in the specified format
	char credentials[MAX_USERNAME_LENGTH + MAX_PASSWORD_LENGTH + 2];
	snprintf(credentials, sizeof(credentials), "%s|%s", username, password);

	printf("\n");
	if (!AquireAndLoadDll(credentials)) {
		printf("\n[-] Invalid password.\n");
	}
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
	printf("=+= ============ =+=\n SecureCLI (LOCKED)\n=+= ============ =+=\n\n");

	// registering commands
	registered_command login_cmd_reg = { "login", login_cmd };
	registered_command exit_cmd_reg = { "exit", exit_cmd };

	registered_command commands_list[2];

	commands_list[0] = login_cmd_reg;
	commands_list[1] = exit_cmd_reg;

	char input[100];

	printf("Type 'login' to startup the system\n");
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
				printf("Type 'login' to startup the system\n");
			}
		}
	}
}

int main()
{
	StartCLI();
    return 0;
}