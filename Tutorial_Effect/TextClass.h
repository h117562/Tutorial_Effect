#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_

#include <dwrite.h>
#include <d2d1.h>

class TextClass
{
public:
	TextClass();
	~TextClass();
	TextClass(const TextClass&);

	bool Initialize(IDXGISwapChain*);
	void Shutdown();

	void RenderText(const wchar_t* text, const D2D1_RECT_F& renderRect);
	void RenderText(const wchar_t* text, const D2D1_RECT_F& renderRect, IDWriteTextFormat*, ID2D1SolidColorBrush*);
	void BeginDraw();
	void EndDraw();

	HRESULT CreateTextFormat(IDWriteTextFormat**, const wchar_t* FontName, DWRITE_FONT_WEIGHT FontWeight, DWRITE_FONT_STYLE FontStyle, DWRITE_FONT_STRETCH FontStretch, float FontSize);
	HRESULT CreateTextBrush(ID2D1SolidColorBrush**, float r, float g, float b, float a);

private:
	ID2D1Factory* m_d2dFactory;
	IDWriteFactory* m_dwFactory;
	ID2D1RenderTarget* m_renderTarget;	
	IDWriteTextFormat* m_defaultFormat;
	ID2D1SolidColorBrush* m_defaultBrush;
};

#endif