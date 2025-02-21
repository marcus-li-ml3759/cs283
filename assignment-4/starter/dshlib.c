#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
Built_In_Cmds match_command(const char *input)
{
	if (!input) return BI_NOT_BI;
	if (strcmp(input, EXIT_CMD) == 0) {
		return BI_CMD_EXIT;
	} else if (strcmp(input, "cd") == 0) {
		return BI_CMD_CD;
	}

	return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd)
{
	if (!cmd || cmd->argc < 1) {
		return BI_NOT_BI;
	}

	Built_In_Cmds bic = match_command(cmd->argv[0]);
	switch (bic)
	{
		case BI_CMD_EXIT:
			exit(0);

		case BI_CMD_CD:
		{
			if (cmd->argc == 1) {

			}

			else if (cmd->argc == 2) {
				if (chdir(cmd->argv[1]) != 0) {
					perror("cd");
				}
			}
			return BI_EXECUTED;
		}
		default:
			return BI_NOT_BI;
	}
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
	if (!cmd_line || !cmd_buff) {
		return ERR_MEMORY;
	}
	cmd_buff->argc = 0;

	cmd_buff->_cmd_buffer = malloc(strlen(cmd_line) + 1);
	if (!cmd_buff->_cmd_buffer) {
		return ERR_MEMORY;
	}
	strcpy(cmd_buff->_cmd_buffer, cmd_line);

	char *p = cmd_buff->_cmd_buffer;
	while (*p) {
		while (*p && isspace((unsigned char)*p))
			p++;
		if (!*p)
			break;

		char *token_start;
		if (*p == '"') {
			p++;
			token_start = p;
			while (*p && *p != '"')
				p++;
			if (*p == '"') {
				*p = '\0';
				p++;
			}
		} else {
			token_start = p;
			while (*p && !isspace((unsigned char)*p))
				p++;
			if (*p) {
				*p = '\0';
				p++;
			}
		}
		cmd_buff->argv[cmd_buff->argc++] = token_start;
		if (cmd_buff->argc >= CMD_ARGV_MAX - 1)
			break;
	}
	cmd_buff->argv[cmd_buff->argc] = NULL;
	if (cmd_buff->argc == 0) {
		return WARN_NO_CMDS;
	}

	return OK;
}
int exec_cmd(cmd_buff_t *cmd)
{
	if (!cmd || cmd->argc < 1) {
		return WARN_NO_CMDS;
	}

	pid_t pid = fork();
	if (pid < 0) {
		perror("fork");
		return ERR_EXEC_CMD;
	}

	if (pid == 0) {
		if (execvp (cmd->argv[0], cmd->argv) == -1) {
			perror("execvp");
			_exit(EXIT_FAILURE);
		}
	}

	int status;
	if (waitpid(pid, &status, 0) < 0) {
		perror("waitpid");
		return ERR_EXEC_CMD;
	}

	if (WIFEXITED(status)) {
		return WEXITSTATUS(status);
	} else {
		return ERR_EXEC_CMD;
	}
}




int exec_local_cmd_loop()
{
	char input_line[SH_CMD_MAX];
	cmd_buff_t cmd;
	memset(&cmd, 0, sizeof(cmd));

	bool firstIteration = true;

	while (true)
	{
		if (!firstIteration) {
			printf("%s", SH_PROMPT);
			fflush(stdout);
		}
		firstIteration = false;

		if (fgets(input_line, sizeof(input_line), stdin) == NULL) {
			printf("\n");
			break;
		}

		input_line[strcspn(input_line, "\n")] = '\0';

		if (cmd._cmd_buffer) {
			free(cmd._cmd_buffer);
			cmd._cmd_buffer = NULL;
		}

		cmd.argc = 0;
		memset(cmd.argv, 0, sizeof(cmd.argv));

		int rc = build_cmd_buff(input_line, &cmd);

		if (rc == WARN_NO_CMDS) {
			continue;
		} else if (rc < 0) {
			fprintf(stderr, "Error parsing command: %d\n", rc);
			continue;
		}

		Built_In_Cmds bic = exec_built_in_cmd(&cmd);
		if (bic == BI_EXECUTED) {
			printf("%s", SH_PROMPT);
			fflush(stdout);
			continue;
		}
		else if (bic == BI_CMD_EXIT) {
			break;
		}
		else {
			exec_cmd(&cmd);
			printf("%s", SH_PROMPT);
			fflush(stdout);
		}
	}
	if (cmd._cmd_buffer) {
		free(cmd._cmd_buffer);
	}
	return 0;
}

