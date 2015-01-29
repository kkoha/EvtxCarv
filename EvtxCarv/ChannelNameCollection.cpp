/**
	@file ChannelNameCollection.cpp
	@brief ä���� �̸��� ��Ƴ��� Ŭ������ ����
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
	@brief ä�� �̸��� �÷��ǿ� �ִ´�
	@param inChannelName ä�� �̸�
*/
void ChannelNameCollection::insertChannelName(const std::wstring& inChannelName)
{
	channelNames_.insert(inChannelName);
}

/**
	@brief �־��� ��Ʈ������ ä�� �̸��� ã�´�
	@param inString ä�� �̸��� ã�� ��Ʈ��
*/
std::wstring ChannelNameCollection::getChannelName(const std::wstring& inString)
{
	std::set<std::wstring>::iterator channelNameIterator;
	std::wstring findString = _T("");
	uint32_t findPos = 0;
	uint32_t minFindPos = 99999;
	std::wstring minFindString  = _T("");

	// �̹� �ִ� �̸��鿡�� ã�ƺ���
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

	// ������ �־��� ��Ʈ���߿� �ִ��� ã�ƺ���
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
