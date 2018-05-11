#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "gdiplus.lib")

#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;
TCHAR szClassName[] = TEXT("Window");

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton;
	static HWND hEdit;
	static HFONT hFont;
	static GraphicsPath * path;

	switch (msg)
	{
	case WM_CREATE:
		path = new GraphicsPath;
		hFont = CreateFontW(-MulDiv(10, 96, 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, SHIFTJIS_CHARSET, 0, 0, 0, 0, L"MS Shell Dlg");
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, 0);
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("ポリゴン"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, 0);
		break;
	case WM_SIZE:
		MoveWindow(hEdit, 10, 10, LOWORD(lParam) - 20 - 128, 32, TRUE);
		MoveWindow(hButton, LOWORD(lParam) - 10 - 128, 10, 128, 32, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			FontFamily fontFamily(L"Segoe UI");
			StringFormat strformat;
			DWORD dwSize = GetWindowTextLengthW(hEdit);
			LPWSTR lpszText = (LPWSTR)GlobalAlloc(0, (dwSize + 1) * sizeof(WCHAR));
			GetWindowText(hEdit, lpszText, dwSize + 1);
			path->Reset();
			path->AddString(lpszText, dwSize, &fontFamily, FontStyleRegular, 512.0f, Gdiplus::Point(0, 0), &strformat);
			Matrix translateMatrix;
			path->Flatten(&translateMatrix, 1.0f);
			GlobalFree(lpszText);
			InvalidateRect(hWnd, 0, 1);
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			{
				Graphics g(hdc);
				Pen pen(Color(255, 192, 192, 192), 4.0f);
				pen.SetLineJoin(LineJoinRound);
				g.DrawPath(&pen, path);
				{
					const int nPointCount = path->GetPointCount();
					PointF *points = new PointF[nPointCount];
					Status s = path->GetPathPoints(points, nPointCount);
					for (int i = 0; i < nPointCount; ++i)
					{
						g.FillEllipse(&SolidBrush(Color::Black), points[i].X - 2.0f, points[i].Y - 2.0f, 4.0f, 4.0f);
					}
					delete[]points;
				}
			}
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		delete path;
		DeleteObject(hFont);
		PostQuitMessage(0);
		break;
	default:
		return DefDlgProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	ULONG_PTR gdiToken;
	GdiplusStartupInput gdiSI;
	GdiplusStartup(&gdiToken, &gdiSI, NULL);
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		DLGWINDOWEXTRA,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("入力されたテキストの頂点列を取得"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!IsDialogMessage(hWnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	GdiplusShutdown(gdiToken);
	return (int)msg.wParam;
}
