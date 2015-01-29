/**
	@file EvtxFileHeader.h
	@brief EVTX파일의 헤더 구조와 관련된 정의/구조체들
*/

#ifndef _EVTX_FILE_HEADER_H_
#define _EVTX_FILE_HEADER_H_

#define EVTX_FILE_HEADER_MAGIC 0x00656C6946666C45LL /* "ElfFile", 00 */ ///< EVTX파일헤더의 매직값 ElfFile,0
#define EVTX_FILE_HEADER_SIZE 0x1000 ///< EVTX파일 헤더의 크기

#pragma pack(1)

/**
	@class EVTX_FILE_HEADER
	@brief EVTX 파일의 파일 헤더 구조
*/
struct EVTX_FILE_HEADER
{
	uint64_t magic;  ///< 매직값
	int64_t oldestChunk;  ///< oldestChunk
	int64_t currentChunkNum;  ///< chrrentChunkNum
	int64_t nextRecordNum;  ///< 다음 레코드의 번호
	uint32_t headerPart1Len;  ///< 파일 헤더의 길이(의미있는 데이터)
	unsigned __int16 minorVersion;  ///< 파일 헤더의 버전(major)
	unsigned __int16 majorVersion;  ///< 파일 헤더의 버전(minor)
	unsigned __int16 headerSize;  ///< 헤더 전체의 길이
	unsigned __int16 chunkCount;  ///< 가지고 있는 청크의 수
	char unknown[76];
	uint32_t flags; ///< 플래그
	uint32_t checkSum;  ///< 헤더의 체크섬
};

#pragma pack()

#endif /* _EVTX_FILE_HEADER_H_ */