#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>


int main(int argc, char* argv[]) {
	int fd;
	off_t end, cur;
	int length;
	int count = 0;
	char* buf;
	int stat = 0;

	// 인자 갯수 확인
	if(argc != 4) {		
		fprintf(stderr, "Usage : %s <file_name> <offset> <byte>\n", argv[0]);
		exit(1);
	}

	// 파일 열기
	if((fd = open(argv[1], O_RDWR)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	end = (off_t)lseek(fd, (off_t)0, SEEK_END);

	// 커서 이동
	if((cur = lseek(fd, (off_t)(atoi(argv[2]) + atoi(argv[3])), SEEK_SET)) < 0) {
		fprintf(stderr, "lseek error\n");
		exit(1);
	}
	
	buf = (char *)calloc((int)(end - cur + 1), sizeof(char));

	if(read(fd, buf, (int)(end - cur + 1)) < 0) {
		if(cur < end) {
			fprintf(stderr, "read error\n");
			exit(1);
		} else stat = 1; 
	}
	
	lseek(fd, (off_t)atoi(argv[2]), SEEK_SET);
	if(stat == 0)
		write(fd, buf, strlen(buf));
	free(buf);
	buf = (char *)calloc(length, sizeof(char));
	length = (int)lseek(fd, 0, SEEK_CUR);
	lseek(fd, (off_t)0, SEEK_SET);
	if(stat == 0)
		read(fd, buf, length);
	else read(fd, buf, atoi(argv[2]));
	fd = open(argv[1], O_WRONLY | O_TRUNC);
	lseek(fd, (off_t)0, SEEK_SET);
	write(fd, buf, length);
	exit(0);
}


