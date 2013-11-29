/*--------------------------------------------------------
|           Application.h
|     Application�����ش����ܵ�����
|           (c)  mochsh
----------------------------------------------------------*/

#include <windows.h>

class	CApplication
{
protected:
    //�Ƿ���ȫ��ģʽ
    int     m_bFullScreen;
    //���ڿͻ������
    int     ScreenW;
    //���ڿͻ����߶�
    int     ScreenH;
    //���ڱ�����ˢ
    int     ClientBKColor;
    //�Ƿ���Ըı䴰�ڴ�С�����Ա���
    int     Change_Size_Able;
    //�Ƿ�ʹ�����ָ��
    int     Show_Mouse_Cursor;
    //���ڵļ���״̬
    BOOL    bActive;
    //���ڿͻ������������Ļ���Ͻǵ�λ��
    RECT    rectWin;

    //����ͼ��
    HICON   hIcon;
    //���ڹ��
    HICON   hCursor;
    //���ڲ˵�
    HMENU   hMenu;
    //���ھ��
    HWND    hWnd;

public:
    //���캯��
    CApplication( void );
    CApplication( int bFullScr, int ScrW, int ScrH );

    //���ô��ڵ����Ժ���
    void isFullScreen( BOOL bFScr ){m_bFullScreen = bFScr;}
    void SetScreenW( int ScrW )    {ScreenW = ScrW;}
    void SetScreenH( int ScrH )    {ScreenH = ScrH;}
    void ChangeSizeAble(int ChAble){Change_Size_Able = ChAble;}
    void ShowMouseCursor(int M)    {Show_Mouse_Cursor = M;}
    void SetClientBKColor(int C)   {ClientBKColor = C;}
    //����Ӧ�ó����ͼ��
    void Set_hIcon( HINSTANCE hInstance, WORD Icon )
    {
        hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(Icon));
    }
    //����Ӧ�ó���Ĺ��
    void Set_hCursor( HINSTANCE hInstance, WORD Cursor )
    {
        hCursor=LoadCursor(hInstance,MAKEINTRESOURCE(Cursor));
    }
    //����Ӧ�ó���Ĳ˵�
    void Set_hMenu(  HINSTANCE hInstance, WORD Menu )
    {
        hMenu = LoadMenu(hInstance,MAKEINTRESOURCE(Menu));
    }

    //��������
    HWND CreateWin( WNDPROC WinProc,
                    HINSTANCE hInstance, 
                    TCHAR* WindowName,
                    DWORD winStyle);

    //������������Ϣ
    LRESULT MsgProc(HWND hWnd,
                    UINT uMsg,
                    WPARAM wParam,
                    LPARAM lParam );

    //Ĭ����Ϣѭ������
    int RunDefault(void);

    // �ⲿ���ó�Ա����
    int  Get_m_bFullScreen(void)    {return m_bFullScreen;}
    int  Get_ScreenW(void)          {return ScreenW;}
    int  Get_ScreenH(void)          {return ScreenH;}
    int  Get_ClientBKColor(void)    {return ClientBKColor;}
    int  Get_Change_Size_Able(void) {return Change_Size_Able;}
    int  Get_Show_Mouse_Cursor(void){return Show_Mouse_Cursor;}
    BOOL Get_bActive(void)          {return bActive;}
    RECT Get_rectWin(void)          {return rectWin;}
    HICON Get_hIcon(void)           {return hIcon;}
    HICON Get_hCursor(void)         {return hCursor;}
    HMENU Get_hMenu(void)           {return hMenu;}
    HWND Get_hWnd(void)             {return hWnd;}
};
