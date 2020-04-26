#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "flash.h"
// 필요한 경우 헤더파일을 추가한다

FILE *flashfp;	// fdevicedriver.c에서 사용

int dd_erase(int pbn);
int dd_write(int ppn, char *pagebuf);
int dd_read(int ppn, char *pagebuf);
//
// 이 함수는 FTL의 역할 중 일부분을 수행하는데 물리적인 저장장치 flash memory에 Flash device driver를 이용하여 데이터를
// 읽고 쓰거나 블록을 소거하는 일을 한다 (동영상 강의를 참조).
// flash memory에 데이터를 읽고 쓰거나 소거하기 위해서 fdevicedriver.c에서 제공하는 인터페이스를
// 호출하면 된다. 이때 해당되는 인터페이스를 호출할 때 연산의 단위를 정확히 사용해야 한다.
// 읽기와 쓰기는 페이지 단위이며 소거는 블록 단위이다.
// 
int main(int argc, char *argv[])
{	
	// Buffer
	char sectorbuf[SECTOR_SIZE] = { 0 };
	char sparebuf[SPARE_SIZE] = { 0 };
	char pagebuf[PAGE_SIZE] = { 0 };
	char *blockbuf;
	
	// C option
	int blocksize;	// Block size

	// W option 
	int ppn;		// Physical page number
	int selectppn;
	char fword_sector;
	char fword_spare;

	// R option
	char c;

	// E option
	int pbn;

	// Temporary
	char *option;	// Option character
	int i;

	// flash memory 파일 생성: 위에서 선언한 flashfp를 사용하여 flash 파일을 생성한다. 그 이유는 fdevicedriver.c에서 
	//                 flashfp 파일포인터를 extern으로 선언하여 사용하기 때문이다.
	// 페이지 쓰기: pagebuf의 섹터와 스페어에 각각 입력된 데이터를 정확히 저장하고 난 후 해당 인터페이스를 호출한다
	// 페이지 읽기: pagebuf를 인자로 사용하여 해당 인터페이스를 호출하여 페이지를 읽어 온 후 여기서 섹터 데이터와
	//                  스페어 데이터를 분리해 낸다
	// memset(), memcpy() 등의 함수를 이용하면 편리하다. 물론, 다른 방법으로 해결해도 무방하다.
	
	option = argv[1];
	switch(*option) {
		case 'c': // flash memory 파일 생성 : n(블록: #blocks) * 4(페이지: 512byte[sector] + 16byte[spare])
			if(argc != 4) {
				fprintf(stderr, "Usage: %s c <flashfile> <#blocks>\n", argv[0]);
				exit(1);
			}

			if((flashfp = fopen(argv[2], "w+")) == NULL) {
					fprintf(stderr, "fopen error for %s\n", argv[2]);
					exit(1);
			}

			blocksize = atoi(argv[3]); // 블록 개수 읽기
			// 메모리 초기화
			for(i = 0 ; i < blocksize; i++) 
				dd_erase(i);
			fclose(flashfp);
			break;

		case 'w': // 페이지 쓰기
			if(argc != 6) {
				fprintf(stderr, "Usage: %s w <flashfile> <ppn> <sectordata> <sparedata>\n", argv[0]);
				exit(1);
			}

			if(strlen(argv[4]) > SECTOR_SIZE || strlen(argv[5]) > SPARE_SIZE){
				fprintf(stderr, "size of data error\n");
				exit(1);
			}

			if((flashfp = fopen(argv[2], "r+")) == NULL) {
					fprintf(stderr, "fopen error for %s\n", argv[2]);
					exit(1);
			}
			
			ppn = atoi(argv[3]);

			fseek(flashfp, 0, SEEK_END);
			blocksize = ftell(flashfp) / BLOCK_SIZE; // 총 블록 개수

			if(blocksize * PAGE_NUM <= ppn){ // out of bound page
				fprintf(stderr, "page number is wrong!\n");
				exit(1);
			}
			
			memcpy(sectorbuf, argv[4], strlen(argv[4]));
			memcpy(sparebuf, argv[5], strlen(argv[5]));

			fseek(flashfp, ppn * PAGE_SIZE , SEEK_SET); // 해당 페이지 데이터 체크
			fword_sector = fgetc(flashfp);
			fseek(flashfp, SECTOR_SIZE - 1, SEEK_CUR);
			fword_spare = fgetc(flashfp);

			if(fword_sector == (char)0xFF && fword_spare == (char)0xFF){ // 페이지가 비어있는 경우
				memset(pagebuf, (char)0xFF, PAGE_SIZE);
				printf("%s\n", pagebuf);
				memcpy(pagebuf, sectorbuf, strlen(sectorbuf));
				printf("%s\n", pagebuf);
				memcpy(pagebuf + SECTOR_SIZE, sparebuf, strlen(sparebuf));
				printf("%s\n", pagebuf);
				dd_write(ppn, pagebuf);
			} else {
				while(1){ // 빈 블록 탐색
					pbn = rand() % blocksize;
					fseek(flashfp, pbn * BLOCK_SIZE, SEEK_SET);

					for(i = 0; i < PAGE_NUM; i++){
						if(fgetc(flashfp) != 0xFF)
							break;
						fseek(flashfp, SECTOR_SIZE - 1, SEEK_CUR);
						if(fgetc(flashfp) != 0xFF)
							break;
						fseek(flashfp, SPARE_SIZE - 1, SEEK_CUR);
					}

					if(i == pbn)
						break;
				}

				for(i = 1; i < PAGE_NUM; i++){ // 빈 블록으로 데이터 복사
					if(++ppn % PAGE_NUM == 0)
						ppn = ppn - PAGE_NUM;
					dd_read(ppn, pagebuf);
					selectppn = pbn * PAGE_NUM + (ppn % PAGE_NUM);
					dd_write(selectppn, pagebuf);
				}

				dd_erase(ppn / PAGE_NUM); // 덮어 쓸 블록 삭제 

				for(i = 0; i < PAGE_NUM; i++){ // 덮어 쓸 블록으로 복사해둔 데이터 이전
					++selectppn;
					++ppn;
					if(selectppn % PAGE_NUM == 0 && ppn % PAGE_NUM == 0){
						selectppn = selectppn - PAGE_NUM;
						ppn = ppn - PAGE_NUM;
					}
					dd_read(selectppn, pagebuf);
					dd_write(ppn, pagebuf);
				}

				if(++ppn % PAGE_NUM == 0) // 페이지 다운
					ppn -= PAGE_NUM;

				dd_erase(pbn); // free block 삭제 

				memset(pagebuf, (char)0xFF, PAGE_SIZE);
				memcpy(pagebuf, sectorbuf, strlen(sectorbuf));
				memcpy(pagebuf + SECTOR_SIZE, sparebuf, strlen(sparebuf));

				dd_write(ppn, pagebuf);
			}
			fclose(flashfp);
			break;

		case 'r': // 페이지 읽기
			if(argc != 4) {
				fprintf(stderr, "Usage : %s r <flashfile> <ppn>\n", argv[0]);
				exit(1);
			}

			if((flashfp = fopen(argv[2], "r+")) == NULL) {
				fprintf(stderr, "fopen error for %s\n", argv[2]);
				exit(1);
			}
			ppn = atoi(argv[3]);
			dd_read(ppn, pagebuf);

			for(i = 0; i < SECTOR_SIZE; i++) {
				c = pagebuf[i];
				if(c == (char)0xFF)
					break;
				printf("%c", c);
			}

			printf(" ");

			for(i = SECTOR_SIZE; i < PAGE_SIZE; i++) {
				c = pagebuf[i];
				if(c == (char)0xFF)
					break;
				printf("%c", c);
			}
			printf("\n");
			fclose(flashfp);
			break;

		case 'e':
			if(argc != 4) {
				fprintf(stderr, "Usage : %s e <flashfile> <pbn>\n", argv[0]);
				exit(1);
			}
			
			if((flashfp = fopen(argv[2], "r+")) == NULL) {
				fprintf(stderr, "fopen error for %s\n", argv[2]);
				exit(1);
			}

			pbn = atoi(argv[3]);
			dd_erase(pbn);
			break;
	}
	exit(0);
}
