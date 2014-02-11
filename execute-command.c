// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"
#include <error.h>
#include <stdlib.h>
#include "checked_sys_calls.h"
#include <sys/stat.h>
#include <fcntl.h>


// Utility function that will return 1 if all tasks are finished
int tasks_finished(int** waiting_vector, int tasks_num)
{
	// loop through tasks to look for waiting tasks
	for(int i = 0; i < tasks_num; i++)
	{
		// return 0 if any waiting tasks found
		if(waiting_vector[1][i] != 0)
			return 0;
	}
	
	// if no waiting tasks found
	return 1;
}


void execute_time_travel(command_t* trees, int** dependency_graph, int** waiting_vector, int tasks_num)
{
	// Declare array of inital task PIDs, and clear it  before use
	pid_t task_PIDS[tasks_num];
	for(int i = 0; i < tasks_num; i++)
		task_PIDS[i] = 0;
	
	// Look through vector table for unexecute tasks with no dependency and execute them
	for(int i = 0; i < tasks_num; i++)
	{
		// if unexecuted task with no depndencies found, fork and execute it
		if(waiting_vector[i][1] == 0 && waiting_vector[i][2] == 0)
		{
			task_PIDS[i] = checked_fork();
			if (task_PIDS[i] == 0)
				execute_command_normal(trees[i])
		}
	}
	
	// Loop until all tasks are done
	while(!tasks_finished(waiting_vector, tasks_num))
	{
		int child_retval;
		// Wait for any child to finish
		pid_t returned_child = checked_waitpid(0, &child_retval, 0);
		
		// find PID of returned child and update the status, graph, vector
		for(int i = 0; i < tasks_num; i++)
		{
			if(returned_child == task_PIDS[i])
			{
				// Set status of executed tree to return value of command
				trees[i]->status = child_retval; 
				
				// Update waiting vector to indicate that returnd child was executed
				waiting_vector[i][2] = 1;
				
				// Update dependency graph
				for(int j = 0; j < tasks_num; i++)
				{
					// decrement dependency if row depends on executed column
					// also decrement dependency in waiting wector for same row
					if(dependency_graph[j][i] > 0)
					{
						dependency_graph[j][i]--;
						waiting_vector[j][1]--;
						
					}
				}
			}
			break;
		}
		
		// Loop through tasks again to find those ready to be executed
		for(int i = 0; i < tasks_num; i++)
		{
			// if unexecuted task with no depndencies found, fork and execute it
			if(waiting_vector[i][1] == 0 && waiting_vector[i][2] == 0)
			{
				task_PIDS[i] = checked_fork();
				if (task_PIDS[i] == 0)
					execute_command_normal(trees[i])
			}
		}
	}		
}



void
execute_command_normal(command_t cmd)
{
	switch (cmd->type) {
        case AND_COMMAND: {
        	//execute left command and check if it returns with status 0
            execute_command_normal(cmd->u.command[0]);
            if(cmd->u.command[0]->status == 0) {
            	//left command returned with status 0 so execute right command
                execute_command_normal(cmd->u.command[1]);
                //set exit status of execute function to exit status of right child
                cmd->status = cmd->u.command[1]->status;
            }
            else {
            	//left command returned non-0
            	//set exit status of execute function to exit status of left command
                cmd->status = cmd->u.command[0]->status;
            }
            return;
        }
            
        case SEQUENCE_COMMAND: {
        	//simply execute left and right child in sequence
        	//setting exit statuses as they return
            execute_command_normal(cmd->u.command[0]);
            cmd->status = cmd->u.command[0]->status;
            execute_command_normal(cmd->u.command[1]);
            cmd->status = cmd->u.command[1]->status;
            
            return;
        }
            
        case OR_COMMAND: {
        	//execute left command and check if it returns with status non-0
            execute_command_normal(cmd->u.command[0]);
            if(cmd->u.command[0]->status != 0) {
            	//left command returned with status non-0 so execute right command
                execute_command_normal(cmd->u.command[1]);
            	//Set status of command to most recently executed command
                cmd->status = cmd->u.command[1]->status;
            }
            else {
            	//left command returned 0
            	//Set status of command to most recently executed command
                cmd->status = cmd->u.command[0]->status;
            }
            return;
        }
            
		case PIPE_COMMAND: {
            // Set up the pipe.
            int pipefd[2];
            checked_pipe(pipefd);
            pid_t left_child_pid = checked_fork();
            if (left_child_pid == 0) {
                // In the process of left child.
                // Make sure file descriptors are closed before executing
                checked_close(pipefd[0]);
                checked_close(1);
                // Duplicate STDOUT to read end of pipe
                checked_dup2(pipefd[1], 1);
                execute_command_normal(cmd->u.command[0]);
                checked_close(pipefd[1]);
                exit(cmd->u.command[0]->status);
            } else {
                // In the process of the shell.
                // Wait for left child to finish
                checked_waitpid(left_child_pid, &cmd->u.command[0]->status, 0);
                // Close file descriptors before using them
                checked_close(pipefd[1]);
                checked_close(0);
                // Duplicate STDIN to write end of pipe
                checked_dup2(pipefd[0], 0);
                execute_command_normal(cmd->u.command[1]);
                checked_close(pipefd[0]);
                cmd->status = cmd->u.command[1]->status;
            }
            break;
		}
        
		case SIMPLE_COMMAND: {
			// Simple command is the base case of the recursive call.
			int child_pid = checked_fork();
			if (child_pid == 0) {
                
                // Set input and output if availble: redirection operators
                if (cmd->input) {
                    int input_fd = checked_open2(cmd->input, O_RDONLY);
                    checked_dup2(input_fd, 0);
                    checked_close(input_fd);
                }
                
                if (cmd->output) {
                    int output_fd = checked_open3(cmd->output, O_WRONLY|O_CREAT, 0666);
                    checked_dup2(output_fd, 1);
                    checked_close(output_fd);
                }
                
                // Execute command once imputs or outputs are set
			checked_execvp(cmd->u.word[0], cmd->u.word);
                
			} else {
                int child_retval; // Record the returned value of the child.
                checked_waitpid(child_pid, &child_retval, 0);	// Waiting for command to finish
		// Set status of command to return value of command
                cmd->status = child_retval; 
			}
            break;
		}
            
		case SUBSHELL_COMMAND: {
            // Subshell command
            pid_t child_pid = checked_fork();
            if (child_pid == 0) {
                // Set input and output if availble.
                if (cmd->input) {
                    int input_fd = checked_open2(cmd->input, O_RDONLY);
                    checked_dup2(input_fd, 0);
                    checked_close(input_fd);
                }
                
                if (cmd->output) {
                    int output_fd = checked_open3(cmd->output, O_WRONLY|O_CREAT, 0666);
                    checked_dup2(output_fd, 1);
                    checked_close(output_fd);
                }
                // Go into subshell by recursively executing into it
                execute_command_normal(cmd->u.subshell_command);
                // Exit with return value as the return value of subshell execution
                exit(cmd->u.subshell_command->status);
                
            } else {
            	// Wait for the subshell to finish executing
                checked_waitpid(child_pid, &cmd->u.subshell_command->status, 0);
                // Set status of command to status of executed subshell 
                cmd->status = cmd->u.subshell_command->status;
            }
            break;
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
