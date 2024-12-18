#ifndef _SHADER_MANAGER_H_
#define _SHADER_MANAGER_H_

#include "UIShaderClass.h"

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();
	ShaderManager(const ShaderManager&);

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	UIShaderClass* GetUIShader();

private:
	UIShaderClass* m_uiShader;

};

#endif