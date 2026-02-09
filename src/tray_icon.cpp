#include "tray_icon.h"

bool TrayIcon::Create(HWND window, HINSTANCE hInstance) {
    this->hwnd = window;
    this->nid.cbSize = sizeof(NOTIFYICONDATAW);
    this->nid.hWnd = this->hwnd;
    this->nid.uID = 1;
    this->nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    this->nid.uCallbackMessage = WM_TRAYICON;
    this->nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
    wcscpy_s(this->nid.szTip, L"Yandex Music Discord RPC");
    
    return Shell_NotifyIconW(NIM_ADD, &this->nid);
}

void TrayIcon::Remove() {
    Shell_NotifyIconW(NIM_DELETE, &this->nid);
}

void TrayIcon::ShowContextMenu() {
    POINT pt;
    GetCursorPos(&pt);
    
    HMENU hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");
    
    SetForegroundWindow(this->hwnd);
    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, this->hwnd, NULL);
    DestroyMenu(hMenu);
}

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