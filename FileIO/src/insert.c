#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


int main(int argc, char* argv[]) {
	int fd;
	off_t end, cur;
	char* buf;

	// 인자 갯수 확인
	if(argc != 4) {		
		fprintf(stderr, "Usage : %s <file_name> <offset> <data>\n", argv[0]);
		exit(1);
	}

	// 파일 열기
	if((fd = open(argv[1], O_RDWR)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	end = (off_t)lseek(fd, (off_t)0, SEEK_END);

	// 커서 이동
	if((cur = lseek(fd, (off_t)atoi(argv[2]), SEEK_SET)) < 0) {
		fprintf(stderr, "lseek error\n");
		exit(1);
	}
	
	buf = (char *)calloc((int)(end - cur + 1), sizeof(char));

	if(read(fd, buf, (int)(end - cur + 1)) < 0) {
		if(cur < end) { 
			fprintf(stderr, "read error for %s\n", argv[1]);
			exit(1);
		}
	}
	
	lseek(fd, cur, SEEK_SET);
	
	if(write(fd, argv[3], strlen(argv[3])) < 0) {
		fprintf(stderr, "write error for %s\n", argv[1]);
		exit(1);
	}

	write(fd, buf, strlen(buf));

	exit(0);
}


