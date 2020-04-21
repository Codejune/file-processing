#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000
#define SUFFLE_NUM	10000
void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);
void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
off_t size;
int main(int argc, char *argv[]) {
	char buf[100];
	int fd;
	int i = 0;
	struct timeval begin_t, end_t;
	int *read_order_list;
	int num_of_records;

	if((fd = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}
	
	size = lseek(fd, 0, SEEK_END);
	num_of_records = size / 100;
	read_order_list = (int*)calloc(size, sizeof(int));

	GenRecordSequence(read_order_list, num_of_records);

	if(lseek(fd, 0, SEEK_SET) < 0) {
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	gettimeofday(&begin_t, NULL);
	for(i = 0; i < num_of_records; i++) {
		lseek(fd, read_order_list[i] * 100, SEEK_SET);
		if(read(fd, buf, 100) == 0)
			break;
	}
	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);
	close(fd);
	exit(0);
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for(i=0; i<n; i++)
	{
		list[i] = i;
	}
	
	for(i=0; i<SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t)
{
	end_t->tv_sec -= begin_t->tv_sec;

	if(end_t->tv_usec < begin_t->tv_usec){
		end_t->tv_sec--;
		end_t->tv_usec += SECOND_TO_MICRO;
	}

	end_t->tv_usec -= begin_t->tv_usec;
	printf("records: %d timecost: %ldus\n", size/100, end_t->tv_usec);
}
