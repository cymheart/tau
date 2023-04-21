#ifndef _MeasureInfo_h_
#define _MeasureInfo_h_

#include "Synth/TauTypes.h"
#include"MidiMarkerList.h"


namespace tau
{
	// С����Ϣ
	//by cymheart, 2022.
	class MeasureInfo
	{
	public:

		//����С��
		void Create(MidiMarkerList& midiMarkerList, float endSec);

		void Clear()
		{
			mIdx = 0;
			bIdx = 0;
			measureNum = 0;
			atMeasure = 1;
		}

		//��ȡС������
		int GetMeasureCount()
		{
			return measureNum;
		}

		//��ȡָ��С�ڵĿ�ʼʱ���
		float GetMeasureStartSec(int i)
		{
			i = min(measureNum, i) - 1;
			return measure[i * 2];
		}

		//��ȡָ��С�ڵĽ���ʱ���
		float GetMeasureEndSec(int i)
		{
			i = min(measureNum, i);
			if (i == measureNum)
				return beat[bIdx];

			return measure[i * 2];
		}

		//��ȡС����������
		int GetMeasureBeatCount(int i)
		{
			i = min(measureNum, i);
			if (i == measureNum) {
				int a = measure[(i - 1) * 2 + 1];
				return bIdx - a + 1;
			}

			//
			int a = measure[(i - 1) * 2 + 1];
			int b = measure[i * 2 + 1];
			return b - a + 1;

		}

		//��ȡС��ָ�����ӵĽ���ʱ���
		float GetMeasureBeatEndSec(int measureIdx, int beatIdx)
		{
			int a = measure[(measureIdx - 1) * 2 + 1];
			return beat[a + (beatIdx - 1)];
		}


		float* GetMeasureDatas()
		{
			return measure;
		}

		int GetMeasureDataSize()
		{
			return mIdx + 1;
		}

		float* GetBeatDatas()
		{
			return beat;
		}

		int GetBeatDataSize()
		{
			return bIdx + 1;
		}

		//��ȡָ��ʱ�����ڵ�С��
		int GetSecAtMeasure(float sec)
		{
			int left = 0, right = measureNum - 1;
			int curt = 0;

			while (true)
			{
				curt = (right + left) / 2;
				float s = measure[curt * 2];
				if (s > sec) right = curt;
				else if (s < sec) left = curt;
				else return curt + 1;

				if (left + 1 == right || left == right)
					return left + 1;
			}
		}

	private:

		//С����Ϣ
		float measure[10000];

		//������Ϣ
		float beat[10000];

		//С�ڽ���λ��
		int mIdx = 0;

		//���ӽ���λ��
		int bIdx = 0;

		//С������
		int measureNum = 0;

		//����С��
		int atMeasure = 1;

	};
}

#endif