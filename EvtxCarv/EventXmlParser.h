/**
	@file EventXmlParser.h
	@brief �̺�Ʈ XML�κ��� �ʿ��� �������� ��� Ŭ������ ����
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
	@brief �̺�Ʈ XML�κ��� �ʿ��� �������� ��� Ŭ����
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

		std::wstring channel_;  ///< ���� ä�� �̸�
		std::wstring source_;  ///< ���� �̺�Ʈ �ҽ� �̸�
		std::wstring logTime_;  ///< ���� �α� Ÿ�ӽ�����
		std::wstring eventId_;  ///< ���� �̺�Ʈ ���̵�
		std::wstring level_;  ///< ���� �α� ����
		std::wstring keyword_;  ///< ���� �α� Ű����
		std::wstring computer_;  ///< ���� ��ǻ�� �̸�
		std::wstring message_;  ///< ���� �޽���

		std::vector<std::wstring> messageDatas_;  ///< �޽��� ���� ������ ���� ���� ���ڵ�

		static std::wstring logLevelStrings[16];  ///< �α� ���� ���� => ���ڿ��� �ٲٱ� ���� �迭
		static std::wstring logKeywordStrings[12];  ///< �α� Ű���� ���� => ���ڿ��� �ٲٱ� ���� �迭(�Ϲ�)
		static std::wstring logKeywordStringsWin[16];  ///< �α� Ű���� ���� => ���ڿ��� �ٲٱ� ���� �迭(win)
};

#endif /* _EVENT_XML_PARSER_H_ */