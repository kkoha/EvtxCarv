/**
	@file EvtxChunkCarver.cpp
	@brief EVTXûũ�� ī���ϱ� ���� Ŭ������ ����
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
	@brief ī���� ����(����) �� �����Ѵ�
	@param inFile ī���� ����
*/
void EvtxChunkCarver::setCarveFile(BinaryFile& inFile)
{
	carveFile_ = inFile;
}

/**
	@brief ī���� ����(������)�� �����Ѵ�
	@param inFile ī���� ����
*/
void EvtxChunkCarver::setFragmentedFile(BinaryFile& inFile)
{
	fragmentedFile_ = inFile;
}

/**
	@brief ī���� ������ ����� ���� �̸��� ���Ѵ�
	@param inDumpFilePrefix ��� ���ϸ��� ���λ�
*/
void EvtxChunkCarver::setDumpFilePrefix(const std::wstring& inDumpFilePrefix)
{
	dumpFilePrefix_ = inDumpFilePrefix;
}

/**
	@brief ī���� ������ ����� ������ �����Ѵ�
	@param inDumpPathName ��� ������
*/
void EvtxChunkCarver::setDumpPathName(const std::wstring& inDumpPathName)
{
	dumpPathName_ = inDumpPathName;
}

/**
	@brief ī���� ������ ���� �� �� ���Ͽ� �� ûũ�� �ִ� ������ ���Ѵ�
	@param inFragmentSize �ִ� ������
*/
void EvtxChunkCarver::setFragmentSize(int inFragmentSize)
{
	fragmentSize_ = inFragmentSize;
}

/**
	@brief ûũ ī���� �����Ѵ�

	@param inCarvedAreas ī������ ��ŵ�� ����
	@param inUseFragmentedFile �����յ� ������ ������� ����
	@param inWriteOutput ī���� ����� ���Ϸ� �������� ����
	@param outCarvedAreas �Ϲ� ���Ͽ��� ī���� ����
	@param outFragmentedCarvedAreas �����յ� ���Ͽ��� ī���� ����
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

	// �Ϲ� ���Ͽ��� ��ȿ�� ûũ���� ã�Ƽ�
	while ((chunkDataBuffer = signatureFinder.getNext(&filePos)) != NULL)
	{
		if (EvtxChunkValidator::isValidChunk(chunkDataBuffer))
		{
			evtxChunkHeader = (EVTX_CHUNK_HEADER*)chunkDataBuffer;

			signatureFinder.addCarvedArea(std::make_pair(filePos, filePos + evtxChunkHeader->ofsRecNext));

			if (inWriteOutput)
			{
				ChunkInfoParser chunkInfoParser(chunkDataBuffer);

				// ��ǻ��/ä���̸� ������� �׷����Ѵ�
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

		// �������� ���Ͽ��� ��ȿ�� ûũ���� ã�Ƽ�
		while ((chunkDataBuffer = signatureFinderFragmented.getNext(&filePos)) != NULL)
		{
			if (EvtxChunkValidator::isValidChunk(chunkDataBuffer))
			{
				evtxChunkHeader = (EVTX_CHUNK_HEADER*)chunkDataBuffer;

				signatureFinderFragmented.addCarvedArea(std::make_pair(filePos, filePos + evtxChunkHeader->ofsRecNext));

				if (inWriteOutput)
				{
					ChunkInfoParser chunkInfoParser(chunkDataBuffer);

					// ��ǻ��/ä���̸� ������� �׷����Ѵ�
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
		// �� ûũ�� �׷캰��
		for (groupedChunkIterator = groupedChunks.begin(); groupedChunkIterator != groupedChunks.end(); ++groupedChunkIterator)
		{
			fileIndex_ = 0;

			SortedChunk validatedChunks = groupedChunkIterator->sortedChunks;

			chunkInfos.clear();

			// �̺�Ʈ ���ڵ尡 ���ӵ� ûũ���� �ϳ��� evtx���Ϸ� �����Ѵ�
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
	@brief ī���� ûũ���� evtx���Ϸ� ��������
	@param inChunkInfos evtx���Ϸ� ������ ûũ���� ����
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

	// �� ûũ���� �о�ͼ� ���Ͽ� ����Ѵ�
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

	// evtx������ ��� �����͸� �����Ѵ�
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

	// evtx������ ����� ����Ѵ�
	dumpFile.writeData(0, evtxFileHeaderBuffer, EVTX_FILE_HEADER_SIZE);

	dumpFile.closeFile();

	++fileIndex_;

	return chunkCount == inChunkInfos.size();
}

/**
	@brief �� ûũ�� ��ǻ���̸�/ä���̸� ���� �з��� �׷쿡 �߰���Ų��
	@param inOutGroupedChunk ûũ�� �׷��
	@param inChunkInfo �߰��� ûũ�� ����
*/
void EvtxChunkCarver::insertChunkIntoGroup(GroupedChunks& inOutGroupedChunk, const ChunkInfo& inChunkInfo)
{
	GroupedChunks::iterator groupedChunkIterator;

	for (groupedChunkIterator = inOutGroupedChunk.begin(); groupedChunkIterator != inOutGroupedChunk.end(); ++groupedChunkIterator)
	{
		// ûũ�� �׷��� ������ �����ϸ�
		if (groupedChunkIterator->computerName == inChunkInfo.computerName &&
			groupedChunkIterator->channelName == inChunkInfo.channelName)
		{
			// ���� �׷쿡 �߰�
			groupedChunkIterator->sortedChunks.insert(std::make_pair(inChunkInfo.firstRecord, inChunkInfo));
			break;
		}
	}

	// �׷��� ���� ���
	if (groupedChunkIterator == inOutGroupedChunk.end())
	{
		// ���� �׷��� �����
		GroupedChunk groupedChunk;

		groupedChunk.computerName = inChunkInfo.computerName;
		groupedChunk.channelName = inChunkInfo.channelName;

		groupedChunk.sortedChunks.insert(std::make_pair(inChunkInfo.firstRecord, inChunkInfo));

		inOutGroupedChunk.push_back(groupedChunk);
	}
}