#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char* argv[]) {
	int fd;

	if(argc != 4) {
		fprintf(stderr, "Usage : %s destination offset origin\n", argv[0]);
		exit(1);
	}

	if((fd = open(argv[1], O_WRONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	if(lseek(fd, (off_t)atoi(argv[2]), SEEK_SET) < 0) {
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	if(write(fd, argv[3], strlen(argv[3])) < 0) {
		fprintf(stderr, "write error\n");
		exit(1);
	}

	exit(0);
}


	

