/**
	@file BinXmlStream.h
	@brief 바이너리 XML내용을 파싱하기 위한 클래스의 정의
*/

#ifndef _BIN_XML_STREAM_H_
#define _BIN_XML_STREAM_H_

#include <string>
#include <stack>
#include "DataType.h"

/**
	@class BinXmlStream
	@brief 바이너리 XML 내용을 파싱하기 위한 클래스
*/
class BinXmlStream
{
	public:
		/**
			@class BinXmlProcessType
			@brief 바이너리 XML의 내용을 처리하는 방법
		*/
		enum BinXmlProcessType
		{
			BIN_XML_PROCESS_NORMAL = 0,  ///< 일반적인 파싱
			BIN_XML_PROCESS_COLLECT_NAME = 1,  ///< XML이름들을 모음
			BIN_XML_PROCESS_COLLECT_TEMPLATE = 2,  ///< XML템플릿들을 모음
		};

		BinXmlStream(const void* inStreamData, int inStreamSize, uint64_t inRecordId, BinXmlProcessType inProcessType = BIN_XML_PROCESS_NORMAL);

		std::wstring parse(uint32_t* outParsedSize = NULL);

	private:
		uint32_t parseToken(uint32_t inStart, uint32_t inEnd, std::wstring& outString);
		uint32_t parseValue(int valueType, uint32_t inStart, uint32_t inEnd, uint32_t inValueSize, std::wstring& outString);
		uint32_t parseArray(int valueType, uint32_t inStart, uint32_t inEnd, uint32_t inValueSize, std::wstring& outString);

		uint32_t parseString(uint32_t inStart, uint32_t inEnd, std::wstring& outString);
		uint32_t parseStringWithSize(uint32_t inStart, uint32_t inEnd, uint32_t inSize, std::wstring& outString);
		uint32_t parseAnsiString(uint32_t inStart, uint32_t inEnd, std::wstring& outString);
		uint32_t parseAnsiStringWithSize(uint32_t inStart, uint32_t inEnd, uint32_t inSize, std::wstring& outString);

		bool isBinXmlName(uint32_t inStart, uint32_t inEnd);
		uint32_t getBinXmlName(uint32_t inStart, uint32_t inEnd, std::wstring& outName);

		uint32_t parseElement(uint32_t inStart, uint32_t inEnd, std::wstring& outString);
		uint32_t parseAttribute(uint32_t inStart, uint32_t inEnd, std::wstring& outString);
		uint32_t parseTemplate(uint32_t inStart, uint32_t inEnd, std::wstring& outString);

		TemplateSubstitutionMap getTemplateSubstitutionMap();

		const uint8_t* streamData_;  ///< 스트림 버퍼
		int streamSize_;  ///< 스트림 크기
		uint64_t recordId_;  ///< 레코드 번호
		BinXmlProcessType processType_;  ///< 스트림을 어떻게 처리할지(데이터 모으기/단순 파싱)

		std::stack<std::wstring> elementNameStack_;  ///< xml태그 여닫기를 표시하기 위한 스택(현재 태그의 이름)
		TemplateSubstitutionMap templateSubstitutionMap_;  ///< 파싱된 템플릿 정의의 Substitution 정보
};

#endif /* _BIN_XML_STREAM_H_ */