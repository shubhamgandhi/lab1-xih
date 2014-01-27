// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"
#include <error.h>
#include "checked_sys_calls.h"
#include <sys/stat.h>
#include <fcntl.h>

void
execute_command_normal(command_t cmd)
{
	switch (cmd->type) {
		case AND_COMMAND: {
			pid_t left_child_pid = checked_fork();
			if(left_child_pid == 0) {
				execute_command_normal(cmd->u.commands[0]);
			}
			else {
				checked_waitpid(left_child_pid, &cmd->u.commands[0]->status, 0);
				if(cmd->u.commands[0]->status == 0) {
					execute_command_normal(cmd->u.commands[1]);
					cmd->status = cmd->u.commands[1]->status;
				}
				else{
					cmd->status = cmd->u.commands[0]->status;
				}
			}
			return;
		}
            
		case SEQUENCE_COMMAND: {
			pid_t left_child_pid = checked_fork();
			if(left_child_pid == 0) {
				execute_command_normal(cmd->u.commands[0]);
			}
			
			else {
				checked_waitpid(left_child_pid, &cmd->u.commands[0]->status, 0);
				execute_command_normal(cmd->u.commands[1]);
			}

			return;
		}
            
		case OR_COMMAND: {
			case AND_COMMAND: {
			pid_t left_child_pid = checked_fork();
			if(left_child_pid == 0) {
				execute_command_normal(cmd->u.commands[0]);
			}
			else {
				checked_waitpid(left_child_pid, &cmd->u.commands[0]->status, 0);
				if(cmd->u.commands[0]->status != 0) {
					execute_command_normal(cmd->u.commands[1]);
					cmd->status = cmd->u.commands[1]->status;
				}
				else{
					cmd->status = cmd->u.commands[0]->status;
				}
			}
			return;
		}
            
		case PIPE_COMMAND: {
		}
            
		case SIMPLE_COMMAND: {
			// Simple command is the base case of the recursive call.
			
			int child_pid = checked_fork();
			if (child_pid == 0) {
                
                // Set input and output if availble.
                if (cmd->input) {
                    int input_fd = checked_open2(cmd->input, O_RDONLY);
                    checked_dup2(input_fd, 0);
                    checked_close(input_fd);
                }
                
                if (cmd->output) {
                    int output_fd = checked_open3(cmd->output, O_WRONLY|O_CREAT,
                                                  0666);
                    checked_dup2(output_fd, 1);
                    checked_close(output_fd);
                }
                
				checked_execvp(cmd->u.word[0], cmd->u.word); // Execute the command.
                
			} else {
				int child_retval; // Record the returned value of the child.
				checked_waitpid(child_pid, &child_retval, 0);
				cmd->status = child_retval; // Set the return value.
			}
		}
            
		case SUBSHELL_COMMAND: {
		}
	}
}


/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

int
command_status (command_t c)
{
  return c->status;
}

void
execute_command (command_t c, int time_travel)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
    if(time_travel) {
    
    } else {
        execute_command_normal(c);
    }
}
