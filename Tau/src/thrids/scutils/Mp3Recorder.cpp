#include"Mp3Recorder.h"

namespace scutils
{
	lame_global_flags* Mp3Recorder::glf = NULL;

	/**
   * ��ʼ�� lame������
   * @param inSampleRate ���������
   * @param outChannel ������
   * @param outSampleRate ���������
   * @param outBitrate ������(kbps)
   * @param quality 0~9��0���
   */
	void Mp3Recorder::Init(
		int inSamplerate, int outChannel, int outSamplerate,
		int outBitrate, int quality)
	{
		if (glf != NULL) {
			lame_close(glf);
			glf = NULL;
		}

		glf = lame_init();
		lame_set_in_samplerate(glf, inSamplerate);
		lame_set_num_channels(glf, outChannel);
		lame_set_out_samplerate(glf, outSamplerate);
		lame_set_brate(glf, outBitrate);
		lame_set_quality(glf, quality);
		lame_init_params(glf);
	}

	/**
	 *  ���룬��¼�Ƶ� PCM ����ת���� mp3 ��ʽ
	 *
	 * @param buffer_l ��������������
	 * @param buffer_r ��������������
	 * @param nsamples   �������ݵ�size�������ݵ�size
	 * @param mp3buf �������
	 * @return �����mp3buf��byte����
	 */
	int Mp3Recorder::Encode(short* buffer_l, short* buffer_r, int nsamples, uint8_t* mp3buf, int mp3bufSize)
	{
		return lame_encode_buffer(glf, buffer_l, buffer_r, nsamples, mp3buf, mp3bufSize);
	}


	/**
	 *  ���룬��  ¼�Ƶ� PCM ����ת���� mp3 ��ʽ
	 *
	 * @param buffer_l ��������������
	 * @param buffer_r ��������������
	 * @param nsamples   �������ݵ�size
	 * @param mp3buf �������
	 * @return �����mp3buf��byte����
	 */
	int Mp3Recorder::EncodeFloat(float* buffer_l, float* buffer_r, int nsamples, uint8_t* mp3buf, int mp3bufSize)
	{
		return lame_encode_buffer_ieee_float(glf, buffer_l, buffer_r, nsamples, mp3buf, mp3bufSize);
	}


	/**
	 *  ˢд
	 *
	 * @param mp3buf mp3���ݻ�����
	 * @return  ����ˢд������
	 */
	int Mp3Recorder::flush(uint8_t* mp3buf, int mp3bufSize)
	{
		return lame_encode_flush(glf, mp3buf, mp3bufSize);
	}


	/**
* �ر� lame ���������ͷ���Դ
*/
	void Mp3Recorder::close()
	{
		lame_close(glf);
		glf = NULL;
	}

}