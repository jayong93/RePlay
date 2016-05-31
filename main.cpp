// Win32Project1.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include <Windows.h>
#include <windowsx.h>
#include <memory>
#include <list>
#include "include\Box2D.h"

#define MAX_LOADSTRING 100
const float timeStep = 1 / 60.0f;
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
			if (currentTime - prevTime >= 500 * timeStep)
			{
				world->Step(timeStep, 8, 3);
				prevTime = currentTime;
			}
		}
	}

	delete world;

	return (int)msg.wParam;
}

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

class CreateQuery : public b2QueryCallback
{
public:
	CreateQuery() :b2QueryCallback{}, canCreate{ true } {}

	bool canCreate;

	bool ReportFixture(b2Fixture* fixture) override
	{
		canCreate = false;
		return false;
	}
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static RECT cRect;
	static const float PPU{ 40 };
	static bool isLBtnDown{ false };
	static float mouseX, mouseY;

	switch (message)
	{
	case WM_CREATE:
	{
		world = new b2World{ b2Vec2{ 0,9.8f } };
		world->SetAllowSleeping(true);
		world->SetWarmStarting(true);
		world->SetContinuousPhysics(true);

		// 바닥 생성
		b2BodyDef bodyDef;
		bodyDef.position.Set(40.0f, 10.0f);
		auto groundBody = world->CreateBody(&bodyDef);

		b2PolygonShape shape;
		shape.SetAsBox(40.0f, 2.0f);

		groundBody->CreateFixture(&shape, 0.0f);

		// 움직이는 박스 생성
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(4.0f, 3.0f);
		auto moveBody = world->CreateBody(&bodyDef);

		shape.SetAsBox(0.5f, 0.5f);

		b2FixtureDef fDef;
		fDef.shape = &shape;
		fDef.density = 5.0f;
		fDef.friction = 0.3f;
		moveBody->CreateFixture(&fDef);

		SetTimer(hWnd, 16, 1, nullptr);

		GetClientRect(hWnd, &cRect);
	}
	break;
	case WM_SIZE:
		GetClientRect(hWnd, &cRect);
		break;
	case WM_TIMER:
	{
		if (isLBtnDown)
		{
			b2AABB aabb;
			aabb.lowerBound.Set(mouseX - 0.5, mouseY - 0.5);
			aabb.upperBound.Set(mouseX + 0.5, mouseY + 0.5);

			CreateQuery cq;
			world->QueryAABB(&cq, aabb);
			if (cq.canCreate)
			{
				b2BodyDef bodyDef;
				b2PolygonShape shape;
				bodyDef.type = b2_dynamicBody;
				bodyDef.position.Set(mouseX, mouseY);
				auto b = world->CreateBody(&bodyDef);
				shape.SetAsBox(0.5f, 0.5f);

				b2FixtureDef fDef;
				fDef.shape = &shape;
				fDef.density = 5.0f;
				fDef.friction = 0.3f;
				b->CreateFixture(&fDef);
			}
		}

		InvalidateRect(hWnd, nullptr, false);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		float x = GET_X_LPARAM(lParam);
		float y = GET_Y_LPARAM(lParam);
		mouseX = x / PPU;
		mouseY = y / PPU;
		isLBtnDown = true;
	}
	break;
	case WM_MOUSEMOVE:
		mouseX = GET_X_LPARAM(lParam) / PPU;
		mouseY = GET_Y_LPARAM(lParam) / PPU;
		break;
	case WM_LBUTTONUP:
		isLBtnDown = false;
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
				int vtxCount = pShape->GetVertexCount();
				POINT vtxList[b2_maxPolygonVertices];
				for (int i = 0; i < vtxCount; ++i)
				{
					const b2Transform& tran = bodyPointer->GetTransform();
					b2Vec2 tmpVec = b2Mul(tran, pShape->m_vertices[i]);
					vtxList[i].x = (tmpVec.x)*PPU;
					vtxList[i].y = (tmpVec.y)*PPU;
				}
				Polygon(memDC, vtxList, vtxCount);
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