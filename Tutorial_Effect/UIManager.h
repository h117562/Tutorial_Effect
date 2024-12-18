#ifndef _USER_INTERFACE_MANAGER_H_
#define _USER_INTERFACE_MANAGER_H_

#include "TextClass.h"
#include "ShaderManager.h"
#include "InputClass.h"
#include "EventClass.h"

#include "DebugInfo.h"
#include "TestCanvas.h"

class UIManager
{
public:
	UIManager();
	~UIManager();
	UIManager(const UIManager&);

	bool Initialize(D3DClass*, TextClass*);
	bool Frame(D3DClass*, HWND hwnd, ShaderManager*, TextClass*, CameraClass*);
	void Shutdown();

private:
	DebugInfo m_debugUI;
	TestCanvas m_canvas;
};

#endif