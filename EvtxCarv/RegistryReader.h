/**
	@file RegistryReader.h
	@brief 레지스트리의 내용을 읽기위한 클래스의 정의
*/

#ifndef _REGISTRY_READER_H_
#define _REGISTRY_READER_H_

#include <Windows.h>
#include <string>

/**
	@class RegistryReader
	@brief 레지스트리의 내용을 읽기위한 클래스
*/
class RegistryReader
{
	public:
		static bool readString(HKEY rootKey, const std::wstring& inRegPath, const std::wstring& inKeyName, std::wstring& outString);
		static bool readString32(HKEY rootKey, const std::wstring& inRegPath, const std::wstring& inKeyName, std::wstring& outString);
		static bool readString64(HKEY rootKey, const std::wstring& inRegPath, const std::wstring& inKeyName, std::wstring& outString);
};

#endif /* _REGISTRY_READER_H_ */