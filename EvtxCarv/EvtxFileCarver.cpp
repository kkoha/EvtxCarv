/**
	@file EvtxFileCarver.cpp
	@brief EVTX파일을 카빙하기 위한 클래스의 구현
*/

#include "EvtxFileCarver.h"
#include "BinaryFile.h"
#include "EvtxFileHeader.h"
#include "EvtxFileValidator.h"
#include "EvtxChunkHeader.h"
#include "DataType.h"
#include <tchar.h>
#include "string.h"
#include <stdio.h>
#include <string>
#include "SignatureFinder.h"
#include "ChunkInfoParser.h"
#include "EvtxChunkValidator.h"

EvtxFileCarver::EvtxFileCarver()
{
	;
}

/**
	@brief 카빙할 파일을 지정한다
	@param inCarveFile 카빙할 파일
*/
void EvtxFileCarver::setCarveFile(BinaryFile& inCarveFile)
{
	carveFile_ = inCarveFile;
}

/**
	@brief 카빙된 내용을 출력할 파일 이름을 정한다
	@param inDumpFilePrefix 출력 파일명의 접두사
*/
void EvtxFileCarver::setDumpFilePrefix(const std::wstring& inDumpFilePrefix)
{
	dumpFilePrefix_ = inDumpFilePrefix;
}

/**
	@brief 카빙된 내용을 출력할 폴더를 설정한다
	@param inDumpPathName 출력 폴더명
*/
void EvtxFileCarver::setDumpPathName(const std::wstring& inDumpPathName)
{
	dumpPathName_ = inDumpPathName;
}

/**
	@brief 레코드 카빙을 시작한다

	@param outCarvedArea 이후 카빙에서 스킵할 영역
*/
bool EvtxFileCarver::startCarving(CarvedAreas& outCarvedArea)
{
	uint64_t filePos = 0;
	uint64_t dumpedFileSize = 0;

	outCarvedArea.clear();

	if (carveFile_.getFileSize() < EVTX_CHUNK_SIZE)
	{
		return true;
	}

	SignatureFinder signatureFinder(carveFile_, EVTX_FILE_HEADER_MAGIC, 8, 512, NULL, EVTX_CHUNK_SIZE);

	fileIndex_ = 0;

	// 유효한 evtx 파일을 찾아서
	while (signatureFinder.getNext(&filePos) != NULL)
	{
		if (EvtxFileValidator::isValidEvtxFile(carveFile_, filePos))
		{
			// evtx파일로 바로 덤프한다
			if (dumpToEvtxFile(filePos, dumpedFileSize))
			{
				outCarvedArea.push_back(std::make_pair(filePos, filePos + dumpedFileSize));
				filePos += dumpedFileSize;
				continue;
			}
		}
	}

	signatureFinder.closeMap();

	return true;
}

/**
	@brief 특정 위치를 evtx파일로 덤프한다
	@param inFilePos 덤프할 위치
	@param outFileSize 덤프된 사이즈
*/
bool EvtxFileCarver::dumpToEvtxFile(uint64_t inFilePos, uint64_t& outFileSize)
{
	uint8_t evtxFileHeaderBuffer[EVTX_FILE_HEADER_SIZE] = {0,};
	uint8_t evtxChunkDataBuffer[EVTX_CHUNK_SIZE] = {0,};

	EVTX_FILE_HEADER* evtxFileHeader = (EVTX_FILE_HEADER*)evtxFileHeaderBuffer;
	BinaryFile dumpFile;

	DWORD chunkNum = 0;
	
	wchar_t outputFileName[MAX_PATH_SIZE] = {0,};

	outFileSize = 0;

	carveFile_.readData(inFilePos + EVTX_FILE_HEADER_SIZE, evtxChunkDataBuffer, EVTX_CHUNK_SIZE);

	ChunkInfoParser chunkInfoParser(evtxChunkDataBuffer);

	// 첫 번째 청크의 첫 번쨰 레코드에 기록되어있는 컴퓨터/채널이름 기반으로 출력 파일 이름을 정한다
	wsprintf(outputFileName, _T("%s\\%s_%s_%s%05d.evtx"), dumpPathName_.c_str(), chunkInfoParser.getComputerName().c_str(),
		chunkInfoParser.getChannelName().c_str(), dumpFilePrefix_.c_str(), fileIndex_);

	dumpFile.openForWrite(outputFileName);

	if (carveFile_.readData(inFilePos, evtxFileHeaderBuffer, EVTX_FILE_HEADER_SIZE) != EVTX_FILE_HEADER_SIZE)
	{
		return false;
	}

	outFileSize += EVTX_FILE_HEADER_SIZE;

	// evtx헤더를 덤프한다
	dumpFile.writeData(0, evtxFileHeaderBuffer, EVTX_FILE_HEADER_SIZE);

	// evtx헤더에 적혀힜는 청크의 수만큼
	for (chunkNum = 0; chunkNum < evtxFileHeader->chunkCount; ++chunkNum)
	{
		uint64_t chunkPos = inFilePos + EVTX_FILE_HEADER_SIZE + EVTX_CHUNK_SIZE * chunkNum;

		if (carveFile_.readData(chunkPos, evtxChunkDataBuffer, EVTX_CHUNK_SIZE) != EVTX_CHUNK_SIZE)
		{
			break;
		}

		if (!EvtxChunkValidator::isValidChunk(evtxChunkDataBuffer))
		{
			dumpFile.closeFile();
			::DeleteFile(outputFileName);
		}

		outFileSize += EVTX_CHUNK_SIZE;

		// 청크를 읽어서 덤프한다
		dumpFile.writeData(EVTX_FILE_HEADER_SIZE + EVTX_CHUNK_SIZE * chunkNum, evtxChunkDataBuffer, EVTX_CHUNK_SIZE);
	}

	dumpFile.closeFile();

	++fileIndex_;

	return chunkNum == evtxFileHeader->chunkCount;
}