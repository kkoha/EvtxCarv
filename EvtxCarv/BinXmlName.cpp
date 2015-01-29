/**
	@file BinXmlName.cpp
	@brief ���̳ʸ� XML������ �̸��� ǥ���ϴ� Ŭ������ ����
*/

#include "BinXmlName.h"

#include <tchar.h>

BinXmlName::BinXmlName()
{
	recordId_ = 0;
	nameString_ = _T("");
}

/**
	@brief BinXML�̸��� �����Ѵ�
	@param inRecordId ���ڵ� ���̵�
	@param inNameString �̸�
*/
BinXmlName::BinXmlName(uint64_t inRecordId, const std::wstring& inNameString)
{
	recordId_ = inRecordId;
	nameString_ = inNameString;
}

/**
	@brief ���ڵ� ���̵� �����Ѵ�
	@param inRecordId ���ڵ� ���̵�
	@return ����
*/
void BinXmlName::setRecordId(uint64_t inRecordId)
{
	recordId_ = inRecordId;
}

/**
	@brief �̸��� �����Ѵ�
	@param inNameString �̸�
	@return ����
*/
void BinXmlName::setNameString(const std::wstring& inNameString)
{
	nameString_ = inNameString;
}

/**
	@brief ���ڵ� ���̵� ���´�
	@return ���ڵ�ID
*/
uint64_t BinXmlName::getRecordId()
{
	return recordId_;
}

/**
	@brief �̸��� ���´�
	@return �̸�
*/
const std::wstring BinXmlName::getNameString()
{
	return nameString_;
}