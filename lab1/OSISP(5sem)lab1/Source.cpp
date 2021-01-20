#include <Windows.h>
#include <tchar.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR WinName[] = _T("MainFrame");
HINSTANCE hInstance;

int APIENTRY _tWinMain(HINSTANCE This, 
	HINSTANCE Prev, 
	LPTSTR cmd,
	int mode)
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wc;
	
	hInstance = This;
	
	wc.hInstance = This;
	wc.lpszClassName = WinName; 
	wc.lpfnWndProc = WndProc; 
	wc.style = CS_HREDRAW | CS_VREDRAW; 
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); 
	wc.hCursor = LoadCursor(NULL, IDC_HAND);
	wc.lpszMenuName = NULL; 
	wc.cbClsExtra = 0; 
	wc.cbWndExtra = 0; 
	
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 3);
	if (!RegisterClass(&wc)) return 0; 
	// Создание окна
	hWnd = CreateWindow(WinName, 
		_T("Первая лабораторная"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		HWND_DESKTOP,
		NULL, 
		This, 
		NULL); 
	ShowWindow(hWnd, mode); 
	// Цикл обработки сообщений
	BOOL bRet;
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			return 0;
		}
		else
		{
		///	TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}

bool activemouse = false;
bool image = true;
bool play = false;
int winsizeX = 0, winsizeY = 0, speedX = 10, speedY = 10;
int size = 70;
HBITMAP hBitmap;
void DrawPicture(HWND hWnd, int x, int y);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{	
	PAINTSTRUCT ps;
	HDC hdc,hmdc; 
	BITMAP bm;
	static int wheelDelta = 0;
	static int x = 10, y = 10;
	int sx, sy;
	
	int prevX, prevY;
	RECT clientRect;
	RECT rect = { x,y,x + size,y + size };
	// Обработчик сообщений
	switch (message)
	{
	case WM_CREATE:		
		hBitmap = (HBITMAP)LoadImage(hInstance, L"..\\OSISP(5sem)lab1\\bitmap3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		winsizeX = LOWORD(lParam);
		winsizeY = HIWORD(lParam);
		break;
	case WM_SIZE:
		winsizeX = LOWORD(lParam);
		winsizeY = HIWORD(lParam);
		break;

	case WM_TIMER:
		if (x+size > winsizeX) speedX = -abs(speedX);
		if (x <= 0) speedX = abs(speedX);
		if (y+size > winsizeY) speedY = -abs(speedY);
		if (y <= 0) speedY = abs(speedY);
		x += speedX;
		y += speedY;
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_PAINT:
		if (!image) {
			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &clientRect);
			FillRect(hdc, &rect, CreateSolidBrush(RGB(125, 125, 125)));
			EndPaint(hWnd, &ps);
		}
		else DrawPicture(hWnd, x, y);

		break;
	case WM_LBUTTONDOWN:
		sx = LOWORD(lParam);
		sy = HIWORD(lParam);
		if (x <= sx && sx <= (x + size) && y <= sy && sy <= (y + size))
			activemouse = true;
		break;
	case WM_LBUTTONUP:
		activemouse = false;
		break;
	case WM_MOUSEMOVE:
		if (activemouse) {
			if(LOWORD(lParam)+size < winsizeX)x = LOWORD(lParam);
			if(HIWORD(lParam)+size < winsizeY)y = HIWORD(lParam);
			InvalidateRect(hWnd, NULL, TRUE);

		}
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT: if (x > 0) x -= 10;  break;
		case VK_RIGHT: if (x+size < winsizeX) x += 10; break;
		case VK_UP: if (y > 0) y -= 10; break;
		case VK_DOWN: if (y+size < winsizeY) y += 10; ; break;
		case 70: image = !image;  break;//KEY F
		case 82: play = !play; //KEY R
			if (play)
			SetTimer(hWnd, 1, 100, NULL); 
			else KillTimer(hWnd, 1);
		break;
		case VK_ADD: size += 10; break;
		case VK_SUBTRACT: size -= 10; break;			
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break; 
	case WM_MOUSEWHEEL:
		wheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
		for (; wheelDelta > WHEEL_DELTA; wheelDelta -= WHEEL_DELTA) {
			if (MK_SHIFT & wParam)
			{
				if (x > 0) x -= 10;
			}
			else { if (y > 0) y -= 10; }
			InvalidateRect(hWnd, NULL, TRUE);
		}
		for (; wheelDelta < 0; wheelDelta += WHEEL_DELTA) {
			if (MK_SHIFT & wParam)
			{
				if (x + size < winsizeX) x += 10;
			}
			else { if (y + size < winsizeY) y += 10; }
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_DESTROY:
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		break;
		// Завершение програм
		// Обработка сообщения по умолчанию
	default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
void DrawPicture(HWND hWnd, int x, int y) {
	BITMAP bm;
	HDC hDC;
	HDC hMemDC;
	PAINTSTRUCT ps;
	hDC = BeginPaint(hWnd, &ps);
	hMemDC = CreateCompatibleDC(hDC);
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	SelectObject(hMemDC, hBitmap);
	StretchBlt(hDC, x, y, size, size, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	DeleteDC(hMemDC);
	ReleaseDC(hWnd, hDC);
	EndPaint(hWnd, &ps);
}