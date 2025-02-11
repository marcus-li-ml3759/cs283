#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"
static void trim_spaces(char *s)
{
   char *start = s;
   char *end;

   while (isspace((unsigned char)*start)) {
      start++;
   }
   if (*start == 0) {
      s[0] = '\0';
      return;
   }
   end = start + strlen(start) - 1;
   while (end > start && isspace((unsigned char)*end)) {
      end--;
   }
   *(end + 1) = '\0';

   if (start != s) {
      memmove(s, start, (end + 2 - start));
	}
}
/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
	trim_spaces(cmd_line);

	if (strlen(cmd_line) == 0) {
		return WARN_NO_CMDS;
	}

	char *saveptr;
	char *subcmd = strtok_r(cmd_line, PIPE_STRING, &saveptr);

	while (subcmd != NULL) {
		if (clist->num == CMD_MAX) {
			return ERR_TOO_MANY_COMMANDS;
		}

		trim_spaces(subcmd);

		if (strlen(subcmd) == 0) {
			clist->num++;
			subcmd = strtok_r(NULL, PIPE_STRING, &saveptr);
			continue;
		}

		command_t *cmd = &clist->commands[clist->num];
		memset(cmd, 0, sizeof(*cmd));

		char *saveptr2;
		char *token = strtok_r(subcmd, " \t", &saveptr2);

		if (token == NULL) {
			clist->num++;
			subcmd = strtok_r(NULL, PIPE_STRING, &saveptr);
			continue;
		}

		if (strlen(token) >= EXE_MAX) {
			return ERR_CMD_OR_ARGS_TOO_BIG;
		}

		strncpy(cmd->exe, token, EXE_MAX - 1);

		char args_buffer[ARG_MAX];
		args_buffer[0] = '\0';

		while ((token = strtok_r(NULL, " \t", &saveptr2)) != NULL) {
			if ((strlen(args_buffer) + strlen(token) + 1 + 1) >= ARG_MAX) {
				return ERR_CMD_OR_ARGS_TOO_BIG;
			}
			if (args_buffer[0] != '\0') {
				strcat(args_buffer, " ");
			}
			strcat(args_buffer, token);
		}

		if (strlen(args_buffer) > 0) {
			strncpy(cmd->args, args_buffer, ARG_MAX - 1);
		}

		clist->num++;

		subcmd = strtok_r(NULL, PIPE_STRING, &saveptr);
	}

	if (clist->num == 0) {
		return WARN_NO_CMDS;
	}

	return OK;
}
