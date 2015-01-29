/**
	@file StringTransform.cpp
	@brief 문자열을 변형시키는 클래스의 구현
*/

#include "StringTransform.h"
#include <algorithm>

/**
	@brief 문자열 내에서 찾아 바꾸기

	@param inSrc 원본 문자열
	@param inOriginal 찾을 문자열
	@param inDest 바꿀 문자열

	@return 원본 문자열에서 찾아 바꾸기 된 결과
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
	@brief 문자열의 대문자를 소문자로 바꾼다
	@param inSrc 원본 문자열
	@return 대문자가 소문자로 변환된 문자열
*/
std::wstring StringTransform::toLowerCase(const std::wstring &inSrc)
{
	std::wstring result = inSrc;

	std::transform(result.begin(), result.end(), result.begin(), ::tolower);

	return result;
}