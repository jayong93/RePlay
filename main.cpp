// Win32Project1.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include <Windows.h>
#include <windowsx.h>
#include <memory>
#include "include\Box2D.h"

#define MAX_LOADSTRING 100
const float timeStep = 1 / 120.0f;
b2World* world{ nullptr };
DWORD prevTime{ 0 }, currentTime{ 0 };

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING] = L"STL Replay";                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING] = L"STLReplay";            // 기본 창 클래스 이름입니다.

												// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.

	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg;
	msg.message = 0;

	// 기본 메시지 루프입니다.
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			currentTime = GetTickCount();
			if (currentTime - prevTime >= 1000 * timeStep)
			{
				world->Step(timeStep, 8, 3);
				world->ClearForces();
				prevTime = currentTime;
			}
		}
	}

	delete world;

	return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

void CALLBACK TimerProc(HWND hWnd, UINT msg, UINT_PTR iEvent, DWORD timeElapsed)
{
	InvalidateRect(hWnd, nullptr, false);
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static RECT cRect;
	static const float PPU = 40;
	static b2Body* groundBody;
	static b2Body* moveBody;

	switch (message)
	{
	case WM_CREATE:
	{
		world = new b2World{ b2Vec2{ 0,9.8f } };
		world->SetAllowSleeping(true);
		world->SetWarmStarting(true);

		// 바닥 생성
		b2BodyDef bodyDef;
		bodyDef.position.Set(4.0f, 7.5f);
		groundBody = world->CreateBody(&bodyDef);

		b2PolygonShape shape;
		shape.SetAsBox(8.0f, 2.0f);

		groundBody->CreateFixture(&shape, 0.0f);

		// 움직이는 박스 생성
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(4.0f, 3.0f);
		moveBody = world->CreateBody(&bodyDef);

		shape.SetAsBox(0.1f, 0.1f);

		b2FixtureDef fDef;
		fDef.shape = &shape;
		fDef.density = 1.0f;
		fDef.friction = 0.3f;
		moveBody->CreateFixture(&fDef);

		SetTimer(hWnd, 16, 1, TimerProc);

		GetClientRect(hWnd, &cRect);
	}
	break;
	case WM_SIZE:
		GetClientRect(hWnd, &cRect);
		break;
	case WM_LBUTTONDOWN:
	{
		float x = GET_X_LPARAM(lParam);
		float y = GET_Y_LPARAM(lParam);
		x /= PPU;
		y /= PPU;

		b2BodyDef bodyDef;
		b2PolygonShape shape;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(x, y);
		auto b = world->CreateBody(&bodyDef);
		shape.SetAsBox(0.1f, 0.1f);

		b2FixtureDef fDef;
		fDef.shape = &shape;
		fDef.density = 1.0f;
		fDef.friction = 0.3f;
		b->CreateFixture(&fDef);
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		HDC memDC = CreateCompatibleDC(hdc);
		HBITMAP memBit = CreateCompatibleBitmap(hdc, cRect.right, cRect.bottom);
		SelectObject(memDC, memBit);
		FillRect(memDC, &cRect, (HBRUSH)GetStockObject(WHITE_BRUSH));

		b2Body* bodyPointer = world->GetBodyList();
		for (; bodyPointer != nullptr; bodyPointer = bodyPointer->GetNext())
		{
			const b2Vec2& pos = bodyPointer->GetPosition();
			b2Fixture* fixturePointer = bodyPointer->GetFixtureList();
			for (; fixturePointer != nullptr; fixturePointer = fixturePointer->GetNext())
			{
				b2PolygonShape* pShape = static_cast<b2PolygonShape*>(fixturePointer->GetShape());
				switch (fixturePointer->GetType())
				{
				case b2Shape::e_polygon:
				{
					b2PolygonShape* pShape = static_cast<b2PolygonShape*>(fixturePointer->GetShape());
					int vtxCount = pShape->GetVertexCount();
					POINT* vtxList = new POINT[vtxCount];
					for (int i = 0; i < vtxCount; ++i)
					{
						vtxList[i].x = (pShape->GetVertex(i).x + pos.x)*PPU;
						vtxList[i].y = (pShape->GetVertex(i).y + pos.y)*PPU;
					}
					Polygon(memDC, vtxList, vtxCount);
					delete[] vtxList;
				}
				break;
				}
			}
		}

		BitBlt(hdc, 0, 0, cRect.right, cRect.bottom, memDC, 0, 0, SRCCOPY);
		DeleteObject(memBit);
		DeleteDC(memDC);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}