/**
	@file EvtxFileValidator.h
	@brief EVTX의 파일의 헤더/데이터의 유효성을 검증하기 위한 클래스의 정의
*/

#ifndef _EVTX_FILE_VALIDATOR_H_
#define _EVTX_FILE_VALIDATOR_H_

#include <Windows.h>
#include "BinaryFile.h"

/**
	@class EvtxFileValidator
	@brief EVTX파일의 유효성을 검증하기 위한 클래스
*/
class EvtxFileValidator
{
	public:
		static bool isValidEvtxFile(BinaryFile& inDumpFile, uint64_t filePos);
};

#endif /* _EVTX_FILE_VALIDATOR_H_ */