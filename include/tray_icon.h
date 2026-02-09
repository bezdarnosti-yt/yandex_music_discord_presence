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

    bool Create(HWND window, HINSTANCE hInstance);
    void Remove();
    void ShowContextMenu();
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);