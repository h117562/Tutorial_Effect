#ifndef _APPLICATION_CLASS_H_
#define _APPLICATION_CLASS_H_

#include "D3DClass.h"
#include "ShaderManager.h"
#include "CameraClass.h"
#include "InputClass.h"
#include "FrameTimer.h"
#include "UIManager.h"
#include "TextClass.h"
#include "EventClass.h"

class ApplicationClass
{
public:
	ApplicationClass();
	~ApplicationClass();
	ApplicationClass(const ApplicationClass&);

	bool Initialize(HWND);
	void Shutdown();
	bool Frame(HWND, FrameTimer*);

private:
	void HandleInput(FrameTimer*);
	void Render(HWND);

private:
	D3DClass* m_Direct3D;
	TextClass* m_TextClass;
	ShaderManager* m_ShaderManager;
	CameraClass* m_CameraClass;
	UIManager* m_uiManager;
};
#endif