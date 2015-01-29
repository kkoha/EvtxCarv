/**
	@file ChunkInfoParser.cpp
	@brief 청크 버퍼로부터 정보를 얻어오는 클래스의 구현
*/

#include "ChunkInfoParser.h"
#include "EvtxChunkHeader.h"
#include "EvtxRecordHeader.h"
#include "BinXmlStream.h"
#include "StringTransform.h"
#include "ComputerNameCollection.h"
#include "ChannelNameCollection.h"
#include <tchar.h>
#include <stdio.h>

/**
	@brief 청크 정보를 얻는 클래스 초기화
	@param inChunkBuffer 청크 데이터 버퍼
*/
ChunkInfoParser::ChunkInfoParser(uint8_t* inChunkBuffer)
{
	chunkBuffer_ = inChunkBuffer;
}

/**
	@brief 청크에서 첫 번째 레코드의 채널이름
	@return 채널 이름
*/
std::wstring ChunkInfoParser::getChannelName()
{
	uint8_t* xmlStreamBuffer = chunkBuffer_ + EVTX_CHUNK_HEADER_SIZE + EVTX_RECORD_HEADER_SIZE;
	EVTX_RECORD_HEADER* recordHeader = (EVTX_RECORD_HEADER*)(chunkBuffer_ + EVTX_CHUNK_HEADER_SIZE);
	std::wstring xmlString = _T("");
	std::wstring result;

	BinXmlStream xmlStreamName(xmlStreamBuffer, recordHeader->length1 - EVTX_RECORD_HEADER_SIZE - 4, recordHeader->numLogRecord, BinXmlStream::BIN_XML_PROCESS_COLLECT_NAME);
	BinXmlStream xmlStreamTemplate(xmlStreamBuffer, recordHeader->length1 - EVTX_RECORD_HEADER_SIZE - 4, recordHeader->numLogRecord, BinXmlStream::BIN_XML_PROCESS_COLLECT_TEMPLATE);
	BinXmlStream xmlStream(xmlStreamBuffer, recordHeader->length1 - EVTX_RECORD_HEADER_SIZE - 4, recordHeader->numLogRecord);

	xmlStreamName.parse();
	xmlStreamTemplate.parse();

	// xml 버퍼를 문자열로 파싱
	xmlString = xmlStream.parse();

	// 파싱한 문자열 중 <Channel>과 </Channel> 에 둘러쌓인 부분을 찾아낸다
	// 없을 경우 UnknownChannel
	if (xmlString.find(_T("<Channel>")) != xmlString.npos)
	{
		result = xmlString.substr(xmlString.find(_T("<Channel>")));

		if (result.find(_T("</Channel>")) != result.npos)
		{
			result = result.substr(0, result.find(_T("</Channel>")));
			result = StringTransform::replaceString(result, _T("<Channel>"), _T(""));
			result = StringTransform::replaceString(result, _T("/"), _T("%4"));
			ChannelNameCollection::getInstance()->insertChannelName(StringTransform::toLowerCase(result));
		}
		else
		{
			result = _T("UnknownChannel");
		}
	}
	else
	{
		result = _T("UnknownChannel");
	}

	return result;

}

/**
	@brief 청크에서 첫 번째 레코드의 컴퓨터이름
	@return 컴퓨터 이름
*/
std::wstring ChunkInfoParser::getComputerName()
{
	uint8_t* xmlStreamBuffer = chunkBuffer_ + EVTX_CHUNK_HEADER_SIZE + EVTX_RECORD_HEADER_SIZE;
	EVTX_RECORD_HEADER* recordHeader = (EVTX_RECORD_HEADER*)(chunkBuffer_ + EVTX_CHUNK_HEADER_SIZE);
	std::wstring xmlString = _T("");
	std::wstring result;

	BinXmlStream xmlStreamName(xmlStreamBuffer, recordHeader->length1 - EVTX_RECORD_HEADER_SIZE - 4, recordHeader->numLogRecord, BinXmlStream::BIN_XML_PROCESS_COLLECT_NAME);
	BinXmlStream xmlStreamTemplate(xmlStreamBuffer, recordHeader->length1 - EVTX_RECORD_HEADER_SIZE - 4, recordHeader->numLogRecord, BinXmlStream::BIN_XML_PROCESS_COLLECT_TEMPLATE);
	BinXmlStream xmlStream(xmlStreamBuffer, recordHeader->length1 - EVTX_RECORD_HEADER_SIZE - 4, recordHeader->numLogRecord);

	xmlStreamName.parse();
	xmlStreamTemplate.parse();

	// xml 버퍼를 문자열로 파싱
	xmlString = xmlStream.parse();

	// 파싱한 문자열 중 <Computer>과 </Computer> 에 둘러쌓인 부분을 찾아낸다
	// 없을 경우 UnknownComputer
	if (xmlString.find(_T("<Computer>")) != xmlString.npos)
	{
		result = xmlString.substr(xmlString.find(_T("<Computer>")));

		if (result.find(_T("</Computer>")) != result.npos)
		{
			result = result.substr(0, result.find(_T("</Computer>")));
			result = StringTransform::replaceString(result, _T("<Computer>"), _T(""));
			result = StringTransform::replaceString(result, _T("/"), _T("%4"));
			ComputerNameCollection::getInstance()->insertComputerName(result);
		}
		else
		{
			result = _T("UnknownComputer");
		}
	}
	else
	{
		result = _T("UnknownComputer");
	}

	return result;
}