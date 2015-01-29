/**
	@file EvtxChunkHeader.h
	@brief EVTX청크의 헤더 구조와 관련된 정의/구조체들
*/

#ifndef _EVTX_CHUNK_HEADER_
#define _EVTX_CHUNK_HEADER_

#include "DataType.h"

#define EVTX_CHUNK_HEADER_MAGIC 0x006B6E6843666C45LL /* "ElfChnk", 00 */ ///< EVTX청크헤더의 매직값 ElfChnk,0
#define EVTX_CHUNK_SIZE 0x10000  ///< EVTX청크의 전체 사이즈
#define EVTX_CHUNK_HEADER_SIZE 0x200  ///< EVTX청크 헤더의 사이즈

#pragma pack(1)

/**
	@class EVTX_CHUNK_HEADER
	@brief EVTX 파일의 청크 헤더 구조
*/
struct EVTX_CHUNK_HEADER
{
	uint64_t magic;  ///< 매직값
	uint64_t numLogRecFirst;  ///< 청크 시작 레코드의 번호
	uint64_t numLogRecLast;  ///< 청크의 마지막 레코드의 번호
	uint64_t numFileRecFirst;  ///< 청크 시작 레코드의 번호
	uint64_t numFileRecLast;  ///< 청크의 마지막 레코드의 번호
	uint32_t ofsTables;  ///< 스트링/템플릿 테이블의 오프셋
	uint32_t ofsRecLast;  ///< 마지막 레코드의 오프셋
	uint32_t ofsRecNext;  ///< 마지막 레코드가 끝나는 오프셋
	uint32_t dataCRC;  ///< 청크 데이터의 체크섬
	char unknown[68];
	uint32_t headerCRC;  ///< 청크 헤더의 체크섬
	uint32_t stringTable[64];  ///< 스트링의 오프셋 테이블
	uint32_t templateTable[32];  ///< 템플릿의 오프셋 테이블
};

#pragma pack()

#endif _EVTX_CHUNK_HEADER_