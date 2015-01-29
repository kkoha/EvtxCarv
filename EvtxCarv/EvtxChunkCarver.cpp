/**
	@file EvtxChunkCarver.cpp
	@brief EVTX청크를 카빙하기 위한 클래스의 구현
*/

#include "EvtxChunkCarver.h"
#include "EvtxChunkValidator.h"
#include "EvtxFileHeader.h"
#include "EvtxChunkHeader.h"
#include "EvtxRecordHeader.h"
#include "crc32.h"
#include "DataType.h"
#include "SignatureFinder.h"
#include "ChunkInfoParser.h"
#include "StringTransform.h"
#include <tchar.h>
#include <map>

EvtxChunkCarver::EvtxChunkCarver()
{
	;
}

/**
	@brief 카빙할 파일(원본) 을 지정한다
	@param inFile 카빙할 파일
*/
void EvtxChunkCarver::setCarveFile(BinaryFile& inFile)
{
	carveFile_ = inFile;
}

/**
	@brief 카빙할 파일(재조합)을 지정한다
	@param inFile 카빙할 파일
*/
void EvtxChunkCarver::setFragmentedFile(BinaryFile& inFile)
{
	fragmentedFile_ = inFile;
}

/**
	@brief 카빙된 내용을 출력할 파일 이름을 정한다
	@param inDumpFilePrefix 출력 파일명의 접두사
*/
void EvtxChunkCarver::setDumpFilePrefix(const std::wstring& inDumpFilePrefix)
{
	dumpFilePrefix_ = inDumpFilePrefix;
}

/**
	@brief 카빙된 내용을 출력할 폴더를 설정한다
	@param inDumpPathName 출력 폴더명
*/
void EvtxChunkCarver::setDumpPathName(const std::wstring& inDumpPathName)
{
	dumpPathName_ = inDumpPathName;
}

/**
	@brief 카빙한 내요을 담을 때 한 파일에 들어갈 청크의 최대 개수를 정한다
	@param inFragmentSize 최대 사이즈
*/
void EvtxChunkCarver::setFragmentSize(int inFragmentSize)
{
	fragmentSize_ = inFragmentSize;
}

/**
	@brief 청크 카빙을 시작한다

	@param inCarvedAreas 카빙에서 스킵할 영역
	@param inUseFragmentedFile 재조합된 파일을 사용할지 여부
	@param inWriteOutput 카빙된 결과를 파일로 내보낼지 여부
	@param outCarvedAreas 일반 파일에서 카빙된 영역
	@param outFragmentedCarvedAreas 재조합된 파일에서 카빙된 영역
*/
bool EvtxChunkCarver::startCarving(const CarvedAreas& inCarvedAreas, bool inUseFragmentedFile, bool inWriteOutput, CarvedAreas& outCarvedAreas, CarvedAreas& outFragmentedCarvedAreas)
{
	uint8_t* chunkDataBuffer = NULL;
	EVTX_CHUNK_HEADER* evtxChunkHeader = NULL;
	uint64_t filePos = 0;
	uint64_t readChunkHeader = 0;
	uint64_t readSize = 0;
	GroupedChunks groupedChunks;
	GroupedChunks::iterator groupedChunkIterator;
	SortedChunk::iterator sortedChunk;
	ChunkInfos chunkInfos;
	CarvedAreas::const_iterator carvedArea = inCarvedAreas.begin();
	SignatureFinder signatureFinder(carveFile_, EVTX_CHUNK_HEADER_MAGIC, 8, 16, &inCarvedAreas, EVTX_CHUNK_SIZE);

	fileIndex_ = 0;
	outCarvedAreas.clear();
	outFragmentedCarvedAreas.clear();

	if (carveFile_.getFileSize() < EVTX_CHUNK_SIZE) 
	{
		return true;
	}

	// 일반 파일에서 유효한 청크들을 찾아서
	while ((chunkDataBuffer = signatureFinder.getNext(&filePos)) != NULL)
	{
		if (EvtxChunkValidator::isValidChunk(chunkDataBuffer))
		{
			evtxChunkHeader = (EVTX_CHUNK_HEADER*)chunkDataBuffer;

			signatureFinder.addCarvedArea(std::make_pair(filePos, filePos + evtxChunkHeader->ofsRecNext));

			if (inWriteOutput)
			{
				ChunkInfoParser chunkInfoParser(chunkDataBuffer);

				// 컴퓨터/채널이름 기반으로 그루핑한다
				insertChunkIntoGroup(groupedChunks,
					ChunkInfo(FILE_TYPE_NORMAL, filePos, evtxChunkHeader->numLogRecFirst, evtxChunkHeader->numLogRecLast, 
						StringTransform::toLowerCase(chunkInfoParser.getComputerName()), StringTransform::toLowerCase(chunkInfoParser.getChannelName()))
				);
			}
		}
	}

	outCarvedAreas = *(signatureFinder.getCarvedArea());
	signatureFinder.closeMap();

	if (inUseFragmentedFile)
	{
		SignatureFinder signatureFinderFragmented(fragmentedFile_, EVTX_CHUNK_HEADER_MAGIC, 8, 16, NULL, EVTX_CHUNK_SIZE);

		// 재조립된 파일에서 유효한 청크들을 찾아서
		while ((chunkDataBuffer = signatureFinderFragmented.getNext(&filePos)) != NULL)
		{
			if (EvtxChunkValidator::isValidChunk(chunkDataBuffer))
			{
				evtxChunkHeader = (EVTX_CHUNK_HEADER*)chunkDataBuffer;

				signatureFinderFragmented.addCarvedArea(std::make_pair(filePos, filePos + evtxChunkHeader->ofsRecNext));

				if (inWriteOutput)
				{
					ChunkInfoParser chunkInfoParser(chunkDataBuffer);

					// 컴퓨터/채널이름 기반으로 그루핑한다
					insertChunkIntoGroup(groupedChunks,
					ChunkInfo(FILE_TYPE_FRAGMENTED, filePos, evtxChunkHeader->numLogRecFirst, evtxChunkHeader->numLogRecLast, 
						StringTransform::toLowerCase(chunkInfoParser.getComputerName()), StringTransform::toLowerCase(chunkInfoParser.getChannelName()))
					);
				}
			}
		}

		outFragmentedCarvedAreas = *(signatureFinderFragmented.getCarvedArea());
		signatureFinderFragmented.closeMap();
	}

	if (inWriteOutput)
	{
		// 각 청크의 그룹별로
		for (groupedChunkIterator = groupedChunks.begin(); groupedChunkIterator != groupedChunks.end(); ++groupedChunkIterator)
		{
			fileIndex_ = 0;

			SortedChunk validatedChunks = groupedChunkIterator->sortedChunks;

			chunkInfos.clear();

			// 이벤트 레코드가 연속된 청크들을 하나의 evtx파일로 덤프한다
			for (sortedChunk = validatedChunks.begin(); sortedChunk != validatedChunks.end(); ++sortedChunk)
			{
				if (!chunkInfos.empty())
				{
					if ((chunkInfos.back().lastRecord + 1) != sortedChunk->second.firstRecord)
					{
						dumpToEvtxFile(chunkInfos);
						chunkInfos.clear();
					}
				}

				chunkInfos.push_back(sortedChunk->second);

				if (chunkInfos.size() == fragmentSize_)
				{
					dumpToEvtxFile(chunkInfos);
					chunkInfos.clear();
				}
			}

			if (!chunkInfos.empty())
			{
				dumpToEvtxFile(chunkInfos);
			}
		}
	}

	return true;
}

/**
	@brief 카빙된 청크들을 evtx파일로 내보낸다
	@param inChunkInfos evtx파일로 내보낼 청크들의 정보
*/
bool EvtxChunkCarver::dumpToEvtxFile(const ChunkInfos& inChunkInfos)
{
	wchar_t outputFileName[MAX_PATH_SIZE] = {0,};
	uint8_t chunkDataBuffer[EVTX_CHUNK_SIZE] = {0,};
	uint8_t evtxFileHeaderBuffer[EVTX_FILE_HEADER_SIZE] = {0,};

	EVTX_FILE_HEADER* evtxFileHeader = (EVTX_FILE_HEADER*)evtxFileHeaderBuffer;

	BinaryFile dumpFile;
	unsigned __int16 chunkCount = 0;
	ChunkInfos::const_iterator currentChunkInfo = inChunkInfos.begin();

	if (currentChunkInfo->fileType == FILE_TYPE_NORMAL)
	{
		if (carveFile_.readData(currentChunkInfo->chunkPos, chunkDataBuffer, EVTX_CHUNK_SIZE) != EVTX_CHUNK_SIZE)
		{
			return false;
		}
	}
	else if (currentChunkInfo->fileType == FILE_TYPE_FRAGMENTED)
	{
		if (fragmentedFile_.readData(currentChunkInfo->chunkPos, chunkDataBuffer, EVTX_CHUNK_SIZE) != EVTX_CHUNK_SIZE)
		{
			return false;
		}
	}

	if (!EvtxChunkValidator::isValidChunk(chunkDataBuffer))
	{
		return false;
	}

	ChunkInfoParser chunkInfoParser(chunkDataBuffer);

	wsprintf(outputFileName, _T("%s\\%s_%s_%s%05d.evtx"), dumpPathName_.c_str(), chunkInfoParser.getComputerName().c_str(),
		chunkInfoParser.getChannelName().c_str(), dumpFilePrefix_.c_str(), fileIndex_);

	dumpFile.openForWrite(outputFileName);

	// 각 청크들을 읽어와서 파일에 기록한다
	for (currentChunkInfo = inChunkInfos.begin(); currentChunkInfo != inChunkInfos.end(); ++currentChunkInfo)
	{
		if (currentChunkInfo->fileType == FILE_TYPE_NORMAL)
		{
			if (carveFile_.readData(currentChunkInfo->chunkPos, chunkDataBuffer, EVTX_CHUNK_SIZE) != EVTX_CHUNK_SIZE)
			{
				break;
			}
		}
		else if (currentChunkInfo->fileType == FILE_TYPE_FRAGMENTED)
		{
			if (fragmentedFile_.readData(currentChunkInfo->chunkPos, chunkDataBuffer, EVTX_CHUNK_SIZE) != EVTX_CHUNK_SIZE)
			{
				break;
			}
		}

		if (!EvtxChunkValidator::isValidChunk(chunkDataBuffer))
		{
			dumpFile.closeFile();
			_tprintf(_T("Delete!!\n"));
			::DeleteFile(outputFileName);
			return false;
		}

		dumpFile.writeData(EVTX_FILE_HEADER_SIZE + chunkCount * EVTX_CHUNK_SIZE, chunkDataBuffer, EVTX_CHUNK_SIZE);

		++chunkCount;
	}

	// evtx파일의 헤더 데이터를 생성한다
	evtxFileHeader->currentChunkNum = 0;
	evtxFileHeader->flags = 1;
	evtxFileHeader->headerPart1Len = sizeof(EVTX_FILE_HEADER);
	evtxFileHeader->headerSize = EVTX_FILE_HEADER_SIZE;
	evtxFileHeader->magic = EVTX_FILE_HEADER_MAGIC;
	evtxFileHeader->majorVersion = 0x03;
	evtxFileHeader->minorVersion = 0x01;
	evtxFileHeader->nextRecordNum = 0;
	evtxFileHeader->oldestChunk = 0;
	evtxFileHeader->chunkCount = chunkCount;

	evtxFileHeader->checkSum = update_crc32(0, evtxFileHeaderBuffer, 0x78);

	// evtx파일의 헤더를 기록한다
	dumpFile.writeData(0, evtxFileHeaderBuffer, EVTX_FILE_HEADER_SIZE);

	dumpFile.closeFile();

	++fileIndex_;

	return chunkCount == inChunkInfos.size();
}

/**
	@brief 한 청크를 컴퓨터이름/채널이름 으로 분류된 그룹에 추가시킨다
	@param inOutGroupedChunk 청크의 그룹들
	@param inChunkInfo 추가할 청크의 정보
*/
void EvtxChunkCarver::insertChunkIntoGroup(GroupedChunks& inOutGroupedChunk, const ChunkInfo& inChunkInfo)
{
	GroupedChunks::iterator groupedChunkIterator;

	for (groupedChunkIterator = inOutGroupedChunk.begin(); groupedChunkIterator != inOutGroupedChunk.end(); ++groupedChunkIterator)
	{
		// 청크의 그룹이 기존에 존재하면
		if (groupedChunkIterator->computerName == inChunkInfo.computerName &&
			groupedChunkIterator->channelName == inChunkInfo.channelName)
		{
			// 기존 그룹에 추가
			groupedChunkIterator->sortedChunks.insert(std::make_pair(inChunkInfo.firstRecord, inChunkInfo));
			break;
		}
	}

	// 그룹이 없을 경우
	if (groupedChunkIterator == inOutGroupedChunk.end())
	{
		// 새로 그룹을 만든다
		GroupedChunk groupedChunk;

		groupedChunk.computerName = inChunkInfo.computerName;
		groupedChunk.channelName = inChunkInfo.channelName;

		groupedChunk.sortedChunks.insert(std::make_pair(inChunkInfo.firstRecord, inChunkInfo));

		inOutGroupedChunk.push_back(groupedChunk);
	}
}