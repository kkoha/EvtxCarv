/**
	@file PeResourceParser.cpp
	@brief PE������ ���ҽ��� �б����� Ŭ������ ����
*/

#include "PeResourceParser.h"
#include <Windows.h>
#include <stdio.h>
#include "StringTransform.h"

/**
	@brief PE���Ϸκ��� �޽��� ���ҽ��� �о�´�

	@param inFileName �о�� ���� �̸�
	@param stringIdentifier �о�� �޽����� ID
	@param outString �о�� �����

	@return ���� ����
*/
bool PeResourceParser::getMessageFromFile(const std::wstring& inFileName, uint32_t stringIdentifier, std::wstring& outString)
{
	std::wstring fileName = StringTransform::toLowerCase(inFileName);
	HMODULE hModule = LoadLibraryEx(fileName.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
	TCHAR* stringBuf = NULL;
	bool succeeded = false;
	outString = _T("");
	DWORD dwFormatFlags = FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER;

	if (hModule == NULL)
	{
		std::wstring programFilesPath = StringTransform::replaceString(fileName, _T(" (x86)"), _T(""));
		hModule = LoadLibraryEx(programFilesPath.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
	}

	if (hModule == NULL)
	{
		std::wstring system32path = StringTransform::replaceString(fileName, _T("syswow64"), _T("system32"));
		hModule = LoadLibraryEx(system32path.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
	}

	if (hModule != NULL)
	{
		if (FormatMessage(dwFormatFlags, hModule, stringIdentifier, 0, (LPWSTR)&stringBuf, 0, NULL))
		{
			outString = stringBuf;

			if (stringBuf != NULL) { LocalFree(stringBuf); }

			succeeded = true;
		}
	}

	FreeLibrary(hModule);

	return succeeded;
}