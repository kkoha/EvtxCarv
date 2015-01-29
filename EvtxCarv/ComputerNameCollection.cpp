/**
	@file ComputerNameCollection.cpp
	@brief ��ǻ���� �̸��� ��Ƴ��� Ŭ������ ����
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
	@brief ��ǻ�� �̸��� �÷��ǿ� �ִ´�
	@param inComputerName ��ǻ�� �̸�
*/
void ComputerNameCollection::insertComputerName(const std::wstring& inComputerName)
{
	computerNames_.insert(inComputerName);
}

/**
	@brief �־��� ��Ʈ������ ��ǻ�� �̸��� ã�´�
	@param inString ��ǻ�� �̸��� ã�� ��Ʈ��
*/
std::wstring ComputerNameCollection::getComputerName(const std::wstring& inString)
{
	std::set<std::wstring>::iterator computerNameIterator;
	std::wstring findString = _T("");
	uint32_t findPos = 0;
	uint32_t minFindPos = 99999;
	std::wstring minFindString  = _T("");

	// �̹� �ִ� �̸��鿡�� ã�ƺ���
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

	// ������ �־��� ��Ʈ���߿� �ִ��� ã�ƺ���
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
