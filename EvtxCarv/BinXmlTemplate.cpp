/**
	@file BinXmlTemplate.cpp
	@brief 바이터리 XML의 템플릿을 구조화한 클래스의 구현
*/

#include "BinXmlTemplate.h"
#include <tchar.h>
#include <windows.h>
#include "StringTransform.h"

BinXmlTemplate::BinXmlTemplate()
{
	recordId_ = 0;
	templateDefinition_ = _T("");
}

/**
	@brief 템플릿을 생성한다
	@param inRecordId 템플릿이 정의된 레코드 아이디
	@param inTemplateDefinition 템플릿의 정의
*/
BinXmlTemplate::BinXmlTemplate(uint64_t inRecordId, const std::wstring& inTemplateDefinition)
{
	recordId_ = inRecordId;
	templateDefinition_ = inTemplateDefinition;
}

/**
	@brief 레코드 아이디를 세팅한다
	@param inRecordId 레코드 아이디
	@return 없음
*/
void BinXmlTemplate::setRecordId(uint64_t inRecordId)
{
	recordId_ = inRecordId;
}

/**
	@brief 템플릿 정의를 세팅한다
	@param inTemplateDefinition 템플릿 정의 스트링
	@return 없음
*/
void BinXmlTemplate::setTemplateDefinition(const std::wstring& inTemplateDefinition)
{
	templateDefinition_ = inTemplateDefinition;
}

/**
	@brief 레코드 아이디를 얻어온다
	@return 레코드 아이디
*/
uint64_t BinXmlTemplate::getRecordId()
{
	return recordId_;
}

/**
	@brief 템플릿 정의를 얻어온다
	@return 템플릿 정의 문자열
*/
const std::wstring BinXmlTemplate::getTemplateDefinition()
{
	return templateDefinition_;
}

/**
	@brief 템플릿의 인스턴스를 템플릿에 적용하여 완전한 xml을 만든다
	@param inTemplateArgs 템플릿의 인스턴스 값들
*/
std::wstring BinXmlTemplate::createTemplateInstance(const std::vector<std::wstring>& inTemplateArgs)
{
	size_t substituteId = 0;
	wchar_t substituteString[4096] = {0,};
	std::wstring resultString = templateDefinition_;

	for (substituteId = 0; substituteId < inTemplateArgs.size(); ++substituteId)
	{
		wsprintf(substituteString, _T("Sub#%04X"), substituteId);
		resultString = StringTransform::replaceString(resultString, substituteString, inTemplateArgs[substituteId]);
	}

	return resultString;
}