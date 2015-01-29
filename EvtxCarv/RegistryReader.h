/**
	@file RegistryReader.h
	@brief ������Ʈ���� ������ �б����� Ŭ������ ����
*/

#ifndef _REGISTRY_READER_H_
#define _REGISTRY_READER_H_

#include <Windows.h>
#include <string>

/**
	@class RegistryReader
	@brief ������Ʈ���� ������ �б����� Ŭ����
*/
class RegistryReader
{
	public:
		static bool readString(HKEY rootKey, const std::wstring& inRegPath, const std::wstring& inKeyName, std::wstring& outString);
		static bool readString32(HKEY rootKey, const std::wstring& inRegPath, const std::wstring& inKeyName, std::wstring& outString);
		static bool readString64(HKEY rootKey, const std::wstring& inRegPath, const std::wstring& inKeyName, std::wstring& outString);
};

#endif /* _REGISTRY_READER_H_ */