#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

#include <Windows.h>
#include <fstream>
#include <string>

#include "ApplicationClass.h"
#include "InputClass.h"
#include "FrameTimer.h"
#include "EventClass.h"

using namespace std;

class SystemClass
{
public:
	SystemClass();
	~SystemClass();
	SystemClass(const SystemClass&);

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool InitializeWindow();
	void InitializeDisplaySettings();
	bool Frame();

	bool SearchOptions(string, const char*);
	bool StringToBool(string);
private:
	LPCWSTR m_applicationName;	//응용 프로그램 이름
	HINSTANCE m_hinstance;		//인스턴스 핸들
	HWND m_hwnd;				//윈도우 핸들

	ApplicationClass* m_applicationClass;
	FrameTimer* m_frameTimer;

};

static SystemClass* ApplicationHandle = nullptr;
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#endif