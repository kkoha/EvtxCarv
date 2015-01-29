/**
	@file ChannelNameCollection.h
	@brief ä���� �̸��� ��Ƴ��� Ŭ������ ����
*/

#ifndef _CHANNEL_NAME_COLLECTION_H_
#define _CHANNEL_NAME_COLLECTION_H_

#include <string>
#include <set>

/**
	@class ChannelNameCollection
	@brief ä���� �̸��� ��Ƴ��� Ŭ����
*/
class ChannelNameCollection
{
public:
	static ChannelNameCollection* getInstance();

	void insertChannelName(const std::wstring& inChannelName);
	std::wstring getChannelName(const std::wstring& inString);

private:
	std::set<std::wstring> channelNames_;  ///< ä�� �̸����� ����
};

#endif /* _CHANNEL_NAME_COLLECTION_H_ */