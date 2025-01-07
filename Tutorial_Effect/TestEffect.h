#ifndef _TEST_EFFECT_
#define _TEST_EFFECT_

#include <d3d11.h>
#include <directxmath.h>

#include "TextureResource.h"
#include "RectTexture2D.h"
#include "ShaderManager.h"

class TestEffect : private RectTexture2D
{
public:
	TestEffect()
	{

	}

	~TestEffect()
	{

	}

	HRESULT Initialize(ID3D11Device* pDevice)
	{
		HRESULT result;

		result = InitializeBuffer(pDevice);
		if (FAILED(result))
		{
			return result;
		}

		const wchar_t* filepath[3] = {
			L"..//data//Effect//fire01.png",
			L"..//data//Effect//noise01.png",
			L"..//data//Effect//alpha01.png" };

		result = SetTexture(pDevice, filepath, 3);
		if (FAILED(result))
		{
			return result;
		}

		return S_OK;
	}

	void Render(ID3D11DeviceContext* pDeviceContext)
	{
		TestDraw(pDeviceContext);
	}

private:

};

#endif