/**
	@file ChunkInfoParser.cpp
	@brief ûũ ���۷κ��� ������ ������ Ŭ������ ����
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
	@brief ûũ ������ ��� Ŭ���� �ʱ�ȭ
	@param inChunkBuffer ûũ ������ ����
*/
ChunkInfoParser::ChunkInfoParser(uint8_t* inChunkBuffer)
{
	chunkBuffer_ = inChunkBuffer;
}

/**
	@brief ûũ���� ù ��° ���ڵ��� ä���̸�
	@return ä�� �̸�
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

	// xml ���۸� ���ڿ��� �Ľ�
	xmlString = xmlStream.parse();

	// �Ľ��� ���ڿ� �� <Channel>�� </Channel> �� �ѷ����� �κ��� ã�Ƴ���
	// ���� ��� UnknownChannel
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
	@brief ûũ���� ù ��° ���ڵ��� ��ǻ���̸�
	@return ��ǻ�� �̸�
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

	// xml ���۸� ���ڿ��� �Ľ�
	xmlString = xmlStream.parse();

	// �Ľ��� ���ڿ� �� <Computer>�� </Computer> �� �ѷ����� �κ��� ã�Ƴ���
	// ���� ��� UnknownComputer
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