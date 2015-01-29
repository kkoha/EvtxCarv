/**
	@file EvtxChunkValidator.cpp
	@brief EVTX�� ûũ�� ���/�������� ��ȿ���� �����ϱ� ���� Ŭ������ ����
*/

#include "EvtxChunkHeader.h"
#include "EvtxChunkValidator.h"
#include "crc32.h"
#include <stdio.h>

/**
	@brief �ش��ϴ� ���۰� ��ȿ�� ûũ���� Ȯ���Ѵ�

	@param arg �޸� ����

	@return ��ȿ�� ����
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

	// ûũ����� crc��ȿ�� ���� Ȯ��
	if (headerCrc != chunkHeader->headerCRC) { return false; }

	// ����ִ� ûũ�� ��ȿ���� �ʴٰ� ����
	if (chunkHeader->numLogRecFirst == 1 && chunkHeader->numLogRecLast == 0xFFFFFFFFFFFFFFFFll) { return false; }

	dataCrc = update_crc32(0, chunkData + EVTX_CHUNK_HEADER_SIZE, chunkHeader->ofsRecNext - EVTX_CHUNK_HEADER_SIZE);

	// ûũ �������� crc��ȿ�� ���� Ȯ��
	return dataCrc == chunkHeader->dataCRC;
}

/**
	@brief �ش��ϴ� ���۰� ��ȿ�� ûũ ��� ���� Ȯ���Ѵ�

	@param arg �޸� ����

	@return ��ȿ�� ����
*/
bool EvtxChunkValidator::isValidChunkHeader(void* arg)
{
	EVTX_CHUNK_HEADER* chunkHeader = (EVTX_CHUNK_HEADER*)arg;
	uint8_t* chunkData = (uint8_t*)arg;
	uint32_t headerCrc = 0;
	
	if (chunkHeader->magic != EVTX_CHUNK_HEADER_MAGIC) return false;

	headerCrc = update_crc32(0, chunkData, 0x78);
	headerCrc = update_crc32(headerCrc, chunkData + 0x80, 0x180);

	// ûũ ����� crc��ȿ�� Ȯ��
	if (headerCrc != chunkHeader->headerCRC) { return false; }

	// ����ִ� ûũ�� ��ȿ���� �ʴٰ� ����
	if (chunkHeader->numLogRecFirst == 1 && chunkHeader->numLogRecLast == 0xFFFFFFFFFFFFFFFFll) { return false; }

	return true;
}