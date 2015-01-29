/**
	@file EvtxRecordCarver.cpp
	@brief EVTX���ڵ带 ī���ϱ� ���� Ŭ������ ����
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
	@brief ī���� ������ ����� ������ �����Ѵ�
	@param inDumpPathName ��� ������
*/
void EvtxRecordCarver::setDumpPathName(const std::wstring& inDumpPathName)
{
	dumpPathName_ = inDumpPathName;
}

/**
	@brief ī���� ������ ����� ���� �̸��� ���Ѵ�
	@param inDumpFilePrefix ��� ���ϸ��� ���λ�
*/
void EvtxRecordCarver::setDumpFilePrefix(const std::wstring& inDumpFilePrefix)
{
	dumpFilePrefix_ = inDumpFilePrefix;
}

/**
	@brief ī���� ������ �����Ѵ�
	@param inFile ī���� ����
*/
void EvtxRecordCarver::setCarveFile(BinaryFile& inFile)
{
	carveFile_ = inFile;
}

/**
	@brief ���ڵ� ī���� �����Ѵ�

	@param inCarvedAreas ī������ ��ŵ�� ����
	@param inJustCollectNameTemplate ���� ����� ���� �ʰ� ���� �̸�/���ø� �������� ������
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

	// ��ȿ�� ���ڵ���� ��Ƽ�(��ŵ ����)
	while ((recordBuffer = signatureFinder.getNext(&filePos)) != NULL)
	{
		evtxRecordHeader = (EVTX_RECORD_HEADER*)recordBuffer;

		if (EvtxRecordValidator::isValidRecord(recordBuffer, EVTX_CHUNK_SIZE))
		{
			validatedRecord.insert(std::make_pair(evtxRecordHeader->numLogRecord, filePos));
		}
	}

	// ���ø��� �̸� ���Ǹ� ������
	collectDatas(validatedRecord, BinXmlStream::BIN_XML_PROCESS_COLLECT_NAME);
	collectDatas(validatedRecord, BinXmlStream::BIN_XML_PROCESS_COLLECT_TEMPLATE);

	if (inJustCollectNameTemplate)
	{
		return true;
	}

	validatedRecord.clear();

	// �ؼ��� ���ڵ���� ��Ƽ�(������ ī���� ���� ��ŵ�ϸ鼭)
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
	@brief ī���� ���ڵ��� �ؼ��Ͽ� csv ���Ϸ� ��������
	@param inRecordPoses ī���� ���ڵ���� ��ġ
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

		// csv�� ����� ���ڵ� ���̵� ��, Ÿ�ӽ�����
		wsprintf(stringBuffer, _T("%I64u, %04d-%02d-%02d %02d:%02d:%02d.%03d"), evtxRecordHeader->numLogRecord,
			recordTimeStamp.wYear, recordTimeStamp.wMonth, recordTimeStamp.wDay,
			recordTimeStamp.wHour, recordTimeStamp.wMinute, recordTimeStamp.wSecond, recordTimeStamp.wMilliseconds);

		writeData.append(stringBuffer);

		BinXmlStream BinXmlStream(recordDataBuffer + EVTX_RECORD_HEADER_SIZE,
			evtxRecordHeader->length1 - 0x1C, evtxRecordHeader->numLogRecord);

		// ���ڵ� ���� �ִ� ���̳ʸ� xml�� ���ڿ��� �Ľ�
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
	@brief ���ڵ�鿡�� �̸�/���ø� �����͵��� �����Ѵ�
	@param inSortedRecords ������ ���ڵ��
	@param inProcessType ������ �����͵�
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

		// ���̳ʸ� xml�� �Ľ��ϸ鼭 �������� �����Ѵ�
		BinXmlStream.parse();
	}
}