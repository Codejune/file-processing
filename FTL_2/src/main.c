#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include "sectormap.h"

#define true 1
#define false 0
#define FLASH_MEMORY "flashmemory"

void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);
void ftl_print();
void init_flash_memory(int size); // flash memory 초기화
int dd_erase(int pbn);
void print_usage();

FILE *flashfp; // flash memory 파일 포인터
extern int mapping_table[DATAPAGES_PER_DEVICE];
extern SpareData spare_table[DATAPAGES_PER_DEVICE];
extern int write_count;
extern int return_signal;

int main(void)
{
	int input;
	int lsn;
	char value[SECTOR_SIZE] = { 0 };
	int i;
	char c;

	if((flashfp = fopen(FLASH_MEMORY, "w+")) == NULL) { // flash memory 준비
		fprintf(stderr, "fopen error for %s\n", FLASH_MEMORY);
		exit(1);
	}

	ftl_open();

	while(true) {

		print_usage();
		printf(">> ");
		scanf("%d", &input);

		switch(input) {
			case 1: // ftl_open()

				ftl_open();
				break;

			case 2: // ftl_read()

				printf("Logical sector number: ");
				scanf("%d", &lsn);

				ftl_read(lsn, value);
				if(return_signal) {
					// sector area 출력
					for(i = 0; i < SECTOR_SIZE; i++) {
						c = value[i];
						if(c == (char)0xFF)
							break;
						printf("%c", c);
					}

					printf(" ");

					// spare area 출력
					for(i = SECTOR_SIZE; i < PAGE_SIZE; i++) {
						c = value[i];
						if(c == (char)0xFF)
							break;
						printf("%c", c);
					}
					printf("\n");
				}
				memset(value, 0, SECTOR_SIZE);
				break;

			case 3: // ftl_write()

				printf("Logical sector number: ");
				scanf("%d", &lsn);
				printf("Value: ");
				scanf("%s", value);

				ftl_write(lsn, value);
				memset(value, 0, SECTOR_SIZE);
				break;

			case 4: // ftl_print()

				ftl_print();
				break;

			case 0:

				exit(0);
				break;

			default:

				printf("Invalid input command\n");
				break;
		}
		printf("\n");
	}

	exit(0);
}

void print_usage() // 사용법 출력
{
	printf("FTL sector mapping test program:\n");
	printf(" (1) Re-create & initialize\n");
	printf(" (2) Read\n");
	printf(" (3) Write\n");
	printf(" (4) Print mapping table\n");
	printf(" (0) Program exit\n");
}




