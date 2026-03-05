#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include "resource.h"

HWND g_hDlg = NULL;
HWND g_hDot = NULL;

INT_PTR CALLBACK DialogMessageHandler(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, DialogMessageHandler);
    return TRUE;
}

VOID CALLBACK TimerProc(
    HWND hwnd,        // handle to window for timer messages 
    UINT message,     // WM_TIMER message 
    UINT_PTR idTimer, // timer identifier 
    DWORD dwTime)     // current system time 
{
}

INT_PTR CALLBACK DialogMessageHandler(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
        {
            g_hDlg = hDlg;
            g_hDot = GetDlgItem(g_hDlg, IDC_DOT);
            ShowWindow(g_hDot, SW_HIDE);

            HFONT hFont;

            hFont = CreateFont(
                48,// height
                0, // width
                0, // escapement
                0, // orientation
                FW_NORMAL, // weight
                0, // italic
                0, // underline
                0, // strikeout
                ANSI_CHARSET, // char set
                OUT_DEFAULT_PRECIS, // out precision
                CLIP_DEFAULT_PRECIS, // clip precision
                DEFAULT_QUALITY, // quality
                DEFAULT_PITCH | FF_DONTCARE, // Pitch and family
                L"Arial" // Face name
            );
            SendMessage(g_hDot, WM_SETFONT, (WPARAM)hFont, TRUE);

            HWND hEdit = GetDlgItem(g_hDlg, IDC_EDIT1);
            SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

            return (INT_PTR)TRUE;
        }

        case WM_COMMAND:
        {
            int id = LOWORD(wParam);
            if (id == IDC_START)
            {
            }
            else if (id == IDC_STOP)
            {
            }
            else if (id == IDC_RESET)
            {
            }
            break;
        }
        case WM_CLOSE:
        {
            EndDialog(hDlg, LOWORD(wParam));
            g_hDlg = NULL;
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}
