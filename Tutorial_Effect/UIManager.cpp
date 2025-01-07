#include "UIManager.h"

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
}

UIManager::UIManager(const UIManager& other)
{
}

bool UIManager::Initialize(D3DClass* pD3Dclass, TextClass* pTextClass)
{
	bool result;

	//각종 디버깅 정보들을 화면에 렌더링하는 UI 클래스
	result = m_debugUI.Initialize(pD3Dclass);
	if (!result)
	{
		return false;
	}

	EventClass::GetInstance().Subscribe(UI_EVENT::TOGGLE_DEBUG_MODE, [&]() {m_debugUI.ToggleActive(); });

	return true;
}


bool UIManager::Frame(D3DClass* pD3DClass, HWND hwnd, ShaderManager* pShaderManager, TextClass* pTextClass, CameraClass* pCameraClass)
{
	bool result, state;

	pD3DClass->TurnZBufferOff();

	if (m_debugUI.GetActive())
	{
		m_debugUI.Frame(pTextClass, pCameraClass);
	}

	pD3DClass->TurnZBufferOn();

	return true;
}


void UIManager::Shutdown()
{
	return;
}