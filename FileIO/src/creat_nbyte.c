#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
	FILE* fp;
	int i;
	char name[100];

	if(argc != 3) {
		fprintf(stderr, "usage: %s <filename> <size>\n", argv[0]);
		exit(1);
	}
	if((fp = fopen(argv[1], "w+")) == NULL) {
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}
	for(i = 0; i < atoi(argv[2]); i++) {
		memset((char*)name, 1, 99);
		fprintf(fp, "%s\n", name);
	}	
	fclose(fp);
	exit(0);
}
