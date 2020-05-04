// 주의사항
// 1. sectormap.h에 정의되어 있는 상수 변수를 우선적으로 사용해야 함
// 2. sectormap.h에 정의되어 있지 않을 경우 본인이 이 파일에서 만들어서 사용하면 됨
// 3. 필요한 data structure가 필요하면 이 파일에서 정의해서 쓰기 바람(sectormap.h에 추가하면 안됨)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "sectormap.h"
// 필요한 경우 헤더 파일을 추가하시오.

#define true  1
#define false 0
#define null -1
#define FLASH_MEMORY "flashmemory"
#define PECYCLE 100000

void ftl_open();
void ftl_read(int lsn, char *sectorbuf);
int dd_read(int ppn, char *pagebuf);
void ftl_write(int lsn, char *sectorbuf);
int dd_erase(int pbn);
int dd_write(int lsn, char *pagebuf);
void ftl_print();

extern FILE *flashfp;
int mapping_table[DATAPAGES_PER_DEVICE];
SpareData spare_table[BLOCKS_PER_DEVICE * PAGES_PER_BLOCK];
int return_signal;
int write_count;
int free_block;

// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// file system에 의해 반드시 먼저 호출이 되어야 한다.
void ftl_open()
{
	int i;

	// address mapping table 초기화
	// free block's pbn 초기화
	// address mapping table에서 lbn 수는 DATABLKS_PER_DEVICE 동일

	if(flashfp == NULL) {
		fprintf(stderr, "%s doesn't opening\n", FLASH_MEMORY);
		return_signal = false;
		exit(1);
		return;
	}

	for(i = 0; i < BLOCKS_PER_DEVICE; i++) // flash memory 초기화
		dd_erase(i);


	for(i = 0; i < DATAPAGES_PER_DEVICE; i++) // table 초기화
		// init mapping table
		mapping_table[i] = null; // ppn 초기화(-1)


	for(i = 0; i < PAGES_PER_BLOCK * BLOCKS_PER_DEVICE; i++) {
		// spare data table
		spare_table[i].lpn = null; // lpn 초기화(-1)
		spare_table[i].is_invalid = true; // 유효값 초기화
	}

	write_count = false;
	free_block = DATABLKS_PER_DEVICE;
	return_signal = true;
	return;
}

// 이 함수를 호출하기 전에 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 한다.
// 즉, 이 함수에서 메모리를 할당받으면 안된다.
void ftl_read(int lsn, char *sectorbuf)
{
	int ppn;
	char pagebuf[PAGE_SIZE] = { 0 };

	if(DATAPAGES_PER_DEVICE <= lsn || lsn < 0) { // lsn(lpn) 범위 초과
		fprintf(stderr, "ftl_read: Invalid input logical sector number(0 < lsn < %d)\n", DATAPAGES_PER_DEVICE);
		return_signal = false;
		exit(1);
		return;
	}

	if((ppn = mapping_table[lsn]) == null) { // mapping_table에 쓰여진 ppn이 존재하지 않음
		fprintf(stderr, "ftl_read: Physical page number doesn't exist on mapping_table[%d]\n", lsn);
		return_signal = false;
		exit(1);
		return;
	}

	dd_read(ppn, sectorbuf);
	return_signal = true;
	return;
}


// mapping_table[idx][1](ppn) == -1 일 경우: 최초 쓰기 수행
// mapping_talbe[idx][1](ppn) != -1 일 경우: 이미 존재하는 데이터를 갱신(update)
// 두 경우 모두 비어있는 ppn중에서 하나를 선택하여 그 값으로 mapping_table의 해당 ppn을 갱신한다
// 최초 쓰기인지 덮어쓰기인지 체크하는 과정 필요
// 테이블에 존재하지 않는 경우는 가비지...
void ftl_write(int lsn, char *sectorbuf)
{
	char pagebuf[PAGE_SIZE];
	char sparebuf[SPARE_SIZE];
	int is_available;
	int is_all_invalid;
	int i, j;

	if(DATAPAGES_PER_DEVICE <= lsn || lsn < 0) { // lsn(lpn) 범위 초과
		fprintf(stderr, "ftl_write: Invalid input logical sector number(0 < lsn < %d)\n", DATAPAGES_PER_DEVICE);
		return_signal = false;
		exit(1);
		return;
	}

	is_all_invalid = true;
	for(i = 0; i < DATAPAGES_PER_DEVICE; i++) { // 빈 페이지 탐색
		if(i / PAGES_PER_BLOCK == free_block)
			continue;
		if(spare_table[i].is_invalid == true) { // 유효하지 않은 페이지 발견시
			is_all_invalid = false;
			break;
		}
	}

	// garbage collection
	if(is_all_invalid) {  // 빈 페이지가 존재하지 않는 경우(모든 페이지가 유효한 경우)
		// 해당 페이지를 제외한 나머지 페이지를 빈 블록으로 복사한 뒤 업데이트, 기존 블록 삭제
		int g_block = mapping_table[lsn] / PAGES_PER_BLOCK; // 해당 블록 인덱스
		int g_page = g_block * PAGES_PER_BLOCK; // 해당 블록의 페이지 시작 인덱스
		for(i = g_page, j = 0; i < g_page + PAGES_PER_BLOCK; i++, j++) {
			// 빈 블록 초기화
			spare_table[free_block * PAGES_PER_BLOCK + j].lpn = null;
			spare_table[free_block * PAGES_PER_BLOCK + j].is_invalid = true;

			if(i != mapping_table[lsn]) { // 탐색하는 페이지가 mapping table에 존재하는 페이지일 경우 빈 블록에 복사
				fseek(flashfp, PAGE_SIZE * i, SEEK_SET); // 해당 페이지 위치 이동
				fread(pagebuf, PAGE_SIZE, 1, flashfp); // 해당 페이지 위치 읽기
				fseek(flashfp, (BLOCK_SIZE * free_block) + (j * PAGE_SIZE), SEEK_SET); // 빈 블록에서 페이지 위치 이동 
				fwrite(pagebuf, PAGE_SIZE, 1, flashfp); // 빈 블록에 페이지 쓰기
				fflush(flashfp);
				spare_table[free_block * PAGES_PER_BLOCK + j].lpn = spare_table[i].lpn;
				spare_table[free_block * PAGES_PER_BLOCK + j].is_invalid = false;
				mapping_table[spare_table[i].lpn] = free_block * PAGES_PER_BLOCK + j;
			} 
			spare_table[i].lpn = null;
			spare_table[i].is_invalid = true;
		}
		dd_erase(g_block);
		free_block = g_block;
		write_count = 0;
	}


	// mapping table update
	for(i = 0; i < BLOCKS_PER_DEVICE * PAGES_PER_BLOCK; i++) { // 빈 페이지 탐색, i = ppn

		is_available = true; // 해당 페이지 쓰기 가능 여부

		if((i / PAGES_PER_BLOCK) == free_block)
			continue;

		for(j = 0; j < DATAPAGES_PER_DEVICE; j++) // mapping table에 존재하는 페이지인지 확인
			if(mapping_table[j] == i) { // mapping table에서 참조하는 페이지일 경우
				is_available = false; // 해당 페이지에 쓰기 불가능
				break;
			}

		if(spare_table[i].is_invalid == false) // 해당 페이지가 유효한 페이지인 경우
			continue; // 다음 페이지 탐색

		if(is_available) { // 유효하지 않은 페이지인 경우
			spare_table[i].lpn = lsn; // 해당 페이지를 참조하는 테이블 번호(lsn) 갱신
			spare_table[i].is_invalid = false; // 유효함으로 갱신
			if(mapping_table[lsn] > null) { // mapping table의 갱신이 필요한 경우
				spare_table[mapping_table[lsn]].lpn = null;
				spare_table[mapping_table[lsn]].is_invalid = true;
			}
			mapping_table[lsn] = i; // mapping table의 페이지 번호(psn) 갱신
			write_count++;
			break;
		}

	}

	memset(pagebuf, (char)0xFF, PAGE_SIZE); // 버퍼 초기화
	sprintf(sparebuf, "%d", lsn);
	memcpy(pagebuf, sectorbuf, strlen(sectorbuf)); // value를 sector area에 저장
	memcpy(pagebuf + SECTOR_SIZE, sparebuf, strlen(sparebuf)); // logical page number를 spare area에 저장

	dd_write(i, pagebuf);
	fflush(flashfp);
	return_signal = true;
	return;
}

void ftl_print()
{
	int i;

	printf("lpn ppn\n");
	for(i = 0; i < DATAPAGES_PER_DEVICE; i++)
		printf("%d %d\n", i, mapping_table[i]);
	printf("free block's pbn=%d\n", free_block);

	return;
}
