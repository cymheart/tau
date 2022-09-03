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

	//������������
	void Editor::SetPlayType(MidiEventPlayType playType)
	{
		//
		waitSem.reset(tau->midiEditorSyntherCount - 1);
		for (int i = 0; i < tau->midiEditorSyntherCount; i++)
			tau->midiEditorSynthers[i]->SetPlayTypeTask(playType, &waitSem);
		waitSem.wait();
	}

	//��ȡ��ǰʱ��֮���notekeys
	void Editor::GetCurTimeLateNoteKeys(float lateSec)
	{
		memset(lateHavKeys, 0, sizeof(bool) * 128);

		//
		waitSem.reset(tau->midiEditorSyntherCount - 1);
		for (int i = 0; i < tau->midiEditorSyntherCount; i++)
			tau->midiEditorSynthers[i]->GetCurTimeLateNoteKeysTask(lateSec, &waitSem);
		waitSem.wait();

		//
		for (int i = 0; i < tau->midiEditorSyntherCount; i++)
		{
			vector<int>& noteKeys = tau->midiEditorSynthers[i]->midiEditor->tempNoteKeys;
			for (int i = 0; i < noteKeys.size(); i++)
				lateHavKeys[noteKeys[i]] = true;
		}

	}


	//���뵽��������ģʽ
	void Editor::EnterStepPlayMode()
	{
		if (playMode == EditorPlayMode::Step) {
			printf("��ǰģʽ�Ѿ�Ϊ��������ģʽ! \n");
			return;
		}

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
		if (playMode == EditorPlayMode::Wait) {
			printf("��ǰģʽ�Ѿ�Ϊ�ȴ�����ģʽ! \n");
			return;
		}

		if (playMode == EditorPlayMode::Step) {
			printf("��ǰģʽΪ:��������ģʽ����Ҫ�뿪��������ģʽ! \n");
			return;
		}

		waitOnKeyLock.lock();
		playMode = EditorPlayMode::Wait;
		isWait = false;
		memset(onkey, 0, sizeof(int) * 128);
		memset(needOnkey, 0, sizeof(int) * 128);
		memset(needOffkey, 0, sizeof(int) * 128);
		needOnKeyCount = 0;
		needOffKeyCount = 0;
		waitOnKeyLock.unlock();

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
			waitOnKeyLock.lock();
			memset(onkey, 0, sizeof(int) * 128);
			memset(needOnkey, 0, sizeof(int) * 128);
			memset(needOffkey, 0, sizeof(int) * 128);
			needOnKeyCount = 0;
			needOffKeyCount = 0;
			isWait = false;
			waitOnKeyLock.unlock();
		}


		//
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
		lock_guard<mutex> lock(waitOnKeyLock);

		if (onkey[key] > 0) {
			onkey[key]--;
			onKeyCount--;
			return;
		}

		needOnkey[key]++;
		needOnKeyCount++;
		isWait = true;
		printf("�ȴ�����:%d \n", key);
	}

	//��Ҫ�ɿ������ź�
	void Editor::NeedOffKeySignal(int key)
	{
		lock_guard<mutex> lock(waitOnKeyLock);

		if (onkey[key] == 0)
			return;

		needOffkey[key]++;
		needOffKeyCount++;
		isWait = true;
		printf("�ȴ��ɿ���:%d \n", key);
	}

	//�����ź�
	void Editor::OnKeySignal(int key)
	{
		GetCurTimeLateNoteKeys(2);

		lock_guard<mutex> lock(waitOnKeyLock);

		printf("���°���:%d \n", key);
		onkey[key]++;
		onKeyCount++;

		if (needOnkey[key] > 0) //������Ҫ��Ӧ���µİ���
		{
			onkey[key]--;
			onKeyCount--;

			//
			needOnkey[key]--;
			needOnKeyCount--;
			if (needOnKeyCount == 0 && needOffKeyCount == 0)
				isWait = false;
		}
		else if (!lateHavKeys[key])  //��ǰʱ��֮��û����������¼�
		{
			isWait = true;
			printf("�����,�ȴ���\n");
		}

	}

	//�ɿ������ź�
	void Editor::OffKeySignal(int key)
	{
		lock_guard<mutex> lock(waitOnKeyLock);

		if (onkey[key] > 0)
		{
			printf("�ɿ�����:%d \n", key);
			onkey[key]--;
			onKeyCount--;

			if (needOffkey[key] > 0) {
				needOffkey[key]--;
				needOffKeyCount--;
			}

			if (needOnKeyCount == 0 && needOffKeyCount == 0 && onKeyCount == 0) {
				isWait = false;
			}
		}
	}

}