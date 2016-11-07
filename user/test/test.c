#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
int main(void)
{
	struct sched_param param;
	pid_t child_pid;
	param.sched_priority = 0;

	struct sched_param param_child;
	param_child.sched_priority = 0;
	if (sched_setscheduler(getpid(), 6, &param) == -1) {
		perror("sched_setscheduler() failed");
		exit(1);
	}

	child_pid = fork();
	if (child_pid < 0) {
		perror("fork() failed");
		exit(1);
	} else if (child_pid == 0) {
		
		if (sched_setscheduler(getpid(),SCHED_OTHER , &param) == -1) {
			perror("sched_setscheduler() failed in child proc");
			exit(1);
		}
	
		printf("child process running SCHED_NORMAL\n");
	} else {
		int status;
		wait(&status);	
	}
	
	return 0;

}
