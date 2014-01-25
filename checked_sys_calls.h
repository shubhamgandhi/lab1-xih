#ifndef __CHECKED_SYS_CALLS_H__
#define __CHECKED_SYS_CALLS_H__
#include <unistd.h>

// Checked system calls for Lab 1b.
pid_t checked_fork(void);
pid_t checked_waitpid(pid_t pid, int *status, int options);
int checked_pipe(int pipefd[2]);
int checked_dup2(int oldfd, int newfd);
int checked_open(const char *pathname, int flags);
int checked_close(int fd);
int checked_execvp(const char *file, char *const argv[]);

#endif