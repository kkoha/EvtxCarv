/**
	@file ChunkInfoParser.h
	@brief ûũ ���۷κ��� ������ ������ Ŭ������ ����
*/

#ifndef _CHUNK_INFO_PARSER_H_
#define _CHUNK_INFO_PARSER_H_

#include "DataType.h"
#include <string>

/**
	@class ChunkInfoParser
	@brief ûũ ���۷κ��� ä���̸�/��ǻ�� �̸��� ���� �� �ִ� Ŭ����
*/
class ChunkInfoParser
{
	public:
		ChunkInfoParser(uint8_t* inChunkBuffer);

		std::wstring getChannelName();
		std::wstring getComputerName();

	private:
		uint8_t* chunkBuffer_; ///< ûũ�� ������ ��� ����
};

#endif _CHUNK_INFO_PARSER_H_