#include "InputClass.h"

InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboardDevice = 0;
	m_mouseDevice = 0;
	m_sensitivity = 0.1f;
	m_textInputEnabled = false;
}

InputClass::~InputClass()
{

	//마우스 DirectDevice 해제
	if (m_mouseDevice)
	{
		m_mouseDevice->Unacquire();
		m_mouseDevice->Release();
		m_mouseDevice = NULL;
	}

	//키보드 DirectDevice 해제
	if (m_keyboardDevice)
	{
		m_keyboardDevice->Unacquire();
		m_keyboardDevice->Release();
		m_keyboardDevice = NULL;
	}

	//DirectInput 해제
	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = NULL;
	}
}

bool InputClass::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result;

	m_hwnd = hwnd;

	//DirectInput 생성
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	//키보드용 Device 생성
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboardDevice, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	//데이터 형식을 키보드 형식으로 설정
	result = m_keyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	//협력 수준 설정
	result = m_keyboardDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	//키보드 장치 획득
	result = m_keyboardDevice->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	//마우스용 Device 생성
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouseDevice, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	//데이터 형식을 마우스 형식으로 설정
	result = m_mouseDevice->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(result))
	{
		return false;
	}

	//협력 수준 설정
	result = m_mouseDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	//마우스 장치 획득
	result = m_mouseDevice->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool InputClass::Frame()
{
	bool result;

	//마우스 State를 업데이트
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	//키보드 State를 업데이트
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	//이전 키 상태를 현재 상태로 덮어쓰기
	memcpy(m_prevKeyState, m_currKeyState, 256);

	//현재 키보드 장치의 상태를 가져옴
	result = m_keyboardDevice->GetDeviceState(sizeof(m_currKeyState), (LPVOID)&m_currKeyState);
	if (FAILED(result))
	{
		//DirectInputDevice에 문제가 생겼을 경우 장치를 다시 받아오는 부분
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboardDevice->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;

	//이전 마우스 버튼 상태를 현재 상태로 덮어쓰기
	memcpy(m_prevMouseState, m_mouseState.rgbButtons, 8);

	//마우스 장치의 상태를 가져옴
	result = m_mouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{

		//DirectInputDevice에 문제가 생겼을 경우 장치를 다시 받아오는 부분
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouseDevice->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

//전체화면 기준 마우스의 좌표를 얻는 함수
void InputClass::GetMousePosition(long& posX, long& posY)
{
	POINT pos;

	GetCursorPos(&pos);

	posX = pos.x;
	posY = pos.y;

	return;
}

//윈도우 위에서의 정규화된 마우스의 좌표를 얻는 함수
void InputClass::GetNormalizedMousePosition(float& posX, float& posY)
{
	POINT pos;
	RECT rect;

	//전체화면 기준 좌표
	GetCursorPos(&pos);

	ScreenToClient(m_hwnd, &pos);
	GetClientRect(m_hwnd, &rect);

	//마우스 위치를 정규화
	posX = ((2.0f * static_cast<float>(pos.x)) / static_cast<float>(rect.right - rect.left)) - 1.0f;
	posY = ((-2.0f * static_cast<float>(pos.y)) / static_cast<float>(rect.bottom - rect.top)) + 1.0f;

	return;
}

//마우스의 움직임을 얻는 함수
void InputClass::GetMouseDelta(float& posX, float& posY)
{
	posX = m_mouseState.lX * m_sensitivity;
	posY = m_mouseState.lY * m_sensitivity;

	return;
}

//키가 눌리면 true를 리턴하는 함수
bool InputClass::GetKeyPressed(const unsigned char keyCode)
{
	if (KEYDOWN(m_currKeyState, keyCode))
	{
		return true;
	}

	return false;
}

//키가 눌렸다가 땟을 때 true를 리턴하는 함수
bool InputClass::GetKeyPressedAndRelease(const unsigned char keyCode)
{
	if (KEYDOWN(m_prevKeyState, keyCode))
	{
		if (!KEYDOWN(m_currKeyState, keyCode))
		{
			return true;
		}
	}

	return false;
}

//키가 땟다가 눌렸을 때 true를 리턴하는 함수
bool InputClass::GetKeyReleasedAndPress(const unsigned char keyCode)
{
	if (!KEYDOWN(m_prevKeyState, keyCode))
	{
		if (KEYDOWN(m_currKeyState, keyCode))
		{
			return true;
		}
	}

	return false;
}

//마우스 왼쪽 버튼이 Press 상태인지 확인하는 함수
bool InputClass::GetLeftMouseButtonPressed()
{
	if (KEYDOWN(m_mouseState.rgbButtons, 0))
	{
		return true;
	}

	return false;
}

//마우스 왼쪽 버튼이 Release 상태인지 확인하는 함수
bool InputClass::GetLeftMouseButtonReleased()
{
	if (!KEYDOWN(m_mouseState.rgbButtons, 0))
	{
		return true;
	}

	return false;
}

//Press 상태에서 Release 상태로 전환 됐는지 확인하는 함수
bool InputClass::GetLeftMouseButtonPressedAndReleased()
{
	if (KEYDOWN(m_prevMouseState, 0))
	{
		if (!KEYDOWN(m_mouseState.rgbButtons, 0))
		{
			return true;
		}
	}

	return false;
}

//Release 상태에서 Press 상태로 전환 됐는지 확인하는 함수
bool InputClass::GetLeftMouseButtonReleasedAndPressed()
{
	if (!KEYDOWN(m_prevMouseState, 0))
	{
		if (KEYDOWN(m_mouseState.rgbButtons, 0))
		{
			return true;
		}
	}

	return false;
}

//마우스 오른쪽 버튼이 눌렸는지 체크하는 함수
bool InputClass::GetRightMouseButtonPressed()
{
	if (KEYDOWN(m_mouseState.rgbButtons, 1))
	{
		return true;
	}

	return false;
}

//마우스 오른쪽 버튼이 눌렸는지 체크하는 함수
bool InputClass::GetRightMouseButtonReleased()
{
	if (!KEYDOWN(m_mouseState.rgbButtons, 1))
	{
		return true;
	}

	return false;
}

void InputClass::AddText(const wchar_t text)
{
	unsigned int len = wcslen(m_text);

	if (len < 255)//빈칸 한자리가 남을 때 까지
	{
		wcscat_s(m_text, 256, &text);
	}

	return;
}

void InputClass::SetText(const wchar_t* text)
{
	unsigned int len = wcslen(text);

	if (len < 256)//배열 길이를 초과하지 않을 경우
	{
		wcscpy_s(m_text, text);
	}

	return;
}

void InputClass::RemoveLastChar()
{
	unsigned int len = wcslen(m_text);
	
	//저장된 문자가 없을 경우 함수 종료
	if (len == 0)
	{
		return;
	}

	m_text[len - 1] = L'\0';

	return;
}

const wchar_t* InputClass::GetText()
{
	return m_text;
}