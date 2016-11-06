#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main(void)
{
	struct sched_param param;
	param.sched_priority = 0;
	if (sched_setscheduler(getpid(), 6, &param) == -1) {
		perror("sched_setscheduler() failed");
		exit(1);
	}
	return 0;

}
