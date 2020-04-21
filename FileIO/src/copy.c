#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define S_MODE 0644
#define BUFFER_SIZE 100

int main(int argc, char* argv[]) 
{
	char buf[BUFFER_SIZE];
	int fd1, fd2;
	int length;

	// 인자 개수가 부족할 경우
	if(argc != 3) {
		fprintf(stderr, "Usage : %s <filein> <fileout>\n", argv[0]);
		exit(1);
	}

	// 파일이 정상적으로 존재하지 않을 경우
	if((fd1 = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	// 정상적으로 복사본 파일이 생성되지 않을 경우
	if((fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_MODE)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[2]);
		exit(1);
	}

	while((length = read(fd1, buf, BUFFER_SIZE)) > 0)
		write(fd2, buf, length);

	exit(0);
}
