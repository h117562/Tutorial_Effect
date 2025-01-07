#include "EffectShaderClass.h"

EffectShaderClass::EffectShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_noiseBuffer = 0;
	m_samplerState = 0;
}

EffectShaderClass::~EffectShaderClass()
{
}

EffectShaderClass::EffectShaderClass(const EffectShaderClass& other)
{
}

bool EffectShaderClass::Initialize(ID3D11Device* pDevice, HWND hwnd)
{
	HRESULT result;
	ID3DBlob* vertexShaderBuffer = 0;
	ID3DBlob* pixelShaderBuffer = 0;
	ID3DBlob* errorMessage = 0;

	unsigned int numElements;

	const wchar_t* vsFilename = L"../EffectVS.hlsl";
	const wchar_t* psFilename = L"../EffectPS.hlsl";

	//HLSL ������
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		//���Ϸκ��� �����Ͽ� �������� ��� ������ �޽��� �ڽ��� ����
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd);
		}
		//���� ���� �޽����� ���� ��� ������ ã�� �� ���ٴ� �ǹ�
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing EffectVS Shader File", MB_OK);
		}

		return false;
	}

	//HLSL ������
	result = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		//���Ϸκ��� �����Ͽ� �������� ��� ������ �޽��� �ڽ��� ����
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd);
		}
		//���� ���� �޽����� ���� ��� ������ ã�� �� ���ٴ� �ǹ�
		else
		{
			MessageBox(hwnd, psFilename, L"Missing EffectPS Shader File", MB_OK);
		}

		return false;
	}

	//���� ���̴� ���� (������ �� ���̴� ����)
	result = pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//�ȼ� ���̴� ���� (������ �� ���̴� ����)
	result = pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";//Semantic ���
	polygonLayout[0].SemanticIndex = 0;//���� Semantic ��� ���� ��ȣ
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//��� ������ ����
	polygonLayout[0].InputSlot = 0;//0~15
	polygonLayout[0].AlignedByteOffset = 0;//��Ұ� ���۵Ǵ� ��ġ(���۾� �������� ����)
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;//������ ������
	polygonLayout[0].InstanceDataStepRate = 0;//D3D11_INPUT_PER_VERTEX_DATA�� 0���� ����

	polygonLayout[1].SemanticName = "TEXCOORD";//Semantic ���
	polygonLayout[1].SemanticIndex = 0;//���� Semantic ��� ���� ��ȣ
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;//��� ������ ����
	polygonLayout[1].InputSlot = 0;//0~15
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;//�ڵ� ���
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;//������ ������
	polygonLayout[1].InstanceDataStepRate = 0;//D3D11_INPUT_PER_VERTEX_DATA�� 0���� ����

	//�Է� �������� ����
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//���� �Է� ���̾ƿ� ����
	result = pDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	//���̴� ���� ����
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	D3D11_BUFFER_DESC matrixBufferDesc;

	ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));

	//��Ʈ���� ���� ����
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);//���� ũ��
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;//GPU(�б� ����)�� CPU(���� ����)���� �׼��� ���� (�� ������ ���� ������Ʈ)
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//���۸� ��� ���۷� ���ε�
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//CPU �׼��� ����
	matrixBufferDesc.MiscFlags = 0;//���ҽ��� ���� �÷���
	matrixBufferDesc.StructureByteStride = 0;

	//��Ʈ���� ���� ����
	result = pDevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC noiseBufferDesc;

	ZeroMemory(&noiseBufferDesc, sizeof(D3D11_BUFFER_DESC));

	//��Ʈ���� ���� ����
	noiseBufferDesc.ByteWidth = sizeof(NoiseBufferType);//���� ũ��
	noiseBufferDesc.Usage = D3D11_USAGE_DYNAMIC;//GPU(�б� ����)�� CPU(���� ����)���� �׼��� ���� (�� ������ ���� ������Ʈ)
	noiseBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//���۸� ��� ���۷� ���ε�
	noiseBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//CPU �׼��� ����
	noiseBufferDesc.MiscFlags = 0;//���ҽ��� ���� �÷���
	noiseBufferDesc.StructureByteStride = 0;

	//��Ʈ���� ���� ����
	result = pDevice->CreateBuffer(&noiseBufferDesc, nullptr, &m_noiseBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_SAMPLER_DESC sampDesc;

	//�޸� �ʱ�ȭ
	ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));

	//Sampler State ����
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//�ؽ�ó ���͸� ��� (���� ����)
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;//�ؽ�ó ��ǥ�� 0���� 1���̸� �ݺ��ϴ� ����� ������
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;//�� ��쿡�� 1�� ������ �������� ��ǥ�� ���
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;//D3D11_TEXTURE_ADDRESS_CLAMP�� �ʰ��ϰų� �̸��� ��� 0 �Ǵ� 1�� ����
	sampDesc.MipLODBias = 0.0f;//Mipmap ������
	sampDesc.MaxAnisotropy = 1;//1~16
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;//�׻� �� ��� (��� �񱳰� true)
	sampDesc.MinLOD = 0;//�ּ� Mipmap ����
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;//�ִ� Mipmap ����

	//Sampler State ����
	result = pDevice->CreateSamplerState(&sampDesc, &m_samplerState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool EffectShaderClass::Render(ID3D11DeviceContext* pDeviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, 
	float frameTime, XMFLOAT3 scrollSpeeds, XMFLOAT3 scales)
{
	bool result;

	//���� ������Ʈ
	result = UpdateShaderBuffers(pDeviceContext, worldMatrix, viewMatrix, projectionMatrix,
		frameTime, scrollSpeeds, scales);
	if (!result)
	{
		return false;
	}

	pDeviceContext->IASetInputLayout(m_layout);//�Է� ���̾ƿ� ���ε�
	pDeviceContext->VSSetShader(m_vertexShader, NULL, 0);
	pDeviceContext->PSSetShader(m_pixelShader, NULL, 0);
	pDeviceContext->PSSetSamplers(0, 1, &m_samplerState);

	return true;
}

void EffectShaderClass::Shutdown()
{
	if (m_samplerState)
	{
		m_samplerState->Release();
		m_samplerState = 0;
	}

	if (m_noiseBuffer)
	{
		m_noiseBuffer->Release();
		m_noiseBuffer = 0;
	}

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

//���� ������ �޽��� �ڽ��� ���
void EffectShaderClass::OutputShaderErrorMessage(ID3DBlob* errorMessage, HWND hwnd)
{
	char* compileErrors;

	//���� �޽��� �ؽ�Ʈ ���ۿ� ���� �����͸� ������
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//�޽��� �ڽ� ����
	MessageBoxA(hwnd, compileErrors, "Error", MB_OK);

	//����
	errorMessage->Release();
	errorMessage = 0;

	return;
}


//���̴����� ����ϴ� ���۸� ������Ʈ
bool EffectShaderClass::UpdateShaderBuffers(ID3D11DeviceContext* pDeviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	float frameTime, XMFLOAT3 speeds, XMFLOAT3 scales)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr0 = 0;
	NoiseBufferType* dataPtr1 = 0;
	UINT bufferNum, bufferCount;

	////////////////////////MatrixBufferType////////////////////////
	//��� ��ġ
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//��� ���۸� ��� (GPU �׼��� ��Ȱ��ȭ) 
	result = pDeviceContext->Map(m_matrixBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//���� ���̴��� ��� ������ �����͸� ����
	dataPtr0 = (MatrixBufferType*)mappedResource.pData;

	//�Ű������� ������ �����ͷ� ������Ʈ
	dataPtr0->world = worldMatrix;
	dataPtr0->view = viewMatrix;
	dataPtr0->projection = projectionMatrix;

	//��� ���۸� ��� ���� (GPU �׼��� �ٽ� Ȱ��ȭ)
	pDeviceContext->Unmap(m_matrixBuffer, NULL);

	bufferNum = 0;
	bufferCount = 1;

	//���� ���̴��� ��� ���۸� ������Ʈ
	pDeviceContext->VSSetConstantBuffers(bufferNum, bufferCount, &m_matrixBuffer);

	////////////////////////NoiseBufferType////////////////////////
		
	//��� ���۸� ��� (GPU �׼��� ��Ȱ��ȭ) 
	result = pDeviceContext->Map(m_noiseBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//���� ���̴��� ��� ������ �����͸� ����
	dataPtr1 = (NoiseBufferType*)mappedResource.pData;

	//�Ű������� ������ �����ͷ� ������Ʈ
	dataPtr1->frameTime = frameTime;
	dataPtr1->speeds = speeds;
	dataPtr1->scales = scales;

	//��� ���۸� ��� ���� (GPU �׼��� �ٽ� Ȱ��ȭ)
	pDeviceContext->Unmap(m_noiseBuffer, NULL);

	bufferNum = 0;
	bufferCount = 1;

	//���� ���̴��� ��� ���۸� ������Ʈ
	pDeviceContext->PSSetConstantBuffers(bufferNum, bufferCount, &m_noiseBuffer);

	return true;
}
