/**
	@file crc32.h
	@brief CRC32를 계산하기 위한 함수의 정의
*/

#ifndef _CRC32_H_
#define _CRC32_H_

uint32_t update_crc32(uint32_t crc, const void *buf, size_t size);

#endif /* _CRC32_H_ */