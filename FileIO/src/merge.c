#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
	int fd1, fd2;
	int length;
	char buf[BUFFER_SIZE];
	
	// 인자 개수가 부족할 경우
	if(argc != 3) {
		fprintf(stderr, "Usage : %s <destination> <source>\n", argv[0]);
		exit(1);
	}

	// 파일이 정상적으로 존재하지 않을 경우
	if((fd1 = open(argv[1], O_RDWR)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	if((fd2 = open(argv[2], O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[2]);
		exit(1);
	}
	
	if(lseek(fd1, (off_t)0, SEEK_END) < 0) {
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	while((length = read(fd2, buf, BUFFER_SIZE)) > 0)
		write(fd1, buf, length);

	exit(0);
}
