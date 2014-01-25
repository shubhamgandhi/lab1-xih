#include "checked_sys_calls.h"
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

pid_t
checked_fork(void)
{
	pid_t retval = fork();
	if (retval == -1) {
		int errsv = errno; // Save the error number
		error(1, errsv, "Fork failed.");
	}
	return retval;
}

pid_t
checked_waitpid(pid_t pid, int *status, int options)
{
	pid_t retval = waitpid(pid, status, options);
	if (retval == -1) {
		int errsv = errno;
		error(1, errsv, "Waitpid failed.");
	}
	return retval;
}

int
checked_pipe(int pipefd[2])
{
	int retval = pipe(pipefd);
	if (retval == -1) {
		int errsv = errno;
		error(1, errsv, "Pipe failed.");
	}
	return retval;
}

int
checked_dup2(int oldfd, int newfd)
{
	int retval = dup2(oldfd, newfd);
	if (retval == -1) {
		int errsv = errno;
		error(1, errsv, "Dup2 failed.");
	}
	return retval;
}

int
checked_open(const char *pathname, int flags)
{
	int retval = open(pathname, flags);
	if (retval == -1) {
		int errsv = errno;
		error(1, errsv, "Open failed.");
	}
	return retval;
}

int
checked_close(int fd)
{
	int retval = close(fd);
	if (retval == -1) {
		int errsv = errno;
		error(1, errsv, "Close failed.");
	}
	return retval;
}

int
checked_execvp(const char *file, char *const argv[])
{
	int retval = execvp(file, argv);
	if (retval == -1) {
		int errsv = errno;
		error(1, errsv, "Execvp failed.");
	}
	return retval;
}
