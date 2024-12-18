
//#pragma comment(linker , "/entry:WinMainCRTStartup /subsystem:console")

#include <Windows.h>

#include "Global.h"
#include "Systemclass.h"

//hInstance 인스턴스 핸들, 메모리에 로드되는 exe 식별용
//hPrevInstance 안쓰는 변수
//pScmdline 유니코드 argv 명령줄
//nCmdshow ShowWindow() 커맨드 0~11 정도 있음 ex) SW_HIDE 0
//Window는 여기서 구현하지 않고 SystemClass에서 따로

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdshow)
{
	bool result;

	SystemClass* systemClass;

	//SystemClass 생성
	systemClass = new SystemClass;
	if (!systemClass)
	{
		return 0;
	}

	//SystemClass 초기화
	result = systemClass->Initialize();
	if (result)
	{
		systemClass->Run();
	}

	//System Class의 모든 메모리 반환
	systemClass->Shutdown();
	delete systemClass;
	systemClass = 0;

	return 0;
}