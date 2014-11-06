#include "Constants.h"
#include "Direct3D.h"
#include "Input.h"
#include "Logger.h"
#include <iostream>

HWND				g_hWnd;
Input				g_input;
HRESULT             InitWindow( HINSTANCE hInstance, int nCmdShow );
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	std::ofstream logFile("logFile.txt", std::ofstream::trunc);

	Logger::addOutput(Logger::Level::DEBUG_L, logFile);
	Logger::addOutput(Logger::Level::INFO, std::cout);

	if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
	{
		Logger::log(Logger::Level::ERROR_L, "Initializing window failed!");
		return -1;
	}

	Direct3D D3D(g_hWnd);
	Logger::log(Logger::Level::INFO, "Initializing input...");
	g_input = Input();

	D3D.init(&g_input);
	

	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;

	__int64 prevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);
	Logger::log(Logger::Level::INFO, "Initializing complete starting program");
	// Main message loop
	MSG msg = {0};
	while(WM_QUIT != msg.message)
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			__int64 currTimeStamp = 0;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
			float dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;
			POINT tempSize;
			tempSize.x = (LONG)(WIDTH * 0.5f);
			tempSize.y = (LONG)(HEIGHT * 0.5f);
			ClientToScreen(g_hWnd, &tempSize);
			SetCursorPos(tempSize.x, tempSize.y);
			ShowCursor(false);
			//render
			D3D.update(dt);
			D3D.draw();
			prevTimeStamp = currTimeStamp;
		}
	}

	D3D.release();
	logFile.close();
	return (int) msg.wParam;
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:

		switch(wParam)
		{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
		}
		break;
	case WM_INPUT:
		{
			g_input.update(lParam);
		}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	Logger::log(Logger::Level::INFO, "Initializing window...");

	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = 0;
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = "BTH_D3D_Template";
	wcex.hIconSm        = 0;
	if( !RegisterClassEx(&wcex) )
		return E_FAIL;

	// Create window
	 
	RECT rc = { 0, 0, (LONG)WIDTH, (LONG)HEIGHT };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	
	if(!(g_hWnd = CreateWindow(
							"BTH_D3D_Template",
							"BTH - Direct3D 11.0 Template",
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							rc.right - rc.left,
							rc.bottom - rc.top,
							NULL,
							NULL,
							hInstance,
							NULL)))
	{
		return E_FAIL;
	}

	ShowWindow( g_hWnd, nCmdShow );
	
	return S_OK;
}
