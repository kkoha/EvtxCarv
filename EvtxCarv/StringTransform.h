/**
	@file StringTransform.h
	@brief 문자열을 변형시키는 클래스의 정의
*/

#ifndef _STRING_TRANSFORM_H_
#define _STRING_TRANSFORM_H_

#include <string>

/**
	@class StringTransform
	@brief 문자열의 변형(찾아바꾸기/소문자화)을 다루는 클래스
*/
class StringTransform
{
	public:
		static std::wstring replaceString(const std::wstring& inSrc, const std::wstring& inOriginal, const std::wstring& inDest);
		static std::wstring toLowerCase(const std::wstring& inSrc);
};

#endif /* _STRING_TRANSFORM_H_ */