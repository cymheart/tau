﻿#ifndef _Preset_h_
#define _Preset_h_

#include "TauTypes.h"


namespace tau
{
	class Preset
	{
	public:
		Preset();
		~Preset();

		Region* GetGlobalRegion()
		{
			return globalRegion;
		}

		inline void SetBankNum(int bankSelectMSB, int bankSelectLSB, int instrumentNum)
		{
			this->bankSelectMSB = bankSelectMSB;
			this->bankSelectLSB = bankSelectLSB;
			this->instrumentNum = instrumentNum;
		}

		inline int GetBankKey()
		{
			return (bankSelectMSB << 16 | bankSelectLSB << 8 | instrumentNum);
		}

		vector<InstLinkToPresetRegionInfo>* GetPresetRegionLinkInfoList();

		// 连接一个乐器到一个presetRegion
		Region* LinkInstrument(Instrument* inst);

	public:
		string name;
		int bankSelectMSB = 0;
		int bankSelectLSB = 0;
		int instrumentNum = 0;

		Region* globalRegion;
		vector<InstLinkToPresetRegionInfo>* presetRegionLinkInfoList;
	};
}

#endif
