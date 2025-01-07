#include "ShaderManager.h"

ShaderManager::ShaderManager()
{
	m_effectShader = 0;
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

	m_effectShader = new EffectShaderClass;
	if (!m_effectShader)
	{
		return false;
	}

	result = m_effectShader->Initialize(pDevice, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the EffectShader.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void ShaderManager::Shutdown()
{
	if (m_effectShader)
	{
		m_effectShader->Shutdown();
		delete m_effectShader;
		m_effectShader = 0;
	}

	return;
}

EffectShaderClass* ShaderManager::GetEffectShader()
{
	return m_effectShader;
}