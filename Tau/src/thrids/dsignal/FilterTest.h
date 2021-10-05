#ifndef _FilterTest_h_
#define _FilterTest_h_

#include "scutils/Utils.h"
#include "scutils/MathUtils.h"
#include <dsignal/Biquad.h>
using namespace scutils;

namespace dsignal
{
	class FilterTest
	{
	public:
		FilterTest();

		//�������ƫ������
		void SetMaxOffsetGainDB(float maxOffsetGain)
		{
			maxOffsetGain_ = abs(maxOffsetGain);
			maxOffsetGainlin = pow(10.0, -maxOffsetGain_ / 20);
		}

		//�������ƫ������
		float GetMaxOffsetGainDB()
		{
			return maxOffsetGain_;
		}

		//����Ƶ�ʵ�
		void SetFreq(float freqHZ);

		//����Q
		void SetQ(float q);

		//����Qs
		void SetQs(int idx, float q);

		//��������λ��0-1
		void SetGainPos(float pos);

		//�����˲�����
		void Compute();

		//��ȡ�˲���
		vector<dsignal::Filter*> GetFilters();

		double Filtering(double input);

	private:
		Biquad lowPass[3];
		float qs_[3] = { 0.707f };


		Biquad lowShelf;
		Biquad lowShelf2;
		Biquad peak;
		Biquad highShelf;

		float maxOffsetGainlin;
		float maxOffsetGain_ = 10;//dB
		float freq_ = 0;
		float q_ = 0.707f;
		float gain[3] = { 0 };
		float pos_ = 0;

	};
}

#endif
