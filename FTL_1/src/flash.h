// 주의 사항
// 1. 이 파일에 새로운 상수 변수를 추가 하지 마시오.
// 2. 이 파일에 정의 되어있는 상수 변수의 이름과 값을 변경하지 마시오.
// 1과 2를 지키지 않으면 채점 시 컴파일이 제대로 안됩니다.

#ifndef	_FLASH_H_
#define	_FLASH_H_

#define	PAGE_NUM	4
#define	SECTOR_SIZE	512			
#define	SPARE_SIZE	16	
#define	PAGE_SIZE	(SECTOR_SIZE+SPARE_SIZE) // 512 * 16
#define	BLOCK_SIZE	(PAGE_SIZE*PAGE_NUM) // (512 * 16) * 4

#endif
