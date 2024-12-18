#include "textclass.h"

TextClass::TextClass()
{
	m_d2dFactory = 0;
	m_dwFactory = 0;
	m_defaultFormat = 0;
	m_renderTarget = 0;
	m_defaultBrush = 0;
}

TextClass::~TextClass()
{
}

TextClass::TextClass(const TextClass& other)
{
}

bool TextClass::Initialize(IDXGISwapChain* pSwapChain)
{
	HRESULT result;
	IDXGISurface* backBuffer = 0;
	D2D1_RENDER_TARGET_PROPERTIES props;

	//D3D의 스왑체인의 첫번째 버퍼 포인터를 얻음
	result = pSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	if (FAILED(result))
	{
		return false;
	}

	//메모리 초기화
	ZeroMemory(&props, sizeof(D2D1_RENDER_TARGET_PROPERTIES));

	//Direct2D 렌더링 옵션 설정
	props.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
	props.dpiX = 0.0f;//0 기본
	props.dpiY = 0.0f;//0 기본
	props.usage = D2D1_RENDER_TARGET_USAGE_NONE;
	props.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
	props.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);

	//D2D 팩토리 생성 (팩토리에 액세스하거나 쓰기 위한 동기화가 제공되지 않음)
	result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_d2dFactory);
	if (FAILED(result))
	{
		return false;
	}

	//렌더 타켓 생성
	result = m_d2dFactory->CreateDxgiSurfaceRenderTarget(
		backBuffer,
		&props,
		&m_renderTarget
	);

	//백버퍼 포인터 해제
	backBuffer->Release();
	backBuffer = 0;

	if (FAILED(result))
	{
		return false;
	}

	//Dwrite 팩토리 생성
	result = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,	//해당 팩토리를 공유할지 격리할지 정하는 플래그
		__uuidof(IDWriteFactory),	//팩토리 인터페이스를 식별하는 GUID
		reinterpret_cast<IUnknown**>(&m_dwFactory)//팩토리에 대한 포인터 주소
	);

	if (FAILED(result))
	{
		return false;
	}

	//폰트 설정
	result = m_dwFactory->CreateTextFormat(
		L"굴림",	//폰트 이름
		NULL,					//폰트 컬렉션에 대한 포인터 주소 NULL은 시스템 폰트 컬렉션
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL,//폰트 두께
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,//폰트 스타일
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,//폰트 스트레치
		30.0f,					//폰트 사이즈
		L"ko",					//지역 이름 ex) KO, EN
		&m_defaultFormat		//텍스트 형식(IDWriteTextFormat)에 대한 포인터 주소를 반환한다
	);

	if (FAILED(result))
	{
		return false;
	}

	//기본 브러쉬 설정 (초록)
	result = m_renderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF(0.0f, 1.0f, 0.0f, 1.0f)),
		&m_defaultBrush
	);
	if (FAILED(result))
	{
		return false;
	}

	//기본 텍스트 정렬 (왼쪽)
	result = m_defaultFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

HRESULT TextClass::CreateTextFormat(IDWriteTextFormat** pFormat, const wchar_t* fontName, DWRITE_FONT_WEIGHT weight, DWRITE_FONT_STYLE style, DWRITE_FONT_STRETCH stretch, float fontSize)
{
	HRESULT result;

	//폰트 설정
	result = m_dwFactory->CreateTextFormat(
		fontName,		//폰트 이름
		NULL,			//폰트 컬렉션에 대한 포인터 주소 NULL은 시스템 폰트 컬렉션
		weight,			//폰트 두께
		style,			//폰트 스타일
		stretch,		//폰트 스트레치
		fontSize,		//폰트 사이즈
		L"ko",			//지역 이름 ex) KO, EN
		pFormat			//텍스트 형식(IDWriteTextFormat)에 대한 포인터 주소를 반환한다
	);

	return result;
}

HRESULT TextClass::CreateTextBrush(ID2D1SolidColorBrush** pBrush, float r, float g, float b, float a)
{
	HRESULT result;

	//기본 브러쉬 설정 (초록)
	result = m_renderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF(r, g, b, a)),
		pBrush
	);

	return result;
}



//텍스트 렌더링 기본 폰트, 컬러
void TextClass::RenderText(const wchar_t* ptext, const D2D1_RECT_F& renderRect)
{
	m_renderTarget->DrawTextW(
		ptext,
		wcslen(ptext),
		m_defaultFormat,
		renderRect,
		m_defaultBrush,
		D2D1_DRAW_TEXT_OPTIONS_NONE,
		DWRITE_MEASURING_MODE_NATURAL
	);
}

//텍스트 렌더링 지정 폰트, 컬러
void TextClass::RenderText(const wchar_t* ptext, const D2D1_RECT_F& renderRect, IDWriteTextFormat* pformat, ID2D1SolidColorBrush* pbrush)
{
	m_renderTarget->DrawTextW(
		ptext,
		wcslen(ptext),
		pformat,
		renderRect,
		pbrush,
		D2D1_DRAW_TEXT_OPTIONS_NONE,
		DWRITE_MEASURING_MODE_NATURAL
	);
}

//텍스트 렌더링 시작 함수
void TextClass::BeginDraw()
{
	m_renderTarget->BeginDraw();
}

//텍스트 렌더링 종료 함수
void TextClass::EndDraw()
{
	m_renderTarget->EndDraw();
}

void TextClass::Shutdown()
{
	//브러쉬 해제
	if (m_defaultBrush)
	{
		m_defaultBrush->Release();
		m_defaultBrush = 0;
	}

	//텍스트 형식 해제
	if (m_defaultFormat)
	{
		m_defaultFormat->Release();
		m_defaultFormat = 0;
	}

	//렌더 타겟 해제
	if (m_renderTarget)
	{
		m_renderTarget->Release();
		m_renderTarget = 0;
	}

	//팩토리 해제
	if (m_dwFactory)
	{
		m_dwFactory->Release();
		m_dwFactory = 0;
	}

	//팩토리 해제
	if (m_d2dFactory)
	{
		m_d2dFactory->Release();
		m_d2dFactory = 0;
	}

}