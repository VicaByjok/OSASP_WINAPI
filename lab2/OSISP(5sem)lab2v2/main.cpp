#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <gdiplus.h>
#include <iostream>
#include <fstream>
#include "vector"
#include <string>

using namespace std;

vector<string> text;
int columnAmount = 6;
const int penWidth = 4;

HFONT hfnt;
HPEN hpen1;


void drawVertical(HDC hdc, int windowWidth, int windowHeight) {

	int columnWidth = windowWidth / columnAmount;

	for (int i = 0; i < columnAmount; i++) {
		MoveToEx(hdc, i * columnWidth, 0, NULL);
		LineTo(hdc, i * columnWidth, windowHeight);
	}
	MoveToEx(hdc, windowWidth - 1, 0, NULL);
	LineTo(hdc, windowWidth - 1, windowHeight);
}

void drawHorizontal(HDC hdc, int windowWidth, int lineHeight) {

	MoveToEx(hdc, 0, lineHeight, NULL);
	LineTo(hdc, windowWidth, lineHeight);

}

void drawTable(HDC hdc, int windowWidth, int windowHeight) {

	int columnWidth = windowWidth / columnAmount;
	int textHeightTop = 0;

	for (int i = 0; i < text.size();) {

		RECT rect;
		rect.top = 5 * 0.5 + textHeightTop;
		int nextColumnHeight = 0;

		for (int j = 0; j < columnAmount && i < text.size(); j++) {

			const CHAR* str = text[i++].c_str();

			rect.left = j * columnWidth + 10 * 0.5;
			rect.right = (j + 1) * columnWidth - 10 * 0.5;

			int textHeight = DrawText(hdc, (LPCSTR)str, strlen(str), &rect, DT_VCENTER | DT_EDITCONTROL | DT_WORDBREAK | DT_NOCLIP);

			if (textHeight > nextColumnHeight)
				nextColumnHeight = textHeight;

		}

		textHeightTop += nextColumnHeight + 5;

		drawHorizontal(hdc, windowWidth, textHeightTop);
		drawVertical(hdc, windowWidth, textHeightTop);
	}
	drawHorizontal(hdc, windowWidth, 1);
}

HBRUSH hbrBkGnd = CreateSolidBrush(RGB(255, 255, 255));
RECT rc;

void initCompDC(HWND hWnd, LPPAINTSTRUCT oldps, int windowWidth, int windowHeight)
{
	HDC compatibleDC;
	HBITMAP compatibleHBitmap;

	compatibleDC = CreateCompatibleDC(oldps->hdc);
	compatibleHBitmap = CreateCompatibleBitmap(oldps->hdc, rc.right - rc.left, rc.bottom - rc.top);
	SelectObject(compatibleDC, compatibleHBitmap); 
	FillRect(compatibleDC, &rc, hbrBkGnd);
	
	SelectObject(compatibleDC, hpen1);
	SetTextColor(compatibleDC, RGB(200, 0, 0));
	SelectObject(compatibleDC, hfnt);

	drawTable(compatibleDC, windowWidth, windowHeight);
	BitBlt(oldps->hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, compatibleDC, 0, 0, SRCCOPY);

	DeleteObject(compatibleHBitmap);
	DeleteDC(compatibleDC);

}

void readFile() {

	ifstream fs(R"(text.txt)", ios::in | ios::binary);
	std::string line;
	if (fs.is_open())
	{
		while (getline(fs, line))
		{
			text.push_back(line);
		}
	}
	fs.close();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { // Обработчик сообщений

	PAINTSTRUCT ps;
	static int width, height;

	switch (message) {

	case WM_CREATE: {
		hpen1 = CreatePen(PS_SOLID, penWidth, RGB(0, 0, 0));
		hfnt = CreateFont(20, 10, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, "Bahnschrift");
		break;
	}

	case WM_SIZE: {
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		GetClientRect(hWnd, &rc);
		break;
	}

	int key;
	case WM_KEYDOWN:
		key = wParam;
		switch (key) {
		case VK_ADD:
			columnAmount++;
			InvalidateRect(hWnd, NULL, NULL);
			break;
		case VK_SUBTRACT:
			if (columnAmount > 1) {
				columnAmount--;
				InvalidateRect(hWnd, NULL, NULL);
			}
			break;
		}
		break;
	case WM_PAINT: {
		BeginPaint(hWnd, &ps);
		initCompDC(hWnd, &ps, width, height);
		EndPaint(hWnd, &ps);
		break;
	}

	case WM_DESTROY: {
		DeleteObject(hfnt);
		DeleteObject(hpen1);
		DeleteObject(hbrBkGnd);
		PostQuitMessage(0);
		break;
	}

	default: {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	}
	return 0;
}

TCHAR WinName[] = _T("MainFrame");

int WINAPI _tWinMain(HINSTANCE This, HINSTANCE Prev, LPTSTR cmd, int mode) {

	readFile();

	HWND hWnd; // Дескриптор главного окна программы
	MSG msg; // Структура для хранения сообщения
	WNDCLASS wc; // Класс окна

	wc.hInstance = This; //Дескриптор текущего приложения
	wc.lpszClassName = WinName; // Имя класса окна
	wc.lpfnWndProc = WndProc; // Имя функции для обработки сообщений.
	wc.style = CS_HREDRAW | CS_VREDRAW; // Стиль окна

	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Стандартная иконка
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // Стандартный курсор
	wc.lpszMenuName = NULL; // Нет меню
	wc.cbClsExtra = 0; // Нет дополнительных данных класса
	wc.cbWndExtra = 0; // Нет дополнительных данных окна

	// Заполнение окна белым цветом
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	if (!RegisterClass(&wc))
		return 0;

	// Создание окна
	hWnd = CreateWindow(WinName, // Имя класса окна
		_T("Lab2"), // Заголовок окна
		WS_OVERLAPPEDWINDOW, // Стиль окна
		CW_USEDEFAULT, // x левого верхнего угла
		CW_USEDEFAULT, // y девого верхнего угла
		CW_USEDEFAULT, // width
		CW_USEDEFAULT, // Height
		HWND_DESKTOP, // Дескриптор родительского окна
		NULL, // Нет меню
		This, // Дескриптор приложения
		NULL); // Дополнительной информации нет

	ShowWindow(hWnd, mode);
	UpdateWindow(hWnd);


	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg); 
		DispatchMessage(&msg);
	}
	return 0;
}