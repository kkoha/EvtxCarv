/**
	@file ChannelNameCollection.h
	@brief 채널의 이름을 모아놓은 클래스의 정의
*/

#ifndef _CHANNEL_NAME_COLLECTION_H_
#define _CHANNEL_NAME_COLLECTION_H_

#include <string>
#include <set>

/**
	@class ChannelNameCollection
	@brief 채널의 이름을 모아놓은 클래스
*/
class ChannelNameCollection
{
public:
	static ChannelNameCollection* getInstance();

	void insertChannelName(const std::wstring& inChannelName);
	std::wstring getChannelName(const std::wstring& inString);

private:
	std::set<std::wstring> channelNames_;  ///< 채널 이름들의 모음
};

#endif /* _CHANNEL_NAME_COLLECTION_H_ */