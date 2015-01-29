/**
	@file BinXmlStream.h
	@brief ���̳ʸ� XML������ �Ľ��ϱ� ���� Ŭ������ ����
*/

#ifndef _BIN_XML_STREAM_H_
#define _BIN_XML_STREAM_H_

#include <string>
#include <stack>
#include "DataType.h"

/**
	@class BinXmlStream
	@brief ���̳ʸ� XML ������ �Ľ��ϱ� ���� Ŭ����
*/
class BinXmlStream
{
	public:
		/**
			@class BinXmlProcessType
			@brief ���̳ʸ� XML�� ������ ó���ϴ� ���
		*/
		enum BinXmlProcessType
		{
			BIN_XML_PROCESS_NORMAL = 0,  ///< �Ϲ����� �Ľ�
			BIN_XML_PROCESS_COLLECT_NAME = 1,  ///< XML�̸����� ����
			BIN_XML_PROCESS_COLLECT_TEMPLATE = 2,  ///< XML���ø����� ����
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

		const uint8_t* streamData_;  ///< ��Ʈ�� ����
		int streamSize_;  ///< ��Ʈ�� ũ��
		uint64_t recordId_;  ///< ���ڵ� ��ȣ
		BinXmlProcessType processType_;  ///< ��Ʈ���� ��� ó������(������ ������/�ܼ� �Ľ�)

		std::stack<std::wstring> elementNameStack_;  ///< xml�±� ���ݱ⸦ ǥ���ϱ� ���� ����(���� �±��� �̸�)
		TemplateSubstitutionMap templateSubstitutionMap_;  ///< �Ľ̵� ���ø� ������ Substitution ����
};

#endif /* _BIN_XML_STREAM_H_ */