#include "D3DClass.h"

D3DClass::D3DClass()
{
	m_device = 0;
	m_deviceContext = 0;
	m_swapChain = 0;
	m_renderTargetView = 0;

	m_depthStencilState = 0;
	m_depthDisabledStencilState = 0;
	m_depthStencilView = 0;
	m_depthStencilBuffer = 0;

	m_alphaEnableBlendingState = 0;
	m_alphaDisableBlendingState = 0;

	m_rasterState = 0;
	m_rasterStateWireframe = 0;
}

D3DClass::~D3DClass()
{
	if (m_alphaDisableBlendingState)
	{
		m_alphaDisableBlendingState->Release();
		m_alphaDisableBlendingState = 0;
	}

	if (m_alphaEnableBlendingState)
	{
		m_alphaEnableBlendingState->Release();
		m_alphaEnableBlendingState = 0;
	}

	if (m_mirrorState)
	{
		m_mirrorState->Release();
		m_mirrorState = 0;
	}

	if (m_maskState)
	{
		m_maskState->Release();
		m_maskState = 0;
	}

	if (m_depthDisabledStencilState)
	{
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = 0;
	}

	if (m_rasterStateWireframe)
	{
		m_rasterStateWireframe->Release();
		m_rasterStateWireframe = 0;
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

}

D3DClass::D3DClass(const D3DClass &other)
{
}

bool D3DClass::Initialize(HWND hwnd)
{
	HRESULT result;

	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_BLEND_DESC blendStateDesc;

	float fieldOfView, screenAspect;

	//기본 새로고침 빈도 초기화
	numerator = 60;		//분자
	denominator = 1;	//분모

	//DXGI 팩토리 생성
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	//0번째 어댑터를 가져옴
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	//0번째 출력장치를 가져옴
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	//출력 장치로부터 해당 픽셀 포맷을 사용하는 디스플레이 모드 리스트를 찾음
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//모드 리스트의 개수를 초기화
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	//배열 초기화 후 MODE_DESC 저장
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	//화면 크기와 일치하는 모드를 찾아 새로고침 빈도를 저장
	for (UINT i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == SCREEN_WIDTH)
		{
			if (displayModeList[i].Height == SCREEN_HEIGHT)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//해당 어댑터로부터 정보를 가져옴
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}
	
	//그래픽 카드의 메모리 크기를 저장 (MB)
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//그래픽 카드의 이름을 저장
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	//이제 쓸모없는 자원을 반환
	delete[] displayModeList;
	displayModeList = 0;

	adapterOutput->Release();
	adapterOutput = 0;

	adapter->Release();
	adapter = 0;

	factory->Release();
	factory = 0;


	//메모리 초기화
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	//스왑 체인 설정
	swapChainDesc.BufferCount = 2;						//백 버퍼 갯수
	swapChainDesc.BufferDesc.Width = SCREEN_WIDTH;		//백 버퍼 너비
	swapChainDesc.BufferDesc.Height = SCREEN_HEIGHT;		//백 버퍼 높이
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//픽셀 포맷
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //백 버퍼 사용 방식 설정
	swapChainDesc.OutputWindow = hwnd;	//출력할 윈도우의 Hwnd 핸들 
	swapChainDesc.SampleDesc.Count = 1;	//샘플 갯수 설정(1은 다중 샘플링을 끔) ex)슈퍼 샘플링, 멀티 샘플링
	swapChainDesc.SampleDesc.Quality = 0;//품질 레벨 설정(0은 다중 샘플링을 끔)
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//스캔라인 순서 기본
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;//Scaling 기본
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;//현재 백버퍼를 버리고 새로 그리기
	swapChainDesc.Flags = 0;//플래그 없음

	//새로고침 빈도 설정
	if (VSYNC_ENABLED)
	{
		//Vsync가 켜져있을 경우 모니터의 설정을 따라감
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		//아니라면 제한 없음
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//전체화면 설정
	if (FULL_SCREEN)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	//기능 수준 설정
	featureLevel = D3D_FEATURE_LEVEL_11_1;

	//D3DDevice, 스왑체인 그리고 DeviceContext 생성
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	//백 버퍼 포인터를 얻음
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	//랜더 타겟 뷰 생성
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	//포인터 해제
	backBufferPtr->Release();
	backBufferPtr = 0;


	//메모리 초기화
	ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

	//깊이 버퍼 설정
	depthBufferDesc.Width = SCREEN_WIDTH;	//너비
	depthBufferDesc.Height = SCREEN_HEIGHT;	//높이
	depthBufferDesc.MipLevels = 1;//Mipmap 수준 
	depthBufferDesc.ArraySize = 1;//텍스처 개수
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//텍스처 형식
	depthBufferDesc.SampleDesc.Count = 1;//픽셀당 샘플 갯수 설정
	depthBufferDesc.SampleDesc.Quality = 0;//이미지 품질 수준
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;//텍스처를 읽고 쓰는 방법
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;//파이프라인 스테이지 바인딩 플래그
	depthBufferDesc.CPUAccessFlags = 0;//CPU 액세스 유형 (0는 사용안함)
	depthBufferDesc.MiscFlags = 0;

	//깊이 버퍼 생성
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}


	//메모리 초기화
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	//깊이 스텐실 State 설정
	depthStencilDesc.DepthEnable = true;//깊이 테스트 활성화
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;//깊이 스텐실 버퍼에 쓰기를 켬
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;//깊이 비교 함수 - 원본 데이터가 대상 데이터보다 작으면 비교 통과

	depthStencilDesc.StencilEnable = true;//스텐실 테스트 활성화
	depthStencilDesc.StencilReadMask = 0xFF;//읽기 마스크 값 - 모든 8비트
	depthStencilDesc.StencilWriteMask = 0xFF;//쓰기 마스크 값 - 모든 8비트

	//앞면 픽셀의 경우 작업할 스텐실
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;//스텐실 테스트 실패 시 데이터 유지
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;//깊이 테스트 실패 시 스텐실 값 1 증분
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;//둘다 성공 시 데이터 유지
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;//스텐실 데이터 비교 함수

	//뒷면 픽셀의 경우 작업할 스텐실
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;//스텐실 값을 1 감소
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;//항상 비교

	//깊이 스텐실 State 생성
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	//깊이 스텐실 State 적용
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	//2D 깊이 스텐실 State 설정
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;

	//앞면 픽셀의 경우 작업할 스텐실
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//뒷면 픽셀의 경우 작업할 스텐실
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//2D 깊이 스텐실 상태 생성
	result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if (FAILED(result))
	{
		return false;
	}

	//메모리 초기화
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	//깊이 스텐실 뷰 설정
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//깊이 스텐실 뷰 생성
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	//렌더 타겟 뷰와 깊이 스텐실 뷰를 출력 렌더 파이프 라인에 바인딩
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	//기본 레스터라이저 설정 
	rasterDesc.FillMode = D3D11_FILL_SOLID;//폴리곤을 그림
	rasterDesc.CullMode = D3D11_CULL_NONE;//폴리곤 양면을 그림
	rasterDesc.FrontCounterClockwise = false;//시계 방향으로 그려진 폴리곤이 앞면
	rasterDesc.DepthBias = 0;//깊이 바이어스 깊이 값
	rasterDesc.DepthBiasClamp = 0.0f;//픽셀의 최대 깊이 바이어스
	rasterDesc.SlopeScaledDepthBias = 0.0f;//픽셀의 기울기에서 스칼라
	rasterDesc.DepthClipEnable = true;//거리에 따라 클리핑을 사용하도록 설정
	rasterDesc.ScissorEnable = false;//가위 직사각형 컬링을 사용하지 않도록 설정
	rasterDesc.MultisampleEnable = false;//다중 샘플 안티앨리어싱 사용 안함
	rasterDesc.AntialiasedLineEnable = false;//라인 안티앨리언싱 사용안함


	//기본 레스터라이저 상태 생성
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	//기본 레스터라이저 상태 적용
	m_deviceContext->RSSetState(m_rasterState);

	//와이어 프레임 모드 레스터라이저 설정
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	//와이어 프레임 모드 상태 생성
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterStateWireframe);
	if (FAILED(result))
	{
		return false;
	}

	ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));

	//DirectX가 클립 공간 좌표를 렌더링 대상 공간에 매핑할 수 있도록 뷰포트를 설정함
	m_viewport.Width = (float)SCREEN_WIDTH;
	m_viewport.Height = (float)SCREEN_HEIGHT;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	//뷰포트 바인딩
	m_deviceContext->RSSetViewports(1, &m_viewport);

	//투영 행렬 설정
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

	//기본 월드 행렬 생성
	m_worldMatrix = DirectX::XMMatrixIdentity();

	//Perspective Projection 행렬 생성
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, SCREEN_NEAR, SCREEN_FAR);

	//2D 렌더링을 위해 Orthographic Projection(직교 투영) 행렬 생성
	m_orthoMatrix = DirectX::XMMatrixOrthographicLH((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, SCREEN_NEAR, SCREEN_FAR);

	//변수 초기화
	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));

	//블랜드 상태 설정(알파 블랜딩)
	blendStateDesc.AlphaToCoverageEnable = false;
	blendStateDesc.IndependentBlendEnable = false;
	blendStateDesc.RenderTarget[0].BlendEnable = true;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;				//SourceBlend 현재 픽셀에 대한 블랜드
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;		//DestinationBlend 이전에 그려진 픽셀들에 대한 블랜드
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//알파 블랜딩 상태 생성
	result = m_device->CreateBlendState(&blendStateDesc, &m_alphaEnableBlendingState);
	if (FAILED(result))
	{
		return false;
	}

	//블랜딩 비활성 상태 설정
	blendStateDesc.RenderTarget[0].BlendEnable = false;

	//블랜딩 비활성 상태 생성
	result = m_device->CreateBlendState(&blendStateDesc, &m_alphaDisableBlendingState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void D3DClass::Shutdown()
{
	return;
}

void D3DClass::BeginScene(float r, float g, float b, float a) 
{
	float color[4];

	//버퍼를 채울 색상을 설정
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	//백 버퍼 초기화
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	//깊이 버퍼 초기화
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene() 
{
	//백 버퍼를 화면에 표시
	if (VSYNC_ENABLED)
	{
		//동기화
		m_swapChain->Present(1, 0);
	}
	else
	{
		//동기화 없음
		m_swapChain->Present(0, 0);
	}
	
	return;
}

ID3D11Device* D3DClass::GetDevice() 
{
	return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

IDXGISwapChain* D3DClass::GetSwapChain() 
{
	return m_swapChain;
}

void D3DClass::GetProjectionMatrix(DirectX::XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void D3DClass::GetWorldMatrix(DirectX::XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

void D3DClass::GetOrthoMatrix(DirectX::XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

void D3DClass::GetVideoCardInfo(char* description, int& memory)
{
	strcpy_s(description, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

void D3DClass::TurnZBufferOn()
{
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	return;
}

void D3DClass::TurnZBufferOff()
{
	m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
	return;
}

void D3DClass::EnableAlphaBlending()
{
	float blendFactor[4];

	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);

	return;
}

void D3DClass::DisableAlphaBlending()
{
	float blendFactor[4];

	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);

	return;
}

void D3DClass::EnableWireframe()
{
	//와이어 프레임 모드 적용
	m_deviceContext->RSSetState(m_rasterStateWireframe);

	return;
}

void D3DClass::DisableWireframe() 
{
	//와이어 프레임 모드 해제
	m_deviceContext->RSSetState(m_rasterState);

	return;
}

