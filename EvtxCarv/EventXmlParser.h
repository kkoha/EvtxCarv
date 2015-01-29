/**
	@file EventXmlParser.h
	@brief 이벤트 XML로부터 필요한 정보들을 얻는 클래스의 정의
*/

#ifndef _EVENT_XML_PARSER_H_
#define _EVENT_XML_PARSER_H_

#include <string>
#include "rapidxml.hpp"
#include <vector>

#define NORMAL_KEYWORD_START 0x0000000000001000ll
#define WIN_KEYWORD_START 0x0001000000000000ll

/**
	@class EventXmlParser
	@brief 이벤트 XML로부터 필요한 정보들을 얻는 클래스
*/
class EventXmlParser
{
	public:
		EventXmlParser(const std::wstring& inXmlString);

		std::wstring getChannel();
		std::wstring getSource();
		std::wstring getLogTime();
		std::wstring getEventId();
		std::wstring getLevel();
		std::wstring getKeyword();
		std::wstring getComputer();
		std::wstring getMessage();

	private:
		static std::wstring getResourceDllPath(const std::wstring& inProdiverName, const std::wstring& inProviderGuid, const std::wstring& inChannel);
		void buildMessageDatas(rapidxml::xml_node<wchar_t>* rootNode);

		std::wstring channel_;  ///< 얻어온 채널 이름
		std::wstring source_;  ///< 얻어온 이벤트 소스 이름
		std::wstring logTime_;  ///< 얻어온 로그 타임스탬프
		std::wstring eventId_;  ///< 얻어온 이벤트 아이디
		std::wstring level_;  ///< 얻어온 로그 레벨
		std::wstring keyword_;  ///< 얻어온 로그 키워드
		std::wstring computer_;  ///< 얻어온 컴퓨터 이름
		std::wstring message_;  ///< 얻어온 메시지

		std::vector<std::wstring> messageDatas_;  ///< 메시지 값을 만들어내기 위해 들어가는 인자들

		static std::wstring logLevelStrings[16];  ///< 로그 레벨 숫자 => 문자열로 바꾸기 위한 배열
		static std::wstring logKeywordStrings[12];  ///< 로그 키워드 숫자 => 문자열로 바꾸기 위한 배열(일반)
		static std::wstring logKeywordStringsWin[16];  ///< 로그 키워드 숫자 => 문자열로 바꾸기 위한 배열(win)
};

#endif /* _EVENT_XML_PARSER_H_ */