/**
	@file PeResourceParser.h
	@brief PE파일의 리소스를 읽기위한 클래스의 정의
*/

#ifndef _PE_RESOURCE_PARSER_H_
#define _PE_RESOURCE_PARSER_H_

#include <string>
#include <tchar.h>
#include "DataType.h"

/**
	@class PeResourceParser
	@brief PE파일에서 리소스를 읽기위한 클래스
*/
class PeResourceParser
{
	public:
		static bool getMessageFromFile(const std::wstring& inFileName, uint32_t stringIdentifier, std::wstring& outString);
};

#endif /* _PE_RESOURCE_PARSER_H_ */