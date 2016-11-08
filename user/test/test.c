#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <errno.h>

#define MAX_CPUS 8
struct wrr_info {
	int num_cpus;
	int nr_running[MAX_CPUS];
	int total_weight[MAX_CPUS];
	int per_cpu_nr_running[MAX_CPUS];
	int per_cpu_cfs_nr_running[MAX_CPUS];
	int per_cpu_rt_nr_running[MAX_CPUS];
};

void check_wrr_info() {
	struct wrr_info my_wrr_info;
	int i;
	while (1) {
		syscall(244, &my_wrr_info);
		printf("Num of CPU : %d\n", my_wrr_info.num_cpus);
		printf("Task running using wrr on cpus :");
		for (i = 0; i < MAX_CPUS; ++i) {
			printf("%d, ", my_wrr_info.nr_running[i]);
		}
		printf("\n");
		printf("Total weight using wrr on cpus :");
		for (i = 0; i < MAX_CPUS; ++i) {
			printf("%d, ", my_wrr_info.total_weight[i]);
		}
		printf("\n");
		printf("Task running using cfs on cpus :");
		for (i = 0; i < MAX_CPUS; ++i) {
			printf("%d, ", my_wrr_info.per_cpu_cfs_nr_running[i]);
		}
		printf("\n");
		printf("Task running using rt on cpus :");
		for (i = 0; i < MAX_CPUS; ++i) {
			printf("%d, ", my_wrr_info.per_cpu_rt_nr_running[i]);
		}
		printf("\n");

		sleep(1);
	}
}

int main(void)
{
	
	int n = 30;
	pid_t pid[30];
	int i;

	setuid(10001);
	for (i = 0; i < n; i++) {
		pid[i] = fork();

		if (pid[i] < 0) {
			printf("%s\n", strerror(errno));
			exit(EXIT_FAILURE);
		} else if (pid[i] == 0) {
			while(1)
				;	
			exit(EXIT_SUCCESS);
		}
	}	

	
	//struct sched_param param;
	//pid_t child_pid;
	//param.sched_priority = 50;
	/*
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
	*/
	//sched_setscheduler(getpid(), SCHED_RR, &param);
	check_wrr_info();
	
	int status;
	pid_t w_pid;
	while (1) {
		w_pid = wait(&status);
		if (w_pid < 0 && errno == ECHILD)
			break;
	}
	return 0;
}
