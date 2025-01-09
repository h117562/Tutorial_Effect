#ifndef _EFFECT_SHADER_CLASS_
#define _EFFECT_SHADER_CLASS_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <string>

using namespace DirectX;

class EffectShaderClass
{
private:
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    struct NoiseBufferType
    {
        XMFLOAT3 padding;
        float frameTime;
    };

public:
    EffectShaderClass();
    EffectShaderClass(const EffectShaderClass&);
    ~EffectShaderClass();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, const XMMATRIX&, const XMMATRIX&, const XMMATRIX&, 
        float);

private:
    void OutputShaderErrorMessage(ID3DBlob*, HWND);
    bool UpdateShaderBuffers(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX,
        float);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_noiseBuffer;
	ID3D11SamplerState* m_samplerState;
};

#endif