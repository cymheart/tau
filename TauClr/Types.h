#pragma once

#include <msclr\marshal_cppstd.h>
using namespace msclr::interop;

namespace TauClr
{
	// Editor״̬
	public enum class EditorState
	{
		//ֹͣ
		STOP,
		//����
		PLAY,
		//��ͣ
		PAUSE
	};
}