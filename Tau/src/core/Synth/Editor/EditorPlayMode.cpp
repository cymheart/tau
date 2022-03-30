#include"Editor.h"
#include"Track.h"
#include"MidiEditor.h"
#include"Synth/Tau.h"
#include"Synth/Synther/MidiEditorSynther.h"
#include"Synth/Channel.h"
#include"Synth/KeySounder.h"
#include"Midi/MidiEvent.h"
#include"Midi/MidiTrack.h"
#include"Midi/MidiFile.h"
#include"Synth/VirInstrument.h"
#include"Synth/Preset.h"


namespace tau
{
	//���ù���¼����෽ʽ
	void Editor::SetTrackPlayType(int trackIdx, MidiEventPlayType playType)
	{
		waitSem.reset(0);
		MidiEditorSynther* synther = tracks[trackIdx]->GetMidiEditor()->GetSynther();
		synther->SetTrackPlayTypeTask(&waitSem, tracks[trackIdx], playType);
		waitSem.wait();
	}


	//���뵽��������ģʽ
	void Editor::EnterStepPlayMode()
	{
		if (playMode == EditorPlayMode::Wait) {
			printf("��ǰģʽΪ:�ȴ�����ģʽ����Ҫ�뿪�ȴ�����ģʽ! \n");
			return;
		}

		playMode = EditorPlayMode::Step;
		isWait = false;

		//
		waitSem.reset(tau->midiEditorSyntherCount - 1);
		for (int i = 0; i < tau->midiEditorSyntherCount; i++)
			tau->midiEditorSynthers[i]->EnterPlayModeTask(EditorPlayMode::Step, &waitSem);
		waitSem.wait();
	}


	//���뵽�ȴ�����ģʽ
	void Editor::EnterWaitPlayMode()
	{
		if (playMode == EditorPlayMode::Step) {
			printf("��ǰģʽΪ:��������ģʽ����Ҫ�뿪��������ģʽ! \n");
			return;
		}

		playMode = EditorPlayMode::Wait;
		isWait = false;
		memset(onkey, 0, sizeof(int) * 128);
		memset(needOnkey, 0, sizeof(int) * 128);
		memset(needOffkey, 0, sizeof(int) * 128);
		needOnKeyCount = 0;
		needOffKeyCount = 0;

		//
		waitSem.reset(tau->midiEditorSyntherCount - 1);
		for (int i = 0; i < tau->midiEditorSyntherCount; i++)
			tau->midiEditorSynthers[i]->EnterPlayModeTask(EditorPlayMode::Wait, &waitSem);
		waitSem.wait();

	}

	//�뿪��ǰ����ģʽ
	void Editor::LeavePlayMode()
	{
		playMode = EditorPlayMode::Common;
		waitSem.reset(tau->midiEditorSyntherCount - 1);
		for (int i = 0; i < tau->midiEditorSyntherCount; i++)
			tau->midiEditorSynthers[i]->LeavePlayModeTask(&waitSem);

		waitSem.wait();
	}


	//�ƶ���ָ��ʱ���
	void Editor::Runto(double sec)
	{
		//����ǵȴ�����ģʽ������յȴ�����ģʽ������
		if (playMode == EditorPlayMode::Wait) {
			memset(onkey, 0, sizeof(int) * 128);
			memset(needOnkey, 0, sizeof(int) * 128);
			memset(needOffkey, 0, sizeof(int) * 128);
			needOnKeyCount = 0;
			needOffKeyCount = 0;
			isWait = false;
		}

		waitSem.reset(tau->midiEditorSyntherCount - 1);
		for (int i = 0; i < tau->midiEditorSyntherCount; i++)
			tau->midiEditorSynthers[i]->RuntoTask(&waitSem, sec);

		waitSem.wait();
	}

	//�ȴ�(��������ͣ���ȴ��൱����ԭʼλ�ò���)
	void Editor::Wait()
	{
		if (playMode != EditorPlayMode::Common)
			return;

		isWait = true;
	}

	//����������ڵȴ�����
	void Editor::Continue()
	{
		if (playMode != EditorPlayMode::Common)
			return;

		isWait = false;
	}

	//��Ҫ�����ź�
	void Editor::NeedOnKeySignal(int key)
	{
		waitOnKeyLock.lock();

		needOnkey[key]++;

		if (needOnkey[key] > 0)
			needOnKeyCount++;

		if (needOnKeyCount > 0)
		{
			isWait = true;
			printf("�ȴ�����:%d \n", key);
		}

		waitOnKeyLock.unlock();
	}

	//��Ҫ�ɿ������ź�
	void Editor::NeedOffKeySignal(int key)
	{
		waitOnKeyLock.lock();

		if (onkey[key] == 0) {
			waitOnKeyLock.unlock();
			return;
		}

		needOffkey[key]++;

		if (needOffkey[key] > 0)
			needOffKeyCount++;

		if (needOffKeyCount > 0) {
			isWait = true;
			printf("�ȴ��ɿ���:%d \n", key);
		}


		waitOnKeyLock.unlock();
	}

	//�����ź�
	void Editor::OnKeySignal(int key)
	{
		waitOnKeyLock.lock();

		needOnkey[key]--;
		onkey[key]++;

		if (needOnkey[key] >= 0)
			needOnKeyCount--;

		if (needOnKeyCount <= 0 && needOffKeyCount <= 0) {
			isWait = false;
			printf("���°���,����:%d \n", key);
		}
		else
		{
			printf("���°���:%d \n", key);
		}

		waitOnKeyLock.unlock();
	}

	//�ɿ������ź�
	void Editor::OffKeySignal(int key)
	{
		waitOnKeyLock.lock();

		if (onkey[key] > 0 && needOffkey[key] == 0 && needOnkey[key] == 0)
		{
			needOnkey[key]++;
			needOnKeyCount++;
			onkey[key]--;

			if (needOnKeyCount > 0) {
				isWait = true;
				printf("�ȴ�����:%d \n", key);
			}

			waitOnKeyLock.unlock();
			return;
		}


		if (needOffkey[key] == 0)
		{
			if (onkey[key] > 0)
				onkey[key]--;

			waitOnKeyLock.unlock();
			return;
		}

		needOffkey[key]--;

		if (onkey[key] > 0)
			onkey[key]--;

		if (needOffkey[key] >= 0)
			needOffKeyCount--;

		if (needOnKeyCount <= 0 && needOffKeyCount <= 0) {
			isWait = false;
			printf("�ɿ�����������:%d \n", key);
		}
		else
		{
			printf("�ɿ�����:%d \n", key);
		}

		waitOnKeyLock.unlock();
	}

}