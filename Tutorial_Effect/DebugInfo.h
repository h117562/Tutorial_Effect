#ifndef _DEBUG_INFO_H_
#define _DEBUG_INFO_H_

#include <DirectXMath.h>

#include "D3DClass.h"
#include "textclass.h"
#include "CameraClass.h"
#include "InputClass.h"

class DebugInfo
{
public:
	DebugInfo();
	~DebugInfo();
	DebugInfo(const DebugInfo&);

	bool Initialize(D3DClass*);
	void Frame(TextClass*, CameraClass*);
	void Render(TextClass*);

	void ToggleActive();
	void SetActive(bool state);
	bool GetActive();

private:
	void GetFps();

private:
	bool m_active;
	int m_fps, m_count;
	wchar_t m_fpsString[20];
	wchar_t m_videoNameString[144];
	wchar_t m_memoryString[32];
	wchar_t m_tempString[16];
	unsigned long m_startTime;

	DirectX::XMINT3 m_position, m_rotation;
	DirectX::XMINT3 m_prevPos, m_prevRot;
	long m_mouseX, m_mouseY;
	long m_prevMX, m_prevMY;
	wchar_t m_stringPX[16], m_stringPY[16], m_stringPZ[16];
	wchar_t m_stringRX[16], m_stringRY[16], m_stringRZ[16];
	wchar_t m_stringMX[30], m_stringMY[30];
};

#endif