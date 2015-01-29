/**
	@file crc32.h
	@brief CRC32�� ����ϱ� ���� �Լ��� ����
*/

#ifndef _CRC32_H_
#define _CRC32_H_

uint32_t update_crc32(uint32_t crc, const void *buf, size_t size);

#endif /* _CRC32_H_ */