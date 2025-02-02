﻿#include"Equalizer.h"
#include"Synth/Synther/Synther.h"

namespace tauFX
{
	Equalizer::Equalizer(Synther* synther)
		:TauEffect(synther)
	{
		eq = new dsignal::GraphEqualizer();
	}

	Equalizer::~Equalizer()
	{
		DEL(eq);
	}

	void Equalizer::SetSynther(Synther* synther)
	{
		this->synther = synther;
		eq->SetSampleRate(synther->GetSampleProcessRate());
	}

	vector<dsignal::Filter*> Equalizer::GetFilters()
	{
		return eq->GetFilters();
	}

	void Equalizer::SetFreqBandGain(int bandIdx, float gainDB)
	{
		eq->SetFreqBandGain(bandIdx, gainDB);
	}

	void Equalizer::EffectProcess(float* leftChannelSamples, float* rightChannelSamples, int channelSampleCount)
	{
		for (int i = 0; i < channelSampleCount; i++)
		{
			leftChannelSamples[i] = (float)eq->Filtering(leftChannelSamples[i]);
			rightChannelSamples[i] = (float)eq->Filtering(rightChannelSamples[i]);
		}
	}
}
