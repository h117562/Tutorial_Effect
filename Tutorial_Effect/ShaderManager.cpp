#include "ShaderManager.h"

ShaderManager::ShaderManager()
{
	m_uiShader = 0;
}


ShaderManager::~ShaderManager()
{
}

ShaderManager::ShaderManager(const ShaderManager& other)
{
}

bool ShaderManager::Initialize(ID3D11Device* pDevice, HWND hwnd)
{
	bool result;

	m_uiShader = new UIShaderClass;
	if (!m_uiShader)
	{
		return false;
	}

	result = m_uiShader->Initialize(pDevice, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the TextureShader.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void ShaderManager::Shutdown()
{

	if (m_uiShader)
	{
		m_uiShader->Shutdown();
		delete m_uiShader;
		m_uiShader = 0;
	}

	return;
}

UIShaderClass* ShaderManager::GetUIShader()
{
	return m_uiShader;
}