/**
	@file EvtxRecordCarver.cpp
	@brief EVTX레코드를 카빙하기 위한 클래스의 구현
*/

#include "EvtxRecordCarver.h"
#include "EvtxChunkHeader.h"
#include "EvtxRecordValidator.h"
#include "EvtxRecordHeader.h"
#include "EvtxFileHeader.h"
#include "BinXmlStream.h"
#include <string.h>
#include <tchar.h>
#include "crc32.h"
#include <list>
#include <map>
#include <set>
#include "StringTransform.h"
#include "SignatureFinder.h"
#include "ComputerNameCollection.h"
#include "ChannelNameCollection.h"
#include <io.h>
#include <fcntl.h>
#include "EventXmlParser.h"

#pragma warning(disable:4996) // wsprintf not able for %lf

EvtxRecordCarver::EvtxRecordCarver()
{
	;
}

/**
	@brief 카빙된 내용을 출력할 폴더를 설정한다
	@param inDumpPathName 출력 폴더명
*/
void EvtxRecordCarver::setDumpPathName(const std::wstring& inDumpPathName)
{
	dumpPathName_ = inDumpPathName;
}

/**
	@brief 카빙된 내용을 출력할 파일 이름을 정한다
	@param inDumpFilePrefix 출력 파일명의 접두사
*/
void EvtxRecordCarver::setDumpFilePrefix(const std::wstring& inDumpFilePrefix)
{
	dumpFilePrefix_ = inDumpFilePrefix;
}

/**
	@brief 카빙할 파일을 지정한다
	@param inFile 카빙할 파일
*/
void EvtxRecordCarver::setCarveFile(BinaryFile& inFile)
{
	carveFile_ = inFile;
}

/**
	@brief 레코드 카빙을 시작한다

	@param inCarvedAreas 카빙에서 스킵할 영역
	@param inJustCollectNameTemplate 실제 출력을 내지 않고 단지 이름/템플릿 모음만을 생성함
*/
bool EvtxRecordCarver::startCarving(const CarvedAreas& inCarvedAreas, bool inJustCollectNameTemplate)
{
	uint64_t filePos = 0;
	uint8_t* recordBuffer = NULL;
	EVTX_RECORD_HEADER* evtxRecordHeader = NULL;
	DWORD readRecordHeaderMagic = 0;
	SortedRecord validatedRecord;
	SortedRecord::iterator sortedRecord;
	RecordPoses recordPoses;

	SignatureFinder signatureFinder(carveFile_, EVTX_RECORD_HEADER_MAGIC, 4, 8, NULL, EVTX_CHUNK_SIZE);
	SignatureFinder signatureFinderWithSkip(carveFile_, EVTX_RECORD_HEADER_MAGIC, 4, 8, &inCarvedAreas, EVTX_CHUNK_SIZE);

	// 유효한 레코드들을 모아서(스킵 없이)
	while ((recordBuffer = signatureFinder.getNext(&filePos)) != NULL)
	{
		evtxRecordHeader = (EVTX_RECORD_HEADER*)recordBuffer;

		if (EvtxRecordValidator::isValidRecord(recordBuffer, EVTX_CHUNK_SIZE))
		{
			validatedRecord.insert(std::make_pair(evtxRecordHeader->numLogRecord, filePos));
		}
	}

	// 템플릿과 이름 정의를 모은다
	collectDatas(validatedRecord, BinXmlStream::BIN_XML_PROCESS_COLLECT_NAME);
	collectDatas(validatedRecord, BinXmlStream::BIN_XML_PROCESS_COLLECT_TEMPLATE);

	if (inJustCollectNameTemplate)
	{
		return true;
	}

	validatedRecord.clear();

	// 해석할 레코드들을 모아서(이전에 카빙한 영역 스킵하면서)
	while ((recordBuffer = signatureFinderWithSkip.getNext(&filePos)) != NULL)
	{
		evtxRecordHeader = (EVTX_RECORD_HEADER*)recordBuffer;

		if (EvtxRecordValidator::isValidRecord(recordBuffer, EVTX_CHUNK_SIZE))
		{
			validatedRecord.insert(std::make_pair(evtxRecordHeader->numLogRecord, filePos));
		}
	}

	recordPoses.clear();
	signatureFinder.closeMap();
	signatureFinderWithSkip.closeMap();

	for (sortedRecord = validatedRecord.begin(); sortedRecord != validatedRecord.end(); ++sortedRecord)
	{
		recordPoses.push_back(sortedRecord->second);
	}

	if (!recordPoses.empty())
	{
		dumpToCsvFile(recordPoses);
	}

	return true;
}

/**
	@brief 카빙된 레코들을 해석하여 csv 파일로 내보낸다
	@param inRecordPoses 카빙된 레코드들의 위치
*/
bool EvtxRecordCarver::dumpToCsvFile(const RecordPoses& inRecordPoses)
{
	wchar_t outputFileName[MAX_PATH_SIZE] = {0,};
	uint8_t recordDataBuffer[EVTX_CHUNK_SIZE] = {0,};
	EVTX_RECORD_HEADER* evtxRecordHeader = (EVTX_RECORD_HEADER*)recordDataBuffer;

	wchar_t stringBuffer[65536] = {0,};
	std::wstring writeData = _T("");
	std::wstring computerName = _T("");
	std::wstring channelName = _T("");
	RecordPoses::const_iterator recordPos;
	SYSTEMTIME recordTimeStamp;
	std::wstring xmlData = _T("");
	std::wstring messageData = _T("");
	std::set<std::wstring> openedFiles;
	FILE* csvFile = NULL;

	for (recordPos = inRecordPoses.begin(); recordPos != inRecordPoses.end(); ++recordPos)
	{
		writeData.clear();

		memset(recordDataBuffer, 0, sizeof(recordDataBuffer));

		carveFile_.readData(*recordPos, recordDataBuffer, EVTX_CHUNK_SIZE);

		if (!EvtxRecordValidator::isValidRecord(recordDataBuffer, EVTX_CHUNK_SIZE)) { continue; }

		FileTimeToSystemTime(&evtxRecordHeader->timeCreated, &recordTimeStamp);

		// csv에 기록한 레코드 아이디 및, 타임스탬프
		wsprintf(stringBuffer, _T("%I64u, %04d-%02d-%02d %02d:%02d:%02d.%03d"), evtxRecordHeader->numLogRecord,
			recordTimeStamp.wYear, recordTimeStamp.wMonth, recordTimeStamp.wDay,
			recordTimeStamp.wHour, recordTimeStamp.wMinute, recordTimeStamp.wSecond, recordTimeStamp.wMilliseconds);

		writeData.append(stringBuffer);

		BinXmlStream BinXmlStream(recordDataBuffer + EVTX_RECORD_HEADER_SIZE,
			evtxRecordHeader->length1 - 0x1C, evtxRecordHeader->numLogRecord);

		// 레코드 내에 있는 바이너리 xml을 문자열로 파싱
		xmlData = BinXmlStream.parse();

		computerName = ComputerNameCollection::getInstance()->getComputerName(xmlData).c_str();
		channelName = ChannelNameCollection::getInstance()->getChannelName(xmlData).c_str();

		if (computerName == _T(""))
		{
			computerName = _T("UnknownComputer");
		}
		if (channelName == _T(""))
		{
			channelName = _T("UnknownChannel");
		}

		EventXmlParser eventXmlParser(xmlData);

		wsprintf(outputFileName, _T("%s\\%s_%s_%s.csv"), dumpPathName_.c_str(),
			computerName.c_str(), channelName.c_str(), dumpFilePrefix_.c_str());

		messageData = eventXmlParser.getMessage();

		xmlData = StringTransform::replaceString(xmlData, _T("\""), _T("\"\""));
		messageData = StringTransform::replaceString(messageData, _T("\""), _T("\"\""));

		if (openedFiles.find(outputFileName) != openedFiles.end())
		{
			csvFile = _tfopen(outputFileName, _T("a+"));
		}
		else
		{
			csvFile = _tfopen(outputFileName, _T("w"));

			if (csvFile != NULL)
			{
				_setmode(fileno(csvFile), _O_U8TEXT);
				openedFiles.insert(outputFileName);
				fwprintf(csvFile, _T("\uFEFF"));
				fwprintf(csvFile, _T("Event Record Number, Event Timestamp, Event Source, Event Keyword, Event Level, Event Computer, Event Cahnnel, EventID, Event Message, Event XML\n"));
			}
		}
		
		if (csvFile != NULL)
		{
			_setmode(fileno(csvFile), _O_U8TEXT);
			fwprintf(csvFile, _T("%s"), writeData.c_str());
			fwprintf(csvFile, _T(",%s,%s"), eventXmlParser.getSource().c_str(), eventXmlParser.getKeyword().c_str());
			fwprintf(csvFile, _T(",%s,%s"), eventXmlParser.getLevel().c_str(), eventXmlParser.getComputer().c_str());
			fwprintf(csvFile, _T(",%s,%s"), eventXmlParser.getChannel().c_str(), eventXmlParser.getEventId().c_str());
			fwprintf(csvFile, _T(",\"\n%s\n\""), messageData.c_str());
			fwprintf(csvFile, _T(",\"\n%s\n\"\n"), xmlData.c_str());
			fclose(csvFile);
		}
	}

	return true;
}

/**
	@brief 레코드들에서 이름/템플릿 데이터들을 수집한다
	@param inSortedRecords 수집할 레코드들
	@param inProcessType 수집할 데이터들
*/
void EvtxRecordCarver::collectDatas(const SortedRecord& inSortedRecords, BinXmlStream::BinXmlProcessType inProcessType)
{
	uint8_t recordDataBuffer[EVTX_CHUNK_SIZE] = {0,};
	EVTX_RECORD_HEADER* evtxRecordHeader = (EVTX_RECORD_HEADER*)recordDataBuffer;
	SortedRecord::const_iterator sortedRecord;

	for (sortedRecord = inSortedRecords.begin(); sortedRecord != inSortedRecords.end(); ++sortedRecord)
	{
		memset(recordDataBuffer, 0, sizeof(recordDataBuffer));

		carveFile_.readData(sortedRecord->second, recordDataBuffer, EVTX_CHUNK_SIZE);

		if (!EvtxRecordValidator::isValidRecord(recordDataBuffer, EVTX_CHUNK_SIZE))
		{
			return;
		}

		BinXmlStream BinXmlStream(recordDataBuffer + EVTX_RECORD_HEADER_SIZE,
			evtxRecordHeader->length1 - 0x1C, evtxRecordHeader->numLogRecord, inProcessType);

		// 바이너리 xml을 파싱하면서 정보들을 수집한다
		BinXmlStream.parse();
	}
}