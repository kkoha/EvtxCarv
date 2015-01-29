/**
	@file BinXmlNameCollection.cpp
	@brief 바이너리 XML 의 이름들을 모아서 관리하는 클래스의 구현
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
	@brief 이름 콜렉션에 이름을 집어넣는다

	@param inRecordId 이름이 정의되어있던 레코드 아이디
	@param inNameOffset 이름의 오프셋
	@param inNameString 이름
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
	@brief 이름 콜렉션에서 특정 이름을 찾는다
	
	@param inRecordId 찾은 이름을 사용 할 레코드 아이디
	@param inNameOffset 이름의 오프셋
	@param outNameString 찾은 이름

	@return 이름을 찾았는지 여부
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