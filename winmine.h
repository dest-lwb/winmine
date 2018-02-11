#include <time.h> 

#define ID_TIMER 1

//����ש��ṹ�嶨��
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
//int				nx, ny, mx, my;  // ���λ�� �� ��Ӧ�Ķ�ά�����±�
//int               ms;              //ש��״̬��Ӧλͼλ�õ�ƫ����
static MINE       mine[10][10];    //������С����
static int        mineNum = 10;    //����
int               mdiv = 9;        //�����߳�
int               mineFlag, mineTime = 0;  //��������   ��ʱ������
RECT              timeRc, countRc; //�������ͼ�ʱ������Ч��

//windows��Ϣ������
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

void InitMineMap();           //��ʼ������
void DrawMineMap(HWND hwnd, HDC hdc, HDC hMem);  //������
void ReDraw(HDC hdc, HDC hMem);       //ˢ��
void DrawTime(HDC hdc, HDC hMem);     //����ʱ��
void DrawCount(HDC hdc, HDC hMem);    //������ ʣ������
void MineFail(HWND hwnd, HDC hdc);     //ɨ��ʧ��
bool MineWin();             //ɨ�׳ɹ�
void LbtDown(HWND hwnd, HDC hdc, LONG lParam);  //����������
void LbtUp(HWND hwnd, HDC hdc, LONG lParam);  //�������ſ�
void RbtDown(HWND hwnd, HDC hdc, LONG lParam);  //����Ҽ�����
void DoubleDown(HWND hwnd, HDC hdc, LONG lParam); //������Ҽ�������

// ��ʼ������
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
		ry = rand() % mdiv; // row mdiv �����߳�

		//�Ѿ����׵�λ�ò����ٴ�����
		if( mine[rx][ry].status != 10 )
		{ 
			mine[rx][ry].status = 10;
			i++;
		}else{ 
			continue; 
		}
		//����Ź������½�����λ�õ�ש��
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
		//����Ź������Ͻ�����λ�õ�ש��
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
		//����Ź������Ͻ�λ�õ�ש��
		if(rx != 0 && ry != mdiv-1){ //
			if(mine[rx - 1][ry + 1].status != 10)
				 ++mine[rx - 1][ry + 1].status;
			// �������½ǵ�ש��Ϊ�׵Ļ���������������ש
			if(rx == mdiv-1 && ry == 0 && mine[rx -1][ry].status != 10)
				++mine[rx - 1][ry].status;
			if(rx == mdiv-1 && ry == 0 && mine[rx][ry + 1].status != 10)
				++mine[rx][ry + 1].status;
		}
		//�Ź������½�λ�õ�ש��			
		if(rx != mdiv-1 && ry != 0){
			if(mine[rx + 1][ry - 1].status != 10)
				 ++mine[rx + 1][ry - 1].status;
			// �������Ͻǵ�ש��Ϊ�׵Ļ���������������ש
			if(rx == 0 && ry == mdiv-1 && mine[rx][ry - 1].status != 10)
				++mine[rx][ry - 1].status;
			if(rx == 0 && ry == mdiv-1 && mine[rx + 1][ry].status != 10)
				++mine[rx + 1][ry].status;
			
		}
	}//while
	
}

// ������
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

//ˢ������
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
//��������
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

// ����ʱ��
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

//ɨ��ʧ��
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
	rs= MessageBox(hwnd,TEXT("        �����ˣ����¿�ʼ��"),TEXT("hello"),MB_OKCANCEL);
	if(rs == IDOK){
		InitMineMap();
		InvalidateRect( hwnd, NULL, TRUE);
	}
	DeleteDC(hdcMem);
}

// ɨ�׳ɹ�
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

//�������ͷ�
void LbtUp(HWND hwnd,HDC hdc, LONG lParam)
{
	int ms;             //ש��״̬��Ӧλͼλ�õ�ƫ����
	int	nx, ny, mx, my; // ���λ�� �� ��Ӧ�Ķ�ά�����±�
//	int rs;

	HDC hdcMem;
	nx = LOWORD(lParam)-border;
	ny = HIWORD(lParam)-border;
	
	//����꽹����������ȷ��Ӧ�÷�����ש��
	if(nx >= 0 && nx < mdiv*cxBmpMine && ny >= 0 && ny < mdiv*cyBmpMine)
	{
		mx = (nx/cxBmpMine);
		my = (ny/cyBmpMine);
		nx = mx*cxBmpMine + border;
		ny = my*cyBmpMine + border;		

		if(!mine[mx][my].isOpen && mine[mx][my].RDownStat == 0)  //ש��λ��mx,my
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
				MineFail(hwnd,hdc); //ɨ��ʧ��				
				/*KillTimer(hwnd,ID_TIMER);
				ms = 4;
				BitBlt(hdc, nx, ny, cxBmpMine, cyBmpMine, hdcMem, 0, ms*cyBmpMine, SRCCOPY);
				
				rs= MessageBox(hwnd,TEXT("        �����ˣ����¿�ʼ��"),TEXT("hello"),MB_OKCANCEL);
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

//����������
void LbtDown(HWND hwnd,HDC hdc, LONG lParam)
{
	int ms;             //ש��״̬��Ӧλͼλ�õ�ƫ����
	int	nx, ny, mx, my; // ���λ�� �� ��Ӧ�Ķ�ά�����±�
	HDC hdcMem;
	nx = LOWORD(lParam)-border;
	ny = HIWORD(lParam)-border;
	if(nx>=0 && nx<mdiv*cxBmpMine && ny>=0 && ny<mdiv*cyBmpMine)
	{
		mx = (nx/cxBmpMine);
		my = (ny/cyBmpMine);
		nx = mx*cxBmpMine + border;
		ny = my*cyBmpMine + border;

		if(!mine[mx][my].isOpen && mine[mx][my].RDownStat == 0)      //ש��λ��nx,ny
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

//����Ҽ�����
void RbtDown(HWND hwnd,HDC hdc, LONG lParam)
{
	int ms;             //ש��״̬��Ӧλͼλ�õ�ƫ����
	int	nx, ny, mx, my; // ���λ�� �� ��Ӧ�Ķ�ά�����±�
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

//˫������
void DoubleDown(HWND hwnd,HDC hdc, LONG lParam){
//	int ms;             //ש��״̬��Ӧλͼλ�õ�ƫ����
	int	nx, ny, mx, my; // ���λ�� �� ��Ӧ�Ķ�ά�����±�
//	HDC hdcMem;
	int flagNum = 0;        //ש����ΧС��������
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
		//��������ש����ʾ����һ��
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