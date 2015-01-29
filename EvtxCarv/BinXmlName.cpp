/**
	@file BinXmlName.cpp
	@brief 바이너리 XML에서의 이름을 표현하는 클래스의 구현
*/

#include "BinXmlName.h"

#include <tchar.h>

BinXmlName::BinXmlName()
{
	recordId_ = 0;
	nameString_ = _T("");
}

/**
	@brief BinXML이름을 생성한다
	@param inRecordId 레코드 아이디
	@param inNameString 이름
*/
BinXmlName::BinXmlName(uint64_t inRecordId, const std::wstring& inNameString)
{
	recordId_ = inRecordId;
	nameString_ = inNameString;
}

/**
	@brief 레코드 아이디를 세팅한다
	@param inRecordId 레코드 아이디
	@return 없음
*/
void BinXmlName::setRecordId(uint64_t inRecordId)
{
	recordId_ = inRecordId;
}

/**
	@brief 이름을 세팅한다
	@param inNameString 이름
	@return 없음
*/
void BinXmlName::setNameString(const std::wstring& inNameString)
{
	nameString_ = inNameString;
}

/**
	@brief 레코드 아이디를 얻어온다
	@return 레코드ID
*/
uint64_t BinXmlName::getRecordId()
{
	return recordId_;
}

/**
	@brief 이름을 얻어온다
	@return 이름
*/
const std::wstring BinXmlName::getNameString()
{
	return nameString_;
}