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

int alloc_cmd_buff(cmd_buff_t *cmd_buff)
{
	cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
	if (cmd_buff->_cmd_buffer)
		return OK;
	return ERR_MEMORY;
}

int free_cmd_buff(cmd_buff_t *cmd_buff)
{
	if (cmd_buff->_cmd_buffer) {
		free(cmd_buff->_cmd_buffer);
		cmd_buff->_cmd_buffer = NULL;
	}
	return OK;
}

int execute_pipeline(command_list_t *clist) {
	int n = clist->num;  
	int pipe_fds[n - 1][2];
	pid_t pids[n];

	for (int i = 0; i < n - 1; i++) {
		if (pipe(pipe_fds[i]) < 0) {
			perror("pipe");
			return ERR_EXEC_CMD;
		}
	}

	for (int i = 0; i < n; i++) {
		pid_t pid = fork();
		if (pid < 0) {
			perror("fork");
			return ERR_EXEC_CMD;
		}

		pids[i] = pid;
		if (pid == 0) { 
			if (i > 0) {
				if (dup2(pipe_fds[i - 1][0], STDIN_FILENO) < 0) {
					perror("dup2 for stdin");
					exit(ERR_EXEC_CMD);
				}
			}

			if (i < n - 1) {
				if (dup2(pipe_fds[i][1], STDOUT_FILENO) < 0) {
					perror("dup2 for stdout");
					exit(ERR_EXEC_CMD);
				}
			}

			for (int j = 0; j < n - 1; j++) {
				close(pipe_fds[j][0]);
				close(pipe_fds[j][1]);
			}

			execvp(clist->commands[i].argv[0], clist->commands[i].argv);
			perror("execvp");
			exit(ERR_EXEC_CMD);
		}
	}

	for (int i = 0; i < n - 1; i++) {
		close(pipe_fds[i][0]);
		close(pipe_fds[i][1]);
	}

	for (int i = 0; i < n; i++) {
		int status;
		waitpid(pids[i], &status, 0);
	}
	return OK;
}

int exec_local_cmd_loop()
{
	char input_line[SH_CMD_MAX];
	cmd_buff_t cmd;
	memset(&cmd, 0, sizeof(cmd));
	command_list_t clist;
	bool firstIteration = true;

	while (true)
	{	
		if (isatty(STDIN_FILENO) || !firstIteration) {
			printf("%s", SH_PROMPT);
			fflush(stdout);
		}
		firstIteration = false;

		if (fgets(input_line, sizeof(input_line), stdin) == NULL) {
			printf("\n");
			break;
		}

		input_line[strcspn(input_line, "\n")] = '\0';

		clist.num = 0;
		char *eachPipedCommand = strtok(input_line, PIPE_STRING);
		while (eachPipedCommand != NULL) {
			if (clist.num >= CMD_MAX) {
				printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
				break;
			}

			if (alloc_cmd_buff(&clist.commands[clist.num]) != OK) {
				return ERR_MEMORY;
			}

			int rc = build_cmd_buff(eachPipedCommand, &clist.commands[clist.num]);
			if (rc == WARN_NO_CMDS) {
				printf(CMD_WARN_NO_CMD);
				break;
			}

			clist.num++;
			eachPipedCommand = strtok(NULL, PIPE_STRING);

		}
		if (clist.num == 0)
			continue;
		if (clist.num == 1) {
			if (exec_built_in_cmd(&clist.commands[0]) == BI_NOT_BI) {
				exec_cmd(&clist.commands[0]);
			}
		} else {
			execute_pipeline(&clist);
		}

		for (int i = 0; i < clist.num; i++) {
			free_cmd_buff(&clist.commands[i]);
		}
	}

	printf("%s", SH_PROMPT);
	fflush(stdout);
	return 0;
}
