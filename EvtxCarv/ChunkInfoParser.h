/**
	@file ChunkInfoParser.h
	@brief 청크 버퍼로부터 정보를 얻어오는 클래스의 정의
*/

#ifndef _CHUNK_INFO_PARSER_H_
#define _CHUNK_INFO_PARSER_H_

#include "DataType.h"
#include <string>

/**
	@class ChunkInfoParser
	@brief 청크 버퍼로부터 채널이름/컴퓨터 이름을 얻어올 수 있는 클래스
*/
class ChunkInfoParser
{
	public:
		ChunkInfoParser(uint8_t* inChunkBuffer);

		std::wstring getChannelName();
		std::wstring getComputerName();

	private:
		uint8_t* chunkBuffer_; ///< 청크의 내용이 담긴 버퍼
};

#endif _CHUNK_INFO_PARSER_H_