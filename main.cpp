// Win32Project1.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include <Windows.h>
#include <windowsx.h>
#include <memory>
#include <list>
#include "include\Box2D.h"
#include "Query.h"
#include "Object.h"

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static RECT cRect;
	static const float PPU{ 15 };
	static bool isLBtnDown{ false };
	static float mouseX, mouseY;
	static MovingBox selectedObject{ nullptr };

	switch (message)
	{
	case WM_CREATE:
	{
		GetClientRect(hWnd, &cRect);

		world = new b2World{ b2Vec2{ 0,0 } };
		world->SetAllowSleeping(true);
		world->SetWarmStarting(true);
		world->SetContinuousPhysics(true);


		b2PolygonShape shape;
		b2BodyDef bodyDef;
		b2FixtureDef fDef;
		fDef.shape = &shape;
		fDef.density = 5.0f;
		fDef.restitution = 1.0f;
		fDef.friction = 0.0f;


		shape.SetAsBox(0.5f, cRect.bottom / 2.0f / PPU);
		bodyDef.position.Set(cRect.right / PPU + 0.5f, cRect.bottom / 2.0f / PPU);
		auto b = world->CreateBody(&bodyDef);
		b->CreateFixture(&fDef);

		bodyDef.position.Set(-0.5f, cRect.bottom / 2.0f / PPU);
		b = world->CreateBody(&bodyDef);
		b->CreateFixture(&fDef);

		shape.SetAsBox(cRect.right / 2.0f / PPU, 0.5f);
		bodyDef.position.Set(cRect.right / 2.0f / PPU, cRect.bottom / PPU + 0.5f);
		b = world->CreateBody(&bodyDef);
		b->CreateFixture(&fDef);

		bodyDef.position.Set(cRect.right / 2.0f / PPU, -0.5f);
		b = world->CreateBody(&bodyDef);
		b->CreateFixture(&fDef);

		SetTimer(hWnd, 16, 1, nullptr);
	}
	break;
	case WM_SIZE:
		GetClientRect(hWnd, &cRect);
		break;
	case WM_TIMER:
	{
		if (isLBtnDown && !selectedObject.GetBody())
		{
			b2AABB aabb;
			aabb.lowerBound.Set(mouseX - 0.5, mouseY - 0.5);
			aabb.upperBound.Set(mouseX + 0.5, mouseY + 0.5);

			BoxQuery bq;
			world->QueryAABB(&bq, aabb);
			if (bq.canCreate)
			{
				b2BodyDef bodyDef;
				b2PolygonShape shape;
				bodyDef.type = b2_dynamicBody;
				bodyDef.position.Set(mouseX, mouseY);
				bodyDef.angularDamping = 1.0f;
				bodyDef.linearDamping = 1.0f;
				auto b = world->CreateBody(&bodyDef);
				shape.SetAsBox(0.5f, 0.5f);

				b2FixtureDef fDef;
				fDef.shape = &shape;
				fDef.density = 5.0f;
				fDef.restitution = 1.0f;
				fDef.friction = 0.0f;
				b->CreateFixture(&fDef);
			}
		}

		auto body = selectedObject.GetBody();
		if (body)
		{
			b2Vec2 bPos = body->GetPosition();
			auto& target = selectedObject.GetTarget();
			if ((target - bPos).Length() < 0.1)
				body->SetLinearVelocity(b2Vec2_zero);
			else
			{
				auto v = target - bPos;
				v *= 10;
				body->SetLinearVelocity(v);
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

		b2Vec2 p{ mouseX, mouseY };
		b2AABB aabb;
		aabb.lowerBound.Set(mouseX - 0.001, mouseY - 0.001);
		aabb.upperBound.Set(mouseX + 0.001, mouseY + 0.001);

		ClickQuery cq{ p };
		world->QueryAABB(&cq, aabb);

		if (cq.fixture)
		{
			selectedObject.SetBody(cq.fixture->GetBody());
			selectedObject.SetTarget(p);
		}

		SetCapture(hWnd);
	}
	break;
	case WM_MOUSEMOVE:
		mouseX = GET_X_LPARAM(lParam) / PPU;
		mouseY = GET_Y_LPARAM(lParam) / PPU;

		if (selectedObject.GetBody())
		{
			b2Vec2 mPos{ mouseX,mouseY };
			selectedObject.SetTarget(mPos);
		}
		break;
	case WM_LBUTTONUP:
		isLBtnDown = false;
		selectedObject.SetBody(nullptr);
		ReleaseCapture();
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
				if (bodyPointer->IsAwake())
					SelectObject(memDC, GetStockObject(GRAY_BRUSH));
				else
					SelectObject(memDC, GetStockObject(WHITE_BRUSH));
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