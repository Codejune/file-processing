#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
	int fd1, fd2;
	off_t end, cur;
	int length;
	int count = 0;
	char* buf;
	char* temp_buf[BUFFER_SIZE];

	// 인자 갯수 확인
	if(argc != 4) {		
		fprintf(stderr, "Usage : %s <file_name> <offset> <data>\n", argv[0]);
		exit(1);
	}

	// 파일 열기
	if((fd1 = open(argv[1], O_RDWR)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	// 데이터 열기
	if((fd2 = open(argv[3], O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[3]);
		exit(1);
	}

	end = (off_t)lseek(fd1, (off_t)0, SEEK_END);

	// 커서 이동
	if((cur = lseek(fd1, (off_t)atoi(argv[2]), SEEK_SET)) < 0) {
		fprintf(stderr, "lseek error\n");
		exit(1);
	}
	
	buf = (char *)calloc((int)(end - cur + 1), sizeof(char));

	if(read(fd1, buf, (int)(end - cur + 1)) < 0) {
		fprintf(stderr, "read error\n");
		exit(1);
	}
	
	lseek(fd1, cur, SEEK_SET);
	
	while((length = read(fd2, temp_buf, BUFFER_SIZE)) > 0) {
		write(fd1, temp_buf, length);
		count += length;
	}
	write(fd1, buf, strlen(buf));
	exit(0);
}


