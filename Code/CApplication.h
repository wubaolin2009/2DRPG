/*--------------------------------------------------------
|           Application.h
|     Application类的相关处理功能的声明
|           (c)  mochsh
----------------------------------------------------------*/

#include <windows.h>

class	CApplication
{
protected:
    //是否是全屏模式
    int     m_bFullScreen;
    //窗口客户区宽度
    int     ScreenW;
    //窗口客户区高度
    int     ScreenH;
    //窗口背景画刷
    int     ClientBKColor;
    //是否可以改变窗口大小的属性变量
    int     Change_Size_Able;
    //是否使用鼠标指针
    int     Show_Mouse_Cursor;
    //窗口的激活状态
    BOOL    bActive;
    //窗口客户区的相对于屏幕左上角的位置
    RECT    rectWin;

    //窗口图标
    HICON   hIcon;
    //窗口光标
    HICON   hCursor;
    //窗口菜单
    HMENU   hMenu;
    //窗口句柄
    HWND    hWnd;

public:
    //构造函数
    CApplication( void );
    CApplication( int bFullScr, int ScrW, int ScrH );

    //设置窗口的属性函数
    void isFullScreen( BOOL bFScr ){m_bFullScreen = bFScr;}
    void SetScreenW( int ScrW )    {ScreenW = ScrW;}
    void SetScreenH( int ScrH )    {ScreenH = ScrH;}
    void ChangeSizeAble(int ChAble){Change_Size_Able = ChAble;}
    void ShowMouseCursor(int M)    {Show_Mouse_Cursor = M;}
    void SetClientBKColor(int C)   {ClientBKColor = C;}
    //设置应用程序的图标
    void Set_hIcon( HINSTANCE hInstance, WORD Icon )
    {
        hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(Icon));
    }
    //设置应用程序的光标
    void Set_hCursor( HINSTANCE hInstance, WORD Cursor )
    {
        hCursor=LoadCursor(hInstance,MAKEINTRESOURCE(Cursor));
    }
    //设置应用程序的菜单
    void Set_hMenu(  HINSTANCE hInstance, WORD Menu )
    {
        hMenu = LoadMenu(hInstance,MAKEINTRESOURCE(Menu));
    }

    //创建窗口
    HWND CreateWin( WNDPROC WinProc,
                    HINSTANCE hInstance, 
                    TCHAR* WindowName,
                    DWORD winStyle);

    //处理主窗口消息
    LRESULT MsgProc(HWND hWnd,
                    UINT uMsg,
                    WPARAM wParam,
                    LPARAM lParam );

    //默认消息循环机制
    int RunDefault(void);

    // 外部调用成员变量
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
