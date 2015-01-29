/**
	@file EvtxFileValidator.cpp
	@brief EVTX�� ������ ���/�������� ��ȿ���� �����ϱ� ���� Ŭ������ ����
*/

#include "EvtxFileValidator.h"
#include "EvtxFileHeader.h"
#include "EvtxChunkHeader.h"
#include "EvtxChunkValidator.h"
#include "BinaryFile.h"
#include "crc32.h"

/**
	@brief ������ Ư�� ��ġ�� ��ȿ�� evtx�������� Ȯ���Ѵ�

	@param inDumpFile ����
	@param filePos Ȯ���� ��ġ

	@return ��ȿ�� ����
*/
bool EvtxFileValidator::isValidEvtxFile(BinaryFile& inDumpFile, uint64_t filePos)
{
	uint8_t evtxFileHeaderBuffer[EVTX_FILE_HEADER_SIZE] = {0,};
	uint8_t evtxChunkDataBuffer[EVTX_CHUNK_SIZE] = {0,};

	EVTX_FILE_HEADER* evtxFileHeader = (EVTX_FILE_HEADER*)evtxFileHeaderBuffer;
	EVTX_CHUNK_HEADER* evtxChunkHeader = (EVTX_CHUNK_HEADER*)evtxChunkDataBuffer;

	DWORD chunkNum = 0;
	uint64_t prevChunkLastRecord = 0;

	if (inDumpFile.readData(filePos, evtxFileHeaderBuffer, EVTX_FILE_HEADER_SIZE) != EVTX_FILE_HEADER_SIZE)
	{
		return false;
	}

	if (evtxFileHeader->magic != EVTX_FILE_HEADER_MAGIC) { return false; }
	if (update_crc32(0, evtxFileHeaderBuffer, 0x78) != evtxFileHeader->checkSum) { return false; }

	for (chunkNum = 0; chunkNum < evtxFileHeader->chunkCount; ++chunkNum)
	{
		uint64_t chunkPos = filePos + EVTX_FILE_HEADER_SIZE + EVTX_CHUNK_SIZE * chunkNum;

		if (inDumpFile.readData(chunkPos, evtxChunkDataBuffer, EVTX_CHUNK_SIZE) != EVTX_CHUNK_SIZE)
		{
			break;
		}

		if (!EvtxChunkValidator::isValidChunk(evtxChunkDataBuffer))
		{
			break;
		}

		if (prevChunkLastRecord != 0 && prevChunkLastRecord + 1 != evtxChunkHeader->numLogRecFirst)
		{
			break;
		}

		prevChunkLastRecord = evtxChunkHeader->numLogRecLast;
	}

	return (chunkNum == evtxFileHeader->chunkCount);
}