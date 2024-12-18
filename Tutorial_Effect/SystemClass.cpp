#include "Systemclass.h"

SystemClass::SystemClass()
{
	m_applicationClass = 0;
	m_frameTimer = 0;
}

SystemClass::~SystemClass()
{
}

SystemClass::SystemClass(const SystemClass& other)
{
}

bool SystemClass::Initialize()
{
	bool result;

	//디스플레이 설정 초기화 - Settings.txt 파일로 부터 설정 가져옴
	InitializeDisplaySettings();

	//윈도우 초기화
	result = InitializeWindow();
	if (!result)
	{
		return false;
	}

	//EventClass 초기화
	EventClass::GetInstance();

	//InputClass 초기화
	result = InputClass::GetInstance().Initialize(m_hinstance, m_hwnd);
	if (!result)
	{
		return false;
	}

	//FrameTimer 생성
	m_frameTimer = new FrameTimer();
	if (!m_frameTimer)
	{
		return false;
	}

	//FrameTimer 초기화
	result = m_frameTimer->Initialize();
	if (!result)
	{
		return false;
	}

	//ApplicationClass 생성
	m_applicationClass = new ApplicationClass();
	if (!m_applicationClass)
	{
		return false;
	}

	//ApplicationClass 초기화
	result = m_applicationClass->Initialize(m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

bool SystemClass::InitializeWindow()
{
	bool result;

	WNDCLASSEX wc;
	RECT winSize;//윈도우 사이즈
	int posX, posY;//윈도우 위치

	//WndProc 함수에서 SystemClass의 인스턴스를 참조하기 위한 전역 변수
	ApplicationHandle = this;

	//현재 프로그램의 프로세스 인스턴스 핸들을 가져옴
	m_hinstance = GetModuleHandle(NULL);

	//윈도우 클래스 이름
	m_applicationName = L"Effect Test";

	//윈도우 클래스 설정
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.hIconSm = wc.hIcon;

	//윈도우 클래스 등록
	RegisterClassEx(&wc);

	//화면 중앙 위치 계산
	posX = (GetSystemMetrics(SM_CXSCREEN) - SCREEN_WIDTH) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - SCREEN_HEIGHT) / 2;

	//윈도우 위치, 크기 설정
	winSize.left = 0;
	winSize.top = 0;
	winSize.right = SCREEN_WIDTH;
	winSize.bottom = SCREEN_HEIGHT;

	//창모드 일때 
	if (!FULL_SCREEN)
	{
		//창 테두리, 상단바 크기를 포함한 크기로 재설정
		AdjustWindowRect(&winSize, WS_OVERLAPPEDWINDOW, FALSE);
	}

	//윈도우 생성
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,//일반 윈도우에서 크기 조정만 뺏음
		posX, posY, winSize.right - winSize.left, winSize.bottom - winSize.top, NULL, NULL, m_hinstance, NULL);

	//윈도우를 활성화하고 화면에 띄움
	ShowWindow(m_hwnd, SW_SHOW);

	//윈도우를 가장 앞으로 보냄
	SetForegroundWindow(m_hwnd);

	//키보드 포커스를 해당 윈도우로 설정
	SetFocus(m_hwnd);

	return true;
}

void SystemClass::InitializeDisplaySettings()
{
	bool result;

	//해당 텍스트 파일로 부터 설정 값을 가져옴
	string filename = "../settings.txt";

	ifstream fin;

	//파일 읽기
	fin.open(filename);
	result = fin.is_open();

	if (!result)//파일이 없는 경우
	{
		//디스플레이 기본 설정
		FULL_SCREEN = false;
		VSYNC_ENABLED = true;
		SCREEN_WIDTH = 1200;
		SCREEN_HEIGHT = 800;
		SCREEN_NEAR = 0.1f;
		SCREEN_FAR = 1000.0f;
	}
	else
	{
		const int optionNum = 11;	//옵션 개수
		string options[optionNum];

		//옵션 개수만큼 텍스트 파일에서 라인을 받아옴
		for (int i = 0; i < optionNum; i++)
		{
			getline(fin, options[i]);
		}

		for (string str : options)
		{
			result = SearchOptions(str, "FULL_SCREEN = ");
			if (result)
			{
				string temp = str.substr(str.find_last_of("FULL_SCREEN = "));
				auto value = StringToBool(temp);
				FULL_SCREEN = value;
				break;
			}
		}

		for (string str : options)
		{
			result = SearchOptions(str, "VSYNC_ENABLED = ");
			if (result)
			{
				string temp = str.substr(str.find_last_of("VSYNC_ENABLED = "));
				auto value = StringToBool(temp);
				VSYNC_ENABLED = value;
				break;
			}
		}

		for (string str : options)
		{
			result = SearchOptions(str, "SCREEN_WIDTH = ");
			if (result)
			{
				string temp = str.substr(str.find_last_of("SCREEN_WIDTH = "));
				auto value = stoi(temp);
				SCREEN_WIDTH = (unsigned int)value;
				break;
			}
		}

		for (string str : options)
		{
			result = SearchOptions(str, "SCREEN_HEIGHT = ");
			if (result)
			{
				string temp = str.substr(str.find_last_of("SCREEN_HEIGHT = "));
				auto value = stoi(temp);
				SCREEN_HEIGHT = (unsigned int)value;
				break;
			}
		}

		for (string str : options)
		{
			result = SearchOptions(str, "SCREEN_NEAR = ");
			if (result)
			{
				string temp = str.substr(str.find_last_of("SCREEN_NEAR = "));
				auto value = stof(temp);
				SCREEN_NEAR = (float)value;
				break;
			}
		}

		for (string str : options)
		{
			result = SearchOptions(str, "SCREEN_FAR = ");
			if (result)
			{
				string temp = str.substr(str.find_last_of("SCREEN_FAR = "));
				auto value = stof(temp);
				SCREEN_FAR = (float)value;
				break;
			}
		}

		//for (string str : options)
		//{
		//	result = SearchOptions(str, "Sample = ");
		//	if (result)
		//	{
		//		string temp = str.substr(str.find_last_of("Sample = "));//뒷부분의 문자열을 잘라서 저장
		//		bool value = StringToBool(temp);
		//		Sample = value;
		//		break;
		//	}
		//}
	}

	return;
}

void SystemClass::Shutdown()
{

	if (m_applicationClass)
	{
		m_applicationClass->Shutdown();
		delete m_applicationClass;
		m_applicationClass = 0;
	}

	if (m_frameTimer)
	{
		delete m_frameTimer;
		m_frameTimer = 0;
	}

	//윈도우 핸들 파괴
	DestroyWindow(m_hwnd);
	m_hwnd = nullptr;

	//등록된 윈도우 클래스를 해제
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = nullptr;

	//SystemClass 포인터를 nullptr로 초기화
	ApplicationHandle = nullptr;

	return;
}


void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	//변수 초기화
	ZeroMemory(&msg, sizeof(MSG));


	done = false;
	while (!done)
	{
		//메시지 전달확인
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//종료 메시지 확인
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//종료가 아니면 사이클반복
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

	}

	return;
}

bool SystemClass::Frame()
{
	bool result;

	m_frameTimer->Frame();

	result = InputClass::GetInstance().Frame();
	if (!result)
	{
		return false;
	}

	result = m_applicationClass->Frame(m_hwnd, m_frameTimer);
	if (!result)
	{
		return false;
	}

	return true;
}

bool SystemClass::StringToBool(string target)
{
	bool result;

	result = target.find("true") != string::npos;
	if (result)
	{
		return true;
	}

	return false;
}

bool SystemClass::SearchOptions(string target, const char* str)
{
	bool result;

	result = target.find(str) != string::npos;
	if (result)
	{
		return true;
	}

	return false;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	if (msg == WM_CHAR)
	{
		if (wparam == VK_BACK)
		{
			//백스페이스의 경우 '\b'가 저장되므로 예외로 문자를 지우는 함수 호출 (\n는 괜찮음)
			InputClass::GetInstance().RemoveLastChar();
		}
		else
		{
			//글자가 입력될 때 마다 inputClass의 멤버 변수에 저장
			InputClass::GetInstance().AddText(static_cast<wchar_t>(wparam));
		}
	}
	
	return DefWindowProc(hwnd, msg, wparam, lparam);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}