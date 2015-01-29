/**
	@file ComputerNameCollection.cpp
	@brief 컴퓨터의 이름을 모아놓은 클래스의 구현
*/

#include "ComputerNameCollection.h"
#include "StringTransform.h"
#include <tchar.h>
#include "DataType.h"

ComputerNameCollection* ComputerNameCollection::getInstance()
{
	static ComputerNameCollection instance;

	return &instance;
}

/**
	@brief 컴퓨터 이름을 컬렉션에 넣는다
	@param inComputerName 컴퓨터 이름
*/
void ComputerNameCollection::insertComputerName(const std::wstring& inComputerName)
{
	computerNames_.insert(inComputerName);
}

/**
	@brief 주어진 스트링에서 컴퓨터 이름을 찾는다
	@param inString 컴퓨터 이름을 찾을 스트링
*/
std::wstring ComputerNameCollection::getComputerName(const std::wstring& inString)
{
	std::set<std::wstring>::iterator computerNameIterator;
	std::wstring findString = _T("");
	uint32_t findPos = 0;
	uint32_t minFindPos = 99999;
	std::wstring minFindString  = _T("");

	// 이미 있는 이름들에서 찾아본다
	for (computerNameIterator = computerNames_.begin(); computerNameIterator != computerNames_.end(); ++computerNameIterator)
	{
		findString = std::wstring(_T(">")) + *computerNameIterator + std::wstring(_T("</"));
		findPos = inString.find(findString);

		if (findPos != inString.npos)
		{
			if (findPos < minFindPos)
			{
				minFindPos = findPos;
				minFindString = *computerNameIterator;
			}
		}
	}

	// 없으면 주어진 스트링중에 있는지 찾아본다
	if (minFindString == _T(""))
	{
		if (inString.find(_T("<Computer>")) != inString.npos)
		{
			minFindString = inString.substr(inString.find(_T("<Computer>")));

			if (minFindString.find(_T("</Computer>")) != minFindString.npos)
			{
				minFindString = minFindString.substr(0, minFindString.find(_T("</Computer>")));
				minFindString = StringTransform::replaceString(minFindString, _T("<Computer>"), _T(""));
				minFindString = StringTransform::replaceString(minFindString, _T("/"), _T("%4"));
				insertComputerName(minFindString);
			}
			else
			{
				minFindString = _T("");
			}
		}
	}

	return minFindString;
}
