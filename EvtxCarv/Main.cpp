/**
	@file Main.cpp
	@brief 메인 프로그램
*/

#include <windows.h>
#include <functional>
#include <stdio.h>
#include <tchar.h>
#include <list>
#include "EvtxChunkHeader.h"
#include "EvtxChunkCarver.h"
#include "EvtxRecordCarver.h"
#include "EvtxFileCarver.h"
#include "BinaryFile.h"
#include "EvtxCarver.h"
#include "ClusterReassembler.h"
#include "RegistryReader.h"
#include "PeResourceParser.h"
#include <locale.h>
#include <time.h>

void CarveInfoReceived(const std::wstring& inInfo)
{
	_tprintf(_T("%s\r"), inInfo.c_str());
}

int _tmain(int argc, TCHAR** argv)
{
	_tprintf(_T("\n"));
	_tprintf(_T("##################################################################\n"));
	_tprintf(_T("#  ______           _             _____                          #\n"));
	_tprintf(_T("# |  ____|         | |           / ____|                         #\n"));
	_tprintf(_T("# | |__    __   __ | |_  __  __ | |        __ _   _ __  __   __  #\n"));
	_tprintf(_T("# |  __|   \\ \\ / / | __| \\ \\/ / | |       / _` | | '__| \\ \\ / /  #\n"));
	_tprintf(_T("# | |____   \\ V /  | |_   >  <  | |____  | (_| | | |     \\ V /   #\n"));
	_tprintf(_T("# |______|   \\_/    \\__| /_/\\_\\  \\_____|  \\__,_| |_|       \\_/   #\n"));
	_tprintf(_T("#                                                           0.13 #\n"));
	_tprintf(_T("#                          by HBK Team                           #\n"));
	_tprintf(_T("#                 https://github.com/kkoha/EvtxCarv              #\n"));
	_tprintf(_T("##################################################################\n"));
                                                            
	clock_t start_time, end_time; 

	std::wstring imageFilePath = _T("");
	std::wstring outputPath = _T("");
	std::wstring option = _T("");
	EvtxCarver::CarveLevel carveLevel = EvtxCarver::CARVE_LEVEL_RECORD;
	EvtxCarver evtxCarver;

	if (argc != 3 && argc != 4)
	{
		_tprintf(_T("Usage : %s (-r|-c) <target_image_path> <output_path>\n"), argv[0]);
		_tprintf(_T("\n"));
		_tprintf(_T("Options\n"));
		_tprintf(_T("    --record   (-r)    : Recover by record\n"));
		_tprintf(_T("    --complete (-c)    : Recover by chunk\n"));
		_tprintf(_T("\n"));

		return -1;
	}

	if (argc == 3)
	{
		imageFilePath = argv[1];
		outputPath = argv[2];
	}
	else if (argc == 4)
	{
		option = argv[1];
		imageFilePath = argv[2];
		outputPath = argv[3];

		if (option == _T("-r") || option == _T("--record"))
		{
			carveLevel = EvtxCarver::CARVE_LEVEL_RECORD;
		}
		else if (option == _T("-c") || option == _T("--complete"))
		{
			carveLevel = EvtxCarver::CARVE_LEVEL_CHUNK;
		}
		else
		{
			_tprintf(_T("Error : Invalid option : %s\n"), option.c_str());

			return -1;
		}
	}

	::CreateDirectory(outputPath.c_str(), NULL);
	start_time = clock(); 

	if (evtxCarver.setCarveFile(imageFilePath))
	{
		evtxCarver.setCarveInfoCallback(CarveInfoReceived);
		evtxCarver.setCarveLevel(carveLevel);
		evtxCarver.setDumpPathName(outputPath);
		evtxCarver.startCarving();
	}
	else
	{
		_tprintf(_T("Error : Open Failed %s\n"), imageFilePath.c_str());
	}
	end_time = clock(); 
	
	_tprintf(_T("Time : %f\n"), ((double)(end_time-start_time)) / CLOCKS_PER_SEC);
	return 0;
}