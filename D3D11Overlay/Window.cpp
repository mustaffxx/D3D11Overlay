#include "window.hpp"
#include <stdexcept>

Window::Window(HINSTANCE hInstance, const std::wstring& title, int width, int height) {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszClassName = WINDOW_CLASS_NAME;

    if (!RegisterClassEx(&wc)) {
        throw std::runtime_error("Failed to register window class");
    }

    hWindow = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST,
                            WINDOW_CLASS_NAME,
                            title.c_str(),
                            WS_POPUP,
                            CW_USEDEFAULT, 
                            CW_USEDEFAULT,
                            width, 
                            height,
                            nullptr, 
                            nullptr,
                            hInstance, 
                            nullptr);

    if (!hWindow) {
        throw std::runtime_error("Failed to create window");
    }

    SetLayeredWindowAttributes(hWindow, 0, 255, LWA_ALPHA);
    ShowWindow(hWindow, SW_SHOW);
}

Window::~Window() {
	if (hWindow) DestroyWindow(hWindow);
	UnregisterClass(WINDOW_CLASS_NAME, GetModuleHandle(nullptr));
}

bool Window::ProcessMessages() {
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return false;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return true;
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                PostQuitMessage(0);
                return 0;
            }
            break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}