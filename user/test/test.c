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
};

void check_wrr_info(void)
{
	struct wrr_info my_wrr_info;
	int i;
	while (1) {
		syscall(244, &my_wrr_info);
		printf("Num of CPU : %d\n", my_wrr_info.num_cpus);
		printf("Task running using wrr on cpus :");
		for (i = 0; i < MAX_CPUS; ++i)
			printf("%d\t", my_wrr_info.nr_running[i]);
		printf("\n");
		printf("Total weight using wrr on cpus :");
		for (i = 0; i < MAX_CPUS; ++i)
			printf("%d\t", my_wrr_info.total_weight[i]);
		printf("\n");

		sleep(1);
	}
}

int main(void)
{
	int n = 5000;
	pid_t pid[5000];
	int i;

	/*syscall(245, 100);*/
	setuid(10001);
	for (i = 0; i < n; i++) {
		pid[i] = fork();

		if (pid[i] < 0) {
			printf("%s\n", strerror(errno));
			exit(EXIT_FAILURE);
		} else if (pid[i] == 0) {
			while (1)
				usleep(100);
			exit(EXIT_SUCCESS);
		}
	}

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
