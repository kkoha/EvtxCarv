/**
	@file EvtxChunkValidator.cpp
	@brief EVTX의 청크의 헤더/데이터의 유효성을 검증하기 위한 클래스의 구현
*/

#include "EvtxChunkHeader.h"
#include "EvtxChunkValidator.h"
#include "crc32.h"
#include <stdio.h>

/**
	@brief 해당하는 버퍼가 유효한 청크인지 확인한다

	@param arg 메모리 버퍼

	@return 유효성 여부
*/
bool EvtxChunkValidator::isValidChunk(void* arg)
{
	EVTX_CHUNK_HEADER* chunkHeader = (EVTX_CHUNK_HEADER*)arg;
	uint8_t* chunkData = (uint8_t*)arg;
	uint32_t headerCrc = 0;
	uint32_t dataCrc = 0;

	if (chunkHeader->magic != EVTX_CHUNK_HEADER_MAGIC) { return false; }

	headerCrc = update_crc32(0, chunkData, 0x78);
	headerCrc = update_crc32(headerCrc, chunkData + 0x80, 0x180);

	// 청크헤더의 crc유효성 여부 확인
	if (headerCrc != chunkHeader->headerCRC) { return false; }

	// 비어있는 청크는 유효하지 않다고 가정
	if (chunkHeader->numLogRecFirst == 1 && chunkHeader->numLogRecLast == 0xFFFFFFFFFFFFFFFFll) { return false; }

	dataCrc = update_crc32(0, chunkData + EVTX_CHUNK_HEADER_SIZE, chunkHeader->ofsRecNext - EVTX_CHUNK_HEADER_SIZE);

	// 청크 데이터의 crc유효성 여부 확인
	return dataCrc == chunkHeader->dataCRC;
}

/**
	@brief 해당하는 버퍼가 유효한 청크 헤더 인지 확인한다

	@param arg 메모리 버퍼

	@return 유효성 여부
*/
bool EvtxChunkValidator::isValidChunkHeader(void* arg)
{
	EVTX_CHUNK_HEADER* chunkHeader = (EVTX_CHUNK_HEADER*)arg;
	uint8_t* chunkData = (uint8_t*)arg;
	uint32_t headerCrc = 0;
	
	if (chunkHeader->magic != EVTX_CHUNK_HEADER_MAGIC) return false;

	headerCrc = update_crc32(0, chunkData, 0x78);
	headerCrc = update_crc32(headerCrc, chunkData + 0x80, 0x180);

	// 청크 헤더의 crc유효성 확인
	if (headerCrc != chunkHeader->headerCRC) { return false; }

	// 비어있는 청크는 유효하지 않다고 가정
	if (chunkHeader->numLogRecFirst == 1 && chunkHeader->numLogRecLast == 0xFFFFFFFFFFFFFFFFll) { return false; }

	return true;
}