#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include "resource.h"
#include <mmeapi.h>
#include <mmiscapi.h>

#include <vector>

HWND g_hDlg = NULL;
HWND g_hDot = NULL;
std::vector<int> notes;

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

std::vector<unsigned char> LoadFileBytes(char const* pPath)
{
    std::vector<unsigned char> result;

    FILE* pInputFile = nullptr;
    fopen_s(&pInputFile, pPath, "rb");

    // Check size
    fseek(pInputFile, 0, SEEK_END);
    long sizeA = ftell(pInputFile);
    fseek(pInputFile, 0, SEEK_SET);

    result.resize(sizeA);
    fread(result.data(), 1, sizeA, pInputFile);
    fclose(pInputFile);

    return result;
}


DWORD WINAPI SoundThreadProc(LPVOID)
{
    HWAVEOUT    hWaveOut{};
    HGLOBAL     hWaveHdr{};
    LPWAVEHDR   lpWaveHdr{};
    HMMIO       hmmio{};
    UINT        wResult{};
    HANDLE      hFormat{};
    WAVEFORMAT* pFormat{};
    DWORD       dwDataSize{};

    HWND hwndApp = nullptr;


    int samplesPerSecond = 111320;

    WAVEFORMATEX wfx =
    {
        WAVE_FORMAT_PCM,  // wFormatTag
        1,                // nChannels
        (DWORD)samplesPerSecond,             // nSamplesPerSec
        (DWORD)samplesPerSecond,             // nAvgBytesPerSec
        1,                // nBlockAlign
        8,                // wBitsPerSample
        0                 // cbSize
    };

    HANDLE soundDonePlaying = CreateEvent(0, FALSE, FALSE, 0);

    if (waveOutOpen(
        (LPHWAVEOUT)&hWaveOut,
        WAVE_MAPPER,
        (LPWAVEFORMATEX)&wfx,
        (DWORD_PTR)soundDonePlaying,
        0L,
        CALLBACK_EVENT))
    {
        MessageBoxA(hwndApp,
            "Failed to open waveform output device.",
            NULL, MB_OK | MB_ICONEXCLAMATION);
        LocalUnlock(hFormat);
        LocalFree(hFormat);
        mmioClose(hmmio, 0);
        return 0;
    }


    std::vector<char> buffer;
    int noteLength = 66000;
    int amp = 5;
    for (int i = 0; i < notes.size(); ++i)
    {
        int note = notes[i];

        for (int j = 0; j < noteLength; ++j)
        {
            unsigned char b;

            int v = j / note; // wave-peak length
            if (v % 2 == 0)
            {
                b = 128 + amp;
            }
            else
            {
                b = 128 - amp;
            }

            buffer.push_back(b);
        }
    }

    while (true)
    {
        ResetEvent(soundDonePlaying);

        WAVEHDR header = { buffer.data(), (DWORD)buffer.size(), 0, 0, 0, 0, 0, 0 };
        waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
        waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
        waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
        waveOutClose(hWaveOut);

        if (WaitForSingleObject(soundDonePlaying, INFINITE) != WAIT_OBJECT_0)
        {
            return 0; // Error
        }
    }

    return 0;
}

void InitSound(void)
{
    CreateThread(nullptr, 0, SoundThreadProc, nullptr, 0, nullptr);
}

void OnInit()
{
    // Load test file
    std::vector<unsigned char> midiFile = LoadFileBytes("D:\\repos\\mid\\Midis\\lmiasw_samp.mid");

    // Check header
    if (midiFile[0] != 'M' || midiFile[1] != 'T' || midiFile[2] != 'h' || midiFile[3] != 'd')
    {
        return; // Error
    }

    int offs = 0xA6;


    // 0x43 == G

    for (int i = 0; i < 7; ++i)
    {
        char note = midiFile[offs];

        if (note == 0x3E)
        {
            notes.push_back(372);
        }
        else if (note == 0x40)
        {
            notes.push_back(332); // F sharp
        }
        else if (note == 0x42)
        {
            notes.push_back(296); // G natural
        }
        else if (note == 0x43)
        {
            notes.push_back(282); //G
        }
        else if (note == 0x45)
        {
            notes.push_back(252); // A
        }

        offs += 9;
    }

    InitSound();
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

            OnInit();

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
