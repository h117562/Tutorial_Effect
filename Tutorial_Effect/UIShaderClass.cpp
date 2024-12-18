#include "UIShaderClass.h"

UIShaderClass::UIShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_samplerState = 0;
	m_bufferCount = 1;
	m_startNumber = 0;
}

UIShaderClass::~UIShaderClass()
{
}

UIShaderClass::UIShaderClass(const UIShaderClass& other)
{
}

bool UIShaderClass::Initialize(ID3D11Device* pDevice, HWND hwnd)
{
	HRESULT result;
	ID3DBlob* vertexShaderBuffer;
	ID3DBlob* pixelShaderBuffer;
	ID3DBlob* errorMessage;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

	unsigned int numElements;

	const wchar_t* vsFilename = L"../TextureVS.hlsl";
	const wchar_t* psFilename = L"../TexturePS.hlsl";

	//초기화
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//HLSL 컴파일
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		//파일로부터 컴파일에 실패했을 경우 오류를 메시지 박스로 띄우기
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd);
		}
		//만약 오류 메시지가 없을 경우 파일을 찾을 수 없다는 의미
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Vs Texture Shader File", MB_OK);
		}

		return false;
	}

	//HLSL 컴파일
	result = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		//파일로부터 컴파일에 실패했을 경우 오류를 메시지 박스로 띄우기
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd);
		}
		//만약 오류 메시지가 없을 경우 파일을 찾을 수 없다는 의미
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Ps Texture Shader File", MB_OK);
		}

		return false;
	}

	//정점 쉐이더 생성 (컴파일 된 쉐이더 전달)
	result = pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//픽셀 쉐이더 생성 (컴파일 된 쉐이더 전달)
	result = pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}
	
	polygonLayout[0].SemanticName = "POSITION";//Semantic 요소
	polygonLayout[0].SemanticIndex = 0;//동일 Semantic 요소 구분 번호
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//요소 데이터 형식
	polygonLayout[0].InputSlot = 0;//0~15
	polygonLayout[0].AlignedByteOffset = 0;//요소가 시작되는 위치(버퍼안 데이터의 간격)
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;//정점당 데이터
	polygonLayout[0].InstanceDataStepRate = 0;//D3D11_INPUT_PER_VERTEX_DATA라서 0으로 고정

	polygonLayout[1].SemanticName = "TEXCOORD";//Semantic 요소
	polygonLayout[1].SemanticIndex = 0;//동일 Semantic 요소 구분 번호
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;//요소 데이터 형식
	polygonLayout[1].InputSlot = 0;//0~15
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;//자동 계산
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;//정점당 데이터
	polygonLayout[1].InstanceDataStepRate = 0;//D3D11_INPUT_PER_VERTEX_DATA라서 0으로 고정

	//입력 데이터의 개수
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//정점 입력 레이아웃 생성
	result = pDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	//쉐이더 버퍼 해제
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	
	D3D11_BUFFER_DESC matrixBufferDesc;

	ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));

	//매트릭스 버퍼 설정
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);//버퍼 크기
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;//GPU(읽기 전용)와 CPU(쓰기 전용)에서 액세스 가능 (매 프레임 마다 업데이트)
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//버퍼를 상수 버퍼로 바인딩
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//CPU 액세스 유형
	matrixBufferDesc.MiscFlags = 0;//리소스에 대한 플래그
	matrixBufferDesc.StructureByteStride = 0;

	//매트릭스 버퍼 생성
	result = pDevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_SAMPLER_DESC sampDesc;

	//메모리 초기화
	ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));

	//Sampler State 설정
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//텍스처 필터링 모드 (선형 보간)
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;//텍스처 좌표가 0에서 1사이를 반복하는 방법을 결정함
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;//이 경우에는 1로 나누어 나머지를 좌표로 사용
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;//D3D11_TEXTURE_ADDRESS_CLAMP는 초과하거나 미만인 경우 0 또는 1로 고정
	sampDesc.MipLODBias = 0.0f;//Mipmap 오프셋
	sampDesc.MaxAnisotropy = 1;//1~16
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;//항상 비교 통과 (모든 비교가 true)
	sampDesc.MinLOD = 0;//최소 Mipmap 레벨
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;//최대 Mipmap 레벨

	//Sampler State 생성
	result = pDevice->CreateSamplerState(&sampDesc, &m_samplerState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool UIShaderClass::Render(ID3D11DeviceContext* pDeviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix)
{
	bool result;

	//버퍼 업데이트
	result = UpdateShaderBuffers(pDeviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//도형 설정(삼각형)
	pDeviceContext->IASetInputLayout(m_layout);//입력 레이아웃 바인딩
	pDeviceContext->VSSetShader(m_vertexShader, NULL, 0);
	pDeviceContext->PSSetShader(m_pixelShader, NULL, 0);
	pDeviceContext->PSSetSamplers(0, 1, &m_samplerState);

	return true;
}

void UIShaderClass::Shutdown()
{
	if (m_samplerState)
	{
		m_samplerState->Release();
		m_samplerState = 0;
	}

	//매트릭스 버퍼 해제
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	//레이아웃 해제
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	//픽셀 쉐이더 해제
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	//정점 쉐이더 해제
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}


//에러 내용을 메시지 박스로 출력
void UIShaderClass::OutputShaderErrorMessage(ID3DBlob* errorMessage, HWND hwnd)
{
	char* compileErrors;

	//에러 메시지 텍스트 버퍼에 대한 포인터를 가져옴
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//메시지 박스 띄우기
	MessageBoxA(hwnd, compileErrors, "Error", MB_OK);

	//해제
	errorMessage->Release();
	errorMessage = 0;

	return;
}

//쉐이더에서 사용하는 버퍼를 업데이트
bool UIShaderClass::UpdateShaderBuffers(ID3D11DeviceContext* pDeviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* dataPtr = 0;

	//행렬 전치
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//상수 버퍼를 잠금 (GPU 액세스 비활성화) 
	result = pDeviceContext->Map(m_matrixBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//정점 쉐이더의 상수 버퍼의 포인터를 저장
	dataPtr = (MatrixBuffer*)mappedResource.pData;

	//매개변수로 가져온 데이터로 업데이트
	dataPtr->worldMatrix = worldMatrix;
	dataPtr->viewMatrix = viewMatrix;
	dataPtr->projectionMatrix = projectionMatrix;

	//상수 버퍼를 잠금 해제 (GPU 액세스 다시 활성화)
	pDeviceContext->Unmap(m_matrixBuffer, NULL);

	//정점 쉐이더의 상수 버퍼를 업데이트
	pDeviceContext->VSSetConstantBuffers(m_startNumber, m_bufferCount, &m_matrixBuffer);

	return true;
}