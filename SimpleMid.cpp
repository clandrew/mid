#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include "resource.h"

HWND g_hDlg = NULL;

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
