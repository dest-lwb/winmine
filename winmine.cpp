/*------------------------------------------------------------------------
   HELLOWIN.C -- Displays "Hello, Windows 98!" in client area
      (c) Charles Petzold, 1998
-----------------------------------------------------------------------*/

#include <windows.h>
#include "resource.h"
#include "mine.h"

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("扫雷");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style   = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc  = WndProc;
	wndclass.cbClsExtra   = 0;
	wndclass.cbWndExtra   = 0;
	wndclass.hInstance   = hInstance;
	wndclass.hIcon   = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MINE));
	wndclass.hCursor   = LoadCursor (NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass (&wndclass))
	{
		MessageBox ( NULL, TEXT ("This program requires Windows NT!"), 
					szAppName, MB_ICONERROR) ;
		return 0;
	}
	hwnd = CreateWindow( szAppName,		// window class name
	TEXT ("扫雷"),			            // window caption
	WS_OVERLAPPEDWINDOW& ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,				// window style
	CW_USEDEFAULT,						// initial x position
	CW_USEDEFAULT,						// initial y position
	CW_USEDEFAULT,//478,				// initial x size
	CW_USEDEFAULT,//576,				// initial y size
	NULL,								// parent window handle
	NULL,								// window menu handle
	hInstance,							// program instance handle
	NULL);								// creation parameters

	ShowWindow (hwnd, iCmdShow);
	UpdateWindow (hwnd);
     
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}
	return msg.wParam;
}

//WndProc 函数
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC				  hdc, hdcMem;
	PAINTSTRUCT       ps;
	int               cxWindow,cyWindow;
	int rs;
	static bool isWin;
	switch (message)
	{
		case WM_CREATE:
			//PlaySound (TEXT ("ring.wav"), NULL, SND_FILENAME | SND_ASYNC);
			hInstance = ((LPCREATESTRUCT)lParam)->hInstance;   
			hBmpMine = LoadBitmap(hInstance,  MAKEINTRESOURCE(IDB_MINE)); 
			hBmpBorder = LoadBitmap(hInstance,  MAKEINTRESOURCE(IDB_BORDER));
			hBmpNum = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_NUM));

			GetObject(hBmpMine, sizeof(BITMAP), &bMine);
			cxBmpMine = bMine.bmWidth;  
			cyBmpMine = bMine.bmHeight/17;

			GetObject(hBmpBorder, sizeof(BITMAP), &bBorder);
			cxBmpBorder = bBorder.bmWidth;  
			cyBmpBorder = bBorder.bmHeight;

			GetObject(hBmpNum, sizeof(BITMAP), &bNum);
			cxBmpNum = bNum.bmWidth;
			cyBmpNum = bNum.bmHeight/12;

			mineFlag = mineNum;
			border = cxBmpBorder;
			cxWindow = mdiv * cxBmpMine + border * 2 + 6;
			cyWindow = mdiv * cyBmpMine + border * 2 + cyBmpNum + 58;
			InitMineMap(); // 初始化雷区
			MoveWindow(hwnd, 200, 200, cxWindow, cyWindow, TRUE);

			return 0;

		case WM_COMMAND:
			switch (LOWORD(wParam))  
			{
				case ID_NEWGAME: //开局
					InitMineMap();
					InvalidateRect( hwnd, NULL, TRUE);
					//PlaySound (TEXT ("ring.wav"), NULL, SND_FILENAME | SND_ASYNC);
					break;
				case ID_QUIT: //退出
					PostQuitMessage (0);
					break;
				case ID_ABOUT: //帮助
					MessageBox(hwnd, TEXT("     MADE IN CHINA     "), TEXT("帮助"), MB_OK);
					break;
			}
			break;

		case WM_SIZE:  
			cxClient = LOWORD(lParam);  
			cyClient = HIWORD(lParam);

			countRc.top = cyClient - border - cyBmpNum;
			countRc.right = border + 3*cxBmpNum;
			countRc.bottom = cyClient - border;
			countRc.left = border;

			timeRc.top = cyClient - border - cyBmpNum;
			timeRc.right = cxClient - border;
			timeRc.bottom = cyClient - border;
			timeRc.left = timeRc.right - 3*cxBmpNum;

			return 0;

		case WM_PAINT:
			hdc = BeginPaint (hwnd, &ps);
			hdcMem = CreateCompatibleDC(hdc);
			
			SelectObject(hdcMem, hBmpMine);
			// 画雷区
			DrawMineMap(hwnd, hdc,hdcMem);
			//画边框
			SelectObject(hdcMem, hBmpBorder);
			StretchBlt (hdc, 0, 0, border, cyClient-56, hdcMem, 0, 0, cxBmpBorder, cyBmpBorder, MERGECOPY) ;
			StretchBlt (hdc, 0, 0, cxClient, border, hdcMem, 0, 0, cxBmpBorder, cyBmpBorder, MERGECOPY) ;
			StretchBlt (hdc, 0, cyClient-border-56, cxClient, cyClient, hdcMem, 0, 0, cxBmpBorder, cyBmpBorder, MERGECOPY) ;
			StretchBlt (hdc, cxClient-border, 0, cxClient, cyClient, hdcMem, 0, 0, cxBmpBorder, cyBmpBorder, MERGECOPY) ;
			
			//画计时器和计数器
			SelectObject(hdcMem, hBmpNum); 
			DrawTime(hdc, hdcMem);
			DrawCount(hdc, hdcMem);
			DeleteDC(hdcMem);
			
			EndPaint (hwnd, &ps);
		    return 0;

		case WM_LBUTTONDOWN:
			hdc = GetDC(hwnd);
			if(wParam & MK_RBUTTON){				
				DoubleDown(hwnd,hdc,lParam);
				InvalidateRect( hwnd, NULL, TRUE);
			}else{
				LbtDown(hwnd,hdc,lParam);
			}
			SetTimer(hwnd, ID_TIMER, 1000, NULL); //设置计时器
			break;

		case WM_LBUTTONUP:
			hdc = GetDC(hwnd);
			LbtUp(hwnd,hdc,lParam);
			isWin = MineWin();
 			if(isWin){
				KillTimer(hwnd,ID_TIMER);
				InitMineMap();
				rs = MessageBox(hwnd,TEXT("     你赢了！"),TEXT("hello"),MB_OK);
				if(rs == IDOK){					
					InvalidateRect( hwnd, NULL, TRUE);
				}
				/*
				InvalidateRect( hwnd, NULL, TRUE);*/
			}
			break;

		case WM_RBUTTONDOWN:
			hdc = GetDC(hwnd);
			if(wParam & MK_LBUTTON){
				DoubleDown(hwnd,hdc,lParam);
				//ReDraw(hwnd,hdc);
				InvalidateRect( hwnd, NULL, TRUE);
			}else{
				RbtDown(hwnd,hdc,lParam);
			}
			break;

		case WM_TIMER:
			if(mineTime <= 999){
				mineTime ++;
				InvalidateRect(hwnd, &timeRc, FALSE);
			}
			return 0;

		case WM_DESTROY:
			PostQuitMessage (0);

			return 0;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

