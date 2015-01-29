/**
	@file StringTransform.cpp
	@brief ���ڿ��� ������Ű�� Ŭ������ ����
*/

#include "StringTransform.h"
#include <algorithm>

/**
	@brief ���ڿ� ������ ã�� �ٲٱ�

	@param inSrc ���� ���ڿ�
	@param inOriginal ã�� ���ڿ�
	@param inDest �ٲ� ���ڿ�

	@return ���� ���ڿ����� ã�� �ٲٱ� �� ���
*/
std::wstring StringTransform::replaceString(const std::wstring& inSrc, const std::wstring& inOriginal, const std::wstring& inDest)
{
	std::wstring resultString = inSrc;

	size_t start_pos = 0;
	while((start_pos = resultString.find(inOriginal, start_pos)) != std::string::npos)
	{
		resultString.replace(start_pos, inOriginal.length(), inDest);
		start_pos += inDest.length();
	}

	return resultString;
}

/**
	@brief ���ڿ��� �빮�ڸ� �ҹ��ڷ� �ٲ۴�
	@param inSrc ���� ���ڿ�
	@return �빮�ڰ� �ҹ��ڷ� ��ȯ�� ���ڿ�
*/
std::wstring StringTransform::toLowerCase(const std::wstring &inSrc)
{
	std::wstring result = inSrc;

	std::transform(result.begin(), result.end(), result.begin(), ::tolower);

	return result;
}