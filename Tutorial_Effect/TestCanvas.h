#ifndef _TEST_CANVAS_
#define _TEST_CANVAS_

#include "D3DClass.h"
#include "TextClass.h"
#include "InputClass.h"
#include "ShaderManager.h"
#include "CameraClass.h"
#include "EventClass.h"

//Elements
#include "Button.h"
#include "Plane.h"
#include "TextBox.h"

class TestCanvas
{
public:
	TestCanvas();
	~TestCanvas();

	bool Initialize(ID3D11Device*, TextClass*);
	void Frame(D3DClass*, HWND, ShaderManager*, TextClass*, CameraClass*);
	bool Render(D3DClass*, TextClass*, ShaderManager*, const XMMATRIX&, const XMMATRIX&);
	void ToggleActive();
	void SetActive(bool state);
	bool GetActive();

private:
	Button* m_btn;
	Plane* m_plane;
	TextBox* m_textBox;
	TextBox* m_textBox2;

	bool m_active;
};

#endif