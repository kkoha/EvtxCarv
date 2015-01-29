/**
	@file EvtxCarver.h
	@brief EVTXī���� ���õ� ��ɵ��� ��Ƴ��� ���� Ŭ������ ����
*/

#ifndef _EVTX_CARVER_H_
#define _EVTX_CARVER_H_

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
#include "ClusterReassembler.h"
#include <string>

/**
	@class EvtxCarver
	@brief EVTXī���� ���õ� ��ɵ��� ��Ƴ��� ���� Ŭ����
*/
class EvtxCarver
{
	public:
		enum CarveLevel
		{
			CARVE_LEVEL_RECORD = 0,  ///< ���ڵ� ���� ī��
			CARVE_LEVEL_CHUNK = 1,  ///< ûũ ���� ī��
		};

		bool setCarveFile(const std::wstring& inFileName);
		void setDumpPathName(const std::wstring& inDumpPathName);
		void setCarveLevel(CarveLevel inCarveLevel);
		void closeCarveFile();

		void setCarveInfoCallback(CarveInfoCallback inCarveInfoCallback);

		bool startCarving();

	private:
		void sendCarveInfo(const std::wstring& inMessage);

		std::wstring dumpPathName_;  ///< ī���� ������� ������ ������ ���丮
		CarveLevel carveLevel_;  ///< ī���� ����(���� �� ûũ / ���ڵ�)
		BinaryFile carveFile_;  ///< ī���� ����
		EvtxFileCarver fileCarver_;  ///< ���� ī���� ��ü
		EvtxChunkCarver chunkCarver_;  ///< ûũ ī���� ��ü
		EvtxRecordCarver recordCarver_;  ///< ���ڵ� ī���� ��ü

		ClusterReassembler clusterReassembler_;  ///< ������ �����͸� �������ϱ� ���� ��ü

		EvtxChunkCarver reassembledChunkCarver_;  ///< �������� ûũ�� ī���ϱ� ���� ��ü
		EvtxRecordCarver reassembledRecordCarver_;  ///< �������� ���ڵ带 ī���ϱ� ���� ��ü

		CarveInfoCallback carveInfoCallback_;  ///< ī������ ������ ǥ���ϱ� ���� �ݹ�
};

#endif /* _EVTX_CARVER_H_ */