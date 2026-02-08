#pragma once
#include <Windows.h>
#include <shellapi.h>
#include "resource.h"

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001

class TrayIcon {
public:
    NOTIFYICONDATAW nid = {};
    HWND hwnd;
    bool running = true;

    bool Create(HWND window, HINSTANCE hInstance) {
        hwnd = window;
        nid.cbSize = sizeof(NOTIFYICONDATAW);
        nid.hWnd = hwnd;
        nid.uID = 1;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_TRAYICON;
        nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
        wcscpy_s(nid.szTip, L"Yandex Music Discord RPC");
        
        return Shell_NotifyIconW(NIM_ADD, &nid);
    }

    void Remove() {
        Shell_NotifyIconW(NIM_DELETE, &nid);
    }

    void ShowContextMenu() {
        POINT pt;
        GetCursorPos(&pt);
        
        HMENU hMenu = CreatePopupMenu();
        AppendMenuW(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");
        
        SetForegroundWindow(hwnd);
        TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
        DestroyMenu(hMenu);
    }
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static TrayIcon* tray = nullptr;
    
    switch (msg) {
        case WM_CREATE:
            tray = (TrayIcon*)((CREATESTRUCT*)lParam)->lpCreateParams;
            tray->Create(hwnd, ((CREATESTRUCT*)lParam)->hInstance);
            break;
            
        case WM_TRAYICON:
            if (lParam == WM_RBUTTONUP) {
                tray->ShowContextMenu();
            }
            break;
            
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_TRAY_EXIT) {
                tray->running = false;
                PostQuitMessage(0);
            }
            break;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
