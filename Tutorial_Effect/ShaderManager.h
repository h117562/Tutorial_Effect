#ifndef _SHADER_MANAGER_H_
#define _SHADER_MANAGER_H_

#include "EffectShaderClass.h"

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();
	ShaderManager(const ShaderManager&);

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();

	EffectShaderClass* GetEffectShader();

private:
	EffectShaderClass* m_effectShader;

};

#endif