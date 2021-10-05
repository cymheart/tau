#ifndef _DsignalTypes_h_
#define _DsignalTypes_h_

namespace dsignal
{


	//Freqz����������Ϣ
	struct FreqzSampleRegionInfo
	{
		//��Ƶ���Ͳ�������
		int lowFreqTypeSampleCount = 0;
		//��Ƶ������ʼ���
		int lowFreqStartSampleIdx = 0;
		//��Ƶʵ�ʲ�������
		int lowFreqSampleCount = 0;
		//��Ƶ��ʼλ��Ƶ��
		float lowFreqStartHz = 0;
		//��Ƶ����λ��Ƶ��
		float lowFreqEndHz = 0;
		//��Ƶ����ֵ
		float perLowFreqHz = 0;

		//
		//��Ƶ���Ͳ�������
		int highFreqTypeSampleCount = 0;
		//��Ƶ������ʼ���
		int highFreqStartSampleIdx = 0;
		//��Ƶʵ�ʲ�������
		int highFreqSampleCount = 0;
		//��Ƶ��ʼλ��Ƶ��
		float highFreqStartHz = 0;
		//��Ƶ����λ��Ƶ��
		float highFreqEndHz = 0;
		//��Ƶ����ֵ
		float perHighFreqHz = 0;

		//ת����ŵ�Ƶ�ʵ�λ
		float TransIdxToFreq(int idx)
		{
			float freq;
			if (idx < lowFreqSampleCount)
				freq = idx * perLowFreqHz + lowFreqStartHz;
			else
				freq = (idx - lowFreqSampleCount) * perHighFreqHz + highFreqStartHz;

			return freq;
		}
	};


	using BodeFreqzProcessCallBack = bool (*)(double x_out[], double y_out[], FreqzSampleRegionInfo& regionInfo, void* data);

}

#endif
