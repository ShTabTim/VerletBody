#include "ShWin.h"
#include <chrono>
#include <thread>

bool ShWinRendCPU::init(HINSTANCE hInst, int W, int H, WNDPROC WndProc) {
    WNDCLASSEX wcex;

    memset(&wcex, 0, sizeof(WNDCLASSEX));

    db.init(W, H);

    initSim();

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = AppName.c_str() + 1;
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (RegisterClassEx(&wcex) == 0)
        return MessageBox(NULL, L"RegisterClassEx failed!", L"hhhh", MB_ERR_INVALID_CHARS), false;

    if ((hWnd = CreateWindow(
		wcex.lpszClassName,
		AppName.c_str(), 
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		W, 
		H, 
		NULL, 
		NULL, 
		hInst, 
		NULL)) == NULL)
        return MessageBox(NULL, L"CreateWindowEx failed!", L"hhhh", MB_ERR_INVALID_CHARS), false;
    return 1;
}

void ShWinRendCPU::Start() {
	Show(false);
	Rend();
}

int ShWinRendCPU::ScreenW() { return db.GetW(); }
int ShWinRendCPU::ScreenH() { return db.GetH(); }


ShWinRendCPU::sKeyState ShWinRendCPU::GetKey(int nKeyID) { return m_keys[nKeyID]; }
ShWinRendCPU::sKeyState ShWinRendCPU::GetMouse(int nMouseButtonID) { return m_mouse[nMouseButtonID]; }
int ShWinRendCPU::GetMouseX() { return m_mousePosX; }
int ShWinRendCPU::GetMouseY() { return m_mousePosY; }
bool ShWinRendCPU::IsFocused() { return m_bInFocus; }

void ShWinRendCPU::Rend() {

	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();

	MSG msg;

	RECT rect;

	GetClientRect(hWnd, &rect);

	HDC dc = GetDC(hWnd);

	for (int i = 0; i < 256; i++) {
		m_keys[i].bPressed = false;
		m_keys[i].bHeld = false;
		m_keys[i].bReleased = false;
	}
	while (1) {
		if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {

			tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> dTime = tp2 - tp1;
			tp1 = tp2;
			float dt = dTime.count();

			for (int i = 0; i < 256; i++) {
				m_keyNewState[i] = GetAsyncKeyState(i);

				m_keys[i].bPressed = false;
				m_keys[i].bReleased = false;

				if (m_keyNewState[i] != m_keyOldState[i])
					if (m_keyNewState[i] & 0x8000) {
						m_keys[i].bPressed = !m_keys[i].bHeld;
						m_keys[i].bHeld = true;
					} else {
						m_keys[i].bReleased = true;
						m_keys[i].bHeld = false;
					}

				m_keyOldState[i] = m_keyNewState[i];
			}
			m_mouse[0] = m_keys[VK_LBUTTON];
			m_mouse[1] = m_keys[VK_RBUTTON];
			m_mouse[2] = m_keys[VK_MBUTTON];
			m_mouse[3] = m_keys[VK_XBUTTON1];
			m_mouse[4] = m_keys[VK_XBUTTON2];
			{
				RECT rcClient, rcWind;
				GetClientRect(hWnd, &rcClient);
				GetWindowRect(hWnd, &rcWind);
				int border_thickness_x = ((rcWind.right - rcWind.left) - rcClient.right) / 2;
				POINT pt;
				GetCursorPos(&pt);
				m_mousePosX = pt.x - rcWind.left - border_thickness_x;
				m_mousePosY = rcWind.bottom - pt.y - border_thickness_x;
			}


			if (!loopSim(dt))
				break;

			wchar_t s[64];
			swprintf_s(s, 64, L"ConsoleRenderer - %s - FPS: %5.2f", AppName.c_str(), 1.0f / dt);

			SetWindowText(hWnd, s);

			db.OnDrawContextDrawBuffer(&dc);
		}
	}

	if (finitSim())
		db.~DB();
}

void ShWinRendCPU::Show(bool Maximized) {
	RECT wRect, cRect;

	GetWindowRect(hWnd, &wRect);
	GetClientRect(hWnd, &cRect);

	wRect.right  += db.GetW() - cRect.right  - wRect.left;
	wRect.bottom += db.GetW() - cRect.bottom - wRect.top;

	MoveWindow(hWnd, wRect.left, wRect.top, wRect.right, wRect.bottom, FALSE);

	ShowWindow(hWnd, Maximized ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL);
}