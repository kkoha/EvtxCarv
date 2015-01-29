/**
	@file EvtxFileValidator.cpp
	@brief EVTX의 파일의 헤더/데이터의 유효성을 검증하기 위한 클래스의 구현
*/

#include "EvtxFileValidator.h"
#include "EvtxFileHeader.h"
#include "EvtxChunkHeader.h"
#include "EvtxChunkValidator.h"
#include "BinaryFile.h"
#include "crc32.h"

/**
	@brief 파일의 특정 위치가 유효한 evtx파일인지 확인한다

	@param inDumpFile 파일
	@param filePos 확인할 위치

	@return 유효성 여부
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