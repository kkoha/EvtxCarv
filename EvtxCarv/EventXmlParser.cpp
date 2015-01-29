/**
	@file EventXmlParser.cpp
	@brief �̺�Ʈ XML�κ��� �ʿ��� �������� ��� Ŭ������ ����
*/

#include "EventXmlParser.h"
#include "rapidxml.hpp"
#include "RegistryReader.h"
#include "PeResourceParser.h"
#include "StringTransform.h"

#include <vector>
#include <string>

#include <tchar.h>

#include "DataType.h"

std::wstring EventXmlParser::logLevelStrings[16] = {
	_T("LogAlways"), _T("Critical"), _T("Error"), _T("Warning"), _T("Information"), _T("Verbose"), _T("ReservedLevel6"),
	_T("ReservedLevel7"), _T("ReservedLevel8"), _T("ReservedLevel9"), _T("ReservedLevel10"), _T("ReservedLevel11"),
	_T("ReservedLevel12"), _T("ReservedLevel13"), _T("ReservedLevel14"), _T("ReservedLevel15")
};

std::wstring EventXmlParser::logKeywordStrings[12] = {
	_T("Shell"), _T("Properties"), _T("FileClassStoreAndIconCache"), _T("Controls"), _T("APICalls"), _T("Internet Explorer"),
	_T("ShutdownUX"), _T("CopyEngine"), _T("Tasks"), _T("WDI"), _T("StartupPerf"), _T("StructuredQuery"),
};

std::wstring EventXmlParser::logKeywordStringsWin[16] = {
	_T("win:Reserved"), _T("win:WDIContext"), _T("win:EDIDiag"), _T("win:SQM"), _T("win:AuditFailure"), _T("win:AudirSuccess"),
	_T("win:CorrelationHint"), _T("win:EventlogClassic"), _T("win:ReversedKeyword56"), _T("win:ReversedKeyword57"), _T("win:ReversedKeyword58"),
	_T("win:ReversedKeyword59"), _T("win:ReversedKeyword60"), _T("win:ReversedKeyword61"), _T("win:ReversedKeyword62"), _T("Microsoft-Windows-Shell-Core/Diagnostic")
};

/**
	@brief XML�κ��� �������� �Ľ��� �����Ѵ�
	@param inXmlString XML���ڿ�
*/
EventXmlParser::EventXmlParser(const std::wstring& inXmlString)
{
	rapidxml::xml_node<wchar_t>* rootNode = NULL;
	rapidxml::xml_node<wchar_t>* systemNode = NULL;
	rapidxml::xml_node<wchar_t>* eventDataNode = NULL;

	rapidxml::xml_node<wchar_t>* providerNode = NULL;
	rapidxml::xml_node<wchar_t>* eventIdNode = NULL;
	rapidxml::xml_node<wchar_t>* versionNode = NULL;
	rapidxml::xml_node<wchar_t>* levelNode = NULL;
	rapidxml::xml_node<wchar_t>* taskNode = NULL;
	rapidxml::xml_node<wchar_t>* opCodeNode = NULL;
	rapidxml::xml_node<wchar_t>* keywordNode = NULL;
	rapidxml::xml_node<wchar_t>* timeCreatedNode = NULL;
	rapidxml::xml_node<wchar_t>* eventRecordIdNode = NULL;
	rapidxml::xml_node<wchar_t>* correlationNode = NULL;
	rapidxml::xml_node<wchar_t>* executionNode = NULL;
	rapidxml::xml_node<wchar_t>* channelNode = NULL;
	rapidxml::xml_node<wchar_t>* computerNode = NULL;
	rapidxml::xml_node<wchar_t>* securityNode = NULL;
	rapidxml::xml_node<wchar_t>* unknownNode = NULL;

	messageDatas_.clear();

	std::vector<std::wstring> eventDatas;

	std::wstring providerName = _T("");
	std::wstring providerUuid = _T("");
	std::wstring eventMessage = _T("");
	std::wstring messageDllPath = _T("");

	uint32_t eventQualfier = 0;
	uint32_t eventId = 0;
	uint32_t eventLevel = 0;
	uint64_t keyword = 0;


	channel_ = _T("");
	source_ = _T("");
	logTime_ = _T("");
	eventId_ = _T("");
	level_ = _T("");
	keyword_ = _T("");
	computer_ = _T("");

	TCHAR* messageBuf = NULL;

	messageBuf = new TCHAR[inXmlString.length() + 1];
	wcscpy_s(messageBuf, inXmlString.length() + 1, inXmlString.c_str());

	try
	{
		rapidxml::xml_document<wchar_t> xmlDocument;

		xmlDocument.parse<0>(messageBuf);

		rootNode = xmlDocument.first_node();
		if (rootNode != NULL)
		{
			systemNode = rootNode->first_node();
			eventDataNode = systemNode->next_sibling();
		}

		// <System> ��Ʈ
		if (systemNode != NULL)
		{
			providerNode = systemNode->first_node();

			if (providerNode != NULL)
			{
				rapidxml::xml_attribute<wchar_t>* nameAttribute = NULL;
				rapidxml::xml_attribute<wchar_t>* uuidAttribute = NULL;

				nameAttribute = providerNode->first_attribute();

				if (nameAttribute != NULL)
				{
					source_ = nameAttribute->value();
					providerName = nameAttribute->value();
					uuidAttribute = nameAttribute->next_attribute();
				}
				if (uuidAttribute != NULL)
				{
					providerUuid = uuidAttribute->value();
				}

				eventIdNode = providerNode->next_sibling();
			}
			if (eventIdNode != NULL)
			{
				rapidxml::xml_attribute<wchar_t>* qualifierAttribute = NULL;

				qualifierAttribute = eventIdNode->first_attribute();

				if (qualifierAttribute != NULL)
				{
					eventQualfier = _tcstoul(qualifierAttribute->value(), NULL, 10);
				}

				eventId = _tcstoul(eventIdNode->value(), NULL, 10);
				eventId_ = eventIdNode->value();

				versionNode = eventIdNode->next_sibling();
			}
			if (versionNode != NULL)
			{
				levelNode = versionNode->next_sibling();
			}
			if (levelNode != NULL)
			{
				uint32_t logLevel = _tcstoul(levelNode->value(), NULL, 10);

				if (logLevel < 16)
				{
					level_ = logLevelStrings[logLevel];
				}

				taskNode = levelNode->next_sibling();
			}
			if (taskNode != NULL)
			{
				opCodeNode = taskNode->next_sibling();
			}
			if (opCodeNode != NULL)
			{
				keywordNode = opCodeNode->next_sibling();
			}
			if (keywordNode != NULL)
			{
				if (_tcslen(keywordNode->value()) > 2)
				{
					uint64_t keywordValue = _tcstoui64(keywordNode->value() + 2, NULL, 16);
					uint64_t i = 0;

					for (i = 0; i < 12; ++i)
					{
						if ((keywordValue & (NORMAL_KEYWORD_START << i)) == (NORMAL_KEYWORD_START << i))
						{
							if (keyword_ != _T(""))
							{
								keyword_ += _T("|");
							}

							keyword_ += logKeywordStrings[i];
						}
					}

					for (i = 0; i < 16; ++i)
					{
						if ((keywordValue & (WIN_KEYWORD_START << i)) == (WIN_KEYWORD_START << i))
						{
							if (keyword_ != _T(""))
							{
								keyword_ += _T("|");
							}

							keyword_ += logKeywordStrings[i];
						}
					}
				}

				timeCreatedNode = keywordNode->next_sibling();
			}
			if (timeCreatedNode != NULL)
			{
				logTime_ = timeCreatedNode->value();
				eventRecordIdNode = timeCreatedNode->next_sibling();
			}
			if (eventRecordIdNode != NULL)
			{
				unknownNode = eventRecordIdNode->next_sibling();

				if (unknownNode != NULL)
				{
					if (unknownNode->first_attribute() != NULL)
					{
						correlationNode = unknownNode;
						executionNode = unknownNode->next_sibling();

						if (executionNode != NULL && executionNode->first_attribute() == NULL)
						{
							channelNode = executionNode;
						}
						else if (executionNode != NULL)
						{
							channelNode = executionNode->next_sibling();
						}
					}
					else
					{
						channelNode = unknownNode;
					}
				}
			}
			if (channelNode != NULL)
			{
				channel_ = channelNode->value();
				computerNode = channelNode->next_sibling();
			}
			if (computerNode != NULL)
			{
				computer_ = computerNode->value();
				securityNode = computerNode->next_sibling();
			}
			if (securityNode != NULL)
			{

			}
		}

		// Event Provider �޽��� ���ҽ����� �� ���´�
		messageDllPath = getResourceDllPath(providerName, providerUuid, channel_);

		messageDatas_.clear();

		// <EventData> ��� �Ľ�
		buildMessageDatas(eventDataNode);
		int substitutionIndex = 0;

		if (messageDllPath != _T(""))
		{
			// Event Provider �޽���DLL�κ��� �޽����� �����Ѵ�
			TCHAR substitutionBuf[32] = {0,};

			PeResourceParser::getMessageFromFile(messageDllPath, (eventQualfier << 16) | eventId, message_);

			if (message_ != _T(""))
			{
				for (substitutionIndex = messageDatas_.size() - 1; substitutionIndex > 0; --substitutionIndex)
				{
					wsprintf(substitutionBuf, _T("%%%d"), substitutionIndex);
					message_ = StringTransform::replaceString(message_, substitutionBuf, messageDatas_[substitutionIndex]);
				}
			}
		}

		if (message_ == _T(""))
		{
			substitutionIndex = 0;
			for (substitutionIndex = messageDatas_.size() - 1; substitutionIndex > 0; --substitutionIndex)
			{
				message_ += messageDatas_[substitutionIndex] + _T("\n");
			}
		}
	}
	catch(...)
	{
		// �Ľ��� ����
		;
	}

	if (messageBuf != NULL)
	{
		delete [] messageBuf;
	}
}

/**
	@brief ä�� �̸��� ���´�
	@return ä�� �̸�
*/
std::wstring EventXmlParser::getChannel()
{
	return channel_;
}

/**
	@brief ��ǻ�� �̸��� ���´�
	@return ��ǻ�� �̸�
*/
std::wstring EventXmlParser::getComputer()
{
	return computer_;
}

/**
	@brief �̺�Ʈ ���̵� ���´�
	@return �̺�Ʈ ���̵�
*/
std::wstring EventXmlParser::getEventId()
{
	return eventId_;
}

/**
	@brief �̺�Ʈ Ű���带 ���´�
	@return �̺�Ʈ Ű����
*/
std::wstring EventXmlParser::getKeyword()
{
	return keyword_;
}

/**
	@brief �̺�Ʈ �α� ������ ���´�
	@return �̺�Ʈ �α� ����
*/
std::wstring EventXmlParser::getLevel()
{
	return level_;
}

/**
	@brief �̺�Ʈ �α� Ÿ�ӽ������� ���´�
	@return �̺�Ʈ �α� Ÿ�ӽ�����
*/
std::wstring EventXmlParser::getLogTime()
{
	return logTime_;
}

/**
	@brief �̺�Ʈ �α� �޽����� ���´�
	@return �̺�Ʈ �α� �޽���
*/
std::wstring EventXmlParser::getMessage()
{
	return message_;
}

/**
	@brief �̺�Ʈ �α� �ҽ� �̸��� ���´�
	@return �̺�Ʈ �α� �ҽ� �̸�
*/
std::wstring EventXmlParser::getSource()
{
	return source_;
}

/**
	@brief Event Prodiver�����κ��� �ش��ϴ� �޽��� ���ҽ������� ���´�

	@param inProdiverName ���ι��̴� �̸�
	@param inProviderGuid ���ι��̴� GUID
	@param inChannel �̺�Ʈ�α� ä�� �̸�

	@return ���� ���ҽ� ����
*/
std::wstring EventXmlParser::getResourceDllPath(const std::wstring& inProdiverName, const std::wstring& inProviderGuid, const std::wstring& inChannel)
{
	std::wstring dllPath = _T("");

	if (inProviderGuid != _T(""))
	{
		if (!RegistryReader::readString(HKEY_LOCAL_MACHINE, std::wstring(_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WINEVT\\Publishers\\")) + inProviderGuid,
			_T("MessageFileName"), dllPath))
		{
			RegistryReader::readString(HKEY_LOCAL_MACHINE, std::wstring(_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WINEVT\\Publishers\\")) + inProviderGuid,
				_T("EventMessageFile"), dllPath);
		}
	}
	else
	{
		RegistryReader::readString(HKEY_LOCAL_MACHINE, std::wstring(_T("SYSTEM\\CurrentControlSet\\services\\eventlog\\")) + inChannel + _T("\\") + inProdiverName,
			_T("EventMessageFile"), dllPath);

		if (dllPath == _T(""))
		{
			RegistryReader::readString(HKEY_LOCAL_MACHINE, std::wstring(_T("SYSTEM\\CurrentControlSet\\services\\eventlog\\Application\\")) + inProdiverName,
				_T("EventMessageFile"), dllPath);
		}

		if (dllPath == _T(""))
		{
			RegistryReader::readString(HKEY_LOCAL_MACHINE, std::wstring(_T("SYSTEM\\CurrentControlSet\\services\\eventlog\\Security\\")) + inProdiverName,
				_T("EventMessageFile"), dllPath);
		}

		if (dllPath == _T(""))
		{
			RegistryReader::readString(HKEY_LOCAL_MACHINE, std::wstring(_T("SYSTEM\\CurrentControlSet\\services\\eventlog\\System\\")) + inProdiverName,
				_T("EventMessageFile"), dllPath);
		}
	}

	return dllPath;
}

/**
	@brief <EventData> ���κ��� �޽����� ���� �����͵��� �����
	@param rootNode ������ ���� ���
*/
void EventXmlParser::buildMessageDatas(rapidxml::xml_node<wchar_t>* rootNode)
{
	if (rootNode == NULL) return;

	buildMessageDatas(rootNode->first_node());
	
	while (rootNode != NULL)
	{
		std::wstring nodeValue = _T("");

		nodeValue = rootNode->value();

		if (nodeValue != _T(""))
		{
			messageDatas_.push_back(nodeValue);
		}

		rootNode = rootNode->next_sibling();
	}

}