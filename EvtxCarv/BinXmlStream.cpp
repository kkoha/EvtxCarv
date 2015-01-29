/**
	@file BinXmlStream.cpp
	@brief 바이너리 XML내용을 파싱하기 위한 클래스의 구현
*/

#include "BinXmlStream.h"
#include <string>

#include <tchar.h>
#include <Windows.h>
#include <Sddl.h>
#include <vector>

#include "BinaryXmlDef.h"
#include "BinaryXmlTokens.h"
#include "EvtxChunkHeader.h"
#include "BinXmlTemplateCollection.h"
#include "BinXmlNameCollection.h"
#include "BinXmlTemplate.h"
#include "StringTransform.h"
#include "ChannelNameCollection.h"
#include "ComputerNameCollection.h"

#pragma warning(disable:4996) // wsprintf not able for %lf

/**
	@brief 바이너리 xml 파서를 생성한다

	@param inStreamData 바이너리 xml 데이터의 버퍼
	@param inStreamSize 바이너리 xml 데이터 버퍼의 사이즈
	@param inRecordId 레코드 아이디
	@param inProcessType 파싱할 타입(이름/템플릿 수집 혹은 단순 파싱)
*/
BinXmlStream::BinXmlStream(const void* inStreamData, int inStreamSize, uint64_t inRecordId, BinXmlProcessType inProcessType)
{
	streamData_ = (const uint8_t*)inStreamData;
	streamSize_ = inStreamSize;
	recordId_ = inRecordId;
	processType_ = inProcessType;
	templateSubstitutionMap_.clear();
}

/**
	@brief 바이너리 xml을 파싱하여 xml문자열로 얻는다
	@param outParsedSize 파싱된 사이드(null일 경우 스킵)
	@return 파싱된 xml문자열
*/
std::wstring  BinXmlStream::parse(uint32_t* outParsedSize)
{
	std::wstring outText = _T("");
	std::wstring tokenText = _T("");

	int parsePos = 0;
	int parsedSize = 0;

	for (parsePos = 0; parsePos < streamSize_; )
	{
		parsedSize = parseToken(parsePos, streamSize_, tokenText);
		if (parsedSize == 0) { break; }

		parsePos += parsedSize;

		if (tokenText.length() != 0)
		{
			outText += tokenText;
		}
	}

	if (outParsedSize != NULL)
	{
		*outParsedSize = parsePos + 1;
	}

	return outText;
}

TemplateSubstitutionMap BinXmlStream::getTemplateSubstitutionMap()
{
	return templateSubstitutionMap_;
}

/**
	@brief 바이너리 xml 토큰 하나를 파싱한다

	@param inStart 토큰의 시작점
	@param inEnd 토큰의 끝
	@param outString 파싱된 토큰 문자열

	@return 파싱된 사이즈
*/
uint32_t BinXmlStream::parseToken(uint32_t inStart, uint32_t inEnd, std::wstring& outString)
{
	int parsedSize = 0;
	std::wstring childString = _T("");
	uint32_t i = 0;

	if (inStart >= inEnd) { return 0; }

	uint8_t preamble = streamData_[inStart];

	if (inStart < 0 || inStart >= inEnd) { return 0; }

	if (((preamble >> 4) != 0) && ((preamble >> 4) != 4)) { return 0; }

	switch (GET_TOKEN(preamble))
	{
		case BIN_XML_TOKEN_EOF:
			parsedSize = 0;
			break;
		case BIN_XML_TOKEN_OPEN_START_ELEMENT:
			parsedSize = parseElement(inStart, inEnd, outString);
			break;
		case BIN_XML_TOKEN_CLOSE_START_ELEMENT:
			outString = _T(">");
			parsedSize = 1;
			break;
		case BIN_XML_TOKEN_CLOSE_EMPTY_ELEMENT:
			if (!elementNameStack_.empty())
			{
				elementNameStack_.pop();
			}
			outString = _T("/>");
			parsedSize = 1;
			break;
		case BIN_XML_TOKEN_END_ELEMENT_TAG:
			if (!elementNameStack_.empty())
			{
				outString = std::wstring(_T("</")) + elementNameStack_.top() + _T(">");
				elementNameStack_.pop();
			}
			else
			{
				outString = _T("</>");
			}
			parsedSize = 1;
			break;
		case BIN_XML_TOKEN_VALUE:
			parsedSize = 2 + parseValue(streamData_[inStart + 1], inStart + 2, inEnd, inEnd - inStart - 2, childString);
			outString = childString;
			break;
		case BIN_XML_TOKEN_ATTRIBUTE:
			parsedSize = parseAttribute(inStart, inEnd, outString);
			break;
		case BIN_XML_TOKEN_CDATA_SECTION:
			parsedSize = 1 + parseString(inStart + 1, inEnd, childString);
			break;
		case BIN_XML_TOKEN_CHAR_REF:
			parsedSize = 3;
			break;
		case BIN_XML_TOKEN_ENTITY_REF:
			parsedSize = 5;
			break;
		case BIN_XML_TOKEN_PI_TARGET:
			parsedSize = 5;
			break;
		case BIN_XML_TOKEN_PI_DATA:
			parsedSize = 1 + parseString(inStart + 1, inEnd, childString);
			break;
		case BIN_XML_TOKEN_TEMPLATE_INSTANCE:
			parsedSize = parseTemplate(inStart, inEnd, outString);
			break;
		case BIN_XML_TOKEN_NORMAL_SUBSTITUTE:
			{
				BXML_NORMAL_SUBSTITUTION* normalSubstitution = (BXML_NORMAL_SUBSTITUTION*)(streamData_ + inStart);
				wchar_t substitutionText[4096] = {0,};
				wsprintf(substitutionText, _T("Sub#%04X"), normalSubstitution->identifier);
				outString = substitutionText;
				templateSubstitutionMap_.insert(std::make_pair(normalSubstitution->identifier, SUBSTITUTION_INFO(normalSubstitution->valueType, 0)));
				parsedSize = 4;
			}
			break;
		case BIN_XML_TOKEN_OPTIONAL_SUBSTITUTE:
			{
				BXML_OPTIONAL_SUBSTITUTION* optionalSubstitution = (BXML_OPTIONAL_SUBSTITUTION*)(streamData_ + inStart);
				wchar_t substitutionText[4096] = {0,};
				wsprintf(substitutionText, _T("Sub#%04X"), optionalSubstitution->identifier);
				outString = substitutionText;
				templateSubstitutionMap_.insert(std::make_pair(optionalSubstitution->identifier, SUBSTITUTION_INFO(optionalSubstitution->valueType, 1)));
				parsedSize = 4;
			}
			parsedSize = 4;
			break;
		case BIN_XML_TOKEN_FRAGMENT_HEADER:
			parsedSize = 4;
			break;
		default:
			break;
	}

	return parsedSize;
}

/**
	@brief 바이너리 xml 문자열을 파싱한다(유니코드)

	@param inStart 문자열의 시작점
	@param inEnd 문자열의 끝
	@param outString 파싱된 문자열

	@return 파싱된 사이즈
*/
uint32_t BinXmlStream::parseString(uint32_t inStart, uint32_t inEnd, std::wstring& outString)
{
	wchar_t unicodeString[EVTX_CHUNK_SIZE] = {0,};
	uint32_t numChars = *((uint16_t*)(streamData_ + inStart));

	outString = _T("");

	if (inStart + numChars * 2 >= inEnd) { return 0; }

	wcsncpy_s(unicodeString, (const wchar_t*)(streamData_ + inStart + 2), numChars);

	outString = unicodeString;

	return numChars * 2 + 2;
}

/**
	@brief 바이너리 xml 문자열을 파싱한다(유니코드 / 크기지정)

	@param inStart 문자열의 시작점
	@param inEnd 문자열의 끝
	@param inSize 문자열의 크기(바이트)
	@param outString 파싱된 문자열

	@return 파싱된 사이즈
*/
uint32_t BinXmlStream::parseStringWithSize(uint32_t inStart, uint32_t inEnd, uint32_t inSize, std::wstring& outString)
{
	wchar_t unicodeString[EVTX_CHUNK_SIZE] = {0,};
	uint32_t numChars = inSize / 2;

	outString = _T("");

	if (inStart + numChars * 2 >= inEnd) { return 0; }

	wcsncpy_s(unicodeString, (const wchar_t*)(streamData_ + inStart), numChars);

	outString = unicodeString;

	return numChars * 2;
}

/**
	@brief 바이너리 xml 문자열을 파싱한다(아스키)

	@param inStart 문자열의 시작점
	@param inEnd 문자열의 끝
	@param outString 파싱된 문자열

	@return 파싱된 사이즈
*/
uint32_t BinXmlStream::parseAnsiString(uint32_t inStart, uint32_t inEnd, std::wstring& outString)
{
	uint32_t numChars = *((uint16_t*)(streamData_ + inStart));

	outString = _T("");

	if (inStart + numChars >= inEnd) { return 0; }

	outString.assign((uint8_t *)(streamData_ + inStart + 2), (uint8_t *)(streamData_ + inStart + 2 + numChars));

	return numChars + 2;
}

/**
	@brief 바이너리 xml 문자열을 파싱한다(아스키 / 크기지정)

	@param inStart 문자열의 시작점
	@param inEnd 문자열의 끝
	@param inSize 문자열의 크기(바이트)
	@param outString 파싱된 문자열

	@return 파싱된 사이즈
*/
uint32_t BinXmlStream::parseAnsiStringWithSize(uint32_t inStart, uint32_t inEnd, uint32_t inSize, std::wstring& outString)
{
	uint32_t numChars = inSize / 2;

	outString = _T("");

	if (inStart + numChars >= inEnd) { return 0; }

	outString.assign((uint8_t *)(streamData_ + inStart), (uint8_t *)(streamData_ + inStart + numChars));

	return numChars;
}

/**
	@brief 특정 위치가 바이너리 xml의 이름이지 여부를 확인한다

	@param inStart 확인할 지점의 시작
	@param inEnd 확인할 지점의 끝

	@return 바이너리 xml의 이름인지 여부
*/
bool BinXmlStream::isBinXmlName(uint32_t inStart, uint32_t inEnd)
{
	BXML_NAME* bxmlName = (BXML_NAME*)(streamData_ + inStart);
	uint16_t* stringData = (uint16_t*)(streamData_ + inStart + 8);
	bool result = false;
	uint16_t calcedHash = 0;
	uint32_t i = 0;

	if ((inStart + 8 + bxmlName->numChars < inEnd && stringData[bxmlName->numChars] == 0))
	{
		for (i = 0; i < bxmlName->numChars; ++i)
		{
			calcedHash = calcedHash * 65599 + stringData[i];
		}

		if (calcedHash == bxmlName->nameHash) { result = true; }
	}

	return result;
}

/**
	@brief 특정 위치에 있는 바이너리 xml 이름을 얻어온다

	@param inStart 얻어올 지점의 시작
	@param inEnd 얻어올 지점의 끝
	@param outName 얻어온 이름

	@return 이름의 크기(바이트, 널문자 포함)
*/
uint32_t BinXmlStream::getBinXmlName(uint32_t inStart, uint32_t inEnd, std::wstring& outName)
{
	uint32_t parsedSize = 0;
	outName = _T("");
	BXML_NAME* bxmlName = (BXML_NAME*)(streamData_ + inStart);

	if (isBinXmlName(inStart, inEnd))
	{
		wchar_t xmlName[4096] = {0,};

		wcscpy_s(xmlName, 4096, (wchar_t*)(streamData_ + inStart + 8));

		outName = xmlName;
	}

	return 8 + (bxmlName->numChars + 1) * 2;
}

/**
	@brief 바이너리 xml 엘리먼트 하나를 파싱한다

	@param inStart 파싱의 시작점
	@param inEnd 파싱의 끝점
	@param outString 파싱한 엘리먼트의 문자열

	@return 파싱한 사이즈
*/
uint32_t BinXmlStream::parseElement(uint32_t inStart, uint32_t inEnd, std::wstring& outString)
{
	BXML_ELEMENT_START* elementStart = (BXML_ELEMENT_START*)(streamData_ + inStart);
	uint32_t parsedAttributeSize = 0;
	uint32_t nameSize = 0;
	uint32_t attributeListSize = 0;
	uint32_t attributeSize = 0;
	std::wstring elementName = _T("");
	std::wstring childString = _T("");
	std::wstring attributeString = _T("");
	uint32_t parsedSize = 0;
	uint8_t preamble = streamData_[inStart];


	if (inStart + 11 < inEnd && isBinXmlName(inStart + 11, inEnd))
	{
		nameSize = getBinXmlName(inStart + 11, inEnd, elementName);

		if (processType_ == BIN_XML_PROCESS_COLLECT_NAME)
		{
			BinXmlNameCollection::getInstance()->insertName(recordId_, elementStart->elementNameOffset, elementName);
		}
	}
	else
	{
		if (!BinXmlNameCollection::getInstance()->getName(recordId_, elementStart->elementNameOffset, elementName))
		{
			elementName = _T("");
		}
	}

	if (elementName == _T(""))
	{
		elementName = _T("Element");
	}

	elementNameStack_.push(elementName);

	if ((preamble & 0x40) == 0x40)
	{
		if (inStart + 11 + nameSize + 4 < inEnd)
		{
			attributeListSize = *((uint32_t*)(streamData_ + inStart + 11 + nameSize));
		}
	}
	else
	{
		attributeListSize = 0;
	}

	outString = std::wstring(_T("<")) + elementName;

	if (11 + nameSize + 4 + attributeListSize < inEnd)
	{
		for (parsedAttributeSize = 0; parsedAttributeSize < attributeListSize;)
		{
			attributeSize = parseToken(inStart + 11 + nameSize + 4 + parsedAttributeSize, 
				inStart + 11 + nameSize + 4 + attributeListSize, childString);

			attributeString += std::wstring(_T(" ")) + childString;

			if (attributeSize == 0) { break; }

			parsedAttributeSize += attributeSize;
		}
	}

	if (attributeListSize != 0 && attributeSize == 0)
	{
		parsedSize = 0;
	}
	else if ((preamble & 0x40) == 0x40)
	{
		parsedSize = 11 + nameSize + 4 + attributeListSize;
	}
	else
	{
		parsedSize = 11 + nameSize;
	}

	outString = outString + attributeString;

	return parsedSize;
}

/**
	@brief 바이너리 xml 속성 하나를 파싱한다

	@param inStart 파싱의 시작점
	@param inEnd 파싱의 끝점
	@param outString 파싱한 속성의 문자열

	@return 파싱한 사이즈
*/
uint32_t BinXmlStream::parseAttribute(uint32_t inStart, uint32_t inEnd, std::wstring& outString)
{
	std::wstring attributeName = _T("");
	std::wstring attributeValue = _T("");
	int attributeNameSize = 0;
	int attributeValueSize = 0;
	wchar_t attributeText[4096] = {0,};
	BXML_ATTRIBUTE* attributeHeader = (BXML_ATTRIBUTE*)(streamData_ + inStart);
	uint32_t parsedSize = 0;

	if (isBinXmlName(inStart + 5, inEnd))
	{
		attributeNameSize = getBinXmlName(inStart + 5, inEnd, attributeName);

		if (processType_ == BIN_XML_PROCESS_COLLECT_NAME)
		{
			BinXmlNameCollection::getInstance()->insertName(recordId_, attributeHeader->nameOffset, attributeName);
		}
	}
	else
	{
		if (!BinXmlNameCollection::getInstance()->getName(recordId_, attributeHeader->nameOffset, attributeName))
		{
			attributeName = _T("");
		}
	}

	attributeValueSize = parseToken(inStart + 5 + attributeNameSize, inEnd, attributeValue);

	if (attributeName == _T(""))
	{
		attributeName = _T("Attribute");
	}

	wsprintf(attributeText, _T("%s=\"%s\""), attributeName.c_str(), attributeValue.c_str());

	outString = attributeText;

	parsedSize = 5 + attributeNameSize + attributeValueSize;

	return parsedSize;
}

/**
	@brief 바이너리 xml 템플릿 하나를 파싱한다

	@param inStart 파싱의 시작점
	@param inEnd 파싱의 끝점
	@param outString 파싱한 템플릿의 문자열

	@return 파싱한 사이즈
*/
uint32_t BinXmlStream::parseTemplate(uint32_t inStart, uint32_t inEnd, std::wstring& outString)
{
	uint32_t definitionLength = 0;
	uint32_t templateInstances = 0;
	uint32_t instanceLength = 0;
	uint32_t instanceDataStart = NULL;
	uint32_t instanceDataPos = 0;
	std::wstring templateDefinitionString = _T("");
	std::wstring templateInstanceString = _T("");
	std::vector<std::wstring> templateInstanceArray;
	BinXmlTemplate xmlTemplate;
	uint32_t parsedSize = 0;
	uint32_t i = 0;

	BXML_TEMPLATE_DEFINITION* templateDefinition = (BXML_TEMPLATE_DEFINITION*)(streamData_ + inStart + 1);
	BXML_TEMPLATE_VALUE_DESCRIPTOR* templateValueDescriptor = NULL;

	if (templateDefinition->templateIdentifier == templateDefinition->guid1 && templateDefinition->fragment == 0x0001010F)
	{
		definitionLength = 33 + templateDefinition->dataSize;

		BinXmlStream templateStream(streamData_ + inStart + 34, templateDefinition->dataSize, recordId_);

		templateDefinitionString = templateStream.parse();

		if (processType_ == BIN_XML_PROCESS_COLLECT_TEMPLATE)
		{
			BinXmlTemplateCollection::getInstance()->insertTemplate(recordId_, templateDefinition->templateIdentifier, 
				templateStream.getTemplateSubstitutionMap(), templateDefinitionString);
		}
	}
	else
	{
		definitionLength = 9;
	}

	templateInstances = *((uint32_t*)(streamData_ + 1 + inStart + definitionLength));

	instanceLength += 4 + 4 * templateInstances;

	templateValueDescriptor = (BXML_TEMPLATE_VALUE_DESCRIPTOR*)(streamData_ + inStart + 1 + definitionLength + 4);
	instanceDataStart = (inStart + 1 + definitionLength + 4 + 4 * templateInstances);

	TemplateInstanceInfos templateInstanceInfos;

	for (i = 0; i < templateInstances; ++i)
	{
		templateInstanceInfos.push_back(SUBSTITUTION_INFO(templateValueDescriptor[i].valueType, 0));

		if (templateValueDescriptor[i].valueType == BIN_XML_VALUE_STRING)
		{
			if (parseStringWithSize(instanceDataStart + instanceDataPos, inEnd, templateValueDescriptor[i].valueSize, templateInstanceString) == 0)
			{
				break;
			}
		}
		else if (templateValueDescriptor[i].valueType == BIN_XML_VALUE_ANSI_STRING)
		{
			if (parseAnsiStringWithSize(instanceDataStart + instanceDataPos, inEnd, templateValueDescriptor[i].valueSize, templateInstanceString) == 0)
			{
				break;
			}
		}
		else
		{
			if (parseValue(templateValueDescriptor[i].valueType, instanceDataStart + instanceDataPos, inEnd,
				templateValueDescriptor[i].valueSize, templateInstanceString) == 0)
			{
				break;
			}
		}

		templateInstanceArray.push_back(templateInstanceString);

		instanceDataPos += templateValueDescriptor[i].valueSize;
	}

	if (BinXmlTemplateCollection::getInstance()->getTemplate(recordId_, templateDefinition->templateIdentifier, templateInstanceInfos, xmlTemplate))
	{
		outString = xmlTemplate.createTemplateInstance(templateInstanceArray);
	}
	else
	{
		if (processType_ == BIN_XML_PROCESS_NORMAL)
		{
			outString = _T("<templateInstance>");
			for (i = 0; i < templateInstanceArray.size(); ++i)
			{
				outString += _T("<data>");
				outString += templateInstanceArray[i];
				outString += _T("</data>");
			}
			outString += _T("</templateInstance>");
		}
	}

	parsedSize = 1 + definitionLength + instanceLength + instanceDataPos;

	return parsedSize;
}

/**
	@brief 바이너리 xml 값 하나를 파싱한다

	@param valueType 값의 타입
	@param inStart 파싱의 시작점
	@param inEnd 파싱의 끝점
	@param inValueSize 값의 크기(바이트)
	@param outString 파싱한 값의 문자열

	@return 파싱한 사이즈
*/
uint32_t BinXmlStream::parseValue(int valueType, uint32_t inStart, uint32_t inEnd, uint32_t inValueSize, std::wstring& outString)
{
	wchar_t stringBuffer[EVTX_CHUNK_SIZE] = {0,};
	uint32_t parsedSize = 0;
	outString = _T("");

	if ((valueType & 0x80) == 0x80)
	{
		return parseArray(valueType, inStart, inEnd, inValueSize, outString);
	}

	switch(valueType)
	{
		case BIN_XML_VALUE_NULL:
			outString = _T("");
			parsedSize = 2;
			break;
		case BIN_XML_VALUE_STRING:
			if (inStart < inEnd)
			{
				parsedSize = parseString(inStart, inEnd, outString);
			}
			break;
		case BIN_XML_VALUE_ANSI_STRING:
			if (inStart < inEnd)
			{
				parsedSize = parseAnsiString(inStart, inEnd, outString);
			}
			break;
		case BIN_XML_VALUE_INT_8:
			if (inStart + 1 <= inEnd)
			{
				wsprintf(stringBuffer, _T("%d"), *((int8_t*)(streamData_ + inStart)));
				outString = stringBuffer;
			}
			parsedSize = 1;
			break;
		case BIN_XML_VALUE_UINT_8:
			parsedSize = 1;
			if (inStart + 1 <= inEnd)
			{
				wsprintf(stringBuffer, _T("%u"), *((uint8_t*)(streamData_ + inStart)));
				outString = stringBuffer;
			}
			break;
		case BIN_XML_VALUE_INT_16:
			if (inStart + 2 <= inEnd)
			{
				wsprintf(stringBuffer, _T("%d"), *((int16_t*)(streamData_ + inStart)));
				outString = stringBuffer;
			}
			parsedSize = 2;
			break;
		case BIN_XML_VALUE_UINT_16:
			if (inStart + 2 <= inEnd)
			{
				wsprintf(stringBuffer, _T("%u"), *((uint16_t*)(streamData_ + inStart)));
				outString = stringBuffer;
			}
			parsedSize = 2;
			break;
		case BIN_XML_VALUE_INT_32:
			if (inStart + 4 <= inEnd)
			{
				wsprintf(stringBuffer, _T("%ld"), *((int32_t*)(streamData_ + inStart)));
				outString = stringBuffer;
			}
			parsedSize = 4;
			break;
		case BIN_XML_VALUE_UINT_32:
			if (inStart + 4 <= inEnd)
			{
				wsprintf(stringBuffer, _T("%lu"), *((uint32_t*)(streamData_ + inStart)));
				outString = stringBuffer;
			}
			parsedSize = 4;
			break;
		case BIN_XML_VALUE_INT_64:
			if (inStart + 8 <= inEnd)
			{
				swprintf(stringBuffer, _T("%lld"), *((int64_t*)(streamData_ + inStart)));
				outString = stringBuffer;
			}
			parsedSize = 8;
			break;
		case BIN_XML_VALUE_UINT_64:
			if (inStart + 8 <= inEnd)
			{
				swprintf(stringBuffer, _T("%llu"), *((uint64_t*)(streamData_ + inStart)));
				outString = stringBuffer;
			}
			parsedSize = 8;
			break;
		case BIN_XML_VALUE_REAL_32:
			if (inStart + 4 <= inEnd)
			{
				swprintf(stringBuffer, _T("%f"), *((float*)(streamData_ + inStart)));
				outString = stringBuffer;
			}
			parsedSize = 4;
			break;
		case BIN_XML_VALUE_REAL_64:
			if (inStart + 8 <= inEnd)
			{
				swprintf(stringBuffer, _T("%lf"), *((long double*)(streamData_ + inStart)));
				outString = stringBuffer;
			}
			parsedSize = 8;
			break;
		case BIN_XML_VALUE_BOOL:
			if (inStart + 4 <= inEnd)
			{
				if (*((int32_t*)(streamData_ + inStart)) == 0)
				{
					outString = _T("False");
				}
				else
				{
					outString = _T("True");
				}
			}
			parsedSize = 4;
			break;
		case BIN_XML_VALUE_BINARY:
			{
				wchar_t binaryStringBuffer[16] = {0,};
				std::wstring binaryString;
				uint32_t binaryIndex = 0;
				
				outString = _T("");

				for (binaryIndex = 0; (binaryIndex < inValueSize) && (inStart + binaryIndex < inEnd); ++binaryIndex)
				{
					wsprintf(binaryStringBuffer, _T("%02X"), streamData_[inStart + binaryIndex]);
					binaryString = binaryStringBuffer;
					outString += binaryString;
				}

				parsedSize = binaryIndex;
			}
			break;
		case BIN_XML_VALUE_GUID:
			if (inStart + 16 <= inEnd)
			{
				uint8_t* guidBuffer = (uint8_t*)(streamData_ + inStart);

				wsprintf(stringBuffer, _T("{%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
					guidBuffer[3], guidBuffer[2], guidBuffer[1], guidBuffer[0],
					guidBuffer[5], guidBuffer[4],
					guidBuffer[7], guidBuffer[6],
					guidBuffer[8], guidBuffer[9],
					guidBuffer[10], guidBuffer[11], guidBuffer[12], guidBuffer[13], guidBuffer[14], guidBuffer[15]);
				outString = stringBuffer;
			}
			parsedSize = 16;
			break;
		case BIN_XML_VALUE_SIZE_T:
			parsedSize = 8;
			break;
		case BIN_XML_VALUE_FILE_TIME:
			if (inStart + 8 <= inEnd)
			{
				FILETIME* fileTime = (FILETIME*)(streamData_ + inStart);
				SYSTEMTIME systemTime;

				FileTimeToSystemTime(fileTime, &systemTime);

				wsprintf(stringBuffer, _T("%04d-%02d-%02d %02d:%02d:%02d.%d"), 
					systemTime.wYear, systemTime.wMonth, systemTime.wDay,
					systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);

				outString = stringBuffer;
			}
			parsedSize = 8;
			break;
		case BIN_XML_VALUE_SYS_TIME:
			if (inStart + 16 <= inEnd)
			{
				uint16_t* systemTime = (uint16_t*)(streamData_ + inStart);

				wsprintf(stringBuffer, _T("%04d-%02d-%02d %02d:%02d:%02d.%d"), 
					systemTime[0], systemTime[1], systemTime[3], systemTime[4], systemTime[5], 
					systemTime[6], systemTime[7]);

				outString = stringBuffer;
			}
			parsedSize = 16;
			break;
		case BIN_XML_VALUE_SID:
			if (inStart + 68 <= inEnd)
			{
				LPTSTR stringSid = NULL;

				ConvertSidToStringSid((PSID)(streamData_ + inStart), &stringSid);
				if (stringSid != NULL)
				{
					outString = stringSid;
				}
				else
				{
					outString = _T("UnknownSid");
				}

				LocalFree(stringSid);
			}
			parsedSize = 68;
			break;
		case BIN_XML_VALUE_HEX_INT32:
			if (inStart + 4 <= inEnd)
			{
				wsprintf(stringBuffer, _T("0x%X"), *((uint32_t*)(streamData_ + inStart)));
				outString = stringBuffer;
			}
			parsedSize = 4;
			break;
		case BIN_XML_VALUE_HEX_INT64:
			if (inStart + 8 <= inEnd)
			{
				swprintf(stringBuffer, _T("0x%llx"), *((uint64_t*)(streamData_ + inStart)));
				outString = stringBuffer;
			}
			parsedSize = 8;
			break;
		case BIN_XML_VALUE_EVT_HANDLE:
			break;
		case BIN_XML_VALUE_BIN_XML:
			{
				BinXmlStream binXmlStream(streamData_ + inStart, inEnd - inStart, recordId_, processType_);
				outString = binXmlStream.parse(&parsedSize);
			}
			break;
		case BIN_XML_VALUE_EVT_XML:
			break;
		default:
			break;
	}

	return parsedSize;
}

/**
	@brief 바이너리 xml 값 배열을 파싱한다

	@param valueType 값의 타입
	@param inStart 파싱의 시작점
	@param inEnd 파싱의 끝점
	@param inValueSize 배열의 크기(바이트)
	@param outString 파싱한 값의 문자열

	@return 파싱한 사이즈
*/
uint32_t BinXmlStream::parseArray(int valueType, uint32_t inStart, uint32_t inEnd, uint32_t inValueSize, std::wstring& outString)
{
	wchar_t stringBuffer[EVTX_CHUNK_SIZE] = {0,};
	uint32_t parsedSize = 0;
	uint32_t parsePos = 0;
	std::wstring tempString = _T("");
	outString = _T("");

	switch(valueType)
	{
		case BIN_XML_VALUE_ARRAY_STRING:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos * 2 + inStart + 2 <= inEnd && parsePos * 2 < inValueSize; ++parsePos)
				{
					if (((uint16_t*)(streamData_ + inStart))[parsePos] == 0)
					{
						outString = outString + _T("<data>") + tempString + _T("</data>");
						tempString = _T("");
					}
					else
					{
						stringBuffer[0] = ((uint16_t*)(streamData_ + inStart))[parsePos];
						stringBuffer[1] = 0;
						tempString.append(stringBuffer);
					}
				}

				parsedSize = parsePos * 2;
			}
			break;
		case BIN_XML_VALUE_ARRAY_ANSI_STRING:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 1 <= inEnd && parsePos < inValueSize; ++parsePos)
				{
					if ((streamData_ + inStart)[parsePos] == 0)
					{
						outString = outString + _T("<data>") + tempString + _T("</data>");
						tempString = _T("");
					}
					else
					{
						stringBuffer[0] = (streamData_ + inStart)[parsePos];
						stringBuffer[1] = 0;
						tempString.append(stringBuffer);
					}
				}

				parsedSize = parsePos;
			}
			break;
		case BIN_XML_VALUE_ARRAY_INT_8:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 1 <= inEnd && parsePos < inValueSize; ++parsePos)
				{
					wsprintf(stringBuffer, _T("%d"), *((int8_t*)(streamData_ + inStart + parsePos)));
					outString = outString + _T("<data>") + stringBuffer + _T("</data>");
				}
				parsedSize = parsePos;
			}
			break;
		case BIN_XML_VALUE_ARRAY_UINT_8:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 1 <= inEnd && parsePos < inValueSize; ++parsePos)
				{
					wsprintf(stringBuffer, _T("%u"), *((int8_t*)(streamData_ + inStart + parsePos)));
					outString = outString + _T("<data>") + stringBuffer + _T("</data>");
				}
				parsedSize = parsePos;
			}
			break;
		case BIN_XML_VALUE_ARRAY_INT_16:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 2 <= inEnd && parsePos < inValueSize; parsePos += 2)
				{
					wsprintf(stringBuffer, _T("%d"), *((int16_t*)(streamData_ + inStart + parsePos)));
					outString = outString + _T("<data>") + stringBuffer + _T("</data>");
				}
				parsedSize = parsePos;
			}
			break;
		case BIN_XML_VALUE_ARRAY_UINT_16:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 2 <= inEnd && parsePos < inValueSize; parsePos += 2)
				{
					wsprintf(stringBuffer, _T("%u"), *((uint16_t*)(streamData_ + inStart + parsePos)));
					outString = outString + _T("<data>") + stringBuffer + _T("</data>");
				}
				parsedSize = parsePos;
			}
			break;
		case BIN_XML_VALUE_ARRAY_INT_32:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 4 <= inEnd && parsePos < inValueSize; parsePos += 4)
				{
					wsprintf(stringBuffer, _T("%ld"), *((int32_t*)(streamData_ + inStart + parsePos)));
					outString = outString + _T("<data>") + stringBuffer + _T("</data>");
				}
				parsedSize = parsePos;
			}
			break;
		case BIN_XML_VALUE_ARRAY_UINT_32:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 4 <= inEnd && parsePos < inValueSize; parsePos += 4)
				{
					wsprintf(stringBuffer, _T("%lu"), *((uint32_t*)(streamData_ + inStart + parsePos)));
					outString = outString + _T("<data>") + stringBuffer + _T("</data>");
				}
				parsedSize = parsePos;
			}
			break;
		case BIN_XML_VALUE_ARRAY_INT_64:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 8 <= inEnd && parsePos < inValueSize; parsePos += 8)
				{
					swprintf(stringBuffer, _T("%lld"), *((int64_t*)(streamData_ + inStart + parsePos)));
					outString = outString + _T("<data>") + stringBuffer + _T("</data>");
				}
				parsedSize = parsePos;
			}
			break;
		case BIN_XML_VALUE_ARRAY_UINT_64:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 8 <= inEnd && parsePos < inValueSize; parsePos += 8)
				{
					swprintf(stringBuffer, _T("%llu"), *((int64_t*)(streamData_ + inStart + parsePos)));
					outString = outString + _T("<data>") + stringBuffer + _T("</data>");
				}
				parsedSize = parsePos;
			}
			break;
		case BIN_XML_VALUE_ARRAY_REAL_32:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 4 <= inEnd && parsePos < inValueSize; parsePos += 4)
				{
					swprintf(stringBuffer, _T("%f"), *((float*)(streamData_ + inStart + parsePos)));
					outString = outString + _T("<data>") + stringBuffer + _T("</data>");
				}
				parsedSize = parsePos;
			}
			break;
		case BIN_XML_VALUE_ARRAY_REAL_64:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 8 <= inEnd && parsePos < inValueSize; parsePos += 8)
				{
					swprintf(stringBuffer, _T("%lf"), *((long double*)(streamData_ + inStart + parsePos)));
					outString = outString + _T("<data>") + stringBuffer + _T("</data>");
				}
				parsedSize = parsePos;
			}
			break;
		case BIN_XML_VALUE_ARRAY_BOOL:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 4 <= inEnd && parsePos < inValueSize; parsePos += 4)
				{
					if (*((int32_t*)(streamData_ + inStart + parsePos)) == 0)
					{
						outString += _T("<data>False</data>");
					}
					else
					{
						outString += _T("<data>True</data>");
					}
				}
				parsedSize = parsePos;
			}
			break;
		case BIN_XML_VALUE_ARRAY_GUID:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 16 <= inEnd && parsePos < inValueSize; parsePos += 16)
				{
					uint8_t* guidBuffer = (uint8_t*)(streamData_ + inStart + parsePos);

					wsprintf(stringBuffer, _T("{%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
						guidBuffer[3], guidBuffer[2], guidBuffer[1], guidBuffer[0],
						guidBuffer[5], guidBuffer[4],
						guidBuffer[7], guidBuffer[6],
						guidBuffer[8], guidBuffer[9],
						guidBuffer[10], guidBuffer[11], guidBuffer[12], guidBuffer[13], guidBuffer[14], guidBuffer[15]);
					outString = outString + _T("<data>") + stringBuffer + _T("</data>");
				}
				parsedSize = parsePos;
			}
			break;
		case BIN_XML_VALUE_SIZE_T:
			break;
		case BIN_XML_VALUE_ARRAY_FILE_TIME:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 8 <= inEnd && parsePos < inValueSize; parsePos += 8)
				{
					FILETIME* fileTime = (FILETIME*)(streamData_ + inStart + parsePos);
					SYSTEMTIME systemTime;

					FileTimeToSystemTime(fileTime, &systemTime);

					wsprintf(stringBuffer, _T("%04d-%02d-%02d %02d:%02d:%02d.%d"), 
						systemTime.wYear, systemTime.wMonth, systemTime.wDay,
						systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);

					outString = outString + _T("<data>") + stringBuffer + _T("</data>");
				}
				parsedSize = parsePos;
			}
			break;
		case BIN_XML_VALUE_ARRAY_SYS_TIME:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 16 <= inEnd && parsePos < inValueSize; parsePos += 16)
				{
					uint16_t* systemTime = (uint16_t*)(streamData_ + inStart + parsePos);

					wsprintf(stringBuffer, _T("%04d-%02d-%02d %02d:%02d:%02d.%d"), 
						systemTime[0], systemTime[1], systemTime[3], systemTime[4], systemTime[5], 
						systemTime[6], systemTime[7]);

					outString = outString + _T("<data>") + stringBuffer + _T("</data>");
				}
				parsedSize = parsePos;
			}
			break;
		case BIN_XML_VALUE_ARRAY_SID:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 68 <= inEnd && parsePos < inValueSize; parsePos += 68)
				{
					LPTSTR stringSid = NULL;

					ConvertSidToStringSid((PSID)(streamData_ + inStart + parsePos), &stringSid);
					if (stringSid != NULL)
					{
						outString = outString + _T("<data>") + stringSid + _T("</data>");
					}
					else
					{
						outString = outString + _T("<data>UnknownSID</data>");
					}

					LocalFree(stringSid);
				}
				parsedSize = parsePos;
			}
			break;
		case BIN_XML_VALUE_ARRAY_HEX_INT32:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 4 <= inEnd && parsePos < inValueSize; parsePos += 4)
				{
					wsprintf(stringBuffer, _T("0x%x"), *((uint32_t*)(streamData_ + inStart + parsePos)));
					outString = outString + _T("<data>") + stringBuffer + _T("</data>");
				}
				parsedSize = parsePos;
			}
			break;
		case BIN_XML_VALUE_ARRAY_HEX_INT64:
			if (inStart < inEnd)
			{
				for (parsePos = 0; parsePos + inStart + 8 <= inEnd && parsePos < inValueSize; parsePos += 8)
				{
					swprintf(stringBuffer, _T("0x%llx"), *((uint64_t*)(streamData_ + inStart + parsePos)));
					outString = outString + _T("<data>") + stringBuffer + _T("</data>");
				}
				parsedSize = parsePos;
			}
			break;
		default:
			break;
	}

	return parsedSize;
}