#include "TestCanvas.h"

TestCanvas::TestCanvas()
{
	m_btn = 0;
	m_plane = 0;
	m_textBox = 0;
	m_textBox2 = 0;
	m_active = true;
}

TestCanvas::~TestCanvas()
{
	if (m_btn)
	{
		delete[] m_btn;
		m_btn = nullptr;
	}

	if (m_plane)
	{
		delete m_plane;
		m_plane = nullptr;
	}

	if (m_textBox)
	{
		delete m_textBox;
		m_textBox = nullptr;
	}

	if (m_textBox2)
	{
		delete m_textBox2;
		m_textBox2 = nullptr;
	}
}

bool TestCanvas::Initialize(ID3D11Device* pDevice, TextClass* pTextClass)
{
	HRESULT result;
	
	m_btn = new Button[4];
	if (!m_btn)
	{
		return false;
	}
	
	for (int i = 0; i< 4; i++)
	{
		result = m_btn[i].Initialize(pDevice, 
			L"..//data//assets//mobile-button.png",
			L"..//data//assets//mobile-button-1.png",
			L"..//data//assets//mobile-button.png"
		);
		if (FAILED(result))
		{
			return false;
		}
	}

	m_btn[0].SetScale(100, 100, 1);
	m_btn[0].SetAlign(ALIGNMENT_LEFT);
	m_btn[0].UpdateTransform();

	m_btn[1].SetScale(100, 100, 1);
	m_btn[1].SetAlign(ALIGNMENT_RIGHT);
	m_btn[1].UpdateTransform();

	m_btn[2].SetScale(100, 200, 1);
	m_btn[2].SetAlign(ALIGNMENT_TOP);
	m_btn[2].UpdateTransform();

	m_btn[3].SetScale(100, 100, 1);
	m_btn[3].SetAlign(ALIGNMENT_BOTTOM);
	m_btn[3].UpdateTransform();

	m_plane = new Plane;
	if (!m_plane)
	{
		return false;
	}

	result = m_plane->Initialize(pDevice, L"..//data//assets//panel-2.png");
	if (FAILED(result))
	{
		return false;
	}

	m_plane->SetScale(500, 500, 1);
	m_plane->UpdateTransform();

	//////TextBox1 Initialize
	m_textBox = new TextBox;
	if (!m_textBox)
	{
		return false;
	}

	result = m_textBox->Initialize(pDevice, L"..//data//assets//panel-header-1.png", L"..//data//assets//panel-header-2.png");
	if (FAILED(result))
	{
		return false;
	}

	m_textBox->SetScale(300, 100, 1);
	m_textBox->SetAlign(ALIGNMENT_RIGHT_BOTTOM);
	m_textBox->SetLocalPosition(-100, 100, 0);
	m_textBox->UpdateTransform();

	result = pTextClass->CreateTextFormat(m_textBox->GetTextFormat(),
		L"바탕", 
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL, 
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL, 
		16.0f);
	if (FAILED(result))
	{
		return false;
	}

	m_textBox->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	result = pTextClass->CreateTextBrush(m_textBox->GetTextBrush(), 1.0f, 0.3f, 0.6f, 1.0f);
	if (FAILED(result))
	{
		return false;
	}

	//////TextBox2 Initialize
	m_textBox2 = new TextBox;
	if (!m_textBox2)
	{
		return false;
	}

	result = m_textBox2->Initialize(pDevice, L"..//data//assets//panel-header-1.png", L"..//data//assets//panel-header-2.png");
	if (FAILED(result))
	{
		return false;
	}

	m_textBox2->SetScale(300, 100, 1);
	m_textBox2->SetAlign(ALIGNMENT_RIGHT_TOP);
	m_textBox2->SetLocalPosition(-100, -100, 0);
	m_textBox2->UpdateTransform();

	result = pTextClass->CreateTextFormat(m_textBox2->GetTextFormat(),
		L"궁서체",
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		20.0f);
	if (FAILED(result))
	{
		return false;
	}

	m_textBox2->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);

	result = pTextClass->CreateTextBrush(m_textBox2->GetTextBrush(), 0.0f, 0.8f, 0.5f, 1.0f);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TestCanvas::Frame(D3DClass* pD3DClass, HWND hwnd, ShaderManager* pShaderManager, TextClass* pTextClass, CameraClass* pCameraClass)
{
	bool state;
	
	float mouseX, mouseY;//정규화 마우스 좌표

	InputClass::GetInstance().GetNormalizedMousePosition(mouseX, mouseY);
	
	XMMATRIX view, proj;

	pCameraClass->GetBaseViewMatrix(view);
	pD3DClass->GetOrthoMatrix(proj);

	for (int i = 0; i < 4; i++)
	{
		m_btn[i].Frame(view, proj, mouseX, mouseY);
	}

	m_textBox->Frame(view, proj, mouseX, mouseY);
	m_textBox2->Frame(view, proj, mouseX, mouseY);

	state = m_btn[0].IsPressed();
	if (state)
	{
		//TODO
		MessageBox(hwnd, L"왼쪽 버튼 눌림", L"Check", MB_OK);
	}

	state = m_btn[1].IsPressed();
	if (state)
	{
		//TODO
		MessageBox(hwnd, L"오른쪽 버튼 눌림", L"Check", MB_OK);
	}

	state = m_btn[2].IsPressed();
	if (state)
	{
		//TODO
		MessageBox(hwnd, L"상단 버튼 눌림", L"Check", MB_OK);
	}

	state = m_btn[3].IsPressed();
	if (state)
	{
		//TODO
		MessageBox(hwnd, L"하단 버튼 눌림", L"Check", MB_OK);
	}

	Render(pD3DClass, pTextClass, pShaderManager, view, proj);
	
	return;
}

bool TestCanvas::Render(D3DClass* pD3DClass,TextClass* pTextClass, ShaderManager* pShaderManager, const XMMATRIX& view, const XMMATRIX& proj)
{
	bool result;

	result = m_plane->Render(pD3DClass->GetDeviceContext(), pShaderManager->GetUIShader(), m_plane->GetWorldMatrix(), view, proj);
	if (!result)
	{
		return false;
	}

	for (int i = 0; i < 4; i++)
	{
		result = m_btn[i].Render(pD3DClass->GetDeviceContext(), pShaderManager->GetUIShader(), m_btn[i].GetWorldMatrix(), view, proj);
		if (!result)
		{
			return false;
		}
	}

	result = m_textBox->Render(pD3DClass->GetDeviceContext(), pTextClass, pShaderManager->GetUIShader(), m_textBox->GetWorldMatrix(), view, proj);
	if (!result)
	{
		return false;
	}

	result = m_textBox2->Render(pD3DClass->GetDeviceContext(), pTextClass, pShaderManager->GetUIShader(), m_textBox2->GetWorldMatrix(), view, proj);
	if (!result)
	{
		return false;
	}

	return true;
}

void TestCanvas::ToggleActive()
{
	m_active ^= true;
}

void TestCanvas::SetActive(bool state)
{
	m_active = state;
}

bool TestCanvas::GetActive()
{
	return m_active;
}