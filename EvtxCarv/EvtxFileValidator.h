/**
	@file EvtxFileValidator.h
	@brief EVTX�� ������ ���/�������� ��ȿ���� �����ϱ� ���� Ŭ������ ����
*/

#ifndef _EVTX_FILE_VALIDATOR_H_
#define _EVTX_FILE_VALIDATOR_H_

#include <Windows.h>
#include "BinaryFile.h"

/**
	@class EvtxFileValidator
	@brief EVTX������ ��ȿ���� �����ϱ� ���� Ŭ����
*/
class EvtxFileValidator
{
	public:
		static bool isValidEvtxFile(BinaryFile& inDumpFile, uint64_t filePos);
};

#endif /* _EVTX_FILE_VALIDATOR_H_ */