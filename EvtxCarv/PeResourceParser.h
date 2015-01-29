/**
	@file PeResourceParser.h
	@brief PE������ ���ҽ��� �б����� Ŭ������ ����
*/

#ifndef _PE_RESOURCE_PARSER_H_
#define _PE_RESOURCE_PARSER_H_

#include <string>
#include <tchar.h>
#include "DataType.h"

/**
	@class PeResourceParser
	@brief PE���Ͽ��� ���ҽ��� �б����� Ŭ����
*/
class PeResourceParser
{
	public:
		static bool getMessageFromFile(const std::wstring& inFileName, uint32_t stringIdentifier, std::wstring& outString);
};

#endif /* _PE_RESOURCE_PARSER_H_ */