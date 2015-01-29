/**
	@file ChannelNameCollection.cpp
	@brief 채널의 이름을 모아놓은 클래스의 구현
*/

#include "ChannelNameCollection.h"
#include "StringTransform.h"
#include <tchar.h>
#include "DataType.h"

ChannelNameCollection* ChannelNameCollection::getInstance()
{
	static ChannelNameCollection instance;

	return &instance;
}

/**
	@brief 채널 이름을 컬렉션에 넣는다
	@param inChannelName 채널 이름
*/
void ChannelNameCollection::insertChannelName(const std::wstring& inChannelName)
{
	channelNames_.insert(inChannelName);
}

/**
	@brief 주어진 스트링에서 채널 이름을 찾는다
	@param inString 채널 이름을 찾을 스트링
*/
std::wstring ChannelNameCollection::getChannelName(const std::wstring& inString)
{
	std::set<std::wstring>::iterator channelNameIterator;
	std::wstring findString = _T("");
	uint32_t findPos = 0;
	uint32_t minFindPos = 99999;
	std::wstring minFindString  = _T("");

	// 이미 있는 이름들에서 찾아본다
	for (channelNameIterator = channelNames_.begin(); channelNameIterator != channelNames_.end(); ++channelNameIterator)
	{
		findString = std::wstring(_T(">")) + *channelNameIterator + std::wstring(_T("</"));
		findPos = inString.find(findString);

		if (findPos != inString.npos)
		{
			if (findPos < minFindPos)
			{
				minFindPos = findPos;
				minFindString = *channelNameIterator;
			}
		}
	}

	// 없으면 주어진 스트링중에 있는지 찾아본다
	if (minFindString == _T(""))
	{
		if (inString.find(_T("<Channel>")) != inString.npos)
		{
			minFindString = inString.substr(inString.find(_T("<Channel>")));

			if (minFindString.find(_T("</Channel>")) != minFindString.npos)
			{
				minFindString = minFindString.substr(0, minFindString.find(_T("</Channel>")));
				minFindString = StringTransform::replaceString(minFindString, _T("<Channel>"), _T(""));
				minFindString = StringTransform::replaceString(minFindString, _T("/"), _T("%4"));
				insertChannelName(StringTransform::toLowerCase(minFindString));
			}
			else
			{
				minFindString = _T("");
			}
		}
	}

	return minFindString;
}
