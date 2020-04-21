#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);
int size;

int main(int argc, char *argv[]) {
	char buf[100];
	int fd;
	struct timeval begin_t, end_t;

	if((fd = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	size = lseek(fd, 0, SEEK_END) / 100;

	if(lseek(fd, 0, SEEK_SET) < 0) {
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	gettimeofday(&begin_t, NULL);
	while(1) {
		if(read(fd, buf, 100) == 0)
			break;
	}
	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);
	close(fd);
	exit(0);
}

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t)
{
	end_t->tv_sec -= begin_t->tv_sec;

	if(end_t->tv_usec < begin_t->tv_usec){
		end_t->tv_sec--;
		end_t->tv_usec += SECOND_TO_MICRO;
	}

	end_t->tv_usec -= begin_t->tv_usec;
	printf("records: %d timecost: %ldus\n", size, end_t->tv_usec);
}
