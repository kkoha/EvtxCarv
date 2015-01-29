/**
	@file EvtxChunkValidator.h
	@brief EVTX의 청크의 헤더/데이터의 유효성을 검증하기 위한 클래스의 정의
*/

#ifndef _EVTX_CHUNK_VALIDATOR_H_
#define _EVTX_CHUNK_VALIDATOR_H_

/**
	@class EvtxChunkValidator
	@brief EVTX청크의 헤더/데이터 유효성을 검증하기 위한 클래스
*/
class EvtxChunkValidator
{
	public:
		static bool isValidChunk(void* arg);
		static bool isValidChunkHeader(void* arg);
};

#endif _EVTX_CHUNK_VALIDATOR_H_