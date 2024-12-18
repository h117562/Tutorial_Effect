#ifndef _GRAPHIC_STRUCTURES_H_
#define _GRAPHIC_STRUCTURES_H_

#include <directxmath.h>

struct VertexUV
{
	DirectX::XMVECTOR position;
	DirectX::XMFLOAT2 textureCoord;
};

struct MatrixBuffer
{
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
};

#endif