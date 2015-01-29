/**
	@file BinXmlNameCollection.cpp
	@brief ���̳ʸ� XML �� �̸����� ��Ƽ� �����ϴ� Ŭ������ ����
*/

#include "BinXmlNameCollection.h"
#include <tchar.h>
#include <stdio.h>

BinXmlNameCollection::BinXmlNameCollection()
{

}

BinXmlNameCollection* BinXmlNameCollection::getInstance()
{
	static BinXmlNameCollection instance;

	return &instance;
}

/**
	@brief �̸� �ݷ��ǿ� �̸��� ����ִ´�

	@param inRecordId �̸��� ���ǵǾ��ִ� ���ڵ� ���̵�
	@param inNameOffset �̸��� ������
	@param inNameString �̸�
*/
void BinXmlNameCollection::insertName(uint64_t inRecordId, uint32_t inNameOffset, const std::wstring& inNameString)
{
	std::map<uint32_t, std::map<uint64_t, BinXmlName> >::iterator foundName = nameContainer_.find(inNameOffset);

	if (foundName == nameContainer_.end())
	{
		std::map<uint64_t, BinXmlName> newNameList;
		newNameList.insert(std::make_pair(0, BinXmlName(0, inNameString)));

		nameContainer_[inNameOffset] = newNameList;
	}
	else
	{
		if (foundName->second.size() != 0)
		{
			std::wstring currentName = _T("");
			

			if (!getName(inRecordId, inNameOffset, currentName) || (currentName != inNameString))
			{
				if (foundName->second.find(inRecordId) == foundName->second.end())
				{
					foundName->second.insert(std::make_pair(inRecordId, BinXmlName(inRecordId, inNameString)));
				}
			}
		}
		else
		{
			foundName->second.insert(std::make_pair(inRecordId, BinXmlName(inRecordId, inNameString)));
		}
	}
}

/**
	@brief �̸� �ݷ��ǿ��� Ư�� �̸��� ã�´�
	
	@param inRecordId ã�� �̸��� ��� �� ���ڵ� ���̵�
	@param inNameOffset �̸��� ������
	@param outNameString ã�� �̸�

	@return �̸��� ã�Ҵ��� ����
*/
bool BinXmlNameCollection::getName(uint64_t inRecordId, uint32_t inNameOffset, std::wstring& outNameString)
{
	if (nameContainer_.find(inNameOffset) == nameContainer_.end()) { return false; }

	std::map<uint64_t, BinXmlName>::iterator nameIterator = nameContainer_[inNameOffset].begin();
	std::map<uint64_t, BinXmlName>::iterator nextNameIterator = nameIterator;
	std::map<uint64_t, BinXmlName>::iterator nameIteratorEnd = nameContainer_[inNameOffset].end();

	for(; nameIterator != nameIteratorEnd; ++nameIterator)
	{
		nextNameIterator = nameIterator;
		++nextNameIterator;

		if (nextNameIterator == nameIteratorEnd) { break; }

		if (nextNameIterator->first > inRecordId) { break; }
	}

	outNameString = nameIterator->second.getNameString();

	return true;
}