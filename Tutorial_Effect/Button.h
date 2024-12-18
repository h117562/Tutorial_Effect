#ifndef _BUTTON_
#define _BUTTON_

#include "RectTransform.h"
#include "RectTexture2D.h"
#include "ShaderManager.h"
#include "InputClass.h"
#include "EventClass.h"

class Button : public RectTransform, public RectTexture2D
{
public:
	Button()
	{
		m_btnState = ButtonState::BUTTON_IDLE;
		m_skip = false;
	}

	HRESULT Initialize(ID3D11Device* pDevice, const wchar_t* idleTexturePath, const wchar_t* hoverTexturePath, const wchar_t* pressedTexturePath)
	{
		HRESULT result;

		result = InitializeBuffer(pDevice);
		if (FAILED(result))
		{
			return result;
		}

		const wchar_t* filepath[3] = { idleTexturePath , hoverTexturePath , pressedTexturePath };

		result = SetTexture(pDevice, filepath, _countof(filepath));
		if (FAILED(result))
		{
			return result;
		}

		return S_OK;
	}

	void Frame(DirectX::XMMATRIX view, DirectX::XMMATRIX proj, float mouseX, float mouseY)
	{
		bool state;

		if (m_skip)
		{
			if (InputClass::GetInstance().GetLeftMouseButtonReleased())//마우스 클릭을 Release 하기 전까진 계속 건너뛰기
			{
				m_skip = false;//스킵 해제
			}

			return;//함수 조기 종료 (뒤에 있는 연산은 불필요하게 됨)
		}

		//마우스가 버튼 범위 내에 없을 때
		state = RayCast(GetWorldMatrix(), view, proj, mouseX, mouseY);
		if (!state)
		{
			m_btnState = ButtonState::BUTTON_IDLE;//기본 상태로 초기화

			if (InputClass::GetInstance().GetLeftMouseButtonPressed())//범위 밖에서 눌린 경우 Release 하기 전까진 레이캐스트 스킵
			{
				m_skip = true;//스킵 상태
			}

			return;//함수 조기 종료
		}
		else//버튼 범위 내에 있을 때
		{
			if (m_btnState != ButtonState::BUTTON_PRESSED)//버튼 상태가 Idle 혹은 Hover 일 경우
			{
				if (InputClass::GetInstance().GetLeftMouseButtonPressed())//버튼 클릭 확인 후 참이면 Pressed 상태로 전환
				{
					m_btnState = ButtonState::BUTTON_PRESSED;
				}
				else//클릭이 없을 경우
				{
					m_btnState = ButtonState::BUTTON_HOVER;//버튼 상태 Hover로 전환
				}
			}
			else//Pressed 상태일 경우
			{
				if (InputClass::GetInstance().GetLeftMouseButtonReleased())//범위 내에서 Release 했을 때 상태 전환
				{
					m_btnState = ButtonState::BUTTON_RELEASED;
				}
			}
		}

		return;
	}

	bool IsPressed()
	{
		//버튼이 눌렸다면 스택 방식 처럼 버튼이 눌렸는지 체크할 때 까지 Release 상태를 유지
		//따라서 버튼을 매 프레임마다 눌렸는지 체크해야함 (예외로 버튼 범위를 벗어난 클릭을 하게 되면 초기상태로)
		if (m_btnState == ButtonState::BUTTON_RELEASED)
		{
			m_btnState = ButtonState::BUTTON_IDLE;//버튼 눌렸는지 체크했다면 초기 상태로 되돌리고 true 리턴

			return true;
		}

		return false;
	}

	void SetActive(bool state)
	{
		m_skip = state;
	}

	bool Render(ID3D11DeviceContext* pDeviceContext,UIShaderClass* pUIShader,const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
	{
		bool result;

		//행렬 업데이트
		result = pUIShader->Render(pDeviceContext ,world, view, proj);
		if (!result)
		{
			return false;
		}

		switch (m_btnState)
		{
		case ButtonState::BUTTON_IDLE:
			Draw(pDeviceContext, 0);
			break;
		case ButtonState::BUTTON_HOVER:
			Draw(pDeviceContext, 1);
			break;
		case ButtonState::BUTTON_PRESSED:
			Draw(pDeviceContext, 2);
			break;
		case ButtonState::BUTTON_RELEASED:
			Draw(pDeviceContext, 0);
			break;
		}

		return true;
	}

private:
		enum struct ButtonState
		{
			BUTTON_IDLE,//평상시 상태
			BUTTON_HOVER,//마우스를 버튼 위에 올린 상태
			BUTTON_PRESSED,//누르고 있는 중인 상태
			BUTTON_RELEASED//버튼이 눌렸다고 판정한 상태
		};

private:
	ButtonState m_btnState;
	bool m_skip;
};

#endif
