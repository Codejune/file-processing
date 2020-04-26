// 주의 사항
// 1. 이 파일에 새로운 상수 변수를 추가 하지 마시오.
// 2. 이 파일에 정의 되어 있는 상수 변수의 이름을 변경하지 마시오.
// 3. 이 파일에서 수정할 수 있는 것은 BLOCKS_PER_DEVICE의
//    상수값 뿐이며, 다른 것은 어떤 것도 수정하지 마시오.
// 4. 새로운 구조체 타입을 정의하지 마시오.
// 1, 2, 3, 4를 지키지 않으면 채점 시 컴파일이 제대로 안됩니다.
// 나중에 이 파일은 제출할 필요가 없습니다.
// 혹시 이 파일에 버그가 있으면 바로 알려 주세요.

#ifndef	_SECTOR_MAPPING_H_
#define	_SECTOR_MAPPING_H_

#define TRUE			1
#define FALSE			0

#define SECTOR_SIZE		512			
#define SPARE_SIZE		16			
#define PAGE_SIZE			(SECTOR_SIZE+SPARE_SIZE)
#define SECTORS_PER_PAGE	1
#define PAGES_PER_BLOCK		4
#define BLOCK_SIZE		(PAGE_SIZE*PAGES_PER_BLOCK)
#define BLOCKS_PER_DEVICE	16 // 상수값 수정 가능
#define DATABLKS_PER_DEVICE	(BLOCKS_PER_DEVICE - 1)	// 쓰기연산 시 flash memory가 full인 경우를 대비하기 위해
							// 하나의  free block가 필요하며, 따라서 file system이 사용할
							// 수 있는 가용 메모리는 전체 블록보다 하나 작다.
#define DATAPAGES_PER_DEVICE	(DATABLKS_PER_DEVICE*PAGES_PER_BLOCK)	
							// flash memory에 데이터를 저장할 수 있는 실제 페이지의 수

//
// flash memory의 spare area를 다루기 위한 구조체
//
typedef struct
{
	int lpn;				// page에 데이터를 저장할 때 spare area에 lpn도 같이 저장함
	int is_invalid;			// page가 유효한지 그렇지 않은지 판별할 때 사용
	char dummy[SPARE_SIZE - 8];
} SpareData;

#endif
