#include "ApplicationClass.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_CameraClass = 0;
	m_uiManager = 0;
	m_TextClass = 0;
	m_ShaderManager = 0;
}

ApplicationClass::~ApplicationClass()
{
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}

bool ApplicationClass::Initialize(HWND hwnd)
{
	bool result;

	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	//DirectX 초기화
	result = m_Direct3D->Initialize(hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	m_ShaderManager = new ShaderManager;
	if (!m_ShaderManager)
	{
		return false;
	}

	//쉐이더 매니저 초기화
	result = m_ShaderManager->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ShaderManager.", L"Error", MB_OK);
		return false;
	}

	m_TextClass = new TextClass;
	if (!m_TextClass)
	{
		return false;
	}

	//TextClass 초기화
	result = m_TextClass->Initialize(m_Direct3D->GetSwapChain());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the TextClass.", L"Error", MB_OK);
		return false;
	}

	m_uiManager = new UIManager;
	if (!m_uiManager)
	{
		return false;
	}

	//UI 매니저 초기화
	result = m_uiManager->Initialize(m_Direct3D, m_TextClass);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the UI Manager.", L"Error", MB_OK);
		return false;
	}

	m_CameraClass = new CameraClass;
	if (!m_CameraClass)
	{
		return false;
	}
	
	//블랜드 활성화
	m_Direct3D->EnableAlphaBlending();

	//기본 뷰 매트릭스 초기화
	m_CameraClass->SetBaseViewMatrix();

	return result;
}

void ApplicationClass::Shutdown()
{

	if (m_uiManager)
	{
		m_uiManager->Shutdown();
		delete m_uiManager;
		m_uiManager = 0;
	}

	if (m_TextClass)
	{
		m_TextClass->Shutdown();
		delete m_TextClass;
		m_TextClass = 0;
	}

	if (m_CameraClass)
	{
		delete m_CameraClass;
		m_CameraClass = 0;
	}

	if (m_ShaderManager)
	{
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}

	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	return;
}

bool ApplicationClass::Frame(HWND hwnd, FrameTimer* pFrameTimer)
{
	bool result;

	//사용자 입력 처리
	HandleInput(pFrameTimer);

	//카메라 업데이트
	m_CameraClass->Render();

	//3D RenderTarget 초기화(특정 컬러로)
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.2f, 1.0f);

	//2D RenderTarget 초기화
	m_TextClass->BeginDraw();

	//그래픽 렌더링
	Render(hwnd);

	//UI 렌더링
	m_uiManager->Frame(m_Direct3D, hwnd, m_ShaderManager, m_TextClass, m_CameraClass);

	m_TextClass->EndDraw();
	m_Direct3D->EndScene();

	return true;
}

void ApplicationClass::HandleInput(FrameTimer* pFrameTimer)
{
	bool state;
	float deltaX, deltaY, frameTime;

	frameTime = pFrameTimer->GetTime();
	m_CameraClass->SetFrameTime(frameTime);

	state = InputClass::GetInstance().GetKeyPressed(DIK_A);
	m_CameraClass->MoveLeft(state);

	state = InputClass::GetInstance().GetKeyPressed(DIK_S);
	m_CameraClass->MoveBackward(state);

	state = InputClass::GetInstance().GetKeyPressed(DIK_D);
	m_CameraClass->MoveRight(state);

	state = InputClass::GetInstance().GetKeyPressed(DIK_W);
	m_CameraClass->MoveForward(state);

	InputClass::GetInstance().GetMouseDelta(deltaX, deltaY);
	m_CameraClass->UpdateRotation(deltaX, deltaY);

	state = InputClass::GetInstance().GetKeyPressedAndRelease(DIK_F5);
	if (state)
	{
		EventClass::GetInstance().Publish(UI_EVENT::TOGGLE_DEBUG_MODE);
	}

	state = InputClass::GetInstance().GetKeyPressedAndRelease(DIK_F6);
	if (state)
	{
		EventClass::GetInstance().Publish(UI_EVENT::TOGGLE_TEST_CANVAS);
	}

	return;
}


void ApplicationClass::Render(HWND hwnd)
{
	return;
}