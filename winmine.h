#include <time.h> 

#define ID_TIMER 1

//雷区砖块结构体定义
typedef struct tagMINE{
	bool isOpen;
	int RDownStat;
	int status;
} MINE;

static HBITMAP    hBmpMine, hBmpBorder, hBmpNum;
static int        cxClient, cyClient, cxBmpMine, cyBmpMine, cxBmpBorder, 
				  cyBmpBorder, cxBmpNum, cyBmpNum, border;
BITMAP            bMine, bBorder, bNum;
HINSTANCE         hInstance; 
// 
//int				nx, ny, mx, my;  // 鼠标位置 和 对应的二维数组下标
//int               ms;              //砖块状态对应位图位置的偏移量
static MINE       mine[10][10];    //雷区大小数组
static int        mineNum = 10;    //雷数
int               mdiv = 9;        //雷区边长
int               mineFlag, mineTime = 0;  //计数器数   计时器秒数
RECT              timeRc, countRc; //计数器和计时器的无效区

//windows消息处理函数
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

void InitMineMap();           //初始化雷区
void DrawMineMap(HWND hwnd, HDC hdc, HDC hMem);  //画雷区
void ReDraw(HDC hdc, HDC hMem);       //刷新
void DrawTime(HDC hdc, HDC hMem);     //画计时器
void DrawCount(HDC hdc, HDC hMem);    //计数器 剩余雷数
void MineFail(HWND hwnd, HDC hdc);     //扫雷失败
bool MineWin();             //扫雷成功
void LbtDown(HWND hwnd, HDC hdc, LONG lParam);  //鼠标左键按下
void LbtUp(HWND hwnd, HDC hdc, LONG lParam);  //鼠标左键放开
void RbtDown(HWND hwnd, HDC hdc, LONG lParam);  //鼠标右键按下
void DoubleDown(HWND hwnd, HDC hdc, LONG lParam); //鼠标左右键都按下

// 初始化雷区
void InitMineMap()
{
	int i , j;
	int rx, ry;
	mineFlag = mineNum;
	mineTime = 0;
	srand((unsigned int)(time(0)));

	for ( i = 0; i < mdiv; i++)  
		for (j = 0; j < mdiv; j++)  
		{
			mine[i][j].isOpen = false;   //TRUE FALSE
			mine[i][j].RDownStat = 0;
			mine[i][j].status = 0;
		}

	i = 0;
	while ( i < mineNum ) 
	{
		rx = rand() % mdiv; // col mineNum
		ry = rand() % mdiv; // row mdiv 雷区边长

		//已经有雷的位置不能再次撒雷
		if( mine[rx][ry].status != 10 )
		{ 
			mine[rx][ry].status = 10;
			i++;
		}else{ 
			continue; 
		}
		//处理九宫格右下角三个位置的砖块
		if(rx != 0 && ry != 0){
			if(mine[rx - 1][ry - 1].status != 10)
				 ++mine[rx - 1][ry - 1].status;
			if(mine[rx - 1][ry].status != 10)
				 ++mine[rx - 1][ry].status;
			if(mine[rx][ry - 1].status != 10)
				 ++mine[rx][ry - 1].status;
			if(rx == mdiv-1 && mine[rx][ry + 1].status != 10)
				++mine[rx][ry + 1].status;
			if(ry == mdiv-1 && mine[rx + 1][ry].status != 10)
				++mine[rx + 1][ry].status;

		}
		//处理九宫格左上角三个位置的砖块
		if(rx != mdiv-1 && ry != mdiv-1){
			if(mine[rx + 1][ry + 1].status != 10)
				 ++mine[rx + 1][ry + 1].status;
			if(mine[rx + 1][ry].status != 10)
				 ++mine[rx + 1][ry].status;
			if(mine[rx][ry + 1].status != 10)
				 ++mine[rx][ry + 1].status;
			if( rx == 0 && mine[rx][ry - 1].status != 10)
				++mine[rx][ry - 1].status;
			if( ry == 0 && mine[rx - 1][ry].status != 10)
				++mine[rx - 1][ry].status;
		}
		//处理九宫格右上角位置的砖块
		if(rx != 0 && ry != mdiv-1){ //
			if(mine[rx - 1][ry + 1].status != 10)
				 ++mine[rx - 1][ry + 1].status;
			// 雷区左下角的砖块为雷的话，处理另外两块砖
			if(rx == mdiv-1 && ry == 0 && mine[rx -1][ry].status != 10)
				++mine[rx - 1][ry].status;
			if(rx == mdiv-1 && ry == 0 && mine[rx][ry + 1].status != 10)
				++mine[rx][ry + 1].status;
		}
		//九宫格左下角位置的砖块			
		if(rx != mdiv-1 && ry != 0){
			if(mine[rx + 1][ry - 1].status != 10)
				 ++mine[rx + 1][ry - 1].status;
			// 雷区右上角的砖块为雷的话，处理另外两块砖
			if(rx == 0 && ry == mdiv-1 && mine[rx][ry - 1].status != 10)
				++mine[rx][ry - 1].status;
			if(rx == 0 && ry == mdiv-1 && mine[rx + 1][ry].status != 10)
				++mine[rx + 1][ry].status;
			
		}
	}//while
	
}

// 画雷区
void DrawMineMap(HWND hwnd, HDC hdc, HDC hdcMem)
{
	int i, j;
	int x, y;
	int ms;
	for (i = 0; i < mdiv; i++)  
		for (j = 0; j < mdiv; j++)  
		{	
			x = border + i * cxBmpMine;
			y = border + j * cyBmpMine;
			if( mine[i][j].isOpen ){
				if(mine[i][j].status == 0){
					if(i != 0 && j != 0){
						mine[i-1][j-1].isOpen = true;
						mine[i-1][j].isOpen = true;
						mine[i][j-1].isOpen = true;
						if(i == mdiv-1)
							mine[i][j + 1].isOpen = true;
						if(j == mdiv-1)
							mine[i + 1][j].isOpen = true;
					}
					if(i != (mdiv-1) && j != (mdiv-1)){
						mine[i+1][j+1].isOpen = true;
						mine[i+1][j].isOpen = true;
						mine[i][j+1].isOpen = true;
						if(i == 0)
							mine[i][j - 1].isOpen = true;
						if(j == 0)
							mine[i - 1][j].isOpen = true;
					}
					if(i != 0 && j != (mdiv-1)){
						mine[i-1][j+1].isOpen = true;
						
						if(i == (mdiv-1) && j == 0){
							mine[i - 1][j].isOpen = true;
							mine[i][j + 1].isOpen = true;
						}
					}
					
					if(i != (mdiv-1) && j != 0){
						mine[i+1][j-1].isOpen = true;
						if(i == 0 && j == (mdiv-1) ){
							mine[i + 1][j].isOpen = true;
							mine[i][j - 1].isOpen = true;
						}
					}

				}
			}
		}
	//
	for (i = mdiv-1; i >= 0; i--)  
		for (j = mdiv-1; j >= 0; j--)  
		{	
			x = border + i * cxBmpMine;
			y = border + j * cyBmpMine;
			if( mine[i][j].isOpen ){
				if(mine[i][j].status == 0){
					if(i != 0 && j != 0){
						mine[i-1][j-1].isOpen = true;
						mine[i-1][j].isOpen = true;
						mine[i][j-1].isOpen = true;
						if(i == mdiv-1)
							mine[i][j + 1].isOpen = true;
						if(j == mdiv-1)
							mine[i + 1][j].isOpen = true;
					}
					if(i != (mdiv-1) && j != (mdiv-1)){
						mine[i+1][j+1].isOpen = true;
						mine[i+1][j].isOpen = true;
						mine[i][j+1].isOpen = true;
						if(i == 0)
							mine[i][j - 1].isOpen = true;
						if(j == 0)
							mine[i - 1][j].isOpen = true;
					}
					if(i != 0 && j != (mdiv-1)){
						mine[i-1][j+1].isOpen = true;
						
						if(i == (mdiv-1) && j == 0){
							mine[i - 1][j].isOpen = true;
							mine[i][j + 1].isOpen = true;
						}
					}
					
					if(i != (mdiv-1) && j != 0){
						mine[i+1][j-1].isOpen = true;
						if(i == 0 && j == (mdiv-1)){
							mine[i + 1][j].isOpen = true;
							mine[i][j - 1].isOpen = true;
						}
					}

				}
			}
		}
	//
	for (i = 0; i < mdiv; i++)  
		for (j = 0; j < mdiv; j++)  
		{	
			x = border + i * cxBmpMine;
			y = border + j * cyBmpMine;
			
			if( !mine[i][j].isOpen ){
				BitBlt(hdc, x, y, cxBmpMine, cyBmpMine, hdcMem, 0, 0, SRCCOPY);
			}else {				
				ms = 16 - mine[i][j].status;
				BitBlt(hdc, x, y, cxBmpMine, cyBmpMine, hdcMem, 0, ms*cyBmpMine, SRCCOPY);
			}

			if(mine[i][j].RDownStat == 1){
				ms = 2;
				BitBlt(hdc, x, y, cxBmpMine, cyBmpMine, hdcMem, 0, ms*cyBmpMine, SRCCOPY);
			}else if(mine[i][j].RDownStat == 2){
				ms = 3;
				BitBlt(hdc, x, y, cxBmpMine, cyBmpMine, hdcMem, 0, ms*cyBmpMine, SRCCOPY);
			}
		}
}

//刷新雷区
void ReDraw(HWND hwnd, HDC hdc)
{
	int i, j;
	int x, y;
	int ms;
	HDC hdcMem;

	for (i = 0; i < mdiv; i++)  
		for (j = 0; j < mdiv; j++)  
		{	
			x = border + i * cxBmpMine;
			y = border + j * cyBmpMine;
			hdcMem = CreateCompatibleDC(hdc);
			if( !mine[i][j].isOpen ){
				BitBlt(hdc, x, y, cxBmpMine, cyBmpMine, hdcMem, 0, 0, SRCCOPY);
			}else {				
				ms = 16 - mine[i][j].status;
				BitBlt(hdc, x, y, cxBmpMine, cyBmpMine, hdcMem, 0, ms*cyBmpMine, SRCCOPY);
			}

			if(mine[i][j].RDownStat == 1){
				ms = 2;
				BitBlt(hdc, x, y, cxBmpMine, cyBmpMine, hdcMem, 0, ms*cyBmpMine, SRCCOPY);
			}else if(mine[i][j].RDownStat == 2){
				ms = 3;
				BitBlt(hdc, x, y, cxBmpMine, cyBmpMine, hdcMem, 0, ms*cyBmpMine, SRCCOPY);
			}
			DeleteDC(hdcMem);
		}
}
//画计数器
void DrawCount(HDC hdc, HDC hdcMem)
{
	int dx = border;
	int dy = cyClient - border - cyBmpNum;
	int ms;
	if(mineFlag >= 0){		
		ms = 1;
		BitBlt(hdc, dx, dy, cxBmpNum, cyBmpNum, hdcMem, 0, ms*cyBmpNum, SRCCOPY);
		dx += cxBmpNum;
		ms = 11 - mineFlag/10;
		BitBlt(hdc, dx, dy, cxBmpNum, cyBmpNum, hdcMem, 0, ms*cyBmpNum, SRCCOPY);
		dx += cxBmpNum;
		ms = 11 - mineFlag%10;
		BitBlt(hdc, dx, dy, cxBmpNum, cyBmpNum, hdcMem, 0, ms*cyBmpNum, SRCCOPY);
	}else{
		ms = 0;
		BitBlt(hdc, dx, dy, cxBmpNum, cyBmpNum, hdcMem, 0, ms*cyBmpNum, SRCCOPY);
		dx += cxBmpNum;
		ms = 11 - -mineFlag/10;
		BitBlt(hdc, dx, dy, cxBmpNum, cyBmpNum, hdcMem, 0, ms*cyBmpNum, SRCCOPY);
		dx += cxBmpNum;
		ms = 11 - -mineFlag%10;
		BitBlt(hdc, dx, dy, cxBmpNum, cyBmpNum, hdcMem, 0, ms*cyBmpNum, SRCCOPY);
	}
	
}

// 画计时器
void DrawTime(HDC hdc, HDC hdcMem)
{
	int dx = cxClient - border - cxBmpNum;
	int dy = cyClient - border - cyBmpNum;
	int ms;

	ms =11 - mineTime%10;
	BitBlt(hdc, dx, dy, cxBmpNum, cyBmpNum, hdcMem, 0, ms*cyBmpNum, SRCCOPY);
	dx -= cxBmpNum;
	ms = mineTime/10; 
	ms = 11 - ms%10;
	BitBlt(hdc, dx, dy, cxBmpNum, cyBmpNum, hdcMem, 0, ms*cyBmpNum, SRCCOPY);
	dx -= cxBmpNum;
	ms = 11 - mineTime/100;
	BitBlt(hdc, dx, dy, cxBmpNum, cyBmpNum, hdcMem, 0, ms*cyBmpNum, SRCCOPY);
	
}

//扫雷失败
void MineFail(HWND hwnd, HDC hdc)
{
	int i, j;
	int x, y;
	int ms;
	int rs;
	HDC hdcMem;
	hdcMem = CreateCompatibleDC(hdc);
	SelectObject(hdcMem, hBmpMine);

	for (i = 0; i <= mdiv; i++)
		for (j = 0; j <= mdiv; j++)
		{
			mine[i][j].isOpen = true;
			x = border + i * cxBmpMine;
			y = border + j * cyBmpMine;
			if(mine[i][j].status == 10)
			{
				ms = 6;
				BitBlt(hdc, x, y, cxBmpMine, cyBmpMine, hdcMem, 0, ms*cyBmpMine, SRCCOPY);
			}
			else if(mine[i][j].RDownStat == 1 )
			{
				ms = 5;
				BitBlt(hdc, x, y, cxBmpMine, cyBmpMine, hdcMem, 0, ms*cyBmpMine, SRCCOPY);
			}
		}
	KillTimer(hwnd,ID_TIMER);
	/*ms = 4;
	BitBlt(hdc, nx, ny, cxBmpMine, cyBmpMine, hdcMem, 0, ms*cyBmpMine, SRCCOPY);*/
	rs= MessageBox(hwnd,TEXT("        你输了！重新开始吗？"),TEXT("hello"),MB_OKCANCEL);
	if(rs == IDOK){
		InitMineMap();
		InvalidateRect( hwnd, NULL, TRUE);
	}
	DeleteDC(hdcMem);
}

// 扫雷成功
bool MineWin()
{
	int i, j;
	int flag = 0;
	
	for (i = 0; i < mdiv; i++)
		for (j = 0; j < mdiv; j++)
		{
			if(mine[i][j].isOpen && mine[i][j].status != 10){
				flag++;
			}else if(mine[i][j].isOpen && mine[i][j].status == 10){
				return false;
			}else{
				continue;
			}
		}
	if(flag == (mdiv*mdiv - mineNum) ){
		return true;
	}else{
		return false;
	}

}

//鼠标左键释放
void LbtUp(HWND hwnd,HDC hdc, LONG lParam)
{
	int ms;             //砖块状态对应位图位置的偏移量
	int	nx, ny, mx, my; // 鼠标位置 和 对应的二维数组下标
//	int rs;

	HDC hdcMem;
	nx = LOWORD(lParam)-border;
	ny = HIWORD(lParam)-border;
	
	//若鼠标焦点在雷区，确定应该翻开的砖块
	if(nx >= 0 && nx < mdiv*cxBmpMine && ny >= 0 && ny < mdiv*cyBmpMine)
	{
		mx = (nx/cxBmpMine);
		my = (ny/cyBmpMine);
		nx = mx*cxBmpMine + border;
		ny = my*cyBmpMine + border;		

		if(!mine[mx][my].isOpen && mine[mx][my].RDownStat == 0)  //砖块位置mx,my
		{
			mine[mx][my].isOpen = !mine[mx][my].isOpen;
										
			hdcMem = CreateCompatibleDC(hdc);  
			SelectObject(hdcMem, hBmpMine); 
			if(mine[mx][my].status != 10){
				ms = 16 - mine[mx][my].status ;
				BitBlt(hdc, nx, ny, cxBmpMine, cyBmpMine, hdcMem, 0, ms*cyBmpMine, SRCCOPY);
				if(mine[mx][my].status == 0)
				{
					DrawMineMap(hwnd, hdc, hdcMem);
					InvalidateRect(hwnd, NULL, FALSE);
				}
			}else{
				ms = 4;
				BitBlt(hdc, nx, ny, cxBmpMine, cyBmpMine, hdcMem, 0, ms*cyBmpMine, SRCCOPY);
				//Sleep(100);
				MineFail(hwnd,hdc); //扫雷失败				
				/*KillTimer(hwnd,ID_TIMER);
				ms = 4;
				BitBlt(hdc, nx, ny, cxBmpMine, cyBmpMine, hdcMem, 0, ms*cyBmpMine, SRCCOPY);
				
				rs= MessageBox(hwnd,TEXT("        你输了！重新开始吗？"),TEXT("hello"),MB_OKCANCEL);
				if(rs == IDOK){
					InitMineMap();
					InvalidateRect( hwnd, NULL, TRUE);
				}*/

			}
			DeleteDC(hdcMem);
		}else{		 
			return ;
		}
	}else{
		return ;
	}
}

//鼠标左键按下
void LbtDown(HWND hwnd,HDC hdc, LONG lParam)
{
	int ms;             //砖块状态对应位图位置的偏移量
	int	nx, ny, mx, my; // 鼠标位置 和 对应的二维数组下标
	HDC hdcMem;
	nx = LOWORD(lParam)-border;
	ny = HIWORD(lParam)-border;
	if(nx>=0 && nx<mdiv*cxBmpMine && ny>=0 && ny<mdiv*cyBmpMine)
	{
		mx = (nx/cxBmpMine);
		my = (ny/cyBmpMine);
		nx = mx*cxBmpMine + border;
		ny = my*cyBmpMine + border;

		if(!mine[mx][my].isOpen && mine[mx][my].RDownStat == 0)      //砖块位置nx,ny
		{
			hdcMem = CreateCompatibleDC(hdc);  
			SelectObject(hdcMem, hBmpMine); 
			ms = 16 ;
			BitBlt(hdc, nx, ny, cxBmpMine, cyBmpMine, hdcMem, 0, ms*cyBmpMine, SRCCOPY);
			DeleteDC(hdcMem);
		}else{
			return ;
		}

	}else{
		return ;
	}
}

//鼠标右键按下
void RbtDown(HWND hwnd,HDC hdc, LONG lParam)
{
	int ms;             //砖块状态对应位图位置的偏移量
	int	nx, ny, mx, my; // 鼠标位置 和 对应的二维数组下标
	HDC hdcMem;
	nx = LOWORD(lParam)-border;
	ny = HIWORD(lParam)-border;
	if(nx>=0&&nx<=10*cxBmpMine&&ny>=0&&ny<=10*cyBmpMine)
	{
		mx = (nx/cxBmpMine);
		my = (ny/cyBmpMine);
		nx = mx*cxBmpMine + border;
		ny = my*cyBmpMine + border;
						
		if(!mine[mx][my].isOpen || mine[mx][my].RDownStat != 0)
		{				
			hdcMem = CreateCompatibleDC(hdc);  
			SelectObject(hdcMem, hBmpMine); 
			switch(mine[mx][my].RDownStat)
			{ 
				case 0:
					ms = 2;
					BitBlt(hdc, nx, ny, cxBmpMine, cyBmpMine, hdcMem, 0, ms*cyBmpMine, SRCCOPY);
					mine[mx][my].RDownStat = 1;
					--mineFlag;	
					InvalidateRect(hwnd, &countRc, FALSE);
					break; 

				case 1:
					ms = 3;
					BitBlt(hdc, nx, ny, cxBmpMine, cyBmpMine, hdcMem, 0, ms*cyBmpMine, SRCCOPY);
					mine[mx][my].RDownStat = 2;	
					++mineFlag;	
					InvalidateRect(hwnd, &countRc, FALSE);
					break;

				case 2:
					ms = 0;
					BitBlt(hdc, nx, ny, cxBmpMine, cyBmpMine, hdcMem, 0, ms*cyBmpMine, SRCCOPY);
					mine[mx][my].RDownStat = 0;
					break;

				default: 
					return ;
			}		
			DeleteDC(hdcMem);
		}else{
			return ;
		}
	}else{
		return ;
	}

}

//双键按下
void DoubleDown(HWND hwnd,HDC hdc, LONG lParam){
//	int ms;             //砖块状态对应位图位置的偏移量
	int	nx, ny, mx, my; // 鼠标位置 和 对应的二维数组下标
//	HDC hdcMem;
	int flagNum = 0;        //砖块周围小旗子数量
	nx = LOWORD(lParam)-border;
	ny = HIWORD(lParam)-border;

	if(nx>=0&&nx<=10*cxBmpMine&&ny>=0&&ny<=10*cyBmpMine)
	{
		mx = (nx/cxBmpMine);
		my = (ny/cyBmpMine);
		nx = mx*cxBmpMine + border;
		ny = my*cyBmpMine + border;
		if( mine[mx][my].isOpen ){			
			if(mx>0){
				if(mine[mx-1][my].RDownStat == 1){
					flagNum++;
				}
				if( my>0 ){
					if(mine[mx-1][my-1].RDownStat == 1){
						flagNum++;
					}
				}
			}
			if(my>0){
				if(mine[mx][my-1].RDownStat == 1){
					flagNum++;
				}
			}
			if(mx < (mdiv-1)){
				if(mine[mx+1][my].RDownStat == 1){
					flagNum++;
				}
				if(my < (mdiv-1)){
					if(mine[mx+1][my+1].RDownStat == 1){
						flagNum++;
					}
				}
			}
			if(my < (mdiv-1)){
				if(mine[mx][my+1].RDownStat == 1){
					flagNum++;
				}
			}
			if(mx>0 && my<(mdiv-1)){
				if(mine[mx-1][my+1].RDownStat == 1){
					flagNum++;
				}
			}
			if(mx<(mdiv-1) && my>0){
				if(mine[mx+1][my-1].RDownStat == 1){
					flagNum++;
				}
			}
		}
		//旗子数与砖块显示数字一致
		if(mine[mx][my].status == flagNum){
			if(mx>0){
				mine[mx-1][my].isOpen = true;
				if( my>0 ){
					mine[mx-1][my-1].isOpen = true;						
				}
			}
			if(my>0){
				mine[mx][my-1].isOpen = true;
			}
			if(mx < (mdiv-1)){
				mine[mx+1][my].isOpen = true;
				if(my < (mdiv-1)){
					mine[mx+1][my+1].isOpen = true;
				}
			}
			if(my < (mdiv-1)){
				mine[mx][my+1].isOpen = true;
			}
			if(mx>0 && my<(mdiv-1)){
				mine[mx-1][my+1].isOpen = true;
			}
			if(mx<(mdiv-1) && my>0){
				mine[mx+1][my-1].isOpen = true;
			}
		}

	}
}