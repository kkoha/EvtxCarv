/**
	@file StringTransform.h
	@brief ���ڿ��� ������Ű�� Ŭ������ ����
*/

#ifndef _STRING_TRANSFORM_H_
#define _STRING_TRANSFORM_H_

#include <string>

/**
	@class StringTransform
	@brief ���ڿ��� ����(ã�ƹٲٱ�/�ҹ���ȭ)�� �ٷ�� Ŭ����
*/
class StringTransform
{
	public:
		static std::wstring replaceString(const std::wstring& inSrc, const std::wstring& inOriginal, const std::wstring& inDest);
		static std::wstring toLowerCase(const std::wstring& inSrc);
};

#endif /* _STRING_TRANSFORM_H_ */