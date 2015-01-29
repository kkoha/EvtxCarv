/**
	@file BinXmlTemplate.cpp
	@brief �����͸� XML�� ���ø��� ����ȭ�� Ŭ������ ����
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
	@brief ���ø��� �����Ѵ�
	@param inRecordId ���ø��� ���ǵ� ���ڵ� ���̵�
	@param inTemplateDefinition ���ø��� ����
*/
BinXmlTemplate::BinXmlTemplate(uint64_t inRecordId, const std::wstring& inTemplateDefinition)
{
	recordId_ = inRecordId;
	templateDefinition_ = inTemplateDefinition;
}

/**
	@brief ���ڵ� ���̵� �����Ѵ�
	@param inRecordId ���ڵ� ���̵�
	@return ����
*/
void BinXmlTemplate::setRecordId(uint64_t inRecordId)
{
	recordId_ = inRecordId;
}

/**
	@brief ���ø� ���Ǹ� �����Ѵ�
	@param inTemplateDefinition ���ø� ���� ��Ʈ��
	@return ����
*/
void BinXmlTemplate::setTemplateDefinition(const std::wstring& inTemplateDefinition)
{
	templateDefinition_ = inTemplateDefinition;
}

/**
	@brief ���ڵ� ���̵� ���´�
	@return ���ڵ� ���̵�
*/
uint64_t BinXmlTemplate::getRecordId()
{
	return recordId_;
}

/**
	@brief ���ø� ���Ǹ� ���´�
	@return ���ø� ���� ���ڿ�
*/
const std::wstring BinXmlTemplate::getTemplateDefinition()
{
	return templateDefinition_;
}

/**
	@brief ���ø��� �ν��Ͻ��� ���ø��� �����Ͽ� ������ xml�� �����
	@param inTemplateArgs ���ø��� �ν��Ͻ� ����
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